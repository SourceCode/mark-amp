#pragma once

#include "core/DebugSessionManager.h"
#include "core/ThemeEngine.h"

#include <wx/button.h>
#include <wx/panel.h>
#include <wx/sizer.h>

#include <functional>

namespace markamp::core
{
class EventBus;
} // namespace markamp::core

namespace markamp::ui
{

/// Phase 19 Task 6: Floating debug toolbar shown at top-center of editor area.
/// Contains Continue/Pause, Step Over/Into/Out, Restart, Stop buttons.
/// Only visible when a debug session is active.
class DebugToolbar : public wxPanel
{
public:
    DebugToolbar(wxWindow* parent,
                 core::ThemeEngine& theme_engine,
                 core::EventBus& event_bus,
                 core::DebugSessionManager& session_mgr);

    /// Apply theme colors to buttons.
    void ApplyTheme();

    /// Update button visibility based on debug state.
    void UpdateState();

    // ── Callbacks ──
    void SetOnContinue(std::function<void()> callback);
    void SetOnPause(std::function<void()> callback);
    void SetOnStepOver(std::function<void()> callback);
    void SetOnStepInto(std::function<void()> callback);
    void SetOnStepOut(std::function<void()> callback);
    void SetOnRestart(std::function<void()> callback);
    void SetOnStop(std::function<void()> callback);

private:
    core::ThemeEngine& theme_engine_;
    [[maybe_unused]] core::EventBus& event_bus_;
    core::DebugSessionManager& session_mgr_;

    wxButton* btn_continue_{nullptr};
    wxButton* btn_pause_{nullptr};
    wxButton* btn_step_over_{nullptr};
    wxButton* btn_step_into_{nullptr};
    wxButton* btn_step_out_{nullptr};
    wxButton* btn_restart_{nullptr};
    wxButton* btn_stop_{nullptr};

    std::function<void()> on_continue_;
    std::function<void()> on_pause_;
    std::function<void()> on_step_over_;
    std::function<void()> on_step_into_;
    std::function<void()> on_step_out_;
    std::function<void()> on_restart_;
    std::function<void()> on_stop_;

    void CreateLayout();
};

} // namespace markamp::ui
