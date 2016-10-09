template <typename TYPE>
std::chrono::duration<double> timeFunction(TYPE const& f)
{
	auto startTime = std::chrono::system_clock::now();
	f();
	auto stopTime = std::chrono::system_clock::now();

	return stopTime - startTime;
}

//auto duration = timeFunction([](){ foo(); });

template <typename CONTAINER>
typename CONTAINER::value_type average(CONTAINER const &c)
{
	auto sum = std::accumulate(c.cbegin(), c.cend(), 
		typename CONTAINER::value_type(), [](typename CONTAINER::value_type a, typename CONTAINER::value_type b){ return a+b;});
	auto sum2 = std::accumulate(c.cbegin(), c.cend(), 0.0);
	return sum / c.size();	
}

template <typename TYPE>
std::pair<
	std::chrono::deviation<double>
	,std::chrono::deviation<double>>
stdDevFunc(TYPE const &f, int iterations)
{
	std::vector<std::chrono::duration<double>> times(iterations);
	std::generate(times.begin(), times.end(), [](){ timeFunction(f); });
	auto avg = average (times);
	auto sigma = std::transform(times.begin(), times.end(), times.begin(), 
		[=](std::chrono::duration<double> a){ return (a-avg)*(a-avg); });
	auto stddev = std::sqrt(std::accumulate(times.begin(), times.end()) / (times.size() - 1));

	return std::make_pair(avg, stddev);
}
