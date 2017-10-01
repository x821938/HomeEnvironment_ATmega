#include "SensorPIR.h"
#include "Logging.h"


extern volatile bool collectData;



/* Sets up input pin for sensor and resets the statistics */
void SensorPIR::setup() {
	isSetup = true;

	pinMode( PIR_SENSOR_PIN, INPUT );
	resetStats();

	LOG_NOTICE( "PIR", "Sensor started" );
}



/* Should be called frequently. If allowed to collect data the time of motion is accumulated */
void SensorPIR::handle() {
	static unsigned long goingHighAt = millis();
	static bool lastPinValue = false;

	if ( isSetup ) {
		if ( prevCollectData == false && collectData == true ) resetStats(); // We were just pulled for data, reset statistics
		if ( collectData ) {
			bool pinValue = ( digitalRead( PIR_SENSOR_PIN ) == HIGH );
			if ( lastPinValue == false && pinValue == true ) { // The pin went from low to high - motion detected
				goingHighAt = millis();
				pirMotionCount++;
				LOG_DEBUG( "PIR", "Motion count is now " << pirMotionCount );
			} else if ( lastPinValue == true && pinValue == false ) { // The pin went high to low - no more motion
				pirHighDuration += millis() - goingHighAt;
				LOG_DEBUG( "PIR", "Duration is now " << pirHighDuration / 1000L << "s" );
			}
			lastPinValue = pinValue;
		}
		prevCollectData = collectData;
	}
}



/* Zeroes statistics */
void SensorPIR::resetStats() {
	meassurmentStartedAt = millis();
	pirHighDuration = 0; 
	pirMotionCount = 0;
}



/* Returns the percentage of time there was movement over the measuring period */
float SensorPIR::getMotionPtc() {
	float meassurementTime = millis() - meassurmentStartedAt;
	return 100.0 * pirHighDuration / meassurementTime;
}
