/// @file ToolWindowHost.h
/// @brief V8 Phase 18 — Dockable, pinnable tool window containers.
/// Manages tool window registration, pin/auto-hide state, dock position,
/// dock constraints, size constraints, ordering, and layout persistence.

#pragma once

#include "core/EventBus.h"
#include "core/Events.h"

#include <cmath>
#include <functional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace markamp::core
{
class Config;
} // namespace markamp::core

namespace markamp::ui
{

/// Size constraint pair for a tool window panel.
struct PanelSizeConstraint
{
    int min_width{100};
    int min_height{80};
    int max_width{2000};
    int max_height{1500};
};

/// State of a single tool window panel.
struct ToolWindowState
{
    std::string panel_id;
    std::string title;
    core::events::DockPosition dock_position{core::events::DockPosition::kBottom};
    bool pinned{false};
    bool visible{false};
    int width{300};
    int height{200};

    // Phase 18 Task 2: Auto-hide
    bool auto_hide{false};
    int auto_hide_timeout_ms{2000};

    // Phase 18 Task 3: Size constraints
    PanelSizeConstraint size_constraint;

    // Phase 18 Task 4: Ordering
    int order{0};

    // Phase 18 Task 1: Dock constraints
    std::unordered_set<core::events::DockPosition> allowed_positions{
        core::events::DockPosition::kBottom,
        core::events::DockPosition::kLeft,
        core::events::DockPosition::kRight};
};

/// Manages dockable, pinnable tool window containers modeled after JetBrains-style
/// tool windows. Supports left/right/bottom dock positions, pin/auto-hide toggle,
/// dock constraints, size constraints, ordering, and workspace-aware layout persistence.
class ToolWindowHost
{
public:
    ToolWindowHost(core::EventBus& event_bus, core::Config& config);

    // ── Registration ────────────────────────────────────────────────────
    /// Register a tool window panel with an ID and title.
    void register_panel(
        const std::string& panel_id,
        const std::string& title,
        core::events::DockPosition default_position = core::events::DockPosition::kBottom);

    /// Unregister a tool window panel.
    void unregister_panel(const std::string& panel_id);

    // ── Visibility ──────────────────────────────────────────────────────
    /// Toggle a panel's visibility.
    void toggle_panel(const std::string& panel_id);

    /// Set a panel's visibility explicitly.
    void set_visible(const std::string& panel_id, bool visible);

    /// Check if a panel is currently visible.
    [[nodiscard]] auto is_visible(const std::string& panel_id) const -> bool;

    // ── Dock Position ───────────────────────────────────────────────────
    /// Set a panel's dock position (enforces dock constraints).
    void set_dock_position(const std::string& panel_id, core::events::DockPosition position);

    /// Get a panel's current dock position.
    [[nodiscard]] auto dock_position(const std::string& panel_id) const
        -> core::events::DockPosition;

    // ── Pinning ─────────────────────────────────────────────────────────
    /// Set a panel's pinned state.
    void set_pinned(const std::string& panel_id, bool pinned);

    /// Check if a panel is pinned.
    [[nodiscard]] auto is_pinned(const std::string& panel_id) const -> bool;

    // ── Task 1: Dock Constraints ────────────────────────────────────────
    /// Set the allowed dock positions for a panel.
    void set_allowed_positions(const std::string& panel_id,
                               const std::unordered_set<core::events::DockPosition>& positions);

    /// Get the allowed dock positions for a panel.
    [[nodiscard]] auto allowed_positions(const std::string& panel_id) const
        -> std::unordered_set<core::events::DockPosition>;

    /// Check if a dock position is allowed for a panel.
    [[nodiscard]] auto is_position_allowed(const std::string& panel_id,
                                           core::events::DockPosition position) const -> bool;

    // ── Task 2: Auto-Hide ───────────────────────────────────────────────
    /// Set auto-hide mode for a panel.
    void set_auto_hide(const std::string& panel_id, bool enabled);

    /// Check if a panel is in auto-hide mode.
    [[nodiscard]] auto is_auto_hide(const std::string& panel_id) const -> bool;

