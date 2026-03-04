// ============================================================================
// File: src/testing/UIAutomation.cpp
// Phase 50: UI Integration Testing Harness — Automation helpers
// ============================================================================
#include "UIAutomation.h"

namespace markamp::testing
{

std::vector<AutomationEvent> UIAutomation::event_queue_; // NOLINT

void UIAutomation::queue_event(AutomationEvent event)
{
    event_queue_.push_back(std::move(event));
}

auto UIAutomation::queued_events() -> const std::vector<AutomationEvent>&
{
    return event_queue_;
}

void UIAutomation::clear_events()
{
    event_queue_.clear();
}

auto UIAutomation::event_count() -> int
{
    return static_cast<int>(event_queue_.size());
}

auto UIAutomation::make_click(int x, int y) -> AutomationEvent
{
    return {.type = AutomationEventType::Click, .x = x, .y = y};
}

auto UIAutomation::make_type(const std::string& text) -> AutomationEvent
{
    return {.type = AutomationEventType::TypeText, .text = text};
}

auto UIAutomation::make_key_press(int key_code, int modifiers) -> AutomationEvent
{
    return {.type = AutomationEventType::PressKey, .key_code = key_code, .modifiers = modifiers};
}

auto UIAutomation::make_drag(int from_x, int from_y, int to_x, int to_y) -> AutomationEvent
{
    return {
        .type = AutomationEventType::Drag, .x = from_x, .y = from_y, .end_x = to_x, .end_y = to_y};
}

} // namespace markamp::testing
