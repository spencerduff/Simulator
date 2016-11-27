#include "EventTypes.h"
#include <iostream>

void TaskEvent::processEvent(EventQueue* eq){
	double prevTime = eq->getTime();
	eq->setTime(timeToProcess);
	eq->idleCPUTime += eq->CPUAvailable*(eq->getTime() - prevTime);
	task->setStartedTime(eq->getTime());
	if (task->getNextSubTask().process == CPU){
		eq->CPUQueue.push(task);
		if (eq->CPUAvailable > 0){
			eq->CPUQueue.pop();
			eq->CPUAvailable--;
			eq->idleCPUTime += eq->getContextSwitchCost();
			Event* e = new DoneEvent(task, eq->getTime() + task->getNextSubTask().timeToComplete + eq->getContextSwitchCost());
			eq->taskQueue.push(e);
		}
	}
	else if (task->getNextSubTask().process == IO){
		eq->IOQueue.push(task);
		if (eq->IOAvailable > 0){
			eq->IOQueue.pop();
			eq->IOAvailable--;
			Event* e = new DoneEvent(task, eq->getTime() + task->getNextSubTask().timeToComplete);
			eq->taskQueue.push(e);
		}
	}
}

void TaskEvent::processSJFEvent(EventQueue* eq){
	double prevTime = eq->getTime();
	eq->setTime(timeToProcess);
	eq->idleCPUTime += eq->CPUAvailable*(eq->getTime() - prevTime);
	task->setStartedTime(eq->getTime());
	if (task->getNextSubTask().process == CPU){
		eq->CPUQueueSJF.push(task);
		if (eq->CPUAvailable > 0){
			eq->CPUQueueSJF.pop();
			eq->CPUAvailable--;
			eq->idleCPUTime += eq->getContextSwitchCost();
			Event* e = new DoneEvent(task, eq->getTime() + task->getNextSubTask().timeToComplete + eq->getContextSwitchCost());
			eq->SJFQueue.push(e);
		}
	}
	else if (task->getNextSubTask().process == IO){
		eq->IOQueueSJF.push(task);
		if (eq->IOAvailable > 0){
			eq->IOQueueSJF.pop();
			eq->IOAvailable--;
			Event* e = new DoneEvent(task, eq->getTime() + task->getNextSubTask().timeToComplete);
			eq->SJFQueue.push(e);
		}
	}
}

void TaskEvent::processASJFEvent(EventQueue* eq){
	double prevTime = eq->getTime();
	eq->setTime(timeToProcess);
	eq->idleCPUTime += eq->CPUAvailable*(eq->getTime() - prevTime);
	task->setStartedTime(eq->getTime());
	if (task->getNextSubTask().process == CPU){
		eq->CPUQueueASJF.push(task);
		if (eq->CPUAvailable > 0){
			eq->CPUQueueASJF.pop();
			eq->CPUAvailable--;
			eq->idleCPUTime += eq->getContextSwitchCost();
			Event* e = new DoneEvent(task, eq->getTime() + task->getNextSubTask().timeToComplete + eq->getContextSwitchCost());
			eq->ASJFQueue.push(e);
		}
	}
	else if (task->getNextSubTask().process == IO){
		eq->IOQueueASJF.push(task);
		if (eq->IOAvailable > 0){
			eq->IOQueueASJF.pop();
			eq->IOAvailable--;
			Event* e = new DoneEvent(task, eq->getTime() + task->getNextSubTask().timeToComplete);
			eq->ASJFQueue.push(e);
		}
	}
}

void TaskEvent::processRoundRobin(EventQueue* eq){
	double prevTime = eq->getTime();
	eq->setTime(timeToProcess);
	eq->subtractTimeUntilInterrupt(eq->getTime() - prevTime);
	eq->idleCPUTime += eq->CPUAvailable*(eq->getTime() - prevTime);
	task->setStartedTime(eq->getTime());

	if (task->getNextSubTask().process == CPU){
		eq->CPUQueue.push(task);
		if (eq->CPUAvailable > 0){
			if (eq->getTimeUntilInterrupt() > 0){
				eq->CPUQueue.pop();
				eq->CPUAvailable--;
				eq->idleCPUTime += eq->getContextSwitchCost();
				Event* e = new DoneEvent(task, eq->getTime() + task->getNextSubTask().timeToComplete + eq->getContextSwitchCost());
				eq->taskQueue.push(e);
			}
			else{
				eq->resetTimeUntilInterrupt();
			}
		}
	}
}

