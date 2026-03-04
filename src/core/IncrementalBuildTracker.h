#pragma once

/// @file IncrementalBuildTracker.h
/// @brief Phase 38 Task 27 — Track per-file build status for incremental builds.

#include <chrono>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

/// Build status for an individual source file.
enum class FileBuildStatus : uint8_t
{
    kUpToDate,
    kModified,
    kBuilding,
    kFailed,
    kUnknown,
};

/// Tracking info for a source file.
struct FileTrackingInfo
{
    std::string file_path;
    FileBuildStatus status{FileBuildStatus::kUnknown};
    std::string last_build_time; ///< ISO 8601 timestamp
    std::string content_hash;    ///< Hash of file contents at last build
    int error_count{0};
};

/// Tracks incremental build status for source files.
class IncrementalBuildTracker
{
public:
    IncrementalBuildTracker() = default;

    /// Update tracking info for a file.
    void update_file(const std::string& path, FileBuildStatus status);

    /// Get tracking info for a file.
    [[nodiscard]] auto get_file(const std::string& path) const -> const FileTrackingInfo*;

    /// Mark a file as modified.
    void mark_modified(const std::string& path);

    /// Mark a file as building.
    void mark_building(const std::string& path);

    /// Mark a file as built successfully.
    void mark_built(const std::string& path);

    /// Mark a file as build failed.
    void mark_failed(const std::string& path, int error_count = 1);

    /// Get all files with a given status.
    [[nodiscard]] auto files_with_status(FileBuildStatus status) const -> std::vector<std::string>;

    /// Get count of modified (dirty) files.
    [[nodiscard]] auto dirty_count() const -> std::size_t;

    /// Get count of failed files.
    [[nodiscard]] auto failed_count() const -> std::size_t;

    /// Total tracked files.
    [[nodiscard]] auto tracked_count() const -> std::size_t;

    /// Clear all tracking data.
    void clear();

    /// Reset all files to Unknown status.
    void reset_all();

private:
    std::unordered_map<std::string, FileTrackingInfo> files_;
};

} // namespace markamp::core
