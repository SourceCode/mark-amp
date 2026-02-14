/// @file test_feature_guard_integration.cpp
/// Verifies FeatureRegistry guards and listener notifications.

#include "core/Config.h"
#include "core/EventBus.h"
#include "core/Events.h"
#include "core/FeatureRegistry.h"

#include <catch2/catch_test_macros.hpp>

#include <string>
#include <vector>

using namespace markamp::core;
using namespace markamp::core::events;

// ── Guard behavior tests ────────────────────────────────────────────────────

TEST_CASE("FeatureGuard: enabled feature passes guard", "[feature_registry][guard]")
{
    EventBus bus;
    Config config;
    FeatureRegistry registry(bus, config);

    registry.register_feature({"render-mermaid", "Mermaid Rendering", "Renders diagrams", true});

    REQUIRE(registry.is_enabled("render-mermaid"));
}

TEST_CASE("FeatureGuard: disabled feature does not pass guard", "[feature_registry][guard]")
{
    EventBus bus;
    Config config;
    FeatureRegistry registry(bus, config);

    registry.register_feature({"experimental", "Experimental", "Beta feature", false});

    REQUIRE_FALSE(registry.is_enabled("experimental"));
}

TEST_CASE("FeatureGuard: guard reflects runtime state changes", "[feature_registry][guard]")
{
    EventBus bus;
    Config config;
    FeatureRegistry registry(bus, config);

    registry.register_feature({"dynamic-feature", "Dynamic", "Toggleable", true});

    REQUIRE(registry.is_enabled("dynamic-feature"));

    registry.disable("dynamic-feature");
    REQUIRE_FALSE(registry.is_enabled("dynamic-feature"));

    registry.enable("dynamic-feature");
    REQUIRE(registry.is_enabled("dynamic-feature"));
}

// ── Listener notification tests ─────────────────────────────────────────────

TEST_CASE("FeatureGuard: listener notified on state change", "[feature_registry][guard]")
{
    EventBus bus;
    Config config;
    FeatureRegistry registry(bus, config);

    registry.register_feature({"watched", "Watched Feature", "Test", true});

    std::vector<std::pair<std::string, bool>> notifications;
    auto sub = bus.subscribe<FeatureToggledEvent>(
        [&](const FeatureToggledEvent& evt)
        { notifications.emplace_back(evt.feature_id, evt.enabled); });

    SECTION("disable fires notification with enabled=false")
    {
        registry.disable("watched");
        REQUIRE(notifications.size() == 1);
        REQUIRE(notifications[0].first == "watched");
        REQUIRE(notifications[0].second == false);
    }

    SECTION("enable fires notification with enabled=true")
    {
        registry.disable("watched");
        notifications.clear();

        registry.enable("watched");
        REQUIRE(notifications.size() == 1);
        REQUIRE(notifications[0].first == "watched");
        REQUIRE(notifications[0].second == true);
    }

    SECTION("no notification on no-op enable")
    {
        registry.enable("watched"); // already enabled
        REQUIRE(notifications.empty());
    }

    SECTION("no notification on no-op disable")
    {
        registry.disable("watched");
        notifications.clear();

        registry.disable("watched"); // already disabled
        REQUIRE(notifications.empty());
    }
}

// ── Persistence integration ─────────────────────────────────────────────────

TEST_CASE("FeatureGuard: config persistence round-trip", "[feature_registry][guard]")
{
    EventBus bus;
    Config config;

    // First registry: disable a feature
    {
        FeatureRegistry reg1(bus, config);
        reg1.register_feature({"persist-test", "Persist Test", "Test", true});
        REQUIRE(reg1.is_enabled("persist-test"));

        reg1.disable("persist-test");
        REQUIRE_FALSE(reg1.is_enabled("persist-test"));
    }

    // Second registry: reads persisted state
    {
        FeatureRegistry reg2(bus, config);
        reg2.register_feature({"persist-test", "Persist Test", "Test", true});

        // Despite default=true, config should override to disabled
        REQUIRE_FALSE(reg2.is_enabled("persist-test"));
    }
}

// ── Multiple features ───────────────────────────────────────────────────────

TEST_CASE("FeatureGuard: independent feature guards", "[feature_registry][guard]")
{
    EventBus bus;
    Config config;
    FeatureRegistry registry(bus, config);

    registry.register_feature({"feat-a", "Feature A", "Test A", true});
    registry.register_feature({"feat-b", "Feature B", "Test B", true});
    registry.register_feature({"feat-c", "Feature C", "Test C", false});

    REQUIRE(registry.is_enabled("feat-a"));
    REQUIRE(registry.is_enabled("feat-b"));
    REQUIRE_FALSE(registry.is_enabled("feat-c"));

    // Disabling one does not affect others
    registry.disable("feat-a");
    REQUIRE_FALSE(registry.is_enabled("feat-a"));
    REQUIRE(registry.is_enabled("feat-b"));
    REQUIRE_FALSE(registry.is_enabled("feat-c"));
}
