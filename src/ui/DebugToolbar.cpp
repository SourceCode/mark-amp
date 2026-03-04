#include "DebugToolbar.h"

#include "core/Events.h"

namespace markamp::ui
{

DebugToolbar::DebugToolbar(wxWindow* parent,
                           core::ThemeEngine& theme_engine,
                           core::EventBus& event_bus)
    : FloatingToolbar(parent, theme_engine, event_bus, "debug_toolbar")
{
    SetAutoHideMs(0); // Debug toolbar stays visible until stopped
    SetDraggable(true);
    SetSnapToEdges(true);
    BuildButtons();
}

void DebugToolbar::OnDebugStarted()
{
    is_paused_ = false;
    RefreshButtonStates();
    ShowAtScreenCenter();
}

void DebugToolbar::OnDebugPaused()
{
    is_paused_ = true;
    RefreshButtonStates();
}

void DebugToolbar::OnDebugContinued()
{
    is_paused_ = false;
    RefreshButtonStates();
}

void DebugToolbar::OnDebugStopped()
{
    is_paused_ = false;
    HideToolbar();
}

auto DebugToolbar::is_paused() const -> bool
{
    return is_paused_;
}

void DebugToolbar::BuildButtons()
{
    std::vector<FloatingToolbarButton> buttons;

    // Continue / Pause
    FloatingToolbarButton continue_btn;
    continue_btn.id = "continue";
    continue_btn.label = "\u25B6"; // ▶
    continue_btn.tooltip = "Continue (F5)";
    continue_btn.shortcut = "F5";
    continue_btn.is_enabled = [this]() { return is_paused_; };
    continue_btn.callback = [this]()
    {
        core::events::CommandExecutedEvent evt;
        evt.command_id = "debug.continue";
        evt.source = "toolbar";
        event_bus_.publish(evt);
    };
    buttons.push_back(std::move(continue_btn));

    // Step Over
    FloatingToolbarButton step_over_btn;
    step_over_btn.id = "step_over";
    step_over_btn.label = "\u2B9E"; // ⮞
    step_over_btn.tooltip = "Step Over (F10)";
    step_over_btn.shortcut = "F10";
    step_over_btn.is_enabled = [this]() { return is_paused_; };
    step_over_btn.callback = [this]()
    {
        core::events::CommandExecutedEvent evt;
        evt.command_id = "debug.step_over";
        evt.source = "toolbar";
        event_bus_.publish(evt);
    };
    buttons.push_back(std::move(step_over_btn));

    // Step In
    FloatingToolbarButton step_in_btn;
    step_in_btn.id = "step_in";
    step_in_btn.label = "\u2B07"; // ⬇
    step_in_btn.tooltip = "Step Into (F11)";
    step_in_btn.shortcut = "F11";
    step_in_btn.is_enabled = [this]() { return is_paused_; };
    step_in_btn.callback = [this]()
    {
        core::events::CommandExecutedEvent evt;
        evt.command_id = "debug.step_in";
        evt.source = "toolbar";
        event_bus_.publish(evt);
    };
    buttons.push_back(std::move(step_in_btn));

    // Step Out
    FloatingToolbarButton step_out_btn;
    step_out_btn.id = "step_out";
    step_out_btn.label = "\u2B06"; // ⬆
    step_out_btn.tooltip = "Step Out (Shift+F11)";
    step_out_btn.shortcut = "Shift+F11";
    step_out_btn.is_enabled = [this]() { return is_paused_; };
    step_out_btn.callback = [this]()
    {
        core::events::CommandExecutedEvent evt;
        evt.command_id = "debug.step_out";
        evt.source = "toolbar";
        event_bus_.publish(evt);
    };
    buttons.push_back(std::move(step_out_btn));

    // Separator
    FloatingToolbarButton sep;
    sep.is_separator = true;
    buttons.push_back(std::move(sep));

    // Restart
    FloatingToolbarButton restart_btn;
    restart_btn.id = "restart";
    restart_btn.label = "\u21BB"; // ↻
    restart_btn.tooltip = "Restart (Ctrl+Shift+F5)";
    restart_btn.shortcut = "Ctrl+Shift+F5";
    restart_btn.callback = [this]()
    {
        core::events::CommandExecutedEvent evt;
        evt.command_id = "debug.restart";
        evt.source = "toolbar";
        event_bus_.publish(evt);
    };
    buttons.push_back(std::move(restart_btn));

    // Stop
    FloatingToolbarButton stop_btn;
    stop_btn.id = "stop";
    stop_btn.label = "\u25A0"; // ■
    stop_btn.tooltip = "Stop (Shift+F5)";
    stop_btn.shortcut = "Shift+F5";
    stop_btn.callback = [this]()
    {
        core::events::CommandExecutedEvent evt;
        evt.command_id = "debug.stop";
        evt.source = "toolbar";
        event_bus_.publish(evt);
    };
    buttons.push_back(std::move(stop_btn));

    SetButtons(std::move(buttons));
}

} // namespace markamp::ui
