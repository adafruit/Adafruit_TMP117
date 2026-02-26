/*!
 *  @file Adafruit_TMP119.cpp
 *
 *  @brief I2C Driver for the TMP119 High-Accuracy Temperature Sensor
 *
 *  This is a library for the TMP119 temperature sensor, which is a variant
 *  of the TMP117 with a different chip ID (0x2117 instead of 0x0117).
 *
 *  Adafruit invests time and resources providing this open source code.
 *  Please support Adafruit and open-source hardware by purchasing products from
 *  Adafruit!
 *
 *  BSD (see license.txt)
 */

#include "Adafruit_TMP119.h"

/**
 * @brief Construct a new Adafruit_TMP119::Adafruit_TMP119 object
 *
 */
Adafruit_TMP119::Adafruit_TMP119(void) : Adafruit_TMP117() {}

/**
 * @brief Sets up the hardware and initializes I2C
 *
 * @param i2c_address The I2C address to be used.
 * @param wire The Wire object to be used for I2C connections.
 * @param sensor_id The unique ID to differentiate the sensors from others
 * @return True if initialization was successful, otherwise false.
 */
bool Adafruit_TMP119::begin(uint8_t i2c_address, TwoWire *wire,
                            int32_t sensor_id) {
  i2c_dev = new Adafruit_I2CDevice(i2c_address, wire);

  if (!i2c_dev->begin()) {
    return false;
  }

  return _init(sensor_id);
}

/**
 * @brief Initializer for post bus-init setup (TMP119 variant)
 *
 * @param sensor_id Optional unique ID for the sensor set
 * @return True if chip identified and initialized
 */
bool Adafruit_TMP119::_init(int32_t sensor_id) {
  Adafruit_BusIO_Register chip_id =
      Adafruit_BusIO_Register(i2c_dev, TMP117_WHOAMI, 2, MSBFIRST);

  // make sure we're talking to the right chip (TMP119 has ID 0x2117)
  uint8_t buffer[2];
  chip_id.read(buffer, 2);
  if ((buffer[0] << 8 | buffer[1]) != TMP119_CHIP_ID) {
    return false;
  }

  _sensorid_temp = sensor_id;

  // do any software reset or other initial setup
  config_reg =
      new Adafruit_BusIO_Register(i2c_dev, TMP117_CONFIGURATION, 2, MSBFIRST);

  temp_reg =
      new Adafruit_BusIO_Register(i2c_dev, TMP117_TEMP_DATA, 2, MSBFIRST);

  reset();

  return true;
}