void DoneEvent::processEvent(EventQueue* eq){
	double prevTime = eq->getTime();
	eq->setTime(timeToProcess);
	eq->idleCPUTime += eq->CPUAvailable*(eq->getTime() - prevTime);
	if (task->getNextSubTask().process == CPU){
		eq->CPUAvailable++;
		task->taskCounter++;
		if (task->getTaskCounter() < task->getSubTaskSize()){
			if (task->getNextSubTask().process == IO){
				eq->IOQueue.push(task);
				if (eq->IOAvailable > 0){
					eq->IOQueue.pop();
					eq->IOAvailable--;
					Event* e = new DoneEvent(task, eq->getTime() + task->getNextSubTask().timeToComplete);
					eq->taskQueue.push(e);
				}
			}
			else{
				eq->CPUQueue.push(task);
				if (eq->CPUAvailable > 0){
					eq->CPUQueue.pop();
					eq->CPUAvailable--;
					eq->idleCPUTime += eq->getContextSwitchCost();
					Event* e = new DoneEvent(task, eq->getTime() + task->getNextSubTask().timeToComplete + eq->getContextSwitchCost());
					eq->taskQueue.push(e);
				}
			}
		}
		else{
			task->setFinishedTime(eq->getTime());
			eq->latencyTotal += task->getFinishedTime() - task->getStartedTime();
			eq->completedTasks++;
		}
		if (eq->CPUAvailable > 0){
			if (!eq->CPUQueue.empty()){
				Task* t = eq->CPUQueue.front();
				eq->CPUQueue.pop();
				eq->CPUAvailable--;
				eq->idleCPUTime += eq->getContextSwitchCost();
				if (t->getTaskCounter() < t->getSubTaskSize()){
					Event* e = new DoneEvent(t, eq->getTime() + t->getNextSubTask().timeToComplete + eq->getContextSwitchCost());
					eq->taskQueue.push(e);
				}
			}
		}
	}
	else{
		eq->IOAvailable++;
		if (task->hasResponded() == false){
			task->setRespondTime(eq->getTime());
			task->setResponded();
			eq->responseTotal += task->getRespondTime() - task->getStartedTime();
			eq->responses++;
		}
		task->taskCounter++;
		if (task->getTaskCounter() < task->getSubTaskSize()){
			if (task->getNextSubTask().process == IO){
				eq->IOQueue.push(task);
			}
			else{
				eq->CPUQueue.push(task);
			}
		}
		if (eq->IOAvailable > 0){
			if (!eq->IOQueue.empty()){
				Task* t = eq->IOQueue.front();
				eq->IOQueue.pop();
				eq->IOAvailable--;
				if (t->getTaskCounter() < t->getSubTaskSize()){
					Event* e = new DoneEvent(t, eq->getTime() + t->getNextSubTask().timeToComplete);
					eq->taskQueue.push(e);
				}
			}
		}

	}
}

