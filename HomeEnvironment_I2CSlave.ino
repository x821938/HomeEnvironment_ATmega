#include "DHTSensor.h"
#include "DustSensor.h"
#include "i2c.h"
#include "Microphone.h"

MicrophoneClass microphone;
DHTSensorClass dhtSensor;
DustSensorClass dust;
I2CClass i2c;

void setup() {
	Serial.begin( 115200 );
	microphone.setup();
	dhtSensor.setup();
	dust.setup();
	i2c.setup();
}

void loop() {
	microphone.update();
	dhtSensor.update();
	yield();
}