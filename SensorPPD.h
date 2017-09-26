#ifndef _SENSORPPD_h
#define _SENSORPPD_h

#include "Timing.h"

#define SENSOR_PIN 3


static volatile unsigned long lowDuration=0; // ISR stuff has to be global.

class SensorPPD {
	protected:
		bool isSetup = false;
		Timing sendTimer;

		unsigned long sampleTime;
		unsigned long samplingStartedAt;

		static void particleCountIsr();
		void sendData();

	public:
		void setup( uint16_t measureFreq );
		void handle();
};

#endif

