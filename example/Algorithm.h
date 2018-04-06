#pragma once

#include "IExample.h"
#include "IoCpp.h"

class Algorithm final : public IoCpp::DependsOn<IWorker, IDataProvider, BaseMethod, ICallback>
{

private:

	double m_dOffset;

public:

	Algorithm() = delete;
	explicit Algorithm(double dOffset);
	~Algorithm() = default;

	double run() const;

};


class Callback : public virtual ICallback , public IoCpp::DependsOn<ILogger>
{

public:

	virtual void setResult(double dVal) override;

};