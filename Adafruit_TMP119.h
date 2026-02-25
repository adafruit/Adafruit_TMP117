/*!
 *  @file Adafruit_TMP119.h
 *
 *  I2C Driver for the TMP119 high-accuracy temperature sensor
 *  This is a library written to work with the Adafruit TMP119 breakout.
 *
 *  Adafruit invests time and resources providing this open source code,
 *  please support Adafruit and open-source hardware by purchasing products from
 *  Adafruit!
 *
 *  BSD license (see license.txt)
 */

#ifndef _ADAFRUIT_TMP119_H
#define _ADAFRUIT_TMP119_H

#include "Adafruit_TMP117.h"

#define TMP119_CHIP_ID 0x2117 ///< TMP119 device id

/*!
 *    @brief  Class that stores state and functions for interacting with
 *            the TMP119 High-Accuracy Temperature Sensor
 */
class Adafruit_TMP119 : public Adafruit_TMP117 {
public:
  Adafruit_TMP119();
  bool begin(uint8_t i2c_addr = TMP117_I2CADDR_DEFAULT, TwoWire *wire = &Wire,
             int32_t sensor_id = 119);

protected:
  bool _init(int32_t sensor_id) override;
};

#endif
