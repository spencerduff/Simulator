#include "EventComparator.h"
#include "Event.h"
#include "Task.h"

bool EventComparator::operator () (Event* e1, Event* e2){
	return e1->timeToProcess > e2->timeToProcess;
}

bool SJFComparator::operator () (Task* t1, Task* t2){
	return t2->getRestTTC() < t1->getRestTTC();
}

bool ASJFComparator::operator () (Task* t1, Task* t2){
	return t2->getDoneSoFarTime() < t1->getDoneSoFarTime();
}