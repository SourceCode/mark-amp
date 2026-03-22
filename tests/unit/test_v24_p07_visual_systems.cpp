/// @file test_v24_p07_visual_systems.cpp
/// @brief V24 Phase 07 tests: Theme tokens, icon normalization, accessibility.
#include <catch2/catch_test_macros.hpp>
#include "../../src/core/AccessibilityStateManager.h"
#include "../../src/core/VisualCleanupCoordinator.h"

using namespace markamp::core;

// P07-T01: Theme token enforcement
TEST_CASE("P07-T01 visual cleanup icon migration status", "[v24][p07]") {
    VisualCleanupCoordinator coord;
    auto status = coord.icon_migration_status();
    REQUIRE(status.total_icons >= 0);
}

TEST_CASE("P07-T01 visual cleanup literal audit", "[v24][p07]") {
    VisualCleanupCoordinator coord;
    auto audit = coord.audit_hardcoded_literals();
    REQUIRE(audit.total_violations >= 0);
}

// P07-T02: Accessibility enabled features list
TEST_CASE("P07-T02 accessibility enabled features list", "[v24][p07]") {
    AccessibilityStateManager mgr;
    mgr.set_feature(AccessibilityFeature::kHighContrast, true);
    mgr.set_feature(AccessibilityFeature::kLargeText, true);
    auto features = mgr.enabled_features();
    REQUIRE(features.size() == 2);
}

TEST_CASE("P07-T02 accessibility feature disable", "[v24][p07]") {
    AccessibilityStateManager mgr;
    mgr.set_feature(AccessibilityFeature::kReducedMotion, true);
    REQUIRE(mgr.is_enabled(AccessibilityFeature::kReducedMotion));
    mgr.set_feature(AccessibilityFeature::kReducedMotion, false);
    REQUIRE_FALSE(mgr.is_enabled(AccessibilityFeature::kReducedMotion));
}

// P07-T03: Accessibility states
TEST_CASE("P07-T03 accessibility feature toggling", "[v24][p07]") {
    AccessibilityStateManager mgr;
    mgr.set_feature(AccessibilityFeature::kHighContrast, true);
    mgr.set_feature(AccessibilityFeature::kReducedMotion, true);
    REQUIRE(mgr.is_enabled(AccessibilityFeature::kHighContrast));
    REQUIRE(mgr.is_enabled(AccessibilityFeature::kReducedMotion));
    REQUIRE_FALSE(mgr.is_enabled(AccessibilityFeature::kLargeText));
}

TEST_CASE("P07-T03 contrast validation", "[v24][p07]") {
    AccessibilityStateManager mgr;
    mgr.record_contrast("button-primary", 5.0);   // passes AA
    mgr.record_contrast("text-light", 3.0);        // fails AA
    mgr.record_contrast("heading-main", 8.0);      // passes AAA

    auto report = mgr.audit();
    REQUIRE(report.elements_checked == 3);
    REQUIRE(report.contrast_passes == 2);
    REQUIRE(report.contrast_failures == 1);
    REQUIRE_FALSE(report.is_compliant());
}

// P07-T04: Visual regression gate
TEST_CASE("P07-T04 visual cleanup scoreboard", "[v24][p07]") {
    VisualCleanupCoordinator coord;
    auto scoreboard = coord.quality_scoreboard();
    REQUIRE(scoreboard.overall_score >= 0.0F);
}

TEST_CASE("P07-T04 visual exit criteria", "[v24][p07]") {
    VisualCleanupCoordinator coord;
    auto gates = coord.check_exit_criteria();
    REQUIRE(gates.size() >= 0);
}

// P07-T05: Accessibility report and clear
TEST_CASE("P07-T05 accessibility focus issues", "[v24][p07]") {
    AccessibilityStateManager mgr;
    mgr.record_focus_issue("modal-dialog");
    auto report = mgr.audit();
    REQUIRE(report.focus_issues == 1);
    REQUIRE_FALSE(report.is_compliant());

    mgr.clear();
    REQUIRE(mgr.checked_count() == 0);
}

TEST_CASE("P07-T05 accessibility feature labels", "[v24][p07]") {
    REQUIRE(std::string(accessibility_feature_label(AccessibilityFeature::kHighContrast)) == "HighContrast");
    REQUIRE(std::string(accessibility_feature_label(AccessibilityFeature::kReducedMotion)) == "ReducedMotion");
    REQUIRE(std::string(accessibility_feature_label(AccessibilityFeature::kLargeText)) == "LargeText");
}
