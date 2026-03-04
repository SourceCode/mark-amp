/// @file IncrementalBuildTracker.cpp
/// @brief Phase 38 Task 27 — Incremental build tracking implementation.

#include "core/IncrementalBuildTracker.h"

namespace markamp::core
{

void IncrementalBuildTracker::update_file(const std::string& path, FileBuildStatus status)
{
    files_[path].file_path = path;
    files_[path].status = status;
}

auto IncrementalBuildTracker::get_file(const std::string& path) const -> const FileTrackingInfo*
{
    auto it = files_.find(path);
    if (it != files_.end())
        return &it->second;
    return nullptr;
}

void IncrementalBuildTracker::mark_modified(const std::string& path)
{
    update_file(path, FileBuildStatus::kModified);
}

void IncrementalBuildTracker::mark_building(const std::string& path)
{
    update_file(path, FileBuildStatus::kBuilding);
}

void IncrementalBuildTracker::mark_built(const std::string& path)
{
    update_file(path, FileBuildStatus::kUpToDate);
    files_[path].error_count = 0;
}

void IncrementalBuildTracker::mark_failed(const std::string& path, int error_count)
{
    update_file(path, FileBuildStatus::kFailed);
    files_[path].error_count = error_count;
}

auto IncrementalBuildTracker::files_with_status(FileBuildStatus status) const
    -> std::vector<std::string>
{
    std::vector<std::string> result;
    for (const auto& [path, info] : files_)
    {
        if (info.status == status)
            result.push_back(path);
    }
    return result;
}

auto IncrementalBuildTracker::dirty_count() const -> std::size_t
{
    return files_with_status(FileBuildStatus::kModified).size();
}

auto IncrementalBuildTracker::failed_count() const -> std::size_t
{
    return files_with_status(FileBuildStatus::kFailed).size();
}

auto IncrementalBuildTracker::tracked_count() const -> std::size_t
{
    return files_.size();
}

void IncrementalBuildTracker::clear()
{
    files_.clear();
}

void IncrementalBuildTracker::reset_all()
{
    for (auto& [unused_path, info] : files_)
    {
        info.status = FileBuildStatus::kUnknown;
        info.error_count = 0;
    }
}

} // namespace markamp::core
