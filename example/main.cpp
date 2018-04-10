
#include <iostream>
#include <sstream>

#include "IoCpp.h"
#include "Example.h"
#include "Algorithm.h"

int main(int argc, char** argv)
{


	IoCpp::Container<
		IoCpp::OwnerMap<IDataProvider, DataProvider>,	// Map an instance
		IoCpp::SharedMap<IWorker, SumWorker>,		// Map a singleton
		IoCpp::SharedMap<ILogger, ConsoleLogger>,
		//IoCpp::FactoryMap<ILogger>,
		IoCpp::SharedMap<BaseMethod, ModifiedMethod>,
		IoCpp::FactoryMap<ICallback>		// Map a type that is later created through a factory function
	> container;

	auto pLogger = container.make<ILogger>();	// Create an interface implementation

	pLogger->log("Create callback...");
	container.setFactory<ICallback>([&container] {
		auto pCallback = std::make_shared<Callback>();
		pCallback->inject(container.make<ILogger>());
		return pCallback;
	});		// Inject a factory function

	pLogger->log("Starting test...");
	auto calculator = container.make<Algorithm>(-4.0);	// Create a concrete class with dependencies

	auto pause = container.make<Pause>(1);	// Create concrete class with no dependencies
	pause.wait();
	auto dResult = calculator.run();

	pLogger->log("-> Success!");
	return 0;
}