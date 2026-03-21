/// @file test_v20_unsaved_buffer.cpp
/// @brief V20 Phase 02 – UnsavedDocumentBufferManager unit tests.

#include "core/UnsavedDocumentBuffer.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

// ============================================================================
// Construction
// ============================================================================

TEST_CASE("UnsavedBufferManager: construction", "[v20][unsaved-buffer]")
{
    UnsavedDocumentBufferManager mgr;
    REQUIRE(mgr.count() == 0);
}

// ============================================================================
// Create buffer
// ============================================================================

TEST_CASE("UnsavedBufferManager: create buffer", "[v20][unsaved-buffer]")
{
    UnsavedDocumentBufferManager mgr;
    ArtifactId artifact_id{"test-id"};

    REQUIRE(mgr.create_buffer(artifact_id, "markdown"));
    REQUIRE(mgr.count() == 1);
    REQUIRE(mgr.has_buffer(artifact_id));
}

TEST_CASE("UnsavedBufferManager: create duplicate fails", "[v20][unsaved-buffer]")
{
    UnsavedDocumentBufferManager mgr;
    ArtifactId artifact_id{"dup-id"};

    REQUIRE(mgr.create_buffer(artifact_id, "markdown"));
    REQUIRE_FALSE(mgr.create_buffer(artifact_id, "markdown"));
    REQUIRE(mgr.count() == 1);
}

TEST_CASE("UnsavedBufferManager: create with empty ID fails", "[v20][unsaved-buffer]")
{
    UnsavedDocumentBufferManager mgr;
    ArtifactId empty_id{""};

    REQUIRE_FALSE(mgr.create_buffer(empty_id, "markdown"));
}

TEST_CASE("UnsavedBufferManager: new buffer is not modified", "[v20][unsaved-buffer]")
{
    UnsavedDocumentBufferManager mgr;
    ArtifactId artifact_id{"new-id"};
    mgr.create_buffer(artifact_id, "markdown");

    auto* buffer = mgr.get_buffer(artifact_id);
    REQUIRE(buffer != nullptr);
    REQUIRE_FALSE(buffer->is_modified);
    REQUIRE(buffer->is_empty());
    REQUIRE(buffer->content_length() == 0);
}

// ============================================================================
// Get buffer
// ============================================================================

TEST_CASE("UnsavedBufferManager: get existing buffer", "[v20][unsaved-buffer]")
{
    UnsavedDocumentBufferManager mgr;
    ArtifactId artifact_id{"get-id"};
    mgr.create_buffer(artifact_id, "python");

    auto* buffer = mgr.get_buffer(artifact_id);
    REQUIRE(buffer != nullptr);
    REQUIRE(buffer->language_id == "python");
    REQUIRE(buffer->artifact_id == artifact_id);
}

TEST_CASE("UnsavedBufferManager: get nonexistent returns nullptr", "[v20][unsaved-buffer]")
{
    UnsavedDocumentBufferManager mgr;
    ArtifactId missing{"missing"};
    REQUIRE(mgr.get_buffer(missing) == nullptr);
}

TEST_CASE("UnsavedBufferManager: const get", "[v20][unsaved-buffer]")
{
    UnsavedDocumentBufferManager mgr;
    ArtifactId artifact_id{"const-id"};
    mgr.create_buffer(artifact_id, "cpp");

    const auto& const_mgr = mgr;
    const auto* buffer = const_mgr.get_buffer(artifact_id);
    REQUIRE(buffer != nullptr);
    REQUIRE(buffer->language_id == "cpp");
}

// ============================================================================
// Set content
// ============================================================================

TEST_CASE("UnsavedBufferManager: set content", "[v20][unsaved-buffer]")
{
    UnsavedDocumentBufferManager mgr;
    ArtifactId artifact_id{"content-id"};
    mgr.create_buffer(artifact_id, "markdown");

    REQUIRE(mgr.set_content(artifact_id, "# Hello\n\nWorld"));

    auto* buffer = mgr.get_buffer(artifact_id);
    REQUIRE(buffer->content == "# Hello\n\nWorld");
    REQUIRE(buffer->is_modified);
    REQUIRE(buffer->content_length() == 14);
    REQUIRE(buffer->line_count() == 3);
}

