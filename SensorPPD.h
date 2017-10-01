#ifndef _SENSORPPD_h
#define _SENSORPPD_h

#include "Timing.h"

#define PPD_SENSOR_PIN 3

static volatile unsigned long ppdLowDuration=0; // ISR stuff has to be global.



class SensorPPD {
	protected:
		bool isSetup = false;

		bool prevCollectData = false;
		float concentration;

		unsigned long sampleTime;
		unsigned long samplingStartedAt;

		void startReading();
		void stopReading();
		static void particleCountIsr();

	public:
		void setup();
		void handle();
		float getConcentration();
};


#endif

