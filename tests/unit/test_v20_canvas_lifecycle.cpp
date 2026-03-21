/// @file test_v20_canvas_lifecycle.cpp
/// @brief V20 Phase 04 – CanvasArtifactLifecycle unit tests.

#include "core/CanvasArtifactLifecycle.h"
#include "core/ArtifactCreationService.h"
#include "core/ArtifactRegistry.h"
#include "core/Config.h"
#include "core/EventBus.h"
#include "core/Events.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

struct CanvasFixture
{
    EventBus bus;
    ArtifactRegistry registry{bus};
    Config config;
    ArtifactCreationService creation{bus, registry, config};
    CanvasArtifactLifecycle lifecycle{bus, registry, creation, config};
};

// ============================================================================
// Construction
// ============================================================================

TEST_CASE("CanvasLifecycle: construction", "[v20][canvas-lifecycle]")
{
    CanvasFixture fix;
    REQUIRE(fix.lifecycle.operation_count() == 0);
    REQUIRE(fix.lifecycle.open_board_count() == 0);
}

// ============================================================================
// Create board
// ============================================================================

TEST_CASE("CanvasLifecycle: create board", "[v20][canvas-lifecycle]")
{
    CanvasFixture fix;

    CanvasPlacementContext ctx;
    ctx.entry_point = "palette";

    auto result = fix.lifecycle.create_board(ctx, "My Board");
    REQUIRE(result.ok());
    REQUIRE_FALSE(result.artifact_id.empty());
    REQUIRE(result.board_id.has_value());

    REQUIRE(fix.registry.count() == 1);
    REQUIRE(fix.registry.active_artifact() == result.artifact_id);

    const auto* record = fix.registry.find(result.artifact_id);
    REQUIRE(record != nullptr);
    REQUIRE(record->kind == ArtifactKind::kCanvas);
    REQUIRE(record->is_unsaved());
}

TEST_CASE("CanvasLifecycle: create from template", "[v20][canvas-lifecycle]")
{
    CanvasFixture fix;

    CanvasPlacementContext ctx;
    ctx.entry_point = "toolbar";

    auto result = fix.lifecycle.create_from_template(ctx, "flowchart", "Flowchart Board");
    REQUIRE(result.ok());

    const auto* record = fix.registry.find(result.artifact_id);
    REQUIRE(record != nullptr);
    REQUIRE(record->kind == ArtifactKind::kCanvas);
}

// ============================================================================
// Open board
// ============================================================================

TEST_CASE("CanvasLifecycle: open board", "[v20][canvas-lifecycle]")
{
    CanvasFixture fix;

    auto result = fix.lifecycle.open_board("/workspace/boards/design.canvas");
    REQUIRE(result.ok());
    REQUIRE(result.resolved_path == "/workspace/boards/design.canvas");

    const auto* record = fix.registry.find(result.artifact_id);
    REQUIRE(record != nullptr);
    REQUIRE(record->display_name == "design");
    REQUIRE(record->is_saved());
    REQUIRE(record->kind == ArtifactKind::kCanvas);
}

TEST_CASE("CanvasLifecycle: open same path twice dedupes", "[v20][canvas-lifecycle]")
{
    CanvasFixture fix;

    auto r1 = fix.lifecycle.open_board("/workspace/board.canvas");
    auto r2 = fix.lifecycle.open_board("/workspace/board.canvas");

    REQUIRE(r1.artifact_id == r2.artifact_id);
    REQUIRE(fix.registry.count() == 1);
}

TEST_CASE("CanvasLifecycle: open empty path fails", "[v20][canvas-lifecycle]")
{
    CanvasFixture fix;
    auto result = fix.lifecycle.open_board("");
    REQUIRE_FALSE(result.ok());
}

TEST_CASE("CanvasLifecycle: restore board", "[v20][canvas-lifecycle]")
{
    CanvasFixture fix;
    auto result = fix.lifecycle.restore_board("/workspace/session.canvas");
    REQUIRE(result.ok());
    REQUIRE(fix.registry.count() == 1);
}

// ============================================================================
// Save
// ============================================================================

TEST_CASE("CanvasLifecycle: save unsaved triggers request", "[v20][canvas-lifecycle]")
{
    CanvasFixture fix;

    CanvasPlacementContext ctx;
    ctx.entry_point = "menu";
    auto create_result = fix.lifecycle.create_board(ctx, "Draft Board");

    bool save_requested = false;
    auto sub = fix.bus.subscribe<events::ArtifactSaveRequestEvent>(
        [&save_requested](const events::ArtifactSaveRequestEvent&) { save_requested = true; });

    auto save_result = fix.lifecycle.save(create_result.artifact_id);
    REQUIRE(save_result.ok());
    REQUIRE(save_requested);
}

