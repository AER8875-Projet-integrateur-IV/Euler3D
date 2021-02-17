#ifndef NOWTECH_LOG_NUMERIC_SYSTEM
#define NOWTECH_LOG_NUMERIC_SYSTEM

#include <cstdint>

namespace nowtech::log {

  struct NumericSystem final {
    NumericSystem() = delete;

    static constexpr uint8_t csInvalid =  1u;
    static constexpr uint8_t csBaseMax = 16u;
  };
  
}

#endif