/// @file WorkspaceSessionRestore.h
/// @brief V9 Phase 46 — Workspace session persistence and restore.
#pragma once

#include <chrono>
#include <cstdint>
#include <string>
#include <vector>

namespace markamp::core
{

/// A snapshot of a workspace session.
struct SessionSnapshot
{
    std::string snapshot_id;
    std::string workspace_name;
    std::vector<std::string> open_files;
    std::string active_file;
    int editor_group_count{0};
    int window_x{0};
    int window_y{0};
    int window_width{1280};
    int window_height{720};
    std::chrono::system_clock::time_point created_at;
};

/// Policy for session restore behavior.
struct RestorePolicy
{
    bool auto_restore{true};
    int max_snapshots{10};
    bool restore_window_positions{true};
    bool restore_editor_groups{true};
};

/// Manages workspace session snapshots and restore.
class WorkspaceSessionRestore
{
public:
    WorkspaceSessionRestore() = default;

    // ── Snapshots ─────────────────────────────────────────────────────
    auto save_snapshot(const std::string& workspace_name) -> std::string;
    auto restore_snapshot(const std::string& snapshot_id) -> bool;
    [[nodiscard]] auto latest_snapshot(const std::string& workspace_name) const
        -> const SessionSnapshot*;
    [[nodiscard]] auto list_snapshots() const -> std::vector<const SessionSnapshot*>;
    auto delete_snapshot(const std::string& snapshot_id) -> bool;

    // ── Policy ────────────────────────────────────────────────────────
    void set_policy(RestorePolicy policy);
    [[nodiscard]] auto get_policy() const -> RestorePolicy;

    // ── Snapshot data ─────────────────────────────────────────────────
    auto add_file_to_snapshot(const std::string& snapshot_id, const std::string& file_path) -> bool;

    // ── Statistics ────────────────────────────────────────────────────
    [[nodiscard]] auto snapshot_count() const -> int;
    void clear_snapshots();

private:
    std::vector<SessionSnapshot> snapshots_;
    RestorePolicy policy_;
    int next_id_{1};

    auto find_snapshot_mut(const std::string& snapshot_id) -> SessionSnapshot*;
};

} // namespace markamp::core
