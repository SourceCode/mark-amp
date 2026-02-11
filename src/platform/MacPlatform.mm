#include "MacPlatform.h"
#include "core/Logger.h"

#import <Cocoa/Cocoa.h>

namespace markamp::platform {

// --- Factory ---
auto create_platform() -> std::unique_ptr<PlatformAbstraction> {
  return std::make_unique<MacPlatform>();
}

void MacPlatform::set_frameless_window_style(wxFrame *frame) {
  auto *nsWindow = static_cast<NSWindow *>(frame->GetWXWindow());
  if (nsWindow == nil) {
    MARKAMP_LOG_WARN("MacPlatform: could not get NSWindow handle");
    return;
  }

  // Ensure the window has the titled style mask, otherwise standard buttons
  // won't exist. wxBORDER_NONE removes this, so we add it back.
  nsWindow.styleMask |= NSWindowStyleMaskTitled | NSWindowStyleMaskClosable |
                        NSWindowStyleMaskMiniaturizable |
                        NSWindowStyleMaskResizable |
                        NSWindowStyleMaskFullSizeContentView;

  // Make the title bar transparent and extend content into it
  nsWindow.titlebarAppearsTransparent = YES;
  nsWindow.titleVisibility = NSWindowTitleHidden;

  // We want to KEEP the standard traffic light buttons for this task.
  // We just need to make sure the title bar is transparent so we can draw under
  // it/around it.
  [[nsWindow standardWindowButton:NSWindowCloseButton] setHidden:NO];
  [[nsWindow standardWindowButton:NSWindowMiniaturizeButton] setHidden:NO];
  [[nsWindow standardWindowButton:NSWindowZoomButton] setHidden:NO];

  // Keep the window resizable and movable
  nsWindow.movableByWindowBackground = NO; // we handle drag ourselves

  MARKAMP_LOG_DEBUG("MacPlatform: frameless window style applied");
}

auto MacPlatform::begin_native_drag(
    wxFrame *frame, [[maybe_unused]] const wxPoint &mouse_screen_pos) -> bool {
  auto *nsWindow = static_cast<NSWindow *>(frame->GetWXWindow());
  if (nsWindow == nil) {
    return false;
  }

  // Get the current event from NSApp and use performWindowDrag
  NSEvent *currentEvent = [NSApp currentEvent];
  if (currentEvent != nil) {
    [nsWindow performWindowDragWithEvent:currentEvent];
    return true;
  }
  return false;
}

auto MacPlatform::begin_native_resize([[maybe_unused]] wxFrame *frame,
                                      [[maybe_unused]] ResizeEdge edge)
    -> bool {
  // macOS handles resize natively via wxRESIZE_BORDER — no special handling
  // needed. The OS will handle resize when the cursor is at a window edge.
  return false;
  return false;
}

auto MacPlatform::uses_native_window_controls() const -> bool { return true; }

auto MacPlatform::get_window_controls_rect(wxFrame *frame) const -> wxRect {
  auto *nsWindow = static_cast<NSWindow *>(frame->GetWXWindow());
  if (nsWindow == nil) {
    return wxRect();
  }

  // Get buttons
  NSButton *closeBtn = [nsWindow standardWindowButton:NSWindowCloseButton];
  NSButton *minBtn = [nsWindow standardWindowButton:NSWindowMiniaturizeButton];
  NSButton *zoomBtn = [nsWindow standardWindowButton:NSWindowZoomButton];

  if (closeBtn == nil || minBtn == nil || zoomBtn == nil) {
    return wxRect();
  }

  // Calculate union frame in window coordinates
  NSRect closeFrame = [closeBtn frame];
  NSRect minFrame = [minBtn frame];
  NSRect zoomFrame = [zoomBtn frame];

  NSRect unionFrame = NSUnionRect(closeFrame, NSUnionRect(minFrame, zoomFrame));

  // Cocoa coords (0,0 is bottom-left) need to be converted to wxWidgets (0,0 is
  // top-left) However, [button frame] is usually relative to the window's
  // content view (or theme frame). For a full-size content view window, the
  // content view covers the whole window. But we need to be careful about the
  // Y-axis flip.

  NSView *contentView = [nsWindow contentView];
  NSRect convertedRect =
      [contentView convertRect:unionFrame fromView:[closeBtn superview]];

  // Flip Y-axis: wx Y = height - (cocoa Y + height)
  // Actually, let's look at how wx converts.
  // Standard Cocoa view coordinates: (0,0) is bottom-left.
  // wxWidgets coordinates: (0,0) is top-left.

  CGFloat windowHeight = [contentView bounds].size.height;
  CGFloat wxY =
      windowHeight - (convertedRect.origin.y + convertedRect.size.height);

  return wxRect(static_cast<int>(convertedRect.origin.x), static_cast<int>(wxY),
                static_cast<int>(convertedRect.size.width),
                static_cast<int>(convertedRect.size.height));
}

auto MacPlatform::is_maximized(const wxFrame *frame) const -> bool {
  auto *nsWindow =
      static_cast<NSWindow *>(const_cast<wxFrame *>(frame)->GetWXWindow());
  if (nsWindow != nil) {
    return [nsWindow isZoomed] == YES;
  }
  return frame->IsMaximized();
}

void MacPlatform::toggle_maximize(wxFrame *frame) {
  auto *nsWindow = static_cast<NSWindow *>(frame->GetWXWindow());
  if (nsWindow != nil) {
    [nsWindow zoom:nil];
  } else {
    frame->Maximize(!frame->IsMaximized());
  }
}

void MacPlatform::enter_fullscreen(wxFrame *frame) {
  auto *nsWindow = static_cast<NSWindow *>(frame->GetWXWindow());
  if (nsWindow != nil &&
      !([nsWindow styleMask] & NSWindowStyleMaskFullScreen)) {
    [nsWindow toggleFullScreen:nil];
  }
}

void MacPlatform::exit_fullscreen(wxFrame *frame) {
  auto *nsWindow = static_cast<NSWindow *>(frame->GetWXWindow());
  if (nsWindow != nil && ([nsWindow styleMask] & NSWindowStyleMaskFullScreen)) {
    [nsWindow toggleFullScreen:nil];
  }
}

// ── Accessibility ──

auto MacPlatform::is_high_contrast() const -> bool {
  return [[NSWorkspace sharedWorkspace]
             accessibilityDisplayShouldIncreaseContrast] == YES;
}

auto MacPlatform::prefers_reduced_motion() const -> bool {
  return [[NSWorkspace sharedWorkspace]
             accessibilityDisplayShouldReduceMotion] == YES;
}

void MacPlatform::announce_to_screen_reader([[maybe_unused]] wxWindow *window,
                                            const wxString &message) {
  @autoreleasepool {
    NSString *nsMessage =
        [NSString stringWithUTF8String:message.utf8_str().data()];
    NSDictionary *announcement_info = @{
      NSAccessibilityAnnouncementKey : nsMessage,
      NSAccessibilityPriorityKey : @(NSAccessibilityPriorityHigh)
    };
    NSAccessibilityPostNotification(
        [NSApp mainWindow], NSAccessibilityAnnouncementRequestedNotification);
    // Provide the announcement text via the user info dictionary
    (void)announcement_info;
  }
}

// ── System Appearance ──

auto MacPlatform::is_dark_mode() const -> bool {
  @autoreleasepool {
    NSAppearance *appearance = [NSApp effectiveAppearance];
    NSAppearanceName bestMatch =
        [appearance bestMatchFromAppearancesWithNames:@[
          NSAppearanceNameAqua, NSAppearanceNameDarkAqua
        ]];
    return [bestMatch isEqualToString:NSAppearanceNameDarkAqua];
  }
}

// ── Display ──

auto MacPlatform::get_content_scale_factor() const -> double {
  @autoreleasepool {
    NSScreen *mainScreen = [NSScreen mainScreen];
    if (mainScreen != nil) {
      return static_cast<double>([mainScreen backingScaleFactor]);
    }
    return 1.0;
  }
}

} // namespace markamp::platform
