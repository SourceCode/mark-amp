/// @file test_workspace_layout.cpp
/// @brief V4 Phase 21 – WorkspaceLayoutManager unit tests.

#include "core/Config.h"
#include "core/EventBus.h"
#include "core/ThemeEngine.h"
#include "core/ThemeRegistry.h"
#include "ui/WorkspaceLayout.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::ui;
using namespace markamp::core;

namespace
{

struct TestFixture
{
    EventBus event_bus;
    Config config;
    ThemeRegistry theme_registry;
    ThemeEngine theme_engine{event_bus, theme_registry};
    WorkspaceLayoutManager manager{event_bus, config, theme_engine};
};

} // anonymous namespace

// ---------------------------------------------------------------------------
// Test 1: Default layout applied on init
// ---------------------------------------------------------------------------
TEST_CASE("WorkspaceLayout: default layout applied on init", "[workspace_layout]")
{
    TestFixture fixture;
    const auto& layout = fixture.manager.current_layout();
    REQUIRE_FALSE(layout.empty());

    // Default preset includes file_tree
    auto file_tree = fixture.manager.get_panel_layout("file_tree");
    REQUIRE(file_tree.has_value());
    CHECK(file_tree->visible);
    CHECK(file_tree->position == PanelPosition::kLeftSidebar);
}

// ---------------------------------------------------------------------------
// Test 2: Apply writing preset
// ---------------------------------------------------------------------------
TEST_CASE("WorkspaceLayout: apply writing preset", "[workspace_layout]")
{
    TestFixture fixture;
    REQUIRE(fixture.manager.apply_preset("Writing"));

    // Writing preset hides file_tree, shows outline
    auto file_tree = fixture.manager.get_panel_layout("file_tree");
    REQUIRE(file_tree.has_value());
    CHECK_FALSE(file_tree->visible);

    auto outline = fixture.manager.get_panel_layout("outline");
    REQUIRE(outline.has_value());
    CHECK(outline->visible);
    CHECK(outline->position == PanelPosition::kRightSidebar);
}

// ---------------------------------------------------------------------------
// Test 3: Toggle panel visibility
// ---------------------------------------------------------------------------
TEST_CASE("WorkspaceLayout: toggle panel", "[workspace_layout]")
{
    TestFixture fixture;

    auto panel = fixture.manager.get_panel_layout("backlinks");
    REQUIRE(panel.has_value());
    bool was_visible = panel->visible;

    fixture.manager.toggle_panel("backlinks");

    auto toggled = fixture.manager.get_panel_layout("backlinks");
    REQUIRE(toggled.has_value());
    CHECK(toggled->visible != was_visible);
}

// ---------------------------------------------------------------------------
// Test 4: Move panel to right sidebar
// ---------------------------------------------------------------------------
TEST_CASE("WorkspaceLayout: move panel", "[workspace_layout]")
{
    TestFixture fixture;

    fixture.manager.move_panel("file_tree", PanelPosition::kRightSidebar, 0);

    auto panel = fixture.manager.get_panel_layout("file_tree");
    REQUIRE(panel.has_value());
    CHECK(panel->position == PanelPosition::kRightSidebar);
    CHECK(panel->order == 0);
}

// ---------------------------------------------------------------------------
// Test 5: Save custom preset
// ---------------------------------------------------------------------------
TEST_CASE("WorkspaceLayout: save custom preset", "[workspace_layout]")
{
    TestFixture fixture;
    auto presets_before = fixture.manager.list_presets();

    fixture.manager.save_preset("My Layout", "Custom test layout");

    auto presets_after = fixture.manager.list_presets();
    CHECK(presets_after.size() == presets_before.size() + 1);

    // Find the new preset
    bool found = false;
    for (const auto& preset : presets_after)
    {
        if (preset.name == "My Layout")
        {
            found = true;
            CHECK(preset.description == "Custom test layout");
            CHECK_FALSE(preset.is_builtin);
        }
    }
    CHECK(found);
}

// ---------------------------------------------------------------------------
// Test 6: Delete preset (user-defined only)
// ---------------------------------------------------------------------------
TEST_CASE("WorkspaceLayout: delete preset", "[workspace_layout]")
{
    TestFixture fixture;

    // Cannot delete builtin
    CHECK_FALSE(fixture.manager.delete_preset("Default"));
    CHECK_FALSE(fixture.manager.delete_preset("Writing"));

    // Can delete user preset
    fixture.manager.save_preset("Deletable", "To be deleted");
    CHECK(fixture.manager.delete_preset("Deletable"));

    // Verify removed
    for (const auto& preset : fixture.manager.list_presets())
    {
        CHECK(preset.name != "Deletable");
    }
}

// ---------------------------------------------------------------------------
// Test 7: Layout persistence (save/restore)
// ---------------------------------------------------------------------------
TEST_CASE("WorkspaceLayout: layout persistence", "[workspace_layout]")
{
    TestFixture fixture;

    // Modify layout
    fixture.manager.resize_panel("file_tree", 300, 0);
    fixture.manager.move_panel("outline", PanelPosition::kBottomPanel, 2);

    // Serialize
    auto json = fixture.manager.save_layout();
    CHECK_FALSE(json.empty());

    // Create new manager and restore
    TestFixture fixture2;
    REQUIRE(fixture2.manager.restore_layout(json));

    auto ft = fixture2.manager.get_panel_layout("file_tree");
    REQUIRE(ft.has_value());
    CHECK(ft->width == 300);

    auto ol = fixture2.manager.get_panel_layout("outline");
    REQUIRE(ol.has_value());
    CHECK(ol->position == PanelPosition::kBottomPanel);
    CHECK(ol->order == 2);
}

// ---------------------------------------------------------------------------
// Test 8: Panel registration
// ---------------------------------------------------------------------------
TEST_CASE("WorkspaceLayout: panel registration", "[workspace_layout]")
{
    TestFixture fixture;

    fixture.manager.register_panel("custom_panel", "My Custom Panel");
    fixture.manager.register_panel("another_panel", "Another Panel");

    auto registered = fixture.manager.registered_panels();
    CHECK(registered.size() == 2);
    CHECK(registered[0].first == "custom_panel");
    CHECK(registered[0].second == "My Custom Panel");

    // No duplicate registration
    fixture.manager.register_panel("custom_panel", "Duplicate");
    CHECK(fixture.manager.registered_panels().size() == 2);
}

// ---------------------------------------------------------------------------
// Test 9: Resize panel
// ---------------------------------------------------------------------------
TEST_CASE("WorkspaceLayout: resize panel", "[workspace_layout]")
{
    TestFixture fixture;

    fixture.manager.resize_panel("file_tree", 400, 0);

    auto panel = fixture.manager.get_panel_layout("file_tree");
    REQUIRE(panel.has_value());
    CHECK(panel->width == 400);
}

// ---------------------------------------------------------------------------
// Test 10: Panel collapsed state
// ---------------------------------------------------------------------------
TEST_CASE("WorkspaceLayout: panel collapsed state", "[workspace_layout]")
{
    TestFixture fixture;

    // Panels start not collapsed
    auto panel = fixture.manager.get_panel_layout("file_tree");
    REQUIRE(panel.has_value());
    CHECK_FALSE(panel->collapsed);

    // Apply nonexistent preset returns false
    CHECK_FALSE(fixture.manager.apply_preset("NonExistent"));

    // List builtin presets
    auto presets = fixture.manager.list_presets();
    int builtin_count = 0;
    for (const auto& preset : presets)
    {
        if (preset.is_builtin)
        {
            ++builtin_count;
        }
    }
    CHECK(builtin_count == 5);
}
