#ifndef TASK_H
#define TASK_H

#include <vector>
#include <random>

enum processType{ CPU = 0, IO = 1};

struct SubTask{
	processType process;
	double timeToComplete;
	int device;
	SubTask(processType p, int d, double t){
		process = p;
		device = d;
		timeToComplete = t;
	}
};

class Task{
public:
	Task(int tasks, std::mt19937 &mt, std::uniform_int_distribution<> &ioDeviceRange, bool CPUB);
	Task(){
		CPUbound = false;
		taskCounter = 0;
	}
	double getTotalTTC();
	double getRestTTC();
	double getDoneSoFarTime();
	double getCPUTTC();

	SubTask getNextSubTask(){ 
		return subTasks[taskCounter]; 
	}
	int getTaskCounter(){ return taskCounter; }
	int getSubTaskSize(){ return subTasks.size(); }

	bool operator<(Task &other);

	int taskCounter;

	bool getCPUbound();

	bool hasResponded(){ return responded; }
	void setResponded(){ responded = true; }
	void setRespondTime(double t){ respondTime = t; }
	void setStartedTime(double t){ startedTime = t; }
	double getStartedTime(){ return startedTime; }
	double getRespondTime(){ return respondTime; }
	double getFinishedTime(){ return finishedTime; }
	void setFinishedTime(double ft){ finishedTime = ft; }
	int getCPUSubTasksNum(){ return (subTasks.size() / 2) + 1; }

	void subCurrSubTaskTime(double t){ 
		subTasks[taskCounter].timeToComplete -= t;
		if (subTasks[taskCounter].timeToComplete < 0){
			subTasks[taskCounter].timeToComplete = 0;
			++taskCounter;
		}
	}

	std::vector<SubTask> subTasks;

private:
	bool CPUbound;
	bool responded;
	double startedTime, respondTime, finishedTime;
};

#endif