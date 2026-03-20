/// @file ShellLayoutState.h
/// @brief P03-T04: Centralized shell layout visibility state model.
///
/// Provides a single source of truth for sidebar, bottom panel, and status bar
/// visibility. All toggle operations go through this state model so that
/// persistence and zen mode save/restore work correctly.
#pragma once

#include <string>

namespace markamp::core
{
class EventBus;
class Config;

/// Central shell layout visibility state.
///
/// All panel toggle commands should mutate this state, which then
/// publishes the corresponding visibility events and persists changes.
class ShellLayoutState
{
public:
    ShellLayoutState() = default;
    explicit ShellLayoutState(EventBus& bus, Config& cfg);

    // ── Queries ──

    [[nodiscard]] auto is_sidebar_visible() const -> bool { return sidebar_visible_; }
    [[nodiscard]] auto is_secondary_sidebar_visible() const -> bool
    {
        return secondary_sidebar_visible_;
    }
    [[nodiscard]] auto is_bottom_panel_visible() const -> bool { return bottom_panel_visible_; }
    [[nodiscard]] auto is_status_bar_visible() const -> bool { return status_bar_visible_; }
    [[nodiscard]] auto is_zen_mode() const -> bool { return zen_mode_; }
    [[nodiscard]] auto sidebar_mode() const -> int { return sidebar_mode_; }
    [[nodiscard]] auto bottom_panel_height() const -> int { return bottom_panel_height_; }

    // ── Mutations ──

    void toggle_sidebar();
    void toggle_secondary_sidebar();
    void toggle_bottom_panel();
    void toggle_status_bar();
    void set_sidebar_mode(int mode);
    void set_bottom_panel_height(int height);

    /// Enter zen mode: save current state and hide all chrome.
    void enter_zen_mode();
    /// Exit zen mode: restore state from before zen entry.
    void exit_zen_mode();

    // ── Persistence ──

    /// Save current state to Config.
    void save_to_config();
    /// Load state from Config.
    void load_from_config();

private:
    EventBus* event_bus_{nullptr};
    Config* config_{nullptr};

    // Current state
    bool sidebar_visible_{true};
    bool secondary_sidebar_visible_{false};
    bool bottom_panel_visible_{false};
    bool status_bar_visible_{true};
    bool zen_mode_{false};
    int sidebar_mode_{0};
    int bottom_panel_height_{200};

    // Pre-zen saved state
    bool pre_zen_sidebar_{true};
    bool pre_zen_secondary_sidebar_{false};
    bool pre_zen_bottom_panel_{false};
    bool pre_zen_status_bar_{true};
};

} // namespace markamp::core
