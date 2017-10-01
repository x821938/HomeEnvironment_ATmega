#ifndef _I2C_h
#define _I2C_h

#include <Arduino.h>

#define I2C_ADDRESS 8	// Our I2C slave eaddress
static volatile char I2C_RecievedQuestion; // ISR stuff has to be global



class I2C {
	protected:
		static void I2C_ReceiveEvent( int howMany );
		static void I2C_RequestEvent();

	public:
		void setup();
		static void sendData( const void* var, const uint8_t size );
};


#endif