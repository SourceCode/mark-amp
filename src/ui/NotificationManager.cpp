#include "NotificationManager.h"

#include "core/Logger.h"

#include <wx/dcbuffer.h>
#include <wx/sizer.h>

namespace markamp::ui
{

NotificationManager::NotificationManager(wxWindow* parent,
                                         core::ThemeEngine& theme_engine,
                                         core::EventBus& event_bus)
    : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTRANSPARENT_WINDOW)
    , theme_engine_(theme_engine)
    , event_bus_(event_bus)
    , animation_timer_(this)
{
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    Bind(wxEVT_PAINT, &NotificationManager::OnPaint, this);
    Bind(wxEVT_TIMER, &NotificationManager::OnAnimationTimer, this, animation_timer_.GetId());

    // R16 Fix 31: Click anywhere to dismiss top toast
    Bind(wxEVT_LEFT_DOWN, [this](wxMouseEvent& /*evt*/) { DismissTop(); });

    // Subscribe to notification events
    notification_sub_ = event_bus_.subscribe<core::events::NotificationEvent>(
        [this](const core::events::NotificationEvent& evt)
        { ShowNotification(evt.message, evt.level, evt.duration_ms); });

    // Subscribe to theme changes
    theme_sub_ = event_bus_.subscribe<core::events::ThemeChangedEvent>(
        [this](const core::events::ThemeChangedEvent& /*evt*/) { ApplyTheme(); });
}

void NotificationManager::ShowNotification(const std::string& message,
                                           core::events::NotificationLevel level,
                                           int duration_ms)
{
    NotificationEntry entry;

    // R17 Fix 31: Prepend level-specific icon emoji
    std::string icon_prefix;
    switch (level)
    {
        case core::events::NotificationLevel::Info:
            icon_prefix = "\xE2\x84\xB9\xEF\xB8\x8F "; // ℹ️
            break;
        case core::events::NotificationLevel::Warning:
            icon_prefix = "\xE2\x9A\xA0\xEF\xB8\x8F "; // ⚠️
            break;
        case core::events::NotificationLevel::Error:
            icon_prefix = "\xE2\x9D\x8C "; // ❌
            break;
        case core::events::NotificationLevel::Success:
            icon_prefix = "\xE2\x9C\x85 "; // ✅
            break;
    }
    entry.message = icon_prefix + message;

    entry.level = level;
    entry.duration_ms = duration_ms;
    entry.opacity = 0.0F;
    entry.elapsed_ms = 0;
    entry.dismissing = false;

    toasts_.push_back(entry);

    // Limit visible toasts
    while (static_cast<int>(toasts_.size()) > kMaxVisibleToasts)
    {
        toasts_.pop_front();
    }

    if (!animation_timer_.IsRunning())
    {
        animation_timer_.Start(kFadeStepMs);
    }

    UpdateLayout();
    MARKAMP_LOG_DEBUG("Notification shown: {}", message);
}

void NotificationManager::DismissTop()
{
    if (!toasts_.empty())
    {
        toasts_.back().dismissing = true;
    }
}

void NotificationManager::DismissAll()
{
    for (auto& toast : toasts_)
    {
        toast.dismissing = true;
    }
}

void NotificationManager::OnAnimationTimer(wxTimerEvent& /*event*/)
{
    bool needs_timer = false;

    for (auto iter = toasts_.begin(); iter != toasts_.end();)
    {
        auto& toast = *iter;

        if (toast.dismissing)
        {
            toast.opacity -= kFadeOutSpeed;
            if (toast.opacity <= 0.0F)
            {
                iter = toasts_.erase(iter);
                continue;
            }
            needs_timer = true;
        }
        else if (toast.opacity < 1.0F)
        {
            toast.opacity += kFadeInSpeed;
            if (toast.opacity > 1.0F)
            {
                toast.opacity = 1.0F;
            }
            needs_timer = true;
        }
        else
        {
            // Fully visible — track duration
            toast.elapsed_ms += kFadeStepMs;
            if (toast.duration_ms > 0 && toast.elapsed_ms >= toast.duration_ms)
            {
                toast.dismissing = true;
            }
            needs_timer = true;
        }

        ++iter;
    }

    if (!needs_timer && toasts_.empty())
    {
        animation_timer_.Stop();
    }

    Refresh();
}

void NotificationManager::OnPaint(wxPaintEvent& /*event*/)
{
    wxAutoBufferedPaintDC paint_dc(this);
    paint_dc.Clear();

    if (toasts_.empty())
    {
        return;
    }

    const auto& theme = theme_engine_.current_theme();
    const auto& clr = theme.colors;
    auto size = GetClientSize();

    int bottom_y = size.GetHeight() - kToastMargin;

    for (auto iter = toasts_.rbegin(); iter != toasts_.rend(); ++iter)
    {
        const auto& toast = *iter;

        const int toast_x = size.GetWidth() - kToastWidth - kToastMargin;
        const int toast_y = bottom_y - kToastHeight;

        if (toast_y < 0)
        {
            break;
        }

        // Apply opacity
        auto level_color = GetLevelColor(toast.level);

        // Background
        paint_dc.SetBrush(wxBrush(wxColour(clr.bg_panel.r,
                                           clr.bg_panel.g,
                                           clr.bg_panel.b,
                                           static_cast<unsigned char>(toast.opacity * 230.0F))));
        paint_dc.SetPen(*wxTRANSPARENT_PEN);
        paint_dc.DrawRoundedRectangle(toast_x, toast_y, kToastWidth, kToastHeight, 6);

        // Left accent bar
        paint_dc.SetBrush(wxBrush(level_color));
        paint_dc.DrawRectangle(toast_x, toast_y, 4, kToastHeight);

        // Text
        paint_dc.SetTextForeground(clr.editor_fg.to_wx_colour());
        paint_dc.DrawText(toast.message, toast_x + 12, toast_y + (kToastHeight - 16) / 2);

        // R16 Fix 32: Dismiss progress bar at bottom of toast
        if (toast.duration_ms > 0 && !toast.dismissing && toast.opacity >= 1.0F)
        {
            float progress = 1.0F - (static_cast<float>(toast.elapsed_ms) /
                                     static_cast<float>(toast.duration_ms));
            int bar_width = static_cast<int>(static_cast<float>(kToastWidth - 8) * progress);
            paint_dc.SetBrush(wxBrush(level_color));
            paint_dc.SetPen(*wxTRANSPARENT_PEN);
            paint_dc.DrawRoundedRectangle(toast_x + 4, toast_y + kToastHeight - 4, bar_width, 2, 1);
        }

        bottom_y = toast_y - kToastMargin;
    }
}

void NotificationManager::UpdateLayout()
{
    auto parent_size = GetParent()->GetClientSize();
    SetSize(parent_size);
    Raise();
}

void NotificationManager::ApplyTheme()
{
    Refresh();
}

auto NotificationManager::GetLevelColor(core::events::NotificationLevel level) -> wxColour
{
    switch (level)
    {
        case core::events::NotificationLevel::Info:
            return {70, 140, 255};
        case core::events::NotificationLevel::Warning:
            return {255, 200, 50};
        case core::events::NotificationLevel::Error:
            return {255, 80, 80};
        case core::events::NotificationLevel::Success:
            return {80, 200, 120};
    }
    return {70, 140, 255};
}

} // namespace markamp::ui
