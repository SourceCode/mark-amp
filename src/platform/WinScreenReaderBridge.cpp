#include "ScreenReaderBridge.h"

#include <spdlog/spdlog.h>

namespace markamp::ui::accessibility
{

class WinScreenReaderBridge : public ScreenReaderBridge
{
public:
    void announce(const std::string& message, bool assertive) override
    {
        // Stub for Windows UI Automation
        spdlog::debug("WinScreenReaderBridge::announce: {} (assertive: {})", message, assertive);
    }

    void announce_focus(const std::string& control_name,
                        const std::string& role,
                        const std::string& state) override
    {
        spdlog::debug("WinScreenReaderBridge::announce_focus: {} ({}) - state: {}",
                      control_name,
                      role,
                      state);
    }

    void notify_state_change(const std::string& control_name, const std::string& state) override
    {
        spdlog::debug(
            "WinScreenReaderBridge::notify_state_change: {} - state: {}", control_name, state);
    }
};

// If compiling on Windows, this would be the definition for create():
// auto ScreenReaderBridge::create() -> std::unique_ptr<ScreenReaderBridge>
// {
//     return std::make_unique<WinScreenReaderBridge>();
// }

} // namespace markamp::ui::accessibility
