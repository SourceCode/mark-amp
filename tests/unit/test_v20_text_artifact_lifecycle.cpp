/// @file test_v20_text_artifact_lifecycle.cpp
/// @brief V20 Phase 02 – TextArtifactLifecycle unit tests.

#include "core/TextArtifactLifecycle.h"
#include "core/ArtifactCreationService.h"
#include "core/ArtifactRegistry.h"
#include "core/Config.h"
#include "core/EventBus.h"
#include "core/Events.h"
#include "core/UnsavedDocumentBuffer.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

/// Test fixture helper
struct LifecycleTestFixture
{
    EventBus bus;
    ArtifactRegistry registry{bus};
    Config config;
    ArtifactCreationService creation{bus, registry, config};
    UnsavedDocumentBufferManager buffer_mgr;
    TextArtifactLifecycle lifecycle{bus, registry, creation, buffer_mgr, config};
};

// ============================================================================
// Construction
// ============================================================================

TEST_CASE("TextArtifactLifecycle: construction", "[v20][text-lifecycle]")
{
    LifecycleTestFixture fix;
    REQUIRE(fix.lifecycle.operation_count() == 0);
    REQUIRE(fix.lifecycle.open_text_file_count() == 0);
    REQUIRE(fix.lifecycle.unsaved_count() == 0);
}

// ============================================================================
// Create new file
// ============================================================================

TEST_CASE("TextArtifactLifecycle: create new file", "[v20][text-lifecycle]")
{
    LifecycleTestFixture fix;

    FilePlacementContext ctx;
    ctx.entry_point = "menu";

    auto result = fix.lifecycle.create_new_file(ctx, "test.md", "markdown");
    REQUIRE(result.ok());
    REQUIRE_FALSE(result.artifact_id.empty());

    // Should be registered and active
    REQUIRE(fix.registry.count() == 1);
    REQUIRE(fix.registry.active_artifact() == result.artifact_id);

    // Should have unsaved buffer
    REQUIRE(fix.buffer_mgr.has_buffer(result.artifact_id));

    // Should be unsaved state
    const auto* record = fix.registry.find(result.artifact_id);
    REQUIRE(record != nullptr);
    REQUIRE(record->is_unsaved());
    REQUIRE_FALSE(record->has_path());
}

TEST_CASE("TextArtifactLifecycle: create with default name", "[v20][text-lifecycle]")
{
    LifecycleTestFixture fix;

    FilePlacementContext ctx;
    ctx.entry_point = "shortcut";

    auto result = fix.lifecycle.create_new_file(ctx);
    REQUIRE(result.ok());

    const auto* record = fix.registry.find(result.artifact_id);
    REQUIRE(record != nullptr);
    REQUIRE(record->display_name.find("Untitled") != std::string::npos);
}

TEST_CASE("TextArtifactLifecycle: create from different entry points", "[v20][text-lifecycle]")
{
    LifecycleTestFixture fix;

    FilePlacementContext menu_ctx;
    menu_ctx.entry_point = "menu";
    auto r1 = fix.lifecycle.create_new_file(menu_ctx);

    FilePlacementContext palette_ctx;
    palette_ctx.entry_point = "palette";
    auto r2 = fix.lifecycle.create_new_file(palette_ctx);

    REQUIRE(r1.ok());
    REQUIRE(r2.ok());
    REQUIRE(r1.artifact_id != r2.artifact_id);
    REQUIRE(fix.registry.count() == 2);
}

// ============================================================================
// Open file
// ============================================================================

TEST_CASE("TextArtifactLifecycle: open file", "[v20][text-lifecycle]")
{
    LifecycleTestFixture fix;

    auto result = fix.lifecycle.open_file("/workspace/readme.md");
    REQUIRE(result.ok());
    REQUIRE(result.resolved_path.has_value());
    REQUIRE(result.resolved_path.value() == "/workspace/readme.md");

    const auto* record = fix.registry.find(result.artifact_id);
    REQUIRE(record != nullptr);
    REQUIRE(record->display_name == "readme.md");
    REQUIRE(record->is_saved());
    REQUIRE(record->has_path());
}

