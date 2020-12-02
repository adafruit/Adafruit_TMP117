/*!
 *  @file Adafruit_TMP117.cpp
 *
 *  @mainpage Adafruit  Arduino library for the TMP117 high-accuracy temperature
 sensor
 *
 *  @section intro_sec Introduction
 *
 * 	I2C Driver for the Library for the TMP117 High-Accuracy Temperature
 * Sensor
 *
 * This is a library for the Adafruit TMP117 breakout:
 * https://www.adafruit.com/product/4821
 *
 * Adafruit invests time and resources providing this open source code.
 * Please support Adafruit and open-source hardware by purchasing products from
 * Adafruit!
 *
 *  @section dependencies Dependencies
 *  This library depends on the Adafruit BusIO library, the Adafruit Unified
 Sensor library.
 *
 *  Integrated tests and examples for this library depend on the Adafruit
 SSD1306 library
 *
 *  @section author Author
 *
 *  Bryan Siepert for Adafruit Industries
 *
 * 	@section license License
 *
 * 	BSD (see license.txt)

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

/**************************************************************************/
/*!
    @brief  Gets the pressure sensor and temperature values as sensor events

    @param  temp Sensor event object that will be populated with temp data
    @returns True
*/
/**************************************************************************/
bool Adafruit_TMP117::getEvent(sensors_event_t *temp) {
  uint32_t t = millis();

  readAlertsDRDY();

  // Temp reg will report old value until new value is ready; "clears" on new
  // data ready
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
 * @brief Get the current state of the alert flags
 *
 * **NOTE:** Because the high/low temperature status is based on temperature
 * data, their status returned by this method is only updated when new
 * temperature data is available. This ensures that the reported value is based
 * on temperature data and not a cleared but not updated alert status.
 *
 * @param alerts Pointer to an alerts struct to be filled with the trigger
 * status of the alerts
 * @return true:success false: failure
 */
bool Adafruit_TMP117::getAlerts(tmp117_alerts_t *alerts) {
  readAlertsDRDY();
  memset(alerts, 0, sizeof(tmp117_alerts_t));
  alerts->high = alert_drdy_flags.high;
  alerts->low = alert_drdy_flags.low;
  alerts->data_ready = alert_drdy_flags.data_ready;

  return true;
}

/**
 * @brief Read the current low temperature threshold
 *
 * @return float The current low temperature threshold in degrees C
 */
float Adafruit_TMP117::getLowThreshold(void) {
  Adafruit_BusIO_Register low_threshold_reg =
      Adafruit_BusIO_Register(i2c_dev, TMP117_T_LOW_LIMIT, 2, MSBFIRST);

  return (int16_t)low_threshold_reg.read() * TMP117_RESOLUTION;
}

/**
 * @brief Set a new low temperature threshold
 *
 * @param low_threshold The new threshold to be set, in degrees C. An alert will
 * trigger when the current temperature measurement is lower than the given
 * threshold.
 * @return true:success false: failure
 */
bool Adafruit_TMP117::setLowThreshold(float low_threshold) {
  Adafruit_BusIO_Register low_threshold_reg =
      Adafruit_BusIO_Register(i2c_dev, TMP117_T_LOW_LIMIT, 2, MSBFIRST);

  return low_threshold_reg.write(low_threshold / TMP117_RESOLUTION);
}

/**
 * @brief Read the current high temperature threshold
 *
 * @return float The  current high temperature threshold in degrees C
 */
float Adafruit_TMP117::getHighThreshold(void) {
  Adafruit_BusIO_Register high_threshold_reg =
      Adafruit_BusIO_Register(i2c_dev, TMP117_T_HIGH_LIMIT, 2, MSBFIRST);

  return (int16_t)high_threshold_reg.read() * TMP117_RESOLUTION;
}

/**
 * @brief Set a new high temperature threshold
 *
 * @param high_threshold The new threshold to be set, in degrees C. An alert
 * will trigger when the current temperature measurement is higher than the
 * given threshold.
 * @return true:success false: failure
 */
bool Adafruit_TMP117::setHighThreshold(float high_threshold) {
  Adafruit_BusIO_Register high_threshold_reg =
      Adafruit_BusIO_Register(i2c_dev, TMP117_T_HIGH_LIMIT, 2, MSBFIRST);

  return high_threshold_reg.write(high_threshold / TMP117_RESOLUTION);
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

/**
 * @brief Get the polarity of the INT pin.
 *
 * @return active_low
 *
 * true: INT pin is active when low
 * false: INT pin is active when high
 */
bool Adafruit_TMP117::interruptsActiveLow(void) {

  Adafruit_BusIO_RegisterBits active_low_bit =
      Adafruit_BusIO_RegisterBits(config_reg, 1, 3);
  return active_low_bit.read();
}
/**
 * @brief Read the current temperature offset
 *
 * @return float The currently set temperature offset.
 */
float Adafruit_TMP117::getOffset(void) {
  Adafruit_BusIO_Register temp_offset_reg =
      Adafruit_BusIO_Register(i2c_dev, TMP117_TEMP_OFFSET, 2, MSBFIRST);
  return temp_offset_reg.read() * TMP117_RESOLUTION;
}

/**
 * @brief Write a new temperature offset.
 *
 * @param offset The new temperature offset in degrees C. When set, the given
 * offset will be added to all future temperature reads reported by `getEvent`
 * @return true: success false: failure
 */
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

/**
 * @brief Enable or disable "THERM" alert mode
 *
 *
 * WHen enabled, the 'low' alert will never trigger and acts in combination with
 * the high threshold to determine the behavior of the high temperature alert.
 *
 * In "Therm" mode, the "high" temperature alert statys triggered until the
 * measured temperature goes below the 'low' temperature threshold, allowing it
 * to act like a hysteresis value to prevent thrashing around the threshold
 * temperature.
 *
 * @param therm_enabled
 * @return true:success false:failure
 */
bool Adafruit_TMP117::thermAlertModeEnabled(bool therm_enabled) {
  Adafruit_BusIO_RegisterBits therm_enable_bit =
      Adafruit_BusIO_RegisterBits(config_reg, 1, 4);
  return therm_enable_bit.write(therm_enabled);
}

/**
 * @brief Get the current enable status of the "THERM" alert mode
 *
 * @return true: Therm mode enabled
 * @return false Normal high/low alert mode enabled
 */
bool Adafruit_TMP117::thermAlertModeEnabled(void) {
  Adafruit_BusIO_RegisterBits therm_enable_bit =
      Adafruit_BusIO_RegisterBits(config_reg, 1, 4);
  return therm_enable_bit.read();
}

/**
 * @brief Read the current number of samples that are averaged to calculate the
 * reported temperature
 *
 * @return tmp117_average_count_t The current average setting enum value
 */
tmp117_average_count_t Adafruit_TMP117::getAveragedSampleCount(void) {
  Adafruit_BusIO_RegisterBits average_count_bits =
      Adafruit_BusIO_RegisterBits(config_reg, 2, 5);
  return (tmp117_average_count_t)average_count_bits.read();
}

/**
 * @brief Set the number of raw measurements that are averaged into the reported
 * temperature.
 *
 * Each sample read takes 15.5ms so the higher the number of averaged samples,
 * the longer the amount of time between new measurements. For larger average
 * counts the amount of time required for a new measurement will exceed the
 * interval specified by `setReadDelay`
 *
 * @param count The `tmp117_average_count_t` that specifies the desired sample
 * count
 * @return true:success false:failure
 */
bool Adafruit_TMP117::setAveragedSampleCount(tmp117_average_count_t count) {
  Adafruit_BusIO_RegisterBits average_count_bits =
      Adafruit_BusIO_RegisterBits(config_reg, 2, 5);
  return average_count_bits.write(count);
}
/**
 * @brief Get current setting for the minimum delay between calculated
 * temperature reads.
 *
 * @return tmp117_delay_t The minimum time between new temperature measurements.
 * This amount of time will be exceeded if the time required for the configured
 * number of averaged reads is more than the delay setting.
 */
tmp117_delay_t Adafruit_TMP117::getReadDelay(void) {
  Adafruit_BusIO_RegisterBits read_delay_bits =
      Adafruit_BusIO_RegisterBits(config_reg, 3, 7);
  return (tmp117_delay_t)read_delay_bits.read();
}
/**
 * @brief Set a new minimum delay between calculated reads
 *
 * @param delay The minimum time between new temperature `measurements as a
 * tmp117_delay_t`. This amount of time will be exceeded if the time required
 * for the configured number of averaged reads is more than the delay setting.
 * @return true:success false:failure
 */
bool Adafruit_TMP117::setReadDelay(tmp117_delay_t delay) {
  Adafruit_BusIO_RegisterBits read_delay_bits =
      Adafruit_BusIO_RegisterBits(config_reg, 3, 7);
  return read_delay_bits.write(delay);
}

/**
 * @brief Read the active measurement mode
 *
 * @return tmp117_mode_t The current measurement mode.
 */
tmp117_mode_t Adafruit_TMP117::getMeasurementMode(void) {
  Adafruit_BusIO_RegisterBits mode_bits =
      Adafruit_BusIO_RegisterBits(config_reg, 2, 10);
  return (tmp117_mode_t)mode_bits.read();
}

/**
 * @brief Set a new measurement mode
 *
 * @param mode the new mode to set, a `tmp117_mode_t`. If `mode` is
 * `TMP117_MODE_ONE_SHOT`, initiate a new reading before switching to
 * `TMP117_MODE_SHUTDOWN`.
 * @return true:success false:failure
 */
bool Adafruit_TMP117::setMeasurementMode(tmp117_mode_t mode) {
  Adafruit_BusIO_RegisterBits mode_bits =
      Adafruit_BusIO_RegisterBits(config_reg, 2, 10);
  return mode_bits.write(mode);
}
///////////////////  Misc private methods //////////////////////////////
void Adafruit_TMP117::waitForData(void) {
  while (!getDataReady()) {
    delay(1);
  }
}

bool Adafruit_TMP117::getDataReady(void) {
  readAlertsDRDY();
  return alert_drdy_flags.data_ready;
}

void Adafruit_TMP117::readAlertsDRDY(void) {
  Adafruit_BusIO_RegisterBits alert_drdy_bits =
      Adafruit_BusIO_RegisterBits(config_reg, 3, 13);
  uint8_t alert_bits = alert_drdy_bits.read();

  alert_drdy_flags.data_ready = (alert_bits & DRDY_ALRT_FLAG) > 0;

  // drdy means a new read finished, verifies that the alert values
  // match the currently reported temperature
  if (alert_drdy_flags.data_ready) {
    alert_drdy_flags.high = (alert_bits & HIGH_ALRT_FLAG) > 0;
    alert_drdy_flags.low = (alert_bits & LOW_ALRT_FLAG) > 0;
  }
}
