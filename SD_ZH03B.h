/**
# SD_ZH03B Library for Winsen ZH03B Laser Dust Sensor Module

by **ShaggyDog18@gmail.com**, JULY 2020

github: https://github.com/ShaggyDog18/SD_ZH03B

Library for a **ZH03B Laser Dust Sensor Module** by [Zhengzhou Winsen Electronics Technology Co., Ltd](www.winsen-sensor.com)

License: [GNU GPLv3](https://choosealicense.com/licenses/gpl-3.0/)

The ZH03B is a Laser Dust sensor module, small size sensor, that uses laser scattering principle to detect the dust particles in air, with good selectivity and stability. It is easy to use, with serial port output & PWM output. The module is software programmable through serial interface.

This library features access to all module features though an abstracted class and methods to manage the module implemented in user-level functionality.

The range of measured PM1.0, PM2.5 and PM10 values are all 0-1000ug/m3.

## Library Methods

- **SD_ZH03B( Stream& serial )** - Class Constructor
- **bool readData(void)** - read data from the module; returns **true** if data are read, verified and valid (validate by calculating checkSum of the data received).
- **void setInitiativeMode(void)** - sets the "Initiative Upload" (IU) operation mode. The module launches in that mode by default and shoot data to the COM port every second. So, no need to set up the mode after the module initialization.
- **void setQandAmode(void)** - sets the Q&A operation mode; Module sends the data on demand.
- **void setMode( const mode_t mode = IU_MODE )** - same as above two methods, can be used interchangable: sets the operation mode by using pre-defined constants IU_MODE and QA_MODE. Can be used interchangibly. 
- **mode_t getMode(void)** - returns current mode.
- **bool sleep(void)** - put the module into a "Dormaint" mode. Dormancy consumption current <20mA. Returns **true** if command is confirmed by the module as successful.
- **bool wakeup(void)** - wake up from a "Dormaint" mode. Working Current <120mA. Returns **true** if command is confirmed by the module as successful.
- **uint16_t getPM1_0(void)** - returns a value of PM1.0 particles concentration in ug/m3 
- **uint16_t getPM2_5(void)** - returns a value of PM2.5 particles concentration in ug/m3 
- **uint16_t getPM10_0(void)** - returns a value of PM10 particles concentration in ug/m3 

For more details on the library use refer to the example that utilizes all library methods.

## Compatibility

The library developed for Arduino UNO, NANO, Pro Mini, ESP8266, etc.

**If you like and use this library please consider making a small donation using [PayPal](https://paypal.me/shaggyDog18/3USD)**
 */

#pragma once
#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif


