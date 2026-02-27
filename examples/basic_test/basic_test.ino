/**
 * @file basic_test.ino
 * @author Bryan Siepert for Adafruit Industries
 * @brief Basic temperature reading example for TMP117/TMP119
 * @date 2020-11-10
 *
 * @copyright Copyright (c) 2020
 *
 */
#include <Adafruit_Sensor.h>
#include <Adafruit_TMP117.h>
#include <Adafruit_TMP119.h>
#include <Wire.h>

Adafruit_TMP117 tmp11x;
// Adafruit_TMP119 tmp11x;

// To use with TMP119 instead, uncomment the TMP119 include/line above
// and comment out the TMP117 include/line
void setup(void) {
  Serial.begin(115200);
  while (!Serial) delay(10);     // will pause Zero, Leonardo, etc until serial console opens
  Serial.println("Adafruit TMP117/TMP119 test!");

  // Try to initialize!
  if (!tmp11x.begin()) {
    Serial.println("Failed to find TMP117/TMP119 chip");
    while (1) { delay(10); }
  }
  Serial.println("TMP117/TMP119 Found!");

}
void loop() {
  // Wait for fresh data before reading
  while (!tmp11x.dataReady()) {
    delay(10);
  }

  sensors_event_t temp; // create an empty event to be filled
  tmp11x.getEvent(&temp); //fill the empty event object with the current measurements
  Serial.print("Temperature  "); Serial.print(temp.temperature);Serial.println(" degrees C");
  Serial.println("");

  delay(1000);
}
