#ifndef LOG_SENDER_VOID
#define LOG_SENDER_VOID

#include "Log.h"

namespace nowtech::log {

// For slow transmission medium like UART on embedded this class could implement double buffering.
template<typename tAppInterface, typename tConverter, size_t tTransmitBufferSize, typename tAppInterface::LogTime tTimeout>
class SenderVoid final {
public:
  using tAppInterface_   = tAppInterface;
  using tConverter_      = tConverter;
  using ConversionResult = typename tConverter::ConversionResult;
  using Iterator         = typename tConverter::Iterator;

  static constexpr bool csVoid = true;

private:
  SenderVoid() = delete;

public:
  static void init() noexcept { // nothing to do
  }

  static void done() noexcept { // nothing to do
  }

  static void send(char const * const, char const * const) { // nothing to do
  }

  static auto getBuffer() {
    return std::pair(tConverter::csIteratorVoid, tConverter::csIteratorVoid);
  }
};

}

#endif