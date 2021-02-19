//
// Created by balazs on 2020. 12. 04..
//

#ifndef NOWTECH_LOG_QUEUE_FREERTOS
#define NOWTECH_LOG_QUEUE_FREERTOS

#include <cstddef>
#include "FreeRTOS.h"
#include "queue.h"
#include "stm32f1xx_hal.h"

namespace nowtech::log {

template<typename tMessage, typename tAppInterface, size_t tQueueSize>
class QueueFreeRtos final {
public:
  using tMessage_ = tMessage;
  using tAppInterface_ = tAppInterface;
  using LogTime = typename tAppInterface::LogTime;

  static constexpr size_t csQueueSize = tQueueSize;

private:
  inline static QueueHandle_t sQueue;

  QueueFreeRtos() = delete;

public:
  static void init() { // nothing to do
    sQueue = xQueueCreate(tQueueSize, sizeof(tMessage));
  }

  static void done() {  // nothing to do
  }

  static bool empty() noexcept {
    return xQueueIsQueueEmptyFromISR(sQueue) == pdTRUE;
  }

  static void push(tMessage const &aMessage) noexcept {
    xQueueSendFromISR(sQueue, &aMessage, nullptr); // Don't care about the result, pop logic will manage missing messages.
  }

  static bool pop(tMessage &aMessage, LogTime const aPauseLength) noexcept {
    return xQueueReceive(sQueue, &aMessage, aPauseLength) == pdTRUE;
  }
};

}

#endif
