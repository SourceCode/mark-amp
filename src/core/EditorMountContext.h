/// @file EditorMountContext.h
/// @brief V20 P02-T04: Editor mount context for robust file opening.
///
/// Provides session state tracking and mount context for editor panels.
/// Allows editors to mount from artifact records rather than raw file paths,
/// enabling proper cursor/scroll restore for unsaved files, split views,
/// and reopened documents.
#pragma once

#include "ArtifactRegistry.h"
#include "UnsavedDocumentBuffer.h"

#include <optional>
#include <string>
#include <unordered_map>

namespace markamp::core
{

/// Context for mounting an artifact in an editor.
struct EditorMountDescriptor
{
    ArtifactId artifact_id;
    std::optional<std::string> file_path;           ///< Null for unsaved files
    std::string language_id;
    std::string display_name;
    EditorSessionState session_state;               ///< Cursor/scroll to restore
    bool is_unsaved{false};
    int editor_group{0};                            ///< Which split group to mount in
};

/// Manages per-artifact editor session state. Persists cursor, scroll, and
/// selection position indexed by artifact ID, surviving close/reopen cycles.
class EditorSessionManager
{
public:
    EditorSessionManager() = default;

    /// Store session state for an artifact.
    void save_session(const ArtifactId& artifact_id, const EditorSessionState& state);

    /// Retrieve session state for an artifact. Returns empty state if none stored.
    [[nodiscard]] auto get_session(const ArtifactId& artifact_id) const -> EditorSessionState;

    /// Check if session state exists for an artifact.
    [[nodiscard]] auto has_session(const ArtifactId& artifact_id) const -> bool;

    /// Remove stored session state.
    void remove_session(const ArtifactId& artifact_id);

    /// Build a mount descriptor from registry and session data.
    [[nodiscard]] auto build_mount_descriptor(const ArtifactRecord& record) const
        -> EditorMountDescriptor;

    /// Total stored sessions.
    [[nodiscard]] auto session_count() const -> int
    {
        return static_cast<int>(sessions_.size());
    }

    /// Clear all stored sessions.
    void clear();

private:
    std::unordered_map<std::string, EditorSessionState> sessions_;
};

} // namespace markamp::core
