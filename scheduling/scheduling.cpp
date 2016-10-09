/*
Jonathan Tousley
A01908436
j.tousley@live.com
Dr. Kenneth Sundberg
Assignment 8 - CPU + Memory Scheduling
*/

#include "Process.h"
#include "Event.h"
#include "Initialization.h"
#include "Tools.h"
#include "ReadyQueue.h"
#include "Cache.h"
#include <iostream>
#include <string>
#include <queue>

typedef std::shared_ptr<Event> Eptr;

const unsigned int ENDTIME = 1000;

void printResults(int queuedProcesses, int executedProcesses, std::vector<double>& Latency 
	, std::vector<double>& ResponseTime, std::vector<double>& Utilization, std::vector<double>& CPUExecutionTime
	, double currentTime, unsigned int cacheMisses);


int main(void)
{

try{
	int numberCPUS = getCPUS();
	int numberIODevices = getIODevices();
	double contextSwitchCost = getConSwitchCost();
	double taskMix = getTaskMix();
	double jobFreq = getJobCreationFrequency();
	unsigned int cacheSize = getCacheSize();
	double cacheMiss = getCacheMissPenalty();
	char simulationType = getSimulationType();
	double interruptionTime = getInterruptionTime(simulationType, contextSwitchCost);
	bool approximateSJF = false;
	bool shortestJob = false;
	if(simulationType == 'd' || simulationType == 'D'){
		approximateSJF = true;
		shortestJob = true;
	}
	if(simulationType == 'c' || simulationType == 'C'){
		shortestJob = true;
	}

	Cache cache(cacheSize);
	unsigned int cacheMisses = 0;
	std::priority_queue<Eptr, std::vector<Eptr>, PointerLess<Eptr>> EventQueue;
	std::vector<std::queue<Process>> IOQueue;
	std::vector<bool> AvailableIO;
	for(int i = 0; i < numberIODevices; i++){
		IOQueue.push_back(std::queue<Process>());
		AvailableIO.push_back(true);
	}
	std::vector<bool> AvailableCPUs;
	std::vector<double> CPUExecutionTime;
	for(int i = 0; i < numberCPUS; i++){
		AvailableCPUs.push_back(true);
		CPUExecutionTime.push_back(0.0);
	}
	std::normal_distribution<double> TaskRatio(0.0, 100.0);
	std::random_device Random;
	int queuedProcesses = 0;
	int executedProcesses = 0;

	std::vector<double> Latency;
	std::vector<double> ResponseTime;
	std::vector<double> Utilization;

	//this part adjusts the ratio of new processes to reflect the proper cpu/io balance
	for(double t = 0.0; t < ENDTIME; t += jobFreq){
		double distributor = TaskRatio(Random);
		bool cpuheavy = false;
		if(distributor < taskMix){
			cpuheavy = true;
		}
		else{
			cpuheavy = false;
		}
		EventQueue.push(std::make_shared<startProcess>(t, numberIODevices, cpuheavy, queuedProcesses, approximateSJF));
		queuedProcesses++;
	}

	double currentTime = 0.0;
	RQ<Process> ReadyQueue(shortestJob);

	while(!EventQueue.empty()){
		Eptr currentEvent = EventQueue.top();
		EventQueue.pop();
		currentTime = currentEvent->getTime();
		Process currentProcess = currentEvent->getProcess();
		switch(currentEvent->getType()){
			case 0: //CPU finished
				if(currentEvent->getDevice() == -1){ throw std::string("IO task assigned to CPU device!"); }
				AvailableCPUs[currentEvent->getDevice()] = true; // CPU becomes available

				if(currentEvent->getInterrupt()){
					//got interrupted
					currentProcess.top()->changeDuration(interruptionTime - contextSwitchCost);
					ReadyQueue.push(currentProcess);
				}
				else{
					//process done
					currentProcess.pop();
					if(!currentProcess.empty()){
						IOQueue[currentProcess.top()->getDevice()].push(currentProcess);
					}
					else{
						executedProcesses++;
						Latency.push_back(currentTime - currentProcess.getStartTime());
					}
				}
				break;
			case 1: //IO finished
				if(currentEvent->getDevice() == -1){ throw std::string("IO/CPU mismatch!"); }
				AvailableIO[currentEvent->getDevice()] = true;
				if(currentProcess.firstIOBurst){
					ResponseTime.push_back(currentTime - currentProcess.getStartTime());
					currentProcess.firstIOBurst = false;
				}
				currentProcess.pop();
				if(currentProcess.empty()){ throw std::string("Stack ended on IO task!"); }
				ReadyQueue.push(currentProcess);
				break;
			case 2: // new process to execute
				ReadyQueue.push(currentProcess);
				break;
			default:
				throw std::string("Undefined type in Event Queue!");
				break;
		}
		
		if(!ReadyQueue.empty()){
			Process currentCPUTask = ReadyQueue.front();
			bool foundAvailable = false;
			if(currentCPUTask.top()->getDevice() != -1){ throw std::string("Should have been a CPU process!"); }
			for(int core = 0; core < numberCPUS; core++){
				if(AvailableCPUs[core]){
					if(!foundAvailable){
						ReadyQueue.pop();
						double penalty = 0.0;
						if(!cache.Find(currentCPUTask.getID())){
							penalty = cacheMiss;
							cacheMisses++;
						}
						if((currentCPUTask.top()->getDuration() + contextSwitchCost + penalty) > interruptionTime){
							EventQueue.push(std::make_shared<CPUFinished>(
								currentTime + interruptionTime, 
								core, 
								currentCPUTask,
								true));	
							CPUExecutionTime[core] += interruptionTime;
						}
						else{
							EventQueue.push(std::make_shared<CPUFinished>(
								currentTime + currentCPUTask.top()->getDuration() + contextSwitchCost + penalty, 
								core, 
								currentCPUTask,
								false));							
							CPUExecutionTime[core] += currentCPUTask.top()->getDuration() + contextSwitchCost + penalty;
						}
						AvailableCPUs[core] = false;
						foundAvailable = true;
					}
				}
			}
		}
		for(int io = 0; io < numberIODevices; io++){
			if(AvailableIO[io] && !IOQueue[io].empty()){
				Process currentIOTask = IOQueue[io].front();
				if(currentIOTask.top()->getDevice() != io){ throw std::string("IO task queued to wrong IO device!"); }
				IOQueue[io].pop();
				EventQueue.push(std::make_shared<IOFinished>(
					currentTime + currentIOTask.top()->getDuration(), io, currentIOTask));
				AvailableIO[io] = false;
			}
		}
	} // for all time

	printResults(queuedProcesses, executedProcesses, Latency, ResponseTime, Utilization, CPUExecutionTime, currentTime, cacheMisses);

} // try
catch(std::string ex){
	std::cout << ex << std::endl;
}

return 0;
}


