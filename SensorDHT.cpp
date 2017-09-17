#include "SensorDHT.h"
#include "Logging.h"
#include "DHT.h"
#include "I2C.h"

extern I2C i2c;
DHT dht( DHT_PIN, DHT22 );


void SensorDHT::setup( uint16_t measureFreq ) {
	isSetup = true;
	sendTimer.setup( (long) measureFreq * 1000 );

	dht.begin();
	LOG_NOTICE( "DHT", "Sensor started" );
}


void SensorDHT::handle() {
	if ( isSetup && sendTimer.triggered() ) sendData();
}


void SensorDHT::sendData() {
	float temperature = dht.readTemperature();
	if ( isnan( temperature ) || temperature < -40 || temperature > 80 ) {
		LOG_ERROR( "DHT", "Could not get sane temperature data" );
	} else {
		LOG_INFO( "DHT", "Temperature = " << temperature << " C" );
		i2c.send( 'T', temperature );
	}

	float humidity = dht.readHumidity();
	if ( isnan( humidity ) || humidity < 0 || humidity > 100 ) {
		LOG_ERROR( "DHT", "Could not get sane humidity data" );
	} else {
		LOG_INFO( "DHT", "Humidity = " << humidity << " %" );
		i2c.send( 'H', humidity );
	}
}
