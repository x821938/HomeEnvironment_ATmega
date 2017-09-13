#include <SPI.h>
#include "SPImaster.h"
#include "Logging.h"


SPImaster::SPImaster() {
	SPI.begin();
	SPI.setClockDivider( SPI_CLOCK_DIV128 );
	pinMode( LED_PIN, OUTPUT );
}


void SPImaster::send( const char msgType, void* data, uint8_t msgSize ) {
	digitalWrite( LED_PIN, HIGH );
	
	pulseSS();

	SPI.transfer( 0x02 ); // This tells slave that message is destined for it
	SPI.transfer( 0x00 ); // This tells slave that message is destined for it

	SPI.transfer( msgType ); 
	SPI.transfer( msgSize );

	debug( data, msgSize, "Sending bytes in hex:" );

	uint8_t pos = 0;
	while ( msgSize-- && pos< 30) {
		char *c = (char*) data + (pos++);
		SPI.transfer( *c );
	}
	while ( pos++ < 30 ) {
		SPI.transfer( 0 );
	}

	pulseSS();
	delay( 10 ); // Make sure nothing is sent for a little period so slave gets it all.

	digitalWrite( LED_PIN, LOW );
}


void SPImaster::pulseSS() {
	digitalWrite( SS, HIGH ); 
	delayMicroseconds( 5 ); 
	digitalWrite( SS, LOW );
}

void SPImaster::debug( const void* var, const uint8_t varSize, const char* msg ) {
	String hexString;
	for ( uint8_t i = 0; i < varSize; i++ ) { // traverse the var byte by byte
		char* p = (char *) var;

		uint8_t currentByte = *( p + i ); // get byte number i
		char currentByteHex[3];
		sprintf( currentByteHex, "%02X", currentByte ); // convert it to hex
		hexString = hexString + currentByteHex; // and concatenate it into a printable string of all bytes

		if ( i != varSize - 1 ) hexString = hexString + ",";
	}
	LOG_DEBUG( "SPI", msg << " " << varSize << " bytes in hex: " << hexString );
}