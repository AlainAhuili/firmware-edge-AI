// Walking skeleton: sample an analog input into a sliding window and print a feature.
// The model inference step replaces the Serial print in a later iteration.
#include <Arduino.h>

#include "ring_buffer.h"

namespace {
constexpr int kSensorPin = 1;               // ADC1_CH0 on the ESP32-S3
constexpr uint32_t kSamplePeriodMs = 10;    // 100 Hz
constexpr std::size_t kWindow = 32;

RingBuffer<uint16_t, kWindow> window;
uint32_t last_sample_ms = 0;
}  // namespace

void setup() {
  Serial.begin(115200);
  analogReadResolution(12);
}

void loop() {
  const uint32_t now = millis();
  if (now - last_sample_ms < kSamplePeriodMs) {  // wrap-safe unsigned subtraction
    return;
  }
  last_sample_ms = now;

  if (window.full()) {
    uint16_t oldest;
    window.pop(oldest);  // slide the window by one sample
  }
  window.push(static_cast<uint16_t>(analogRead(kSensorPin)));

  if (window.full()) {
    Serial.printf("mean=%.1f\n", window_mean(window));
  }
}
