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
- **void setInitiativeMode(void)** - sets the "Initiative Upload" (IU) operstion mode. The module launches in that mode by default and shoot data to the COM port every second. So, no need to set up the mode after the module initialization.
- **void setQandAmode(void)** - sets the Q&A opertion mode; Module sends the data on demand.
- **void setMode( const mode_t mode = IU_MODE )** - same as above two methods, can be used interchangable: sets the operation mode by using pre-defined constants IU_MODE and QA_MODE. Can be used interchangibly. 
- **mode_t getMode()** - returns current mode.
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

#include "Arduino.h"
#include <SD_ZH03B.h>

extern "C" {
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
}

//#define DEBUG


/**
 * @brief Constructor for SD_ZH03B class
 * @param  a Stream ({Software/Hardware}Serial) object.
 * @note The serial stream should be already initialized
 * @return  void
 */
SD_ZH03B::SD_ZH03B(Stream& serial): _serial(serial) {
  _serial.setTimeout(100);
}


bool SD_ZH03B::readData(void) {

  if( _currentMode == QA_MODE ) {  // request data: send a request command
    sendCmd( 0x86, 0x00, 0x79 );
    delay(10);
  }

  // expect data header to fly in
  while( (_serial.peek() != (_currentMode == IU_MODE ? 0x42 : 0xFF)) && _serial.available() ) {
    _serial.read();
  }

  if( _serial.available() < _sizeFrame ) {  //overall 24 bytes or 9 bytes for QA_MODE
    return false;
  }

  // read the entire buffer of 24 or 9 bytes
  _serial.readBytes( _unionFrame.buffer, _sizeFrame );
  _serial.flush();  // Waits for the transmission of outgoing serial data to complete.

  #ifdef DEBUG
    //Serial.println("Initial buffer");
    //for( uint8_t i = 0; i < _sizeFrame; i++ ) {
    //  Serial.print( _unionFrame.buffer[i], HEX); Serial.print( ":");
    //}
    //Serial.println();

    char printbuf[80];
    Serial.print("ZH03B ");
    if( _currentMode == IU_MODE )
      sprintf(printbuf, "IU Header:[%02x %02x] (%0x) ", 
        _unionFrame.ZH03B_IUframe.frameHeader[0], _unionFrame.ZH03B_IUframe.frameHeader[1], _unionFrame.ZH03B_IUframe.frameLen);
    else
      sprintf(printbuf, "Q&A Header[%02x %02x]", 
        _unionFrame.ZH03B_QAframe.frameHeader[0], _unionFrame.ZH03B_QAframe.frameHeader[1] );

    Serial.println(printbuf);
  #endif
  
  // re-sort the buffer: swap high and low bytes since they are not in the "machine" order 
  uint8_t tmp;
  uint8_t nPairsToSwap = (_currentMode == IU_MODE) ? 11 : 3;
  for (uint8_t i = 1; i <= nPairsToSwap; i++) { 
    tmp = _unionFrame.buffer[2*i];
    _unionFrame.buffer[2*i]   = _unionFrame.buffer[2*i+1];
    _unionFrame.buffer[2*i+1] = tmp;
  }

  #ifdef DEBUG
    Serial.println("re-sorted buffer");
    for( uint8_t i = 0; i < _sizeFrame; i++ ) {
      Serial.print( _unionFrame.buffer[i], HEX); Serial.print( ":");
    }
    Serial.println();
  #endif
  
  // calculate checksum : different ways of calculation for each mode
  switch( _currentMode ) {  
  case IU_MODE: {
      uint16_t calcCheckSum = 0; // refer to datasheet for check sum calculation
      for( uint8_t i = 0; i < _sizeFrame-2; i++ ) {
        calcCheckSum += _unionFrame.buffer[i];
      }
      if( calcCheckSum != _unionFrame.ZH03B_IUframe.checksum ) {
        #ifdef DEBUG
          Serial.println( "IU Check sum error" );
        #endif
        return false;
      }
    } 
    break;

  case QA_MODE: {
      uint8_t calcCheckSum = 0; // refer to datasheet for check sum calculation
      for( uint8_t i = 1; i < _sizeFrame-1; i++ ) {
        calcCheckSum += _unionFrame.buffer[i];
      }
      calcCheckSum = (~calcCheckSum)+1; 
      if( calcCheckSum != _unionFrame.ZH03B_QAframe.checksum ) {
        #ifdef DEBUG
          Serial.println( "Q&A Check sum error" );
        #endif
        return false;
      }
    } 
    break;
  }  //  switch( _currentMode )
  return true;
}


 void SD_ZH03B::setMode( const mode_t _mode ) {
  switch( _mode ) {  
  case IU_MODE:
    setInitiativeMode();
    break;

  case QA_MODE:
    setQandAmode();
    break;
  }
}


 SD_ZH03B::mode_t SD_ZH03B::getMode(void) {
  return _currentMode;
}


void SD_ZH03B::setQandAmode(void) {
  _currentMode = QA_MODE;
  _sizeFrame = SIZEOF_QA_FRAME;
  sendCmd( 0x78, 0x41, 0x46 );
}


void SD_ZH03B::setInitiativeMode(void) {  // default mode
  _currentMode = IU_MODE;
  _sizeFrame = SIZEOF_IU_FRAME;
  sendCmd( 0x78, 0x40, 0x47 );
}


bool SD_ZH03B::sleep(void) {
  sendCmd( 0xA7, 0x01, 0x57 );
  return getCmdConfirmation();
}


bool SD_ZH03B::wakeup(void) {
  sendCmd( 0xA7, 0x00, 0x58 );
  return getCmdConfirmation();
}


bool SD_ZH03B::getCmdConfirmation(void) {

  // wait for header to fly in
  while( (_serial.peek() != 0xFF ) && _serial.available() ) {
    _serial.read();
  }
  
  _serial.readBytes( _unionFrame.buffer, 9 );  // read the response to a buffer
  _serial.flush();  // Waits for the transmission of outgoing serial data to complete.

  // check up the received response
  if( _unionFrame.buffer[1] == 0xA7 && _unionFrame.buffer[2] == 0x01 && _unionFrame.buffer[8] == 0x58 )
    return true;

  return false;
}

void SD_ZH03B::sendCmd(const char ch1, const char ch2, const char ch3) {
  _serial.flush(); // clear buffer

  // send command header
  _serial.write(0xFF); 
  _serial.write(0x01);

  // send command
  _serial.write(ch1);    
  _serial.write(ch2);

  // send reserved 4 x zeros
  for( uint8_t i = 0; i<4; i++ ) _serial.write(0x00);
  
  //send command tail/check value
  _serial.write(ch3);
}


/* old non-optomosed function of sending command

void SD_ZH03B::sendCmd(const char * cmdBuf) {
  //Clear RX
  //while (_serial.available()) _serial.read();
  _serial.flush(); 

  // sent command header
  _serial.write(0xFF); 
  _serial.write(0x01);

  // send command
  _serial.write(cmdBuf[0]);    
  _serial.write(cmdBuf[1]);

  // send reserved 4 x zeros
  for( uint8_t i = 0; i<4; i++ ) _serial.write(0x00);
  
  //send command tail/check value
  _serial.write(cmdBuf[2]);
}
*/
