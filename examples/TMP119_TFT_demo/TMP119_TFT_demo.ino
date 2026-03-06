/*!
 * @file TMP119_TFT_demo.ino
 *
 * TFT demo for TMP119 High-Accuracy Temperature Sensor on Feather ESP32-S2 TFT
 * Displays temperature as a large color-coded readout.
 * Uses GFXcanvas16 to eliminate flicker.
 *
 * Hardware:
 *  - Adafruit Feather ESP32-S2 TFT
 *  - TMP119 Temperature Sensor breakout (I2C)
 *
 * Written by Limor Fried/Ladyada for Adafruit Industries.
 * BSD license, all text above must be included in any redistribution
 */

#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <Adafruit_TMP119.h>
#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeSansBold12pt7b.h>
#include <Fonts/FreeSansBold18pt7b.h>
#include <Fonts/FreeSansBold24pt7b.h>
#include <Fonts/FreeSansBold9pt7b.h>
#include <Wire.h>

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);
Adafruit_TMP119 tmp119;

#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 135

#define COLOR_TITLE 0xFFFF

// Canvas for the temperature area (below title)
#define TEMP_Y 28
#define TEMP_W 240
#define TEMP_H 107

GFXcanvas16 tempCanvas(TEMP_W, TEMP_H);

uint16_t getTempColor(float temp) {
  if (temp < 15.0)
    return 0x3BFF; // cool blue
  else if (temp < 22.0)
    return 0x07E0; // green
  else if (temp < 30.0)
    return 0xFD20; // orange
  else
    return 0xF800; // red/hot
}

void setup() {
  Serial.begin(115200);
  unsigned long start = millis();
  while (!Serial && (millis() - start < 3000))
    delay(10);

  Serial.println(F("TMP119 Temperature Sensor TFT Demo"));

  pinMode(TFT_BACKLITE, OUTPUT);
  digitalWrite(TFT_BACKLITE, HIGH);
  pinMode(TFT_I2C_POWER, OUTPUT);
  digitalWrite(TFT_I2C_POWER, HIGH);
  delay(10);

  tft.init(135, 240);
  tft.setRotation(3);
  tft.fillScreen(ST77XX_BLACK);

  // Centered title
  tft.setFont(&FreeSansBold12pt7b);
  tft.setTextColor(COLOR_TITLE);
  int16_t x1, y1;
  uint16_t tw, th;
  tft.getTextBounds("Adafruit TMP119", 0, 0, &x1, &y1, &tw, &th);
  tft.setCursor((SCREEN_WIDTH - tw) / 2, 22);
  tft.print(F("Adafruit TMP119"));

  Wire.begin();

  if (!tmp119.begin()) {
    Serial.println(F("Failed to find TMP119 sensor!"));
    tft.setFont(&FreeSansBold24pt7b);
    tft.setTextColor(ST77XX_RED);
    tft.setCursor(20, 90);
    tft.print(F("No Sensor!"));
    while (1)
      delay(100);
  }

  Serial.println(F("TMP119 found!"));

  tmp119.setAveragedSampleCount(TMP117_AVERAGE_64X);
  tmp119.setReadDelay(TMP117_DELAY_0_MS);
  tmp119.setMeasurementMode(TMP117_MODE_CONTINUOUS);
}

void loop() {
  sensors_event_t t;
  if (!tmp119.getEvent(&t)) {
    Serial.println(F("Failed to read TMP119!"));
    delay(500);
    return;
  }

  float temp = t.temperature;
  Serial.print(F("Temp: "));
  Serial.print(temp, 4);
  Serial.println(F(" C"));

  // Render temperature to canvas
  tempCanvas.fillScreen(ST77XX_BLACK);

  uint16_t color = getTempColor(temp);

  // Big temperature number
  tempCanvas.setFont(&FreeSansBold24pt7b);
  tempCanvas.setTextSize(1);
  tempCanvas.setTextColor(color);

  char buf[10];
  dtostrf(temp, 0, 2, buf);

  // Measure the number portion
  int16_t x1, y1;
  uint16_t numW, numH;
  tempCanvas.getTextBounds(buf, 0, 0, &x1, &y1, &numW, &numH);

  // Measure "C" in 18pt for the unit
  tempCanvas.setFont(&FreeSansBold18pt7b);
  int16_t cx1, cy1;
  uint16_t cW, cH;
  tempCanvas.getTextBounds("C", 0, 0, &cx1, &cy1, &cW, &cH);

  // Total width: number + small gap + degree circle + C
  uint16_t degGap = 4;
  uint16_t degR = 4;
  uint16_t totalW = numW + degGap + degR * 2 + 2 + cW;
  int16_t tx = (TEMP_W - totalW) / 2;
  int16_t ty = (TEMP_H + numH) / 2;

  // Draw temperature number
  tempCanvas.setFont(&FreeSansBold24pt7b);
  tempCanvas.setCursor(tx, ty);
  tempCanvas.print(buf);

  // Draw degree symbol (small circle)
  int16_t degX = tempCanvas.getCursorX() + degGap + degR;
  int16_t degY = ty - numH + degR + 2;
  tempCanvas.drawCircle(degX, degY, degR, color);
  tempCanvas.drawCircle(degX, degY, degR - 1, color);

  // Draw "C"
  tempCanvas.setFont(&FreeSansBold18pt7b);
  tempCanvas.setCursor(degX + degR + 2, ty);
  tempCanvas.print(F("C"));

  tft.drawRGBBitmap(0, TEMP_Y, tempCanvas.getBuffer(), TEMP_W, TEMP_H);

  delay(500);
}
