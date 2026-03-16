#include "../ui/accessibility/ScreenReaderBridge.h"

#include <spdlog/spdlog.h>

namespace markamp::ui::accessibility
{

// Linux implementation stub
class LinuxScreenReaderBridge : public ScreenReaderBridge
{
public:
    void announce(const std::string& message, bool assertive) override
    {
        // Wire to AT-SPI D-Bus interface for Orca/BRLTTY screen readers.
        spdlog::debug("LinuxScreenReaderBridge::announce: {} (assertive: {})", message, assertive);

#ifdef __linux__
        // In a full implementation, we would send a D-Bus message to the
        // AT-SPI bus at org.a11y.Bus. The message would be routed to:
        //   org.a11y.atspi.Event.Object:Announcement
        // The assertive flag maps to:
        //   ATSPI_LIVE_ASSERTIVE (2) vs ATSPI_LIVE_POLITE (1)
        (void)assertive;
        (void)message;
#endif
    }

    void announce_focus(const std::string& control_name,
                        const std::string& role,
                        const std::string& state) override
    {
        spdlog::debug("LinuxScreenReaderBridge::announce_focus: {} ({}) - state: {}",
                      control_name,
                      role,
                      state);
    }

    void notify_state_change(const std::string& control_name, const std::string& state) override
    {
        spdlog::debug(
            "LinuxScreenReaderBridge::notify_state_change: {} - state: {}", control_name, state);
    }
};

// If compiling on Linux, this would be the definition for create():
// auto ScreenReaderBridge::create() -> std::unique_ptr<ScreenReaderBridge>
// {
//     return std::make_unique<LinuxScreenReaderBridge>();
// }

} // namespace markamp::ui::accessibility
