#ifndef _TIMING_h
#define _TIMING_h

#include <Arduino.h>


class Timing {
	protected:
		unsigned long _triggerFreq;
		unsigned long lastTriggered;

	public:
		void setup( unsigned long triggerFreq );
		bool triggered();
		void reset();
};


#endif

