#include "RecentFilesManager.h"

#include <algorithm>
#include <filesystem>

namespace markamp::core
{

RecentFilesManager::RecentFilesManager(FrecencyTracker& frecency_tracker)
    : frecency_tracker_(frecency_tracker)
{
}

void RecentFilesManager::record_open(const std::string& file_path)
{
    removed_files_.erase(file_path);
    frecency_tracker_.record_access(file_path);
}

auto RecentFilesManager::ranked_files(std::size_t limit) const -> std::vector<RecentFileEntry>
{
    auto ranked_keys = frecency_tracker_.ranked_keys(kMaxTrackedFiles);

    std::vector<RecentFileEntry> pinned_entries;
    std::vector<RecentFileEntry> regular_entries;

    for (const auto& key : ranked_keys)
    {
        if (removed_files_.count(key) > 0)
        {
            continue;
        }

        RecentFileEntry entry;
        entry.file_path = key;
        entry.display_name = extract_display_name(key);
        entry.frecency_score = frecency_tracker_.score(key);
        entry.is_pinned = pinned_files_.count(key) > 0;

        if (entry.is_pinned)
        {
            pinned_entries.push_back(std::move(entry));
        }
        else
        {
            regular_entries.push_back(std::move(entry));
        }
    }

    // Also add pinned files that might not be in ranked_keys
    for (const auto& pinned_path : pinned_files_)
    {
        if (removed_files_.count(pinned_path) > 0)
        {
            continue;
        }
        bool already_added = false;
        for (const auto& existing : pinned_entries)
        {
            if (existing.file_path == pinned_path)
            {
                already_added = true;
                break;
            }
        }
        if (!already_added)
        {
            RecentFileEntry entry;
            entry.file_path = pinned_path;
            entry.display_name = extract_display_name(pinned_path);
            entry.frecency_score = frecency_tracker_.score(pinned_path);
            entry.is_pinned = true;
            pinned_entries.push_back(std::move(entry));
        }
    }

    // Combine: pinned first, then regular
    std::vector<RecentFileEntry> result;
    result.reserve(pinned_entries.size() + regular_entries.size());
    for (auto& entry : pinned_entries)
    {
        result.push_back(std::move(entry));
    }
    for (auto& entry : regular_entries)
    {
        result.push_back(std::move(entry));
    }

    // Limit
    if (result.size() > limit)
    {
        result.resize(limit);
    }

    return result;
}

void RecentFilesManager::pin_file(const std::string& file_path)
{
    if (pinned_files_.size() < kMaxPinnedFiles)
    {
        pinned_files_.insert(file_path);
    }
}

void RecentFilesManager::unpin_file(const std::string& file_path)
{
    pinned_files_.erase(file_path);
}

auto RecentFilesManager::is_pinned(const std::string& file_path) const -> bool
{
    return pinned_files_.count(file_path) > 0;
}

void RecentFilesManager::remove_file(const std::string& file_path)
{
    removed_files_.insert(file_path);
    pinned_files_.erase(file_path);
}

auto RecentFilesManager::tracked_count() const -> std::size_t
{
    auto ranked = frecency_tracker_.ranked_keys(kMaxTrackedFiles);
    std::size_t count = 0;
    for (const auto& key : ranked)
    {
        if (removed_files_.count(key) == 0)
        {
            ++count;
        }
    }
    return count;
}

auto RecentFilesManager::pinned_count() const -> std::size_t
{
    return pinned_files_.size();
}

void RecentFilesManager::clear()
{
    pinned_files_.clear();
    removed_files_.clear();
    frecency_tracker_.clear();
}

auto RecentFilesManager::extract_display_name(const std::string& file_path) -> std::string
{
    std::filesystem::path path(file_path);
    return path.filename().string();
}

} // namespace markamp::core
