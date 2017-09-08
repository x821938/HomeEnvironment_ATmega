#ifndef _DUSTSENSOR_h
#define _DUSTSENSOR_h

#define SENSOR_PIN 3

static volatile long lowDuration=0; // ISR stuff has to be global.

class DustSensorClass {
	protected:
		static void particleCountIsr();

	public:
		void setup();
		float getConcentration();
};

#endif

