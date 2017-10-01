#ifndef _SENSORPIR_h
#define _SENSORPIR_h

#define PIR_SENSOR_PIN 9

#include <Arduino.h>



class SensorPIR
{
	protected:
		bool isSetup = false;
		bool prevCollectData = false;

		unsigned long meassurmentStartedAt = 0;
		unsigned long pirHighDuration = 0;
		uint16_t pirMotionCount = 0; 

		void resetStats();

	public:
		void setup();
		void handle();
		float getMotionPtc();
};


#endif