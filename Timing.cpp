#include "Timing.h"


void Timing::setup( long triggerFreq ) {
	_triggerFreq = triggerFreq;
	lastTriggered = millis();
}


bool Timing::triggered() {
	if ( millis() - lastTriggered >= _triggerFreq ) {
		lastTriggered = millis();
		return true;
	}
	return false;
}