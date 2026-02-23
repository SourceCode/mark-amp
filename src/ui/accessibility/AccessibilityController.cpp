#include "AccessibilityController.h"

#include "../../core/EventBus.h"
#include "../../core/Events.h"
#include "../FocusManager.h"

// Note: Ensure core::events::FocusChangedEvent and StateChangedEvent exist,
// or subscribe to whatever events are appropriate if added later.

namespace markamp::ui::accessibility
{

AccessibilityController::AccessibilityController() = default;

AccessibilityController::~AccessibilityController() = default;

void AccessibilityController::initialize()
{
    bridge_ = ScreenReaderBridge::create();

    // In a full implementation, we would subscribe to EventBus events here:
    // auto& bus = core::EventBus::get();
    // bus.subscribe<core::events::FocusChangedEvent>([this](const auto& ev) {
    //     // map zone/item to an accessible name and role
    //     // bridge_->announce_focus(name, role, state);
    // });
}

void AccessibilityController::announce(const std::string& message, bool assertive)
{
    if (bridge_)
    {
        bridge_->announce(message, assertive);
    }
}

} // namespace markamp::ui::accessibility
