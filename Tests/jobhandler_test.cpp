#include "catch.hpp"
#include "tr/core/JobHandler.h"

TEST_CASE("JobHandler", "[jobs]")
{
	// Create a JobHandler
	tr::JobHandler handler;

	// Start the handler right away
	handler.Start();

	SECTION("Adding a task")
	{
		int i = 0;

		auto status = handler.AddJob([&](){
			i = 10;
		});

		handler.Stop();

		REQUIRE(status);
		REQUIRE(i == 10);

	}
}
