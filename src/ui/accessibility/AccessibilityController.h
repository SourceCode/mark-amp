#pragma once

#include "ScreenReaderBridge.h"

#include <memory>

namespace markamp::ui::accessibility
{

/// Central singleton controller that wires application events to the Screen Reader Bridge.
class AccessibilityController
{
public:
    static auto get() -> AccessibilityController&;

    /// Instantiates the accessibility bridge and registers event listeners.
    void initialize();

    /// Announces a general message.
    void announce(const std::string& message, bool assertive = false);

    /// Announces that focus has moved to a new control.
    void announce_focus(const std::string& control_name,
                        const std::string& role,
                        const std::string& state = "");

    /// Notifies the screen reader that the state of a control has changed.
    void notify_state_change(const std::string& control_name, const std::string& state);

private:
    AccessibilityController();
    ~AccessibilityController();

    AccessibilityController(const AccessibilityController&) = delete;
    auto operator=(const AccessibilityController&) -> AccessibilityController& = delete;

    std::unique_ptr<ScreenReaderBridge> bridge_;
};

} // namespace markamp::ui::accessibility
