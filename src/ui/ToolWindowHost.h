/// @file ToolWindowHost.h
/// @brief V8 Phase 11 (Phase 23) — Dockable, pinnable tool window containers.
/// Manages tool window registration, pin/auto-hide state, dock position,
/// and layout persistence per workspace.

#pragma once

#include "core/EventBus.h"
#include "core/Events.h"

#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{
class Config;
} // namespace markamp::core

namespace markamp::ui
{

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
};

/// Manages dockable, pinnable tool window containers modeled after JetBrains-style
/// tool windows. Supports left/right/bottom dock positions, pin/auto-hide toggle,
/// and workspace-aware layout persistence.
class ToolWindowHost
{
public:
    ToolWindowHost(core::EventBus& event_bus, core::Config& config);

    /// Register a tool window panel with an ID and title.
    void register_panel(
        const std::string& panel_id,
        const std::string& title,
        core::events::DockPosition default_position = core::events::DockPosition::kBottom);

    /// Unregister a tool window panel.
    void unregister_panel(const std::string& panel_id);

    /// Toggle a panel's visibility.
    void toggle_panel(const std::string& panel_id);

    /// Set a panel's visibility explicitly.
    void set_visible(const std::string& panel_id, bool visible);

    /// Set a panel's dock position.
    void set_dock_position(const std::string& panel_id, core::events::DockPosition position);

    /// Set a panel's pinned state.
    void set_pinned(const std::string& panel_id, bool pinned);

    /// Check if a panel is currently visible.
    [[nodiscard]] auto is_visible(const std::string& panel_id) const -> bool;

    /// Check if a panel is pinned.
    [[nodiscard]] auto is_pinned(const std::string& panel_id) const -> bool;

    /// Get a panel's current dock position.
    [[nodiscard]] auto dock_position(const std::string& panel_id) const
        -> core::events::DockPosition;

    /// Get a panel's state.
    [[nodiscard]] auto panel_state(const std::string& panel_id) const -> const ToolWindowState*;

    /// Get all registered panel IDs.
    [[nodiscard]] auto panel_ids() const -> std::vector<std::string>;

    /// Get all panels docked at a specific position.
    [[nodiscard]] auto panels_at(core::events::DockPosition position) const
        -> std::vector<std::string>;

    /// Number of registered panels.
    [[nodiscard]] auto panel_count() const -> int;

    /// Serialize layout to JSON for workspace persistence.
    [[nodiscard]] auto save_layout() const -> std::string;

    /// Restore layout from JSON string.
    auto restore_layout(const std::string& json) -> bool;

private:
    core::EventBus& event_bus_;
    core::Config& config_;
    std::unordered_map<std::string, ToolWindowState> panels_;

    core::Subscription toggle_sub_;
    core::Subscription dock_changed_sub_;
};

} // namespace markamp::ui
