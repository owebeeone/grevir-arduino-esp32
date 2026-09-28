#pragma once

#if defined(CONFIG_IDF_TARGET_ESP32)
#include <cstdint>
#include <esp_err.h>
#include <esp_intr_alloc.h>
#include <esp_private/periph_ctrl.h>
#include <grevir/arduino_esp32/start_policy.hpp>
#include <soc/interrupts.h>
#include <soc/periph_defs.h>
#include <soc/timer_group_struct.h>

namespace grevir::arduino_esp32 {

// Classic ESP32 only. The application reserves TG0/T0 against other timer
// drivers. The callback runs at interrupt level 1 with cache-dependent code.
class TimerGroup0Timer0 {
 public:
  using Callback = void (*)() noexcept;

  static bool install(Callback callback) noexcept {
    if (!clock_enabled_ || handle_ != nullptr || callback == nullptr || failed_) {
      return false;
    }
    callback_ = callback;
    const esp_err_t result = esp_intr_alloc(ETS_TG0_T0_LEVEL_INTR_SOURCE,
      ESP_INTR_FLAG_LEVEL1, &entry, nullptr, &handle_);
    if (result != ESP_OK) {
      callback_ = nullptr;
      failed_ = true;
      return false;
    }
    mask();
    return true;
  }

  static bool configure_periodic(std::uint16_t divider,
                                 std::uint32_t alarm_ticks) noexcept {
    if (clock_enabled_ || handle_ != nullptr || failed_ || divider < 2
        || alarm_ticks == 0) { return false; }
    periph_module_enable(PERIPH_TIMG0_MODULE);
    clock_enabled_ = true;
    mask();
    TIMERG0.hw_timer[0].config.val = 0;
    TIMERG0.hw_timer[0].config.tx_divider = divider;
    TIMERG0.hw_timer[0].config.tx_increase = 1;
    TIMERG0.hw_timer[0].config.tx_autoreload = 1;
    TIMERG0.hw_timer[0].config.tx_level_int_en = 1;
    TIMERG0.hw_timer[0].config.tx_alarm_en = 1;
    TIMERG0.hw_timer[0].alarmlo.val = alarm_ticks;
    TIMERG0.hw_timer[0].alarmhi.val = 0;
    TIMERG0.hw_timer[0].loadlo.val = 0;
    TIMERG0.hw_timer[0].loadhi.val = 0;
    TIMERG0.hw_timer[0].load.val = 1;
    return true;
  }

  static void mask() noexcept {
    if (!clock_enabled_) { return; }
    TIMERG0.int_ena_timers.t0_int_ena = 0;
    TIMERG0.hw_timer[0].config.tx_en = 0;
  }

  static void enable_preserving_pending() noexcept {
    if (!clock_enabled_) { return; }
    TIMERG0.int_ena_timers.t0_int_ena = 1;
    TIMERG0.hw_timer[0].config.tx_en = 1;
  }

  static void start_without_interrupt() noexcept {
    if (!clock_enabled_) { return; }
    TIMERG0.int_ena_timers.t0_int_ena = 0;
    TIMERG0.hw_timer[0].config.tx_en = 1;
  }

  static bool cleanup() noexcept {
    mask();
    if (handle_ != nullptr) {
      const esp_err_t result = esp_intr_free(handle_);
      if (result != ESP_OK) { failed_ = true; return false; }
      handle_ = nullptr;
    }
    callback_ = nullptr;
    if (clock_enabled_) {
      periph_module_disable(PERIPH_TIMG0_MODULE);
      clock_enabled_ = false;
    }
    return true;
  }

 private:
  static void entry(void*) noexcept {
    const bool pending = TIMERG0.int_st_timers.t0_int_st != 0;
    if (!pending) { return; }
    TIMERG0.int_clr_timers.t0_int_clr = 1;
    TIMERG0.hw_timer[0].config.tx_alarm_en = 1;
    if (callback_ != nullptr) { callback_(); }
  }

  inline static intr_handle_t handle_ = nullptr;
  inline static Callback callback_ = nullptr;
  inline static bool failed_ = false;
  inline static bool clock_enabled_ = false;
};

} // namespace grevir::arduino_esp32
#endif
