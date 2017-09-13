#include "DHTSensor.h"
#include "Logging.h"
#include "SPImaster.h"
#include "DHT.h"

extern SPImaster spi;
DHT dht( DHT_PIN, DHT22 );


void DHTSensor::setup( uint16_t measureFreq ) {
	isSetup = true;
	sendTimer.setup( (long) measureFreq * 1000 );

	dht.begin();
	LOG_NOTICE( "DHT", "Sensor started" );
}


void DHTSensor::handle() {
	if ( isSetup && sendTimer.triggered() ) sendData();
}


void DHTSensor::sendData() {
	float temperature = dht.readTemperature();
	if ( isnan( temperature ) || temperature < -40 || temperature > 80 ) {
		LOG_ERROR( "DHT", "Could not get sane temperature data" );
	} else {
		LOG_INFO( "DHT", "Temperature = " << temperature << " C" );
		spi.send( 'T', &temperature, sizeof( temperature ) );
	}

	float humidity = dht.readHumidity();
	if ( isnan( humidity ) || humidity < 0 || humidity > 100 ) {
		LOG_ERROR( "DHT", "Could not get sane humidity data" );
	} else {
		LOG_INFO( "DHT", "Humidity = " << humidity << " %" );
		spi.send( 'H', &humidity, sizeof( humidity ) );
	}
}
