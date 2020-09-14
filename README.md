# SD_ZH03B Library for Winsen ZH03B Laser Dust Sensor Module

by **ShaggyDog18@gmail.com**, JULY 2020

github: https://github.com/ShaggyDog18/SD_ZH03B

Arduino library for a **ZH03B Laser Dust Sensor Module** by [Zhengzhou Winsen Electronics Technology Co., Ltd](www.winsen-sensor.com)

License: [GNU GPLv3](https://choosealicense.com/licenses/gpl-3.0/)

The ZH03B is a Laser Dust sensor module, small size sensor, that uses laser scattering principle to detect the dust particles in air, with good selectivity and stability. It is easy to use, with serial port output & PWM output. The module is software programmable through serial interface.

This library features access to all module features though an abstracted class and methods to manage the module implemented in user-level functionality.

The range of measured **PM1.0, PM2.5 and PM10** values are all **0-1000ug/m3**.

## Library Methods

- `SD_ZH03B( Stream& serial )` - Class Constructor
- `bool readData(void)` - read data from the module; returns `true` if data are read, verified and valid (validate by calculating checkSum of the data received).
- `void setInitiativeMode(void)` - sets the "Initiative Upload" (IU) operation mode. The module launches in that mode by default and shoot data to the COM port every second. So, no need to set up the mode after the module initialization.
- `void setQandAmode(void)` - sets the Q&A operation mode; Module sends the data on demand.
- `void setMode( const mode_t mode = IU_MODE )` - same as above two methods, can be used interchangable: sets the operation mode by using pre-defined constants IU_MODE and QA_MODE. Can be used interchangibly. 
- `mode_t getMode(void)` - returns current mode.
- `bool sleep(void)` - put the module into a "Dormaint" mode. Dormancy consumption current <20mA. Returns `true` if command is confirmed by the module as successful.
- `bool wakeup(void)` - wake up from a "Dormaint" mode. Working Current <120mA. Returns `true` if command is confirmed by the module as successful.
- `uint16_t getPM1_0(void)` - returns a value of `PM1.0` particles concentration in ug/m3 
- `uint16_t getPM2_5(void)` - returns a value of `PM2.5` particles concentration in ug/m3 
- `uint16_t getPM10_0(void)` - returns a value of `PM10` particles concentration in ug/m3 

For more details on the library use refer to the example that utilizes all library methods.

## Compatibility

The library developed for Arduino UNO, NANO, Pro Mini, ESP8266, etc.

## Gratitude

**If you use and like this library, please, consider making a small "cup of coffee" donation using [PayPal](https://paypal.me/shaggyDog18/3USD)**

Enjoy!
