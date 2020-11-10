
#include <Adafruit_SSD1306.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_TMP117.h>
#include <Wire.h>

Adafruit_SSD1306 display = Adafruit_SSD1306(128, 32, &Wire);

Adafruit_TMP117 tmp117;
void setup(void) {
  Serial.begin(115200);
  while (!Serial)
    delay(10); // will pause Zero, Leonardo, etc until serial console opens
  Serial.println("Adafruit TMP117 test!");

  // Try to initialize!
  if (!tmp117.begin()) {
    Serial.println("Failed to find TMP117 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("TMP117 Found!");

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ; // Don't proceed, loop forever
  }
  display.clearDisplay();
  display.display();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setRotation(0);

  tmp117.setHighThreshold(30.0);
  Serial.print("High threshold: "); Serial.println(tmp117.getHighThreshold(), 1);
  tmp117.setLowThreshold(10.5);
  Serial.print("Low threshold: "); Serial.println(tmp117.getLowThreshold(), 1);
}
void loop() {
  display.clearDisplay();
  display.setCursor(0, 0);

  tmp117_alerts_t alerts;
  sensors_event_t temp;
  // Reading temp clears alerts, so read alerts first
  tmp117.getAlerts(&alerts); // get the status of any alerts
  tmp117.getEvent(&temp);   // get temperature

  Serial.print("Temperature: ");
  Serial.print(temp.temperature);
  Serial.println(" degrees C");
  Serial.println("");

  Serial.print("High temperature alert active:");
  if (alerts.high) {
    Serial.println("True");
  } else {
    Serial.println("False");
  }

  Serial.print("Low temperature alert active:");
  if (alerts.low) {
    Serial.println("True");
  } else {
    Serial.println("False");
  }

  //       Print to OLED
  display.print("Tmp:");
  display.print(temp.temperature, 1);
  display.println(" C");

  display.print("HI:");
  if (alerts.high) {
    display.print("Y");
  } else {
    display.print("N");
  }

  display.print(" LOW:");
  if (alerts.low) {
    display.println("Y");
  } else {
    display.println("N");
  }

  display.display();
  delay(300);
}