TEST_CASE("UnsavedBufferManager: set content on missing buffer fails", "[v20][unsaved-buffer]")
{
    UnsavedDocumentBufferManager mgr;
    ArtifactId missing{"missing"};
    REQUIRE_FALSE(mgr.set_content(missing, "content"));
}

// ============================================================================
// Session state
// ============================================================================

TEST_CASE("UnsavedBufferManager: set session state", "[v20][unsaved-buffer]")
{
    UnsavedDocumentBufferManager mgr;
    ArtifactId artifact_id{"session-id"};
    mgr.create_buffer(artifact_id, "markdown");

    EditorSessionState state;
    state.cursor_line = 42;
    state.cursor_column = 10;
    state.scroll_top_line = 30;

    REQUIRE(mgr.set_session_state(artifact_id, state));

    auto* buffer = mgr.get_buffer(artifact_id);
    REQUIRE(buffer->session_state.cursor_line == 42);
    REQUIRE(buffer->session_state.cursor_column == 10);
    REQUIRE(buffer->session_state.scroll_top_line == 30);
}

// ============================================================================
// Mark clean
// ============================================================================

TEST_CASE("UnsavedBufferManager: mark clean", "[v20][unsaved-buffer]")
{
    UnsavedDocumentBufferManager mgr;
    ArtifactId artifact_id{"clean-id"};
    mgr.create_buffer(artifact_id, "markdown");
    mgr.set_content(artifact_id, "modified content");

    auto* buffer = mgr.get_buffer(artifact_id);
    REQUIRE(buffer->is_modified);

    mgr.mark_clean(artifact_id);
    REQUIRE_FALSE(buffer->is_modified);
}

// ============================================================================
// Remove buffer
// ============================================================================

TEST_CASE("UnsavedBufferManager: remove buffer", "[v20][unsaved-buffer]")
{
    UnsavedDocumentBufferManager mgr;
    ArtifactId artifact_id{"remove-id"};
    mgr.create_buffer(artifact_id, "markdown");

    REQUIRE(mgr.remove_buffer(artifact_id));
    REQUIRE(mgr.count() == 0);
    REQUIRE_FALSE(mgr.has_buffer(artifact_id));
}

TEST_CASE("UnsavedBufferManager: remove nonexistent fails", "[v20][unsaved-buffer]")
{
    UnsavedDocumentBufferManager mgr;
    ArtifactId missing{"missing"};
    REQUIRE_FALSE(mgr.remove_buffer(missing));
}

// ============================================================================
// All artifact IDs
// ============================================================================

TEST_CASE("UnsavedBufferManager: all_artifact_ids", "[v20][unsaved-buffer]")
{
    UnsavedDocumentBufferManager mgr;
    mgr.create_buffer(ArtifactId{"a"}, "md");
    mgr.create_buffer(ArtifactId{"b"}, "py");

    auto ids = mgr.all_artifact_ids();
    REQUIRE(ids.size() == 2);
}

// ============================================================================
// Clear
// ============================================================================

TEST_CASE("UnsavedBufferManager: clear", "[v20][unsaved-buffer]")
{
    UnsavedDocumentBufferManager mgr;
    mgr.create_buffer(ArtifactId{"x"}, "md");
    mgr.create_buffer(ArtifactId{"y"}, "py");

    mgr.clear();
    REQUIRE(mgr.count() == 0);
}

// ============================================================================
// UnsavedBuffer helpers
// ============================================================================

TEST_CASE("UnsavedBuffer: line_count", "[v20][unsaved-buffer]")
{
    UnsavedBuffer buffer;
    REQUIRE(buffer.line_count() == 0);

    buffer.content = "line1";
    REQUIRE(buffer.line_count() == 1);

    buffer.content = "line1\nline2\nline3";
    REQUIRE(buffer.line_count() == 3);
}

TEST_CASE("EditorSessionState: has_cursor", "[v20][unsaved-buffer]")
{
    EditorSessionState state;
    REQUIRE_FALSE(state.has_cursor());

    state.cursor_line = 5;
    REQUIRE(state.has_cursor());
}
