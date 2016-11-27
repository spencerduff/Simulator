#ifndef EVENTTYPES_H
#define EVENTTYPES_H

#include "Event.h"

struct TaskEvents{
	double timeToAdd;
	std::vector<Task*>* taskList;

	TaskEvents(double tta, std::vector<Task*> *tasks){
		timeToAdd = tta;
		taskList = tasks;
	}

	TaskEvents(){
		timeToAdd = 0;
	}
};

struct TaskEvent : public Event{
	Task *task;

	void processEvent(EventQueue* eq) override;
	void processSJFEvent(EventQueue* eq) override;
	void processASJFEvent(EventQueue* eq) override;
	void processRoundRobin(EventQueue* eq) override;

	TaskEvent(Task *t, double tta) : Event(tta){
		task = t;
	}
};

struct DoneEvent : public Event{

	Task* task;

	DoneEvent(Task* dt, double td) : Event(td){
		task = dt;
	}

	void processEvent(EventQueue* eq) override;
	void processSJFEvent(EventQueue* eq) override;
	void processASJFEvent(EventQueue* eq) override;
	void processRoundRobin(EventQueue* eq) override;

};

#endif