/// test_profiler.cpp — Unit tests

#include "core/Profiler.h"

#include <catch2/catch_test_macros.hpp>
#include <string>

using namespace markamp::core;

TEST_CASE("ScopedTimer: basic timing", "[profiler]")
{
    {
        ScopedTimer timer("test_section");
        // Just verify it compiles and runs
    }
}

TEST_CASE("Profiler: default instance", "[profiler]")
{
    auto& prof = Profiler::instance();
    (void)prof;
}

TEST_CASE("Profiler: begin and end section", "[profiler]")
{
    auto& prof = Profiler::instance();
    prof.begin("test");
    prof.end("test");
}

TEST_CASE("TimingResult: default values", "[profiler]")
{
    Profiler::TimingResult result;
    REQUIRE(result.name.empty());
}
