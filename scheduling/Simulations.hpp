#ifndef SIMULATIONS_HPP
#define SIMULATIONS_HPP

#include <iostream>
#include <string>
#include <queue>

typedef std::shared_ptr<Event> Eptr;

void executeApproxSJF(std::priority_queue<Eptr, std::vector<Eptr>, PointerLess<Eptr>>& EventQueue
	, std::vector<std::queue<Process>>& IOQueue
	, std::vector<bool>& AvailableIO, std::vector<bool>& AvailableCPUs, std::vector<double>& CPUExecutionTime
	, int &numberCPUS, int &numberIODevices, double &contextSwitchCost
	, std::vector<double>& Latency, std::vector<double>& ResponseTime, double &currentTime, int& executedProcesses)
{
	std::priority_queue<Process, std::vector<Process>, ProcessCompare> ReadyQueue;

	while(!EventQueue.empty()){
		Eptr currentEvent = EventQueue.top();
		EventQueue.pop();
		currentTime = currentEvent->getTime();
		Process currentProcess = currentEvent->getProcess();
		switch(currentEvent->getType()){
			case 0: //CPU finished
				if(currentEvent->getDevice() == -1){ throw std::string("IO task assigned to CPU device!"); }
				AvailableCPUs[currentEvent->getDevice()] = true; // CPU becomes available
				currentProcess.pop();
				if(!currentProcess.empty()){
					IOQueue[currentProcess.top()->getDevice()].push(currentProcess);
				}
				else{
					executedProcesses++;
					Latency.push_back(currentTime - currentProcess.getStartTime());
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
			Process currentCPUTask = ReadyQueue.top();
			bool foundAvailable = false;
			if(currentCPUTask.top()->getDevice() != -1){ throw std::string("Should have been a CPU process!"); }
			for(int core = 0; core < numberCPUS; core++){
				if(AvailableCPUs[core]){
					if(!foundAvailable){
						ReadyQueue.pop();
						EventQueue.push(std::make_shared<CPUFinished>(
							currentTime + currentCPUTask.top()->getDuration() + contextSwitchCost, 
							core, 
							currentCPUTask, false));
						AvailableCPUs[core] = false;
						foundAvailable = true;
						CPUExecutionTime[core] += currentCPUTask.top()->getDuration() + contextSwitchCost;
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
}

void executeSJF(std::priority_queue<Eptr, std::vector<Eptr>, PointerLess<Eptr>>& EventQueue
	, std::vector<std::queue<Process>>& IOQueue
	, std::vector<bool>& AvailableIO, std::vector<bool>& AvailableCPUs, std::vector<double>& CPUExecutionTime
	, int &numberCPUS, int &numberIODevices, double &contextSwitchCost
	, std::vector<double>& Latency, std::vector<double>& ResponseTime, double &currentTime, int& executedProcesses)
{
	std::priority_queue<Process, std::vector<Process>, ProcessCompare> ReadyQueue;

	while(!EventQueue.empty()){
		Eptr currentEvent = EventQueue.top();
		EventQueue.pop();
		currentTime = currentEvent->getTime();
		Process currentProcess = currentEvent->getProcess();
		switch(currentEvent->getType()){
			case 0: //CPU finished
				if(currentEvent->getDevice() == -1){ throw std::string("IO task assigned to CPU device!"); }
				AvailableCPUs[currentEvent->getDevice()] = true; // CPU becomes available
				currentProcess.pop();
				if(!currentProcess.empty()){
					IOQueue[currentProcess.top()->getDevice()].push(currentProcess);
				}
				else{
					executedProcesses++;
					Latency.push_back(currentTime - currentProcess.getStartTime());
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
			Process currentCPUTask = ReadyQueue.top();
			bool foundAvailable = false;
			if(currentCPUTask.top()->getDevice() != -1){ throw std::string("Should have been a CPU process!"); }
			for(int core = 0; core < numberCPUS; core++){
				if(AvailableCPUs[core]){
					if(!foundAvailable){
						ReadyQueue.pop();
						EventQueue.push(std::make_shared<CPUFinished>(
							currentTime + currentCPUTask.top()->getDuration() + contextSwitchCost, 
							core, 
							currentCPUTask, false));
						AvailableCPUs[core] = false;
						foundAvailable = true;
						CPUExecutionTime[core] += currentCPUTask.top()->getDuration() + contextSwitchCost;
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
}

void executeRoundRobin(std::priority_queue<Eptr, std::vector<Eptr>, PointerLess<Eptr>>& EventQueue
	, std::vector<std::queue<Process>>& IOQueue
	, std::vector<bool>& AvailableIO, std::vector<bool>& AvailableCPUs, std::vector<double>& CPUExecutionTime
	, int &numberCPUS, int &numberIODevices, double &contextSwitchCost
	, std::vector<double>& Latency, std::vector<double>& ResponseTime, double &currentTime, int& executedProcesses
	, double interruptionTime)
{
	std::queue<Process> ReadyQueue;

	while(!EventQueue.empty()){
		Eptr currentEvent = EventQueue.top();
		EventQueue.pop();
		currentTime = currentEvent->getTime();
		Process currentProcess = currentEvent->getProcess();
		switch(currentEvent->getType()){
			case 0: //CPU finished
				if(currentEvent->getDevice() == -1){ throw std::string("IO task assigned to CPU device!"); }
				AvailableCPUs[currentEvent->getDevice()] = true; // CPU becomes available
				currentProcess.top()->changeDuration(interruptionTime - contextSwitchCost);

				if(currentEvent->getInterrupt()){
					//got interrupted
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
						if((currentCPUTask.top()->getDuration() + contextSwitchCost) > interruptionTime){
							EventQueue.push(std::make_shared<CPUFinished>(
								currentTime + interruptionTime, 
								core, 
								currentCPUTask,
								true));	
							CPUExecutionTime[core] += interruptionTime;
						}
						else{
							EventQueue.push(std::make_shared<CPUFinished>(
								currentTime + currentCPUTask.top()->getDuration() + contextSwitchCost, 
								core, 
								currentCPUTask,
								false));							
							CPUExecutionTime[core] += currentCPUTask.top()->getDuration() + contextSwitchCost;
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

}

void executeFIFO(std::priority_queue<Eptr, std::vector<Eptr>, PointerLess<Eptr>>& EventQueue
	, std::vector<std::queue<Process>>& IOQueue
	, std::vector<bool>& AvailableIO, std::vector<bool>& AvailableCPUs, std::vector<double>& CPUExecutionTime
	, int &numberCPUS, int &numberIODevices, double &contextSwitchCost
	, std::vector<double>& Latency, std::vector<double>& ResponseTime, double &currentTime, int &executedProcesses)
{
	std::queue<Process> ReadyQueue;

	while(!EventQueue.empty()){
		Eptr currentEvent = EventQueue.top();
		EventQueue.pop();
		currentTime = currentEvent->getTime();
		Process currentProcess = currentEvent->getProcess();
		switch(currentEvent->getType()){
			case 0: //CPU finished
				if(currentEvent->getDevice() == -1){ throw std::string("IO task assigned to CPU device!"); }
				AvailableCPUs[currentEvent->getDevice()] = true; // CPU becomes available
				currentProcess.pop();
				if(!currentProcess.empty()){
					IOQueue[currentProcess.top()->getDevice()].push(currentProcess);
				}
				else{
					executedProcesses++;
					Latency.push_back(currentTime - currentProcess.getStartTime());
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
						EventQueue.push(std::make_shared<CPUFinished>(
							currentTime + currentCPUTask.top()->getDuration() + contextSwitchCost, 
							core, 
							currentCPUTask, false));
						AvailableCPUs[core] = false;
						foundAvailable = true;
						CPUExecutionTime[core] += currentCPUTask.top()->getDuration() + contextSwitchCost;
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
}

#endif