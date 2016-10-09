#ifndef TOOLS_H
#define TOOLS_H

#include <numeric>
#include <cmath>
#include <chrono>
#include <ctime>
#include <algorithm>
#include <vector>
#include <string>
#include <sstream>

//return average of the elements in A
template <typename TYPE>
double average(const TYPE& A)
{
	double sum = std::accumulate(A.begin(), A.end(), 0.0);
	return sum / static_cast<double>(A.size());
}

//return standard deviation of the elements in A
template <typename TYPE>
double standardDeviation(const TYPE& A)
{
	double avg = average(A);
	double sum;
	
	for(auto&& ele : A){
		sum += std::pow(ele-avg,2);
	}

	double std_dev = std::sqrt(sum / (A.size() - 1));

	return std_dev;
}

//returns the time to execute function f in ***seconds***
//use [=]()->NOTVOID{ return functionCall(param, param); }
template <typename TYPE>
double timeFunction(TYPE f)
{
	auto startTime = std::chrono::system_clock::now();
	f();
	auto endTime = std::chrono::system_clock::now();

	auto elapsedTime = std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - startTime).count();

	return elapsedTime / 1000000000.0;
}

//Calls function f iterations times, returns the average of the time taken for the call and the standard deviation
template <typename TYPE>
std::string repeatedTimer(TYPE f, int iterations)
{
	std::vector<double> clockTimes(iterations);

	std::generate(clockTimes.begin(), clockTimes.end(), [&]()->double{return timeFunction(f);});

	std::stringstream toReturn;
	toReturn << "Average time = " << average(clockTimes) << " s; St. Dev = " 
		<< standardDeviation(clockTimes) << " s";
	
	return toReturn.str();
}

#endif
