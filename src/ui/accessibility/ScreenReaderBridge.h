#pragma once

#include <memory>
#include <string>

namespace markamp::ui::accessibility
{

/// Interface for interacting with native platform screen readers.
class ScreenReaderBridge
{
public:
    virtual ~ScreenReaderBridge() = default;

    /// Creates the platform-specific implementation.
    static auto create() -> std::unique_ptr<ScreenReaderBridge>;

    /// Announces a general message to the screen reader.
    virtual void announce(const std::string& message, bool assertive = false) = 0;

    /// Announces that focus has moved to a new control.
    virtual void announce_focus(const std::string& control_name,
                                const std::string& role,
                                const std::string& state = "") = 0;

    /// Notifies the screen reader that the state of a control has changed (e.g. checked, expanded).
    virtual void notify_state_change(const std::string& control_name, const std::string& state) = 0;
};

} // namespace markamp::ui::accessibility
