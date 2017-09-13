#include "PPDSensor.h"
#include "SPImaster.h"
#include "Logging.h"

extern SPImaster spi;


void PPDSensor::setup( uint16_t measureFreq ) {
	isSetup = true;
	sendTimer.setup( (long)measureFreq * 1000 ); 

	samplingStartedAt = millis();
	lowDuration = 0;

	pinMode( SENSOR_PIN, INPUT_PULLUP );
	attachInterrupt( digitalPinToInterrupt( SENSOR_PIN ), this->particleCountIsr, CHANGE );
	LOG_NOTICE( "PPD", "Sensor started. Interrupts registered to pin " << SENSOR_PIN );
}


void PPDSensor::handle() {
	if ( isSetup && sendTimer.triggered() )	sendData();
}


void PPDSensor::particleCountIsr() {
	volatile static long goingLowAt = millis();
	volatile static bool lastPinValue = true;

	bool pinValue = ( digitalRead( SENSOR_PIN ) == HIGH );
	if (  pinValue == true && lastPinValue == false ) { // The pin went from high to low
		lowDuration += micros() - goingLowAt;
	} else if ( pinValue == false && lastPinValue == true ) { // The pin went low to high
		goingLowAt = micros();
	}
	lastPinValue = pinValue;
}


void PPDSensor::sendData() {
	sampleTime = millis() - samplingStartedAt;

	float ratio = lowDuration / ( sampleTime*10.0 );  // Integer percentage 0=>100
	float concentration = 1.1*pow( ratio, 3 ) - 3.8*pow( ratio, 2 ) + 520 * ratio + 0.62; // using spec sheet curve

	LOG_INFO( "PPD", "Stats:    SampleTime: " << sampleTime << "ms    LowDuration: " <<
		 lowDuration / 1000 << "ms    Ratio: " << ratio << "%    Concentration: " << concentration << "pcs/l." );

	lowDuration = 0;
	
	spi.send( 'D', &concentration, sizeof( concentration ) );
	samplingStartedAt = millis();
}
