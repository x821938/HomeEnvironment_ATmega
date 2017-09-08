#include "i2c.h"
#include "Logging.h"
#include "Microphone.h"
#include "DustSensor.h"
#include "DHTSensor.h"
#include <Wire.h>

extern MicrophoneClass microphone;
extern DHTSensorClass dhtSensor;
extern DustSensorClass dust;
extern I2CClass i2c;

void I2CClass::setup() {
	pinMode( LED_PIN, OUTPUT );
	digitalWrite( LED_PIN, LOW );

	Wire.begin( I2C_ADDRESS );
	Wire.onRequest( this->I2C_RequestEvent ); // data request from master to slave(me). He wants data
	Wire.onReceive( this->I2C_ReceiveEvent );// data from master to slave(me). 
	LOG_INFO( "I2C", "Now listening for bus events" );
}

void I2CClass::I2C_ReceiveEvent( int howMany ) {
	// remember the incoming command from master. eg what to return
	while ( 0 < Wire.available() ) {
		byte rx = Wire.read();
		I2C_RecievedQuestion = rx;
		LOG_DEBUG( "I2C", "Recieved command: " << (char)rx );
	}
}

void I2CClass::I2C_RequestEvent() {
	// respond to the stored question
	digitalWrite( LED_PIN, HIGH );
	switch ( I2C_RecievedQuestion ) {
		case 'M': // Get microphone data and send them on I2C bus
			MicData md;
			md = microphone.getData();
			i2c.sendData( &md, sizeof( md ) );
			break;
		case 'D': // Get dust data and send them on I2C bus
			float dustConcentration;
			dustConcentration = dust.getConcentration();
			i2c.sendData( &dustConcentration, sizeof( dustConcentration ) );
			break;
		case 'H': // Get humidity data from DHT sensor and send them on I2C bus
			DHTSensorData dhtSensorData;
			dhtSensorData = dhtSensor.getData();
			i2c.sendData( &dhtSensorData, sizeof( dhtSensorData ) );
			break;
		default:
			LOG_ERROR( "I2C", "Wrong command" );
			break;
	}
	digitalWrite( LED_PIN, LOW );
}

void I2CClass::sendData( const void* var, const uint8_t size ) {
	String hexString;

	for ( uint8_t i = 0; i < size; i++ ) {
		char* p = (char *) var;

		uint8_t currentByte = *( p + i );
		char currentByteHex[3];
		sprintf( currentByteHex, "%02X", currentByte );
		hexString = hexString + currentByteHex;

		if ( i != size - 1 ) hexString = hexString + ",";
	}
	LOG_DEBUG( "I2C", "Sending bytes : " << hexString );
	Wire.write( (uint8_t*) var, size );
}