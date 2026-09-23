#pragma once

#include <grevir/arduino/arduino_api.hpp>
#include <grevir/core/resource_claims.hpp>

namespace ardo::arduino_esp32 {

template <unsigned long Baud>
class SerialPort0 {
public:
  // The selected Dev Module variant maps UART0 TX/RX to GPIO 1/3.
  using Claims = ardo::ResourceClaim<
    ardo::SerialResource<0>, ardo::GPIOResource<1>, ardo::GPIOResource<3>>;

  static void runSetup() {
    Serial.begin(Baud);
  }

  static void runLoop() {}

  template <typename... Parts>
  static void print(const Parts&... parts) {
    (Serial.print(parts), ...);
  }

  template <typename... Parts>
  static void println(const Parts&... parts) {
    print(parts...);
    Serial.println();
  }
};

} // namespace ardo::arduino_esp32
