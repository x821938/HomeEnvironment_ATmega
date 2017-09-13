#ifndef _DUSTSENSOR_h
#define _DUSTSENSOR_h

#include "Timing.h"

#define SENSOR_PIN 3


static volatile long lowDuration=0; // ISR stuff has to be global.

class PPDSensor {
	protected:
		bool isSetup = false;
		Timing sendTimer;

		long sampleTime;
		long samplingStartedAt;

		static void particleCountIsr();
		void sendData();

	public:
		void setup( uint16_t measureFreq );
		void handle();
};

#endif

