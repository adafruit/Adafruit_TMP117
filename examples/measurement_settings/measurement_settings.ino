/**
 * @file average_counts.ino
 * @author Bryan Siepert for Adafruit Industries
 * @brief Demonstrates the different settings that can be adjusted to
 * change the behavior of the sensor, including measurement averaging,
 * interval between readings, and measurement mode
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
  Serial.println("Adafruit TMP117 settings example");

  // Try to initialize!
  if (!tmp117.begin()) {
    Serial.println("Failed to find TMP117 chip");
    while (1) { delay(10); }
  }
  Serial.println("TMP117 Found!");

  // tmp117.setAveragedSampleCount(TMP117_AVERAGE_32X);
  Serial.print("Temperature averaged from ");
  switch(tmp117.getAveragedSampleCount()){
    case TMP117_AVERAGE_1X: Serial.print(" 1");break;
    case TMP117_AVERAGE_8X: Serial.print(" 8");break;
    case TMP117_AVERAGE_32X: Serial.print(" 32");break;
    case TMP117_AVERAGE_64X: Serial.print(" 64");break;
  }
  Serial.println(" samples");

  // tmp117.setReadDelay(TMP117_DELAY_125_MS);
  Serial.print("Interval between reads is at least ");
  switch(tmp117.getReadDelay()){
    case TMP117_DELAY_0_MS: Serial.print(0); break;
    case TMP117_DELAY_125_MS: Serial.print(125); break;
    case TMP117_DELAY_250_MS: Serial.print(250); break;
    case TMP117_DELAY_500_MS: Serial.print(500); break;
    case TMP117_DELAY_1000_MS: Serial.print(1000); break;
    case TMP117_DELAY_4000_MS: Serial.print(4000); break;
    case TMP117_DELAY_8000_MS: Serial.print(8000); break;
    case TMP117_DELAY_16000_MS: Serial.print(16000); break;
  }
  Serial.println(" milliseconds");

  // tmp117.setMeasurementMode(TMP117_MODE_SHUTDOWN);
  Serial.print("Measurement mode: ");
  switch(tmp117.getMeasurementMode()){
    case TMP117_MODE_SHUTDOWN: Serial.println("Shut down"); break;
    // This will almost never be read because of the switch to SHUTDOWN after the measurement is
    // taken.
    case TMP117_MODE_ONE_SHOT: Serial.println("One shot"); break;
    case TMP117_MODE_CONTINUOUS: Serial.println("Continuous"); break;
  }
  Serial.println("");

}
void loop() {

  sensors_event_t temp;
  tmp117.getEvent(&temp);// get temperature
  Serial.print("Temperature: ");Serial.print(temp.temperature);Serial.println(" degrees C");
  Serial.println("");
  delay(1000);
}