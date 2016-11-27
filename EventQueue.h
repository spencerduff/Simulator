#ifndef EVENTQUEUE_H
#define EVENTQUEUE_H

#include "Task.h"
#include <queue>
#include <functional>
#include "EventComparator.h"

struct TaskEvents;
struct Event;

class EventQueue{
public:
	EventQueue(TaskEvents *te, int IODevices, int CPUs, double csc);

	std::priority_queue<Event*, std::vector<Event*>, EventComparator> taskQueue;
	std::priority_queue<Event*, std::vector<Event*>, EventComparator> SJFQueue;
	std::priority_queue<Event*, std::vector<Event*>, EventComparator> ASJFQueue;

	int CPUAvailable, IOAvailable;

	std::queue<Task*> CPUQueue;
	std::queue<Task*> IOQueue;
	std::priority_queue<Task*, std::vector<Task*>, SJFComparator> CPUQueueSJF;
	std::priority_queue<Task*, std::vector<Task*>, SJFComparator> IOQueueSJF;

	std::priority_queue<Task*, std::vector<Task*>, ASJFComparator> CPUQueueASJF;
	std::priority_queue<Task*, std::vector<Task*>, ASJFComparator> IOQueueASJF;

	double responseTotal;
	int responses;

	double latencyTotal;
	int completedTasks;

	int CPUSubTasks;

	double idleCPUTime;

	void setTime(double t){ currTime = t; }
	void addTime(double t){ currTime += t; }
	double getTime(){ return currTime; }
	double getContextSwitchCost(){ return contextSwitchCost; }

	double calcLatency(){ return latencyTotal / completedTasks; }
	double calcResponseTime(){ return responseTotal / responses; }

	double getThroughput(){ return completedTasks / currTime; }
	double getEfficiency(){ return (currTime - idleCPUTime) / currTime; }

	void setInterruptFreq(double i){ interruptFreq = i; timeUntilInterrupt = i; }

	double getTimeUntilInterrupt(){
		return timeUntilInterrupt;
	}
	
	void subtractTimeUntilInterrupt(double t){
		prevTimeUntilInterrupt = timeUntilInterrupt;
		timeUntilInterrupt -= t;
		if (timeUntilInterrupt < 0)
			timeUntilInterrupt = 0;
	}

	void resetTimeUntilInterrupt(){
		timeUntilInterrupt = interruptFreq;
	}


	double prevTimeUntilInterrupt;

private:
	double currTime;
	double contextSwitchCost;
	double interruptFreq;
	double timeUntilInterrupt;
	double usefulWorkTime;

	void addEvents(TaskEvents *te);
	void addSJFEvents(TaskEvents *te);
	void addASJFEvents(TaskEvents *te);
};


#endif