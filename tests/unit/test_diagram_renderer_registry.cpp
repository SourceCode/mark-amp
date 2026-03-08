/// test_diagram_renderer_registry.cpp — Unit tests
#include "core/DiagramRendererRegistry.h"

#include <catch2/catch_test_macros.hpp>
using namespace markamp::core;
TEST_CASE("DiagramRendererRegistry: type compiles", "[diagram_renderer_registry]")
{
    static_assert(sizeof(DiagramRendererRegistry) > 0);
}
