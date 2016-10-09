#ifndef PROCESS_H
#define PROCESS_H

#include <random>
#include <queue>
#include <stack>
#include <memory>
#include <iostream>

const int 
	MINSHORTDURATION = 3,
	MAXSHORTDURATION = 10,
	MINLONGDURATION = 50,
	MAXLONGDURATION = 80,
	MINPROCESSBURSTPAIRS = 5,
	MAXPROCESSBURSTPAIRS = 25;

class Burst{
public:
	Burst(double t){ durationTotal = t; }
	virtual int getDevice() = 0;
	double getDuration()const{ return durationTotal; }
	void changeDuration(double edit){ durationTotal -= edit; }		//for FIFO with interruption

private:
	double durationTotal;
};

class CPUBurst : public Burst{
public:
	// CPUBurst();
	CPUBurst(double dur) : Burst(dur){}
	int getDevice(){ return -1; }
};

class IOBurst : public Burst{
public:
	// IOBurst();
	IOBurst(double dur, int dev) : Burst(dur){device = dev;}
	int getDevice(){ return device; }
private:
	int device;
};


class Process{
public:
	// Process(){}
	Process(double t, int IOdevices, bool x, int id, bool approx);
	void pop();
	std::shared_ptr<Burst> top() const{ return bursts.top(); }
	bool empty() const{ return bursts.empty(); }
	double getStartTime(){ return startTime; }
	int getID(){ return PID; }
	double getAverage()const{ return averageCPUBurst; }
	bool getApprox()const{ return approximation; }
		
	bool firstIOBurst;
private:
	void calculateAverageBurstTime();
	std::stack<std::shared_ptr<Burst>> bursts;
	double startTime;
	int PID;
	double averageCPUBurst;		//used for ASJF
	double totalCPUBurstTime;	//used for ASJF
	int finishedCPUBursts;		//used for ASJF
	bool approximation;		//used for ASJF
};

Process::Process(double t, int IOdevices, bool cpuheavyprocess, int id, bool approx)
	: firstIOBurst(true), startTime(t), PID(id), averageCPUBurst(0.0),
		totalCPUBurstTime(0.0), finishedCPUBursts(0), approximation(approx)
{	
	std::random_device Random;
	std::uniform_int_distribution<int> LONGAVERAGE(MINLONGDURATION, MAXLONGDURATION);
	std::uniform_int_distribution<int> SHORTAVERAGE(MINSHORTDURATION, MAXSHORTDURATION);
	std::uniform_int_distribution<int> StackSize(MINPROCESSBURSTPAIRS, MAXPROCESSBURSTPAIRS);
	std::uniform_int_distribution<int> Devices(0, IOdevices - 1);

	//task is heavy on CPU bursts
	if(cpuheavyprocess){
		bursts.push(std::make_shared<CPUBurst>(LONGAVERAGE(Random)));
		for(int i = 0; i < StackSize(Random); i++){
			bursts.push(std::make_shared<IOBurst>(SHORTAVERAGE(Random), Devices(Random)));
			bursts.push(std::make_shared<CPUBurst>(LONGAVERAGE(Random)));
		}
	}
	//task is heavy on IO bursts
	else{
		bursts.push(std::make_shared<CPUBurst>(SHORTAVERAGE(Random)));
		for(int i = 0; i < StackSize(Random); i++){
			bursts.push(std::make_shared<IOBurst>(LONGAVERAGE(Random), Devices(Random)));
			bursts.push(std::make_shared<CPUBurst>(SHORTAVERAGE(Random)));
		}
	}
}

void Process::pop()
{
	calculateAverageBurstTime();
	bursts.pop(); 
}

void Process::calculateAverageBurstTime()
{
	if(bursts.top()->getDevice() == -1){
		finishedCPUBursts++;
		totalCPUBurstTime += bursts.top()->getDuration();
		averageCPUBurst = totalCPUBurstTime / finishedCPUBursts;
	}
}

bool operator<(const Process& lhs, const Process& rhs)
{
	if(lhs.empty() || rhs.empty()){
		throw std::string("Comparison with empty process is not defined!");
	}
	if(lhs.getApprox()){
		return lhs.getAverage() > rhs.getAverage();
	}
	else{
		return lhs.top()->getDuration() > rhs.top()->getDuration();
	}
}

class ProcessCompare{
public:
	bool operator()(const Process& lhs, const Process& rhs) const
	{
		if(lhs.empty() || rhs.empty()){
			throw std::string("Comparison with empty process is not defined!");
		}
		return lhs < rhs;
	}
};

#endif