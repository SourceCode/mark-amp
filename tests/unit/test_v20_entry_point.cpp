/// @file test_v20_entry_point.cpp
/// @brief V20 Phase 06 – EntryPointNormalizer unit tests.

#include "core/EntryPointNormalizer.h"
#include "core/Events.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("EntryPointNormalizer: construction", "[v20][entry-point]")
{
    EntryPointNormalizer normalizer;
    REQUIRE(normalizer.entry_point_count() == 0);
    REQUIRE(normalizer.panel_rule_count() == 0);
}

TEST_CASE("EntryPointNormalizer: register and resolve", "[v20][entry-point]")
{
    EntryPointNormalizer normalizer;

    EntryPointMapping m;
    m.surface = EntryPointSurface::kMainMenu;
    m.command_id = "workbench.action.files.newFile";
    m.label = "New File";
    normalizer.register_entry_point(m);

    auto cmd = normalizer.resolve_command(EntryPointSurface::kMainMenu);
    REQUIRE(cmd == "workbench.action.files.newFile");
}

TEST_CASE("EntryPointNormalizer: multiple surfaces same command", "[v20][entry-point]")
{
    EntryPointNormalizer normalizer;

    auto reg = [&](EntryPointSurface s) {
        EntryPointMapping m;
        m.surface = s;
        m.command_id = "workbench.action.files.newFile";
        m.label = "New File";
        normalizer.register_entry_point(m);
    };

    reg(EntryPointSurface::kMainMenu);
    reg(EntryPointSurface::kPalette);
    reg(EntryPointSurface::kShortcut);
    reg(EntryPointSurface::kToolbar);

    REQUIRE(normalizer.entry_point_count() == 4);

    // All resolve to the same command
    REQUIRE(normalizer.resolve_command(EntryPointSurface::kMainMenu) == "workbench.action.files.newFile");
    REQUIRE(normalizer.resolve_command(EntryPointSurface::kPalette) == "workbench.action.files.newFile");
    REQUIRE(normalizer.resolve_command(EntryPointSurface::kShortcut) == "workbench.action.files.newFile");
}

TEST_CASE("EntryPointNormalizer: mappings_for_surface", "[v20][entry-point]")
{
    EntryPointNormalizer normalizer;

    EntryPointMapping m1;
    m1.surface = EntryPointSurface::kMainMenu;
    m1.command_id = "cmd.newFile";
    m1.label = "New File";
    normalizer.register_entry_point(m1);

    EntryPointMapping m2;
    m2.surface = EntryPointSurface::kMainMenu;
    m2.command_id = "cmd.newNotebook";
    m2.label = "New Notebook";
    normalizer.register_entry_point(m2);

    auto menus = normalizer.mappings_for_surface(EntryPointSurface::kMainMenu);
    REQUIRE(menus.size() == 2);
}

TEST_CASE("EntryPointNormalizer: resolve unknown returns empty", "[v20][entry-point]")
{
    EntryPointNormalizer normalizer;
    auto cmd = normalizer.resolve_command(EntryPointSurface::kDragDrop);
    REQUIRE(cmd.empty());
}

TEST_CASE("EntryPointNormalizer: panel rules", "[v20][entry-point]")
{
    EntryPointNormalizer normalizer;

    PanelActivationRule rule;
    rule.panel_id = "explorer";
    rule.trigger = "on_create";
    rule.should_activate = true;
    rule.focus_return_target = "active_editor";
    normalizer.register_panel_rule(rule);

    REQUIRE(normalizer.panel_rule_count() == 1);

    auto rules = normalizer.rules_for_trigger("on_create");
    REQUIRE(rules.size() == 1);
    REQUIRE(rules[0].should_activate);
}

TEST_CASE("EntryPointNormalizer: focus_return_for", "[v20][entry-point]")
{
    EntryPointNormalizer normalizer;

    PanelActivationRule rule;
    rule.panel_id = "search";
    rule.trigger = "on_search";
    rule.focus_return_target = "editor";
    normalizer.register_panel_rule(rule);

    REQUIRE(normalizer.focus_return_for("search") == "editor");

    // Default fallback for unknown panels
    REQUIRE(normalizer.focus_return_for("unknown") == "active_editor");
}

TEST_CASE("EntryPointMapping: is_valid", "[v20][entry-point]")
{
    EntryPointMapping valid;
    valid.command_id = "cmd.test";
    valid.is_active = true;
    REQUIRE(valid.is_valid());

    EntryPointMapping invalid;
    invalid.is_active = true;
    REQUIRE_FALSE(invalid.is_valid());
}

TEST_CASE("Phase 06 events: EntryPointResolvedEvent", "[v20][entry-events]")
{
    events::EntryPointResolvedEvent evt;
    evt.surface = "menu";
    evt.command_id = "cmd.new";
    REQUIRE(evt.command_id == "cmd.new");
}

TEST_CASE("Phase 06 events: PanelActivationEvent", "[v20][entry-events]")
{
    events::PanelActivationEvent evt;
    evt.panel_id = "explorer";
    evt.trigger = "on_create";
    evt.activated = true;
    REQUIRE(evt.activated);
}

TEST_CASE("Phase 06 events: FocusReturnEvent", "[v20][entry-events]")
{
    events::FocusReturnEvent evt;
    evt.from_panel = "search";
    evt.return_target = "editor";
    REQUIRE(evt.return_target == "editor");
}
