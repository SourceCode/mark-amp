/// test_flowchart_renderer.cpp — Unit tests
#include "core/FlowchartRenderer.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("FlowchartRenderer: compiles", "[flowchart_renderer]")
{
    static_assert(sizeof(FlowchartRenderer) > 0);
}
