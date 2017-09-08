#ifndef _DHTSENSOR_h
#define _DHTSENSOR_h

#define DHT_PIN 4
#define DHT_UPDATE_FREQ 2500 // DHT22 can not be polled faster than this

struct DHTSensorData {
	float temperature;
	float humidity;
	bool temperatureOk;
	bool humidityOk;
};

class DHTSensorClass
{
	 protected:
		 DHTSensorData dhtSensorData;

	 public:
		DHTSensorClass();
		void setup();
		void update();
		DHTSensorData getData();
};

#endif

