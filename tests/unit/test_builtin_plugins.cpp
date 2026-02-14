#include "core/BuiltInPlugins.h"
#include "core/Config.h"
#include "core/EventBus.h"
#include "core/Events.h"
#include "core/FeatureRegistry.h"
#include "core/IPlugin.h"
#include "core/PluginManager.h"

#include <catch2/catch_test_macros.hpp>

#include <algorithm>
#include <string>
#include <vector>

using namespace markamp::core;
using namespace markamp::core::events;

// ── Helpers ──────────────────────────────────────────────────────────────

namespace
{

/// The 7 well-known built-in feature IDs from BuiltInPlugins.h.
const std::vector<std::string> kAllBuiltInFeatureIds = {
    builtin_features::kMermaid,
    builtin_features::kTableEditor,
    builtin_features::kFormatBar,
    builtin_features::kThemeGallery,
    builtin_features::kLinkPreview,
    builtin_features::kImagePreview,
    builtin_features::kBreadcrumb,
};

/// The 7 well-known built-in plugin manifest IDs.
const std::vector<std::string> kAllBuiltInPluginIds = {
    "markamp.mermaid",
    "markamp.table-editor",
    "markamp.format-bar",
    "markamp.theme-gallery",
    "markamp.link-preview",
    "markamp.image-preview",
    "markamp.breadcrumb",
};

} // anonymous namespace

// ── Registration Tests ───────────────────────────────────────────────────

TEST_CASE("register_builtin_plugins: registers 7 features", "[builtin-plugins]")
{
    EventBus bus;
    Config cfg;
    FeatureRegistry registry(bus, cfg);
    PluginManager mgr(bus, cfg);

    REQUIRE(registry.feature_count() == 0);
    REQUIRE(mgr.plugin_count() == 0);

    register_builtin_plugins(mgr, registry);

    REQUIRE(registry.feature_count() == 7);
    REQUIRE(mgr.plugin_count() == 7);
}

TEST_CASE("register_builtin_plugins: all feature IDs are present", "[builtin-plugins]")
{
    EventBus bus;
    Config cfg;
    FeatureRegistry registry(bus, cfg);
    PluginManager mgr(bus, cfg);

    register_builtin_plugins(mgr, registry);

    for (const auto& feature_id : kAllBuiltInFeatureIds)
    {
        INFO("Checking feature: " << feature_id);
        auto* info = registry.get_feature(feature_id);
        REQUIRE(info != nullptr);
        REQUIRE_FALSE(info->display_name.empty());
        REQUIRE_FALSE(info->description.empty());
    }
}

TEST_CASE("register_builtin_plugins: all features default enabled", "[builtin-plugins]")
{
    EventBus bus;
    Config cfg;
    FeatureRegistry registry(bus, cfg);
    PluginManager mgr(bus, cfg);

    register_builtin_plugins(mgr, registry);

    for (const auto& feature_id : kAllBuiltInFeatureIds)
    {
        INFO("Checking feature: " << feature_id);
        REQUIRE(registry.is_enabled(feature_id));
    }
}

TEST_CASE("register_builtin_plugins: feature toggle persists via Config", "[builtin-plugins]")
{
    EventBus bus;
    Config cfg;
    FeatureRegistry registry(bus, cfg);
    PluginManager mgr(bus, cfg);

    register_builtin_plugins(mgr, registry);

    // Disable mermaid feature
    registry.disable(builtin_features::kMermaid);
    REQUIRE_FALSE(registry.is_enabled(builtin_features::kMermaid));

    // Verify Config was updated
    REQUIRE(cfg.get_bool("feature.mermaid.enabled", true) == false);

    // Create a fresh registry from the same Config — it should pick up the
    // persisted state.
    FeatureRegistry registry2(bus, cfg);
    PluginManager mgr2(bus, cfg);
    register_builtin_plugins(mgr2, registry2);

    REQUIRE_FALSE(registry2.is_enabled(builtin_features::kMermaid));
    // Others remain enabled
    REQUIRE(registry2.is_enabled(builtin_features::kTableEditor));
    REQUIRE(registry2.is_enabled(builtin_features::kBreadcrumb));
}

// ── Plugin Manifest Tests ────────────────────────────────────────────────

