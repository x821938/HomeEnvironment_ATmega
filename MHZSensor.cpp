#include "MHZSensor.h"
#include "Logging.h"
#include <Arduino.h>
#include <SoftwareSerial.h>
#include "SPImaster.h"

SoftwareSerial softSerial( MHZ_RX_PIN, MHZ_TX_PIN );
extern SPImaster spi;


void MHZSensor::setup( uint16_t measureFreq ) {
	isSetup = true;
	warmedUp = false;

	sendTimer.setup( (long)measureFreq * 1000 );
	warmupTimer.setup( (long)MHZ_WARMUP_TIME * 1000 );

	softSerial.begin( 9600 );
	sensorStartedAt = millis();
	LOG_NOTICE( "MHZ", "Sensor started, but warming up for " << MHZ_WARMUP_TIME << " seconds");
}


void MHZSensor::handle() {
	if ( isSetup ) {
		if ( warmedUp == false && warmupTimer.triggered() ) {
			LOG_NOTICE( "MHZ", "Sensor is now warmed up" );
			warmedUp = true;
		}
		if ( warmedUp && sendTimer.triggered() ) sendData();
	}
}


void MHZSensor::sendData() {
	byte cmd[9] = { 0xFF,0x01,0x86,0x00,0x00,0x00,0x00,0x00,0x79 };
	unsigned char response[9];

	softSerial.write( cmd, 9 );
	softSerial.readBytes( response, 9 );
	unsigned int responseHigh = (unsigned int) response[2];
	unsigned int responseLow = (unsigned int) response[3];
	float ppm = ( 256 * responseHigh ) + responseLow;
	if ( ppm > 300 && ppm < 5000 ) {
		LOG_DEBUG( "MZH", "CO2 level = " << ppm << "ppm" );
		spi.send( 'Q', &ppm, sizeof( ppm ) );
	} else {
		LOG_ERROR( "MZH", "Could not get valid data from sensor" );
	}
}
