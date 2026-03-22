/// @file SidebarStateContract.h
/// @brief V21 Phase 05 — Unified sidebar-state contract for save/restore.
///
/// Defines a structured contract for persisting and restoring sidebar
/// panel state, including:
///   - Per-panel collapse/expand state
///   - Active panel selection
///   - Section ordering
///   - State snapshots for zen-mode or workspace switching
#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

// ============================================================================
// SidebarPanelState — per-panel persisted state
// ============================================================================

/// Persisted state for a single sidebar panel.
struct SidebarPanelState
{
    std::string panel_id;
    bool is_expanded{true};    ///< Whether the panel is expanded
    bool is_visible{true};     ///< Whether the panel is visible
    int height{-1};            ///< Custom height (-1 = default)
    int scroll_position{0};    ///< Saved scroll position
};

// ============================================================================
// SidebarSnapshot — complete sidebar state at a point in time
// ============================================================================

/// Complete sidebar state for save/restore operations.
struct SidebarSnapshot
{
    std::string active_panel_id;    ///< Currently active panel
    std::string sidebar_area;       ///< "primary" or "secondary"
    bool is_sidebar_visible{true};  ///< Overall sidebar visibility
    int sidebar_width{250};         ///< Sidebar width in pixels
    std::vector<SidebarPanelState> panel_states;

    /// Find a panel state by ID.
    [[nodiscard]] auto find_panel(const std::string& panel_id) const
        -> const SidebarPanelState*
    {
        for (const auto& state : panel_states)
        {
            if (state.panel_id == panel_id) return &state;
        }
        return nullptr;
    }
};

// ============================================================================
// SidebarStateContract — unified save/restore
// ============================================================================

/// Manages sidebar state persistence with snapshot support.
class SidebarStateContract
{
public:
    SidebarStateContract() = default;

    // ── Current State Tracking ──

    /// Set the active panel for an area.
    void set_active_panel(const std::string& area, const std::string& panel_id);

    /// Get the active panel for an area.
    [[nodiscard]] auto active_panel(const std::string& area) const -> std::string;

    /// Set panel expand/collapse state.
    void set_panel_expanded(const std::string& panel_id, bool expanded);

    /// Get panel expand/collapse state.
    [[nodiscard]] auto is_panel_expanded(const std::string& panel_id) const -> bool;

    /// Set panel visibility.
    void set_panel_visible(const std::string& panel_id, bool visible);

    /// Get panel visibility.
    [[nodiscard]] auto is_panel_visible(const std::string& panel_id) const -> bool;

    /// Set sidebar width.
    void set_sidebar_width(const std::string& area, int width);

    /// Get sidebar width.
    [[nodiscard]] auto sidebar_width(const std::string& area) const -> int;

    // ── Snapshot Operations ──

    /// Take a snapshot of the current state for an area.
    [[nodiscard]] auto take_snapshot(const std::string& area) const -> SidebarSnapshot;

    /// Restore from a snapshot.
    void restore_snapshot(const SidebarSnapshot& snapshot);

    /// Store a named snapshot (e.g., "zen_pre", "workspace_switch").
    void save_named_snapshot(const std::string& name, const SidebarSnapshot& snapshot);

    /// Retrieve a named snapshot.
    [[nodiscard]] auto get_named_snapshot(const std::string& name) const
        -> const SidebarSnapshot*;

    /// Check if a named snapshot exists.
    [[nodiscard]] auto has_snapshot(const std::string& name) const -> bool;

    /// Get all snapshot names.
    [[nodiscard]] auto snapshot_names() const -> std::vector<std::string>;

    // ── Registration ──

    /// Register a panel ID for tracking (so we can include it in snapshots).
    void register_panel(const std::string& panel_id, const std::string& area);

    /// Get all registered panel IDs for an area.
    [[nodiscard]] auto panels_for_area(const std::string& area) const
        -> std::vector<std::string>;

    /// Total number of tracked panels.
    [[nodiscard]] auto tracked_panel_count() const -> std::size_t;

private:
    /// area → active panel ID
    std::unordered_map<std::string, std::string> active_panels_;

    /// panel_id → current state
    std::unordered_map<std::string, SidebarPanelState> panel_states_;

    /// area → sidebar width
    std::unordered_map<std::string, int> sidebar_widths_;

    /// area → list of panel IDs
    std::unordered_map<std::string, std::vector<std::string>> area_panels_;

    /// Named snapshots
    std::unordered_map<std::string, SidebarSnapshot> named_snapshots_;
};

} // namespace markamp::core
