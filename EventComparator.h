#ifndef EVENTCOMPARATOR_H
#define EVENTCOMPARATOR_H

struct Event;
class Task;

struct EventComparator{
	bool operator () (Event* e1, Event* e2);
};

struct SJFComparator{
	bool operator () (Task* t1, Task* t2);
};

struct ASJFComparator{
	bool operator () (Task* t1, Task* t2);
};

#endif