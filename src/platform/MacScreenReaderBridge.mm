#include "../ui/accessibility/ScreenReaderBridge.h"

#import <AppKit/AppKit.h>
#include <spdlog/spdlog.h>

namespace markamp::ui::accessibility {

class MacScreenReaderBridge : public ScreenReaderBridge {
public:
  void announce(const std::string &message, bool assertive) override {
    @autoreleasepool {
      NSString *nsMessage = [NSString stringWithUTF8String:message.c_str()];

      // For standard announcements, we post
      // NSAccessibilityAnnouncementRequestedNotification to the main app
      NSAccessibilityPriorityLevel priority =
          assertive ? NSAccessibilityPriorityHigh
                    : NSAccessibilityPriorityMedium;

      NSDictionary *userInfo = @{
        NSAccessibilityAnnouncementKey : nsMessage,
        NSAccessibilityPriorityKey : @(priority)
      };

      NSAccessibilityPostNotificationWithUserInfo(
          NSApp, NSAccessibilityAnnouncementRequestedNotification, userInfo);

      spdlog::debug("MacScreenReaderBridge::announce posted: {}", message);
    }
  }

  void announce_focus(const std::string &control_name, const std::string &role,
                      const std::string &state) override {
    // Construct a full string representing the focused item. A true native
    // integration would ideally use native focused UI elements, but this bridge
    // allows custom rendered elements to manually tell the screen reader what
    // to say.
    std::string full_announcement = control_name + ", " + role;
    if (!state.empty()) {
      full_announcement += ", " + state;
    }

    announce(full_announcement, false);
  }

  void notify_state_change(const std::string &control_name,
                           const std::string &state) override {
    std::string full_announcement = control_name + " is now " + state;
    announce(full_announcement, false);
  }
};

#ifdef __APPLE__
auto ScreenReaderBridge::create() -> std::unique_ptr<ScreenReaderBridge> {
  return std::make_unique<MacScreenReaderBridge>();
}
#endif

} // namespace markamp::ui::accessibility
