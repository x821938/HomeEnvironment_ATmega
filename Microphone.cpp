#include "Microphone.h"
#include "Logging.h"


void MicrophoneClass::setup() {
	ADCSRA = 0xe0 + 7; // "ADC Enable", "ADC Start Conversion", "ADC Auto Trigger Enable" and divider.
	sbi( ADCSRA, ADPS2 ); cbi( ADCSRA, ADPS1 ); sbi( ADCSRA, ADPS0 ); // my board runs at 8mhz. prescaler 32=19200 samples/sek instead of 128=4800 samples/sek.
	ADMUX |= 0x40; // Use Vcc for analog reference.
	DIDR0 = 0x01; // turn off the digital input for adc0
	LOG_INFO( "MIC", "ADC is now setup for microphone on pin A0" );
	samplingStarted = millis();
}

void MicrophoneClass::update() {
	// Hardware read A0, bitbanging for faster read that ReadAnalog
	while ( !( ADCSRA & /*0x10*/_BV( ADIF ) ) ); // wait for adc to be ready (ADIF)
	sbi( ADCSRA, ADIF ); // restart adc
	byte m = ADCL; // fetch adc data
	byte j = ADCH;
	int k = ( (int) j << 8 ) | m; // form into an int

	// All statistics is made from the analogue value k. Microphone with 0 sound has output voltage of vcc/2
	int amp = abs( k - HIGHEST_AMPLITUDE );
	soundVolMax = max( soundVolMax, amp );
	soundVolAcc += amp;
	soundVolRMS += ( ( uint64_t )amp*amp );
	samples++;
}

MicData MicrophoneClass::getData() {
	MicData micData;

	micData.samples = samples;
	micData.sampleTime = millis() - samplingStarted;

	micData.volAvgPtc = 100 * ( (float) soundVolAcc / samples ) / HIGHEST_AMPLITUDE;
	micData.volMaxPtc = 100 * (float) soundVolMax / HIGHEST_AMPLITUDE;

	// RMS calculation
	float soundVolRMSflt = sqrt( soundVolRMS / samples );
	float soundVolRMSfltPtc = 100 * soundVolRMSflt / HIGHEST_AMPLITUDE;
	micData.volRmsPtc = 10 * soundVolRMSflt / 7;

	LOG_INFO( "MIC", "Sample time = " << micData.sampleTime << " ms");
	LOG_INFO( "MIC", "Samples = " << micData.samples );
	LOG_INFO( "MIC", "Avg = " << micData.volAvgPtc << " %" );
	LOG_INFO( "MIC", "Max = " << micData.volMaxPtc << " %" );
	LOG_INFO( "MIC", "RMS = " << micData.volRmsPtc << " %");

	// Reset statistic to start all over
	soundVolMax = soundVolAcc = soundVolRMS = 0;
	samples = 0;
	samplingStarted = millis();

	return micData;
}