/// @file test_v23_plugin_completion.cpp
#include <catch2/catch_test_macros.hpp>
#include "../../src/core/PluginMarketplaceCompletionAuditor.h"
using namespace markamp::core;

TEST_CASE("P11 plugin capability labels", "[v23][p11]") {
    REQUIRE(std::string(plugin_capability_label(PluginCapabilityArea::kContributionFulfillment)) == "ContributionFulfillment");
    REQUIRE(std::string(plugin_capability_label(PluginCapabilityArea::kMarketplaceListings)) == "MarketplaceListings");
    REQUIRE(std::string(plugin_capability_label(PluginCapabilityArea::kCompletionGates)) == "CompletionGates");
}
TEST_CASE("P11 plugin item completeness", "[v23][p11]") {
    PluginCompletionItem a; REQUIRE_FALSE(a.is_complete());
    a.is_fulfilled = true; REQUIRE_FALSE(a.is_complete());
    a.has_error_handling = true; REQUIRE(a.is_complete());
}
TEST_CASE("P11 auditor registration", "[v23][p11]") {
    PluginMarketplaceCompletionAuditor aud;
    REQUIRE(aud.item_count() == 0);
    aud.add_item({PluginCapabilityArea::kThemeSharing, "theme-export", true, true, "f.cpp", 1});
    REQUIRE(aud.item_count() == 1);
}
TEST_CASE("P11 auditor queries", "[v23][p11]") {
    PluginMarketplaceCompletionAuditor aud;
    aud.add_item({PluginCapabilityArea::kTelemetry, "activation-telemetry", true, true, "f.cpp", 1});
    aud.add_item({PluginCapabilityArea::kTelemetry, "deactivation-telemetry", false, false, "f.cpp", 2});
    auto by_area = aud.items_by_area(PluginCapabilityArea::kTelemetry); REQUIRE(by_area.size() == 2);
    REQUIRE(aud.complete_items().size() == 1); REQUIRE(aud.incomplete_items().size() == 1);
}
TEST_CASE("P11 auditor report", "[v23][p11]") {
    PluginMarketplaceCompletionAuditor aud;
    aud.add_item({PluginCapabilityArea::kContributionFulfillment, "cmds", true, true, "f.cpp", 1});
    aud.add_item({PluginCapabilityArea::kMarketplaceListings, "fetch", false, false, "f.cpp", 2});
    auto r = aud.report(); REQUIRE(r.total == 2); REQUIRE(r.complete == 1); REQUIRE(r.has_gaps());
}
TEST_CASE("P11 auditor clear", "[v23][p11]") {
    PluginMarketplaceCompletionAuditor aud;
    aud.add_item({PluginCapabilityArea::kServiceSurfaces, "grammar", true, true, "f.cpp", 1});
    aud.clear(); REQUIRE(aud.item_count() == 0);
}
TEST_CASE("P11 auditor export json", "[v23][p11]") {
    PluginMarketplaceCompletionAuditor aud;
    aud.add_item({PluginCapabilityArea::kQuarantineRecovery, "quarantine", true, true, "f.cpp", 1});
    auto j = aud.export_json(); REQUIRE(j.find("\"total\": 1") != std::string::npos);
}
TEST_CASE("P11 auditor export markdown", "[v23][p11]") {
    PluginMarketplaceCompletionAuditor aud;
    auto md = aud.export_markdown(); REQUIRE(md.find("Plugin Marketplace") != std::string::npos);
}
TEST_CASE("P11 integration mixed items", "[v23][p11]") {
    PluginMarketplaceCompletionAuditor aud;
    aud.add_item({PluginCapabilityArea::kActivationLifecycle, "lazy-activation", true, true, "f.cpp", 1});
    aud.add_item({PluginCapabilityArea::kCompletionGates, "gate-check", false, true, "f.cpp", 2});
    aud.add_item({PluginCapabilityArea::kThemeSharing, "export-pkg", true, true, "f.cpp", 3});
    auto r = aud.report(); REQUIRE(r.total == 3); REQUIRE(r.complete == 2); REQUIRE(r.incomplete == 1);
}
