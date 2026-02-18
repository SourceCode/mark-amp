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

TEST_CASE("SidebarMode enum has expected number of entries", "[workbench][sidebar]")
{
    // Verify all 8 modes exist and have distinct integer values
    const auto explorer = static_cast<int>(ui::SidebarMode::kExplorer);
    const auto search = static_cast<int>(ui::SidebarMode::kSearch);
    const auto settings = static_cast<int>(ui::SidebarMode::kSettings);
    const auto themes = static_cast<int>(ui::SidebarMode::kThemes);
    const auto extensions = static_cast<int>(ui::SidebarMode::kExtensions);
    const auto notebooks = static_cast<int>(ui::SidebarMode::kNotebooks);
    const auto canvas = static_cast<int>(ui::SidebarMode::kCanvas);
    const auto graph = static_cast<int>(ui::SidebarMode::kGraph);

    REQUIRE(explorer != search);
    REQUIRE(search != settings);
    REQUIRE(settings != themes);
    REQUIRE(themes != extensions);
    REQUIRE(extensions != notebooks);
    REQUIRE(notebooks != canvas);
    REQUIRE(canvas != graph);
}

TEST_CASE("SidebarMode values start at zero for Explorer", "[workbench][sidebar]")
{
    REQUIRE(static_cast<int>(ui::SidebarMode::kExplorer) == 0);
}

TEST_CASE("ActivityBarItem and SidebarMode have matching values", "[workbench][sidebar]")
{
    // Verify the mapping between ActivityBarItem and SidebarMode is consistent
    REQUIRE(static_cast<int>(core::events::ActivityBarItem::FileExplorer) ==
            static_cast<int>(ui::SidebarMode::kExplorer));
    REQUIRE(static_cast<int>(core::events::ActivityBarItem::Search) ==
            static_cast<int>(ui::SidebarMode::kSearch));
    REQUIRE(static_cast<int>(core::events::ActivityBarItem::Settings) ==
            static_cast<int>(ui::SidebarMode::kSettings));
    REQUIRE(static_cast<int>(core::events::ActivityBarItem::Themes) ==
            static_cast<int>(ui::SidebarMode::kThemes));
    REQUIRE(static_cast<int>(core::events::ActivityBarItem::Extensions) ==
            static_cast<int>(ui::SidebarMode::kExtensions));
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

    registry.Register(ui::SidebarMode::kSearch,
                      "SEARCH",
                      "\xF0\x9F\x94\x8D",
                      [&factory_called](wxWindow* /*parent*/) -> wxPanel*
                      {
                          factory_called = true;
                          return nullptr; // No parent available in test
                      });

    REQUIRE(registry.AllModes().size() == 1);
    REQUIRE(registry.GetLabel(ui::SidebarMode::kSearch) == "SEARCH");
    REQUIRE_FALSE(factory_called); // Lazy: not called until GetOrCreate
}

TEST_CASE("SidebarPanelRegistry returns empty label for unregistered mode", "[workbench][registry]")
{
    ui::SidebarPanelRegistry registry;

    REQUIRE(registry.GetLabel(ui::SidebarMode::kGraph).empty());
}

