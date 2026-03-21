/// @file test_v20_file_lifecycle_integration.cpp
/// @brief V20 Phase 02 – End-to-end file lifecycle integration tests.
///
/// Tests the full flow: entry-point → create → edit → save → rename → duplicate → close.

#include "core/ArtifactCreationService.h"
#include "core/ArtifactRegistry.h"
#include "core/Config.h"
#include "core/EditorMountContext.h"
#include "core/EventBus.h"
#include "core/Events.h"
#include "core/FileEntryPointResolver.h"
#include "core/TextArtifactLifecycle.h"
#include "core/UnsavedDocumentBuffer.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

struct IntegrationFixture
{
    EventBus bus;
    ArtifactRegistry registry{bus};
    Config config;
    ArtifactCreationService creation{bus, registry, config};
    UnsavedDocumentBufferManager buffer_mgr;
    TextArtifactLifecycle lifecycle{bus, registry, creation, buffer_mgr, config};
    FileEntryPointResolver resolver;
    EditorSessionManager session_mgr;
};

// ============================================================================
// Full create → edit → save-as flow
// ============================================================================

TEST_CASE("Integration: Cmd+N → type → Save As", "[v20][file-integration]")
{
    IntegrationFixture fix;

    // 1. User presses Cmd+N
    auto placement = fix.resolver.resolve(FileCreationEntryPoint::kShortcut, "/workspace");
    FilePlacementContext ctx;
    ctx.entry_point = "shortcut";
    ctx.target_directory = placement.target_directory;

    auto create_result = fix.lifecycle.create_new_file(ctx);
    REQUIRE(create_result.ok());

    // 2. User types content
    fix.buffer_mgr.set_content(create_result.artifact_id, "# My Document\n\nHello world.");
    REQUIRE(fix.lifecycle.has_unsaved_changes(create_result.artifact_id));

    // 3. User saves
    auto save_result =
        fix.lifecycle.save_as(create_result.artifact_id, "/workspace/my-document.md");
    REQUIRE(save_result.ok());
    REQUIRE_FALSE(fix.lifecycle.has_unsaved_changes(create_result.artifact_id));

    // Verify final state
    const auto* record = fix.registry.find(create_result.artifact_id);
    REQUIRE(record->is_saved());
    REQUIRE(record->file_path.value() == "/workspace/my-document.md");
}

// ============================================================================
// Full open → edit → save flow
// ============================================================================

TEST_CASE("Integration: Open → Edit → Save", "[v20][file-integration]")
{
    IntegrationFixture fix;

    // 1. Open existing file
    auto open_result = fix.lifecycle.open_file("/workspace/readme.md");
    REQUIRE(open_result.ok());

    // 2. Track session state
    EditorSessionState state;
    state.cursor_line = 25;
    state.cursor_column = 8;
    fix.session_mgr.save_session(open_result.artifact_id, state);

    // 3. Simulate edit
    fix.registry.set_state(open_result.artifact_id, ArtifactLifecycleState::kDirty);

    // 4. Save
    auto save_result = fix.lifecycle.save(open_result.artifact_id);
    REQUIRE(save_result.ok());

    // Session state survives save
    REQUIRE(fix.session_mgr.has_session(open_result.artifact_id));
    auto restored = fix.session_mgr.get_session(open_result.artifact_id);
    REQUIRE(restored.cursor_line == 25);
}

// ============================================================================
// Explorer create flow
// ============================================================================

TEST_CASE("Integration: Explorer create with placement", "[v20][file-integration]")
{
    IntegrationFixture fix;

    // Resolve from explorer with selected subfolder
    auto placement = fix.resolver.resolve(FileCreationEntryPoint::kExplorer, "/workspace",
                                            "/workspace/docs");

    REQUIRE(placement.target_directory == "/workspace/docs");
    REQUIRE(placement.should_inline_edit);

    // Create file with resolved placement
    FilePlacementContext ctx;
    ctx.entry_point = "explorer";
    ctx.target_directory = placement.target_directory;

    auto result = fix.lifecycle.create_new_file(ctx, "notes.md");
    REQUIRE(result.ok());
}

// ============================================================================
// Rename open file
// ============================================================================

TEST_CASE("Integration: Rename open file updates state", "[v20][file-integration]")
{
    IntegrationFixture fix;

    auto open_result = fix.lifecycle.open_file("/workspace/old-name.md");
    auto rename_result = fix.lifecycle.rename(open_result.artifact_id, "new-name.md");

    REQUIRE(rename_result.ok());

    const auto* record = fix.registry.find(open_result.artifact_id);
    REQUIRE(record->display_name == "new-name.md");
}

// ============================================================================
// Duplicate and modify independently
// ============================================================================

TEST_CASE("Integration: Duplicate creates independent copy", "[v20][file-integration]")
{
    IntegrationFixture fix;

    // Create original
    FilePlacementContext ctx;
    ctx.entry_point = "menu";
    auto original = fix.lifecycle.create_new_file(ctx, "original.md");
    fix.buffer_mgr.set_content(original.artifact_id, "Original content");

    // Duplicate
    auto copy = fix.lifecycle.duplicate(original.artifact_id);
    REQUIRE(copy.ok());

    // Modify copy independently
    fix.buffer_mgr.set_content(copy.artifact_id, "Modified copy");

    // Original should be unchanged
    auto* orig_buf = fix.buffer_mgr.get_buffer(original.artifact_id);
    REQUIRE(orig_buf != nullptr);
    REQUIRE(orig_buf->content == "Original content");

    // Copy should have new content
    auto* copy_buf = fix.buffer_mgr.get_buffer(copy.artifact_id);
    REQUIRE(copy_buf != nullptr);
    REQUIRE(copy_buf->content == "Modified copy");
}

