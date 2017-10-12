#include "i2c.h"
#include "Logging.h"
#include "SensorMIC.h"
#include "SensorPPD.h"
#include "SensorDHT.h"
#include "SensorPIR.h"
#include <Wire.h>

extern SensorMIC sensorMIC;
extern SensorDHT sensorDHT;
extern SensorPPD sensorPPD;
extern SensorPIR sensorPIR;

extern I2C i2c;

bool I2C::justBooted = true;
bool I2C::masterReportedFinished = false;



/* Registrers handlser for I2C communication */
void I2C::setup() {
	Wire.begin( I2C_ADDRESS );
	Wire.onRequest( this->I2C_RequestEvent ); // data request from master to slave(me). He wants data
	Wire.onReceive( this->I2C_ReceiveEvent );// data from master to slave(me). 
	LOG_INFO( "I2C", "Now listening for bus events" );
}



/* Reads one incoming byte from master and stores it for later use.
   This is for us to know what to give our beloved master */
void I2C::I2C_ReceiveEvent( int howMany ) {
	while ( 0 < Wire.available() ) {
		byte rx = Wire.read();
		I2C_RecievedQuestion = rx;
		LOG_DEBUG( "I2C", "Recieved command: " << (char) rx );
	}
}



/* Depending on the previously received command we now give the master it's data */
void I2C::I2C_RequestEvent() {
	float sendValue;

	// respond to the stored question
	switch ( I2C_RecievedQuestion ) {
		case 'M':
			sendValue = sensorMIC.getMaxPtc();
			LOG_NOTICE( "I2C", "Sending max volume of " << sendValue << "%" );
			sendData( &sendValue, sizeof( sendValue ) );
			break;
		case 'A': 
			sendValue = sensorMIC.getAvgPtc();
			LOG_NOTICE( "I2C", "Sending avarage volume of " << sendValue << "%" );
			sendData( &sendValue, sizeof( sendValue ) );
			break;
		case 'R':
			sendValue = sensorMIC.getRmsPtc();
			LOG_NOTICE( "I2C", "Sending RMS volume of " << sendValue << "%" );
			sendData( &sendValue, sizeof( sendValue ) );
			break;
		case 'D':
			sendValue = sensorPPD.getConcentration();
			LOG_NOTICE( "I2C", "Sending dust concentration of " << sendValue << "pcs/l" );
			sendData( &sendValue, sizeof( sendValue ) );
			break;
		case 'T':
			sendValue = sensorDHT.getTemperature();
			LOG_NOTICE( "I2C", "Sending temperature of " << sendValue << "C" );
			sendData( &sendValue, sizeof( sendValue ) );
			break;
		case 'H':
			sendValue = sensorDHT.getHumidity();
			LOG_NOTICE( "I2C", "Sending humidity of " << sendValue << "%" );
			sendData( &sendValue, sizeof( sendValue ) );
			break;
		case 'S':
			sendValue = sensorPIR.getMotionPtc();
			LOG_NOTICE( "I2C", "Sending motion time of " << sendValue << "%" );
			sendData( &sendValue, sizeof( sendValue ) );
			break;
		case 'U':
			sendValue = millis() / ( 1000.0 * 60.0 );
			LOG_NOTICE( "I2C", "Sending uptime of " << sendValue << "min" );
			sendData( &sendValue, sizeof( sendValue ) );
			break;
		case 'Z':
			sendValue = 0;
			LOG_NOTICE( "I2C", "Master is finished with us" );
			sendData( &sendValue, sizeof( sendValue ) );
			masterReportedFinished = true;
			break;
		case 'B':
			sendValue = justBooted;
			LOG_NOTICE( "I2C", "Sending just booted = " << sendValue );
			sendData( &sendValue, sizeof( sendValue ) );
			justBooted = false;
			break;
		default:
			LOG_ERROR( "I2C", "Wrong command" );
			break;
	}
}



bool I2C::isMasterFinished() {
	bool finished = masterReportedFinished;
	masterReportedFinished = false;
	return finished;
}



/* Sends a variable via I2C. Most is this method is delicious hex logging */
void I2C::sendData( const void* var, const uint8_t size ) {
	String hexString; // This is the debug hex string we will spit out in the end

	for ( uint8_t i = 0; i < size; i++ ) { // Traverse provided var byte by byte
		char* p = (char *) var;

		uint8_t currentByte = *( p + i );
		char currentByteHex[3];
		sprintf( currentByteHex, "%02X", currentByte ); // And convert it to hex
		hexString = hexString + currentByteHex;

		if ( i != size - 1 ) hexString = hexString + ",";
	}
	LOG_DEBUG( "I2C", "Sending bytes : " << hexString );
	Wire.write( (uint8_t*) var, size );
}