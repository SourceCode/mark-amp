/// @file test_v20_file_entry_point.cpp
/// @brief V20 Phase 02 – FileEntryPointResolver unit tests.

#include "core/FileEntryPointResolver.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

// ============================================================================
// Construction
// ============================================================================

TEST_CASE("FileEntryPointResolver: construction", "[v20][entry-point]")
{
    FileEntryPointResolver resolver;
    REQUIRE(resolver.resolution_count() == 0);
}

// ============================================================================
// Resolution with workspace
// ============================================================================

TEST_CASE("FileEntryPointResolver: resolve from menu with workspace", "[v20][entry-point]")
{
    FileEntryPointResolver resolver;

    auto placement =
        resolver.resolve(FileCreationEntryPoint::kMenu, "/workspace");

    REQUIRE(placement.target_directory == "/workspace");
    REQUIRE(placement.has_workspace);
    REQUIRE(placement.should_focus);
    REQUIRE_FALSE(placement.should_inline_edit);
    REQUIRE(placement.source == FileCreationEntryPoint::kMenu);
}

TEST_CASE("FileEntryPointResolver: resolve from shortcut", "[v20][entry-point]")
{
    FileEntryPointResolver resolver;

    auto placement = resolver.resolve(FileCreationEntryPoint::kShortcut, "/workspace");

    REQUIRE(placement.target_directory == "/workspace");
    REQUIRE_FALSE(placement.should_inline_edit);
}

TEST_CASE("FileEntryPointResolver: resolve from explorer uses selected path", "[v20][entry-point]")
{
    FileEntryPointResolver resolver;

    auto placement = resolver.resolve(FileCreationEntryPoint::kExplorer, "/workspace",
                                        "/workspace/src");

    REQUIRE(placement.target_directory == "/workspace/src");
    REQUIRE(placement.should_inline_edit);
}

TEST_CASE("FileEntryPointResolver: explorer with file selected uses parent", "[v20][entry-point]")
{
    FileEntryPointResolver resolver;

    auto placement = resolver.resolve(FileCreationEntryPoint::kExplorer, "/workspace",
                                        "/workspace/src/main.cpp");

    REQUIRE(placement.target_directory == "/workspace/src");
}

// ============================================================================
// Resolution without workspace
// ============================================================================

TEST_CASE("FileEntryPointResolver: resolve without workspace", "[v20][entry-point]")
{
    FileEntryPointResolver resolver;

    auto placement = resolver.resolve(FileCreationEntryPoint::kMenu);

    REQUIRE(placement.target_directory.empty());
    REQUIRE_FALSE(placement.has_workspace);
}

// ============================================================================
// Explicit directory overrides
// ============================================================================

TEST_CASE("FileEntryPointResolver: explicit directory wins", "[v20][entry-point]")
{
    FileEntryPointResolver resolver;

    auto placement = resolver.resolve(FileCreationEntryPoint::kProgrammatic, "/workspace",
                                        "/workspace/src", "/explicit/dir");

    REQUIRE(placement.target_directory == "/explicit/dir");
}

// ============================================================================
// Entry point labels
// ============================================================================

TEST_CASE("FileEntryPointResolver: entry point labels", "[v20][entry-point]")
{
    REQUIRE(FileEntryPointResolver::entry_point_label(FileCreationEntryPoint::kMenu) == "menu");
    REQUIRE(FileEntryPointResolver::entry_point_label(FileCreationEntryPoint::kToolbar) == "toolbar");
    REQUIRE(FileEntryPointResolver::entry_point_label(FileCreationEntryPoint::kPalette) == "palette");
    REQUIRE(FileEntryPointResolver::entry_point_label(FileCreationEntryPoint::kExplorer) == "explorer");
    REQUIRE(FileEntryPointResolver::entry_point_label(FileCreationEntryPoint::kTabBar) == "tab-bar");
    REQUIRE(FileEntryPointResolver::entry_point_label(FileCreationEntryPoint::kWelcome) == "welcome");
    REQUIRE(FileEntryPointResolver::entry_point_label(FileCreationEntryPoint::kShortcut) == "shortcut");
    REQUIRE(FileEntryPointResolver::entry_point_label(FileCreationEntryPoint::kDragDrop) == "drag-drop");
    REQUIRE(FileEntryPointResolver::entry_point_label(FileCreationEntryPoint::kProgrammatic) == "programmatic");
}

// ============================================================================
// Should inline edit
// ============================================================================

TEST_CASE("FileEntryPointResolver: inline edit only for explorer", "[v20][entry-point]")
{
    REQUIRE(FileEntryPointResolver::should_inline_edit(FileCreationEntryPoint::kExplorer));
    REQUIRE_FALSE(FileEntryPointResolver::should_inline_edit(FileCreationEntryPoint::kMenu));
    REQUIRE_FALSE(FileEntryPointResolver::should_inline_edit(FileCreationEntryPoint::kToolbar));
    REQUIRE_FALSE(FileEntryPointResolver::should_inline_edit(FileCreationEntryPoint::kShortcut));
    REQUIRE_FALSE(FileEntryPointResolver::should_inline_edit(FileCreationEntryPoint::kPalette));
}

// ============================================================================
// Resolution count
// ============================================================================

TEST_CASE("FileEntryPointResolver: resolution count tracked", "[v20][entry-point]")
{
    FileEntryPointResolver resolver;

    resolver.resolve(FileCreationEntryPoint::kMenu);
    resolver.resolve(FileCreationEntryPoint::kShortcut);

    REQUIRE(resolver.resolution_count() == 2);
}

// ============================================================================
// All entry points
// ============================================================================

TEST_CASE("FileEntryPointResolver: all entry points resolve", "[v20][entry-point]")
{
    FileEntryPointResolver resolver;

    auto from_menu = resolver.resolve(FileCreationEntryPoint::kMenu, "/ws");
    auto from_toolbar = resolver.resolve(FileCreationEntryPoint::kToolbar, "/ws");
    auto from_palette = resolver.resolve(FileCreationEntryPoint::kPalette, "/ws");
    auto from_explorer = resolver.resolve(FileCreationEntryPoint::kExplorer, "/ws", "/ws/src");
    auto from_tab = resolver.resolve(FileCreationEntryPoint::kTabBar, "/ws");
    auto from_welcome = resolver.resolve(FileCreationEntryPoint::kWelcome);
    auto from_shortcut = resolver.resolve(FileCreationEntryPoint::kShortcut, "/ws");
    auto from_drag = resolver.resolve(FileCreationEntryPoint::kDragDrop, "/ws");
    auto from_prog = resolver.resolve(FileCreationEntryPoint::kProgrammatic, "/ws");

    REQUIRE(from_menu.has_workspace);
    REQUIRE(from_toolbar.has_workspace);
    REQUIRE(from_palette.has_workspace);
    REQUIRE(from_explorer.has_workspace);
    REQUIRE(from_tab.has_workspace);
    REQUIRE_FALSE(from_welcome.has_workspace);
    REQUIRE(from_shortcut.has_workspace);
    REQUIRE(from_drag.has_workspace);
    REQUIRE(from_prog.has_workspace);
}
