// Week 1, step 1: board check.
//
// Verifies that the board is the expected N16R8 (16 MB flash, 8 MB PSRAM),
// scans the I2C bus for the MPU-6050, OLED and INA219, reads the MPU-6050's
// WHO_AM_I register, and blinks the RGB LED so a working upload is visible.
// Open the serial monitor at 115200 baud and press the board's RST button to
// see the report again.

#include <Arduino.h>
#include <Wire.h>

#include "board_config.h"

namespace {

int g_checks_failed = 0;

void check(bool ok, const char* what) {
  Serial.printf("  [%s] %s\n", ok ? "PASS" : "FAIL", what);
  if (!ok) {
    ++g_checks_failed;
  }
}

const char* name_for_address(uint8_t address) {
  switch (address) {
    case 0x68:
    case 0x69:
      return "MPU-6050 accelerometer/gyro";
    case 0x3C:
    case 0x3D:
      return "SSD1306 OLED";
    case 0x40:
      return "INA219 current sensor";
    default:
      return "unknown device";
  }
}

void report_chip() {
  Serial.println("Chip");
  Serial.printf("  Model: %s rev %d, %d cores @ %lu MHz\n", ESP.getChipModel(),
                ESP.getChipRevision(), ESP.getChipCores(),
                static_cast<unsigned long>(ESP.getCpuFreqMHz()));
  Serial.printf("  Flash: %lu MB\n",
                static_cast<unsigned long>(ESP.getFlashChipSize() / (1024 * 1024)));
  Serial.printf("  PSRAM: %lu MB (free %lu KB)\n",
                static_cast<unsigned long>(ESP.getPsramSize() / (1024 * 1024)),
                static_cast<unsigned long>(ESP.getFreePsram() / 1024));
  Serial.printf("  Internal heap free: %lu KB\n",
                static_cast<unsigned long>(ESP.getFreeHeap() / 1024));
  Serial.printf("  SDK: %s\n", ESP.getSdkVersion());

  check(ESP.getFlashChipSize() == board::kExpectedFlashBytes, "16 MB flash");
  check(ESP.getPsramSize() >= board::kExpectedPsramBytes - 512u * 1024u,
        "8 MB PSRAM (a few hundred KB are reserved by the system)");
}

bool i2c_device_present(uint8_t address) {
  Wire.beginTransmission(address);
  return Wire.endTransmission() == 0;
}

int scan_i2c_bus() {
  Serial.printf("I2C scan (SDA=%d, SCL=%d)\n", board::kI2cSda, board::kI2cScl);
  int found = 0;
  for (uint8_t address = 0x08; address < 0x78; ++address) {
    if (i2c_device_present(address)) {
      Serial.printf("  0x%02X  %s\n", address, name_for_address(address));
      ++found;
    }
  }
  if (found == 0) {
    Serial.println("  no devices: check wiring (VCC 3V3, GND, SDA, SCL)");
  }
  return found;
}

bool read_register(uint8_t device, uint8_t reg, uint8_t& value) {
  Wire.beginTransmission(device);
  Wire.write(reg);
  if (Wire.endTransmission(false) != 0) {
    return false;
  }
  if (Wire.requestFrom(device, static_cast<uint8_t>(1)) != 1) {
    return false;
  }
  value = static_cast<uint8_t>(Wire.read());
  return true;
}

void check_mpu6050() {
  Serial.println("MPU-6050");
  uint8_t who_am_i = 0;
  if (!read_register(board::kMpu6050Address, board::kMpuWhoAmIRegister, who_am_i)) {
    check(false, "MPU-6050 answers at 0x68");
    return;
  }
  Serial.printf("  WHO_AM_I = 0x%02X\n", who_am_i);
  const bool genuine = who_am_i == 0x68;
  const bool clone = who_am_i == 0x70 || who_am_i == 0x72;
  check(genuine || clone, genuine ? "genuine MPU-6050"
                                  : "MPU-6500-compatible clone (usable)");
}


void run_board_check() {
  g_checks_failed = 0;
  Serial.println();
  Serial.println("=== firmware-edge-AI board check ===");
  report_chip();

  scan_i2c_bus();
  if (i2c_device_present(board::kMpu6050Address)) {
    check_mpu6050();
  } else {
    Serial.println("MPU-6050 not found at 0x68 (skipping WHO_AM_I check)");
  }

  Serial.printf("=== %s: %d check(s) failed ===\n",
                g_checks_failed == 0 ? "ALL PASSED" : "ATTENTION", g_checks_failed);
  Serial.println("(press Enter in the serial monitor to run the check again)");
}

}  // namespace

void setup() {
  Serial.begin(115200);
  // Over the native USB port the serial connection only exists once the PC has
  // re-attached after reset. Wait up to 5 s for it, so the report isn't printed
  // into the void; over the UART port Serial is ready at once.
  const uint32_t start = millis();
  while (!Serial && millis() - start < board::kSerialWaitMs) {
    delay(10);
  }

  Wire.begin(board::kI2cSda, board::kI2cScl, board::kI2cFrequencyHz);
  run_board_check();
}

void loop() {
  // Any input from the serial monitor re-runs the check, so the report can be
  // seen even if the monitor attached after the board started.
  if (Serial.available() > 0) {
    while (Serial.available() > 0) {
      Serial.read();
    }
    run_board_check();
  }

  // Green blink = all checks passed; red blink = a check failed. RGB_BUILTIN is
  // the on-board WS2812 (GPIO 48 on DevKitC-1 v1.0, GPIO 38 on v1.1 boards).
  const uint8_t red = g_checks_failed ? 40 : 0;
  const uint8_t green = g_checks_failed ? 0 : 40;
  neopixelWrite(RGB_BUILTIN, red, green, 0);
  delay(500);
  neopixelWrite(RGB_BUILTIN, 0, 0, 0);
  delay(500);
}
