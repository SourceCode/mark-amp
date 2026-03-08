/// test_graphviz_renderer.cpp — Unit tests
#include "core/GraphvizRenderer.h"

#include <catch2/catch_test_macros.hpp>
using namespace markamp::core;
TEST_CASE("GraphvizRenderer: type compiles", "[graphviz_renderer]")
{
    static_assert(sizeof(GraphvizRenderer) > 0);
}
