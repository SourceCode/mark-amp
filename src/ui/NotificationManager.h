#pragma once

#include "core/EventBus.h"
#include "core/Events.h"
#include "core/ThemeEngine.h"

#include <wx/panel.h>
#include <wx/stattext.h>
#include <wx/timer.h>

#include <deque>
#include <string>

namespace markamp::ui
{

/// A single toast notification entry
struct NotificationEntry
{
    std::string message;
    core::events::NotificationLevel level{core::events::NotificationLevel::Info};
    int duration_ms{3000};
    int elapsed_ms{0};
    float opacity{0.0F};
    bool dismissing{false};
};

/// Toast notification manager inspired by VS Code's notification system.
/// Displays brief messages in the bottom-right corner of the window.
/// Supports Info, Warning, Error, and Success levels with auto-dismiss.
class NotificationManager : public wxPanel
{
public:
    NotificationManager(wxWindow* parent,
                        core::ThemeEngine& theme_engine,
                        core::EventBus& event_bus);

    /// Show a notification toast
    void
    ShowNotification(const std::string& message,
                     core::events::NotificationLevel level = core::events::NotificationLevel::Info,
                     int duration_ms = 3000);

    /// Dismiss the topmost notification
    void DismissTop();

    /// Dismiss all notifications
    void DismissAll();

    static constexpr int kMaxVisibleToasts = 3;
    static constexpr int kToastWidth = 360;
    static constexpr int kToastHeight = 48;
    static constexpr int kToastMargin = 8;
    static constexpr int kFadeStepMs = 16;
    static constexpr float kFadeInSpeed = 0.08F;
    static constexpr float kFadeOutSpeed = 0.12F;

private:
    core::ThemeEngine& theme_engine_;
    core::EventBus& event_bus_;

    std::deque<NotificationEntry> toasts_;
    wxTimer animation_timer_;

    core::Subscription notification_sub_;
    core::Subscription theme_sub_;

    void OnAnimationTimer(wxTimerEvent& event);
    void OnPaint(wxPaintEvent& event);
    void UpdateLayout();
    void ApplyTheme();

    static auto GetLevelColor(core::events::NotificationLevel level) -> wxColour;
};

} // namespace markamp::ui
