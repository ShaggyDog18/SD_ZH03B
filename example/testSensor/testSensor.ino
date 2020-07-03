#include <SD_ZH03B.h>
#include <SoftwareSerial.h>

//Create an instance of software serial

// Arduino UNO, Nano, Pro Mini, etc 
//SoftwareSerial ZHSerial(4, 5); // RX, TX

// ESP8266
SoftwareSerial ZHSerial(D1, D2); // RX, TX
SD_ZH03B ZH03B( ZHSerial );


void setup() {
    Serial.begin(9600);
    Serial.println();
    delay(1000);
    Serial.println("-- Initializing ZH03B...");
	  //Serial.print("sizeof frame(should be 24bytes): "); Serial.println( sizeof(union_t) );

    ZHSerial.begin(9600);
    delay(100);
    ZH03B.setMode( SD_ZH03B::IU_MODE );
    Serial.println("-- Reading ZH03B --");
    delay(200);
    
}


void readSensorData() {
char printbuf1[80];
   
  if( ZH03B.readData() ) {
    Serial.print( ZH03B.getMode() == SD_ZH03B::IU_MODE ? "IU:" : "Q&A:" );  
    sprintf(printbuf1, "PM1.0, PM2.5, PM10=[%d %d %d]", ZH03B.getPM1_0(), ZH03B.getPM2_5(), ZH03B.getPM10_0() );
    Serial.println(printbuf1);
  } else {   
    Serial.println( "ZH03B Error reading stream or Check Sum Error" );
  } 
}


void loop () {
  
  Serial.println( "Normal default IU Mode. reading every 5 sec" );
  for( int i=0; i<4; i++ ){
    readSensorData();
    delay(5000);
  }
 
  Serial.println( "Go to Sleep" );
  if( ZH03B.sleep() ) Serial.println( "Sleep mode confirmed" );
  
  Serial.println( "Sleep mode, should return errors" );
  for( int i=0; i<4; i++ ) {  
    readSensorData();
    delay(5000);
  }

  Serial.println( "Wake up" );
  if( ZH03B.wakeup() ) Serial.println( "Woke up successfully" );
  for( int i=0; i<4; i++ ){
    readSensorData();
    delay(5000);
  }

  Serial.println( "Set Q&A mode, readings every 5 sec" );
  ZH03B.setMode( SD_ZH03B::QA_MODE );
  delay(100);
  for( int i=0; i<4; i++ ) {
    readSensorData();
    delay(5000);
  }
  
  Serial.println( "Go to Sleep" );
  if( ZH03B.sleep() ) Serial.println( "Sleep mode confirmed" );
  
  Serial.println( "Sleep mode, should return errors" );
  for( int i=0; i<4; i++ ) {  
    readSensorData();
    delay(5000);
  }

  Serial.println( "Wake up" );
  if( ZH03B.wakeup() ) Serial.println( "Woke up successfully" );
  for( int i=0; i<4; i++ ){
    readSensorData();
    delay(5000);
  }
  
  Serial.println( "Set IU mode, readings every 5 sec" );
  //ZH03B.setMode( SD_ZH03B::IU_MODE );
  ZH03B.setInitiativeMode(); // provides the same result as command above
  delay(200);
}
