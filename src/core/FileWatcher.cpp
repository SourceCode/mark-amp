/// @file FileWatcher.cpp
/// @brief Phase 20 – Polling-based file system change detection implementation.

#include "core/FileWatcher.h"

#include "core/EventBus.h"
#include "core/Events.h"

#include <algorithm>
#include <chrono>
#include <filesystem>

namespace markamp::core
{

namespace fs = std::filesystem;

// ============================================================================
// Construction
// ============================================================================

FileWatcher::FileWatcher(EventBus& event_bus)
    : event_bus_(event_bus)
{
}

// ============================================================================
// Watch Management
// ============================================================================

auto FileWatcher::watch(const std::string& path) -> bool
{
    if (watches_.contains(path))
    {
        return false; // Already watching.
    }

    WatchEntry entry;
    entry.root_path = path;

    std::error_code error_code;
    entry.is_directory = fs::is_directory(path, error_code);

    if (entry.is_directory)
    {
        scan_directory(entry);
    }
    else if (fs::exists(path, error_code))
    {
        TrackedFile tracked;
        tracked.path = path;
        tracked.last_write_time = fs::last_write_time(path, error_code);
        tracked.last_size = fs::file_size(path, error_code);
        tracked.exists = true;
        entry.files[path] = std::move(tracked);
    }
    else
    {
        return false; // Path does not exist.
    }

    watches_[path] = std::move(entry);
    return true;
}

auto FileWatcher::unwatch(const std::string& path) -> bool
{
    return watches_.erase(path) > 0;
}

auto FileWatcher::is_watching(const std::string& path) const -> bool
{
    return watches_.contains(path);
}

auto FileWatcher::watched_paths() const -> std::vector<std::string>
{
    std::vector<std::string> paths;
    paths.reserve(watches_.size());
    for (const auto& [root_path, entry] : watches_)
    {
        paths.push_back(root_path);
    }
    return paths;
}

auto FileWatcher::watched_count() const -> std::size_t
{
    return watches_.size();
}

// ============================================================================
// Polling
// ============================================================================

auto FileWatcher::poll() -> int
{
    int change_count = 0;

    for (auto& [root_path, entry] : watches_)
    {
        auto changes = detect_changes(entry);
        for (const auto& change : changes)
        {
            notify(change);
            ++change_count;
        }
    }

    total_changes_ += static_cast<std::size_t>(change_count);
    return change_count;
}

// ============================================================================
// Callbacks
// ============================================================================

auto FileWatcher::on_change(ChangeCallback callback) -> std::size_t
{
    auto callback_id = next_callback_id_++;
    callbacks_.emplace_back(callback_id, std::move(callback));
    return callback_id;
}

void FileWatcher::remove_callback(std::size_t callback_id)
{
    callbacks_.erase(std::remove_if(callbacks_.begin(),
                                    callbacks_.end(),
                                    [callback_id](const auto& pair)
                                    { return pair.first == callback_id; }),
                     callbacks_.end());
}

// ============================================================================
// Configuration
// ============================================================================

void FileWatcher::set_debounce_ms(int milliseconds)
{
    debounce_ms_ = milliseconds;
}

auto FileWatcher::debounce_ms() const -> int
{
    return debounce_ms_;
}

void FileWatcher::set_recursive(bool recursive)
{
    recursive_ = recursive;
}

auto FileWatcher::is_recursive() const -> bool
{
    return recursive_;
}

// ============================================================================
// Statistics
// ============================================================================

auto FileWatcher::total_changes_detected() const -> std::size_t
{
    return total_changes_;
}

void FileWatcher::clear()
{
    watches_.clear();
}

// ============================================================================
// Private Helpers
// ============================================================================

void FileWatcher::scan_directory(WatchEntry& entry)
{
    std::error_code error_code;

    auto scan = [&](auto iterator)
    {
        for (const auto& dir_entry : iterator)
        {
            if (dir_entry.is_regular_file(error_code))
            {
                const auto path_str = dir_entry.path().string();
                TrackedFile tracked;
                tracked.path = path_str;
                tracked.last_write_time = dir_entry.last_write_time(error_code);
                tracked.last_size = dir_entry.file_size(error_code);
                tracked.exists = true;
                entry.files[path_str] = std::move(tracked);
            }
        }
    };

    if (recursive_)
    {
        scan(fs::recursive_directory_iterator(entry.root_path, error_code));
    }
    else
    {
        scan(fs::directory_iterator(entry.root_path, error_code));
    }
}

auto FileWatcher::detect_changes(WatchEntry& entry) -> std::vector<FileChangeEvent>
{
    std::vector<FileChangeEvent> changes;
    std::error_code error_code;

    if (entry.is_directory)
    {
        // Check for deleted files.
        std::vector<std::string> deleted_paths;
        for (auto& [file_path, tracked] : entry.files)
        {
            if (!fs::exists(file_path, error_code))
            {
                if (tracked.exists)
                {
                    FileChangeEvent evt;
                    evt.path = file_path;
                    evt.change_type = FileChangeType::kDeleted;
                    evt.timestamp = current_timestamp();
                    changes.push_back(std::move(evt));
                    deleted_paths.push_back(file_path);
                }
            }
        }

        for (const auto& deleted_path : deleted_paths)
        {
            entry.files.erase(deleted_path);
        }

        // Scan for new and modified files.
        auto process_entry = [&](const fs::directory_entry& dir_entry)
        {
            if (!dir_entry.is_regular_file(error_code))
            {
                return;
            }

            const auto path_str = dir_entry.path().string();
            auto file_iter = entry.files.find(path_str);

            if (file_iter == entry.files.end())
            {
                // New file.
                FileChangeEvent evt;
                evt.path = path_str;
                evt.change_type = FileChangeType::kCreated;
                evt.timestamp = current_timestamp();
                changes.push_back(std::move(evt));

                TrackedFile tracked;
                tracked.path = path_str;
                tracked.last_write_time = dir_entry.last_write_time(error_code);
                tracked.last_size = dir_entry.file_size(error_code);
                tracked.exists = true;
                entry.files[path_str] = std::move(tracked);
            }
            else
            {
                // Check for modification.
                auto current_write_time = dir_entry.last_write_time(error_code);
                if (current_write_time != file_iter->second.last_write_time)
                {
                    FileChangeEvent evt;
                    evt.path = path_str;
                    evt.change_type = FileChangeType::kModified;
                    evt.timestamp = current_timestamp();
                    changes.push_back(std::move(evt));

                    file_iter->second.last_write_time = current_write_time;
                    file_iter->second.last_size = dir_entry.file_size(error_code);
                }
            }
        };

        if (recursive_)
        {
            for (const auto& dir_entry :
                 fs::recursive_directory_iterator(entry.root_path, error_code))
            {
                process_entry(dir_entry);
            }
        }
        else
        {
            for (const auto& dir_entry : fs::directory_iterator(entry.root_path, error_code))
            {
                process_entry(dir_entry);
            }
        }
    }
    else
    {
        // Single file watch.
        auto file_iter = entry.files.find(entry.root_path);
        if (file_iter == entry.files.end())
        {
            return changes;
        }

        if (!fs::exists(entry.root_path, error_code))
        {
            if (file_iter->second.exists)
            {
                FileChangeEvent evt;
                evt.path = entry.root_path;
                evt.change_type = FileChangeType::kDeleted;
                evt.timestamp = current_timestamp();
                changes.push_back(std::move(evt));
                file_iter->second.exists = false;
            }
        }
        else
        {
            auto current_write_time = fs::last_write_time(entry.root_path, error_code);
            if (current_write_time != file_iter->second.last_write_time)
            {
                FileChangeEvent evt;
                evt.path = entry.root_path;
                evt.change_type = FileChangeType::kModified;
                evt.timestamp = current_timestamp();
                changes.push_back(std::move(evt));

                file_iter->second.last_write_time = current_write_time;
                file_iter->second.last_size = fs::file_size(entry.root_path, error_code);
            }
        }
    }

    return changes;
}

void FileWatcher::notify(const FileChangeEvent& event)
{
    for (const auto& [callback_id, callback] : callbacks_)
    {
        callback(event);
    }

    events::FileChangedEvent evt;
    evt.path = event.path;
    evt.change_type = static_cast<int>(event.change_type);
    event_bus_.publish(evt);
}

auto FileWatcher::current_timestamp() -> std::string
{
    auto now = std::chrono::system_clock::now();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);
    std::array<char, 32> buffer{};
    struct tm time_info
    {
    };
    gmtime_r(&time_t_now, &time_info);
    std::strftime(buffer.data(), buffer.size(), "%Y-%m-%dT%H:%M:%SZ", &time_info);
    return {buffer.data()};
}

} // namespace markamp::core
