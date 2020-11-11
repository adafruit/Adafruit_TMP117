/**
 * @file basic_test.ino
 * @author Bryan Siepert for Adafruit Industries
 * @brief Shows how to specify a
 * @date 2020-11-10
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#include <Wire.h>
#include <Adafruit_TMP117.h>
#include <Adafruit_Sensor.h>

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

}
void loop() {

  sensors_event_t temp; // create an empty event to be filled
  tmp117.getEvent(&temp); //fill the empty event object with the current measurements
  Serial.print("Temperature  "); Serial.print(temp.temperature);Serial.println(" degrees C");
  Serial.println("");

  delay(1000);
}
