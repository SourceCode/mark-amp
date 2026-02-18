/// @file MultiRootWorkspace.h
/// @brief Phase 20 – Multi-root workspace management with .markamp-workspace files.

#pragma once

#include "core/FileNode.h"

#include <expected>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

class EventBus;

// ============================================================================
// Data Structures
// ============================================================================

/// Represents a single root folder in a multi-root workspace.
struct WorkspaceRoot
{
    std::string id;           ///< Unique root ID.
    std::string path;         ///< Filesystem path.
    std::string display_name; ///< User-friendly display name.
    int sort_order{0};        ///< Order within the workspace.
    bool is_active{false};    ///< Whether this is the currently active root.
    int file_count{0};        ///< Number of files in this root (cached).
};

/// Configuration for a workspace file (.markamp-workspace).
struct WorkspaceConfig
{
    std::string name;                                                ///< Workspace display name.
    std::vector<WorkspaceRoot> roots;                                ///< Root folders.
    std::unordered_map<std::string, std::string> settings_overrides; ///< Workspace-level settings.
    std::string created_at; ///< ISO 8601 creation timestamp.
};

// ============================================================================
// MultiRootWorkspace
// ============================================================================

/// Manages multi-root workspaces with .markamp-workspace file I/O.
///
/// Extends `WorkspaceService` with support for multiple root folders,
/// workspace file persistence, and merged file tree views.
///
/// Usage:
/// ```cpp
/// MultiRootWorkspace workspace(event_bus);
/// workspace.add_root("/path/to/docs", "Documentation");
/// workspace.add_root("/path/to/notes", "Notes");
/// workspace.save_workspace_file("/path/to/my.markamp-workspace");
/// ```
class MultiRootWorkspace
{
public:
    explicit MultiRootWorkspace(EventBus& event_bus);

    // ── Root Management ──

    /// Add a root folder to the workspace.
    [[nodiscard]] auto add_root(const std::string& path, const std::string& display_name = "")
        -> std::expected<std::string, std::string>;

    /// Remove a root folder by ID.
    auto remove_root(const std::string& root_id) -> bool;

    /// Reorder roots by providing the new order of IDs.
    void reorder_roots(const std::vector<std::string>& root_ids);

    /// Set the active root.
    auto set_active_root(const std::string& root_id) -> bool;

    /// Get the currently active root.
    [[nodiscard]] auto active_root() const -> std::optional<WorkspaceRoot>;

    /// Get all roots.
    [[nodiscard]] auto all_roots() const -> std::vector<WorkspaceRoot>;

    /// Get root count.
    [[nodiscard]] auto root_count() const -> std::size_t;

    /// Find which root a file path belongs to.
    [[nodiscard]] auto find_root_for_path(const std::string& file_path) const
        -> std::optional<WorkspaceRoot>;

    // ── Workspace File I/O ──

    /// Save workspace configuration to a .markamp-workspace file.
    [[nodiscard]] auto save_workspace_file(const std::string& path) const
        -> std::expected<void, std::string>;

    /// Load workspace configuration from a .markamp-workspace file.
    [[nodiscard]] auto load_workspace_file(const std::string& path)
        -> std::expected<void, std::string>;

    // ── File Tree ──

    /// Merge all root file trees into a single virtual tree.
    [[nodiscard]] auto merge_file_trees() const -> FileNode;

    // ── Configuration ──

    /// Get workspace name.
    [[nodiscard]] auto name() const -> const std::string&;

    /// Set workspace name.
    void set_name(const std::string& workspace_name);

    /// Get a workspace-level setting override.
    [[nodiscard]] auto get_setting(const std::string& key) const -> std::optional<std::string>;

    /// Set a workspace-level setting override.
    void set_setting(const std::string& key, const std::string& value);

    /// Clear all roots and settings.
    void clear();

private:
    EventBus& event_bus_;
    WorkspaceConfig config_;
    int next_root_id_{1};

    /// Generate a unique root ID.
    [[nodiscard]] auto generate_root_id() -> std::string;

    /// Get current timestamp as ISO 8601 string.
    [[nodiscard]] static auto current_timestamp() -> std::string;
};

} // namespace markamp::core
