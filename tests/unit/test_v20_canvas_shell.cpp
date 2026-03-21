/// @file test_v20_canvas_shell.cpp
/// @brief V20 Phase 04 – CanvasShellIntegration unit tests.

#include "core/CanvasShellIntegration.h"
#include "core/ArtifactCreationService.h"
#include "core/ArtifactRegistry.h"
#include "core/Config.h"
#include "core/EventBus.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

struct ShellFixture
{
    EventBus bus;
    ArtifactRegistry registry{bus};
    Config config;
    ArtifactCreationService creation{bus, registry, config};
    CanvasShellIntegration shell{registry};

    auto add_canvas(const std::string& name, bool saved = false) -> ArtifactId
    {
        ArtifactCreationRequest req;
        req.kind = ArtifactKind::kCanvas;
        req.display_name = name;
        req.language_id = "canvas";
        req.source = "test";
        auto result = creation.create(req);
        if (saved)
        {
            registry.promote_to_saved(result.id, "/workspace/" + name + ".canvas");
        }
        return result.id;
    }
};

// ============================================================================
// Construction
// ============================================================================

TEST_CASE("CanvasShellIntegration: construction", "[v20][canvas-shell]")
{
    ShellFixture fix;
    REQUIRE(fix.shell.tab_count() == 0);
}

// ============================================================================
// Tab descriptors
// ============================================================================

TEST_CASE("CanvasShellIntegration: build tab descriptors", "[v20][canvas-shell]")
{
    ShellFixture fix;

    auto id1 = fix.add_canvas("Board 1");
    auto id2 = fix.add_canvas("Board 2");
    fix.registry.set_active_artifact(id2);

    auto tabs = fix.shell.build_tab_descriptors();
    REQUIRE(tabs.size() == 2);

    // Find the active tab
    bool found_active = false;
    for (const auto& tab : tabs)
    {
        if (tab.artifact_id == id2)
        {
            REQUIRE(tab.is_active);
            REQUIRE(tab.display_name == "Board 2");
            REQUIRE(tab.has_board());
            found_active = true;
        }
        else
        {
            REQUIRE_FALSE(tab.is_active);
        }
    }
    REQUIRE(found_active);
}

TEST_CASE("CanvasShellIntegration: dirty tabs", "[v20][canvas-shell]")
{
    ShellFixture fix;

    auto id = fix.add_canvas("Unsaved Board");

    auto tabs = fix.shell.build_tab_descriptors();
    REQUIRE(tabs.size() == 1);
    REQUIRE(tabs[0].is_dirty); // unsaved = dirty
}

// ============================================================================
// Tree nodes
// ============================================================================

TEST_CASE("CanvasShellIntegration: build tree nodes", "[v20][canvas-shell]")
{
    ShellFixture fix;

    fix.add_canvas("Board A", /*saved=*/true);
    fix.add_canvas("Board B");

    auto nodes = fix.shell.build_tree_nodes();
    REQUIRE(nodes.size() == 2);

    // Saved board should have a file path
    bool found_saved = false;
    for (const auto& node : nodes)
    {
        if (node.display_name == "Board A")
        {
            REQUIRE(node.has_file());
            found_saved = true;
        }
    }
    REQUIRE(found_saved);
}

// ============================================================================
// Active board tab
// ============================================================================

TEST_CASE("CanvasShellIntegration: active board tab", "[v20][canvas-shell]")
{
    ShellFixture fix;

    // No active board
    REQUIRE_FALSE(fix.shell.active_board_tab().has_value());

    // Set active
    auto id = fix.add_canvas("Active Board");
    fix.registry.set_active_artifact(id);

    auto tab = fix.shell.active_board_tab();
    REQUIRE(tab.has_value());
    REQUIRE(tab->display_name == "Active Board");
    REQUIRE(tab->is_active);
}

TEST_CASE("CanvasShellIntegration: active non-canvas returns nullopt", "[v20][canvas-shell]")
{
    ShellFixture fix;

    // Create a text file and set it active
    ArtifactCreationRequest req;
    req.kind = ArtifactKind::kTextFile;
    req.display_name = "readme";
    req.language_id = "markdown";
    req.source = "test";
    auto result = fix.creation.create(req);
    fix.registry.set_active_artifact(result.id);

    // active_board_tab should return nullopt for non-canvas
    REQUIRE_FALSE(fix.shell.active_board_tab().has_value());
}

// ============================================================================
// Visibility
// ============================================================================

TEST_CASE("CanvasShellIntegration: is_visible_in_tree", "[v20][canvas-shell]")
{
    ShellFixture fix;

    auto unsaved = fix.add_canvas("Unsaved");
    auto saved = fix.add_canvas("Saved", /*saved=*/true);

    // Unsaved boards are not visible in tree (no file path)
    REQUIRE_FALSE(fix.shell.is_visible_in_tree(unsaved));

    // Saved boards are visible
    REQUIRE(fix.shell.is_visible_in_tree(saved));
}

// ============================================================================
// Tab count
// ============================================================================

TEST_CASE("CanvasShellIntegration: tab_count", "[v20][canvas-shell]")
{
    ShellFixture fix;

    fix.add_canvas("B1");
    fix.add_canvas("B2");
    fix.add_canvas("B3");

    REQUIRE(fix.shell.tab_count() == 3);
}
