#ifndef _DHTSENSOR_h
#define _DHTSENSOR_h

#include "Timing.h"

#define DHT_PIN 4


class DHTSensor
{
	 protected:
		 bool isSetup = false;
		 Timing sendTimer;

		 void sendData();

	 public:
		 void setup( uint16_t measureFreq );
		 void handle();
};

#endif