    /// Set auto-hide timeout in milliseconds.
    void set_auto_hide_timeout(const std::string& panel_id, int timeout_ms);

    /// Get auto-hide timeout in milliseconds.
    [[nodiscard]] auto auto_hide_timeout(const std::string& panel_id) const -> int;

    // ── Task 3: Size Constraints ────────────────────────────────────────
    /// Set minimum size for a panel.
    void set_min_size(const std::string& panel_id, int min_width, int min_height);

    /// Set maximum size for a panel.
    void set_max_size(const std::string& panel_id, int max_width, int max_height);

    /// Get size constraint for a panel.
    [[nodiscard]] auto size_constraint(const std::string& panel_id) const -> PanelSizeConstraint;

    // ── Task 4: Ordering ────────────────────────────────────────────────
    /// Set a panel's display order within its dock position.
    void set_order(const std::string& panel_id, int order_value);

    /// Get a panel's display order.
    [[nodiscard]] auto order(const std::string& panel_id) const -> int;

    /// Move a panel before another panel in the same dock position.
    void move_panel_before(const std::string& panel_id, const std::string& before_panel_id);

    /// Move a panel after another panel in the same dock position.
    void move_panel_after(const std::string& panel_id, const std::string& after_panel_id);

    // ── Queries ─────────────────────────────────────────────────────────
    /// Get a panel's state.
    [[nodiscard]] auto panel_state(const std::string& panel_id) const -> const ToolWindowState*;

    /// Get all registered panel IDs.
    [[nodiscard]] auto panel_ids() const -> std::vector<std::string>;

    /// Get all panels docked at a specific position, sorted by order.
    [[nodiscard]] auto panels_at(core::events::DockPosition position) const
        -> std::vector<std::string>;

    /// Number of registered panels.
    [[nodiscard]] auto panel_count() const -> int;

    // ── Task 5: State Serialization ─────────────────────────────────────
    /// Serialize layout to JSON for workspace persistence.
    [[nodiscard]] auto save_layout() const -> std::string;

    /// Restore layout from JSON string.
    auto restore_layout(const std::string& json_str) -> bool;

    // ── Task 6: Panel Commands ──────────────────────────────────────────
    /// Register a named command for a panel.
    void register_command(const std::string& panel_id, const std::string& command_id);

    /// Unregister a command.
    void unregister_command(const std::string& panel_id, const std::string& command_id);

    /// Get registered commands for a panel.
    [[nodiscard]] auto commands(const std::string& panel_id) const -> std::vector<std::string>;

    /// Execute a panel command (dispatches PanelCommandEvent).
    void execute_command(const std::string& panel_id, const std::string& command_id);

    // ── Task 7: Keyboard Navigation ─────────────────────────────────────
    /// Focus a specific panel by ID.
    void focus_panel(const std::string& panel_id);

    /// Focus the next panel in the current dock position.
    void focus_next_panel();

    /// Focus the previous panel in the current dock position.
    void focus_previous_panel();

    /// Get the currently focused panel ID (empty if none).
    [[nodiscard]] auto focused_panel() const -> const std::string&;

    // ── Task 8: Badges ──────────────────────────────────────────────────
    /// Set a badge on a panel.
    void set_badge(const std::string& panel_id, const core::events::PanelBadge& badge);

    /// Get the badge for a panel.
    [[nodiscard]] auto badge(const std::string& panel_id) const -> core::events::PanelBadge;

    /// Clear a panel's badge.
    void clear_badge(const std::string& panel_id);

    // ── Task 9: Context Menus ───────────────────────────────────────────
    /// Set context menu items for a panel.
    void set_context_menu(const std::string& panel_id,
                          const std::vector<core::events::PanelMenuItem>& items);

    /// Get context menu items for a panel.
    [[nodiscard]] auto context_menu(const std::string& panel_id) const
        -> std::vector<core::events::PanelMenuItem>;

    // ── Task 10: Panel Grouping ─────────────────────────────────────────
    /// Set the group for a panel (panels in same group share a tab strip).
    void set_group(const std::string& panel_id, const std::string& group_name);

