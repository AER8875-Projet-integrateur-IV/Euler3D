#ifndef NOWTECH_LOG_APP_INTERFACE_STD
#define NOWTECH_LOG_APP_INTERFACE_STD

#include "Log.h"
#include <ios>
#include <mutex>
#include <chrono>
#include <thread>
#include <unordered_set>
#include <condition_variable>

namespace nowtech::log {

template<TaskId tMaxTaskCount, bool tLogFromIsr, size_t tTaskShutdownPollPeriod>
class AppInterfaceStd final {
public:
  using LogTime = uint32_t;
  static constexpr TaskId csMaxTaskCount      = tMaxTaskCount; // Exported just to let the Log de checks.
  static constexpr TaskId csInvalidTaskId     = std::numeric_limits<TaskId>::max();
  static constexpr TaskId csIsrTaskId         = std::numeric_limits<TaskId>::min();
  static constexpr TaskId csFirstNormalTaskId = csIsrTaskId + 1u;
  static constexpr bool   csConstantTaskNames = false;

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
      throw std::bad_alloc();
    }
  };

private:
  class Semaphore final {
  private:
    std::atomic<bool>              mNotified = false;
    std::mutex                     mMutex;
    std::unique_lock<std::mutex>   mLock;
    std::condition_variable        mConditionVariable;

  public:
    Semaphore() noexcept : mLock(mMutex) {
    }

    void wait() noexcept {
      mConditionVariable.wait(mLock, [this] { return mNotified == true; });
    }

    void notify() noexcept {
      mNotified = true;
      mConditionVariable.notify_one();
    }
  };

  inline static Semaphore sSemaphore;

  inline static constexpr char csErrorMessages[static_cast<size_t>(Exception::cCount)][40] = {
    "cOutOfTaskIdsOrDoubleRegistration", "cOutOfTopics", "cSenderError"
  };
  inline static constexpr char   csError[]            = "Error: ";
  inline static constexpr char   csFatalError[]       = "Fatal: ";
  inline static constexpr char   csUnknownTaskName[]  = "UNKNOWN";
  inline static constexpr char   csIsrTaskName[]      = "ISR";
  inline static constexpr size_t csTaskId2threadsSize = csMaxTaskCount + 1u;
  inline static const std::thread::id csNoThreadId;

  inline static thread_local TaskId shTaskId = csInvalidTaskId;
  inline static thread_local std::string shTaskName;
  inline static std::unordered_set<TaskId> sFreeTaskIds;
  inline static std::mutex sRegistrationMutex;
  inline static std::thread *sTransmitterThread;
  
  AppInterfaceStd() = delete;

public:
  static void init() {
    for(TaskId id = csFirstNormalTaskId; id <= tMaxTaskCount; ++id) {
      sFreeTaskIds.insert(id);
    }
  }

  static void init(void(*aFunction)(void)) {
    init();
    sTransmitterThread = new std::thread(aFunction);
  }

  static void done() {
    if(sTransmitterThread != nullptr) {
      sTransmitterThread->join();
      delete sTransmitterThread;
    }
    else { // nothing to do
    }
    sFreeTaskIds.clear();
  }

  /// We assume it won't get called during registering.
  static TaskId getCurrentTaskId() noexcept {
    return shTaskId;
  }

  static TaskId registerCurrentTask(char const * const aTaskName) {
    std::lock_guard<std::mutex> lock (sRegistrationMutex);
    TaskId result;
    if(!sFreeTaskIds.empty()) {
      auto begin = sFreeTaskIds.begin();
      shTaskId = result = *begin;
      shTaskName = aTaskName;
      sFreeTaskIds.erase(begin);
    }
    else {
      result = csInvalidTaskId;
    }
    return result;
  }

  static TaskId unregisterCurrentTask() {
    std::lock_guard<std::mutex> lock (sRegistrationMutex);
    sFreeTaskIds.insert(shTaskId);
    shTaskId = csInvalidTaskId;
    return csInvalidTaskId;
  }

  // Caller will copy contents from returned pointer immediately.
  static char const * getTaskName(TaskId const) noexcept {
    return shTaskName.c_str();
  }

  static LogTime getLogTime() noexcept {
    return static_cast<uint32_t>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count());
  }

  static void finish() noexcept {
    sSemaphore.notify();
  }

  static void waitForFinished() noexcept {
    sSemaphore.wait();
  }

  static void sleepWhileWaitingForTaskShutdown() noexcept {
    std::this_thread::sleep_for(std::chrono::milliseconds(tTaskShutdownPollPeriod));
  }

  static void lock() noexcept { // Now don't care.
  }

  static void unlock() noexcept { // Now don't care.
  }

  static void error(Exception const aError) {
    throw std::ios_base::failure(csErrorMessages[static_cast<size_t>(aError)]);
  }

  static void fatalError(Exception const aError) {
    throw std::ios_base::failure(csErrorMessages[static_cast<size_t>(aError)]);
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
};

}

#endif
