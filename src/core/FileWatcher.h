/// @file FileWatcher.h
/// @brief Phase 20 – Polling-based file system change detection with debouncing.

#pragma once

#include <chrono>
#include <cstdint>
#include <filesystem>
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

class EventBus;

// ============================================================================
// Enums & Data Structures
// ============================================================================

/// Type of file system change detected.
enum class FileChangeType : std::uint8_t
{
    kCreated,
    kModified,
    kDeleted,
    kRenamed
};

/// Describes a single file system change event.
struct FileChangeEvent
{
    std::string path; ///< Affected path.
    FileChangeType change_type{FileChangeType::kModified};
    std::string old_path;  ///< Previous path (renames only).
    std::string timestamp; ///< ISO 8601 timestamp of detection.
};

// ============================================================================
// FileWatcher
// ============================================================================

/// Watches files and directories for changes using polling.
///
/// Extends the `ConfigFileWatcher` pattern to support arbitrary paths,
/// recursive directory watching, debouncing, and rename detection.
///
/// Usage:
/// ```cpp
/// FileWatcher watcher(event_bus);
/// watcher.watch("/path/to/dir");
/// watcher.on_change([](const FileChangeEvent& evt) { ... });
/// watcher.poll(); // Call periodically
/// ```
class FileWatcher
{
public:
    using ChangeCallback = std::function<void(const FileChangeEvent&)>;

    explicit FileWatcher(EventBus& event_bus);

    // ── Watch Management ──

    /// Start watching a file or directory (recursively).
    auto watch(const std::string& path) -> bool;

    /// Stop watching a path.
    auto unwatch(const std::string& path) -> bool;

    /// Check if a path is currently watched.
    [[nodiscard]] auto is_watching(const std::string& path) const -> bool;

    /// Get all currently watched paths.
    [[nodiscard]] auto watched_paths() const -> std::vector<std::string>;

    /// Number of watched paths.
    [[nodiscard]] auto watched_count() const -> std::size_t;

    // ── Polling ──

    /// Poll all watched paths for changes.
    /// Returns the number of changes detected.
    auto poll() -> int;

    // ── Callbacks ──

    /// Register a callback for file changes.
    auto on_change(ChangeCallback callback) -> std::size_t;

    /// Remove a change callback.
    void remove_callback(std::size_t callback_id);

    // ── Configuration ──

    /// Set debounce interval in milliseconds.
    /// Changes within this window are coalesced.
    void set_debounce_ms(int milliseconds);

    /// Get current debounce interval.
    [[nodiscard]] auto debounce_ms() const -> int;

    /// Set whether to watch directories recursively.
    void set_recursive(bool recursive);

    /// Get recursive setting.
    [[nodiscard]] auto is_recursive() const -> bool;

    // ── Statistics ──

    /// Total number of changes detected since creation.
    [[nodiscard]] auto total_changes_detected() const -> std::size_t;

    /// Clear all watches.
    void clear();

private:
    EventBus& event_bus_;

    /// Tracked state for a single file.
    struct TrackedFile
    {
        std::string path;
        std::filesystem::file_time_type last_write_time{};
        std::uintmax_t last_size{0};
        bool exists{true};
    };

    /// Tracked state for a watched root path.
    struct WatchEntry
    {
        std::string root_path;
        bool is_directory{false};
        std::unordered_map<std::string, TrackedFile> files;
    };

    std::unordered_map<std::string, WatchEntry> watches_;
    std::vector<std::pair<std::size_t, ChangeCallback>> callbacks_;
    std::size_t next_callback_id_{0};

    int debounce_ms_{100};
    bool recursive_{true};
    std::size_t total_changes_{0};

    /// Scan a directory and build the initial file map.
    void scan_directory(WatchEntry& entry);

    /// Detect changes for a single watch entry.
    auto detect_changes(WatchEntry& entry) -> std::vector<FileChangeEvent>;

    /// Notify all registered callbacks and publish to EventBus.
    void notify(const FileChangeEvent& event);

    /// Get current timestamp as ISO 8601.
    [[nodiscard]] static auto current_timestamp() -> std::string;
};

} // namespace markamp::core
