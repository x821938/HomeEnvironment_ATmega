#ifndef _SENSORMIC_h
#define _SENSORMIC_h

#include <Arduino.h>
#include "Timing.h"

#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit)) // low level clear bit
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit)) // low level set bit

#define HIGHEST_AMPLITUDE (1024 / 2)
#define MIC_POWER_PIN A1
#define MIC_SAMPLE_TIME 1000UL



class SensorMIC
{
	protected:
		bool isSetup = false;

		Timing sampleTimer;
		uint32_t samples = 0;
		uint32_t samplingStarted = 0;

		uint16_t soundVolMax = 0;
		uint32_t soundVolAcc = 0;
		uint64_t soundVolRMS = 0;

		float accVolPtc=0;
		float accMaxPtc=0;
		float accRmsPtc=0;
		uint32_t accCount=0;

		void newSample();
		void sampleOnce();

		void accumulateData();

	public:
		void setup();
		void handle();
		void newMeasurement();
		float getAvgPtc();
		float getMaxPtc();
		float getRmsPtc();
};


#endif