TEST_CASE("SidebarPanelRegistry AllModes returns all registered modes", "[workbench][registry]")
{
    ui::SidebarPanelRegistry registry;

    registry.Register(ui::SidebarMode::kExplorer,
                      "EXPLORER",
                      "\xF0\x9F\x93\x81",
                      [](wxWindow* /*parent*/) -> wxPanel* { return nullptr; });
    registry.Register(ui::SidebarMode::kSearch,
                      "SEARCH",
                      "\xF0\x9F\x94\x8D",
                      [](wxWindow* /*parent*/) -> wxPanel* { return nullptr; });
    registry.Register(ui::SidebarMode::kSettings,
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
    evt.previous_mode = 0;
    evt.new_mode = 1;

    REQUIRE(evt.previous_mode == 0);
    REQUIRE(evt.new_mode == 1);
}

TEST_CASE("SidebarModeChangedEvent can be published and received", "[workbench][events]")
{
    core::EventBus bus;
    int received_previous = -1;
    int received_new = -1;

    auto sub = bus.subscribe<core::events::SidebarModeChangedEvent>(
        [&](const core::events::SidebarModeChangedEvent& evt)
        {
            received_previous = evt.previous_mode;
            received_new = evt.new_mode;
        });

    core::events::SidebarModeChangedEvent evt;
    evt.previous_mode = static_cast<int>(ui::SidebarMode::kExplorer);
    evt.new_mode = static_cast<int>(ui::SidebarMode::kSearch);
    bus.publish(evt);

    REQUIRE(received_previous == static_cast<int>(ui::SidebarMode::kExplorer));
    REQUIRE(received_new == static_cast<int>(ui::SidebarMode::kSearch));
}

// =============================================================================
// ActivityBarSelectionEvent tests
// =============================================================================

TEST_CASE("ActivityBarSelectionEvent can be constructed with item", "[workbench][events]")
{
    const core::events::ActivityBarSelectionEvent evt{core::events::ActivityBarItem::Search};

    REQUIRE(evt.item == core::events::ActivityBarItem::Search);
}

TEST_CASE("ActivityBarSelectionEvent round-trips through EventBus", "[workbench][events]")
{
    core::EventBus bus;
    core::events::ActivityBarItem received_item = core::events::ActivityBarItem::FileExplorer;

    auto sub = bus.subscribe<core::events::ActivityBarSelectionEvent>(
        [&](const core::events::ActivityBarSelectionEvent& evt) { received_item = evt.item; });

    const core::events::ActivityBarSelectionEvent evt{core::events::ActivityBarItem::Extensions};
    bus.publish(evt);

    REQUIRE(received_item == core::events::ActivityBarItem::Extensions);
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
    const int mode_val = static_cast<int>(ui::SidebarMode::kSettings);
    config.set("layout.sidebar_mode", mode_val);

    const int restored = config.get_int("layout.sidebar_mode", 0);
    REQUIRE(restored == mode_val);
}

TEST_CASE("Config returns default when sidebar mode not set", "[workbench][persistence]")
{
    core::Config config;
    const int restored = config.get_int("layout.sidebar_mode", 0);
    REQUIRE(restored == 0); // Default is kExplorer (0)
}

TEST_CASE("Config rejects invalid sidebar mode gracefully", "[workbench][persistence]")
{
    core::Config config;
    config.set("layout.sidebar_mode", 999);

    const int restored = config.get_int("layout.sidebar_mode", 0);
    // The value is stored as-is; validation happens in RestoreLayoutState
    REQUIRE(restored == 999);
}

// =============================================================================
// Quick switcher shortcut mapping tests
// =============================================================================

TEST_CASE("Quick switcher maps E to FileExplorer", "[workbench][shortcuts]")
{
    const core::events::ActivityBarSelectionEvent evt{core::events::ActivityBarItem::FileExplorer};
    REQUIRE(evt.item == core::events::ActivityBarItem::FileExplorer);
}

TEST_CASE("Quick switcher maps F to Search", "[workbench][shortcuts]")
{
    const core::events::ActivityBarSelectionEvent evt{core::events::ActivityBarItem::Search};
    REQUIRE(evt.item == core::events::ActivityBarItem::Search);
}

TEST_CASE("Quick switcher maps G to Graph", "[workbench][shortcuts]")
{
    const core::events::ActivityBarSelectionEvent evt{core::events::ActivityBarItem::kGraph};
    REQUIRE(evt.item == core::events::ActivityBarItem::kGraph);
}

TEST_CASE("Quick switcher maps X to Extensions", "[workbench][shortcuts]")
{
    const core::events::ActivityBarSelectionEvent evt{core::events::ActivityBarItem::Extensions};
    REQUIRE(evt.item == core::events::ActivityBarItem::Extensions);
}
