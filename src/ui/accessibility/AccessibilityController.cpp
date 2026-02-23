#include "AccessibilityController.h"

#include "../../core/EventBus.h"
#include "../../core/Events.h"
#include "../FocusManager.h"

namespace markamp::ui::accessibility
{

AccessibilityController::AccessibilityController() = default;

AccessibilityController::~AccessibilityController() = default;

auto AccessibilityController::get() -> AccessibilityController&
{
    static AccessibilityController instance;
    return instance;
}

void AccessibilityController::initialize()
{
    bridge_ = ScreenReaderBridge::create();
}

void AccessibilityController::announce(const std::string& message, bool assertive)
{
    if (bridge_)
    {
        bridge_->announce(message, assertive);
    }
}

void AccessibilityController::announce_focus(const std::string& control_name,
                                             const std::string& role,
                                             const std::string& state)
{
    if (bridge_)
    {
        bridge_->announce_focus(control_name, role, state);
    }
}

void AccessibilityController::notify_state_change(const std::string& control_name,
                                                  const std::string& state)
{
    if (bridge_)
    {
        bridge_->notify_state_change(control_name, state);
    }
}

} // namespace markamp::ui::accessibility
