/// @file test_builtin_plugin_behavior.cpp
/// Tests that each built-in plugin registers commands and integrates with FeatureRegistry.

#include "core/BuiltInPlugins.h"
#include "core/Config.h"
#include "core/EventBus.h"
#include "core/FeatureRegistry.h"
#include "core/PluginManager.h"

#include <catch2/catch_test_macros.hpp>

#include <string>
#include <vector>

using namespace markamp::core;

// ── Helpers ─────────────────────────────────────────────────────────────────

static constexpr std::string_view kAllFeatureIds[] = {
    builtin_features::kMermaid,
    builtin_features::kTableEditor,
    builtin_features::kFormatBar,
    builtin_features::kThemeGallery,
    builtin_features::kLinkPreview,
    builtin_features::kImagePreview,
    builtin_features::kBreadcrumb,
};

// ── Registration tests ──────────────────────────────────────────────────────

TEST_CASE("BuiltInPlugins: all 7 plugins register", "[builtin][plugins]")
{
    EventBus bus;
    Config config;
    PluginManager pm(bus, config);
    FeatureRegistry features(bus, config);

    register_builtin_plugins(pm, features);

    REQUIRE(pm.plugin_count() == 7);
    REQUIRE(features.feature_count() == 7);
}

TEST_CASE("BuiltInPlugins: all features enabled by default", "[builtin][plugins]")
{
    EventBus bus;
    Config config;
    PluginManager pm(bus, config);
    FeatureRegistry features(bus, config);

    register_builtin_plugins(pm, features);

    for (const auto& id : kAllFeatureIds)
    {
        INFO("Feature: " << id);
        REQUIRE(features.is_enabled(std::string(id)));
    }
}

// ── Activation tests ────────────────────────────────────────────────────────

TEST_CASE("BuiltInPlugins: activate_all succeeds", "[builtin][plugins]")
{
    EventBus bus;
    Config config;
    PluginManager pm(bus, config);
    FeatureRegistry features(bus, config);

    register_builtin_plugins(pm, features);
    pm.activate_all();

    // Plugin IDs use the "markamp." prefix, not the bare feature IDs
    static constexpr std::string_view kPluginIds[] = {
        "markamp.mermaid",
        "markamp.table-editor",
        "markamp.format-bar",
        "markamp.theme-gallery",
        "markamp.link-preview",
        "markamp.image-preview",
        "markamp.breadcrumb",
    };

    for (const auto& id : kPluginIds)
    {
        INFO("Plugin: " << id);
        REQUIRE(pm.is_plugin_active(std::string(id)));
    }
}

// ── Feature toggle tests ────────────────────────────────────────────────────

TEST_CASE("BuiltInPlugins: features toggle correctly", "[builtin][plugins]")
{
    EventBus bus;
    Config config;
    PluginManager pm(bus, config);
    FeatureRegistry features(bus, config);

    register_builtin_plugins(pm, features);

    SECTION("disable a feature")
    {
        features.disable(builtin_features::kMermaid);
        REQUIRE_FALSE(features.is_enabled(builtin_features::kMermaid));
    }

    SECTION("toggle round-trip")
    {
        features.toggle(builtin_features::kTableEditor);
        REQUIRE_FALSE(features.is_enabled(builtin_features::kTableEditor));

        features.toggle(builtin_features::kTableEditor);
        REQUIRE(features.is_enabled(builtin_features::kTableEditor));
    }

    SECTION("disable does not affect other features")
    {
        features.disable(builtin_features::kFormatBar);

        REQUIRE(features.is_enabled(builtin_features::kMermaid));
        REQUIRE(features.is_enabled(builtin_features::kThemeGallery));
        REQUIRE_FALSE(features.is_enabled(builtin_features::kFormatBar));
    }
}

// ── FeatureToggledEvent integration ─────────────────────────────────────────

TEST_CASE("BuiltInPlugins: toggling fires FeatureToggledEvent", "[builtin][plugins]")
{
    EventBus bus;
    Config config;
    PluginManager pm(bus, config);
    FeatureRegistry features(bus, config);

    register_builtin_plugins(pm, features);

    std::vector<std::pair<std::string, bool>> toggle_events;
    auto sub = bus.subscribe<events::FeatureToggledEvent>(
        [&](const events::FeatureToggledEvent& evt)
        { toggle_events.emplace_back(evt.feature_id, evt.enabled); });

    features.disable(builtin_features::kLinkPreview);
    features.enable(builtin_features::kLinkPreview);

    REQUIRE(toggle_events.size() == 2);
    REQUIRE(toggle_events[0].first == builtin_features::kLinkPreview);
    REQUIRE(toggle_events[0].second == false);
    REQUIRE(toggle_events[1].first == builtin_features::kLinkPreview);
    REQUIRE(toggle_events[1].second == true);
}
