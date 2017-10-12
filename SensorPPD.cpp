#include "SensorPPD.h"
#include "Logging.h"



/* Setup the input pin for PPD42 sensor*/
void SensorPPD::setup() {
	isSetup = true;

	pinMode( PPD_SENSOR_PIN, INPUT_PULLUP );
	LOG_INFO( "PPD", "Sensor started." );
	startReading();
}



/* Resets accumulated dust data and attaches interrupt on the input pin */
void SensorPPD::startReading() {
	samplingStartedAt = millis();
	ppdLowDuration = 0;
	ppdLastPinValue = true;
	attachInterrupt( digitalPinToInterrupt( PPD_SENSOR_PIN ), this->particleCountIsr, CHANGE );
	LOG_DEBUG( "PPD", "Interrupts enabled on pin " << PPD_SENSOR_PIN );
}



/* Disables interrupt for sensor and calculates the dustlevel */
void SensorPPD::stopReading() {
	detachInterrupt( digitalPinToInterrupt( PPD_SENSOR_PIN ) );
	sampleTime = millis() - samplingStartedAt;
	LOG_INFO( "PPD", "Interrupts disabled" );

	float ratio = ppdLowDuration / ( sampleTime*10.0 );  // Integer percentage 0=>100
	concentration = 1.1*pow( ratio, 3 ) - 3.8*pow( ratio, 2 ) + 520 * ratio + 0.62; // using spec sheet curve

	LOG_INFO( "PPD", "SampleTime = " << sampleTime << "ms. LowDuration = " <<
			  ppdLowDuration / 1000 << " ms. Ratio = " << ratio << "%. Concentration = " << concentration << " pcs/l." );
}



/* Returns measured dust concentration */
float SensorPPD::getConcentration() {
	return concentration;
}



/* ISR is called every time a dust particle hits the sensor. The time in microseconds that the pin is low is recorded */
void SensorPPD::particleCountIsr() {
	bool pinValue = ( digitalRead( PPD_SENSOR_PIN ) == HIGH );
	if ( ppdLastPinValue == false && pinValue == true ) { // The pin went from low to high
		ppdLowDuration += micros() - ppdGoingLowAt;
	} else if ( ppdLastPinValue == true && pinValue == false  ) { // The pin went high to low
		ppdGoingLowAt = micros();
	}
	ppdLastPinValue = pinValue;
}