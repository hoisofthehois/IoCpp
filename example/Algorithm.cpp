#include "Algorithm.h"

Algorithm::Algorithm(double dOffset) : m_dOffset(dOffset) {}

double Algorithm::run() const
{
	auto vecData = use<IDataProvider>()->getData();
	auto pWorker = use<IWorker>();
	auto dVal = m_dOffset + pWorker->calculate(vecData);
	dVal = use<BaseMethod>()->transform(dVal);
	if (auto pCallback = use<ICallback>())
		const_cast<ICallback*>(pCallback)->setResult(dVal);
	return dVal;
}


void Callback::setResult(double dVal)
{
	use<ILogger>()->log("Result = " + std::to_string(dVal));
}
