# Grevir Arduino ESP32

Selected Arduino-ESP32 adapters for the classic ESP32 Dev Module. The package
currently supplies pin capability checks and a SerialPort0 module that starts at a
declared baud rate. It relies on the common Grevir Arduino GPIO and clock
adapters. Its first interrupt adapter directly configures classic ESP32
Timer Group 0 / Timer 0 and registers one callback through `esp_intr_alloc`.
The selected firmware compiles and links; physical interrupt routing has not
been tested. See the [interrupt guide](https://github.com/owebeeone/grevir-wz/blob/main/docs/guides/interrupts.md).

```cpp
#include <GrevirArduinoESP32.h>

using StatusLed = ardo::arduino_esp32::OutputPin<2>;
using Console = ardo::arduino_esp32::SerialPort0<115200>;
```

The [workspace documentation](https://github.com/owebeeone/grevir-wz/tree/main/docs)
describes installation and current validation. This package targets
Arduino-ESP32 3.3.11 and the `esp32:esp32:esp32` board profile for its first
application build. Physical-board behavior is not yet validated.
