#ifndef _I2C_h
#define _I2C_h

#include <Arduino.h>

#define I2C_ADDRESS 8
#define STATUS_LED_PIN SCK
#define GOT_DATA_PIN 2

struct I2Cframe {
	float data;
	char dataType;
};

static volatile bool i2cDataIsSent;
static volatile I2Cframe i2cFrame;


class I2C {

protected:
	static void I2C_RequestEvent();
	static void hexDebug( const void * var, const uint8_t varSize, const char * msg );

public:
	void setup();
	void send( const char dataType, float data );
};

#endif