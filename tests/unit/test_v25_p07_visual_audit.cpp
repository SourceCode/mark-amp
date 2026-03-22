/// @file test_v25_p07_visual_audit.cpp
/// @brief V25 Phase 07: Visual audit tests.
#include <catch2/catch_test_macros.hpp>
#include "core/V25ReleaseServices.h"

using namespace markamp::core;

TEST_CASE("V25 P07: Visual violations tracking", "[v25][p07]")
{
    ReleasePathVisualAuditor auditor;
    VisualViolation v;
    v.surface = "toolbar";
    v.description = "Placeholder icon";
    v.is_placeholder_icon = true;
    auditor.add_violation(v);
    REQUIRE(auditor.violation_count() == 1);
    REQUIRE(auditor.has_violations());
    REQUIRE(auditor.placeholder_icon_count() == 1);
}

TEST_CASE("V25 P07: Clean audit", "[v25][p07]")
{
    ReleasePathVisualAuditor auditor;
    REQUIRE_FALSE(auditor.has_violations());
    REQUIRE(auditor.placeholder_icon_count() == 0);
}

TEST_CASE("V25 P07: Clear violations", "[v25][p07]")
{
    ReleasePathVisualAuditor auditor;
    VisualViolation v;
    v.surface = "menu";
    auditor.add_violation(v);
    auditor.clear();
    REQUIRE(auditor.violation_count() == 0);
}
