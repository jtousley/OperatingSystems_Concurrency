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

void miscFunc(){
	int j = 0;
	j--;
	for(int i = 0; i < 100; i++){ j--; }

	return;
}

template <typename TYPE>
double average(const TYPE& A)
{
	double sum = std::accumulate(A.begin(), A.end(), 0.0);
	return sum / static_cast<double>(A.size());
}

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

template <typename TYPE>
int timeFunction(TYPE f)
{
	auto startTime = std::chrono::system_clock::now();
	f();
	auto endTime = std::chrono::system_clock::now();

	auto elapsedTime = std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - startTime).count();

	return elapsedTime;
}

template <typename TYPE>
std::string repeatedTimer(TYPE f, int iterations)
{
	std::vector<int> clockTimes(iterations);

	std::generate(clockTimes.begin(), clockTimes.end(), [&]()->int{timeFunction(f);});

	std::stringstream toReturn;
	toReturn << "Average time = " << average(clockTimes) << " ns; St. Dev = " 
		<< standardDeviation(clockTimes) << " ns";
	
	return toReturn.str();
}

#endif
