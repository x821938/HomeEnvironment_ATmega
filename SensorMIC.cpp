#include "SensorMIC.h"
#include "Logging.h"



/* Sets up ADC (input A0) for free running mode.
   Reset statistics and turns on the microphone. It's feeded on an output pin to reduce supply noise */
void SensorMIC::setup() {
	isSetup = true;
	sampleTimer.setup(MIC_SAMPLE_TIME);

	ADCSRA = 0xe0 + 7; // "ADC Enable", "ADC Start Conversion", "ADC Auto Trigger Enable" and divider.
	sbi( ADCSRA, ADPS2 ); cbi( ADCSRA, ADPS1 ); sbi( ADCSRA, ADPS0 ); // my board runs at 8mhz. prescaler 32=19200 samples/sek instead of 128=4800 samples/sek.
	ADMUX |= 0x40; // Use Vcc for analog reference.
	DIDR0 = 0x01; // turn off the digital input for adc0

	// Reset statistics so we start from clean.
	newSample(); 
	newMeasurement();

	// We connect the microphone to an output pin to ensure it doesn't get noice. Here we power it on.
	pinMode( MIC_POWER_PIN, OUTPUT );
	digitalWrite( MIC_POWER_PIN, HIGH );

	LOG_INFO( "MIC", "ADC is now setup for microphone on pin A0" );
}



/* Should be called frequently. If allowed to collect data, it will do a lot of continious samples.
   Nothing else is allowed to run when this is happening - it affects the readings.
   When we have 1000ms of readings (about 19000 samples) they are accummulated */
void SensorMIC::handle() {
	if ( isSetup ) {
		newSample();
		// all samples must be taken right after each other, this is why nothing else can take place in the meantime
		while ( !sampleTimer.triggered() ) {
			sampleOnce();
		}
		accumulateData();
	}
}



/* Reset stats for the next 1000ms samples */
void SensorMIC::newSample() {
	//LOG_DEBUG( "MIC", "Starting microphone sampling - doing only this" );
	soundVolMax = soundVolAcc = soundVolRMS = 0;
	samples = 0;
	samplingStarted = millis();
}


/* Do one sampling */
void SensorMIC::sampleOnce() {
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



/* After the master has pulled data then we reset the overall accumulated data */
void SensorMIC::newMeasurement() {
	accVolPtc = accMaxPtc = accRmsPtc = accCount = 0;
	sampleTimer.reset();
}



/* Each 100ms sample is calculated and accumulated */
void SensorMIC::accumulateData() {
	if ( samples > 0 ) {
		uint32_t sampleTime = millis() - samplingStarted;

		float volAvgPtc = 100 * ( (float) soundVolAcc / samples ) / HIGHEST_AMPLITUDE;
		float volMaxPtc = 100 * (float) soundVolMax / HIGHEST_AMPLITUDE;

		float soundVolRMSflt = sqrt( soundVolRMS / samples );
		float soundVolRMSfltPtc = 100 * soundVolRMSflt / HIGHEST_AMPLITUDE;
		float volRmsPtc = 10 * soundVolRMSflt / 7;

		if ( volMaxPtc >= 49.7 && volMaxPtc <= 50.1 ) { 
			// These seems to appear when a lot of serial communication is going on. Decrease loglevel to 5 or less avoid this
			LOG_ERROR( "MIC", "Spurious reading removed: max=" << volMaxPtc << "%" );
		} else {
			LOG_DEBUG( "MIC", "Avg=" << volAvgPtc << "% Rms=" << volRmsPtc << "% Max=" << volMaxPtc << "%" );
			accVolPtc = accVolPtc + volAvgPtc;
			accRmsPtc = accRmsPtc + volRmsPtc;
			accMaxPtc = max( accMaxPtc, volMaxPtc );
			accCount++;
		}
	}
}



/* Returns the average volume percentage */
float SensorMIC::getAvgPtc() {
	if ( accCount > 0 ) {
		return accVolPtc / accCount;
	} else return 0;
}



/* Returns the maximum volume percentage */
float SensorMIC::getMaxPtc() {
	if ( accCount > 0 ) {
		return accMaxPtc;
	} else return 0;
}



/* Returns the average rms volume percentage. This is weightet more towards the loud sounds */
float SensorMIC::getRmsPtc() {
	if ( accCount > 0 ) {
		return accRmsPtc / accCount;
	} else return 0;
}
