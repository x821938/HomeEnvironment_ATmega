#include "DustSensor.h"
#include <Arduino.h>
#include "Logging.h"


static void DustSensorClass::particleCountIsr() {
	volatile static long goingLowAt;
	if ( digitalRead( SENSOR_PIN ) == HIGH ) {
		lowDuration += micros() - goingLowAt;
	} else {
		goingLowAt = micros();
	}
}

void DustSensorClass::setup() {
	pinMode( SENSOR_PIN, INPUT_PULLUP );
	attachInterrupt( digitalPinToInterrupt( SENSOR_PIN ), this->particleCountIsr, CHANGE );
	LOG_INFO( "PPD", "Sensor started. Interrupts registered to pin " << SENSOR_PIN );
}


float DustSensorClass::getConcentration() {
	static long samplingStarted = millis();
	long sampleTime = millis() - samplingStarted;
	samplingStarted = millis();

	float ratio = lowDuration / ( sampleTime*10.0 );  // Integer percentage 0=>100
	float concentration = 1.1*pow( ratio, 3 ) - 3.8*pow( ratio, 2 ) + 520 * ratio + 0.62; // using spec sheet curve

	LOG_DEBUG(  "PPD", "Stats:    SampleTime: " << sampleTime << "ms    LowDuration: " <<
		 lowDuration / 1000 << "ms    Ratio: " << ratio << "%    Concentration: " << concentration << "pcs/l." );

	lowDuration = 0;
	return concentration;
}