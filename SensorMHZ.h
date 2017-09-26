#ifndef _SENSORMHZ_h
#define _SENSORMHZ_h

#include "Timing.h"

#define MHZ_WARMUP_TIME 180


class SensorMHZ
{
 protected:
	 bool isSetup = false;
	 bool warmedUp = false;

	 Timing sendTimer;
	 Timing warmupTimer;

	 unsigned long sensorStartedAt;
	 void sendData();

 public:
	 void setup( uint16_t measureFreq );
	 void handle();
};

#endif

