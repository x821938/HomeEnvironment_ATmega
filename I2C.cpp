#include "i2c.h"
#include "Logging.h"
#include <Wire.h>



void I2C::setup() {
	pinMode( STATUS_LED_PIN, OUTPUT );
	digitalWrite( STATUS_LED_PIN, LOW );

	pinMode( GOT_DATA_PIN, OUTPUT );
	digitalWrite( GOT_DATA_PIN, LOW );

	Wire.begin( I2C_ADDRESS );
	Wire.onRequest( this->I2C_RequestEvent ); // data request from master to slave(me). He wants data
	LOG_INFO( "I2C", "Now listening for bus events" );
}


void I2C::send( const char dataType, float data ) {
	i2cFrame.dataType = dataType;
	i2cFrame.data = data;
	i2cDataIsSent = false;
	LOG_DEBUG( "I2C", "Waiting for master to pull our data" );
	digitalWrite( STATUS_LED_PIN, HIGH );
	digitalWrite( GOT_DATA_PIN, HIGH ); // Tell the esp that we want to send data
	while ( !i2cDataIsSent ); // Wait until master as eaten our data.
	digitalWrite( STATUS_LED_PIN, LOW );
}


void I2C::I2C_RequestEvent() {
	
	digitalWrite( GOT_DATA_PIN, LOW ); // Now esp is reading our data, we lower the flag.

	Wire.write( (uint8_t*) &i2cFrame, sizeof( i2cFrame ) );
	i2cDataIsSent = true;

	hexDebug( &i2cFrame, sizeof( i2cFrame ), "Sent" );
}


void I2C::hexDebug( const void* var, const uint8_t varSize, const char* msg ) {
	String hexString;
	for ( uint8_t i = 0; i < varSize; i++ ) { // traverse the var byte by byte
		char* p = (char *) var;

		uint8_t currentByte = *( p + i ); // get byte number i
		char currentByteHex[3];
		sprintf( currentByteHex, "%02X", currentByte ); // convert it to hex
		hexString = hexString + currentByteHex; // and concatenate it into a printable string of all bytes

		if ( i != varSize - 1 ) hexString = hexString + ",";
	}
	LOG_DEBUG( "I2C", msg << " " << varSize << " bytes in hex: " << hexString );
}