// ============================================================================
// Close without save
// ============================================================================

TEST_CASE("Integration: Close unsaved file", "[v20][file-integration]")
{
    IntegrationFixture fix;

    FilePlacementContext ctx;
    ctx.entry_point = "shortcut";
    auto create_result = fix.lifecycle.create_new_file(ctx, "ephemeral.md");
    fix.buffer_mgr.set_content(create_result.artifact_id, "This will be lost");

    auto close_result = fix.lifecycle.close(create_result.artifact_id);
    REQUIRE(close_result.ok());

    REQUIRE(fix.registry.count() == 0);
    REQUIRE(fix.buffer_mgr.count() == 0);
}

// ============================================================================
// Multiple files in session
// ============================================================================

TEST_CASE("Integration: Multiple files with session management", "[v20][file-integration]")
{
    IntegrationFixture fix;

    // Open several files
    auto f1 = fix.lifecycle.open_file("/workspace/a.md");
    auto f2 = fix.lifecycle.open_file("/workspace/b.md");
    FilePlacementContext ctx;
    ctx.entry_point = "shortcut";
    auto f3 = fix.lifecycle.create_new_file(ctx, "c.md");

    REQUIRE(fix.registry.count() == 3);
    REQUIRE(fix.lifecycle.open_text_file_count() == 3);
    REQUIRE(fix.lifecycle.unsaved_count() == 1);

    // Save sessions
    EditorSessionState s1;
    s1.cursor_line = 10;
    fix.session_mgr.save_session(f1.artifact_id, s1);

    EditorSessionState s2;
    s2.cursor_line = 20;
    fix.session_mgr.save_session(f2.artifact_id, s2);

    REQUIRE(fix.session_mgr.session_count() == 2);

    // Close one
    fix.lifecycle.close(f2.artifact_id);
    REQUIRE(fix.registry.count() == 2);

    // Session for f1 survives
    REQUIRE(fix.session_mgr.has_session(f1.artifact_id));
    REQUIRE(fix.session_mgr.get_session(f1.artifact_id).cursor_line == 10);
}

// ============================================================================
// Full lifecycle state machine
// ============================================================================

TEST_CASE("Integration: Complete state machine traversal", "[v20][file-integration]")
{
    IntegrationFixture fix;

    // kUnsaved → create
    FilePlacementContext ctx;
    ctx.entry_point = "menu";
    auto result = fix.lifecycle.create_new_file(ctx, "state-test.md");
    REQUIRE(fix.registry.find(result.artifact_id)->state == ArtifactLifecycleState::kUnsaved);

    // kUnsaved → kDirty (user types)
    fix.registry.set_state(result.artifact_id, ArtifactLifecycleState::kDirty);
    REQUIRE(fix.registry.find(result.artifact_id)->state == ArtifactLifecycleState::kDirty);

    // kDirty → save_as → kSaved
    fix.lifecycle.save_as(result.artifact_id, "/workspace/state-test.md");
    REQUIRE(fix.registry.find(result.artifact_id)->state == ArtifactLifecycleState::kSaved);

    // kSaved → kDirty (user edits again)
    fix.registry.set_state(result.artifact_id, ArtifactLifecycleState::kDirty);

    // kDirty → save → kSaved
    fix.lifecycle.save(result.artifact_id);
    REQUIRE(fix.registry.find(result.artifact_id)->state == ArtifactLifecycleState::kSaved);
}

// ============================================================================
// Phase 02 event tests  
// ============================================================================

TEST_CASE("Phase 02 events: ArtifactFileOpenedEvent fields", "[v20][file-events]")
{
    events::ArtifactFileOpenedEvent evt;
    evt.artifact_id = "test-id";
    evt.file_path = "/workspace/file.md";
    evt.language_id = "markdown";
    evt.source = "open-file";

    REQUIRE(evt.artifact_id == "test-id");
    REQUIRE(evt.file_path == "/workspace/file.md");
}

TEST_CASE("Phase 02 events: ArtifactFileSavedEvent fields", "[v20][file-events]")
{
    events::ArtifactFileSavedEvent evt;
    evt.artifact_id = "save-id";
    evt.was_first_save = true;

    REQUIRE(evt.was_first_save);
}

TEST_CASE("Phase 02 events: ArtifactFileClosedEvent fields", "[v20][file-events]")
{
    events::ArtifactFileClosedEvent evt;
    evt.artifact_id = "close-id";
    evt.had_unsaved_changes = true;

    REQUIRE(evt.had_unsaved_changes);
}

TEST_CASE("Phase 02 events: EditorSessionRestoredEvent fields", "[v20][file-events]")
{
    events::EditorSessionRestoredEvent evt;
    evt.artifact_id = "session-id";
    evt.cursor_line = 42;
    evt.scroll_top_line = 30;

    REQUIRE(evt.cursor_line == 42);
    REQUIRE(evt.scroll_top_line == 30);
}
