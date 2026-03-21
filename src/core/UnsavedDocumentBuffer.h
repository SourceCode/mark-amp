/// @file UnsavedDocumentBuffer.h
/// @brief V20 P02-T01: Replace fake untitled paths with unsaved text-document records.
///
/// Provides in-memory buffer storage for unsaved documents. Each buffer is
/// keyed by ArtifactId and holds content, cursor position, scroll state, and
/// language — without ever touching the filesystem.
#pragma once

#include "ArtifactRegistry.h"

#include <optional>
#include <string>
#include <unordered_map>

namespace markamp::core
{

/// Cursor and scroll position for restoring editor state.
struct EditorSessionState
{
    int cursor_line{0};
    int cursor_column{0};
    int scroll_top_line{0};
    int selection_start_line{0};
    int selection_start_col{0};
    int selection_end_line{0};
    int selection_end_col{0};
    bool has_selection{false};

    [[nodiscard]] auto has_cursor() const noexcept -> bool
    {
        return cursor_line > 0 || cursor_column > 0;
    }
};

/// In-memory buffer for an unsaved document.
struct UnsavedBuffer
{
    ArtifactId artifact_id;
    std::string content;                      ///< Full text content
    std::string language_id;                  ///< Language for syntax highlighting
    EditorSessionState session_state;         ///< Cursor, scroll, selection
    bool is_modified{false};                  ///< True if content differs from last save
    std::chrono::steady_clock::time_point last_edit{std::chrono::steady_clock::now()};

    [[nodiscard]] auto content_length() const noexcept -> size_t { return content.size(); }
    [[nodiscard]] auto is_empty() const noexcept -> bool { return content.empty(); }
    [[nodiscard]] auto line_count() const noexcept -> int;
};

/// Manages in-memory buffers for documents that have no filesystem path yet.
///
/// When a user creates a new file via Cmd+N, the document lives entirely in
/// this buffer manager until the first save. After save, the buffer is promoted
/// and the artifact record gains a file_path.
class UnsavedDocumentBufferManager
{
public:
    UnsavedDocumentBufferManager() = default;

    /// Create a new unsaved buffer for an artifact. Returns false if already exists.
    [[nodiscard]] auto create_buffer(const ArtifactId& artifact_id, const std::string& language_id)
        -> bool;

    /// Get a buffer by artifact ID. Returns nullptr if not found.
    [[nodiscard]] auto get_buffer(const ArtifactId& artifact_id) -> UnsavedBuffer*;
    [[nodiscard]] auto get_buffer(const ArtifactId& artifact_id) const -> const UnsavedBuffer*;

    /// Update buffer content. Marks as modified.
    [[nodiscard]] auto set_content(const ArtifactId& artifact_id, const std::string& content)
        -> bool;

    /// Update editor session state (cursor, scroll, selection).
    [[nodiscard]] auto set_session_state(const ArtifactId& artifact_id,
                                          const EditorSessionState& state) -> bool;

    /// Mark buffer as clean (after save).
    void mark_clean(const ArtifactId& artifact_id);

    /// Remove a buffer (after save to disk or discard).
    [[nodiscard]] auto remove_buffer(const ArtifactId& artifact_id) -> bool;

    /// Check if a buffer exists for this artifact.
    [[nodiscard]] auto has_buffer(const ArtifactId& artifact_id) const -> bool;

    /// Total number of unsaved buffers.
    [[nodiscard]] auto count() const -> int { return static_cast<int>(buffers_.size()); }

    /// Get all buffer IDs.
    [[nodiscard]] auto all_artifact_ids() const -> std::vector<ArtifactId>;

    /// Clear all buffers (workspace close).
    void clear();

private:
    std::unordered_map<std::string, UnsavedBuffer> buffers_;
};

} // namespace markamp::core
