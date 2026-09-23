#pragma once

#if defined(ARDUINO) && !defined(ARDUINO_ESP32_DEV)
#error "Grevir Arduino ESP32 currently supports only the ESP32 Dev Module board profile."
#endif

#include <GrevirArduino.h>
#include <grevir/arduino_esp32/pins.hpp>
#include <grevir/arduino_esp32/serial.hpp>
