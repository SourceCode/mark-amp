/// @file WorkspaceSessionRestore.cpp
/// @brief V9 Phase 46 — WorkspaceSessionRestore implementation.

#include "WorkspaceSessionRestore.h"

#include <algorithm>

namespace markamp::core
{

auto WorkspaceSessionRestore::save_snapshot(const std::string& workspace_name) -> std::string
{
    SessionSnapshot snap;
    snap.snapshot_id = "snap_" + std::to_string(next_id_++);
    snap.workspace_name = workspace_name;
    snap.created_at = std::chrono::system_clock::now();
    snapshots_.push_back(std::move(snap));

    // Enforce max_snapshots policy
    while (static_cast<int>(snapshots_.size()) > policy_.max_snapshots && policy_.max_snapshots > 0)
    {
        snapshots_.erase(snapshots_.begin());
    }

    return snapshots_.back().snapshot_id;
}

auto WorkspaceSessionRestore::restore_snapshot(const std::string& snapshot_id) -> bool
{
    const auto* snap = latest_snapshot("");
    // Search by ID
    for (const auto& snapshot : snapshots_)
    {
        if (snapshot.snapshot_id == snapshot_id)
        {
            return true; // In a real impl, would apply the snapshot state
        }
    }
    static_cast<void>(snap); // Suppress unused warning
    return false;
}

auto WorkspaceSessionRestore::latest_snapshot(const std::string& workspace_name) const
    -> const SessionSnapshot*
{
    auto it =
        std::find_if(snapshots_.rbegin(),
                     snapshots_.rend(),
                     [&](const SessionSnapshot& snap)
                     { return workspace_name.empty() || snap.workspace_name == workspace_name; });
    if (it != snapshots_.rend())
    {
        return &(*it);
    }
    return nullptr;
}

auto WorkspaceSessionRestore::list_snapshots() const -> std::vector<const SessionSnapshot*>
{
    std::vector<const SessionSnapshot*> result;
    result.reserve(snapshots_.size());
    for (const auto& snap : snapshots_)
    {
        result.push_back(&snap);
    }
    return result;
}

auto WorkspaceSessionRestore::delete_snapshot(const std::string& snapshot_id) -> bool
{
    auto iter = std::remove_if(snapshots_.begin(),
                               snapshots_.end(),
                               [&](const SessionSnapshot& snap)
                               { return snap.snapshot_id == snapshot_id; });
    if (iter == snapshots_.end())
    {
        return false;
    }
    snapshots_.erase(iter, snapshots_.end());
    return true;
}

void WorkspaceSessionRestore::set_policy(RestorePolicy policy)
{
    policy_ = policy;
}

auto WorkspaceSessionRestore::get_policy() const -> RestorePolicy
{
    return policy_;
}

auto WorkspaceSessionRestore::add_file_to_snapshot(const std::string& snapshot_id,
                                                   const std::string& file_path) -> bool
{
    auto* snap = find_snapshot_mut(snapshot_id);
    if (snap == nullptr)
    {
        return false;
    }
    snap->open_files.push_back(file_path);
    return true;
}

auto WorkspaceSessionRestore::snapshot_count() const -> int
{
    return static_cast<int>(snapshots_.size());
}

void WorkspaceSessionRestore::clear_snapshots()
{
    snapshots_.clear();
}

auto WorkspaceSessionRestore::find_snapshot_mut(const std::string& snapshot_id) -> SessionSnapshot*
{
    for (auto& snap : snapshots_)
    {
        if (snap.snapshot_id == snapshot_id)
        {
            return &snap;
        }
    }
    return nullptr;
}

// ── Batch 19-22 improvements (#127-129) ──

auto WorkspaceSessionRestore::snapshots_for_workspace(const std::string& workspace_name) const
    -> std::vector<const SessionSnapshot*>
{
    std::vector<const SessionSnapshot*> result;
    for (const auto& snap : snapshots_)
    {
        if (snap.workspace_name == workspace_name)
        {
            result.push_back(&snap);
        }
    }
    return result;
}

auto WorkspaceSessionRestore::total_file_count() const -> std::size_t
{
    std::size_t total = 0;
    for (const auto& snap : snapshots_)
    {
        total += snap.open_files.size();
    }
    return total;
}

auto WorkspaceSessionRestore::has_snapshot(const std::string& snapshot_id) const -> bool
{
    for (const auto& snap : snapshots_)
    {
        if (snap.snapshot_id == snapshot_id)
        {
            return true;
        }
    }
    return false;
}

} // namespace markamp::core
