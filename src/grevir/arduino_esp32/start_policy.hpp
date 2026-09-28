#pragma once

#include <atomic>
#include <grevir/interrupt/start.hpp>

namespace grevir::arduino_esp32 {

// Startup may be called from multiple FreeRTOS tasks. A caller that observes
// startup in progress must not wait: the initiating task may depend on it.
template <class Spec>
class TimerStartPolicy {
 public:
  template <class Body>
  static interrupt::StartResult execute(Body body) noexcept {
    unsigned char expected = 0;
    if (state_.compare_exchange_strong(expected, 1,
          std::memory_order_acq_rel, std::memory_order_acquire)) {
      result_ = body();
      state_.store(2, std::memory_order_release);
      return result_;
    }
    if (expected == 1) {
      return {interrupt::SetupOutcome::in_progress,
        interrupt::SetupOutcome::success,
        interrupt::CallDisposition::in_progress};
    }
    // The acquire failure observed state 2 and therefore the published result.
    auto result = result_;
    result.disposition = interrupt::CallDisposition::replayed;
    return result;
  }

 private:
  inline static std::atomic<unsigned char> state_{0};
  inline static interrupt::StartResult result_{};
};

} // namespace grevir::arduino_esp32
