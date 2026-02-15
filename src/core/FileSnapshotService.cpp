/// @file FileSnapshotService.cpp
/// @brief V4 Phase 39 – File Snapshot Service implementation.

#include "core/FileSnapshotService.h"

#include "core/EventBus.h"
#include "core/Events.h"

#include <algorithm>
#include <cstdint>
#include <iomanip>
#include <sstream>

namespace markamp::core
{

// ============================================================================
// Constructor
// ============================================================================

FileSnapshotService::FileSnapshotService(EventBus& event_bus)
    : event_bus_(event_bus)
{
}

// ============================================================================
// Create snapshot
// ============================================================================

auto FileSnapshotService::create_snapshot(const std::string& path, const std::string& content)
    -> std::string
{
    const std::string hash = compute_hash(content);

    // Check for deduplication: if last snapshot has same hash, skip.
    auto file_it = file_snapshots_.find(path);
    if (file_it != file_snapshots_.end() && !file_it->second.empty())
    {
        const auto& last_id = file_it->second.back();
        auto info_it = snapshot_infos_.find(last_id);
        if (info_it != snapshot_infos_.end() && info_it->second.content_hash == hash)
        {
            return last_id; // Content unchanged, return existing snapshot.
        }
    }

    const std::string sid = "snap-" + std::to_string(next_id_++);

    SnapshotInfo info;
    info.snapshot_id = sid;
    info.file_path = path;
    info.timestamp = current_timestamp();
    info.size_bytes = content.size();
    info.content_hash = hash;

    snapshot_contents_[sid] = content;
    snapshot_infos_[sid] = info;
    file_snapshots_[path].push_back(sid);

    events::SnapshotCreatedEvent event;
    event.snapshot_id = sid;
    event.file_path = path;
    event_bus_.publish(event);

    return sid;
}

// ============================================================================
// List snapshots
// ============================================================================

auto FileSnapshotService::list_snapshots(const std::string& path) const -> std::vector<SnapshotInfo>
{
    std::vector<SnapshotInfo> result;

    auto file_it = file_snapshots_.find(path);
    if (file_it == file_snapshots_.end())
    {
        return result;
    }

    // Return in reverse order (newest first).
    for (auto snap_it = file_it->second.rbegin(); snap_it != file_it->second.rend(); ++snap_it)
    {
        auto info_it = snapshot_infos_.find(*snap_it);
        if (info_it != snapshot_infos_.end())
        {
            result.push_back(info_it->second);
        }
    }

    return result;
}

// ============================================================================
// Get snapshot
// ============================================================================

auto FileSnapshotService::get_snapshot(const std::string& snapshot_id) const
    -> std::optional<std::string>
{
    auto content_it = snapshot_contents_.find(snapshot_id);
    if (content_it != snapshot_contents_.end())
    {
        return content_it->second;
    }
    return std::nullopt;
}

auto FileSnapshotService::get_snapshot_info(const std::string& snapshot_id) const
    -> std::optional<SnapshotInfo>
{
    auto info_it = snapshot_infos_.find(snapshot_id);
    if (info_it != snapshot_infos_.end())
    {
        return info_it->second;
    }
    return std::nullopt;
}

// ============================================================================
// Restore snapshot
// ============================================================================

auto FileSnapshotService::restore_snapshot(const std::string& snapshot_id)
    -> std::optional<std::string>
{
    auto content = get_snapshot(snapshot_id);
    if (content.has_value())
    {
        events::SnapshotRestoredEvent event;
        event.snapshot_id = snapshot_id;
        event_bus_.publish(event);
    }
    return content;
}

// ============================================================================
// Diff snapshots
// ============================================================================

auto FileSnapshotService::diff_snapshots(const std::string& id_a, const std::string& id_b) const
    -> SnapshotDiff
{
    SnapshotDiff result;

    auto content_a = get_snapshot(id_a);
    auto content_b = get_snapshot(id_b);

    if (!content_a.has_value() || !content_b.has_value())
    {
        return result;
    }

    // Split into lines.
    auto split_lines = [](const std::string& text) -> std::vector<std::string>
    {
        std::vector<std::string> lines;
        std::istringstream stream(text);
        std::string line;
        while (std::getline(stream, line))
        {
            lines.push_back(line);
        }
        return lines;
    };

    const auto lines_a = split_lines(*content_a);
    const auto lines_b = split_lines(*content_b);

    // Simple line-by-line comparison.
    const size_t max_lines = std::max(lines_a.size(), lines_b.size());

    std::ostringstream diff_oss;
    diff_oss << "--- a\n+++ b\n";

    for (size_t idx = 0; idx < max_lines; ++idx)
    {
        const bool has_a = idx < lines_a.size();
        const bool has_b = idx < lines_b.size();

        if (has_a && has_b)
        {
            if (lines_a[idx] != lines_b[idx])
            {
                diff_oss << "-" << lines_a[idx] << "\n";
                diff_oss << "+" << lines_b[idx] << "\n";
                ++result.changed_lines;
            }
            else
            {
                diff_oss << " " << lines_a[idx] << "\n";
            }
        }
        else if (has_a)
        {
            diff_oss << "-" << lines_a[idx] << "\n";
            ++result.removed_lines;
        }
        else if (has_b)
        {
            diff_oss << "+" << lines_b[idx] << "\n";
            ++result.added_lines;
        }
    }

    result.unified_diff = diff_oss.str();
    return result;
}

// ============================================================================
// Prune old snapshots
// ============================================================================

auto FileSnapshotService::prune_old(const std::string& path, int max_count) -> int
{
    const int limit = (max_count >= 0) ? max_count : config_.max_snapshots_per_file;

    auto file_it = file_snapshots_.find(path);
    if (file_it == file_snapshots_.end())
    {
        return 0;
    }

    auto& ids = file_it->second;
    int pruned = 0;

    while (static_cast<int>(ids.size()) > limit)
    {
        const std::string old_id = ids.front();
        snapshot_contents_.erase(old_id);
        snapshot_infos_.erase(old_id);
        ids.erase(ids.begin());
        ++pruned;
    }

    if (pruned > 0)
    {
        events::SnapshotsPrunedEvent event;
        event.file_path = path;
        event.pruned_count = pruned;
        event_bus_.publish(event);
    }

    return pruned;
}

// ============================================================================
// Configuration
// ============================================================================

auto FileSnapshotService::set_config(const SnapshotConfig& config) -> void
{
    config_ = config;
}

// ============================================================================
// Accessors
// ============================================================================

auto FileSnapshotService::snapshot_count(const std::string& path) const -> int
{
    auto file_it = file_snapshots_.find(path);
    if (file_it != file_snapshots_.end())
    {
        return static_cast<int>(file_it->second.size());
    }
    return 0;
}

auto FileSnapshotService::total_storage_bytes() const -> size_t
{
    size_t total = 0;
    for (const auto& [_id, content] : snapshot_contents_)
    {
        total += content.size();
    }
    return total;
}

// ============================================================================
// Helpers
// ============================================================================

auto FileSnapshotService::compute_hash(const std::string& content) -> std::string
{
    // Simple FNV-1a hash for deduplication.
    uint64_t hash = 14695981039346656037ULL;
    for (const auto byte : content)
    {
        hash ^= static_cast<uint64_t>(static_cast<unsigned char>(byte));
        hash *= 1099511628211ULL;
    }

    std::ostringstream oss;
    oss << std::hex << std::setfill('0') << std::setw(16) << hash;
    return oss.str();
}

auto FileSnapshotService::current_timestamp() -> std::string
{
    // Return a stub timestamp for testability.
    static int counter = 0;
    return "2026-01-01T00:00:" + std::to_string(counter++) + "Z";
}

} // namespace markamp::core
