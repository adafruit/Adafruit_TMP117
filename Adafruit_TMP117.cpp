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

  reset();
  // do any software reset or other initial setup
  config_reg =
      new Adafruit_BusIO_Register(i2c_dev, TMP117_CONFIGURATION, 2, MSBFIRST);

  temp_reg =
      new Adafruit_BusIO_Register(i2c_dev, TMP117_TEMP_DATA, 2, MSBFIRST);
  // Set to highest rate
  // setDataRate(TMP117_RATE_25_HZ);

  return true;
}

/**
 * @brief Performs a software reset initializing registers to their power on
 * state
 *
 */
void Adafruit_TMP117::reset(void) {
  // _soft_reset = RWBit(_CONFIGURATION, 1, 2, False)

  // def __init__(self, i2c_bus, address=_I2C_ADDR):

  //     self.i2c_device = i2c_device.I2CDevice(i2c_bus, address)
  //     if self._part_id != _DEVICE_ID_VALUE:
  //         raise AttributeError("Cannot find a TMP117")
  //     # currently set when `alert_status` is read, but not exposed
  //     self.reset()
  //     self.initialize()

  // def reset(self):
  //     """Reset the sensor to its unconfigured power-on state"""
  //     self._soft_reset = True

  // def initialize(self):
  //     """Configure the sensor with sensible defaults. `initialize` is
  //     primarily provided to be called after `reset`, however it can also be
  //     used to easily set the sensor to a known configuration""" # Datasheet
  //     specifies that reset will finish in 2ms however by default the first #
  //     conversion will be averaged 8x and take 1s # TODO: sleep depending on
  //     current averaging config
  //     self._set_mode_and_wait_for_measurement(_CONTINUOUS_CONVERSION_MODE)  #
  //     one shot time.sleep(1)

  // Adafruit_BusIO_RegisterBits sw_reset =
  //     Adafruit_BusIO_RegisterBits(config_reg, 1, 2);

  // sw_reset.write(1);

  //     """Configure the sensor with sensible defaults. `initialize` is
  //     primarily provided to be
  //     called after `reset`, however it can also be used to easily set the
  //     sensor to a known configuration""" # Datasheet specifies that reset
  //     will finish in 2ms however by default the first # conversion will be
  //     averaged 8x and take 1s # TODO: sleep depending on current averaging
  //     config
  //     self._set_mode_and_wait_for_measurement(_CONTINUOUS_CONVERSION_MODE)  #
  //     one shot time.sleep(1)
  // while (sw_reset.read()) {
  //   delay(1);
  // }
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
