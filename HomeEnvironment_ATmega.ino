#include "Logging.h"
#include "I2C.h"
#include "SensorMHZ.h"
#include "SensorDHT.h"
#include "SensorPPD.h"
#include "SensorMIC.h"

I2C i2c;
SensorMIC sensorMIC;
SensorDHT sensorDHT;
SensorPPD sensorPPD;
SensorMHZ sensorMHZ;


void setup() {
	Serial.begin( 115200 );
	LOG_DEBUG( "SETUP", "Device booted" );

	i2c.setup();
	sensorMIC.setup( 5 );
	sensorDHT.setup( 5 );
	sensorPPD.setup( 300 );
	sensorMHZ.setup( 10 );
}


void loop() {
	sensorPPD.handle();
	sensorMIC.handle();
	sensorDHT.handle();
	sensorMHZ.handle();
}