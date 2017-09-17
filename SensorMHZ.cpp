#include "SensorMHZ.h"
#include "Logging.h"
#include <Arduino.h>
#include <AltSoftSerial.h>
#include "I2C.h"

AltSoftSerial swSerial;
extern I2C i2c;


void SensorMHZ::setup( uint16_t measureFreq ) {
	isSetup = true;
	warmedUp = false;

	sendTimer.setup( (long)measureFreq * 1000 );
	warmupTimer.setup( (long)MHZ_WARMUP_TIME * 1000 );

	swSerial.begin( 9600 );
	sensorStartedAt = millis();
	LOG_NOTICE( "MHZ", "Sensor started, but warming up for " << MHZ_WARMUP_TIME << " seconds");
}


void SensorMHZ::handle() {
	if ( isSetup ) {
		if ( warmedUp == false && warmupTimer.triggered() ) {
			LOG_NOTICE( "MHZ", "Sensor is now warmed up" );
			warmedUp = true;
		}
		if ( warmedUp && sendTimer.triggered() ) sendData();
	}
}


void SensorMHZ::sendData() {
	byte cmd[9] = { 0xFF,0x01,0x86,0x00,0x00,0x00,0x00,0x00,0x79 };
	unsigned char response[9];

	swSerial.write( cmd, 9 );
	swSerial.readBytes( response, 9 );

	unsigned int responseHigh = (unsigned int) response[2];
	unsigned int responseLow = (unsigned int) response[3];
	float ppm = ( 256 * responseHigh ) + responseLow;
	if ( ppm > 300 && ppm < 5000 ) {
		LOG_DEBUG( "MHZ", "CO2 level = " << ppm << "ppm" );
		i2c.send( 'Q', ppm );
	} else {
		LOG_ERROR( "MZH", "Could not get valid data from sensor" );
	}
}