class SD_ZH03B {
public:

enum mode_t : uint8_t {
  IU_MODE = 0, // Initiative Uploading (IU) Mode: shoot data to com port every second
  QA_MODE      // Q and A (QA) Mode: provides data on demand
};

enum type_t : uint8_t {
	SENSOR_ZH03B,
	SENSOR_ZH06
};
/**
* @brief Constructor for SD_ZH03B class
* @param  a Stream ({Software/Hardware}Serial) object.
* @note The serial stream should be already initialized
* @return  void
*/
  SD_ZH03B( Stream& serial, type_t _sensorModel = SENSOR_ZH03B );

/* 
  Class Destructur
*/
  ~SD_ZH03B();

/* 
  Sets the "Q&A" (QA) operstion mode. 
  The module launches in that mode by default and shoot data to the COM port every second. 
*/
  void setQandAmode(void);

/* 
  Sets the "Initiative Upload" (IU) operstion mode. 
  The module launches in that mode by default and shoot data to the COM port every second. 
*/
  void setInitiativeMode(void);

/*
  Sets the mode by defining it by a parameter IU_MODE and QA_MODE. 
  Can be used interchangibly with above setMode functions. 
*/
  void setMode( const mode_t mode = IU_MODE );

/*
  Function returns current mode.
*/
  mode_t getMode(void) const {
    return _currentMode;
  }

/*
  Puts the module into a "Dormaint" mode. Dormancy consumption current <20mA. 
  Returns true if command is confirmed by the module as successful.
*/
  bool sleep(void); 

/*
  Wakes up the module from a "Dormaint" mode. Working Current <120mA. 
  Returns true if command is confirmed by the module as successful.
*/
  bool wakeup(void);

/*
  Reads data from the module; 
  Returns true if data are read, verified and valid (validate by calculating checkSum of the data received).
  After successful data reading the values of PM concentration to be picked up by getPMxxx() functions below
*/
  bool readData(void);

/**
* @brief Returns the latest PM 1.0 reading
* @note  in IU_Mode Sensor reports new reading ~ every 1 sec.
* @return  PM 1.0 reading (unsigned int16)*/
  uint16_t getPM1_0(void) const {
    return _currentMode == IU_MODE ? _unionFrame.ZH03_IUframe.concPM1_0 : _unionFrame.ZHxx_QAframe.concPM1_0;
  }

/**
* @brief Returns the latest PM 2.5 reading
* @note in IU_Mode Sensor reports new reading ~ every 1 sec.
* @return  PM 2.5 reading (unsigned int16)*/
  uint16_t getPM2_5(void) const {
    return _currentMode == IU_MODE ? _unionFrame.ZH03_IUframe.concPM2_5 : _unionFrame.ZHxx_QAframe.concPM2_5;
  }

/**
* @brief Returns the latest PM 10.0 reading
* @note in IU_Mode Sensor reports new reading ~ every 1 sec.
* @return  PM 10.0 reading (unsigned int16)*/
  uint16_t getPM10_0(void) const {
    return _currentMode == IU_MODE ? _unionFrame.ZH03_IUframe.concPM10_0 : _unionFrame.ZHxx_QAframe.concPM10_0;
  }

#define ZH03_SIZEOF_IU_FRAME 24
#define ZH06_SIZEOF_IU_FRAME 32
#define SIZEOF_QA_FRAME 9

private:
  Stream& _serial;

struct ZH03_frameIUstruct_t {  // 24 bytes - SIZEOF_FRAME in Initiative Upload mode
  uint8_t  frameHeader[2];
  uint16_t frameLen;
  uint16_t res1;
  uint16_t res2;
  uint16_t res3;
  uint16_t concPM1_0;
  uint16_t concPM2_5;
  uint16_t concPM10_0;
  uint16_t res4;
  uint16_t res5;
  uint16_t res6;
  uint16_t checksum;
};

struct ZH06_frameIUstruct_t {  // 32 bytes - SIZEOF_FRAME in Initiative Upload mode
  uint8_t  frameHeader[2];
  uint16_t frameLen;
  uint16_t res1;
  uint16_t res2;
  uint16_t res3;
  uint16_t concPM1_0;
  uint16_t concPM2_5;
  uint16_t concPM10_0;
  uint16_t res4;
  uint16_t res5;
  uint16_t res6;
  uint16_t res7;
  uint16_t res8;
  uint16_t res9;
  uint16_t res10;
  uint16_t checksum;
};

struct ZHxx_frameQAstruct_t {  // 9 bytes - SIZEOF_FRAME in Initiative Upload mode
  uint8_t  frameHeader[2];
  uint16_t concPM2_5;
  uint16_t concPM10_0;
  uint16_t concPM1_0;
  uint8_t  checksum;
};

union unionFrame_t {
  ZH03_frameIUstruct_t  ZH03_IUframe;
  ZH06_frameIUstruct_t  ZH06_IUframe;
  ZHxx_frameQAstruct_t  ZHxx_QAframe;
  uint8_t buffer[sizeof(ZH06_frameIUstruct_t)];
} _unionFrame;

  // send command to the module
  void _sendCmd(const uint8_t ch1, const uint8_t ch2, const uint8_t ch3);

  // get confirmation of the previously sent command
  bool _getCmdConfirmation(void);

  mode_t _currentMode = IU_MODE;
  type_t _sensorModel = SENSOR_ZH03B;

  uint8_t _sizeFrame = ZH03_SIZEOF_IU_FRAME;
};