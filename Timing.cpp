#include "Timing.h"



/* Setup a timer to trigger with a provided trigger frequency */
void Timing::setup( unsigned long triggerFreq ) {
	_triggerFreq = triggerFreq;
	lastTriggered = millis();
}



/* This should be called frequently. It returns true if time of frequency has passed */
bool Timing::triggered() {
	if ( millis() - lastTriggered >= _triggerFreq ) {
		lastTriggered = millis();
		return true;
	}
	return false;
}


/* Reset the timer */
void Timing::reset() {
	lastTriggered = millis();
}