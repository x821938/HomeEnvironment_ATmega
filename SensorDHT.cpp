#include "SensorDHT.h"
#include "Logging.h"
#include "DHT.h"

DHT dht( DHT_PIN, DHT22 );
extern volatile bool collectData;



/* Start the physical sensor and set up a timer for how often we want readings*/
void SensorDHT::setup() {
	isSetup = true;
	meassureTimer.setup( DHT_MEASURE_FREQ );
	resetAccumulation();

	dht.begin();
	LOG_NOTICE( "DHT", "Sensor started" );
}



/* Called often. If we are allowed to collect data, and timer interval exceeded we get data
   If we were just polled for data, we reset the accumulated data */
void SensorDHT::handle() {
	if ( isSetup ) {
		if ( collectData && meassureTimer.triggered() ) accumulateData();
		if ( prevCollectData == false && collectData == true ) resetAccumulation();
		prevCollectData = collectData;
	}
}



/* Zeroes accumulated humidity and temperature */
void SensorDHT::resetAccumulation() {
	accTemp = accHum = accTempCount = accHumCount = 0;
	meassureTimer.reset();
}



/* Reads temperature and hymidity. If there is valid values they are accumulated and counted */
void SensorDHT::accumulateData() {
	float temperature = dht.readTemperature();
	if ( isnan( temperature ) || temperature < -40 || temperature > 80 ) {
		LOG_ERROR( "DHT", "Could not get sane temperature data" );
	} else {
		LOG_INFO( "DHT", "Temperature = " << temperature << " C" );
		accTemp += temperature;
		accTempCount++;
	}

	float humidity = dht.readHumidity();
	if ( isnan( humidity ) || humidity < 0 || humidity > 100 ) {
		LOG_ERROR( "DHT", "Could not get sane humidity data" );
	} else {
		LOG_INFO( "DHT", "Humidity = " << humidity << " %" );
		accHum += humidity;
		accHumCount++;
	}
}



/* If there are any temperature readings the average is returned 
   If not, a zero is returned */
float SensorDHT::getTemperature() {
	if ( isSetup && accTempCount > 0 ) {
		return accTemp / accTempCount;
	} else return 0;
}



/* If there are any humidity readings the average is returned
   If not, a zero is returned */
float SensorDHT::getHumidity() {
	if ( isSetup && accHumCount > 0 ) {
		return accHum / accHumCount;
	} else return 0;
}