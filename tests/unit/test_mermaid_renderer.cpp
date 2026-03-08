/// test_mermaid_renderer.cpp
#include "core/MermaidRenderer.h"

#include <catch2/catch_test_macros.hpp>
using namespace markamp::core;
TEST_CASE("MermaidRenderer: type compiles", "[mermaid_renderer]")
{
    static_assert(sizeof(MermaidRenderer) > 0);
}
