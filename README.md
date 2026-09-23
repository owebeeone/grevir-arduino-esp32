# Grevir Arduino ESP32

Selected Arduino-ESP32 adapters for the classic ESP32 Dev Module. The package
currently supplies pin capability checks and a SerialPort0 module that starts at a
declared baud rate. It relies on the common Grevir Arduino GPIO and clock
adapters. It does not provide an ESP32 timer, interrupt, or peripheral allocator.

```cpp
#include <GrevirArduinoESP32.h>

using StatusLed = ardo::arduino_esp32::OutputPin<2>;
using Console = ardo::arduino_esp32::SerialPort0<115200>;
```

The [workspace documentation](https://github.com/owebeeone/grevir-wz/tree/main/docs)
describes installation and current validation. This package targets
Arduino-ESP32 3.3.11 and the `esp32:esp32:esp32` board profile for its first
application build. Physical-board behavior is not yet validated.
