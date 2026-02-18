/// @file FileWatcherService.cpp
/// @brief V9 Phase 40 — FileWatcherService implementation.

#include "FileWatcherService.h"

#include <algorithm>

namespace markamp::core
{

void FileWatcherService::add_watch(FileWatch watch)
{
    if (watch.watch_id.empty())
    {
        watch.watch_id = "watch_" + std::to_string(watches_.size());
    }
    watches_.push_back(std::move(watch));
}

auto FileWatcherService::remove_watch(const std::string& watch_id) -> bool
{
    auto iter = std::remove_if(watches_.begin(),
                               watches_.end(),
                               [&](const FileWatch& watch) { return watch.watch_id == watch_id; });
    if (iter == watches_.end())
    {
        return false;
    }
    watches_.erase(iter, watches_.end());
    return true;
}

auto FileWatcherService::find_watch(const std::string& watch_id) const -> const FileWatch*
{
    for (const auto& watch : watches_)
    {
        if (watch.watch_id == watch_id)
        {
            return &watch;
        }
    }
    return nullptr;
}

auto FileWatcherService::all_watches() const -> const std::vector<FileWatch>&
{
    return watches_;
}

auto FileWatcherService::watch_count() const -> int
{
    return static_cast<int>(watches_.size());
}

auto FileWatcherService::enable_watch(const std::string& watch_id) -> bool
{
    for (auto& watch : watches_)
    {
        if (watch.watch_id == watch_id)
        {
            watch.enabled = true;
            return true;
        }
    }
    return false;
}

auto FileWatcherService::disable_watch(const std::string& watch_id) -> bool
{
    for (auto& watch : watches_)
    {
        if (watch.watch_id == watch_id)
        {
            watch.enabled = false;
            return true;
        }
    }
    return false;
}

void FileWatcherService::enable_all()
{
    for (auto& watch : watches_)
    {
        watch.enabled = true;
    }
}

void FileWatcherService::disable_all()
{
    for (auto& watch : watches_)
    {
        watch.enabled = false;
    }
}

void FileWatcherService::notify_change(const FileChangeEvent& event)
{
    history_.push_back(event);

    for (const auto& watch : watches_)
    {
        if (!watch.enabled)
        {
            continue;
        }
        if (!is_under_directory(event.file_path, watch.directory))
        {
            continue;
        }
        if (!watch.glob_pattern.empty() && !matches_glob(event.file_path, watch.glob_pattern))
        {
            continue;
        }
        if (watch.callback)
        {
            watch.callback(event);
        }
    }
}

auto FileWatcherService::event_history() const -> const std::vector<FileChangeEvent>&
{
    return history_;
}

auto FileWatcherService::event_count() const -> int
{
    return static_cast<int>(history_.size());
}

auto FileWatcherService::watches_for_directory(const std::string& directory) const
    -> std::vector<const FileWatch*>
{
    std::vector<const FileWatch*> result;
    for (const auto& watch : watches_)
    {
        if (watch.directory == directory)
        {
            result.push_back(&watch);
        }
    }
    return result;
}

void FileWatcherService::clear_watches()
{
    watches_.clear();
}

void FileWatcherService::clear_history()
{
    history_.clear();
}

auto FileWatcherService::matches_glob(const std::string& file_path, const std::string& pattern)
    -> bool
{
    // Simple suffix matching: "*.md" matches any path ending in ".md"
    if (pattern.size() >= 2 && pattern[0] == '*' && pattern[1] == '.')
    {
        auto extension = pattern.substr(1); // ".md"
        if (file_path.size() >= extension.size())
        {
            return file_path.compare(
                       file_path.size() - extension.size(), extension.size(), extension) == 0;
        }
        return false;
    }
    // Exact match or substring
    return file_path.find(pattern) != std::string::npos;
}

auto FileWatcherService::is_under_directory(const std::string& file_path,
                                            const std::string& directory) -> bool
{
    if (directory.empty())
    {
        return true;
    }
    return file_path.find(directory) == 0;
}

} // namespace markamp::core
