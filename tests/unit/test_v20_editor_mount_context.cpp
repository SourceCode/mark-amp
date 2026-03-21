/// @file test_v20_editor_mount_context.cpp
/// @brief V20 Phase 02 – EditorSessionManager unit tests.

#include "core/EditorMountContext.h"
#include "core/ArtifactRegistry.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

// ============================================================================
// Construction
// ============================================================================

TEST_CASE("EditorSessionManager: construction", "[v20][editor-mount]")
{
    EditorSessionManager mgr;
    REQUIRE(mgr.session_count() == 0);
}

// ============================================================================
// Save / Get session
// ============================================================================

TEST_CASE("EditorSessionManager: save and get session", "[v20][editor-mount]")
{
    EditorSessionManager mgr;
    ArtifactId artifact_id{"test-id"};

    EditorSessionState state;
    state.cursor_line = 42;
    state.cursor_column = 10;
    state.scroll_top_line = 30;

    mgr.save_session(artifact_id, state);

    REQUIRE(mgr.has_session(artifact_id));
    REQUIRE(mgr.session_count() == 1);

    auto retrieved = mgr.get_session(artifact_id);
    REQUIRE(retrieved.cursor_line == 42);
    REQUIRE(retrieved.cursor_column == 10);
    REQUIRE(retrieved.scroll_top_line == 30);
}

TEST_CASE("EditorSessionManager: get nonexistent returns empty", "[v20][editor-mount]")
{
    EditorSessionManager mgr;
    ArtifactId missing{"missing"};

    auto state = mgr.get_session(missing);
    REQUIRE(state.cursor_line == 0);
    REQUIRE(state.cursor_column == 0);
    REQUIRE_FALSE(state.has_cursor());
}

TEST_CASE("EditorSessionManager: update existing session", "[v20][editor-mount]")
{
    EditorSessionManager mgr;
    ArtifactId artifact_id{"update-id"};

    EditorSessionState initial;
    initial.cursor_line = 1;
    mgr.save_session(artifact_id, initial);

    EditorSessionState updated;
    updated.cursor_line = 100;
    updated.cursor_column = 50;
    mgr.save_session(artifact_id, updated);

    REQUIRE(mgr.session_count() == 1);
    auto retrieved = mgr.get_session(artifact_id);
    REQUIRE(retrieved.cursor_line == 100);
    REQUIRE(retrieved.cursor_column == 50);
}

// ============================================================================
// Remove session
// ============================================================================

TEST_CASE("EditorSessionManager: remove session", "[v20][editor-mount]")
{
    EditorSessionManager mgr;
    ArtifactId artifact_id{"remove-id"};

    EditorSessionState state;
    state.cursor_line = 5;
    mgr.save_session(artifact_id, state);

    mgr.remove_session(artifact_id);
    REQUIRE_FALSE(mgr.has_session(artifact_id));
    REQUIRE(mgr.session_count() == 0);
}

// ============================================================================
// Build mount descriptor
// ============================================================================

TEST_CASE("EditorSessionManager: build mount descriptor", "[v20][editor-mount]")
{
    EditorSessionManager mgr;

    ArtifactRecord record;
    record.id = ArtifactId{"mount-desc-id"};
    record.display_name = "test.md";
    record.language_id = "markdown";
    record.file_path = "/workspace/test.md";
    record.state = ArtifactLifecycleState::kSaved;

    EditorSessionState session;
    session.cursor_line = 10;
    session.cursor_column = 5;
    mgr.save_session(record.id, session);

    auto desc = mgr.build_mount_descriptor(record);

    REQUIRE(desc.artifact_id == record.id);
    REQUIRE(desc.display_name == "test.md");
    REQUIRE(desc.language_id == "markdown");
    REQUIRE(desc.file_path.has_value());
    REQUIRE(desc.file_path.value() == "/workspace/test.md");
    REQUIRE_FALSE(desc.is_unsaved);
    REQUIRE(desc.session_state.cursor_line == 10);
    REQUIRE(desc.session_state.cursor_column == 5);
}

TEST_CASE("EditorSessionManager: build descriptor for unsaved", "[v20][editor-mount]")
{
    EditorSessionManager mgr;

    ArtifactRecord record;
    record.id = ArtifactId{"unsaved-desc-id"};
    record.display_name = "Untitled-1";
    record.state = ArtifactLifecycleState::kUnsaved;

    auto desc = mgr.build_mount_descriptor(record);

    REQUIRE(desc.is_unsaved);
    REQUIRE_FALSE(desc.file_path.has_value());
}

// ============================================================================
// Clear
// ============================================================================

TEST_CASE("EditorSessionManager: clear", "[v20][editor-mount]")
{
    EditorSessionManager mgr;

    EditorSessionState state;
    state.cursor_line = 1;
    mgr.save_session(ArtifactId{"a"}, state);
    mgr.save_session(ArtifactId{"b"}, state);

    mgr.clear();
    REQUIRE(mgr.session_count() == 0);
}

// ============================================================================
// Selection state
// ============================================================================

TEST_CASE("EditorSessionState: selection tracking", "[v20][editor-mount]")
{
    EditorSessionState state;
    REQUIRE_FALSE(state.has_selection);

    state.has_selection = true;
    state.selection_start_line = 5;
    state.selection_start_col = 0;
    state.selection_end_line = 10;
    state.selection_end_col = 15;

    REQUIRE(state.has_selection);
    REQUIRE(state.selection_start_line == 5);
    REQUIRE(state.selection_end_line == 10);
}
