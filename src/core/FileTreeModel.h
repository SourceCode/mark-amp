/// @file FileTreeModel.h
/// @brief Phase 20 – Sortable, filterable file tree model wrapping FileNode.

#pragma once

#include "core/FileNode.h"

#include <cstdint>
#include <functional>
#include <string>
#include <unordered_set>
#include <vector>

namespace markamp::core
{

class EventBus;

// ============================================================================
// Enums & Data Structures
// ============================================================================

/// Sort order for the file tree.
enum class FileTreeSortOrder : std::uint8_t
{
    kNameAsc,
    kNameDesc,
    kModifiedNewest,
    kModifiedOldest,
    kSizeAsc,
    kSizeDesc,
    kTypeGrouped
};

/// Filter configuration for the file tree.
struct FileTreeFilter
{
    std::vector<std::string> include_extensions; ///< Only show files with these extensions.
    std::vector<std::string> exclude_patterns;   ///< Hide files matching these glob patterns.
    bool show_hidden{false};                     ///< Show hidden files (dotfiles).
    bool show_dotfiles{false};                   ///< Show .git, .markamp, etc.
};

// ============================================================================
// FileTreeModel
// ============================================================================

/// High-level tree model that wraps FileNode with sorting, filtering,
/// expand/collapse state, and integration with FileWatcher for live refresh.
///
/// Usage:
/// ```cpp
/// FileTreeModel model(event_bus);
/// model.load_directory("/path/to/workspace");
/// model.set_sort_order(FileTreeSortOrder::kNameAsc);
/// auto flat = model.flatten();
/// ```
class FileTreeModel
{
public:
    using RefreshCallback = std::function<void()>;

    explicit FileTreeModel(EventBus& event_bus);

    // ── Loading ──

    /// Load a directory tree from disk.
    auto load_directory(const std::string& path) -> bool;

    /// Refresh the tree from disk (re-scan).
    auto refresh() -> bool;

    /// Get the root path currently loaded.
    [[nodiscard]] auto root_path() const -> const std::string&;

    // ── Tree Access ──

    /// Get the root node (mutable).
    [[nodiscard]] auto root() -> FileNode&;

    /// Get the root node (const).
    [[nodiscard]] auto root() const -> const FileNode&;

    /// Find a node by its path.
    [[nodiscard]] auto find_node(const std::string& path) -> FileNode*;

    /// Find a node by its path (const).
    [[nodiscard]] auto find_node(const std::string& path) const -> const FileNode*;

    // ── Expand/Collapse ──

    /// Expand a folder node.
    void expand_node(const std::string& path);

    /// Collapse a folder node.
    void collapse_node(const std::string& path);

    /// Toggle expand/collapse.
    void toggle_node(const std::string& path);

    /// Check if a node is expanded.
    [[nodiscard]] auto is_expanded(const std::string& path) const -> bool;

    /// Get all currently expanded paths.
    [[nodiscard]] auto expanded_paths() const -> std::vector<std::string>;

    /// Expand all nodes.
    void expand_all();

    /// Collapse all nodes.
    void collapse_all();

    // ── Sorting ──

    /// Set the sort order and re-sort the tree.
    void set_sort_order(FileTreeSortOrder order);

    /// Get the current sort order.
    [[nodiscard]] auto sort_order() const -> FileTreeSortOrder;

    // ── Filtering ──

    /// Set the filter and re-apply.
    void set_filter(const FileTreeFilter& filter);

    /// Get the current filter.
    [[nodiscard]] auto filter() const -> const FileTreeFilter&;

    // ── Flattening ──

    /// Flatten the visible tree into a list of nodes (depth-first, respecting expand/filter).
    [[nodiscard]] auto flatten() const -> std::vector<const FileNode*>;

    // ── Statistics ──

    /// Total number of nodes in the tree (including hidden).
    [[nodiscard]] auto node_count() const -> std::size_t;

    /// Number of visible nodes after filtering.
    [[nodiscard]] auto visible_node_count() const -> std::size_t;

    // ── Callbacks ──

    /// Register a callback for when the tree is refreshed.
    auto on_refresh(RefreshCallback callback) -> std::size_t;

    /// Remove a refresh callback.
    void remove_refresh_callback(std::size_t callback_id);

    /// Clear the entire model.
    void clear();

private:
    EventBus& event_bus_;
    FileNode root_;
    std::string root_path_;
    std::unordered_set<std::string> expanded_;
    FileTreeSortOrder sort_order_{FileTreeSortOrder::kNameAsc};
    FileTreeFilter filter_;

    std::vector<std::pair<std::size_t, RefreshCallback>> refresh_callbacks_;
    std::size_t next_callback_id_{0};

    /// Recursively build the tree from a directory.
    void build_tree(FileNode& node, const std::string& dir_path);

    /// Apply sorting to a node and its children recursively.
    void apply_sort(FileNode& node);

    /// Apply filter criteria to a node.
    [[nodiscard]] auto passes_filter(const FileNode& node) const -> bool;

    /// Count nodes recursively.
    [[nodiscard]] static auto count_nodes(const FileNode& node) -> std::size_t;

    /// Flatten helper.
    void flatten_recursive(const FileNode& node, std::vector<const FileNode*>& result) const;

    /// Notify refresh callbacks.
    void notify_refresh();
};

} // namespace markamp::core
