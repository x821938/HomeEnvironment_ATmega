#ifndef _SENSORPPD_h
#define _SENSORPPD_h

#include "Timing.h"

#define SENSOR_PIN 3


static volatile long lowDuration=0; // ISR stuff has to be global.

class SensorPPD {
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