void DoneEvent::processSJFEvent(EventQueue* eq){
	double prevTime = eq->getTime();
	eq->setTime(timeToProcess);
	eq->idleCPUTime += eq->CPUAvailable*(eq->getTime() - prevTime);
	if (task->getSubTaskSize() > task->taskCounter){
		if (task->getNextSubTask().process == CPU){
			eq->CPUAvailable++;
			task->taskCounter++;
			if (task->getTaskCounter() < task->getSubTaskSize()){
				if (task->getNextSubTask().process == IO){
					eq->IOQueueSJF.push(task);
					if (eq->IOAvailable > 0){
						eq->IOQueueSJF.pop();
						eq->IOAvailable--;
						Event* e = new DoneEvent(task, eq->getTime() + task->getNextSubTask().timeToComplete);
						eq->SJFQueue.push(e);
					}
				}
				else{
					eq->CPUQueueSJF.push(task);
				}
			}
			else{
				task->setFinishedTime(eq->getTime());
				eq->latencyTotal += task->getFinishedTime() - task->getStartedTime();
				eq->completedTasks++;
			}
			if (eq->CPUAvailable > 0){
				if (!eq->CPUQueueSJF.empty()){
					Task* t = eq->CPUQueueSJF.top();
					eq->CPUQueueSJF.pop();
					eq->CPUAvailable--;
					eq->idleCPUTime += eq->getContextSwitchCost();
					if (t->getTaskCounter() < t->getSubTaskSize()){
						Event* e = new DoneEvent(t, eq->getTime() + t->getNextSubTask().timeToComplete + eq->getContextSwitchCost());
						eq->SJFQueue.push(e);
					}
				}
			}
		}
		else{
			eq->IOAvailable++;
			if (task->hasResponded() == false){
				task->setRespondTime(eq->getTime());
				task->setResponded();
				eq->responseTotal += task->getRespondTime() - task->getStartedTime();
				eq->responses++;
			}
			task->taskCounter++;
			if (task->getTaskCounter() < task->getSubTaskSize()){
				if (task->getNextSubTask().process == IO){
					eq->IOQueueSJF.push(task);
				}
				else{
					eq->CPUQueueSJF.push(task);
				}
			}
			if (eq->IOAvailable > 0){
				if (!eq->IOQueueSJF.empty()){
					Task* t = eq->IOQueueSJF.top();
					eq->IOQueueSJF.pop();
					eq->IOAvailable--;
					if (t->getTaskCounter() < t->getSubTaskSize()){
						Event* e = new DoneEvent(t, eq->getTime() + t->getNextSubTask().timeToComplete);
						eq->SJFQueue.push(e);
					}
				}
			}

		}
	}
}

void DoneEvent::processASJFEvent(EventQueue* eq){
	double prevTime = eq->getTime();
	eq->setTime(timeToProcess);
	eq->idleCPUTime += eq->CPUAvailable*(eq->getTime() - prevTime);
	if (task->getSubTaskSize() > task->taskCounter){
		if (task->getNextSubTask().process == CPU){
			eq->CPUAvailable++;
			task->taskCounter++;
			if (task->getTaskCounter() < task->getSubTaskSize()){
				if (task->getNextSubTask().process == IO){
					eq->IOQueueASJF.push(task);
					if (eq->IOAvailable > 0){
						eq->IOQueueASJF.pop();
						eq->IOAvailable--;
						Event* e = new DoneEvent(task, eq->getTime() + task->getNextSubTask().timeToComplete);
						eq->ASJFQueue.push(e);
					}
				}
				else{
					eq->CPUQueueASJF.push(task);
				}
			}
			else{
				task->setFinishedTime(eq->getTime());
				eq->latencyTotal += task->getFinishedTime() - task->getStartedTime();
				eq->completedTasks++;
			}
			if (eq->CPUAvailable > 0){
				if (!eq->CPUQueueASJF.empty()){
					Task* t = eq->CPUQueueASJF.top();
					eq->CPUQueueASJF.pop();
					eq->CPUAvailable--;
					eq->idleCPUTime += eq->getContextSwitchCost();
					if (t->getTaskCounter() < t->getSubTaskSize()){
						Event* e = new DoneEvent(t, eq->getTime() + t->getNextSubTask().timeToComplete + eq->getContextSwitchCost());
						eq->ASJFQueue.push(e);
					}
				}
			}
		}
		else{
			eq->IOAvailable++;
			if (task->hasResponded() == false){
				task->setRespondTime(eq->getTime());
				task->setResponded();
				eq->responseTotal += task->getRespondTime() - task->getStartedTime();
				eq->responses++;
			}
			task->taskCounter++;
			if (task->getTaskCounter() < task->getSubTaskSize()){
				if (task->getNextSubTask().process == IO){
					eq->IOQueueASJF.push(task);
				}
				else{
					eq->CPUQueueASJF.push(task);
				}
			}
			if (eq->IOAvailable > 0){
				if (!eq->IOQueueASJF.empty()){
					Task* t = eq->IOQueueASJF.top();
					eq->IOQueueASJF.pop();
					eq->IOAvailable--;
					if (t->getTaskCounter() < t->getSubTaskSize()){
						Event* e = new DoneEvent(t, eq->getTime() + t->getNextSubTask().timeToComplete);
						eq->ASJFQueue.push(e);
					}
				}
			}

		}
	}
}

