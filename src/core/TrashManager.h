/// @file TrashManager.h
/// @brief Phase 20 – Soft-delete with restore support.

#pragma once

#include <cstdint>
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

/// Information about a trashed item.
struct TrashedItem
{
    std::string id;            ///< Unique trash ID.
    std::string original_path; ///< Original file/directory path.
    std::string trash_path;    ///< Path in the trash directory.
    std::string trashed_at;    ///< ISO 8601 timestamp.
    std::uint64_t size_bytes{0};
    bool is_directory{false};
};

/// Configuration for trash behavior.
struct TrashConfig
{
    int auto_purge_days{30};          ///< Auto-purge items older than this (0 = never).
    std::uint64_t max_trash_bytes{0}; ///< Max total trash size (0 = unlimited).
    std::string trash_directory;      ///< Base trash directory (default: .markamp/trash/).
};

// ============================================================================
// TrashManager
// ============================================================================

/// Manages soft-deleted files with restore support.
///
/// Items are moved to a trash directory with metadata sidecar
/// files, enabling restore to the original location.
///
/// Usage:
/// ```cpp
/// TrashManager trash(event_bus);
/// trash.trash("/path/to/file.md");
/// auto items = trash.list_trashed();
/// trash.restore(items[0].id);
/// ```
class TrashManager
{
public:
    explicit TrashManager(EventBus& event_bus);

    // ── Trash Operations ──

    /// Move a file or directory to trash.
    [[nodiscard]] auto trash(const std::string& path) -> std::expected<std::string, std::string>;

    /// Restore a trashed item to its original location.
    [[nodiscard]] auto restore(const std::string& trash_id) -> std::expected<void, std::string>;

    /// Permanently delete a trashed item.
    auto permanent_delete(const std::string& trash_id) -> bool;

    // ── Listing ──

    /// List all trashed items (newest first).
    [[nodiscard]] auto list_trashed() const -> std::vector<TrashedItem>;

    /// Find a trashed item by its original path.
    [[nodiscard]] auto find_by_original_path(const std::string& path) const
        -> std::optional<TrashedItem>;

    /// Find a trashed item by its ID.
    [[nodiscard]] auto find_by_id(const std::string& trash_id) const -> std::optional<TrashedItem>;

    // ── Bulk Operations ──

    /// Empty the trash (permanently delete all items).
    auto empty_trash() -> int;

    /// Auto-purge items older than the specified number of days.
    auto auto_purge_older_than(int days) -> int;

    // ── Statistics ──

    /// Number of items in trash.
    [[nodiscard]] auto trashed_count() const -> std::size_t;

    /// Total size of all trashed items in bytes.
    [[nodiscard]] auto total_trash_size() const -> std::uint64_t;

    // ── Configuration ──

    /// Set trash configuration.
    void set_config(const TrashConfig& config);

    /// Get trash configuration.
    [[nodiscard]] auto config() const -> const TrashConfig&;

    /// Clear all trash entries (in-memory only).
    void clear();

private:
    EventBus& event_bus_;
    TrashConfig config_;
    std::unordered_map<std::string, TrashedItem> items_;
    int next_id_{1};

    /// Generate a unique trash ID.
    [[nodiscard]] auto generate_id() -> std::string;

    /// Get current timestamp as ISO 8601 string.
    [[nodiscard]] static auto current_timestamp() -> std::string;
};

} // namespace markamp::core
