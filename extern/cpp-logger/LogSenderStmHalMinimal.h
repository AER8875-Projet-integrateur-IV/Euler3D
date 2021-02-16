//
// Created by balazs on 2020. 12. 04..
//

#ifndef NOWTECH_LOG_SENDER_STM_HAL_MINIMAL
#define NOWTECH_LOG_SENDER_STM_HAL_MINIMAL

#include "Log.h"
#include "stm32f1xx_hal.h"

namespace nowtech::log {

// For slow transmission medium like UART on embedded this class could implement double buffering.
template<typename tAppInterface, typename tConverter, size_t tTransmitBufferSize, typename tAppInterface::LogTime tTimeout>
class SenderStmHalMinimal final {
public:
  using tAppInterface_   = tAppInterface;
  using tConverter_      = tConverter;
  using ConversionResult = typename tConverter::ConversionResult;
  using Iterator         = typename tConverter::Iterator;

  static constexpr bool csVoid = false;

private:
  inline static UART_HandleTypeDef *sSerialDescriptor = nullptr;
  inline static ConversionResult   *sTransmitBuffer;
  inline static Iterator            sBegin;
  inline static Iterator            sEnd;

  SenderStmHalMinimal() = delete;

public:
  static void init(UART_HandleTypeDef * const aUart) {
    sSerialDescriptor = aUart;
    sTransmitBuffer = tAppInterface::template _newArray<ConversionResult>(tTransmitBufferSize);
    sBegin = sTransmitBuffer;
    sEnd = sTransmitBuffer + tTransmitBufferSize;
  }

  static void done() noexcept {
  }

  static void send(char const * const aBegin, char const * const aEnd) {
    if(sSerialDescriptor != nullptr) {
      HAL_UART_Transmit(sSerialDescriptor, reinterpret_cast<uint8_t*>(const_cast<char*>(aBegin)), aEnd - aBegin, tTimeout);
    }
    else { // nothing to do
    }
  }

  static auto getBuffer() {
    return std::pair(sBegin, sEnd);
  }
};
  
}

#endif
