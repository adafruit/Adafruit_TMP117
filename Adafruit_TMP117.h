/*!
 *  @file Adafruit_TMP117.h
 *
 * 	I2C Driver for the Adafruit  Arduino library for the TMP117
 *high-accuracy temperature sensor This is a library is written to work with the
 *Adafruit TMP117 breakout: https://www.adafruit.com/products/4821
 *
 * 	Adafruit invests time and resources providing this open source code,
 *  please support Adafruit and open-source hardware by purchasing products from
 * 	Adafruit!
 *
 *
 *	BSD license (see license.txt)
 */
// "requires_busio": "y",
//   "requires_sensor": "y",
#ifndef _ADAFRUIT_TMP117_H
#define _ADAFRUIT_TMP117_H

#include "Arduino.h"
#include <Adafruit_BusIO_Register.h>
#include <Adafruit_I2CDevice.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

#define TMP117_I2CADDR_DEFAULT 0x48 ///< TMP117 default i2c address
#define TMP117_CHIP_ID 0x0117       ///< TMP117 default device id from WHOAMI

#define TMP117_WHOAMI 0x0F  ///< Chip ID register
#define _CONFIGURATION 0x01 ///< Configuration register

#define TMP117_TEMP_DATA 0x00     ///< Temperature data register
#define TMP117_CONFIGURATION 0x01 ///< Configuration register
#define TMP117_T_HIGH_LIMIT 0x02  ///< High limit set point register
#define TMP117_T_LOW_LIMIT 0x03   ///<  Low limit set point register
#define TMP117_TEMP_OFFSET 0x07   ///< Temp offset register
#define TMP117_DEVICE_ID 0x0F     ///< Device ID register
#define WHOAMI_ANSWER 0x0117      ///< Correct 2-byte ID register value response

#define HIGH_ALRT_FLAG 0b100 ///< mask to check high threshold alert
#define LOW_ALRT_FLAG 0b010  ///< mask to check low threshold alert
#define DRDY_ALRT_FLAG 0b001 ///< mask to check data ready flag

#define TMP117_RESOLUTION                                                      \
  0.0078125f ///< Scalar to convert from LSB value to degrees C

// #define TMP117_EEPROM_UL 0x04
// #define TMP117_EEPROM1 0x05
// #define TMP117_EEPROM2 0x06
// #define TMP117_EEPROM3 0x08

///////////////////////////////////////////////////////////////

/**
 * @brief
 *
 * Allowed values for `setDataRate`.
 */
typedef enum {
  TMP117_RATE_ONE_SHOT,
} tmp117_rate_t;

/**
 * @brief A struct to hold alert state information.
 *
 * The alert state register is auto-clearing and so must be read together
 *
 */
typedef struct {
  bool high;       ///< Status of the high temperature alert
  bool low;        ///< Status of the low temperature alert
  bool data_ready; ///< Status of the data_ready alert
} tmp117_alerts_t;

/**
 * @brief Options for setAveragedSampleCount
 *
 */
typedef enum {
  TMP117_AVERAGE_1X,
  TMP117_AVERAGE_8X,
  TMP117_AVERAGE_32X,
  TMP117_AVERAGE_64X,
} tmp117_average_count_t;

/**
 * @brief Options to specify the minimum delay between new measurements.
 *
 */
typedef enum {
  TMP117_DELAY_0_MS,
  TMP117_DELAY_125_MS,
  TMP117_DELAY_250_MS,
  TMP117_DELAY_500_MS,
  TMP117_DELAY_1000_MS,
  TMP117_DELAY_4000_MS,
  TMP117_DELAY_8000_MS,
  TMP117_DELAY_16000_MS,
} tmp117_delay_t;

/**
 * @brief Options to set the measurement mode of the sensor
 *
 * In `TMP117_MODE_CONTINUOUS`, new measurements are read and available
 * according to the interval determined by the number of averaged samples and
 * the delay between reads.
 *
 * When the mode is `TMP117_MODE_SHUTDOWN` the sensor is placed in a low power
 * state and new measurements are not taken until a different mode is set. In
 * this mode, active circuitry within this sensor is deactivated, lowering the
 * power consumption dramatically.
 *
 * When the mode is set to `TMP117_MODE_ONE_SHOT`, a single new measurement is
 * calculated from the configured number of samples to be averaged and available
 * as soon as the measurements are Complete.
 *
 * Once the new measurement is calculated and available, the sensor switches to
 * `TMP117_MODE_SHUTDOWN` until `TMP117_MODE_ONE_SHOT` is set again to calculate
 * a new measurement or the mode is switched to `TMP117_MODE_CONTINUOUS`.
 *
 * **NOTE:** This setting ignores the configured minimum delay between
 * measurements.
 *
 */
typedef enum {
  TMP117_MODE_CONTINUOUS,
  TMP117_MODE_SHUTDOWN,
  TMP117_MODE_ONE_SHOT = 3, // skipping 0x2 which is a duplicate CONTINUOUS
} tmp117_mode_t;

/*!
 *    @brief  Class that stores state and functions for interacting with
 *            the TMP117 High-Accuracy Temperature Sensor
 */
class Adafruit_TMP117 {
public:
  Adafruit_TMP117();
  ~Adafruit_TMP117();

  bool begin(uint8_t i2c_addr = TMP117_I2CADDR_DEFAULT, TwoWire *wire = &Wire,
             int32_t sensor_id = 117);
  void reset(void);
  void interruptsActiveLow(bool active_low);
  bool interruptsActiveLow(void);

  bool getEvent(sensors_event_t *temp);
  bool getAlerts(tmp117_alerts_t *alerts);

  bool thermAlertModeEnabled(bool therm_enabled);
  bool thermAlertModeEnabled(void);

  tmp117_average_count_t getAveragedSampleCount(void);
  bool setAveragedSampleCount(tmp117_average_count_t count);

  float getOffset(void);
  bool setOffset(float offset);

  float getLowThreshold(void);
  bool setLowThreshold(float low_threshold);

  float getHighThreshold(void);
  bool setHighThreshold(float high_threshold);

  tmp117_delay_t getReadDelay(void);
  bool setReadDelay(tmp117_delay_t delay);

  tmp117_mode_t getMeasurementMode(void);
  bool setMeasurementMode(tmp117_mode_t mode);

private:
  // void _read(void);
  bool _init(int32_t sensor_id);
  uint16_t _sensorid_temp; ///< ID number for temperature

  tmp117_alerts_t
      alert_drdy_flags; ///< Storage for self-cleared bits in config reg.
  float unscaled_temp;  ///< Last reading's temperature (C) before scaling

  Adafruit_I2CDevice *i2c_dev = NULL; ///< Pointer to I2C bus interface
  Adafruit_BusIO_Register *config_reg =
      NULL; ///< Pointer to config register object
  Adafruit_BusIO_Register *temp_reg =
      NULL; ///< Temperature register, used regularly

  void readAlertsDRDY(void);
  bool getDataReady(void);
  void waitForData(void);
};

#endif