#pragma once

#include <grevir/arduino/pins.hpp>

namespace ardo::arduino_esp32 {

// Classic ESP32 GPIOs exposed by the ESP32 Dev Module. GPIO 6-11 serve flash;
// GPIO 34-39 are input-only. This is a pin capability check, not a claim that
// every exposed pin is safe with every attached board or boot configuration.
constexpr bool is_gpio(unsigned pin) {
  return pin <= 19 || (pin >= 21 && pin <= 23) ||
         (pin >= 25 && pin <= 27) || (pin >= 32 && pin <= 39);
}

constexpr bool is_external_gpio(unsigned pin) {
  return is_gpio(pin) && (pin < 6 || pin > 11);
}

constexpr bool is_output_gpio(unsigned pin) {
  return is_external_gpio(pin) && pin < 34;
}

template <unsigned Pin>
class ExternalPin : public ardo::ExternalPin<Pin> {
  static_assert(is_external_gpio(Pin), "Pin is not an exposed ESP32 Dev Module GPIO");
};

template <unsigned Pin>
class OutputPin : public ardo::arduino::OutputPin<Pin> {
  static_assert(is_output_gpio(Pin), "Pin is not an output-capable ESP32 Dev Module GPIO");
};

} // namespace ardo::arduino_esp32
