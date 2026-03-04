#pragma once

/**
 * @file PanelLifecycleManager.h
 * @brief Phase 29 Task 4: Panel lifecycle management.
 *
 * Snapshot, restore, and reset panel layouts. Enables users to recover
 * from cluttered panel layouts quickly.
 */

#include <string>
#include <vector>

namespace markamp::core
{
namespace events
{
enum class DockPosition : uint8_t;
} // namespace events
} // namespace markamp::core

namespace markamp::ui
{

/// Snapshot of a single panel's layout state.
struct PanelLayoutEntry
{
    std::string panel_id;
    std::string title;
    core::events::DockPosition dock_position{};
    bool visible{false};
    bool pinned{false};
    int width{300};
    int height{200};
    int order{0};
};

/// Complete panel layout snapshot (for save/restore).
struct PanelSnapshot
{
    std::string snapshot_name;
    std::vector<PanelLayoutEntry> entries;
    int timestamp_unix{0};

    /// Get the number of visible panels in this snapshot.
    [[nodiscard]] auto visible_count() const -> int;

    /// Get the number of panels at a specific dock position.
    [[nodiscard]] auto count_at(core::events::DockPosition position) const -> int;

    /// Check if snapshot has any entries.
    [[nodiscard]] auto is_empty() const -> bool;
};

/**
 * @brief Manages panel layout lifecycle: snapshot, restore, and reset.
 *
 * Usage:
 * @code
 *   PanelLifecycleManager mgr;
 *   mgr.save_snapshot("my_layout", entries);
 *   // ... user changes layout ...
 *   auto snapshot = mgr.get_snapshot("my_layout");
 *   // Apply snapshot to ToolWindowHost
 *   mgr.reset_to_defaults();
 * @endcode
 */
class PanelLifecycleManager
{
public:
    PanelLifecycleManager() = default;

    /// Save a named snapshot of the current layout.
    void save_snapshot(const std::string& name, const std::vector<PanelLayoutEntry>& entries);

    /// Get a saved snapshot by name.
    [[nodiscard]] auto get_snapshot(const std::string& name) const -> const PanelSnapshot*;

    /// Check if a named snapshot exists.
    [[nodiscard]] auto has_snapshot(const std::string& name) const -> bool;

    /// Delete a saved snapshot.
    void delete_snapshot(const std::string& name);

    /// Get all snapshot names (sorted alphabetically).
    [[nodiscard]] auto snapshot_names() const -> std::vector<std::string>;

    /// Get the number of saved snapshots.
    [[nodiscard]] auto snapshot_count() const -> int;

    /// Set the default layout (used for reset_to_defaults).
    void set_default_layout(const std::vector<PanelLayoutEntry>& entries);

    /// Get the default layout.
    [[nodiscard]] auto default_layout() const -> const PanelSnapshot*;

    /// Reset all snapshots and default layout.
    void clear();

    /// Get the default panel layout entries (built-in baseline).
    [[nodiscard]] static auto builtin_defaults() -> std::vector<PanelLayoutEntry>;

private:
    std::vector<PanelSnapshot> snapshots_;
    PanelSnapshot default_layout_;
    bool has_default_{false};
};

} // namespace markamp::ui
