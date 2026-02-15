// ============================================================================
// File: src/core/RepositoryService.h
// Phase 34: Repository System — Workspace snapshot service
// ============================================================================
#pragma once

#include "SnapshotTypes.h"

#include <expected>
#include <filesystem>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

class EventBus;
class Config;

/// Git-like workspace snapshot service.
/// Creates, tags, lists, checks out, diffs, and purges
/// compressed snapshots of the entire workspace.
class RepositoryService
{
public:
    RepositoryService(EventBus& event_bus, Config& config, std::filesystem::path workspace_root);
    ~RepositoryService() = default;

    RepositoryService(const RepositoryService&) = delete;
    auto operator=(const RepositoryService&) -> RepositoryService& = delete;

    /// Initialize: create repo directory, load snapshot index.
    [[nodiscard]] auto initialize() -> std::expected<void, std::string>;

    // ── Snapshot Creation ──

    /// Create a new workspace snapshot.
    [[nodiscard]] auto create_snapshot(const std::string& memo = "")
        -> std::expected<std::string, std::string>;

    /// Tag an existing snapshot with a human-friendly name.
    [[nodiscard]] auto tag_snapshot(const std::string& snapshot_id, const std::string& tag)
        -> std::expected<void, std::string>;

    // ── Snapshot Retrieval ──

    /// List all snapshots (newest first).
    [[nodiscard]] auto list_snapshots() const -> std::vector<Snapshot>;

    /// Get a specific snapshot by ID.
    [[nodiscard]] auto get_snapshot(const std::string& snapshot_id) const
        -> std::expected<Snapshot, std::string>;

    /// Get a snapshot by tag name.
    [[nodiscard]] auto get_snapshot_by_tag(const std::string& tag) const
        -> std::expected<Snapshot, std::string>;

    // ── Checkout ──

    /// Check out (restore) a snapshot to the workspace.
    /// Creates an automatic snapshot of current state first.
    [[nodiscard]] auto checkout_snapshot(const std::string& snapshot_id)
        -> std::expected<void, std::string>;

    // ── Diff ──

    /// Diff two snapshots.
    [[nodiscard]] auto diff_snapshots(const std::string& old_snapshot_id,
                                      const std::string& new_snapshot_id) const
        -> std::expected<SnapshotDiff, std::string>;

    // ── Cleanup ──

    /// Purge old snapshots based on retention policy.
    [[nodiscard]] auto purge_old_snapshots() -> std::expected<PurgeStats, std::string>;

    /// Delete a specific snapshot.
    [[nodiscard]] auto delete_snapshot(const std::string& snapshot_id)
        -> std::expected<void, std::string>;

    // ── Configuration ──

    [[nodiscard]] auto repo_config() const -> RepositoryConfig;

private:
    EventBus& event_bus_;
    Config& config_;
    std::filesystem::path workspace_root_;
    std::filesystem::path repo_root_; // workspace_root / ".markamp-repo"

    mutable std::mutex mutex_;
    std::unordered_map<std::string, Snapshot> snapshot_index_;

    /// Collect all files in the workspace (respecting excludes).
    [[nodiscard]] auto collect_workspace_files() const
        -> std::expected<std::vector<std::filesystem::path>, std::string>;

    /// Check if a path matches any exclude pattern.
    [[nodiscard]] auto is_excluded(const std::filesystem::path& path) const -> bool;

    /// Create a compressed archive of the workspace.
    [[nodiscard]] auto create_archive(const std::string& snapshot_id,
                                      const std::vector<std::filesystem::path>& files) const
        -> std::expected<std::filesystem::path, std::string>;

    /// Extract a snapshot archive to a target directory.
    [[nodiscard]] auto extract_archive(const std::string& snapshot_id,
                                       const std::filesystem::path& target) const
        -> std::expected<void, std::string>;

    /// Load the snapshot index from disk.
    [[nodiscard]] auto load_index() -> std::expected<void, std::string>;

    /// Save the snapshot index to disk.
    [[nodiscard]] auto save_index() const -> std::expected<void, std::string>;
};

} // namespace markamp::core
