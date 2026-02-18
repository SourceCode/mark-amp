/// @file FileWatcherService.h
/// @brief V9 Phase 40 — Directory watching service with debounce and glob patterns.
#pragma once

#include <chrono>
#include <cstdint>
#include <functional>
#include <string>
#include <vector>

namespace markamp::core
{

/// Types of file system changes.
enum class FileChangeType : uint8_t
{
    kCreated = 0,
    kModified = 1,
    kDeleted = 2,
    kRenamed = 3,
};

/// A detected file change event.
struct FileChangeEvent
{
    FileChangeType change_type{FileChangeType::kModified};
    std::string file_path; ///< Path of the changed file
    std::string old_path;  ///< Previous path (for renames)
    std::chrono::system_clock::time_point timestamp;
};

/// Callback type for file change notifications.
using FileWatchCallback = std::function<void(const FileChangeEvent&)>;

/// A registered file watch entry.
struct FileWatch
{
    std::string watch_id;     ///< Unique ID
    std::string directory;    ///< Directory being watched
    std::string glob_pattern; ///< File pattern to match (e.g., "*.md")
    FileWatchCallback callback;
    bool recursive{true}; ///< Watch subdirectories
    int debounce_ms{300}; ///< Debounce interval in milliseconds
    bool enabled{true};
};

/// Service that manages file system watches with debounce and glob filtering.
///
/// Allows registering directory watches with glob patterns. Changes are
/// debounced to avoid notification storms during bulk operations.
/// This is a logical watcher — actual OS-level watching would be integrated
/// at the platform layer.
class FileWatcherService
{
public:
    FileWatcherService() = default;

    // ── Watch management ──────────────────────────────────────────────
    void add_watch(FileWatch watch);
    auto remove_watch(const std::string& watch_id) -> bool;
    [[nodiscard]] auto find_watch(const std::string& watch_id) const -> const FileWatch*;
    [[nodiscard]] auto all_watches() const -> const std::vector<FileWatch>&;
    [[nodiscard]] auto watch_count() const -> int;

    // ── State ─────────────────────────────────────────────────────────
    auto enable_watch(const std::string& watch_id) -> bool;
    auto disable_watch(const std::string& watch_id) -> bool;
    void enable_all();
    void disable_all();

    // ── Event processing ──────────────────────────────────────────────
    /// Simulate a file change event and notify matching watchers.
    void notify_change(const FileChangeEvent& event);

    /// Get the event history.
    [[nodiscard]] auto event_history() const -> const std::vector<FileChangeEvent>&;
    [[nodiscard]] auto event_count() const -> int;

    // ── Queries ───────────────────────────────────────────────────────
    [[nodiscard]] auto watches_for_directory(const std::string& directory) const
        -> std::vector<const FileWatch*>;

    void clear_watches();
    void clear_history();

private:
    std::vector<FileWatch> watches_;
    std::vector<FileChangeEvent> history_;

    [[nodiscard]] static auto matches_glob(const std::string& file_path, const std::string& pattern)
        -> bool;
    [[nodiscard]] static auto is_under_directory(const std::string& file_path,
                                                 const std::string& directory) -> bool;
};

} // namespace markamp::core
