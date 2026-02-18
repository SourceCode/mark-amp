// ============================================================================
// File: src/core/SelectiveSyncFilter.h
// Phase 27: Cloud Sync & Collaboration — Selective sync & ignore patterns
// ============================================================================
#pragma once

#include "CloudSyncTypes.h"

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::core
{

class Config;

// SelectiveSyncFilter — manages include/exclude patterns for sync.
// Combines .markamp-syncignore patterns with folder-level toggles.
class SelectiveSyncFilter
{
public:
    explicit SelectiveSyncFilter(Config& config);

    // Load patterns from workspace config or .markamp-syncignore file.
    auto load_patterns(const std::string& syncignore_content) -> void;

    // Pattern management.
    auto add_pattern(const std::string& pattern) -> void;
    auto remove_pattern(const std::string& pattern) -> bool;
    [[nodiscard]] auto patterns() const -> const std::vector<std::string>&;
    [[nodiscard]] auto pattern_count() const -> int32_t;

    // Check if a file should be synced.
    [[nodiscard]] auto should_sync(const std::string& relative_path) const -> bool;

    // Folder-level include/exclude.
    auto exclude_folder(const std::string& folder_path) -> void;
    auto include_folder(const std::string& folder_path) -> void;
    [[nodiscard]] auto excluded_folders() const -> const std::vector<std::string>&;
    [[nodiscard]] auto is_folder_excluded(const std::string& folder_path) const -> bool;

    // Cloud-only files (present remotely but excluded locally).
    auto add_cloud_only_file(const std::string& file_path) -> void;
    auto remove_cloud_only_file(const std::string& file_path) -> bool;
    [[nodiscard]] auto cloud_only_files() const -> const std::vector<std::string>&;
    [[nodiscard]] auto is_cloud_only(const std::string& file_path) const -> bool;

    // Default patterns management.
    auto set_use_defaults(bool use_defaults) -> void;
    [[nodiscard]] auto use_defaults() const -> bool;

    // Get effective patterns (user + defaults if enabled).
    [[nodiscard]] auto effective_patterns() const -> std::vector<std::string>;

    // Serialize patterns to syncignore format.
    [[nodiscard]] auto serialize() const -> std::string;

private:
    [[maybe_unused]] Config& config_;

    std::vector<std::string> user_patterns_;
    std::vector<std::string> excluded_folders_;
    std::vector<std::string> cloud_only_files_;
    bool use_defaults_{true};

    // Check if a path matches a single glob pattern.
    [[nodiscard]] static auto matches_pattern(const std::string& path, const std::string& pattern)
        -> bool;
};

} // namespace markamp::core
