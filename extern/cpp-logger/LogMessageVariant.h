
#ifndef NOWTECH_LOG_MESSAGE_VARIANT
#define NOWTECH_LOG_MESSAGE_VARIANT

#include "LogMessageBase.h"
#include <array>
#include <variant>
#include <cstddef>
#include <type_traits>


namespace nowtech::log {

// Will be copied using operator=
template<size_t tPayloadSize, bool tSupportFloatingPoint>
class MessageVariant final : public MessageBase<tPayloadSize, tSupportFloatingPoint> {
public:
  static constexpr size_t csPayloadSize = tPayloadSize;
  static constexpr bool   csSupportFloatingPoint = tSupportFloatingPoint;

private:
  static constexpr MessageSequence csTerminal     = MessageBase<tPayloadSize, tSupportFloatingPoint>::csTerminal;

  using PayloadFloat32 = std::variant<ShutdownMessageContent, bool, float, uint8_t, uint16_t, uint32_t, int8_t, int16_t, int32_t, char, char const*, std::array<char, csPayloadSize>>;
  using PayloadFloat64 = std::variant<ShutdownMessageContent, bool, float, double, uint8_t, uint16_t, uint32_t, uint64_t, int8_t, int16_t, int32_t, int64_t, char, char const*, std::array<char, csPayloadSize>>;
  using PayloadFloat80 = std::variant<ShutdownMessageContent, bool, float, double, long double, uint8_t, uint16_t, uint32_t, uint64_t, int8_t, int16_t, int32_t, int64_t, char, char const*, std::array<char, csPayloadSize>>;
  using PayloadFloat = std::conditional_t<tPayloadSize < sizeof(int64_t) && sizeof(char*) == sizeof(int32_t), PayloadFloat32,
                  std::conditional_t<tPayloadSize < sizeof(long double), PayloadFloat64, PayloadFloat80>>;
  using PayloadNoFloat32 = std::variant<ShutdownMessageContent, bool, uint8_t, uint16_t, uint32_t, int8_t, int16_t, int32_t, char, char const*, std::array<char, csPayloadSize>>;
  using PayloadNoFloat64 = std::variant<ShutdownMessageContent, bool, uint8_t, uint16_t, uint32_t, uint64_t, int8_t, int16_t, int32_t, int64_t, char, char const*, std::array<char, csPayloadSize>>;
  using PayloadNoFloat80 = std::variant<ShutdownMessageContent, bool, uint8_t, uint16_t, uint32_t, uint64_t, int8_t, int16_t, int32_t, int64_t, char, char const*, std::array<char, csPayloadSize>>;
  using PayloadNoFloat = std::conditional_t<tPayloadSize < sizeof(int64_t) && sizeof(char*) == sizeof(int32_t), PayloadNoFloat32,
                  std::conditional_t<tPayloadSize < sizeof(long double), PayloadNoFloat64, PayloadNoFloat80>>;
  using Payload = std::conditional_t<tSupportFloatingPoint, PayloadFloat, PayloadNoFloat>;
  static_assert(std::is_trivially_copyable_v<Payload>);
  // TODO concept on payload size and long double -- see it when 8 in main

  Payload         mPayload;
  LogFormat       mFormat;
  TaskId          mTaskId;
  MessageSequence mMessageSequence;

public:
  MessageVariant() = default;
  MessageVariant(MessageVariant const &) = default;
  MessageVariant(MessageVariant &&) = default;
  MessageVariant& operator=(MessageVariant const &) = default;
  MessageVariant& operator=(MessageVariant &&) = default;

  void setShutdown(TaskId const aTaskId) noexcept {
    mPayload = ShutdownMessageContent::csSomething;
    mTaskId = aTaskId;
  }

  template<typename tArgument>
  void set(tArgument const aValue, LogFormat const aFormat, TaskId const aTaskId, MessageSequence const aMessageSequence) noexcept {
    mPayload = aValue;
    mFormat = aFormat;
    mTaskId = aTaskId;
    mMessageSequence = aMessageSequence;
  }

  template<typename tConverter>
  void output(tConverter& aConverter) const noexcept {
    auto visitor = [this, &aConverter](const auto aObj) { aConverter.convert(aObj, mFormat.mBase, mFormat.mFill); };
    std::visit(visitor, mPayload);
  }

  bool isShutdown() const noexcept {
    return std::holds_alternative<ShutdownMessageContent>(mPayload);
  }

  bool isTerminal() const noexcept {
    return mMessageSequence == csTerminal;
  }

  uint8_t getBase() const noexcept {
    return mFormat.mBase;
  }  

  uint8_t getFill() const noexcept {
    return mFormat.mFill;
  }  

  TaskId getTaskId() const noexcept {
    return mTaskId;
  }  

  MessageSequence getMessageSequence() const noexcept {
    return mMessageSequence;
  }  
};

}

#endif
