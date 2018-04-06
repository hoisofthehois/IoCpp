#pragma once
#include "IExample.h"

class DataProvider : public virtual IDataProvider
{

public:

	DataProvider() = default;
	virtual ~DataProvider() = default;

	virtual std::vector<double> getData() const override;

};

class SumWorker : public virtual IWorker
{

public:

	virtual double calculate(const std::vector<double>& vecData) const override;

};

class ConsoleLogger : public virtual ILogger
{

public:

	virtual void log(const std::string& strMsg) override;

};

class Pause
{
	int m_nWait;

public:

	explicit Pause(int nSec) : m_nWait(nSec) {}

	void wait() const;

};

class ModifiedMethod : public BaseMethod
{

public:

	virtual double transform(double dVal) const override { return -1.0 * dVal; }

};

