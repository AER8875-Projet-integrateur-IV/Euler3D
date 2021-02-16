#ifndef LOG_QUEUE_STD_BOOST
#define LOG_QUEUE_STD_BOOST

#include <cstddef>
#include <mutex>
#include <condition_variable>
#include <boost/lockfree/queue.hpp>

namespace nowtech::log {

template<typename tMessage, typename tAppInterface, size_t tQueueSize>
class QueueStdBoost final {
public:
  using tMessage_ = tMessage;
  using tAppInterface_ = tAppInterface;
  using LogTime = typename tAppInterface::LogTime;

  static constexpr size_t csQueueSize = tQueueSize;

private:
  class FreeRtosQueue final {
    boost::lockfree::queue<tMessage> mQueue;
    std::atomic<bool>              mNotified;
    std::mutex                     mMutex;
    std::unique_lock<std::mutex>   mLock;
    std::condition_variable        mConditionVariable;

  public:
    /// First implementation, we assume we have plenty of memory.
    FreeRtosQueue() noexcept
      : mQueue(tQueueSize)
      , mLock(mMutex) {
      mNotified = false;
    }

    ~FreeRtosQueue() noexcept = default;

    bool empty() const noexcept {
      return mQueue.empty();
    }

    void push(tMessage const &aMessage) noexcept {
      bool success = mQueue.bounded_push(aMessage);
      if(success) {
        mNotified = true;
        mConditionVariable.notify_one();
      }
      else { // nothing to do
      }
    }

    bool pop(tMessage &aMessage, LogTime const mPauseLength) noexcept {
      bool result;
      // Safe to call empty because there will be only one consumer.
      if(mQueue.empty() && !mConditionVariable.wait_for(mLock, std::chrono::milliseconds(mPauseLength), [this]{return mNotified == true;})) {
        result = false;
      }
      else {
        mNotified = false;
        result = mQueue.pop(aMessage);
      }
      return result;
    }
  };

  inline static FreeRtosQueue sQueue;

  QueueStdBoost() = delete;

public:
  static void init() { // nothing to do
  }

  static void done() {  // nothing to do
  }

  static bool empty() noexcept {
    return sQueue.empty();
  }

  static void push(tMessage const &aMessage) noexcept {
    sQueue.push(aMessage);
  }

  static bool pop(tMessage &aMessage, LogTime const aPauseLength) noexcept {
    return sQueue.pop(aMessage, aPauseLength);
  }
};

}

#endif