TEST_CASE("register_builtin_plugins: plugin manifests are well-formed", "[builtin-plugins]")
{
    EventBus bus;
    Config cfg;
    FeatureRegistry registry(bus, cfg);
    PluginManager mgr(bus, cfg);

    register_builtin_plugins(mgr, registry);
    mgr.activate_all();

    for (const auto& plugin_id : kAllBuiltInPluginIds)
    {
        INFO("Checking plugin: " << plugin_id);
        REQUIRE(mgr.is_plugin_active(plugin_id));
    }
}

TEST_CASE("register_builtin_plugins: activate then deactivate all", "[builtin-plugins]")
{
    EventBus bus;
    Config cfg;
    FeatureRegistry registry(bus, cfg);
    PluginManager mgr(bus, cfg);

    register_builtin_plugins(mgr, registry);
    mgr.activate_all();

    for (const auto& plugin_id : kAllBuiltInPluginIds)
    {
        REQUIRE(mgr.is_plugin_active(plugin_id));
    }

    mgr.deactivate_all();

    for (const auto& plugin_id : kAllBuiltInPluginIds)
    {
        REQUIRE_FALSE(mgr.is_plugin_active(plugin_id));
    }
}

// ── Feature Toggle Event Tests ───────────────────────────────────────────

TEST_CASE("register_builtin_plugins: disabling a feature fires FeatureToggledEvent",
          "[builtin-plugins]")
{
    EventBus bus;
    Config cfg;
    FeatureRegistry registry(bus, cfg);
    PluginManager mgr(bus, cfg);

    register_builtin_plugins(mgr, registry);

    std::vector<std::pair<std::string, bool>> events_received;
    auto sub = bus.subscribe<FeatureToggledEvent>(
        [&](const FeatureToggledEvent& evt)
        { events_received.emplace_back(evt.feature_id, evt.enabled); });

    registry.disable(builtin_features::kBreadcrumb);

    REQUIRE(events_received.size() == 1);
    REQUIRE(events_received[0].first == "breadcrumb");
    REQUIRE(events_received[0].second == false);

    registry.enable(builtin_features::kBreadcrumb);

    REQUIRE(events_received.size() == 2);
    REQUIRE(events_received[1].first == "breadcrumb");
    REQUIRE(events_received[1].second == true);
}

TEST_CASE("register_builtin_plugins: toggling multiple features fires events correctly",
          "[builtin-plugins]")
{
    EventBus bus;
    Config cfg;
    FeatureRegistry registry(bus, cfg);
    PluginManager mgr(bus, cfg);

    register_builtin_plugins(mgr, registry);

    int event_count = 0;
    auto sub = bus.subscribe<FeatureToggledEvent>([&](const FeatureToggledEvent& /*evt*/)
                                                  { ++event_count; });

    // Disable 3 features
    registry.disable(builtin_features::kMermaid);
    registry.disable(builtin_features::kLinkPreview);
    registry.disable(builtin_features::kImagePreview);

    REQUIRE(event_count == 3);

    // Re-enable all 3
    registry.enable(builtin_features::kMermaid);
    registry.enable(builtin_features::kLinkPreview);
    registry.enable(builtin_features::kImagePreview);

    REQUIRE(event_count == 6);
}

// ── Feature Constants Tests ──────────────────────────────────────────────

TEST_CASE("builtin_features constants: unique IDs", "[builtin-plugins]")
{
    // Verify all feature IDs are distinct
    std::vector<std::string> ids = kAllBuiltInFeatureIds;
    std::sort(ids.begin(), ids.end());
    auto last = std::unique(ids.begin(), ids.end());
    REQUIRE(last == ids.end()); // No duplicates
}

// ── Integration: PluginManager + FeatureRegistry ─────────────────────────

TEST_CASE("register_builtin_plugins: double registration is idempotent or rejected",
          "[builtin-plugins]")
{
    EventBus bus;
    Config cfg;
    FeatureRegistry registry(bus, cfg);
    PluginManager mgr(bus, cfg);

    register_builtin_plugins(mgr, registry);
    REQUIRE(registry.feature_count() == 7);
    REQUIRE(mgr.plugin_count() == 7);

    // Second registration: feature registry ignores duplicates,
    // plugin manager should reject duplicate IDs
    register_builtin_plugins(mgr, registry);
    REQUIRE(registry.feature_count() == 7); // No duplicates
    REQUIRE(mgr.plugin_count() == 7);       // No duplicates
}
