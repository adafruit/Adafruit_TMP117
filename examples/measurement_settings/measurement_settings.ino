/**
 * @file measurement_settings.ino
 * @author Bryan Siepert for Adafruit Industries
 * @brief Demonstrates the different TMP117/TMP119 settings that can be adjusted
 * to change the behavior of the sensor, including measurement averaging,
 * interval between readings, setting an offset, and measurement mode
 * @date 2020-11-10
 *
 * @copyright Copyright (c) 2020
 *
 */

#include <Adafruit_Sensor.h>
#include <Adafruit_TMP117.h>
// #include <Adafruit_TMP119.h>
#include <Wire.h>

Adafruit_TMP117 tmp11x;
// Adafruit_TMP119 tmp11x;

// To use with TMP119 instead, uncomment the TMP119 include/line above
// and comment out the TMP117 include/line
void setup(void) {
  Serial.begin(115200);
  while (!Serial)
    delay(10); // will pause Zero, Leonardo, etc until serial console opens
  Serial.println("Adafruit TMP117 settings example");

  // Try to initialize!
  if (!tmp11x.begin()) {
    Serial.println("Failed to find TMP117 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("TMP117 Found!");

  // Raising the number of averaged temperature samples (individual
  // measurements) will make measurements take longer but will make them less
  // sensitive to quick, temporary changes.

  // tmp11x.setAveragedSampleCount(TMP117_AVERAGE_32X);
  Serial.print("Temperature averaged from ");
  switch (tmp11x.getAveragedSampleCount()) {
  case TMP117_AVERAGE_1X:
    Serial.print(" 1");
    break;
  case TMP117_AVERAGE_8X:
    Serial.print(" 8");
    break;
  case TMP117_AVERAGE_32X:
    Serial.print(" 32");
    break;
  case TMP117_AVERAGE_64X:
    Serial.print(" 64");
    break;
  }
  Serial.println(" samples");

  // Adjusting the read delay will allow you to change the sensor's behavior to
  // fir your needs. In normal circumstances temperature doesn't change quickly,
  // however other situations such as when the temperature is being changed by a
  // heater or cooler, less time between readings  will allow you to know more
  // accurately when the temperature is at a given value.

  // tmp11x.setReadDelay(TMP117_DELAY_125_MS);
  Serial.print("Interval between reads is at least ");
  switch (tmp11x.getReadDelay()) {
  case TMP117_DELAY_0_MS:
    Serial.print(0);
    break;
  case TMP117_DELAY_125_MS:
    Serial.print(125);
    break;
  case TMP117_DELAY_250_MS:
    Serial.print(250);
    break;
  case TMP117_DELAY_500_MS:
    Serial.print(500);
    break;
  case TMP117_DELAY_1000_MS:
    Serial.print(1000);
    break;
  case TMP117_DELAY_4000_MS:
    Serial.print(4000);
    break;
  case TMP117_DELAY_8000_MS:
    Serial.print(8000);
    break;
  case TMP117_DELAY_16000_MS:
    Serial.print(16000);
    break;
  }
  Serial.println(" milliseconds");

  // Changing the measurement mode will have a large impact on the power usage
  // of the sensor, Since you can turn it off when not used!

  // One-shot mode: triggers a single conversion, then returns to shutdown.
  // This is the lowest power option for infrequent readings.
  tmp11x.setMeasurementMode(TMP117_MODE_ONE_SHOT);
  Serial.print("Measurement mode: ");
  switch (tmp11x.getMeasurementMode()) {
  case TMP117_MODE_SHUTDOWN:
    Serial.println("Shut down");
    break;
  // This will almost never be read because of the switch to SHUTDOWN after the
  // measurement is taken.
  case TMP117_MODE_ONE_SHOT:
    Serial.println("One shot");
    break;
  case TMP117_MODE_CONTINUOUS:
    Serial.println("Continuous");
    break;
  }

  // Set a positive or negative offset here to see how it changes the measured
  // values. A temperature offset can be used to adjust for a static bias in
  // measuremets

  // tmp11x.setOffset(10.4f);
  float temp_offset = tmp11x.getOffset();
  if (temp_offset != 0) {
    sensors_event_t temp;
    tmp11x.getEvent(&temp); // get temperature
    Serial.print("Temperature without offset:");
    Serial.print(temp.temperature);
    Serial.println(" C");
    Serial.println("");
    Serial.print("Temperatures with offset of ");
    Serial.print(temp_offset, 1);
    Serial.println(" C:");
  }

  Serial.println("");
}
void loop() {
  // ONE-SHOT MODE DEMO
  // In one-shot mode, each write to ONE_SHOT triggers a single conversion,
  // then the sensor returns to shutdown mode to save power.
  //
  // IMPORTANT: You MUST wait for dataReady() before reading, otherwise
  // you'll get stale data from the previous conversion!
  //
  // Conversion time depends on averaging setting (15.5ms per sample):
  //   1 sample  = ~15.5ms
  //   8 samples = ~125ms
  //   32 samples = ~500ms
  //   64 samples = ~1000ms

  // Trigger a one-shot conversion
  tmp11x.setMeasurementMode(TMP117_MODE_ONE_SHOT);

  // Poll dataReady() until conversion is complete
  while (!tmp11x.dataReady()) {
    delay(1);
  }

  // Now we can read the fresh temperature
  sensors_event_t temp;
  tmp11x.getEvent(&temp);
  Serial.print(F("Temperature: "));
  Serial.print(temp.temperature);
  Serial.println(F(" degrees C"));
  Serial.println();

  // Wait before triggering the next one-shot conversion
  delay(1000);
}
