#pragma once

#include "Config.h"

#include <filesystem>
#include <vector>

namespace markamp::core
{

/// Tracks recently opened files, persisted via Config.
class RecentFiles
{
public:
    explicit RecentFiles(Config& config);

    /// Add a path to the recent list (moves to front if already present).
    void add(const std::filesystem::path& path);

    /// Remove a specific path from the recent list.
    void remove(const std::filesystem::path& path);

    /// Clear all recent entries.
    void clear();

    /// Get the current list of recent files (most recent first).
    [[nodiscard]] auto list() const -> const std::vector<std::filesystem::path>&;

    /// Maximum number of entries to keep.
    [[nodiscard]] auto max_entries() const -> std::size_t;

    /// Load recent files from config.
    void load();

    /// Persist recent files to config.
    void save();

    static constexpr std::size_t kMaxEntries = 20;

private:
    Config& config_;
    std::vector<std::filesystem::path> entries_;

    /// Remove entries that no longer exist on disk.
    void prune_nonexistent();
};

} // namespace markamp::core
