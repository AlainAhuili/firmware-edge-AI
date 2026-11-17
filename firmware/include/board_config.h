// Pin and hardware constants for the ESP32-S3-DevKitC-1 (N16R8) setup.
// Everything wiring-related lives here, so a different board means one file to edit.
#pragma once

#include <cstdint>

namespace board {

// I2C bus shared by MPU-6050 (accelerometer), SSD1306 OLED and INA219.
constexpr int kI2cSda = 8;
constexpr int kI2cScl = 9;
constexpr uint32_t kI2cFrequencyHz = 400000;  // fast mode; all three parts support it

// What the N16R8 module must report. Clone boards sometimes ship other modules.
constexpr uint32_t kExpectedFlashBytes = 16u * 1024u * 1024u;
constexpr uint32_t kExpectedPsramBytes = 8u * 1024u * 1024u;

// Known I2C addresses on this setup.
constexpr uint8_t kMpu6050Address = 0x68;  // 0x69 if its AD0 pin is pulled high
constexpr uint8_t kOledAddress = 0x3C;
constexpr uint8_t kIna219Address = 0x40;

// MPU-6050 WHO_AM_I register. Genuine MPU-6050: 0x68. Common clones answer
// 0x70 (MPU-6500) or 0x72; they work, but some registers differ.
constexpr uint8_t kMpuWhoAmIRegister = 0x75;

}  // namespace board
