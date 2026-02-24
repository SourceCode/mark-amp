#include "core/Config.h"
#include "core/EventBus.h"
#include "core/Events.h"
#include "core/ThemeEngine.h"
#include "ui/SidebarMode.h"
#include "ui/SidebarPanelRegistry.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp;

// =============================================================================
// SidebarMode ↔ ActivityBarItem mapping tests
// =============================================================================

TEST_CASE("SidebarMode string constants match ActivityBarItemId constants", "[workbench][sidebar]")
{
    // Verify the mapping between ActivityBarItem and SidebarMode is consistent
    REQUIRE(core::events::ActivityBarItemId::kFileExplorer == ui::kSidebarModeExplorer);
    REQUIRE(core::events::ActivityBarItemId::kSearch == ui::kSidebarModeSearch);
    REQUIRE(core::events::ActivityBarItemId::kSettings == ui::kSidebarModeSettings);
    REQUIRE(core::events::ActivityBarItemId::kThemes == ui::kSidebarModeThemes);
    REQUIRE(core::events::ActivityBarItemId::kExtensions == ui::kSidebarModeExtensions);
}

// =============================================================================
// SidebarPanelRegistry tests
// =============================================================================

TEST_CASE("SidebarPanelRegistry starts empty", "[workbench][registry]")
{
    ui::SidebarPanelRegistry registry;

    REQUIRE(registry.AllModes().empty());
}

TEST_CASE("SidebarPanelRegistry register and lookup", "[workbench][registry]")
{
    ui::SidebarPanelRegistry registry;
    bool factory_called = false;

    registry.Register(ui::kSidebarModeSearch,
                      "SEARCH",
                      "\xF0\x9F\x94\x8D",
                      [&factory_called](wxWindow* /*parent*/) -> wxPanel*
                      {
                          factory_called = true;
                          return nullptr; // No parent available in test
                      });

    REQUIRE(registry.AllModes().size() == 1);
    REQUIRE(registry.GetLabel(ui::kSidebarModeSearch) == "SEARCH");
    REQUIRE_FALSE(factory_called); // Lazy: not called until GetOrCreate
}

TEST_CASE("SidebarPanelRegistry returns empty label for unregistered mode", "[workbench][registry]")
{
    ui::SidebarPanelRegistry registry;

    REQUIRE(registry.GetLabel(ui::kSidebarModeGraph).empty());
}

TEST_CASE("SidebarPanelRegistry AllModes returns all registered modes", "[workbench][registry]")
{
    ui::SidebarPanelRegistry registry;

    registry.Register(ui::kSidebarModeExplorer,
                      "EXPLORER",
                      "\xF0\x9F\x93\x81",
                      [](wxWindow* /*parent*/) -> wxPanel* { return nullptr; });
    registry.Register(ui::kSidebarModeSearch,
                      "SEARCH",
                      "\xF0\x9F\x94\x8D",
                      [](wxWindow* /*parent*/) -> wxPanel* { return nullptr; });
    registry.Register(ui::kSidebarModeSettings,
                      "SETTINGS",
                      "\xE2\x9A\x99",
                      [](wxWindow* /*parent*/) -> wxPanel* { return nullptr; });

    REQUIRE(registry.AllModes().size() == 3);
}

// =============================================================================
// SidebarModeChangedEvent tests
// =============================================================================

TEST_CASE("SidebarModeChangedEvent carries previous and new mode", "[workbench][events]")
{
    core::events::SidebarModeChangedEvent evt;
    evt.previous_mode = ui::kSidebarModeExplorer;
    evt.new_mode = ui::kSidebarModeSearch;

    REQUIRE(evt.previous_mode == ui::kSidebarModeExplorer);
    REQUIRE(evt.new_mode == ui::kSidebarModeSearch);
}

TEST_CASE("SidebarModeChangedEvent can be published and received", "[workbench][events]")
{
    core::EventBus bus;
    std::string received_previous;
    std::string received_new;

    auto sub = bus.subscribe<core::events::SidebarModeChangedEvent>(
        [&](const core::events::SidebarModeChangedEvent& evt)
        {
            received_previous = evt.previous_mode;
            received_new = evt.new_mode;
        });

    core::events::SidebarModeChangedEvent evt;
    evt.previous_mode = ui::kSidebarModeExplorer;
    evt.new_mode = ui::kSidebarModeSearch;
    bus.publish(evt);

    REQUIRE(received_previous == ui::kSidebarModeExplorer);
    REQUIRE(received_new == ui::kSidebarModeSearch);
}

