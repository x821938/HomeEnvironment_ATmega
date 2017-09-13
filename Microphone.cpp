#include "Microphone.h"
#include "Logging.h"
#include "SPImaster.h"

extern SPImaster spi;


void MicrophoneClass::setup( uint16_t sampleTime ) {
	isSetup = true;
	sampleTimer.setup( (long)sampleTime * 1000 );

	ADCSRA = 0xe0 + 7; // "ADC Enable", "ADC Start Conversion", "ADC Auto Trigger Enable" and divider.
	sbi( ADCSRA, ADPS2 ); cbi( ADCSRA, ADPS1 ); sbi( ADCSRA, ADPS0 ); // my board runs at 8mhz. prescaler 32=19200 samples/sek instead of 128=4800 samples/sek.
	ADMUX |= 0x40; // Use Vcc for analog reference.
	DIDR0 = 0x01; // turn off the digital input for adc0
	LOG_NOTICE( "MIC", "ADC is now setup for microphone on pin A0" );
	samplingStarted = millis();
}


void MicrophoneClass::handle() {
	if ( isSetup ) {
		// all samples must be taken right after each other, this is why nothing else can take place in the meantime
		lastMeasured = millis();
		LOG_DEBUG( "MIC", "Starting microphone sampling - doing only this" );
		while ( !sampleTimer.triggered() ) {
			sampleOnce();
		}
		sendData();
		delay( MIN_TIME_BETWEEN_GLOBAL_TRIGGERS ); // givup the timeslot so other sensors get a chance
	}
}


void MicrophoneClass::sampleOnce() {
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


void MicrophoneClass::sendData() {
	uint32_t sampleTime = millis() - samplingStarted;

	float volAvgPtc = 100 * ( (float) soundVolAcc / samples ) / HIGHEST_AMPLITUDE;
	float volMaxPtc = 100 * (float) soundVolMax / HIGHEST_AMPLITUDE;

	// RMS calculation
	float soundVolRMSflt = sqrt( soundVolRMS / samples );
	float soundVolRMSfltPtc = 100 * soundVolRMSflt / HIGHEST_AMPLITUDE;
	float volRmsPtc = 10 * soundVolRMSflt / 7;

	LOG_DEBUG( "MIC", "Sample time = " << sampleTime << " ms");
	LOG_DEBUG( "MIC", "Samples = " << samples );

	LOG_INFO( "MIC", "Avg = " << volAvgPtc << " %" );
	spi.send( 'V', &volAvgPtc, sizeof( volAvgPtc ) );

	LOG_INFO( "MIC", "Max = " << volMaxPtc << " %" );
	spi.send( 'M', &volMaxPtc, sizeof( volMaxPtc ) );

	LOG_INFO( "MIC", "RMS = " << volRmsPtc << " %");
	spi.send( 'R', &volRmsPtc, sizeof( volRmsPtc ) );


	// Reset statistic to start all over
	soundVolMax = soundVolAcc = soundVolRMS = 0;
	samples = 0;
	samplingStarted = millis();
}