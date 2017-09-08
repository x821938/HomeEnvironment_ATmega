#include "DHTSensor.h"
#include "Logging.h"
#include "DHT.h"
#include "Logging.h"

DHT dht( DHT_PIN, DHT22 );

DHTSensorClass::DHTSensorClass() {
}

void DHTSensorClass::setup() {
	dht.begin();
	dhtSensorData.temperatureOk = dhtSensorData.humidityOk = false;
	dhtSensorData.temperature = dhtSensorData.humidity = NAN;
	LOG_INFO( "DHT", "Sensor started" );
}

void DHTSensorClass::update() {
	static long lastUpdated = millis();
	if ( millis() - lastUpdated > DHT_UPDATE_FREQ ) {
		dhtSensorData.humidity = dht.readHumidity();
		dhtSensorData.temperature = dht.readTemperature();
		lastUpdated = millis();
	}
}

DHTSensorData DHTSensorClass::getData() {
	if ( isnan( dhtSensorData.temperature ) || dhtSensorData.temperature < -40 || dhtSensorData.temperature > 80 ) {
		dhtSensorData.temperatureOk = false;
		LOG_ERROR( "DHT", "Could not get sane temperature data" );
	} else {
		dhtSensorData.temperatureOk = true;
		LOG_INFO( "DHT", "Temperature = " << dhtSensorData.temperature << " C" );
	}

	if ( isnan( dhtSensorData.humidity ) || dhtSensorData.humidity < 0 || dhtSensorData.humidity > 100 ) {
		dhtSensorData.humidityOk = false;
		LOG_ERROR( "DHT", "Could not get sane humidity data" );
	} else {
		dhtSensorData.humidityOk = true;
		LOG_INFO( "DHT", "Humidity = " << dhtSensorData.humidity << " %" );
	}
	return dhtSensorData;
}