TEST_CASE("CanvasLifecycle: save_as promotes to saved", "[v20][canvas-lifecycle]")
{
    CanvasFixture fix;

    CanvasPlacementContext ctx;
    ctx.entry_point = "palette";
    auto create_result = fix.lifecycle.create_board(ctx, "My Canvas");

    auto save_result = fix.lifecycle.save_as(create_result.artifact_id, "/workspace/my-canvas.canvas");
    REQUIRE(save_result.ok());

    const auto* record = fix.registry.find(create_result.artifact_id);
    REQUIRE(record->is_saved());
    REQUIRE(record->file_path.value() == "/workspace/my-canvas.canvas");
}

TEST_CASE("CanvasLifecycle: save nonexistent fails", "[v20][canvas-lifecycle]")
{
    CanvasFixture fix;
    auto result = fix.lifecycle.save(ArtifactId{"fake"});
    REQUIRE_FALSE(result.ok());
}

// ============================================================================
// Rename
// ============================================================================

TEST_CASE("CanvasLifecycle: rename board", "[v20][canvas-lifecycle]")
{
    CanvasFixture fix;

    CanvasPlacementContext ctx;
    ctx.entry_point = "menu";
    auto create_result = fix.lifecycle.create_board(ctx, "Old Board");

    auto rename_result = fix.lifecycle.rename(create_result.artifact_id, "New Board");
    REQUIRE(rename_result.ok());
    REQUIRE(fix.registry.find(create_result.artifact_id)->display_name == "New Board");
}

TEST_CASE("CanvasLifecycle: rename with empty title fails", "[v20][canvas-lifecycle]")
{
    CanvasFixture fix;

    CanvasPlacementContext ctx;
    ctx.entry_point = "menu";
    auto result = fix.lifecycle.create_board(ctx, "Board");

    REQUIRE_FALSE(fix.lifecycle.rename(result.artifact_id, "").ok());
}

// ============================================================================
// Duplicate
// ============================================================================

TEST_CASE("CanvasLifecycle: duplicate board", "[v20][canvas-lifecycle]")
{
    CanvasFixture fix;

    CanvasPlacementContext ctx;
    ctx.entry_point = "menu";
    auto original = fix.lifecycle.create_board(ctx, "Original");

    auto dup = fix.lifecycle.duplicate(original.artifact_id);
    REQUIRE(dup.ok());
    REQUIRE(dup.artifact_id != original.artifact_id);
    REQUIRE(fix.registry.count() == 2);
}

// ============================================================================
// Remove / Close
// ============================================================================

TEST_CASE("CanvasLifecycle: remove board", "[v20][canvas-lifecycle]")
{
    CanvasFixture fix;

    CanvasPlacementContext ctx;
    ctx.entry_point = "menu";
    auto result = fix.lifecycle.create_board(ctx, "Temp Board");

    auto remove_result = fix.lifecycle.remove(result.artifact_id);
    REQUIRE(remove_result.ok());
    REQUIRE(fix.registry.count() == 0);
}

TEST_CASE("CanvasLifecycle: close board", "[v20][canvas-lifecycle]")
{
    CanvasFixture fix;

    CanvasPlacementContext ctx;
    ctx.entry_point = "menu";
    auto result = fix.lifecycle.create_board(ctx, "CloseMe");

    auto close_result = fix.lifecycle.close(result.artifact_id);
    REQUIRE(close_result.ok());
    REQUIRE(fix.registry.count() == 0);
}

// ============================================================================
// Queries
// ============================================================================

TEST_CASE("CanvasLifecycle: query counts", "[v20][canvas-lifecycle]")
{
    CanvasFixture fix;

    CanvasPlacementContext ctx;
    ctx.entry_point = "menu";
    fix.lifecycle.create_board(ctx, "B1");
    fix.lifecycle.create_board(ctx, "B2");

    REQUIRE(fix.lifecycle.open_board_count() == 2);
    REQUIRE(fix.lifecycle.unsaved_board_count() == 2);
}

TEST_CASE("CanvasLifecycle: has_unsaved_changes", "[v20][canvas-lifecycle]")
{
    CanvasFixture fix;

    CanvasPlacementContext ctx;
    ctx.entry_point = "menu";
    auto result = fix.lifecycle.create_board(ctx, "Draft");

    REQUIRE(fix.lifecycle.has_unsaved_changes(result.artifact_id));

    fix.lifecycle.save_as(result.artifact_id, "/workspace/draft.canvas");
    REQUIRE_FALSE(fix.lifecycle.has_unsaved_changes(result.artifact_id));
}
