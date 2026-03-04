#pragma once

/// @file DebugToolbar.h
/// @brief V13 Phase 30 — Floating debug toolbar with Continue/Step/Restart/Stop.

#include "FloatingToolbar.h"

namespace markamp::ui
{

/// Floating toolbar that appears during debug sessions at the top-center
/// of the editor area, providing debug control buttons.
class DebugToolbar : public FloatingToolbar
{
public:
    DebugToolbar(wxWindow* parent, core::ThemeEngine& theme_engine, core::EventBus& event_bus);

    /// Call when a debug session starts to show the toolbar.
    void OnDebugStarted();

    /// Call when the debugger pauses (breakpoint hit, step complete).
    void OnDebugPaused();

    /// Call when the debugger continues after pause.
    void OnDebugContinued();

    /// Call when the debug session ends to hide the toolbar.
    void OnDebugStopped();

    [[nodiscard]] auto is_paused() const -> bool;

private:
    bool is_paused_{false};
    core::Subscription debug_start_sub_;
    core::Subscription debug_stop_sub_;

    void BuildButtons();
};

} // namespace markamp::ui
