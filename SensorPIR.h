#ifndef _SENSORPIR_h
#define _SENSORPIR_h

#define PIR_SENSOR_PIN 2

#include <Arduino.h>

// ISR stuff has to be global.
static volatile unsigned long pirHighDuration = 0;
static volatile uint16_t pirMotionCount = 0;
static volatile unsigned long pirGoingHighAt = millis();
static volatile bool pirLastPinValue = false;



class SensorPIR
{
	protected:
		bool isSetup = false;

		unsigned long meassurmentStartedAt = 0;
		float meassurementTime;

		static void pirHandleIsr();

	public:
		void setup();
		void handle();
		void startReading();
		void stopReading();
		float getMotionPtc();
};


#endif