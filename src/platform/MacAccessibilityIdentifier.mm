#include "AccessibilityIdentifier.h"

#include "core/Logger.h"
#import <AppKit/AppKit.h>

namespace markamp {
namespace platform {

/// Ensure a single NSView is set up as an accessible container:
///   - accessibilityIdentifier / label (only if provided)
///   - NSAccessibilityGroupRole if it currently has no role or Unknown
///   - accessibilityEnabled = YES
///   - accessibilityChildren includes all subviews
static void ensure_view_accessible(NSView *view, NSString *nsId) {
  if (nsId != nil && [nsId length] > 0) {
    [view setAccessibilityIdentifier:nsId];
    [view setAccessibilityLabel:nsId];
  }

  // If the view has no role or an unknown role, set it to Group so
  // XCUITest will traverse into its children.
  NSAccessibilityRole currentRole = [view accessibilityRole];
  if (currentRole == nil || [currentRole length] == 0 ||
      [currentRole isEqualToString:NSAccessibilityUnknownRole]) {
    [view setAccessibilityRole:NSAccessibilityGroupRole];
  }

  [view setAccessibilityEnabled:YES];

  // Ensure the view's accessibility children include all its subviews
  NSArray<NSView *> *subviews = [view subviews];
  if ([subviews count] > 0) {
    // Build a merged set: existing children + all subviews
    NSArray *existing = [view accessibilityChildren];
    NSMutableOrderedSet *merged = [NSMutableOrderedSet orderedSet];
    if (existing != nil) {
      [merged addObjectsFromArray:existing];
    }
    for (NSView *child in subviews) {
      if (![merged containsObject:child]) {
        [merged addObject:child];
      }
    }
    [view setAccessibilityChildren:[merged array]];
  }
}

/// Walk from `view` up through every superview until the window's
/// contentView, ensuring each intermediate NSView is set up as an
/// accessible container that exposes its subviews as children.
/// This is the key fix: without this walk, XCUITest cannot reach
/// child panels through multiple layers of wrapper NSViews.
static void propagate_hierarchy_to_window(NSView *view) {
  NSView *current = [view superview];
  while (current != nil) {
    ensure_view_accessible(current, nil); // Don't override identifier
    current = [current superview];
  }
}

/// Post an accessibility layout-changed notification so the system
/// knows the accessibility tree structure has changed.
static void post_ax_layout_changed(NSView *view) {
  NSAccessibilityPostNotification(view,
                                  NSAccessibilityLayoutChangedNotification);

  // Also post from the window if available
  NSWindow *window = [view window];
  if (window != nil) {
    NSAccessibilityPostNotification(window,
                                    NSAccessibilityLayoutChangedNotification);
  }
}

void set_accessibility_identifier(wxWindow *window,
                                  const std::string &identifier) {
  if (window == nullptr) {
    return;
  }

  // Capture the identifier for deferred use
  const std::string id_copy = identifier;

  // Try setting immediately if the native peer exists
  @autoreleasepool {
    auto *const view = reinterpret_cast<NSView *>(window->GetHandle());
    if (view != nil) {
      auto *const nsId = [NSString stringWithUTF8String:identifier.c_str()];
      ensure_view_accessible(view, nsId);
      propagate_hierarchy_to_window(view);

      MARKAMP_LOG_DEBUG("set_accessibility_identifier (immediate): {} on {}",
                        identifier, [[view className] UTF8String]);
    }
  }

  // Also defer via CallAfter to handle the case where the native peer
  // isn't fully realized during the constructor.  CallAfter schedules
  // a callback on the next event-loop iteration, at which point the
  // peer is guaranteed to exist and part of the view hierarchy.
  window->CallAfter([window, id_copy]() {
    @autoreleasepool {
      auto *const view = reinterpret_cast<NSView *>(window->GetHandle());
      if (view != nil) {
        auto *const nsId = [NSString stringWithUTF8String:id_copy.c_str()];
        ensure_view_accessible(view, nsId);
        propagate_hierarchy_to_window(view);
        post_ax_layout_changed(view);

        MARKAMP_LOG_DEBUG(
            "set_accessibility_identifier (deferred): {} on {} (subviews: {})",
            id_copy, [[view className] UTF8String],
            static_cast<unsigned long>([[view subviews] count]));
      }
    }
  });
}

/// Recursively walk all child wxWindows and re-apply accessibility
/// identifiers for any window whose name starts with "ma.".
/// Also ensures the full NSView hierarchy is set up for traversal.
///
/// Call this after showing a previously-hidden parent (e.g. showEditor)
/// to fix the accessibility tree after structural changes.
static void refresh_tree_recursive(wxWindow *window) {
  if (window == nullptr) {
    return;
  }

  // If this window has an identifier (name starts with "ma."),
  // re-apply it to ensure the NSView bridge is correctly set up.
  const std::string name = window->GetName().ToStdString();
  if (name.rfind("ma.", 0) == 0) {
    @autoreleasepool {
      auto *const view = reinterpret_cast<NSView *>(window->GetHandle());
      if (view != nil) {
        auto *const nsId = [NSString stringWithUTF8String:name.c_str()];
        ensure_view_accessible(view, nsId);
        propagate_hierarchy_to_window(view);
      }
    }
  } else {
    // Even without an explicit identifier, ensure this intermediate
    // view exposes its subviews for traversal
    @autoreleasepool {
      auto *const view = reinterpret_cast<NSView *>(window->GetHandle());
      if (view != nil) {
        ensure_view_accessible(view, nil);
      }
    }
  }

  // Recurse into all children
  for (auto *child : window->GetChildren()) {
    refresh_tree_recursive(child);
  }
}

void refresh_accessibility_tree(wxWindow *window) {
  if (window == nullptr) {
    return;
  }

  refresh_tree_recursive(window);

  // Post a single layout-changed notification from the top-level window
  @autoreleasepool {
    auto *const view = reinterpret_cast<NSView *>(window->GetHandle());
    if (view != nil) {
      post_ax_layout_changed(view);

      MARKAMP_LOG_DEBUG(
          "refresh_accessibility_tree: refreshed from {} (subviews: {})",
          [[view className] UTF8String],
          static_cast<unsigned long>([[view subviews] count]));
    }
  }
}

} // namespace platform
} // namespace markamp