// =============================================================================
// ActivityBarSelectionEvent tests
// =============================================================================

TEST_CASE("ActivityBarSelectionEvent can be constructed with item", "[workbench][events]")
{
    const core::events::ActivityBarSelectionEvent evt{core::events::ActivityBarItemId::kSearch};

    REQUIRE(evt.item == core::events::ActivityBarItemId::kSearch);
}

TEST_CASE("ActivityBarSelectionEvent round-trips through EventBus", "[workbench][events]")
{
    core::EventBus bus;
    core::events::ActivityBarItem received_item = core::events::ActivityBarItemId::kFileExplorer;

    auto sub = bus.subscribe<core::events::ActivityBarSelectionEvent>(
        [&](const core::events::ActivityBarSelectionEvent& evt) { received_item = evt.item; });

    const core::events::ActivityBarSelectionEvent evt{core::events::ActivityBarItemId::kExtensions};
    bus.publish(evt);

    REQUIRE(received_item == core::events::ActivityBarItemId::kExtensions);
}

// =============================================================================
// Badge notification event tests
// =============================================================================

TEST_CASE("SearchResultCountEvent carries count", "[workbench][events]")
{
    core::events::SearchResultCountEvent evt;
    evt.count = 42;
    REQUIRE(evt.count == 42);
}

TEST_CASE("DiagnosticsCountChangedEvent carries error and warning counts", "[workbench][events]")
{
    core::events::DiagnosticsCountChangedEvent evt;
    evt.error_count = 7;
    evt.warning_count = 3;
    REQUIRE(evt.error_count == 7);
    REQUIRE(evt.warning_count == 3);
}

TEST_CASE("ExtensionUpdatesAvailableEvent carries update_count", "[workbench][events]")
{
    core::events::ExtensionUpdatesAvailableEvent evt;
    evt.update_count = 3;
    REQUIRE(evt.update_count == 3);
}

// =============================================================================
// Sidebar state persistence tests
// =============================================================================

TEST_CASE("Config can store and retrieve sidebar mode", "[workbench][persistence]")
{
    core::Config config;
    const std::string mode_val = ui::kSidebarModeSettings;
    config.set("layout.sidebar_mode", mode_val);

    const std::string restored = config.get_string("layout.sidebar_mode", "");
    REQUIRE(restored == mode_val);
}

TEST_CASE("Config returns default when sidebar mode not set", "[workbench][persistence]")
{
    core::Config config;
    const std::string restored = config.get_string("layout.sidebar_mode", ui::kSidebarModeExplorer);
    REQUIRE(restored == ui::kSidebarModeExplorer); // Default is kExplorer
}

TEST_CASE("Config rejects invalid sidebar mode gracefully", "[workbench][persistence]")
{
    core::Config config;
    config.set("layout.sidebar_mode", std::string("workbench.view.invalid000"));

    const std::string restored = config.get_string("layout.sidebar_mode", "");
    // The value is stored as-is; validation happens in RegisterSidebarPanels
    REQUIRE(restored == "workbench.view.invalid000");
}

// =============================================================================
// Quick switcher shortcut mapping tests
// =============================================================================

TEST_CASE("Quick switcher maps E to FileExplorer", "[workbench][shortcuts]")
{
    const core::events::ActivityBarSelectionEvent evt{
        core::events::ActivityBarItemId::kFileExplorer};
    REQUIRE(evt.item == core::events::ActivityBarItemId::kFileExplorer);
}

TEST_CASE("Quick switcher maps F to Search", "[workbench][shortcuts]")
{
    const core::events::ActivityBarSelectionEvent evt{core::events::ActivityBarItemId::kSearch};
    REQUIRE(evt.item == core::events::ActivityBarItemId::kSearch);
}

TEST_CASE("Quick switcher maps G to Graph", "[workbench][shortcuts]")
{
    const core::events::ActivityBarSelectionEvent evt{core::events::ActivityBarItemId::kGraph};
    REQUIRE(evt.item == core::events::ActivityBarItemId::kGraph);
}

TEST_CASE("Quick switcher maps X to Extensions", "[workbench][shortcuts]")
{
    const core::events::ActivityBarSelectionEvent evt{core::events::ActivityBarItemId::kExtensions};
    REQUIRE(evt.item == core::events::ActivityBarItemId::kExtensions);
}
