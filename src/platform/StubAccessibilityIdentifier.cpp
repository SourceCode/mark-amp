#include "AccessibilityIdentifier.h"

namespace markamp::platform
{

void set_accessibility_identifier([[maybe_unused]] wxWindow* window,
                                  [[maybe_unused]] const std::string& identifier)
{
    // No-op on Windows/Linux — only macOS XCUITest needs accessibility identifiers
}

void refresh_accessibility_tree([[maybe_unused]] wxWindow* window)
{
    // No-op on Windows/Linux
}

} // namespace markamp::platform
