/// @file test_v23_platform_completion.cpp
#include <catch2/catch_test_macros.hpp>
#include "../../src/core/PlatformAccessibilityCompletionAuditor.h"
using namespace markamp::core;
TEST_CASE("P16 platform labels", "[v23][p16]") {
    REQUIRE(std::string(platform_accessibility_label(PlatformAccessibilityArea::kPlatformStubs)) == "PlatformStubs");
    REQUIRE(std::string(platform_accessibility_label(PlatformAccessibilityArea::kScreenReader)) == "ScreenReader");
}
TEST_CASE("P16 item completeness", "[v23][p16]") {
    PlatformAccessibilityItem a; REQUIRE_FALSE(a.is_complete());
    a.is_real_platform = true; a.has_error_handling = true; REQUIRE(a.is_complete());
}
TEST_CASE("P16 registration", "[v23][p16]") {
    PlatformAccessibilityCompletionAuditor aud;
    aud.add_item({PlatformAccessibilityArea::kAccessibilityBridge, "a11y-ids", true, true, "f.cpp", 1});
    REQUIRE(aud.item_count() == 1);
}
TEST_CASE("P16 queries", "[v23][p16]") {
    PlatformAccessibilityCompletionAuditor aud;
    aud.add_item({PlatformAccessibilityArea::kFileWatchUpdate, "watch", true, true, "f.cpp", 1});
    aud.add_item({PlatformAccessibilityArea::kFileWatchUpdate, "update-check", false, false, "f.cpp", 2});
    REQUIRE(aud.complete_items().size() == 1); REQUIRE(aud.incomplete_items().size() == 1);
}
TEST_CASE("P16 report", "[v23][p16]") {
    PlatformAccessibilityCompletionAuditor aud;
    aud.add_item({PlatformAccessibilityArea::kPromptHost, "prompt-runtime", true, true, "f.cpp", 1});
    auto r = aud.report(); REQUIRE(r.total == 1); REQUIRE_FALSE(r.has_gaps());
}
TEST_CASE("P16 clear", "[v23][p16]") {
    PlatformAccessibilityCompletionAuditor aud;
    aud.add_item({PlatformAccessibilityArea::kShellIntegration, "shell", true, true, "f.cpp", 1});
    aud.clear(); REQUIRE(aud.item_count() == 0);
}
TEST_CASE("P16 export json", "[v23][p16]") {
    PlatformAccessibilityCompletionAuditor aud;
    aud.add_item({PlatformAccessibilityArea::kCapabilityReporting, "caps", true, true, "f.cpp", 1});
    REQUIRE(aud.export_json().find("\"total\": 1") != std::string::npos);
}
TEST_CASE("P16 export markdown", "[v23][p16]") {
    PlatformAccessibilityCompletionAuditor aud;
    REQUIRE(aud.export_markdown().find("Platform Accessibility") != std::string::npos);
}
TEST_CASE("P16 integration", "[v23][p16]") {
    PlatformAccessibilityCompletionAuditor aud;
    aud.add_item({PlatformAccessibilityArea::kPlatformStubs, "linux-vibrancy", false, true, "f.cpp", 1});
    aud.add_item({PlatformAccessibilityArea::kAccessibilityBridge, "win-a11y", true, true, "f.cpp", 2});
    aud.add_item({PlatformAccessibilityArea::kCompletionGates, "os-matrix", true, true, "f.cpp", 3});
    auto r = aud.report(); REQUIRE(r.complete == 2); REQUIRE(r.incomplete == 1);
}
