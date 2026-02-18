/// test_diagnostics_service.cpp — Unit tests
#include "core/DiagnosticsService.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("DiagnosticSeverity: compiles", "[diagnostics_service]")
{
    static_assert(sizeof(DiagnosticSeverity) > 0);
}
