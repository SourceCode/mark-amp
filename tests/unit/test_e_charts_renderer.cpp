/// test_e_charts_renderer.cpp — Unit tests
#include "core/EChartsRenderer.h"

#include <catch2/catch_test_macros.hpp>
using namespace markamp::core;
TEST_CASE("EChartsRenderer: type compiles", "[echarts_renderer]")
{
    static_assert(sizeof(EChartsRenderer) > 0);
}
