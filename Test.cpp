#include "Task.h"
#include <algorithm>
#include <iostream>
#include <time.h>
#include "EventQueue.h"
#include "Event.h"
#include "EventTypes.h"
#include "Cache.h"

int main(){
	std::random_device rd;
	std::mt19937 mt(rd());

	int min, max, ioDevices, processNum, CPUpercentage, CPUs, cacheSize, pageDistribution;
	double timeToAdd, contextSwitchCost, cacheMissPenalty;

	std::cout << "Number of processes to run (int): ";
	std::cin >> processNum;
	std::cout << "Minimum number of subtasks for a process (int): ";
	std::cin >> min;
	std::cout << "Maximum number of subtasks for a process (int): ";
	std::cin >> max;

	std::cout << "Number of IO devices (int): ";
	std::cin >> ioDevices;
	std::cout << "Number of CPUs (int): ";
	std::cin >> CPUs;

	std::cout << "Percentage of CPU bound (int): ";
	std::cin >> CPUpercentage;

	std::cout << "Time between adding tasks (double): ";
	std::cin >> timeToAdd;

	std::cout << "Cost of context switch (double): ";
	std::cin >> contextSwitchCost;

	std::cout << "Size of cache (int): ";
	std::cin >> cacheSize;

	std::cout << "Cache miss penalty (double): ";
	std::cin >> cacheMissPenalty;

	std::cout << "Page distribution (int): ";
	std::cin >> pageDistribution;
		 
	if (ioDevices <= 0)
		ioDevices = 1;

	if (max < min)
		std::swap(max, min);

	if (processNum <= 0)
		processNum = 1;

	std::uniform_int_distribution<> iDis(min, max);
	std::uniform_int_distribution<> iDis2(1, ioDevices);
	std::uniform_real_distribution<> rDis(0, 1);

	std::vector<Task*> taskList;
	for (int i = 0; i < processNum; i++){
		if (rDis(mt) >(double)CPUpercentage / 100)
			taskList.push_back(new Task(iDis(mt), mt, iDis2, true));
		else 
			taskList.push_back(new Task(iDis(mt), mt, iDis2, false));
	}

	std::sort(taskList.begin(), taskList.end());

	/*for (unsigned int i = 0; i < taskList.size(); i++){
		std::cout << taskList[i].getTotalTTC() << std::endl;
		}*/

	int CPUbound = 0, IObound = 0;
	for (unsigned int i = 0; i < taskList.size(); i++){
		if (taskList.at(i)->getCPUbound())
			CPUbound++;
		else
			IObound++;
	}

	std::cout << "Number of CPU bounded: " << CPUbound << std::endl;
	std::cout << "Number of IO bounded: " << IObound << std::endl;

	TaskEvents te(timeToAdd, &taskList);

	EventQueue eq(&te, ioDevices, CPUs, contextSwitchCost);

	char choice = 'n';

	int memChoice = 0;

	std::cout << "Which memory algorithm would you like to use? " << std::endl;
	std::cout << "1: FIFO\n";
	std::cout << "2: Second Chance\n";
	std::cout << "3: Least Recently Used\n";
	std::cout << "4: Most Recently Used\n";
	std::cin >> memChoice;



	std::cout << "FIFO Queue? ";
	std::cin >> choice;
	if (choice == 'y' || choice == 'Y'){
		while (!eq.taskQueue.empty()){
			eq.taskQueue.top()->processEvent(&eq);
			eq.taskQueue.pop();
		}
	}
	choice = 'n';
	std::cout << "SJF Queue? ";
	std::cin >> choice;
	if (choice == 'y' || choice == 'Y'){
		while (!eq.SJFQueue.empty()){
			eq.SJFQueue.top()->processSJFEvent(&eq);
			eq.SJFQueue.pop();
		}
	}
	choice = 'n';
	std::cout << "ASJF Queue? ";
	std::cin >> choice;
	if (choice == 'y' || choice == 'Y'){
		while (!eq.ASJFQueue.empty()){
			eq.ASJFQueue.top()->processASJFEvent(&eq);
			eq.ASJFQueue.pop();
		}
	}
	choice = 'n';
	std::cout << "Round Robin Queue? ";
	std::cin >> choice;
	if (choice == 'y' || choice == 'Y'){
		double interruptTime;
		std::cout << "Interrupt frequency (double): ";
		std::cin >> interruptTime;
		eq.setInterruptFreq(interruptTime);
		while (!eq.taskQueue.empty()){
			eq.taskQueue.top()->processRoundRobin(&eq);
			eq.taskQueue.pop();
		}
	}

	auto memVec = make_list(eq.CPUSubTasks, pageDistribution);
	Fifo fifo(cacheSize);
	SecondChance sc(cacheSize);
	LRU lru(cacheSize);
	MRU mru(cacheSize);

	switch (memChoice){
	case 1:
		eq.addTime(cacheMissPenalty*fifo.run(memVec));
		break;
	case 2:
		eq.addTime(cacheMissPenalty*sc.run(memVec));
		break;
	case 3:
		eq.addTime(cacheMissPenalty*lru.run(memVec));
		break;
	case 4:
		eq.addTime(cacheMissPenalty*mru.run(memVec));
		break;
	}

	std::cout << "Response Time: " << eq.calcResponseTime() << std::endl;
	std::cout << "Latency: " << eq.calcLatency() << std::endl;
	std::cout << "Throughput: " << eq.getThroughput() << std::endl;
	std::cout << "Efficiency: " << eq.getEfficiency() << std::endl;

	return 0;
}