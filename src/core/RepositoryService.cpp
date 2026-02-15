// ============================================================================
// File: src/core/RepositoryService.cpp
// Phase 34: Repository System — RepositoryService implementation
// ============================================================================
#include "RepositoryService.h"

#include <nlohmann/json.hpp>

#include <algorithm>
#include <chrono>
#include <fstream>
#include <functional>
#include <sstream>

namespace markamp::core
{

RepositoryService::RepositoryService(EventBus& event_bus,
                                     Config& config,
                                     std::filesystem::path workspace_root)
    : event_bus_(event_bus)
    , config_(config)
    , workspace_root_(std::move(workspace_root))
    , repo_root_(workspace_root_ / ".markamp-repo")
{
}

auto RepositoryService::initialize() -> std::expected<void, std::string>
{
    std::error_code error_code;
    std::filesystem::create_directories(repo_root_, error_code);
    if (error_code)
    {
        return std::unexpected("Failed to create repo directory: " + error_code.message());
    }

    std::filesystem::create_directories(repo_root_ / "snapshots", error_code);
    return load_index();
}

auto RepositoryService::create_snapshot(const std::string& memo)
    -> std::expected<std::string, std::string>
{
    std::lock_guard lock(mutex_);

    // Generate snapshot ID
    auto now = std::chrono::system_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
    const auto snapshot_id = "snap-" + std::to_string(ms);

    // Collect files
    auto files_result = collect_workspace_files();
    if (!files_result.has_value())
    {
        return std::unexpected(files_result.error());
    }

    // Create archive
    auto archive_result = create_archive(snapshot_id, files_result.value());
    if (!archive_result.has_value())
    {
        return std::unexpected(archive_result.error());
    }

    // Calculate sizes
    std::size_t total_bytes = 0;
    for (const auto& file_path : files_result.value())
    {
        std::error_code error_code;
        total_bytes += std::filesystem::file_size(file_path, error_code);
    }

    std::error_code error_code;
    std::size_t compressed_bytes = std::filesystem::file_size(archive_result.value(), error_code);

    // Build snapshot
    Snapshot snapshot;
    snapshot.id = snapshot_id;
    snapshot.memo = memo;
    snapshot.root_path = workspace_root_.string();
    snapshot.storage_path = archive_result.value().string();
    snapshot.compression = repo_config().default_compression;
    snapshot.file_count = files_result.value().size();
    snapshot.total_bytes = total_bytes;
    snapshot.compressed_bytes = compressed_bytes;
    snapshot.created = now;

    snapshot_index_[snapshot_id] = std::move(snapshot);

    auto save_result = save_index();
    if (!save_result.has_value())
    {
        return std::unexpected(save_result.error());
    }

    return snapshot_id;
}

auto RepositoryService::tag_snapshot(const std::string& snapshot_id, const std::string& tag)
    -> std::expected<void, std::string>
{
    std::lock_guard lock(mutex_);

    auto found = snapshot_index_.find(snapshot_id);
    if (found == snapshot_index_.end())
    {
        return std::unexpected("Snapshot not found: " + snapshot_id);
    }

    found->second.tag = tag;
    return save_index();
}

auto RepositoryService::list_snapshots() const -> std::vector<Snapshot>
{
    std::lock_guard lock(mutex_);

    std::vector<Snapshot> snapshots;
    snapshots.reserve(snapshot_index_.size());

    for (const auto& [snap_id, snap] : snapshot_index_)
    {
        snapshots.push_back(snap);
    }

    // Newest first
    std::sort(snapshots.begin(),
              snapshots.end(),
              [](const Snapshot& left, const Snapshot& right)
              { return left.created > right.created; });

    return snapshots;
}

auto RepositoryService::get_snapshot(const std::string& snapshot_id) const
    -> std::expected<Snapshot, std::string>
{
    std::lock_guard lock(mutex_);

    auto found = snapshot_index_.find(snapshot_id);
    if (found == snapshot_index_.end())
    {
        return std::unexpected("Snapshot not found: " + snapshot_id);
    }

    return found->second;
}

auto RepositoryService::get_snapshot_by_tag(const std::string& tag) const
    -> std::expected<Snapshot, std::string>
{
    std::lock_guard lock(mutex_);

    for (const auto& [snap_id, snap] : snapshot_index_)
    {
        if (snap.tag == tag)
        {
            return snap;
        }
    }

    return std::unexpected("No snapshot with tag: " + tag);
}

auto RepositoryService::checkout_snapshot(const std::string& snapshot_id)
    -> std::expected<void, std::string>
{
    // Create a safety snapshot of current state first
    auto safety_result = create_snapshot("Pre-checkout safety snapshot");
    if (!safety_result.has_value())
    {
        return std::unexpected("Failed to create safety snapshot: " + safety_result.error());
    }

    // Extract the target snapshot
    return extract_archive(snapshot_id, workspace_root_);
}

auto RepositoryService::diff_snapshots(const std::string& old_snapshot_id,
                                       const std::string& new_snapshot_id) const
    -> std::expected<SnapshotDiff, std::string>
{
    std::lock_guard lock(mutex_);

    auto old_it = snapshot_index_.find(old_snapshot_id);
    auto new_it = snapshot_index_.find(new_snapshot_id);

    if (old_it == snapshot_index_.end())
    {
        return std::unexpected("Old snapshot not found: " + old_snapshot_id);
    }
    if (new_it == snapshot_index_.end())
    {
        return std::unexpected("New snapshot not found: " + new_snapshot_id);
    }

    // Stub: real implementation would extract both snapshots
    // and compare file manifests
    SnapshotDiff diff;
    diff.old_snapshot_id = old_snapshot_id;
    diff.new_snapshot_id = new_snapshot_id;
    diff.old_total_bytes = old_it->second.total_bytes;
    diff.new_total_bytes = new_it->second.total_bytes;

    return diff;
}

auto RepositoryService::purge_old_snapshots() -> std::expected<PurgeStats, std::string>
{
    std::lock_guard lock(mutex_);

    const auto config = repo_config();
    const auto cutoff =
        std::chrono::system_clock::now() - std::chrono::hours(config.retention_days * 24);

    PurgeStats stats;

    std::vector<std::string> to_remove;
    for (const auto& [snap_id, snap] : snapshot_index_)
    {
        if (snap.created < cutoff)
        {
            to_remove.push_back(snap_id);
        }
    }

    for (const auto& snap_id : to_remove)
    {
        auto& snap = snapshot_index_[snap_id];
        stats.bytes_freed += snap.compressed_bytes;

        std::error_code error_code;
        std::filesystem::remove(snap.storage_path, error_code);

        snapshot_index_.erase(snap_id);
        ++stats.snapshots_removed;
    }

    stats.snapshots_remaining = static_cast<int>(snapshot_index_.size());

    auto save_result = save_index();
    if (!save_result.has_value())
    {
        return std::unexpected(save_result.error());
    }

    return stats;
}

auto RepositoryService::delete_snapshot(const std::string& snapshot_id)
    -> std::expected<void, std::string>
{
    std::lock_guard lock(mutex_);

    auto found = snapshot_index_.find(snapshot_id);
    if (found == snapshot_index_.end())
    {
        return std::unexpected("Snapshot not found: " + snapshot_id);
    }

    std::error_code error_code;
    std::filesystem::remove(found->second.storage_path, error_code);

    snapshot_index_.erase(found);
    return save_index();
}

auto RepositoryService::repo_config() const -> RepositoryConfig
{
    return {};
}

auto RepositoryService::collect_workspace_files() const
    -> std::expected<std::vector<std::filesystem::path>, std::string>
{
    std::vector<std::filesystem::path> files;
    std::error_code error_code;

    for (const auto& entry :
         std::filesystem::recursive_directory_iterator(workspace_root_, error_code))
    {
        if (error_code)
        {
            return std::unexpected("Directory iteration error: " + error_code.message());
        }

        if (!entry.is_regular_file())
        {
            continue;
        }

        if (is_excluded(entry.path()))
        {
            continue;
        }

        files.push_back(entry.path());
    }

    return files;
}

auto RepositoryService::is_excluded(const std::filesystem::path& path) const -> bool
{
    const auto config = repo_config();
    const auto path_str = path.string();

    for (const auto& pattern : config.exclude_patterns)
    {
        if (path_str.find(pattern) != std::string::npos)
        {
            return true;
        }
    }

    // Always exclude the repo directory itself
    auto repo_str = repo_root_.string();
    if (path_str.starts_with(repo_str))
    {
        return true;
    }

    return false;
}

auto RepositoryService::create_archive(const std::string& snapshot_id,
                                       const std::vector<std::filesystem::path>& files) const
    -> std::expected<std::filesystem::path, std::string>
{
    auto archive_path = repo_root_ / "snapshots" / (snapshot_id + ".tar.gz");

    // Build tar command
    std::ostringstream cmd;
    cmd << "tar czf \"" << archive_path.string() << "\"";
    for (const auto& file : files)
    {
        auto relative = std::filesystem::relative(file, workspace_root_);
        cmd << " \"" << relative.string() << "\"";
    }

    // Note: in production, would use libarchive or similar
    // This is a stub that creates an empty file
    {
        std::ofstream ofs(archive_path, std::ios::binary);
        if (!ofs.is_open())
        {
            return std::unexpected("Failed to create archive: " + archive_path.string());
        }
        ofs << "MARKAMP-SNAPSHOT-V1"; // Stub header
    }

    return archive_path;
}

auto RepositoryService::extract_archive(const std::string& snapshot_id,
                                        const std::filesystem::path& /*target*/) const
    -> std::expected<void, std::string>
{
    std::lock_guard lock(mutex_);

    auto found = snapshot_index_.find(snapshot_id);
    if (found == snapshot_index_.end())
    {
        return std::unexpected("Snapshot not found: " + snapshot_id);
    }

    // Stub: would extract using tar or libarchive
    std::error_code error_code;
    if (!std::filesystem::exists(found->second.storage_path, error_code))
    {
        return std::unexpected("Snapshot archive not found: " + found->second.storage_path);
    }

    return {};
}

auto RepositoryService::load_index() -> std::expected<void, std::string>
{
    auto index_path = repo_root_ / "index.json";
    std::error_code error_code;

    if (!std::filesystem::exists(index_path, error_code) || error_code)
    {
        return {};
    }

    std::ifstream ifs(index_path);
    if (!ifs.is_open())
    {
        return std::unexpected("Failed to read snapshot index");
    }

    try
    {
        auto json = nlohmann::json::parse(ifs);

        for (const auto& entry : json)
        {
            Snapshot snap;
            snap.id = entry.value("id", "");
            snap.tag = entry.value("tag", "");
            snap.memo = entry.value("memo", "");
            snap.root_path = entry.value("root_path", "");
            snap.storage_path = entry.value("storage_path", "");
            snap.compression = static_cast<CompressionType>(entry.value("compression", 1));
            snap.file_count = entry.value("file_count", 0);
            snap.total_bytes = entry.value("total_bytes", 0);
            snap.compressed_bytes = entry.value("compressed_bytes", 0);

            snapshot_index_[snap.id] = std::move(snap);
        }
    }
    catch (const nlohmann::json::parse_error& parse_err)
    {
        return std::unexpected("Snapshot index parse error: " + std::string(parse_err.what()));
    }

    return {};
}

auto RepositoryService::save_index() const -> std::expected<void, std::string>
{
    auto index_path = repo_root_ / "index.json";

    nlohmann::json json_array = nlohmann::json::array();
    for (const auto& [snap_id, snap] : snapshot_index_)
    {
        nlohmann::json entry;
        entry["id"] = snap.id;
        entry["tag"] = snap.tag;
        entry["memo"] = snap.memo;
        entry["root_path"] = snap.root_path;
        entry["storage_path"] = snap.storage_path;
        entry["compression"] = static_cast<int>(snap.compression);
        entry["file_count"] = snap.file_count;
        entry["total_bytes"] = snap.total_bytes;
        entry["compressed_bytes"] = snap.compressed_bytes;
        json_array.push_back(std::move(entry));
    }

    std::ofstream ofs(index_path);
    if (!ofs.is_open())
    {
        return std::unexpected("Failed to write snapshot index");
    }

    ofs << json_array.dump(2);
    return {};
}

} // namespace markamp::core