TEST_CASE("TextArtifactLifecycle: open same path twice activates existing", "[v20][text-lifecycle]")
{
    LifecycleTestFixture fix;

    auto r1 = fix.lifecycle.open_file("/workspace/file.md");
    auto r2 = fix.lifecycle.open_file("/workspace/file.md");

    REQUIRE(r1.ok());
    REQUIRE(r2.ok());
    REQUIRE(r1.artifact_id == r2.artifact_id);
    REQUIRE(fix.registry.count() == 1);
}

TEST_CASE("TextArtifactLifecycle: open empty path fails", "[v20][text-lifecycle]")
{
    LifecycleTestFixture fix;

    auto result = fix.lifecycle.open_file("");
    REQUIRE_FALSE(result.ok());
}

// ============================================================================
// Save
// ============================================================================

TEST_CASE("TextArtifactLifecycle: save unsaved triggers save request", "[v20][text-lifecycle]")
{
    LifecycleTestFixture fix;

    FilePlacementContext ctx;
    ctx.entry_point = "menu";
    auto create_result = fix.lifecycle.create_new_file(ctx, "new.md");

    bool save_requested = false;
    auto sub = fix.bus.subscribe<events::ArtifactSaveRequestEvent>(
        [&save_requested](const events::ArtifactSaveRequestEvent& /*evt*/)
        { save_requested = true; });

    auto save_result = fix.lifecycle.save(create_result.artifact_id);
    REQUIRE(save_result.ok());
    REQUIRE(save_requested);
}

TEST_CASE("TextArtifactLifecycle: save_as promotes unsaved to saved", "[v20][text-lifecycle]")
{
    LifecycleTestFixture fix;

    FilePlacementContext ctx;
    ctx.entry_point = "shortcut";
    auto create_result = fix.lifecycle.create_new_file(ctx, "draft.md");

    auto save_result = fix.lifecycle.save_as(create_result.artifact_id, "/workspace/draft.md");
    REQUIRE(save_result.ok());
    REQUIRE(save_result.resolved_path == "/workspace/draft.md");

    const auto* record = fix.registry.find(create_result.artifact_id);
    REQUIRE(record != nullptr);
    REQUIRE(record->is_saved());
    REQUIRE(record->has_path());
    REQUIRE(record->file_path.value() == "/workspace/draft.md");

    // Unsaved buffer should be removed after first save
    REQUIRE_FALSE(fix.buffer_mgr.has_buffer(create_result.artifact_id));
}

TEST_CASE("TextArtifactLifecycle: save existing file", "[v20][text-lifecycle]")
{
    LifecycleTestFixture fix;

    auto open_result = fix.lifecycle.open_file("/workspace/existing.md");
    auto save_result = fix.lifecycle.save(open_result.artifact_id);

    REQUIRE(save_result.ok());
    REQUIRE(fix.registry.find(open_result.artifact_id)->is_saved());
}

TEST_CASE("TextArtifactLifecycle: save nonexistent fails", "[v20][text-lifecycle]")
{
    LifecycleTestFixture fix;

    ArtifactId fake{"nonexistent"};
    auto result = fix.lifecycle.save(fake);
    REQUIRE_FALSE(result.ok());
}

// ============================================================================
// Rename
// ============================================================================

TEST_CASE("TextArtifactLifecycle: rename artifact", "[v20][text-lifecycle]")
{
    LifecycleTestFixture fix;

    auto open_result = fix.lifecycle.open_file("/workspace/old-name.md");
    auto rename_result = fix.lifecycle.rename(open_result.artifact_id, "new-name.md");

    REQUIRE(rename_result.ok());
    const auto* record = fix.registry.find(open_result.artifact_id);
    REQUIRE(record->display_name == "new-name.md");
}

TEST_CASE("TextArtifactLifecycle: rename with empty name fails", "[v20][text-lifecycle]")
{
    LifecycleTestFixture fix;

    auto open_result = fix.lifecycle.open_file("/workspace/file.md");
    auto result = fix.lifecycle.rename(open_result.artifact_id, "");
    REQUIRE_FALSE(result.ok());
}

// ============================================================================
// Duplicate
// ============================================================================

