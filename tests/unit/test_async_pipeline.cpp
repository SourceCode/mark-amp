/// test_async_pipeline.cpp — Unit tests for AsyncPipeline
#include "core/AsyncPipeline.h"

#include <catch2/catch_test_macros.hpp>
using namespace markamp::core;
TEST_CASE("AsyncPipeline: template instantiates", "[async_pipeline]")
{
    // Verify AsyncPipeline template compiles with basic types
    static_assert(sizeof(AsyncPipeline<std::string, int>) > 0);
}
