#include "WorkspacePersistence.h"

namespace markamp::node_editor
{

// ---------------------------------------------------------------------------
// Snapshot capture/restore
// ---------------------------------------------------------------------------

auto WorkspacePersistence::capture_snapshot(const std::string& graph_data,
                                            const ViewportState& viewport,
                                            const std::vector<NodeId>& selection)
    -> WorkspaceSnapshot
{
    WorkspaceSnapshot snapshot;
    snapshot.graph_data = graph_data;
    snapshot.viewport = viewport;
    snapshot.selected_nodes = selection;
    snapshot.version = next_version_++;
    snapshot.is_modified = modified_;
    snapshot.timestamp = std::chrono::steady_clock::now();
    return snapshot;
}

void WorkspacePersistence::store_snapshot(const WorkspaceSnapshot& snapshot)
{
    snapshots_.push_back(snapshot);
    trim_snapshots();
}

auto WorkspacePersistence::latest_snapshot() const -> const WorkspaceSnapshot*
{
    if (snapshots_.empty())
    {
        return nullptr;
    }
    return &snapshots_.back();
}

auto WorkspacePersistence::snapshots() const -> const std::vector<WorkspaceSnapshot>&
{
    return snapshots_;
}

void WorkspacePersistence::clear_snapshots()
{
    snapshots_.clear();
}

// ---------------------------------------------------------------------------
// Autosave logic
// ---------------------------------------------------------------------------

auto WorkspacePersistence::should_autosave(float elapsed_ms) const -> bool
{
    if (!config_.enabled || !modified_)
    {
        return false;
    }
    return elapsed_ms >= static_cast<float>(config_.interval_ms);
}

// ---------------------------------------------------------------------------
// Crash recovery
// ---------------------------------------------------------------------------

auto WorkspacePersistence::has_recovery_data() const -> bool
{
    return has_recovery_;
}

auto WorkspacePersistence::load_recovery() const -> const WorkspaceSnapshot*
{
    if (!has_recovery_)
    {
        return nullptr;
    }
    return &recovery_snapshot_;
}

void WorkspacePersistence::discard_recovery()
{
    has_recovery_ = false;
    recovery_snapshot_ = {};
}

void WorkspacePersistence::save_as_recovery()
{
    if (!snapshots_.empty())
    {
        recovery_snapshot_ = snapshots_.back();
        has_recovery_ = true;
    }
}

// ---------------------------------------------------------------------------
// Private
// ---------------------------------------------------------------------------

void WorkspacePersistence::trim_snapshots()
{
    while (snapshots_.size() > static_cast<std::size_t>(config_.max_snapshots))
    {
        snapshots_.erase(snapshots_.begin());
    }
}

} // namespace markamp::node_editor
