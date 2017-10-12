#include "Logging.h"
#include "I2C.h"
#include "Timing.h"

#include "SensorDHT.h"
#include "SensorPPD.h"
#include "SensorMIC.h"
#include "SensorPIR.h"


#define MASTER_WAKEUP_PIN 8			// This pin is connected to ESP master RST pin.
#define MASTER_WAKEUP_FREQ 120		// How often we wake up the master to pull and wifi/mqtt send our data
#define MASTER_GIVE_UP_TIME 10		// If master doesn't fetch our data before this time, we continue our life!
#define CONTINUE_DATA_COLLECTION 1  // After master has told us to continue, how many seconds later we do that. 
#define STATUS_LED_PIN SCK


I2C i2c;
Timing masterWakeTimer;
Timing masterGiveUpTimer;
Timing continueDataCollection;

SensorMIC sensorMIC;
SensorDHT sensorDHT;
SensorPPD sensorPPD;
SensorPIR sensorPIR;



void setup() {
	Serial.begin( 115200 ); 
	LOG_NOTICE( "SLV", "Device booted" );

	// Setup I2C slave listener
	i2c.setup();

	// Sensors
	sensorMIC.setup();
	sensorDHT.setup();
	sensorPPD.setup();
	sensorPIR.setup();

	// Prepare pin for master wakeup. We don't want to touch it unless we have to reset/wake it.
	pinMode( MASTER_WAKEUP_PIN, INPUT );
	masterWakeTimer.setup( MASTER_WAKEUP_FREQ * 1000UL );
	masterGiveUpTimer.setup( MASTER_GIVE_UP_TIME * 1000L );
	continueDataCollection.setup( CONTINUE_DATA_COLLECTION * 1000UL );

	pinMode( STATUS_LED_PIN, OUTPUT );
	digitalWrite( STATUS_LED_PIN, LOW );
}



void loop() {
	static enum State {
		MasterWoken,
		MasterReportedFinishedXfer,
		CanCollectData
	} state = CanCollectData;

	switch ( state ) {
		case MasterWoken:
			if ( i2c.isMasterFinished() ) {
				LOG_DEBUG( "SLV", "Waiting " << CONTINUE_DATA_COLLECTION << " seconds for master to sleep" );
				continueDataCollection.reset();
				state = MasterReportedFinishedXfer;
			}
			if ( masterGiveUpTimer.triggered() ) {
				LOG_ALERT( "SLV", "Master has not fetched our data in " << MASTER_GIVE_UP_TIME << " Sec. Is it dead?" );
				state = MasterReportedFinishedXfer; // Fetch data again at hope that ESP master poll us next time
			}
			break;
		case MasterReportedFinishedXfer:
			if ( continueDataCollection.triggered() ) {
				LOG_NOTICE( "SLV", "Starting new data collection" );
				sensorDHT.newMeasurement();
				sensorMIC.newMeasurement();
				sensorPPD.startReading();
				sensorPIR.startReading();
				state = CanCollectData;

				digitalWrite( STATUS_LED_PIN, LOW );
			}
			break;
		case CanCollectData:
			sensorMIC.handle();
			sensorDHT.handle();
			sensorPIR.handle();
			sensorMIC.handle();
			if ( masterWakeTimer.triggered() ) {
				sensorPPD.stopReading();
				sensorPIR.stopReading();
				masterGiveUpTimer.reset();
				wakeMaster();
				state = MasterWoken;
			}
			break;
	}
}



/* Wakes up the master by pulling it's RST pin down for a short period */
void wakeMaster() {
	LOG_NOTICE( "SLV", "Pulling masters reset pin low to wake it up" );
	pinMode( MASTER_WAKEUP_PIN, OUTPUT );
	digitalWrite( MASTER_WAKEUP_PIN, LOW );
	delay( 10 );
	digitalWrite( MASTER_WAKEUP_PIN, HIGH );
	pinMode( MASTER_WAKEUP_PIN, INPUT );

	digitalWrite( STATUS_LED_PIN, HIGH ); // show the world that we are waiting for master
}