cmake_minimum_required(VERSION 3.20)
project(grevir_arduino_esp32 VERSION 0.1.0 LANGUAGES CXX)

foreach(dependency IN ITEMS base core peripherals arduino)
  if(NOT TARGET grevir::${dependency})
    find_package(grevir-${dependency} CONFIG REQUIRED)
  endif()
endforeach()

add_library(grevir_arduino_esp32 INTERFACE)
add_library(grevir::arduino_esp32 ALIAS grevir_arduino_esp32)
set_target_properties(grevir_arduino_esp32 PROPERTIES EXPORT_NAME arduino_esp32)
target_compile_features(grevir_arduino_esp32 INTERFACE cxx_std_23)
target_link_libraries(grevir_arduino_esp32 INTERFACE
  grevir::base grevir::core grevir::peripherals grevir::arduino)
target_include_directories(grevir_arduino_esp32 INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
  $<INSTALL_INTERFACE:include>)

include(GNUInstallDirs)
install(TARGETS grevir_arduino_esp32 EXPORT GrevirArduinoESP32Targets)
install(DIRECTORY src/ DESTINATION include FILES_MATCHING
  PATTERN "*.hpp" PATTERN "*.h")
install(EXPORT GrevirArduinoESP32Targets NAMESPACE grevir::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/grevir-arduino-esp32)
install(FILES cmake/grevir-arduino-esp32-config.cmake
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/grevir-arduino-esp32)
install(FILES LICENSE.txt DESTINATION ${CMAKE_INSTALL_DATADIR}/grevir-arduino-esp32)

option(GREVIR_BUILD_HOST_TESTS "Run native ESP32 adapter policy tests" OFF)
if(GREVIR_BUILD_HOST_TESTS)
  enable_testing()
  find_package(Threads REQUIRED)
  add_executable(grevir_arduino_esp32_start_policy_test tests/start_policy_test.cpp)
  target_link_libraries(grevir_arduino_esp32_start_policy_test PRIVATE
    grevir::arduino_esp32 Threads::Threads)
  add_test(NAME grevir_arduino_esp32_start_policy
    COMMAND grevir_arduino_esp32_start_policy_test)
endif()
