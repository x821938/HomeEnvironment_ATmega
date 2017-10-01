#include "SensorPPD.h"
#include "Logging.h"


extern volatile bool collectData;



/* Setup the input pin for PPD42 sensor*/
void SensorPPD::setup() {
	isSetup = true;

	pinMode( PPD_SENSOR_PIN, INPUT_PULLUP );
	LOG_NOTICE( "PPD", "Sensor started." );
	startReading();
}



/* Should be called frequently. Starts and stops the datarecording if master wants it */
void SensorPPD::handle() {
	if ( isSetup ) {
		if ( prevCollectData == false && collectData == true ) {
			startReading();			
		} else if ( prevCollectData == true && collectData == false ) {
			stopReading();
		}
		prevCollectData = collectData;
	}
}



/* Resets accumulated dust data and attaches interrupt on the input pin */
void SensorPPD::startReading() {
	samplingStartedAt = millis();
	ppdLowDuration = 0;
	attachInterrupt( digitalPinToInterrupt( PPD_SENSOR_PIN ), this->particleCountIsr, CHANGE );
	LOG_NOTICE( "PPD", "Allowed to collect data. Interrupts enabled on pin " << PPD_SENSOR_PIN );
}



/* Disables interrupt for sensor and calculates the dustlevel */
void SensorPPD::stopReading() {
	detachInterrupt( digitalPinToInterrupt( PPD_SENSOR_PIN ) );
	sampleTime = millis() - samplingStartedAt;
	LOG_NOTICE( "PPD", "Interrupts disabled" );

	float ratio = ppdLowDuration / ( sampleTime*10.0 );  // Integer percentage 0=>100
	concentration = 1.1*pow( ratio, 3 ) - 3.8*pow( ratio, 2 ) + 520 * ratio + 0.62; // using spec sheet curve

	LOG_INFO( "PPD", "Stats:    SampleTime: " << sampleTime << "ms    LowDuration: " <<
			  ppdLowDuration / 1000 << "ms    Ratio: " << ratio << "%    Concentration: " << concentration << "pcs/l." );
}



/* Returns measured dust concentration */
float SensorPPD::getConcentration() {
	return concentration;
}



/* ISR is called every time a dust particle hits the sensor. The time in microseconds that the pin is low is recorded */
void SensorPPD::particleCountIsr() {
	volatile static unsigned long goingLowAt = millis();
	volatile static bool lastPinValue = true;

	bool pinValue = ( digitalRead( PPD_SENSOR_PIN ) == HIGH );
	if ( lastPinValue == false && pinValue == true ) { // The pin went from low to high
		ppdLowDuration += micros() - goingLowAt;
	} else if ( lastPinValue == true && pinValue == false  ) { // The pin went high to low
		goingLowAt = micros();
	}
	lastPinValue = pinValue;
}