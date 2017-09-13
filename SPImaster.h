#ifndef _SPIMASTER_h
#define _SPIMASTER_h

#include <Arduino.h>

#define LED_PIN SCK


class SPImaster
{
 protected:
	 void pulseSS();
	 void debug( const void * var, const uint8_t varSize, const char * msg );

 public:
	SPImaster();
	void send( const char msgType, void * data, uint8_t size );

};

#endif

