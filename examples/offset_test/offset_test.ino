/**
 * @file offset_test.ino
 * @author Bryan Siepert for Adafruit Industries
 * @brief Shows how to specify a temperature offset to adjust for bias in the measuremets due to
 * the enviromnent or other factors.
 * @date 2020-11-10
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#include <Wire.h>
#include <Adafruit_TMP117.h>
#include <Adafruit_Sensor.h>
#define TMP_OFFSET 10.4f

Adafruit_TMP117  tmp117;
void setup(void) {
  Serial.begin(115200);
  while (!Serial) delay(10);     // will pause Zero, Leonardo, etc until serial console opens
  Serial.println("Adafruit TMP117 test!");

  // Try to initialize!
  if (!tmp117.begin()) {
    Serial.println("Failed to find TMP117 chip");
    while (1) { delay(10); }
  }
  Serial.println("TMP117 Found!");

  sensors_event_t temp;
  tmp117.getEvent(&temp);// get temperature
  Serial.print("Temperature without offset:"); Serial.print(temp.temperature);Serial.println(" C");
  tmp117.setOffset(TMP_OFFSET);
}
void loop() {

  sensors_event_t temp;
  tmp117.getEvent(&temp);// get temperature
  Serial.print("Temperature with "); Serial.print(TMP_OFFSET, 1); Serial.print(" offset: ");Serial.print(temp.temperature);Serial.println(" degrees C");
  Serial.println("");

  delay(1000);
}
