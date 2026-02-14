#include "core/Config.h"
#include "core/EventBus.h"
#include "core/Events.h"
#include "core/FeatureRegistry.h"

#include <catch2/catch_test_macros.hpp>

#include <string>
#include <vector>

using namespace markamp::core;
using namespace markamp::core::events;

// ── Helpers ──────────────────────────────────────────────────────────────

/// Construct a FeatureInfo with sensible defaults.
static auto make_feature(const std::string& feature_id,
                         const std::string& display_name,
                         bool default_enabled = true) -> FeatureInfo
{
    return {feature_id, display_name, "Test feature", default_enabled};
}

// ─────────────────────────────────────────────────────────────────────────
// Test cases
// ─────────────────────────────────────────────────────────────────────────

TEST_CASE("FeatureRegistry: register and query features", "[feature_registry]")
{
    EventBus bus;
    Config config;
    FeatureRegistry registry(bus, config);

    SECTION("empty registry returns no features")
    {
        REQUIRE(registry.feature_count() == 0);
        REQUIRE(registry.get_all_features().empty());
        REQUIRE_FALSE(registry.is_enabled("nonexistent"));
    }

    SECTION("register a feature with default enabled")
    {
        registry.register_feature(make_feature("mermaid", "Mermaid Diagrams", true));

        REQUIRE(registry.feature_count() == 1);
        REQUIRE(registry.is_enabled("mermaid"));

        const auto* info = registry.get_feature("mermaid");
        REQUIRE(info != nullptr);
        REQUIRE(info->id == "mermaid");
        REQUIRE(info->display_name == "Mermaid Diagrams");
        REQUIRE(info->default_enabled == true);
    }

    SECTION("register a feature with default disabled")
    {
        registry.register_feature(make_feature("experimental", "Experimental", false));

        REQUIRE(registry.feature_count() == 1);
        REQUIRE_FALSE(registry.is_enabled("experimental"));
    }

    SECTION("register multiple features")
    {
        registry.register_feature(make_feature("mermaid", "Mermaid"));
        registry.register_feature(make_feature("table-editor", "Table Editor"));
        registry.register_feature(make_feature("format-bar", "Format Bar"));

        REQUIRE(registry.feature_count() == 3);
        REQUIRE(registry.is_enabled("mermaid"));
        REQUIRE(registry.is_enabled("table-editor"));
        REQUIRE(registry.is_enabled("format-bar"));

        const auto all = registry.get_all_features();
        REQUIRE(all.size() == 3);
    }

    SECTION("duplicate registration is ignored")
    {
        registry.register_feature(make_feature("mermaid", "Mermaid"));
        registry.register_feature(make_feature("mermaid", "Mermaid v2"));

        REQUIRE(registry.feature_count() == 1);
        REQUIRE(registry.get_feature("mermaid")->display_name == "Mermaid");
    }

    SECTION("get_feature returns nullptr for unknown ID")
    {
        REQUIRE(registry.get_feature("unknown") == nullptr);
    }
}

TEST_CASE("FeatureRegistry: enable and disable features", "[feature_registry]")
{
    EventBus bus;
    Config config;
    FeatureRegistry registry(bus, config);

    registry.register_feature(make_feature("mermaid", "Mermaid", true));
    registry.register_feature(make_feature("experimental", "Experimental", false));

    SECTION("disable an enabled feature")
    {
        REQUIRE(registry.is_enabled("mermaid"));
        registry.disable("mermaid");
        REQUIRE_FALSE(registry.is_enabled("mermaid"));
    }

    SECTION("enable a disabled feature")
    {
        REQUIRE_FALSE(registry.is_enabled("experimental"));
        registry.enable("experimental");
        REQUIRE(registry.is_enabled("experimental"));
    }

    SECTION("enable already enabled is no-op")
    {
        REQUIRE(registry.is_enabled("mermaid"));
        registry.enable("mermaid"); // no-op
        REQUIRE(registry.is_enabled("mermaid"));
    }

    SECTION("disable already disabled is no-op")
    {
        REQUIRE_FALSE(registry.is_enabled("experimental"));
        registry.disable("experimental"); // no-op
        REQUIRE_FALSE(registry.is_enabled("experimental"));
    }

    SECTION("enable/disable unknown feature is no-op")
    {
        registry.enable("nonexistent");
        registry.disable("nonexistent");
        REQUIRE(registry.feature_count() == 2);
    }
}

