#pragma once

#include <vector>
#include <string>

class IDataProvider
{

public:

	virtual ~IDataProvider() = default;

	virtual std::vector<double> getData() const = 0;

};

class IWorker
{

public:

	virtual ~IWorker() = default;

	virtual double calculate(const std::vector<double>& vecData) const = 0;

};

class ILogger
{

public:

	virtual ~ILogger() = default;

	virtual void log(const std::string& strMsg) = 0;

};

class ICallback
{

public:

	virtual ~ICallback() = default;

	virtual void setResult(double dVal) = 0;

};

class BaseMethod
{

public:

	virtual double transform(double dVal) const { return dVal; }

};