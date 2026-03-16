// ============================================================================
// File: src/core/SyncEngine.cpp
// Phase 37: Local Sync Engine — SyncEngine implementation
// ============================================================================

#include "SyncEngine.h"

#include "Events.h"

#include <algorithm>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <sstream>

namespace markamp::core
{

SyncEngine::SyncEngine(EventBus& event_bus)
    : event_bus_(event_bus)
{
}

auto SyncEngine::full_sync(const SyncConfig& config, SyncProgressCallback progress) -> SyncResult
{
    std::lock_guard lock(mutex_);
    auto start = std::chrono::steady_clock::now();
    SyncResult result;
    result.status = SyncStatus::Scanning;

    // Generate snapshots of both directories.
    auto local_snap = generate_snapshot(config.local_root);
    if (!local_snap)
    {
        result.status = SyncStatus::Failed;
        result.errors.push_back("Local scan failed: " + local_snap.error());
        return result;
    }

    auto remote_snap = generate_snapshot(config.remote_root);
    if (!remote_snap)
    {
        result.status = SyncStatus::Failed;
        result.errors.push_back("Remote scan failed: " + remote_snap.error());
        return result;
    }

    result.status = SyncStatus::Comparing;

    // Compute diffs from last known state.
    auto local_diff = diff_snapshots(last_snapshot_, *local_snap);
    auto remote_diff = diff_snapshots(last_snapshot_, *remote_snap);

    // Detect conflicts.
    auto conflicts = detect_conflicts(local_diff, remote_diff);
    result.conflicts_found = static_cast<int32_t>(conflicts.size());

    // Resolve conflicts.
    result.status = SyncStatus::Resolving;
    resolve_conflicts(conflicts, config.conflict_resolution);
    for (const auto& c : conflicts)
    {
        if (c.resolved)
            ++result.conflicts_resolved;
        else
            result.unresolved_conflicts.push_back(c);
    }

    // Apply changes.
    result.status = SyncStatus::Syncing;

    if (config.direction == SyncDirection::Bidirectional ||
        config.direction == SyncDirection::PushOnly)
    {
        // Push local changes to remote.
        auto push_result =
            apply_changes(local_diff, config.local_root, config.remote_root, progress);
        if (push_result)
        {
            result.files_added += *push_result;
        }
        else
        {
            result.errors.push_back("Push failed: " + push_result.error());
        }
    }

    if (config.direction == SyncDirection::Bidirectional ||
        config.direction == SyncDirection::PullOnly)
    {
        // Pull remote changes to local.
        auto pull_result =
            apply_changes(remote_diff, config.remote_root, config.local_root, progress);
        if (pull_result)
        {
            result.files_modified += *pull_result;
        }
        else
        {
            result.errors.push_back("Pull failed: " + pull_result.error());
        }
    }

    // Update last snapshot.
    last_snapshot_ = *local_snap;

    auto end = std::chrono::steady_clock::now();
    result.elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    result.status = result.errors.empty() ? SyncStatus::Completed : SyncStatus::Failed;
    return result;
}

auto SyncEngine::generate_snapshot(const std::filesystem::path& root)
    -> std::expected<SyncSnapshot, std::string>
{
    if (!std::filesystem::exists(root))
    {
        return std::unexpected("Directory not found: " + root.string());
    }

    SyncSnapshot snapshot;
    snapshot.root_path = root;
    snapshot.created_at = std::chrono::duration_cast<std::chrono::seconds>(
                              std::chrono::system_clock::now().time_since_epoch())
                              .count();

    std::error_code ec;
    for (const auto& entry : std::filesystem::recursive_directory_iterator(root, ec))
    {
        if (ec)
            continue;

        auto rel = std::filesystem::relative(entry.path(), root, ec);
        if (ec)
            continue;

        SyncFileEntry file_entry;
        file_entry.relative_path = rel;
        file_entry.is_directory = entry.is_directory();

        if (!entry.is_directory())
        {
            file_entry.size_bytes = static_cast<int64_t>(entry.file_size(ec));
            file_entry.hash = hash_file(entry.path());
            auto ftime = entry.last_write_time(ec);
            // Portable conversion: use file_time_type duration since epoch.
            auto duration = ftime.time_since_epoch();
            file_entry.modified_at =
                std::chrono::duration_cast<std::chrono::seconds>(duration).count();
        }

        snapshot.files[rel.string()] = std::move(file_entry);
    }

    return snapshot;
}

auto SyncEngine::diff_snapshots(const SyncSnapshot& old_snap, const SyncSnapshot& new_snap) const
    -> SnapshotDiff
{
    SnapshotDiff diff;

    // Find added and modified files.
    for (const auto& [path, new_entry] : new_snap.files)
    {
        if (new_entry.is_directory)
            continue;

        auto old_it = old_snap.files.find(path);
        if (old_it == old_snap.files.end())
        {
            // Added.
            SnapshotDiffEntry de;
            de.relative_path = new_entry.relative_path;
            de.change_type = FileChangeType::Added;
            de.new_entry = new_entry;
            diff.changes.push_back(std::move(de));
        }
        else if (old_it->second.hash != new_entry.hash)
        {
            // Modified.
            SnapshotDiffEntry de;
            de.relative_path = new_entry.relative_path;
            de.change_type = FileChangeType::Modified;
            de.old_entry = old_it->second;
            de.new_entry = new_entry;
            diff.changes.push_back(std::move(de));
        }
    }

    // Find deleted files.
    for (const auto& [path, old_entry] : old_snap.files)
    {
        if (old_entry.is_directory)
            continue;
        if (new_snap.files.find(path) == new_snap.files.end())
        {
            SnapshotDiffEntry de;
            de.relative_path = old_entry.relative_path;
            de.change_type = FileChangeType::Deleted;
            de.old_entry = old_entry;
            diff.changes.push_back(std::move(de));
        }
    }

    return diff;
}

auto SyncEngine::apply_changes(const SnapshotDiff& diff,
                               const std::filesystem::path& source_root,
                               const std::filesystem::path& target_root,
                               SyncProgressCallback progress) -> std::expected<int32_t, std::string>
{
    int32_t applied = 0;
    int total = static_cast<int>(diff.changes.size());

    for (int i = 0; i < total; ++i)
    {
        const auto& change = diff.changes[static_cast<size_t>(i)];

        if (progress)
        {
            progress(change.relative_path.string(), total > 0 ? (i * 100 / total) : 100);
        }

        auto source = source_root / change.relative_path;
        auto target = target_root / change.relative_path;

        switch (change.change_type)
        {
            case FileChangeType::Added:
            case FileChangeType::Modified:
            {
                auto res = copy_file_safe(source, target);
                if (res)
                    ++applied;
                break;
            }
            case FileChangeType::Deleted:
            {
                auto res = delete_file_safe(target);
                if (res)
                    ++applied;
                break;
            }
            case FileChangeType::Renamed:
            {
                // Handle rename as copy-new + delete-old in sync target.
                // The new_entry has the new path, old_entry has the old path.
                if (!change.old_entry.relative_path.empty() &&
                    !change.new_entry.relative_path.empty())
                {
                    auto old_target = target_root / change.old_entry.relative_path;
                    auto new_target = target_root / change.new_entry.relative_path;
                    auto new_source = source_root / change.new_entry.relative_path;

                    // Copy from source to new target location.
                    auto copy_res = copy_file_safe(new_source, new_target);
                    if (copy_res)
                    {
                        // Remove old target file.
                        auto del_res = delete_file_safe(old_target);
                        (void)del_res;
                        ++applied;
                    }
                }
                break;
            }
        }
    }

    return applied;
}

auto SyncEngine::detect_conflicts(const SnapshotDiff& local_diff,
                                  const SnapshotDiff& remote_diff) const
    -> std::vector<SyncConflict>
{
    std::vector<SyncConflict> conflicts;

    // Build a map of remote changes by path.
    std::unordered_map<std::string, const SnapshotDiffEntry*> remote_map;
    for (const auto& entry : remote_diff.changes)
    {
        remote_map[entry.relative_path.string()] = &entry;
    }

    // Check for local changes that also appear in remote.
    for (const auto& local_entry : local_diff.changes)
    {
        auto it = remote_map.find(local_entry.relative_path.string());
        if (it != remote_map.end())
        {
            SyncConflict conflict;
            conflict.relative_path = local_entry.relative_path;
            conflict.local_entry = local_entry.new_entry;
            conflict.remote_entry = it->second->new_entry;
            conflict.local_change = local_entry.change_type;
            conflict.remote_change = it->second->change_type;
            conflicts.push_back(std::move(conflict));
        }
    }

    return conflicts;
}

auto SyncEngine::resolve_conflicts(std::vector<SyncConflict>& conflicts,
                                   SyncConflictResolution strategy) -> void
{
    for (auto& conflict : conflicts)
    {
        switch (strategy)
        {
            case SyncConflictResolution::KeepLocal:
                conflict.resolution = SyncConflictResolution::KeepLocal;
                conflict.resolved = true;
                break;
            case SyncConflictResolution::KeepRemote:
                conflict.resolution = SyncConflictResolution::KeepRemote;
                conflict.resolved = true;
                break;
            case SyncConflictResolution::KeepNewer:
                conflict.resolution = SyncConflictResolution::KeepNewer;
                if (conflict.local_entry.modified_at >= conflict.remote_entry.modified_at)
                {
                    conflict.resolution = SyncConflictResolution::KeepLocal;
                }
                else
                {
                    conflict.resolution = SyncConflictResolution::KeepRemote;
                }
                conflict.resolved = true;
                break;
            case SyncConflictResolution::KeepBoth:
                conflict.resolution = SyncConflictResolution::KeepBoth;
                conflict.resolved = true;
                break;
            case SyncConflictResolution::AskUser:
                conflict.resolved = false;
                break;
        }
    }
}

auto SyncEngine::last_snapshot() const -> const SyncSnapshot&
{
    return last_snapshot_;
}

auto SyncEngine::set_last_snapshot(SyncSnapshot snapshot) -> void
{
    std::lock_guard lock(mutex_);
    last_snapshot_ = std::move(snapshot);
}

auto SyncEngine::hash_file(const std::filesystem::path& path) -> std::string
{
    std::ifstream file(path, std::ios::binary);
    if (!file)
        return "";

    uint64_t hash = 14695981039346656037ULL; // FNV-1a
    char buf[8192];
    while (file.read(buf, sizeof(buf)) || file.gcount() > 0)
    {
        for (std::streamsize i = 0; i < file.gcount(); ++i)
        {
            hash ^= static_cast<uint8_t>(buf[i]);
            hash *= 1099511628211ULL;
        }
    }

    std::ostringstream ss;
    ss << std::hex << std::setfill('0') << std::setw(16) << hash;
    return ss.str();
}

auto SyncEngine::is_excluded(const std::filesystem::path& relative_path,
                             const std::vector<std::string>& patterns) -> bool
{
    auto path_str = relative_path.string();
    for (const auto& pattern : patterns)
    {
        // Simple glob: support * wildcard.
        if (pattern == "*")
            return true;
        if (pattern.front() == '.')
        {
            // Extension match.
            if (path_str.ends_with(pattern))
                return true;
        }
        if (path_str.find(pattern) != std::string::npos)
            return true;
    }
    return false;
}

auto SyncEngine::copy_file_safe(const std::filesystem::path& source,
                                const std::filesystem::path& target)
    -> std::expected<void, std::string>
{
    std::error_code ec;
    std::filesystem::create_directories(target.parent_path(), ec);
    if (ec)
        return std::unexpected("Cannot create directory: " + ec.message());

    std::filesystem::copy_file(
        source, target, std::filesystem::copy_options::overwrite_existing, ec);
    if (ec)
        return std::unexpected("Copy failed: " + ec.message());

    return {};
}

auto SyncEngine::delete_file_safe(const std::filesystem::path& path)
    -> std::expected<void, std::string>
{
    std::error_code ec;
    if (std::filesystem::exists(path, ec))
    {
        std::filesystem::remove(path, ec);
        if (ec)
            return std::unexpected("Delete failed: " + ec.message());
    }
    return {};
}

} // namespace markamp::core
