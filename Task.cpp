#include "Task.h"

Task::Task(int tasks, std::mt19937 &mt, std::uniform_int_distribution<> &ioDeviceRange, bool CPUB){

	std::normal_distribution<> rDisCPU;
	std::normal_distribution<> rDisIO;
	
	if (CPUB){
		std::normal_distribution<> temp(70, 25);
		std::normal_distribution<> temp2(30, 25);
		rDisCPU.param(temp.param());
		rDisIO.param(temp2.param());
	}
	else{
		std::normal_distribution<> temp(30, 25);
		std::normal_distribution<> temp2(70, 25);
		rDisCPU.param(temp.param());
		rDisIO.param(temp2.param());
	}

	double timeCPU = rDisCPU(mt);

	if (timeCPU < 1)
		timeCPU = 1;
	if (timeCPU > 100)
		timeCPU = 100;
	subTasks.push_back(SubTask(CPU, 0, timeCPU));

	bool flag = false;
	for (int i = 0; i < tasks - 2; i++){
		if (flag){
			double time = rDisCPU(mt);
			if (time < 1)
				time = 1;
			if (time > 100)
				time = 100;
			subTasks.push_back(SubTask(CPU, 0, time));
		}
		else{
			double time = rDisIO(mt);
			if (time < 1)
				time = 1;
			if (time > 100)
				time = 100;
			subTasks.push_back(SubTask(IO, ioDeviceRange(mt), time));
		}
		flag = !flag;
	}
	timeCPU = rDisCPU(mt);
	if (timeCPU < 1)
		timeCPU = 1;
	if (timeCPU > 100)
		timeCPU = 100;
	if (subTasks[subTasks.size() - 1].process != CPU)
		subTasks.push_back(SubTask(CPU, 0, timeCPU));

	double CPUtime = 0, IOtime = 0;

	for (unsigned int i = 0; i < subTasks.size(); i++){
		if (subTasks[i].device <= 0)
			CPUtime += subTasks[i].timeToComplete;
		else
			IOtime += subTasks[i].timeToComplete;
	}
	if (CPUtime >= IOtime)
		CPUbound = true;
	else
		CPUbound = false;
	taskCounter = 0;
	responded = false;
}

double Task::getTotalTTC(){
	double total = 0;
	for (unsigned int i = 0; i < subTasks.size(); i++){
		total += subTasks[i].timeToComplete;
	}
	return total;
}

double Task::getCPUTTC(){
	double total = 0;
	for (unsigned int i = 0; i < subTasks.size(); i++){
		if (subTasks[i].process == CPU){
			total += subTasks[i].timeToComplete;
		}
	}
	return total;
}

double Task::getRestTTC(){
	double total = 0;
	for (unsigned int i = this->taskCounter; i < subTasks.size(); i++){
		total += subTasks[i].timeToComplete;
	}
	return total;
}

double Task::getDoneSoFarTime(){
	double total = 0;
	for (int i = 0; i < this->taskCounter; i++){
		total += subTasks[i].timeToComplete;
	}
	return total;
}

bool Task::operator<(Task &other){
	if (getTotalTTC() <= other.getTotalTTC())
		return false;
	return true;
}

bool Task::getCPUbound(){
	return CPUbound;
}