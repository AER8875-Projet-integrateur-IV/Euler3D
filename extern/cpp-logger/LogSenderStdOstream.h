#ifndef NOWTECH_LOG_SENDER_STD_OSTREAM
#define NOWTECH_LOG_SENDER_STD_OSTREAM

#include <ostream>
#include "Log.h"

namespace nowtech::log {

// For slow transmission medium like UART on embedded this class could implement double buffering.
template<typename tAppInterface, typename tConverter, size_t tTransmitBufferSize, typename tAppInterface::LogTime tTimeout>
class SenderStdOstream final {
public:
  using tAppInterface_   = tAppInterface;
  using tConverter_      = tConverter;
  using ConversionResult = typename tConverter::ConversionResult;
  using Iterator         = typename tConverter::Iterator;

  static constexpr bool csVoid = false;

private:
  inline static std::ostream     *sStream = nullptr;
  inline static ConversionResult *sTransmitBuffer;
  inline static Iterator          sBegin;
  inline static Iterator          sEnd;

  SenderStdOstream() = delete;

public:
  static void init(std::ostream * const aStream) {
    sStream = aStream;
    sTransmitBuffer = tAppInterface::template _newArray<ConversionResult>(tTransmitBufferSize);
    sBegin = sTransmitBuffer;
    sEnd = sTransmitBuffer + tTransmitBufferSize;
  }

  static void done() noexcept {
    tAppInterface::template _deleteArray<ConversionResult>(sTransmitBuffer);
  }

  static void send(char const * const aBegin, char const * const aEnd) {
    try {
      if(sStream != nullptr) {
        sStream->write(aBegin, aEnd - aBegin);
      }
      else { // nothing to do
      }
    } catch(...) {
      tAppInterface::error(Exception::cSenderError);
    }
  }

  static auto getBuffer() {
    return std::pair(sBegin, sEnd);
  }
};
  
}

#endif