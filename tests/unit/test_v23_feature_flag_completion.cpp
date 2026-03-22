/// @file test_v23_feature_flag_completion.cpp
#include <catch2/catch_test_macros.hpp>
#include "../../src/core/FeatureFlagMigrationCompletionAuditor.h"
using namespace markamp::core;
TEST_CASE("P18 feature flag labels", "[v23][p18]") {
    REQUIRE(std::string(feature_flag_label(FeatureFlagArea::kFlagInventory)) == "FlagInventory");
    REQUIRE(std::string(feature_flag_label(FeatureFlagArea::kDualPathMigration)) == "DualPathMigration");
}
TEST_CASE("P18 item completeness", "[v23][p18]") {
    FeatureFlagItem a; REQUIRE_FALSE(a.is_complete());
    a.is_resolved = true; a.has_error_handling = true; REQUIRE(a.is_complete());
}
TEST_CASE("P18 registration", "[v23][p18]") {
    FeatureFlagMigrationCompletionAuditor aud;
    aud.add_item({FeatureFlagArea::kGatedPanels, "debug-panel", true, true, "f.cpp", 1});
    REQUIRE(aud.item_count() == 1);
}
TEST_CASE("P18 queries", "[v23][p18]") {
    FeatureFlagMigrationCompletionAuditor aud;
    aud.add_item({FeatureFlagArea::kShadowMigration, "settings-v2", true, true, "f.cpp", 1});
    aud.add_item({FeatureFlagArea::kShadowMigration, "toolbar-v2", false, false, "f.cpp", 2});
    REQUIRE(aud.complete_items().size() == 1); REQUIRE(aud.incomplete_items().size() == 1);
}
TEST_CASE("P18 report", "[v23][p18]") {
    FeatureFlagMigrationCompletionAuditor aud;
    aud.add_item({FeatureFlagArea::kMigrationReporting, "startup-report", true, true, "f.cpp", 1});
    auto r = aud.report(); REQUIRE(r.total == 1); REQUIRE_FALSE(r.has_gaps());
}
TEST_CASE("P18 clear", "[v23][p18]") {
    FeatureFlagMigrationCompletionAuditor aud;
    aud.add_item({FeatureFlagArea::kExperimentalFlags, "exp-flag", true, true, "f.cpp", 1});
    aud.clear(); REQUIRE(aud.item_count() == 0);
}
TEST_CASE("P18 export json", "[v23][p18]") {
    FeatureFlagMigrationCompletionAuditor aud;
    aud.add_item({FeatureFlagArea::kClosureTests, "one-path", true, true, "f.cpp", 1});
    REQUIRE(aud.export_json().find("\"total\": 1") != std::string::npos);
}
TEST_CASE("P18 export markdown", "[v23][p18]") {
    FeatureFlagMigrationCompletionAuditor aud;
    REQUIRE(aud.export_markdown().find("Feature Flag") != std::string::npos);
}
TEST_CASE("P18 integration", "[v23][p18]") {
    FeatureFlagMigrationCompletionAuditor aud;
    aud.add_item({FeatureFlagArea::kFlagInventory, "hidden-panel", true, true, "f.cpp", 1});
    aud.add_item({FeatureFlagArea::kDualPathMigration, "settings-dual", false, true, "f.cpp", 2});
    aud.add_item({FeatureFlagArea::kRetirementTracking, "old-factory", true, true, "f.cpp", 3});
    auto r = aud.report(); REQUIRE(r.complete == 2); REQUIRE(r.incomplete == 1);
}
