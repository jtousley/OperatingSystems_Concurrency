#ifndef INITIALIZATION_H
#define INITIALIZATION_H
#include <iostream>

const double ABIGNUMBER = 1000000.0;

int getCPUS()
{
	auto input = 0;
	std::cout << "Enter number of CPUs to simulate: ";
	std::cin >> input;
	std::cout << "Stored: Number of CPUs = " << input << std::endl << std::endl;
	if(input < 1){
		throw std::string("Must have at least one CPU!");
	}
	
	return input;
}

int getIODevices()
{
	auto input = 0;
	std::cout << "Enter number of IO devices: ";
	std::cin >> input;
	std::cout << "Stored: Number of IO devices = " << input << std::endl << std::endl;
	if(input < 1){
		throw std::string("Must have at least one IO device!");
	}
	
	return input;
}

double getConSwitchCost()
{
	auto input = 0.0;
	std::cout << "Enter cost of a context switch: ";
	std::cin >> input;
	std::cout << "Stored: Context Switch Cost = " << input << std::endl << std::endl;
	if(input < 0){
		throw std::string("Cost of context switch must be positive!");
	}
	
	return input;
}

double getTaskMix()
{
	auto input = 0.0;
	std::cout << "Enter the percentage of CPU tasks: ";
	std::cin >> input;
	std::cout << "Stored: CPU Task Percentage = " << input << "%" << std::endl << std::endl;
	if(input < 0 || input > 100){
		throw std::string("Cannot have less than zero or more than 100 percent!");
	}

	return input;
}

double getJobCreationFrequency()
{
	auto input = 0.0;
	std::cout << "Enter the number of time quantums between process creations: ";
	std::cin >> input;
	std::cout << "Stored: Time between process creations = " << input << std::endl << std::endl;
	if(input == 0){
		throw std::string("Cannot divide by zero!");
	}
	
	return input;
}

char getSimulationType()
{
	auto input = 'z';
	std::cout << "Choose the type of simulation to run -\n"
		<< "A. FIFO\n"
		<< "B. Round Robin (FIFO + Interruption)\n"
		<< "C. SJF\n"
		<< "D. Approximate SJF\n"
		<< ": ";
	std::cin >> input;
	std::cout << std::endl
		<< "Stored: Simulation type = ";
	switch(input){
		case 'a':
		case 'A':
			std::cout << "FIFO\n";
			break;
		case 'b':
		case 'B':
			std::cout << "Round Robin\n";
			break;
		case 'c':
		case 'C':
			std::cout << "SJF\n";
			break;
		case 'd':
		case 'D':
			std::cout << "Approx. SJF\n";
			break;
		default:
			throw std::string("Invalid type of simulation!"); 
			break;
	}
	std::cout << std::endl << std::endl;

	return input;
}

double getInterruptionTime(char simType, double contextSwitchCost)
{
	auto input = 0.0;
	if(simType == 'b' || simType == 'B'){
		std::cout << "Enter the number of time quantums to execute before interrupting: ";
		std::cin >> input;
		std::cout << "Stored: Time quantums = " << input << std::endl << std::endl;
		if(input <= 0){
			throw std::string("Value must be greater than zero!");
		}
		if(input <= contextSwitchCost){
			throw std::string("Cannot interrupt as frequently as the cost of a context switch!");
		}

		return input;
	}
	else return ABIGNUMBER;
}

unsigned int getCacheSize(){
	auto input = 0;
	std::cout << "Enter the size of the cache: ";
	std::cin >> input;
	std::cout << "Stored: Cache size = " << input << std::endl << std::endl;
	if(input < 0){
		throw std::string("Cache must be a positive number!");
	}

	return input;
}

double getCacheMissPenalty(){
	auto input = 0.0;
	std::cout << "Enter the cost of a cache miss: ";
	std::cin >> input;
	std::cout << "Stored: Cache miss penalty = " << input << std::endl << std::endl;
	if(input < 0){
		throw std::string("Cannot have a negative time penalty for a cache miss!");
	}

	return input;
}

#endif