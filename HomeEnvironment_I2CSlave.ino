#include "Logging.h"
#include "MHZSensor.h"
#include "SPImaster.h"
#include "DHTSensor.h"
#include "PPDSensor.h"
#include "Microphone.h"

SPImaster spi;
MicrophoneClass microphone;
DHTSensor dhtSensor;
PPDSensor ppdSensor;
MHZSensor mhzSensor;


void setup() {
	Serial.begin( 115200 );
	LOG_DEBUG( "SETUP", "Device booted, waiting 5 sec before sending data" );
	delay( 5000 ); // Make sure master is booted before we start sending data to it.

	microphone.setup( 5 );
	dhtSensor.setup( 5 );
	ppdSensor.setup( 300 );
	mhzSensor.setup( 10 );
}


void loop() {
	ppdSensor.handle();
	microphone.handle();
	dhtSensor.handle();
	mhzSensor.handle();
}