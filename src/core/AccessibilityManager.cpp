#include "AccessibilityManager.h"

#include "Events.h"

namespace markamp::core
{

AccessibilityManager::AccessibilityManager(EventBus& event_bus,
                                           Config& config,
                                           platform::PlatformAbstraction& platform)
    : event_bus_(event_bus)
    , config_(config)
    , platform_(platform)
{
    load_from_config();
}

// ═══════════════════════════════════════════════════════
// Scale factor
// ═══════════════════════════════════════════════════════

void AccessibilityManager::set_scale_factor(float factor)
{
    const float clamped = std::clamp(factor, kMinScale, kMaxScale);
    if (std::abs(clamped - scale_factor_) < 0.001F)
    {
        return; // no change
    }
    scale_factor_ = clamped;

    events::UIScaleChangedEvent evt;
    evt.scale_factor = scale_factor_;
    event_bus_.publish(evt);
}

void AccessibilityManager::zoom_in()
{
    set_scale_factor(scale_factor_ + kScaleStep);
}

void AccessibilityManager::zoom_out()
{
    set_scale_factor(scale_factor_ - kScaleStep);
}

void AccessibilityManager::zoom_reset()
{
    set_scale_factor(kDefaultScale);
}

// ═══════════════════════════════════════════════════════
// Input mode
// ═══════════════════════════════════════════════════════

void AccessibilityManager::on_key_input()
{
    if (!using_keyboard_)
    {
        using_keyboard_ = true;

        events::InputModeChangedEvent evt;
        evt.using_keyboard = true;
        event_bus_.publish(evt);
    }
}

void AccessibilityManager::on_mouse_input()
{
    if (using_keyboard_)
    {
        using_keyboard_ = false;

        events::InputModeChangedEvent evt;
        evt.using_keyboard = false;
        event_bus_.publish(evt);
    }
}

// ═══════════════════════════════════════════════════════
// Platform delegation
// ═══════════════════════════════════════════════════════

auto AccessibilityManager::is_high_contrast() const -> bool
{
    return platform_.is_high_contrast();
}

auto AccessibilityManager::prefers_reduced_motion() const -> bool
{
    return platform_.prefers_reduced_motion();
}

void AccessibilityManager::announce(wxWindow* window, const wxString& message)
{
    platform_.announce_to_screen_reader(window, message);
}

// ═══════════════════════════════════════════════════════
// Config persistence
// ═══════════════════════════════════════════════════════

void AccessibilityManager::load_from_config()
{
    const double stored =
        config_.get_double("accessibility.ui_scale", static_cast<double>(kDefaultScale));
    scale_factor_ = std::clamp(static_cast<float>(stored), kMinScale, kMaxScale);
}

void AccessibilityManager::save_to_config()
{
    config_.set("accessibility.ui_scale", static_cast<double>(scale_factor_));
}

} // namespace markamp::core
