#pragma once

#include "ScreenReaderBridge.h"

#include <memory>

namespace markamp::ui::accessibility
{

/// Central controller that wires application events to the Screen Reader Bridge.
///
/// It listens for global FocusChanged and StateChanged events (if any)
/// and delegates announcements.
class AccessibilityController
{
public:
    AccessibilityController();
    ~AccessibilityController();

    /// Instantiates the accessibility bridge and registers event listeners.
    void initialize();

    /// Announces a general message.
    void announce(const std::string& message, bool assertive = false);

private:
    std::unique_ptr<ScreenReaderBridge> bridge_;
};

} // namespace markamp::ui::accessibility
