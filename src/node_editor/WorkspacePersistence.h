#pragma once

#include "NodeEditorTypes.h"

#include <chrono>
#include <cstdint>
#include <string>
#include <vector>

namespace markamp::node_editor
{

// ---------------------------------------------------------------------------
// WorkspaceSnapshot — serializable snapshot of editor state
// ---------------------------------------------------------------------------

struct ViewportState
{
    Vec2 pan{0.0F, 0.0F};
    float zoom{1.0F};
};

struct WorkspaceSnapshot
{
    std::string graph_data; ///< Serialized graph JSON
    ViewportState viewport;
    std::vector<NodeId> selected_nodes;
    std::string active_panel;
    uint64_t version{0};
    bool is_modified{false};
    std::chrono::steady_clock::time_point timestamp;

    [[nodiscard]] auto is_valid() const -> bool
    {
        return !graph_data.empty();
    }

    [[nodiscard]] auto age_ms() const -> float
    {
        auto current_time = std::chrono::steady_clock::now();
        return static_cast<float>(
            std::chrono::duration_cast<std::chrono::milliseconds>(current_time - timestamp)
                .count());
    }
};

// ---------------------------------------------------------------------------
// AutosaveConfig
// ---------------------------------------------------------------------------

struct AutosaveConfig
{
    bool enabled{true};
    int interval_ms{30000}; ///< 30 seconds default
    int max_snapshots{5};   ///< Keep last N snapshots
};

// ---------------------------------------------------------------------------
// WorkspacePersistence — autosave, snapshot, and crash recovery
// ---------------------------------------------------------------------------

class WorkspacePersistence
{
public:
    WorkspacePersistence() = default;

    // --- Snapshot capture/restore -----------------------------------------

    /// Capture a snapshot of current workspace state.
    auto capture_snapshot(const std::string& graph_data,
                          const ViewportState& viewport,
                          const std::vector<NodeId>& selection) -> WorkspaceSnapshot;

    /// Store snapshot for later recovery.
    void store_snapshot(const WorkspaceSnapshot& snapshot);

    /// Get the latest stored snapshot.
    [[nodiscard]] auto latest_snapshot() const -> const WorkspaceSnapshot*;

    /// Get all stored snapshots.
    [[nodiscard]] auto snapshots() const -> const std::vector<WorkspaceSnapshot>&;

    /// Clear all stored snapshots.
    void clear_snapshots();

    // --- Autosave logic ---------------------------------------------------

    /// Check if autosave should trigger based on elapsed time.
    [[nodiscard]] auto should_autosave(float elapsed_ms) const -> bool;

    /// Get autosave configuration.
    [[nodiscard]] auto config() const -> const AutosaveConfig&
    {
        return config_;
    }
    auto config_mut() -> AutosaveConfig&
    {
        return config_;
    }

    void set_autosave_enabled(bool enabled)
    {
        config_.enabled = enabled;
    }
    void set_autosave_interval(int interval)
    {
        config_.interval_ms = interval;
    }

    // --- Modified tracking ------------------------------------------------

    void mark_modified()
    {
        modified_ = true;
    }
    void clear_modified()
    {
        modified_ = false;
    }
    [[nodiscard]] auto is_modified() const -> bool
    {
        return modified_;
    }

    // --- Crash recovery ---------------------------------------------------

    /// Check if recovery data exists.
    [[nodiscard]] auto has_recovery_data() const -> bool;

    /// Get recovery snapshot.
    [[nodiscard]] auto load_recovery() const -> const WorkspaceSnapshot*;

    /// Discard recovery data.
    void discard_recovery();

    /// Mark current snapshot as recovery data.
    void save_as_recovery();

    [[nodiscard]] auto snapshot_count() const -> std::size_t
    {
        return snapshots_.size();
    }

private:
    std::vector<WorkspaceSnapshot> snapshots_;
    AutosaveConfig config_;
    bool modified_{false};
    bool has_recovery_{false};
    WorkspaceSnapshot recovery_snapshot_;
    uint64_t next_version_{1};

    void trim_snapshots();
};

} // namespace markamp::node_editor
