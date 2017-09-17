#ifndef _SENSORDHT_h
#define _SENSORDHT_h

#include "Timing.h"

#define DHT_PIN 4


class SensorDHT
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