TEST_CASE("FeatureRegistry: toggle features", "[feature_registry]")
{
    EventBus bus;
    Config config;
    FeatureRegistry registry(bus, config);

    registry.register_feature(make_feature("mermaid", "Mermaid", true));

    SECTION("toggle enabled → disabled → enabled")
    {
        REQUIRE(registry.is_enabled("mermaid"));

        registry.toggle("mermaid");
        REQUIRE_FALSE(registry.is_enabled("mermaid"));

        registry.toggle("mermaid");
        REQUIRE(registry.is_enabled("mermaid"));
    }

    SECTION("toggle unknown feature is no-op")
    {
        registry.toggle("nonexistent");
        REQUIRE(registry.feature_count() == 1);
    }
}

TEST_CASE("FeatureRegistry: set_enabled explicit", "[feature_registry]")
{
    EventBus bus;
    Config config;
    FeatureRegistry registry(bus, config);

    registry.register_feature(make_feature("mermaid", "Mermaid", true));

    SECTION("set_enabled(false)")
    {
        registry.set_enabled("mermaid", false);
        REQUIRE_FALSE(registry.is_enabled("mermaid"));
    }

    SECTION("set_enabled(true) on already true is no-op event-wise")
    {
        bool event_fired = false;
        auto sub = bus.subscribe<FeatureToggledEvent>([&](const FeatureToggledEvent& /*evt*/)
                                                      { event_fired = true; });

        registry.set_enabled("mermaid", true); // same as current, no-op
        REQUIRE_FALSE(event_fired);
    }
}

TEST_CASE("FeatureRegistry: fires FeatureToggledEvent", "[feature_registry]")
{
    EventBus bus;
    Config config;
    FeatureRegistry registry(bus, config);

    registry.register_feature(make_feature("mermaid", "Mermaid", true));

    std::vector<std::pair<std::string, bool>> events_received;
    auto sub = bus.subscribe<FeatureToggledEvent>(
        [&](const FeatureToggledEvent& evt)
        { events_received.emplace_back(evt.feature_id, evt.enabled); });

    SECTION("disable fires event with enabled=false")
    {
        registry.disable("mermaid");
        REQUIRE(events_received.size() == 1);
        REQUIRE(events_received[0].first == "mermaid");
        REQUIRE(events_received[0].second == false);
    }

    SECTION("enable fires event with enabled=true")
    {
        registry.disable("mermaid");
        events_received.clear();

        registry.enable("mermaid");
        REQUIRE(events_received.size() == 1);
        REQUIRE(events_received[0].first == "mermaid");
        REQUIRE(events_received[0].second == true);
    }

    SECTION("toggle fires event each time")
    {
        registry.toggle("mermaid"); // true → false
        registry.toggle("mermaid"); // false → true

        REQUIRE(events_received.size() == 2);
        REQUIRE(events_received[0].second == false);
        REQUIRE(events_received[1].second == true);
    }

    SECTION("no event fired for no-op operations")
    {
        registry.enable("mermaid"); // already enabled, no-op
        REQUIRE(events_received.empty());

        registry.enable("nonexistent"); // unknown
        REQUIRE(events_received.empty());
    }
}

TEST_CASE("FeatureRegistry: persists state via Config", "[feature_registry]")
{
    EventBus bus;
    Config config;

    SECTION("disabling a feature writes to Config")
    {
        FeatureRegistry registry(bus, config);
        registry.register_feature(make_feature("mermaid", "Mermaid", true));

        registry.disable("mermaid");

        // Config should now have feature.mermaid.enabled = false
        REQUIRE(config.get_bool("feature.mermaid.enabled", true) == false);
    }

    SECTION("enabling a feature writes to Config")
    {
        FeatureRegistry registry(bus, config);
        registry.register_feature(make_feature("experimental", "Experimental", false));

        registry.enable("experimental");

        REQUIRE(config.get_bool("feature.experimental.enabled", false) == true);
    }

    SECTION("new registry picks up persisted state from Config")
    {
        // First registry: disable mermaid
        {
            FeatureRegistry registry1(bus, config);
            registry1.register_feature(make_feature("mermaid", "Mermaid", true));
            registry1.disable("mermaid");
        }

        // Second registry: should read the disabled state from Config
        {
            FeatureRegistry registry2(bus, config);
            registry2.register_feature(make_feature("mermaid", "Mermaid", true));
            REQUIRE_FALSE(registry2.is_enabled("mermaid"));
        }
    }
}
