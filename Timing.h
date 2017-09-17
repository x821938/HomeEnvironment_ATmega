#ifndef _TIMING_h
#define _TIMING_h

#include <Arduino.h>


class Timing {
protected:
	long _triggerFreq;
	long lastTriggered;

public:
	void setup( long triggerFreq );
	bool triggered();
};


#endif

