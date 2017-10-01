#ifndef _SENSORDHT_h
#define _SENSORDHT_h

#include "Timing.h"

#define DHT_PIN 4
#define DHT_MEASURE_FREQ 15000UL



class SensorDHT
{
	 protected:
		bool isSetup = false;

		Timing meassureTimer;
		bool prevCollectData = false;
		 
		float accTemp;
		float accHum;
		uint16_t accTempCount = 0;
		uint16_t accHumCount = 0;

		void resetAccumulation();
		void accumulateData();

	public:
		void setup();
		void handle();
		float getTemperature();
		float getHumidity();

};


#endif