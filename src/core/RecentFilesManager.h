#pragma once

/// @file RecentFilesManager.h
/// @brief Phase 35 – Most Recently Used file list with frecency scoring.

#include "FrecencyTracker.h"

#include <chrono>
#include <string>
#include <unordered_set>
#include <vector>

namespace markamp::core
{

class Config;

/// An entry in the MRU file list.
struct RecentFileEntry
{
    std::string file_path;
    std::string display_name;
    double frecency_score{0.0};
    bool is_pinned{false};
    std::chrono::system_clock::time_point last_opened{};
};

/// Manages the Most Recently Used file list with frecency scoring and pinning.
class RecentFilesManager
{
public:
    /// Maximum number of tracked files.
    static constexpr std::size_t kMaxTrackedFiles = 500;

    /// Maximum number of pinned files.
    static constexpr std::size_t kMaxPinnedFiles = 10;

    explicit RecentFilesManager(FrecencyTracker& frecency_tracker);

    /// Record a file being opened.
    void record_open(const std::string& file_path);

    /// Get the ranked list of recent files.
    /// Pinned files always appear first, then by frecency score.
    [[nodiscard]] auto ranked_files(std::size_t limit = 20) const -> std::vector<RecentFileEntry>;

    /// Pin a file to the top of the list.
    void pin_file(const std::string& file_path);

    /// Unpin a file.
    void unpin_file(const std::string& file_path);

    /// Check if a file is pinned.
    [[nodiscard]] auto is_pinned(const std::string& file_path) const -> bool;

    /// Remove a file from the list entirely.
    void remove_file(const std::string& file_path);

    /// Get the number of tracked files.
    [[nodiscard]] auto tracked_count() const -> std::size_t;

    /// Get the number of pinned files.
    [[nodiscard]] auto pinned_count() const -> std::size_t;

    /// Clear all tracked files and pins.
    void clear();

private:
    FrecencyTracker& frecency_tracker_;
    std::unordered_set<std::string> pinned_files_;
    std::unordered_set<std::string> removed_files_;

    /// Extract display name from a file path.
    [[nodiscard]] static auto extract_display_name(const std::string& file_path) -> std::string;
};

} // namespace markamp::core
