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

#define DRDY_FLAG 0b001 ///< mask to check data ready flag
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
  TMP117_RATE_1_HZ,
  TMP117_RATE_7_HZ,
  TMP117_RATE_12_5_HZ,
  TMP117_RATE_25_HZ,
} tmp117_rate_t;

/*!
 *    @brief  Class that stores state and functions for interacting with
 *            the TMP117 High-Accuract Temperature Sensor
 */
class Adafruit_TMP117 {
public:
  Adafruit_TMP117();
  ~Adafruit_TMP117();

  bool begin(uint8_t i2c_addr = TMP117_I2CADDR_DEFAULT, TwoWire *wire = &Wire,
             int32_t sensor_id = 117);

  void reset(void);
  void interruptsActiveLow(bool active_low);
  bool getDataReady(void);

  tmp117_rate_t getDataRate(void);

  void setDataRate(tmp117_rate_t data_rate);
  bool getEvent(sensors_event_t *temp);

  bool setOffset(float offset);
  float getOffset(void);

private:
  // void _read(void);
  bool _init(int32_t sensor_id);
  uint8_t alert_drdy_flags =
      0;               ///< Storage for self-cleared bits in config reg.
  float unscaled_temp; ///< Last reading's temperature (C) before scaling

  uint16_t _sensorid_temp; ///< ID number for temperature

  Adafruit_I2CDevice *i2c_dev = NULL; ///< Pointer to I2C bus interface
  Adafruit_BusIO_Register *config_reg =
      NULL; ///< Pointer to config register object
  Adafruit_BusIO_Register *temp_reg =
      NULL; ///< Temperature register, used regularly

  void readAlertsDRDY(void);
  void waitForData(void);
};

#endif

// reset(self):
// initialize(self):
// temperature(self):
// temperature_offset(self):
// high_limit(self):
// low_limit(self):
// alert_status(self):
// averaged_measurements(self):
// measurement_mode(self):
// measurement_delay(self):
// take_single_measurememt(self):
// alert_mode(self):
// _set_mode_and_wait_for_measurement(self, mode):
// _read_status(self):
// _read_temperature(self):
