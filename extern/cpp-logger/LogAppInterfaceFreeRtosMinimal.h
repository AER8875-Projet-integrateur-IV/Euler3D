//
// Created by balazs on 2020. 12. 04..
//

#ifndef NOWTECH_LOG_APP_INTERFACE_FREERTOS_MINIMAL
#define NOWTECH_LOG_APP_INTERFACE_FREERTOS_MINIMAL

#include "Log.h"
#include <array>
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "stm32f1xx_hal.h"

extern "C" void logTransmitterTask(void* aFunction);

namespace nowtech::log {

template<TaskId tMaxTaskCount, bool tLogFromIsr, size_t tTaskShutdownPollPeriod>
class AppInterfaceFreeRtosMinimal final {
public:
  using LogTime = uint32_t;
  static constexpr TaskId csMaxTaskCount      = tMaxTaskCount; // Exported just to let the Log de checks.
  static constexpr TaskId csInvalidTaskId     = std::numeric_limits<TaskId>::max();
  static constexpr TaskId csIsrTaskId         = std::numeric_limits<TaskId>::min();
  static constexpr TaskId csFirstNormalTaskId = csIsrTaskId + 1u;
  static constexpr bool   csConstantTaskNames = true;

  class Occupier final {
  public:
    Occupier() = default;

    void* occupy(size_t const aSize) noexcept {
      return new std::byte[aSize];
    }

    void release(void* const aPointer) noexcept {
      delete[](static_cast<std::byte*>(aPointer));
    }
    
    void badAlloc() {
      while(true) {
      }
    }
  };

private:
  inline static constexpr char csUnknownTaskName[]     = "UNKNOWN";
  inline static constexpr char csIsrTaskName[]         = "ISR";
  inline static constexpr char csTransmitterTaskName[] = "logTx";
  inline static constexpr TickType_t csRegistrationMutexTimeout = portMAX_DELAY;

  inline static std::array<TaskHandle_t, csMaxTaskCount> sTaskHandles;
  inline static TaskId sPreviousTaskId = csFirstNormalTaskId - 1u;
  inline static SemaphoreHandle_t sRegistrationMutex;
  inline static TaskHandle_t sTransmitterTask;
  
  AppInterfaceFreeRtosMinimal() = delete;

public:
  static void init() {
    sRegistrationMutex = xSemaphoreCreateMutex();
  }

  static void init(void (*aFunction)(), uint32_t const aStackDepth, uint32_t const aPriority) {
    init();
    xTaskCreate(logTransmitterTask, csTransmitterTaskName, aStackDepth, reinterpret_cast<void*>(aFunction), aPriority, &sTransmitterTask);
  }

  static void done() { // We assume it runs forever.
  }

  /// We assume it won't get called during registering.
  static TaskId getCurrentTaskId() noexcept {
    TaskId result;
    if(isInterrupt()) {
      if constexpr (tLogFromIsr) {
        result = csInvalidTaskId;
      }
      else {
        result = csIsrTaskId;
      }
    }
    else {
      TaskHandle_t taskHandle = xTaskGetCurrentTaskHandle();
      auto end = sTaskHandles.begin() + sPreviousTaskId;
      auto found = std::find(sTaskHandles.begin(), end, taskHandle);
      if(found != end) {
        result = found - sTaskHandles.begin() + csFirstNormalTaskId - csIsrTaskId;
      }
      else {
        result = csInvalidTaskId;
      }
    }
    return result;
  }

  static TaskId registerCurrentTask(char const * const) {
    xSemaphoreTake(sRegistrationMutex, csRegistrationMutexTimeout);
    TaskId result;
    if(sPreviousTaskId < csMaxTaskCount){
      TaskHandle_t taskHandle = xTaskGetCurrentTaskHandle();
      auto end = sTaskHandles.begin() + sPreviousTaskId;
      auto found = std::find(sTaskHandles.begin(), end, taskHandle);
      if(found != end) {
        result = found - sTaskHandles.begin() + csFirstNormalTaskId - csIsrTaskId;
      }
      else {
        result = ++sPreviousTaskId;
        *end = taskHandle;
      }
    }
    else {
      result = csInvalidTaskId;
    }
    xSemaphoreGive(sRegistrationMutex);
    return result;
  }

  static TaskId unregisterCurrentTask() { // We assume everything runs forever.
    return csInvalidTaskId;
  }

  // Application can trick supplied task IDs to make this function return a dangling pointer if concurrent task unregistration
  // occurs. Normal usage should be however safe.
  static char const * getTaskName(TaskId const aTaskId) noexcept {
    char const * result;
    if(isInterrupt()) {
      result = csIsrTaskName;
    }
    else {
      if(aTaskId <= sPreviousTaskId) {
        result = pcTaskGetName(sTaskHandles[aTaskId - csFirstNormalTaskId + csIsrTaskId]);
      }
      else {
        result = csUnknownTaskName;
      }
    }
    return result;
  }

  static LogTime getLogTime() noexcept {
    return portTICK_PERIOD_MS * xTaskGetTickCount();
  }

  static void finish() noexcept { // We assume everything runs forever.
  }

  static void waitForFinished() noexcept { // We assume everything runs forever.
  }

  static void sleepWhileWaitingForTaskShutdown() noexcept {
    vTaskDelay(tTaskShutdownPollPeriod / portTICK_PERIOD_MS);
  }


  static void lock() noexcept { // Now don't care.
  }

  static void unlock() noexcept { // Now don't care.
  }

  static void error(Exception const aError) {
	  while(true) {
	  }
  }

  static void fatalError(Exception const) {
	  while(true) {
	  }
  }

  template<typename tClass, typename ...tParameters>
  static tClass* _new(tParameters... aParameters) {
    return ::new tClass(aParameters...);
  }

  template<typename tClass>
  static tClass* _newArray(uint32_t const aCount) {
    return ::new tClass[aCount];
  }

  template<typename tClass>
  static void _delete(tClass* aPointer) {
    ::delete aPointer;
  }

  template<typename tClass>
  static void _deleteArray(tClass* aPointer) {
    ::delete[] aPointer;
  }

private:
  static bool isInterrupt() noexcept {   // Well, this is STM32-specific, but can easily be changed to other MCUs as well.
    return (SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk) != 0;
  }

};

}

inline void *operator new(size_t size) {
  return pvPortMalloc(size);
}

inline void operator delete(void *aPointer) noexcept {
  vPortFree(aPointer);
}

inline void operator delete(void *aPointer, size_t) noexcept {
  vPortFree(aPointer);
}

inline void *operator new[](size_t size) {
  return pvPortMalloc(size);
}

inline void operator delete[](void *aPointer) noexcept {
  vPortFree(aPointer);
}

inline void operator delete[](void *aPointer, size_t) noexcept {
  vPortFree(aPointer);
}


#endif
