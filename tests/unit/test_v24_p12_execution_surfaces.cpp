/// @file test_v24_p12_execution_surfaces.cpp
/// @brief V24 Phase 12 tests: Build, terminal, debug, feature gating.
#include <catch2/catch_test_macros.hpp>
#include "../../src/core/FeatureRegistry.h"
#include "../../src/core/EventBus.h"
#include "../../src/core/Config.h"

using namespace markamp::core;

// P12-T01: Feature info structure
TEST_CASE("P12-T01 feature info defaults", "[v24][p12]") {
    FeatureInfo info;
    info.id = "terminal";
    info.display_name = "Terminal Integration";
    info.description = "Integrated terminal";
    info.default_enabled = false;
    REQUIRE(info.id == "terminal");
    REQUIRE_FALSE(info.default_enabled);
}

// P12-T02: Feature registry
TEST_CASE("P12-T02 feature registry register and query", "[v24][p12]") {
    EventBus bus;
    Config config;
    FeatureRegistry registry(bus, config);

    FeatureInfo f1;
    f1.id = "terminal";
    f1.display_name = "Terminal";
    f1.default_enabled = true;
    registry.register_feature(f1);

    REQUIRE(registry.feature_count() == 1);
    REQUIRE(registry.is_enabled("terminal"));
}

TEST_CASE("P12-T02 feature registry enable disable", "[v24][p12]") {
    EventBus bus;
    Config config;
    FeatureRegistry registry(bus, config);

    FeatureInfo f;
    f.id = "debug";
    f.display_name = "Debug Console";
    f.default_enabled = true;
    registry.register_feature(f);

    registry.disable("debug");
    REQUIRE_FALSE(registry.is_enabled("debug"));
    registry.enable("debug");
    REQUIRE(registry.is_enabled("debug"));
}

// P12-T03: Feature gating
TEST_CASE("P12-T03 feature toggle", "[v24][p12]") {
    EventBus bus;
    Config config;
    FeatureRegistry registry(bus, config);

    FeatureInfo f;
    f.id = "build_output";
    f.display_name = "Build Output";
    f.default_enabled = true;
    registry.register_feature(f);

    registry.toggle("build_output");
    REQUIRE_FALSE(registry.is_enabled("build_output"));
    registry.toggle("build_output");
    REQUIRE(registry.is_enabled("build_output"));
}

TEST_CASE("P12-T03 unknown feature returns false", "[v24][p12]") {
    EventBus bus;
    Config config;
    FeatureRegistry registry(bus, config);
    REQUIRE_FALSE(registry.is_enabled("nonexistent"));
}

// P12-T04: Feature listing
TEST_CASE("P12-T04 get all features", "[v24][p12]") {
    EventBus bus;
    Config config;
    FeatureRegistry registry(bus, config);

    FeatureInfo f1; f1.id = "a"; f1.display_name = "A"; f1.default_enabled = true;
    FeatureInfo f2; f2.id = "b"; f2.display_name = "B"; f2.default_enabled = false;
    registry.register_feature(f1);
    registry.register_feature(f2);

    auto all = registry.get_all_features();
    REQUIRE(all.size() == 2);
}

TEST_CASE("P12-T04 get enabled features", "[v24][p12]") {
    EventBus bus;
    Config config;
    FeatureRegistry registry(bus, config);

    FeatureInfo f1; f1.id = "a"; f1.display_name = "A"; f1.default_enabled = true;
    FeatureInfo f2; f2.id = "b"; f2.display_name = "B"; f2.default_enabled = false;
    registry.register_feature(f1);
    registry.register_feature(f2);

    auto enabled = registry.get_enabled_features();
    REQUIRE(enabled.size() == 1);
    REQUIRE(enabled[0].id == "a");
}

// P12-T05: Feature lookup
TEST_CASE("P12-T05 get single feature info", "[v24][p12]") {
    EventBus bus;
    Config config;
    FeatureRegistry registry(bus, config);

    FeatureInfo f; f.id = "mermaid"; f.display_name = "Mermaid Diagrams"; f.default_enabled = true;
    registry.register_feature(f);

    auto* found = registry.get_feature("mermaid");
    REQUIRE(found != nullptr);
    REQUIRE(found->display_name == "Mermaid Diagrams");
}

TEST_CASE("P12-T05 get unknown feature returns null", "[v24][p12]") {
    EventBus bus;
    Config config;
    FeatureRegistry registry(bus, config);
    REQUIRE(registry.get_feature("missing") == nullptr);
}
