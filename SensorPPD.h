#ifndef _SENSORPPD_h
#define _SENSORPPD_h

#include "Timing.h"

#define PPD_SENSOR_PIN 3


// ISR stuff has to be global.
static volatile unsigned long ppdLowDuration=0; 
static volatile unsigned long ppdGoingLowAt = millis();
static volatile bool ppdLastPinValue = true;



class SensorPPD {
	protected:
		bool isSetup = false;

		float concentration;

		unsigned long sampleTime;
		unsigned long samplingStartedAt;

		static void particleCountIsr();

	public:
		void setup();
		void startReading();
		void stopReading();
		float getConcentration();
};


#endif

