#ifndef _MICROPHONE_h
#define _MICROPHONE_h

#include <Arduino.h>
#include "Timing.h"

#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit)) // low level clear bit
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit)) // low level set bit

#define HIGHEST_AMPLITUDE (1024 / 2)


class MicrophoneClass
{
	protected:
		bool isSetup = false;
		Timing sampleTimer;

		long lastMeasured;
		uint32_t samples = 0;
		uint32_t samplingStarted = 0;
		uint16_t soundVolMax = 0;
		uint32_t soundVolAcc = 0;
		uint64_t soundVolRMS = 0;

		void sampleOnce();

	public:
		void setup( uint16_t sampleTime );
		void handle();
		void sendData();
};

#endif