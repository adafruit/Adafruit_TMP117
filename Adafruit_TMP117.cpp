/*!
 *  @file Adafruit_TMP117.cpp
 *
 *  @mainpage Adafruit  Arduino library for the TMP117 high-accuracy temperature
 * sensor
 *
 *  @section intro_sec Introduction
 *
 * 	I2C Driver for the Library for the TMP117 High-Accuract Temperature
 * Sensor
 *
 * 	This is a library for the Adafruit TMP117 breakout:
 * 	https://www.adafruit.com/product/4821
 *
 * 	Adafruit invests time and resources providing this open source code,
 *  please support Adafruit and open-source hardware by purchasing products from
 * 	Adafruit!
 *
 *  @section dependencies Dependencies
 *  This library depends on the Adafruit BusIO library
 *
 *  This library depends on the Adafruit Unified Sensor library
 *
 *  @section author Author
 *
 *  Bryan Siepert for Adafruit Industries
 *
 * 	@section license License
 *
 * 	BSD (see license.txt)
 *
 * 	@section  HISTORY
 *
 *     v1.0 - First release
 */

#include "Arduino.h"
#include <Wire.h>

#include "Adafruit_TMP117.h"

/**
 * @brief Construct a new Adafruit_TMP117::Adafruit_TMP117 object
 *
 */
Adafruit_TMP117::Adafruit_TMP117(void) {}

/**
 * @brief Destroy the Adafruit_TMP117::Adafruit_TMP117 object
 *
 */
Adafruit_TMP117::~Adafruit_TMP117(void) {
  if (i2c_dev)
    delete i2c_dev;
  if (config_reg)
    delete config_reg;
  if (temp_reg)
    delete temp_reg;
}

/*!
 *    @brief  Sets up the hardware and initializes I2C
 *    @param  i2c_address
 *            The I2C address to be used.
 *    @param  wire
 *            The Wire object to be used for I2C connections.
 *    @param  sensor_id
 *            The unique ID to differentiate the sensors from others
 *    @return True if initialization was successful, otherwise false.
 */
bool Adafruit_TMP117::begin(uint8_t i2c_address, TwoWire *wire,
                            int32_t sensor_id) {
  i2c_dev = new Adafruit_I2CDevice(i2c_address, wire);

  if (!i2c_dev->begin()) {
    return false;
  }

  return _init(sensor_id);
}

/*!  @brief Initializer for post bus-init setup
 *   @param sensor_id Optional unique ID for the sensor set
 *   @returns True if chip identified and initialized
 */
bool Adafruit_TMP117::_init(int32_t sensor_id) {

  Adafruit_BusIO_Register chip_id =
      Adafruit_BusIO_Register(i2c_dev, TMP117_WHOAMI, 2, MSBFIRST);
  // make sure we're talking to the right chip
  uint8_t buffer[2];
  chip_id.read(buffer, 2);
  if ((buffer[0] << 8 | buffer[1]) != WHOAMI_ANSWER) {
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

bool Adafruit_TMP117::getDataReady(void) {
  readAlertsDRDY();
  return (alert_drdy_flags & DRDY_FLAG) > 0;
}

void Adafruit_TMP117::readAlertsDRDY(void) {
  Adafruit_BusIO_RegisterBits alert_drdy_bits =
      Adafruit_BusIO_RegisterBits(config_reg, 3, 13);
  alert_drdy_flags = alert_drdy_bits.read();
}

void Adafruit_TMP117::waitForData(void) {
  while (!getDataReady()) {
    delay(1);
  }
}
/**
 * @brief Performs a software reset initializing registers to their power on
 * state
 *
 */
void Adafruit_TMP117::reset(void) {
  Adafruit_BusIO_RegisterBits sw_reset =
      Adafruit_BusIO_RegisterBits(config_reg, 1, 1);

  sw_reset.write(1);
  delay(2); // datasheet specifies 2ms for reset
  waitForData();
}

/**
 * @brief Gets the current rate at which pressure and temperature measurements
 * are taken
 *
 * @return tmp117_rate_t The current data rate
 */
tmp117_rate_t Adafruit_TMP117::getDataRate(void) {
  Adafruit_BusIO_Register config =
      Adafruit_BusIO_Register(i2c_dev, TMP117_CONFIGURATION, 1);
  Adafruit_BusIO_RegisterBits data_rate =
      Adafruit_BusIO_RegisterBits(config_reg, 3, 4);
  // _raw_measurement_delay = RWBits(3, _CONFIGURATION, 7, 2, False)

  return (tmp117_rate_t)data_rate.read();
}
/**
 * @brief Sets the rate at which pressure and temperature measurements
 *
 * @param new_data_rate The data rate to set. Must be a `tmp117_rate_t`
 */
void Adafruit_TMP117::setDataRate(tmp117_rate_t new_data_rate) {

  Adafruit_BusIO_RegisterBits data_rate =
      Adafruit_BusIO_RegisterBits(config_reg, 3, 4);

  data_rate.write((uint8_t)new_data_rate);
}

// /******************* Adafruit_Sensor functions *****************/
// /*!
//  *     @brief  Updates the measurement data for all sensors simultaneously
//  */
// /**************************************************************************/
// void Adafruit_TMP117::_read(void) {

// }

/**************************************************************************/
/*!
    @brief  Gets the pressure sensor and temperature values as sensor events

    @param  temp Sensor event object that will be populated with temp data
    @returns True
*/
/**************************************************************************/
bool Adafruit_TMP117::getEvent(sensors_event_t *temp) {
  uint32_t t = millis();

  unscaled_temp = (int16_t)temp_reg->read();

  // use helpers to fill in the events
  memset(temp, 0, sizeof(sensors_event_t));
  temp->version = sizeof(sensors_event_t);
  temp->sensor_id = _sensorid_temp;
  temp->type = SENSOR_TYPE_AMBIENT_TEMPERATURE;
  temp->timestamp = t;
  temp->temperature = (unscaled_temp * TMP117_RESOLUTION);
  return true;
}

/**
 * @brief Sets the polarity of the INT pin.
 *
 * @param active_low Set to true to make the pin active low
 */
void Adafruit_TMP117::interruptsActiveLow(bool active_low) {

  Adafruit_BusIO_RegisterBits active_low_bit =
      Adafruit_BusIO_RegisterBits(config_reg, 1, 3);
  active_low_bit.write(active_low);
}

float Adafruit_TMP117::getOffset(void) {
  Adafruit_BusIO_Register temp_offset_reg =
      Adafruit_BusIO_Register(i2c_dev, TMP117_TEMP_OFFSET, 2, MSBFIRST);
  return temp_offset_reg.read() * TMP117_RESOLUTION;
}

bool Adafruit_TMP117::setOffset(float offset) {
  if ((offset > 256.0) || (offset < -256.0)) {
    return false;
  }
  Adafruit_BusIO_Register temp_offset_reg =
      Adafruit_BusIO_Register(i2c_dev, TMP117_TEMP_OFFSET, 2, MSBFIRST);
  int16_t offset_lsb = (int16_t)round(offset / TMP117_RESOLUTION);

  bool success = temp_offset_reg.write(offset_lsb);
  if (success)
    waitForData();
  return success;
}