TEST_CASE("TextArtifactLifecycle: duplicate artifact", "[v20][text-lifecycle]")
{
    LifecycleTestFixture fix;

    FilePlacementContext ctx;
    ctx.entry_point = "menu";
    auto create_result = fix.lifecycle.create_new_file(ctx, "original.md");

    // Add content to original
    fix.buffer_mgr.set_content(create_result.artifact_id, "# Original content");

    auto dup_result = fix.lifecycle.duplicate(create_result.artifact_id);
    REQUIRE(dup_result.ok());
    REQUIRE(dup_result.artifact_id != create_result.artifact_id);

    // Should have 2 artifacts now
    REQUIRE(fix.registry.count() == 2);

    // Duplicate should have copy suffix
    const auto* dup_record = fix.registry.find(dup_result.artifact_id);
    REQUIRE(dup_record != nullptr);
    REQUIRE(dup_record->display_name.find("copy") != std::string::npos);
}

TEST_CASE("TextArtifactLifecycle: duplicate nonexistent fails", "[v20][text-lifecycle]")
{
    LifecycleTestFixture fix;

    ArtifactId fake{"fake"};
    auto result = fix.lifecycle.duplicate(fake);
    REQUIRE_FALSE(result.ok());
}

// ============================================================================
// Move
// ============================================================================

TEST_CASE("TextArtifactLifecycle: move artifact", "[v20][text-lifecycle]")
{
    LifecycleTestFixture fix;

    auto open_result = fix.lifecycle.open_file("/workspace/src/file.md");
    auto move_result = fix.lifecycle.move_to(open_result.artifact_id, "/workspace/docs");

    REQUIRE(move_result.ok());
    REQUIRE(move_result.resolved_path.has_value());
}

TEST_CASE("TextArtifactLifecycle: move unsaved fails", "[v20][text-lifecycle]")
{
    LifecycleTestFixture fix;

    FilePlacementContext ctx;
    ctx.entry_point = "menu";
    auto create_result = fix.lifecycle.create_new_file(ctx);

    auto result = fix.lifecycle.move_to(create_result.artifact_id, "/workspace/docs");
    REQUIRE_FALSE(result.ok());
}

// ============================================================================
// Remove / Close
// ============================================================================

TEST_CASE("TextArtifactLifecycle: remove artifact", "[v20][text-lifecycle]")
{
    LifecycleTestFixture fix;

    FilePlacementContext ctx;
    ctx.entry_point = "menu";
    auto create_result = fix.lifecycle.create_new_file(ctx, "temp.md");

    auto remove_result = fix.lifecycle.remove(create_result.artifact_id);
    REQUIRE(remove_result.ok());
    REQUIRE(fix.registry.count() == 0);
    REQUIRE_FALSE(fix.buffer_mgr.has_buffer(create_result.artifact_id));
}

TEST_CASE("TextArtifactLifecycle: close artifact", "[v20][text-lifecycle]")
{
    LifecycleTestFixture fix;

    FilePlacementContext ctx;
    ctx.entry_point = "menu";
    auto create_result = fix.lifecycle.create_new_file(ctx, "closeme.md");

    auto close_result = fix.lifecycle.close(create_result.artifact_id);
    REQUIRE(close_result.ok());
    REQUIRE(fix.registry.count() == 0);
}

// ============================================================================
// Queries
// ============================================================================

TEST_CASE("TextArtifactLifecycle: has_unsaved_changes", "[v20][text-lifecycle]")
{
    LifecycleTestFixture fix;

    FilePlacementContext ctx;
    ctx.entry_point = "menu";
    auto create_result = fix.lifecycle.create_new_file(ctx, "draft.md");

    // New unsaved file has unsaved changes
    REQUIRE(fix.lifecycle.has_unsaved_changes(create_result.artifact_id));

    // After save-as, no unsaved changes
    fix.lifecycle.save_as(create_result.artifact_id, "/workspace/saved.md");
    REQUIRE_FALSE(fix.lifecycle.has_unsaved_changes(create_result.artifact_id));
}

TEST_CASE("TextArtifactLifecycle: operation count", "[v20][text-lifecycle]")
{
    LifecycleTestFixture fix;

    FilePlacementContext ctx;
    ctx.entry_point = "test";
    fix.lifecycle.create_new_file(ctx);
    fix.lifecycle.create_new_file(ctx);

    REQUIRE(fix.lifecycle.operation_count() == 2);
}
