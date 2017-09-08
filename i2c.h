#ifndef _I2C_h
#define _I2C_h

#include <Arduino.h>

#define I2C_ADDRESS 8
#define LED_PIN SCK

static volatile char I2C_RecievedQuestion; // ISR stuff has to be global

class I2CClass {
	protected:
		static void I2C_ReceiveEvent( int howMany );
		static void I2C_RequestEvent();

	public:
		void setup();
		void sendData( const void* var, const uint8_t size );
};

#endif