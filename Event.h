#ifndef EVENT_H
#define EVENT_H

#include "EventQueue.h"

struct Event{
	double timeToProcess;

	Event(double ttp){
		timeToProcess = ttp;
	}

	virtual void processEvent(EventQueue* eq);
	virtual void processSJFEvent(EventQueue* eq);
	virtual void processASJFEvent(EventQueue* eq);
	virtual void processRoundRobin(EventQueue* eq);
};



#endif