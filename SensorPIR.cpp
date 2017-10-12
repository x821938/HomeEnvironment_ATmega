#include "SensorPIR.h"
#include "Logging.h"



/* Sets up input pin for sensor and resets the statistics */
void SensorPIR::setup() {
	isSetup = true;

	pinMode( PIR_SENSOR_PIN, INPUT );
	startReading();

	LOG_INFO( "PIR", "Sensor started" );
}



/* Whenever there is a change in pir statistics it is logged */
void SensorPIR::handle() {
	static uint16_t lastPirMotionCount = 0;
	static unsigned long lastPirHighDuration = 0;
	if ( pirHighDuration != 0 && lastPirHighDuration != pirHighDuration ) LOG_INFO( "PIR", "Pir motion time is now " << pirHighDuration << " ms" );
	if ( pirMotionCount != 0 && lastPirMotionCount != pirMotionCount ) LOG_INFO( "PIR", "Pir motion count is now " << pirMotionCount );
	lastPirHighDuration = pirHighDuration;
	lastPirMotionCount = pirMotionCount;
}



/* Resets accumulated PIR data and attaches interrupt on the input pin */
void SensorPIR::startReading() {
	meassurmentStartedAt = millis();
	pirHighDuration = 0;
	pirMotionCount = 0;
	pirLastPinValue = false;
	attachInterrupt( digitalPinToInterrupt( PIR_SENSOR_PIN ), this->pirHandleIsr, CHANGE );
	LOG_DEBUG( "PIR", "Interrupts enabled on pin " << PIR_SENSOR_PIN );
}



/* Disables interrupt for sensor */
void SensorPIR::stopReading() {
	detachInterrupt( digitalPinToInterrupt( PIR_SENSOR_PIN ) );
	meassurementTime = millis() - meassurmentStartedAt;
	LOG_INFO( "PIR", "Interrupts disabled" );
}



/* Should be called frequently. If allowed to collect data the time of motion is accumulated */
void SensorPIR::pirHandleIsr() {
	bool pinValue = ( digitalRead( PIR_SENSOR_PIN ) == HIGH );
	if ( pirLastPinValue == false && pinValue == true ) { // The pin went from low to high - motion detected
		pirGoingHighAt = millis();
		pirMotionCount++;
	} else if ( pirLastPinValue == true && pinValue == false ) { // The pin went high to low - no more motion
		pirHighDuration += millis() - pirGoingHighAt;
	}
	pirLastPinValue = pinValue;
}



/* Returns the percentage of time there was movement over the measuring period */
float SensorPIR::getMotionPtc() {
	return 100.0 * pirHighDuration / meassurementTime;
}