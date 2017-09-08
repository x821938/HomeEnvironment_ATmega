#ifndef _MICROPHONE_h
#define _MICROPHONE_h

#include <Arduino.h>

#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit)) // low level clear bit
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit)) // low level set bit

#define HIGHEST_AMPLITUDE (1024 / 2)

struct MicData {
	uint32_t samples; // How many samples were taken since last poll of i2c microphone
	uint32_t sampleTime; // How long time the sampling has been going on
	float volAvgPtc; // Average volume in percent
	float volMaxPtc; // Max volume detected in sampling period in percent
	float volRmsPtc; // RMS meassured in sampling period in percent
};

class MicrophoneClass
{
	protected:
		uint32_t samples = 0;
		uint32_t samplingStarted = 0;
		uint16_t soundVolMax = 0;
		uint32_t soundVolAcc = 0;
		uint64_t soundVolRMS = 0;

	public:
		void setup();
		void update();
		MicData getData();
};

#endif