void printResults(int queuedProcesses, int executedProcesses, std::vector<double>& Latency 
	, std::vector<double>& ResponseTime, std::vector<double>& Utilization, std::vector<double>& CPUExecutionTime
	, double finishTime, unsigned int cacheMisses)
{
	std::cout << "Queued Processes:\n";
		std::cout << '\t' << queuedProcesses << std::endl;
	std::cout << "Executed Processes:\n";
		std::cout << '\t' << executedProcesses << std::endl;

	std::cout << "Latency (time quantums):\n";
		std::cout << "\tMinimum: " << *std::min_element(Latency.begin(), Latency.end()) << std::endl;
		std::cout << "\tMaximum: " << *std::max_element(Latency.begin(), Latency.end()) << std::endl;
		std::cout << "\tAverage: " << average(Latency) << std::endl;
		std::cout << "\tStandard Deviation: " << standardDeviation(Latency) << std::endl;
	std::cout << "Response time (time quantums):\n";
		std::cout << "\tMinimum: " << *std::min_element(ResponseTime.begin(), ResponseTime.end()) << std::endl;
		std::cout << "\tMaximum: " << *std::max_element(ResponseTime.begin(), ResponseTime.end()) << std::endl;
		std::cout << "\tAverage: " << average(ResponseTime) << std::endl;
		std::cout << "\tStandard Deviation: " << standardDeviation(ResponseTime) << std::endl;

	//utilization calculation
	for(int i = 0; i < static_cast<int>(CPUExecutionTime.size()); i++){
		Utilization.push_back(100* (CPUExecutionTime[i] / finishTime));
	}
	std::cout << "Utilization (time quantums):\n";
		std::cout << "\tMinimum: " << *std::min_element(Utilization.begin(), Utilization.end()) << "%" << std::endl;
		std::cout << "\tMaximum: " << *std::max_element(Utilization.begin(), Utilization.end()) << "%" << std::endl;
		std::cout << "\tAverage: " << average(Utilization) << "%" << std::endl;
		std::cout << "\tStandard Deviation: " << standardDeviation(Utilization) << "%" << std::endl;

	double throughput = executedProcesses / finishTime;

	std::cout << "Throughput (tasks / time quantum):\n"
		<< '\t' << throughput << std::endl;

	std::cout << "Cache Misses:\n"
		<< '\t' << cacheMisses << std::endl;
}

//okay to have such thick functions?
//what is the standard on such things?