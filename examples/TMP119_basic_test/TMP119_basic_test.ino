/**
 * @file TMP119_basic_test.ino
 * @brief Basic test sketch for the TMP117/TMP119 temperature sensor
 * @date 2025-02-25
 *
 * @copyright Copyright (c) 2025
 *
 */
#include <Adafruit_Sensor.h>
// #include <Adafruit_TMP117.h>
#include <Adafruit_TMP119.h>
#include <Wire.h>

// Adafruit_TMP117 tmp117;
Adafruit_TMP119 tmp119;

// To use with TMP117 instead, uncomment the TMP117 lines above
// and comment out the TMP119 lines, then replace tmp119 with tmp117 below

void setup(void) {
  Serial.begin(115200);
  while (!Serial)
    delay(10); // will pause Zero, Leonardo, etc until serial console opens
  Serial.println(F("Adafruit TMP119 test!"));

  // Try to initialize!
  if (!tmp119.begin()) {
    Serial.println(F("Failed to find TMP119 chip"));
    while (1) {
      delay(10);
    }
  }
  Serial.println(F("TMP119 Found!"));
}

void loop() {
  sensors_event_t temp; // create an empty event to be filled
  tmp119.getEvent(
      &temp); // fill the empty event object with the current measurements
  Serial.print(F("Temperature  "));
  Serial.print(temp.temperature);
  Serial.println(F(" degrees C"));
  Serial.println();

  delay(1000);
}
