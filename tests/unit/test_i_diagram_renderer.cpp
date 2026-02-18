/// test_i_diagram_renderer.cpp — Unit tests
#include "core/IDiagramRenderer.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("DiagramDiagnosticLevel: enum values", "[i_diagram_renderer]")
{
    REQUIRE(static_cast<int>(DiagramDiagnosticLevel::kInfo) != static_cast<int>(DiagramDiagnosticLevel::kWarning));
}

TEST_CASE("DiagramDiagnosticLevel: compiles", "[i_diagram_renderer]")
{
    static_assert(sizeof(DiagramDiagnosticLevel) > 0);
}