    /// Get the group for a panel.
    [[nodiscard]] auto group(const std::string& panel_id) const -> std::string;

    /// Get all panels in a specific group.
    [[nodiscard]] auto panels_in_group(const std::string& group_name) const
        -> std::vector<std::string>;

    // ── Task 11: Panel History ──────────────────────────────────────────
    /// Get the panel visibility history (most recent last).
    [[nodiscard]] auto panel_history() const -> const std::vector<std::string>&;

    /// Show the last panel that was hidden.
    void show_last_panel();

    // ── Task 12: Panel Zoom ─────────────────────────────────────────────
    /// Set zoom level for a panel (1.0 = 100%).
    void set_zoom_level(const std::string& panel_id, double zoom);

    /// Get zoom level for a panel.
    [[nodiscard]] auto zoom_level(const std::string& panel_id) const -> double;

    /// Zoom in by 10%.
    void zoom_in(const std::string& panel_id);

    /// Zoom out by 10%.
    void zoom_out(const std::string& panel_id);

    /// Reset zoom to 100%.
    void reset_zoom(const std::string& panel_id);

    // ── Task 13: Panel Search ───────────────────────────────────────────
    /// Set whether a panel supports search.
    void set_searchable(const std::string& panel_id, bool searchable);

    /// Check if a panel supports search.
    [[nodiscard]] auto is_searchable(const std::string& panel_id) const -> bool;

    /// Set search text for a panel (publishes PanelSearchChangedEvent).
    void set_search_text(const std::string& panel_id, const std::string& text);

    /// Get current search text for a panel.
    [[nodiscard]] auto search_text(const std::string& panel_id) const -> std::string;

    /// Clear search text for a panel.
    void clear_search(const std::string& panel_id);

    // ── Task 14: Panel Theme Integration ────────────────────────────────
    using ThemeCallback = std::function<void(const std::string& panel_id)>;

    /// Set a theme update callback for a panel.
    void set_theme_callback(const std::string& panel_id, ThemeCallback callback);

    /// Apply theme to all panels with registered callbacks.
    void apply_theme_to_all();

    // ── Task 15: Panel Extension Support ────────────────────────────────
    /// Register a panel contributed by an extension.
    void register_extension_panel(const std::string& extension_id,
                                  const std::string& panel_id,
                                  const std::string& title);

    /// Unregister all panels contributed by an extension.
    void unregister_extension_panels(const std::string& extension_id);

    /// Get all panels contributed by a specific extension.
    [[nodiscard]] auto extension_panels(const std::string& extension_id) const
        -> std::vector<std::string>;

private:
    core::EventBus& event_bus_;
    core::Config& config_;
    std::unordered_map<std::string, ToolWindowState> panels_;

    core::Subscription toggle_sub_;
    core::Subscription dock_changed_sub_;

    /// Next auto-incrementing order value for new panels.
    int next_order_{0};

    // Task 6: Per-panel command registrations
    std::unordered_map<std::string, std::vector<std::string>> panel_commands_;

    // Task 7: Currently focused panel
    std::string focused_panel_id_;

    // Task 8: Per-panel badges
    std::unordered_map<std::string, core::events::PanelBadge> panel_badges_;

    // Task 9: Per-panel context menus
    std::unordered_map<std::string, std::vector<core::events::PanelMenuItem>> panel_menus_;

    // Task 10: Per-panel group assignments
    std::unordered_map<std::string, std::string> panel_groups_;

    // Task 11: Panel visibility history
    std::vector<std::string> panel_history_;

    // Task 12: Per-panel zoom levels
    std::unordered_map<std::string, double> panel_zoom_;

    // Task 13: Per-panel searchability and search text
    std::unordered_set<std::string> searchable_panels_;
    std::unordered_map<std::string, std::string> panel_search_text_;

    // Task 14: Per-panel theme callbacks
    std::unordered_map<std::string, ThemeCallback> theme_callbacks_;

    // Task 15: Extension panel ownership
    std::unordered_map<std::string, std::vector<std::string>> extension_panel_map_;
};

} // namespace markamp::ui
