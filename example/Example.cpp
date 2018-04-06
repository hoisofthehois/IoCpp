#include "Example.h"

#include <algorithm>
#include <iostream>
#include <chrono>
#include <thread>

std::vector<double> DataProvider::getData() const
{
	return std::vector<double> { 1.0, 3.0, 5.0, 7.0, 11.0, 13.0, 15.0};
}

double SumWorker::calculate(const std::vector<double>& vecData) const
{
	double dSum = 0.0;
	for (auto&& dVal : vecData)
		dSum += dVal;
	return dSum;
}

void ConsoleLogger::log(const std::string& strMsg)
{
	std::cout << strMsg << std::endl;
}

void Pause::wait() const
{
	std::this_thread::sleep_for(std::chrono::seconds{m_nWait});
}
