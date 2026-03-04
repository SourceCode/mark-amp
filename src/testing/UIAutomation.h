// ============================================================================
// File: src/testing/UIAutomation.h
// Phase 50: UI Integration Testing Harness — Automation helpers
// ============================================================================
#pragma once

#include <string>
#include <vector>

namespace markamp::testing
{

/// Result of a UI automation action.
struct AutomationActionResult
{
    bool success{false};
    std::string error_message;
};

/// UI automation event type.
enum class AutomationEventType : uint8_t
{
    Click,
    DoubleClick,
    RightClick,
    TypeText,
    PressKey,
    Drag,
    Hover,
    Scroll,
    WaitForIdle
};

/// A queued automation event for replay.
struct AutomationEvent
{
    AutomationEventType type{AutomationEventType::Click};
    int x{0};
    int y{0};
    int end_x{0};
    int end_y{0};
    std::string text;
    int key_code{0};
    int modifiers{0};
    int scroll_delta{0};
};

/// Static helpers for UI automation (headless-safe).
class UIAutomation
{
public:
    /// Queue an event for later processing.
    static void queue_event(AutomationEvent event);

    /// Get queued events.
    [[nodiscard]] static auto queued_events() -> const std::vector<AutomationEvent>&;

    /// Clear all queued events.
    static void clear_events();

    /// Get queued event count.
    [[nodiscard]] static auto event_count() -> int;

    /// Create a click event.
    [[nodiscard]] static auto make_click(int x, int y) -> AutomationEvent;

    /// Create a type text event.
    [[nodiscard]] static auto make_type(const std::string& text) -> AutomationEvent;

    /// Create a key press event.
    [[nodiscard]] static auto make_key_press(int key_code, int modifiers = 0) -> AutomationEvent;

    /// Create a drag event.
    [[nodiscard]] static auto make_drag(int from_x, int from_y, int to_x, int to_y)
        -> AutomationEvent;

private:
    static std::vector<AutomationEvent> event_queue_; // NOLINT
};

} // namespace markamp::testing
