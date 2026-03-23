/// @file test_v26_p20_visual_qa.cpp
/// @brief V26 Phase 20: Visual cleanup, consolidation, legacy removal & QA gates tests.
#include <catch2/catch_test_macros.hpp>
#include "ui/V26AccessibilityPolishConvergence.h"

using namespace markamp::ui;

TEST_CASE("V26 P20: Visual QA cleanup gate passes", "[v26][p20]")
{
    REQUIRE(VisualQAGates::passes_cleanup_gate());
}

TEST_CASE("V26 P20: Zero hardcoded targets", "[v26][p20]")
{
    REQUIRE(VisualQAGates::kMaxHardcodedColors == 0);
    REQUIRE(VisualQAGates::kMaxLocalDerivations == 0);
    REQUIRE(VisualQAGates::kMaxMagicNumbers == 0);
}

TEST_CASE("V26 P20: Required interaction state count", "[v26][p20]")
{
    REQUIRE(VisualQAGates::kRequiredInteractionStates == 5);
}

TEST_CASE("V26 P20: Theme and density coverage", "[v26][p20]")
{
    REQUIRE(VisualQAGates::kRequiredThemeVariants >= 2);
    REQUIRE(VisualQAGates::kRequiredDensityModes >= 3);
}

TEST_CASE("V26 P20: Core journey count", "[v26][p20]")
{
    REQUIRE(core_journey_count() == 8);
    REQUIRE(VisualQAGates::kCoreJourneyCount == 8);
    REQUIRE(VisualQAGates::kSurfaceGroupCount == 10);
}

TEST_CASE("V26 P20: Core journey enum", "[v26][p20]")
{
    REQUIRE(static_cast<int>(CoreJourney::kNewDocument) == 0);
    REQUIRE(static_cast<int>(CoreJourney::kPluginInstall) == 7);
}
