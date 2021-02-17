#ifndef LOG_QUEUE_VOID
#define LOG_QUEUE_VOID

#include <cstddef>

namespace nowtech::log {

template<typename tMessage, typename tAppInterface, size_t tQueueSize>
class QueueVoid final {
public:
  using tMessage_ = tMessage;
  using tAppInterface_ = tAppInterface;
  using LogTime = typename tAppInterface::LogTime;

private:
  QueueVoid() = delete;

public:
  static void init() { // nothing to do
  }

  static void done() {  // nothing to do
  }

  static bool empty() noexcept {
    return true;
  }

  static void push(tMessage const) noexcept { // nothing to do
  }

  static bool pop(tMessage &, LogTime const) noexcept { // nothing to do
    return false;
  }
};

}

#endif
