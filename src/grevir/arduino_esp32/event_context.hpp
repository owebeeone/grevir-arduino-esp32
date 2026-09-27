#pragma once

#if defined(CONFIG_IDF_TARGET_ESP32)
#include <atomic>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <grevir/base/compat/string_view.hpp>

namespace grevir::arduino_esp32 {

// One lock protects the queue on both cores. The entry/exit operations must
// match the caller's task or ISR context; callbacks run after the guard exits.
class EventLock {
 public:
  inline static constexpr std::string_view identity{"esp32_portmux_loop_task_v1"};

  class TaskGuard {
   public:
    TaskGuard() noexcept { taskENTER_CRITICAL(&mux_); }
    ~TaskGuard() noexcept { taskEXIT_CRITICAL(&mux_); }
    TaskGuard(const TaskGuard&) = delete;
    TaskGuard& operator=(const TaskGuard&) = delete;
  };

  class IsrGuard {
   public:
    IsrGuard() noexcept { taskENTER_CRITICAL_ISR(&mux_); }
    ~IsrGuard() noexcept { taskEXIT_CRITICAL_ISR(&mux_); }
    IsrGuard(const IsrGuard&) = delete;
    IsrGuard& operator=(const IsrGuard&) = delete;
  };

 private:
  inline static portMUX_TYPE mux_ = portMUX_INITIALIZER_UNLOCKED;
};

// Application::start() is called from Arduino setup(), which runs in loopTask.
// A second FreeRTOS task may publish events but cannot execute MainLoop handlers.
class MainLoopContext {
 public:
  static void bind() noexcept {
    owner_.store(xTaskGetCurrentTaskHandle(), std::memory_order_release);
  }

  static bool is_current() noexcept {
    return owner_.load(std::memory_order_acquire) == xTaskGetCurrentTaskHandle();
  }

 private:
  inline static std::atomic<TaskHandle_t> owner_{nullptr};
};

} // namespace grevir::arduino_esp32
#endif
