#include "EventQueue.h"
#include "EventTypes.h"

EventQueue::EventQueue(TaskEvents *te, int IODevices, int CPUs, double csc){
	responses = completedTasks = CPUSubTasks = 0;
	currTime = responseTotal = idleCPUTime = latencyTotal = usefulWorkTime = 0;
	addEvents(te);
	addSJFEvents(te);
	addASJFEvents(te);
	CPUAvailable = CPUs;
	IOAvailable = IODevices;
	contextSwitchCost = csc;
}

void EventQueue::addEvents(TaskEvents *te){
	for (unsigned int i = te->taskList->size() - 1; i >= 0 && i < INT_MAX; i--){
		CPUSubTasks += te->taskList->at(i)->getCPUSubTasksNum();
		Event* e = new TaskEvent(te->taskList->at(i), (te->taskList->size() - 1 - i) * te->timeToAdd);
		taskQueue.push(e);
		usefulWorkTime += te->taskList->at(i)->getCPUTTC();
	}
}

void EventQueue::addSJFEvents(TaskEvents *te){
	for (unsigned int i = te->taskList->size() - 1; i >= 0 && i < INT_MAX; i--){
		Event* e = new TaskEvent(te->taskList->at(i), (te->taskList->size() - 1 - i) * te->timeToAdd);
		SJFQueue.push(e);
	}
}

void EventQueue::addASJFEvents(TaskEvents *te){
	for (unsigned int i = te->taskList->size() - 1; i >= 0 && i < INT_MAX; i--){
		Event* e = new TaskEvent(te->taskList->at(i), (te->taskList->size() - 1 - i) * te->timeToAdd);
		ASJFQueue.push(e);
	}
}