
/**
 * @file oled_test.ino
 * @author Bryan Siepert for Adafruit Industries
 * @brief A simple demo that displays the measurements and alert status to
 * an attached 128x32 OLED screen
 * @date 2020-11-10
 *
 * @copyright Copyright (c) 2020
 *
 */
#include <Adafruit_TMP117.h>
#include <Adafruit_SSD1306.h>

Adafruit_SSD1306 display = Adafruit_SSD1306(128, 32, &Wire);

Adafruit_TMP117  tmp117;
void setup(void) {
  Serial.begin(115200);

  while (!Serial) 
     delay(10);     // will pause Zero, Leonardo, etc until serial console opens
  Serial.println("Adafruit TMP117 test!");

  // Try to initialize!
  if (!tmp117.begin()) {
    Serial.println("Failed to find TMP117 chip");
    while (1) { delay(10); }
  }
  Serial.println("TMP117 Found!");

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    while (1) { delay(10); } // Don't proceed, loop forever
  }
  display.clearDisplay();
  display.display();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setRotation(0);
}


void loop() {
  // Clear the display to be updated with new readings
  display.clearDisplay();
  display.setCursor(0,0);

  tmp117_alerts_t alerts;
  sensors_event_t temp;

  // Reading temp clears alerts, so read alerts first
  tmp117.getAlerts(&alerts);
  tmp117.getEvent(&temp);

  Serial.print("Temperature: ");
  Serial.print(temp.temperature);
  Serial.println(" degrees C");
  Serial.println("");

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
}
