#include "Logging.h"
#include "I2C.h"

#include "SensorDHT.h"
#include "SensorPPD.h"
#include "SensorMIC.h"
#include "SensorPIR.h"


#define REQUEST_DATA_PIN 2
#define STATUS_LED_PIN SCK


I2C i2c;
SensorMIC sensorMIC;
SensorDHT sensorDHT;
SensorPPD sensorPPD;
SensorPIR sensorPIR;


volatile bool collectData = true; // Tells all the sensors if they are allowed to collect data or not



void setup() {
	Serial.begin( 115200 ); 
	LOG_DEBUG( "SETUP", "Device booted" );

	// Setup I2C slave listener
	i2c.setup();

	// Sensors
	sensorMIC.setup();
	sensorDHT.setup();
	sensorPPD.setup();
	sensorPIR.setup();

	LOG_INFO( "SETUP", "Now listening on pin " << REQUEST_DATA_PIN << " to see if we are requested for data" );
	pinMode( REQUEST_DATA_PIN, INPUT_PULLUP );
	attachInterrupt( digitalPinToInterrupt( REQUEST_DATA_PIN ), handleCollectionReqIsr, CHANGE );
}



void loop() {
	sensorMIC.handle();
	sensorDHT.handle();
	sensorPPD.handle();
	sensorPIR.handle();
}



/* If data request pin is pulled low, the global variable "collectData" is set to false. 
   Otherwise to true */
void handleCollectionReqIsr() {
	collectData = digitalRead( REQUEST_DATA_PIN ) == LOW ? false : true;
	pinMode( STATUS_LED_PIN, OUTPUT );
	digitalWrite( STATUS_LED_PIN, !collectData );
}