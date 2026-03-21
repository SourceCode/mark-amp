/// @file test_v20_command_integration.cpp
/// @brief V20 Phase 06 – Command and context integration tests.

#include "core/ActiveContextService.h"
#include "core/CommandModel.h"
#include "core/EntryPointNormalizer.h"
#include "core/EventBus.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

struct CommandIntegFixture
{
    EventBus bus;
    CommandModel commands{bus};
    ActiveContextService context;
    EntryPointNormalizer entry_points;
};

TEST_CASE("IntegCmd: Same command from multiple entry points", "[v20][cmd-integration]")
{
    CommandIntegFixture fix;

    // Register the canonical command
    CommandDescriptor desc;
    desc.id = "workbench.action.files.newFile";
    desc.label = "New File";
    desc.category = CommandCategory::kFile;
    fix.commands.register_command(desc, [](const std::string& source) {
        CommandResult r;
        r.success = true;
        r.source = source;
        return r;
    });

    // Register entry points for multiple surfaces
    for (auto surface : {EntryPointSurface::kMainMenu, EntryPointSurface::kPalette,
                         EntryPointSurface::kShortcut})
    {
        EntryPointMapping m;
        m.surface = surface;
        m.command_id = "workbench.action.files.newFile";
        m.label = "New File";
        fix.entry_points.register_entry_point(m);
    }

    // Execute from each surface
    auto from_menu = fix.commands.execute("workbench.action.files.newFile", "menu");
    auto from_palette = fix.commands.execute("workbench.action.files.newFile", "palette");
    auto from_shortcut = fix.commands.execute("workbench.action.files.newFile", "shortcut");

    REQUIRE(from_menu.ok());
    REQUIRE(from_palette.ok());
    REQUIRE(from_shortcut.ok());
    REQUIRE(fix.commands.execution_count() == 3);
}

TEST_CASE("IntegCmd: Context-aware command targeting", "[v20][cmd-integration]")
{
    CommandIntegFixture fix;

    // Set context to canvas
    ActiveContext ctx;
    ctx.active_artifact = ArtifactId{"board-1"};
    ctx.active_surface = ActiveSurfaceKind::kCanvas;
    ctx.is_editing = true;
    fix.context.set_context(ctx);

    // Register a canvas-only command
    CommandDescriptor desc;
    desc.id = "canvas.save";
    desc.label = "Save Board";
    desc.category = CommandCategory::kCanvas;
    desc.when_clause = "activeSurface == canvas";

    fix.commands.register_command(desc, [&](const std::string&) {
        CommandResult r;
        r.success = fix.context.evaluate_when("activeSurface == canvas");
        r.target_artifact = fix.context.context().active_artifact;
        return r;
    });

    auto result = fix.commands.execute("canvas.save", "shortcut");
    REQUIRE(result.ok());
    REQUIRE(result.target_artifact == ArtifactId{"board-1"});
}

TEST_CASE("IntegCmd: Focus return after panel action", "[v20][cmd-integration]")
{
    CommandIntegFixture fix;

    PanelActivationRule rule;
    rule.panel_id = "search";
    rule.trigger = "on_search";
    rule.should_activate = true;
    rule.focus_return_target = "active_editor";
    fix.entry_points.register_panel_rule(rule);

    auto target = fix.entry_points.focus_return_for("search");
    REQUIRE(target == "active_editor");
}

TEST_CASE("IntegCmd: Context switch editor→canvas→notebook", "[v20][cmd-integration]")
{
    CommandIntegFixture fix;

    // Editor mode
    ActiveContext editor;
    editor.active_artifact = ArtifactId{"file-1"};
    editor.active_surface = ActiveSurfaceKind::kEditor;
    fix.context.set_context(editor);
    REQUIRE(fix.context.evaluate_when("activeSurface == editor"));

    // Switch to canvas
    ActiveContext canvas;
    canvas.active_artifact = ArtifactId{"board-1"};
    canvas.active_surface = ActiveSurfaceKind::kCanvas;
    fix.context.set_context(canvas);
    REQUIRE(fix.context.evaluate_when("activeSurface == canvas"));
    REQUIRE_FALSE(fix.context.evaluate_when("activeSurface == editor"));

    // Switch to notebook
    ActiveContext notebook;
    notebook.active_artifact = ArtifactId{"nb-1"};
    notebook.active_surface = ActiveSurfaceKind::kNotebook;
    fix.context.set_context(notebook);
    REQUIRE(fix.context.evaluate_when("activeSurface == notebook"));
}
