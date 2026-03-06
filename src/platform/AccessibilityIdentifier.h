#pragma once

#include <wx/wx.h>

#include <string>

namespace markamp::platform
{

/// Set a stable accessibility identifier on a wxWindow's underlying native view.
///
/// On macOS this calls [NSView setAccessibilityIdentifier:] so that XCUITest
/// (and therefore Appium mac2) can discover elements by their `identifier`
/// attribute.  On other platforms this is a no-op.
///
/// Call immediately after wxWindow::SetName() to keep the wx name and the
/// native accessibility identifier in sync.
void set_accessibility_identifier(wxWindow* window, const std::string& identifier);

/// Re-apply accessibility identifiers on all descendant wxWindows whose
/// name starts with "ma." and ensure the full NSView hierarchy is traversable.
///
/// Call this after showing a previously-hidden parent window (e.g. after
/// MainFrame::showEditor() makes the LayoutManager visible) to rebuild
/// the accessibility tree that XCUITest traverses.
///
/// On non-macOS platforms this is a no-op.
void refresh_accessibility_tree(wxWindow* window);

} // namespace markamp::platform
