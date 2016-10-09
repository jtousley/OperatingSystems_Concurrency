/*
Jonathan Tousley
A01908436
Assignment 1 - Tools
Dr. Kenneth Sundberg
*/

#include <iostream>
#include <cstdio>
#include "Tools.h"

int main(){
	std::vector<int> V{1,2,3,4,5,6};
	const int REPEATED_CALLS = 1000;

	std::cout << "St. Dev = " << standardDeviation(V) << std::endl;
	std::cout << "Average value = " << average(V) << std::endl;
	std::cout << "Timing function standardDeviation...\nTime elapsed = " 
		<< timeFunction([&](){ standardDeviation(V); }) << " s" 
		<< std::endl;
	std::cout << "Calling function miscFunc " << REPEATED_CALLS << " times...\n" 
		<< repeatedTimer(miscFunc, REPEATED_CALLS) 
		<< std::endl;

	return 0;
}