void DoneEvent::processRoundRobin(EventQueue* eq){
	double prevTime = eq->getTime();
	eq->setTime(timeToProcess);

	eq->subtractTimeUntilInterrupt(eq->getTime() - prevTime);

	eq->idleCPUTime += eq->CPUAvailable*(eq->getTime() - prevTime);
	if (task->getNextSubTask().process == CPU){
		if (eq->getTimeUntilInterrupt() > 0){
			eq->CPUAvailable++;
			task->taskCounter++;
			if (task->getTaskCounter() < task->getSubTaskSize()){
				if (task->getNextSubTask().process == IO){
					eq->IOQueue.push(task);
					if (eq->IOAvailable > 0){
						eq->IOQueue.pop();
						eq->IOAvailable--;
						Event* e = new DoneEvent(task, eq->getTime() + task->getNextSubTask().timeToComplete);
						eq->taskQueue.push(e);
					}
				}
				else{
					eq->CPUQueue.push(task);
					if (eq->CPUAvailable > 0){
						eq->CPUQueue.pop();
						eq->CPUAvailable--;
						eq->idleCPUTime += eq->getContextSwitchCost();
						Event* e = new DoneEvent(task, eq->getTime() + task->getNextSubTask().timeToComplete + eq->getContextSwitchCost());
						eq->taskQueue.push(e);
					}
				}
			}
			else{
				task->setFinishedTime(eq->getTime());
				eq->latencyTotal += task->getFinishedTime() - task->getStartedTime();
				eq->completedTasks++;
			}
			if (eq->CPUAvailable > 0){
				if (!eq->CPUQueue.empty()){
					Task* t = eq->CPUQueue.front();
					eq->CPUQueue.pop();
					eq->CPUAvailable--;
					eq->idleCPUTime += eq->getContextSwitchCost();
					if (t->getTaskCounter() < t->getSubTaskSize()){
						Event* e = new DoneEvent(t, eq->getTime() + t->getNextSubTask().timeToComplete + eq->getContextSwitchCost());
						eq->taskQueue.push(e);
					}
				}
			}
		}
		else{
			eq->CPUAvailable++;
			if (eq->prevTimeUntilInterrupt - eq->getContextSwitchCost() > 0)
				task->subCurrSubTaskTime(eq->prevTimeUntilInterrupt - eq->getContextSwitchCost());
			if (task->getNextSubTask().process == CPU){
				eq->CPUQueue.push(task);
			}
			else{
				eq->IOQueue.push(task);
			}
			if (!eq->CPUQueue.empty()){
				Task* t = eq->CPUQueue.front();
				eq->CPUQueue.pop();
				eq->CPUAvailable--;
				eq->idleCPUTime += eq->getContextSwitchCost();
				if (t->getTaskCounter() < t->getSubTaskSize()){
					Event* e = new DoneEvent(t, eq->getTime() + t->getNextSubTask().timeToComplete + eq->getContextSwitchCost());
					eq->taskQueue.push(e);
				}
			}
			eq->resetTimeUntilInterrupt();
		}
	}
	else{
		eq->IOAvailable++;
		if (task->hasResponded() == false){
			task->setRespondTime(eq->getTime());
			task->setResponded();
			eq->responseTotal += task->getRespondTime() - task->getStartedTime();
			eq->responses++;
		}
		task->taskCounter++;
		if (task->getTaskCounter() < task->getSubTaskSize()){
			if (task->getNextSubTask().process == IO){
				eq->IOQueue.push(task);
			}
			else{
				eq->CPUQueue.push(task);
			}
		}
		if (eq->IOAvailable > 0){
			if (!eq->IOQueue.empty()){
				Task* t = eq->IOQueue.front();
				eq->IOQueue.pop();
				eq->IOAvailable--;
				if (t->getTaskCounter() < t->getSubTaskSize()){
					Event* e = new DoneEvent(t, eq->getTime() + t->getNextSubTask().timeToComplete);
					eq->taskQueue.push(e);
				}
			}
		}

	}
}