#ifndef NOWTECH_LOG_MESSAGE_BASE
#define NOWTECH_LOG_MESSAGE_BASE

#include "LogNumericSystem.h"
#include <limits>

namespace nowtech::log {

struct LogFormat final {
public:
  static constexpr uint8_t csFillValueStoreString = std::numeric_limits<uint8_t>::max();
  static constexpr uint8_t csFillValueStoreStringTerminal = csFillValueStoreString - 1u;

  uint8_t mBase;
  uint8_t mFill;

  void invalidate() noexcept {
    mBase = NumericSystem::csInvalid;
  }

  bool isValid() const noexcept {
    return mBase > NumericSystem::csInvalid && mBase <= NumericSystem::csBaseMax;
  }

  bool isStoredString() const noexcept {
    return mFill >= csFillValueStoreString;
  }
};

using TaskId          = uint8_t;
using MessageSequence = uint8_t;

enum class ShutdownMessageContent : uint8_t {
  csSomething
};

template<std::size_t tPayloadSize, bool tSupportFloatingPoint>
class MessageBase {
public:
  static constexpr MessageSequence csTerminal = 0u;

protected:
  static_assert(!tSupportFloatingPoint || sizeof(float) <= tPayloadSize);
  static_assert(sizeof(int32_t) <= tPayloadSize);
  static_assert(!tSupportFloatingPoint || sizeof(double) <= sizeof(int64_t));
  static_assert(sizeof(char*) <= tPayloadSize);
  static_assert(sizeof(LogFormat) == sizeof(int16_t));
};

} // namespace

#endif
