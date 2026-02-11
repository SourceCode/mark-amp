# Phase 25: Accessibility Features

## Objective

Ensure MarkAmp is accessible to users with disabilities, including screen reader support, high contrast themes, scalable fonts, keyboard-only navigation, and WCAG 2.1 AA compliance for all interactive elements.

## Prerequisites

- Phase 24 (Keyboard Navigation and Shortcuts)
- Phase 18 (Preview Component)

## Deliverables

1. Screen reader compatibility (MSAA/UIA on Windows, VoiceOver on macOS, AT-SPI on Linux)
2. High contrast theme validation
3. Scalable UI (font sizes and spacing)
4. ARIA-like accessibility properties on custom controls
5. Focus indicators for all interactive elements
6. Reduced motion support
7. Accessibility audit report

## Tasks

### Task 25.1: Add accessibility properties to custom controls

All custom-drawn controls (FileTreeCtrl, ThemePreviewCard, StatusBar, CustomChrome, Toolbar) must expose accessibility information.

**wxWidgets accessibility approach:**
Use `wxAccessible` class overrides for custom controls:

```cpp
class FileTreeCtrlAccessible : public wxAccessible
{
public:
    explicit FileTreeCtrlAccessible(FileTreeCtrl* ctrl);

    wxAccStatus GetChildCount(int* child_count) override;
    wxAccStatus GetChild(int child_id, wxAccessible** child) override;
    wxAccStatus GetName(int child_id, wxString* name) override;
    wxAccStatus GetRole(int child_id, wxAccRole* role) override;
    wxAccStatus GetState(int child_id, long* state) override;
    wxAccStatus GetDescription(int child_id, wxString* desc) override;
    wxAccStatus GetValue(int child_id, wxString* value) override;

private:
    FileTreeCtrl* ctrl_;
};
```

**Accessibility roles for each component:**
| Component | Role | Name |
|---|---|---|
| FileTreeCtrl | wxROLE_SYSTEM_OUTLINE (tree view) | "File Explorer" |
| FileNode (folder) | wxROLE_SYSTEM_OUTLINEITEM | Folder name |
| FileNode (file) | wxROLE_SYSTEM_OUTLINEITEM | File name |
| Toolbar | wxROLE_SYSTEM_TOOLBAR | "Main Toolbar" |
| Toolbar button | wxROLE_SYSTEM_PUSHBUTTON | Button name |
| StatusBar | wxROLE_SYSTEM_STATUSBAR | "Status Bar" |
| ThemePreviewCard | wxROLE_SYSTEM_LISTITEM | Theme name |
| ThemeGallery grid | wxROLE_SYSTEM_LIST | "Theme Gallery" |
| EditorPanel | wxROLE_SYSTEM_TEXT | "Markdown Editor" |
| PreviewPanel | wxROLE_SYSTEM_DOCUMENT | "Markdown Preview" |

**States to expose:**
- Selected / not selected
- Expanded / collapsed (for folders)
- Focused / not focused
- Checked / unchecked (for task lists)
- Modified / unmodified (for files)

**Acceptance criteria:**
- Screen readers can enumerate all UI elements
- Each element has a meaningful name and role
- State changes are announced (folder expand/collapse, file selection)
- Custom controls are navigable with screen reader commands

### Task 25.2: Implement high contrast mode detection and response

Detect when the system is in high contrast mode:

**Platform detection:**
- Windows: `SystemParametersInfo(SPI_GETHIGHCONTRAST, ...)`
- macOS: `NSWorkspace.shared.accessibilityDisplayShouldIncreaseContrast`
- Linux: Check GTK high contrast theme setting

**Response:**
1. When high contrast mode is detected, auto-switch to "High Contrast Blue" theme
2. Increase border widths (1px -> 2px)
3. Ensure all text meets WCAG AAA contrast ratio (7:1)
4. Remove decorative elements that reduce clarity (bevels, glows, shadows)

**Manual high contrast:**
- Add a "High Contrast" toggle in View menu
- When enabled, forces high contrast adjustments regardless of system setting

**Acceptance criteria:**
- System high contrast mode is detected on all platforms
- Theme auto-switches to high contrast
- All text meets WCAG AAA contrast ratio
- Decorative elements are simplified

### Task 25.3: Implement scalable UI

All UI elements must scale with user-configurable font size:

**Scale-aware dimensions:**
```cpp
auto scaled(int base_px) -> int {
    return static_cast<int>(base_px * font_scale_factor_);
}
```

**Scalable elements:**
- Chrome height: `scaled(40)`
- Toolbar height: `scaled(40)`
- Status bar height: `scaled(24)`
- Sidebar width: `scaled(256)`
- File tree row height: `scaled(28)`
- Icon sizes: `scaled(14)`, `scaled(16)`
- Font sizes: all font tokens scale proportionally
- Spacing and padding: scale with font size

**Zoom controls:**
- Ctrl++ / Cmd++: increase UI scale (10% increments)
- Ctrl+- / Cmd+-: decrease UI scale
- Ctrl+0 / Cmd+0: reset to 100%
- Scale range: 75% to 200%
- Scale factor persisted in config

**Acceptance criteria:**
- UI scales smoothly from 75% to 200%
- All elements scale proportionally (no clipping, no misalignment)
- Text remains readable at all scales
- Scale persists across sessions

### Task 25.4: Implement focus indicators

Every interactive element must have a visible focus indicator:

**Focus indicator design:**
- Border: 2px solid `accent_primary`
- Offset: -2px (inset) or 2px (outset)
- Visible only when using keyboard navigation (not on mouse click)
- Animated: subtle pulse or glow for clarity

**Detection of keyboard vs mouse focus:**
```cpp
// Track input mode
bool using_keyboard_ = false;

void OnKeyDown(wxKeyEvent& event) {
    using_keyboard_ = true;
    // Process key
}

void OnMouseDown(wxMouseEvent& event) {
    using_keyboard_ = false;
    // Process click
}
```

Show focus indicator only when `using_keyboard_` is true. This prevents distracting outlines during normal mouse use.

**Elements needing focus indicators:**
- Toolbar buttons
- View mode buttons
- Sidebar toggle button
- Window control buttons
- File tree items
- Editor panel
- Preview panel
- Theme gallery cards
- Import/export buttons
- Find bar input
- Status bar clickable items

**Acceptance criteria:**
- Focus indicators appear on keyboard navigation
- Focus indicators are hidden during mouse interaction
- All interactive elements have indicators
- Indicators use accent_primary color

### Task 25.5: Implement reduced motion support

Detect and respect user's reduced motion preference:

**Platform detection:**
- macOS: `NSWorkspace.shared.accessibilityDisplayShouldReduceMotion`
- Windows: `SystemParametersInfo(SPI_GETACCESSTIMEOUTTIMEOUT, ...)`
- Linux: Check `prefers-reduced-motion` via GTK settings

**When reduced motion is active:**
1. Disable sidebar toggle animation (instant show/hide)
2. Disable theme transition animation
3. Disable any hover animations
4. Reduce or eliminate scroll animations
5. Replace animated loading indicators with static ones

**Config override:**
- `accessibility.reduce_motion` setting (true/false/auto)
- "auto" follows system preference

**Acceptance criteria:**
- System reduced motion preference is detected
- All animations are disabled when active
- Application remains fully functional without animations
- Config override works

### Task 25.6: Implement screen reader announcements

Proactively announce important state changes to screen readers:

**Announcement events:**
| Event | Announcement |
|---|---|
| File opened | "Opened {filename}" |
| File saved | "File saved" |
| Theme changed | "Theme changed to {name}" |
| View mode changed | "Switched to {mode} view" |
| Sidebar toggled | "Sidebar {shown/hidden}" |
| Find result | "Found {N} matches" |
| Mermaid render error | "Mermaid diagram error" |
| Theme import success | "Theme {name} imported" |
| Theme import error | "Theme import failed: {reason}" |

**Implementation using wxAccessible::NotifyEvent:**
```cpp
void AnnounceToScreenReader(wxWindow* window, const wxString& message)
{
    // On Windows: use UIA notifications
    // On macOS: use NSAccessibility announcements
    // On Linux: use AT-SPI events
    wxAccessible::NotifyEvent(wxACC_EVENT_VALUE_CHANGE, window, wxOBJID_CLIENT, 0);
}
```

Or use a live region approach: create an off-screen text element that screen readers monitor for changes.

**Acceptance criteria:**
- State changes are announced to screen readers
- Announcements are concise and meaningful
- Announcements do not interrupt active reading

### Task 25.7: Validate all themes for accessibility

Run an accessibility audit on all 8 built-in themes:

**Checks:**
1. **Contrast ratio** (WCAG AA = 4.5:1 for normal text, 3:1 for large text):
   - text_main vs bg_app
   - text_main vs bg_panel
   - text_main vs bg_input
   - text_muted vs bg_app
   - accent_primary vs bg_app
   - accent_primary vs bg_panel

2. **Distinguishability:**
   - bg_app vs bg_panel (must be visually distinct)
   - border_light vs bg_app (border must be visible)
   - accent_primary vs accent_secondary (must be distinguishable)

3. **Color-blind safety:**
   - Information should not rely solely on color (use shapes/text too)
   - Test with deuteranopia, protanopia, tritanopia simulations

Document results in an accessibility audit report.

**Acceptance criteria:**
- All themes are audited for contrast
- "High Contrast Blue" meets WCAG AAA (7:1)
- At least 6 of 8 themes meet WCAG AA (4.5:1) for body text
- Themes that fail are documented with specific failures
- Audit report is written to docs

### Task 25.8: Write accessibility tests

Create `/Users/ryanrentfro/code/markamp/tests/unit/test_accessibility.cpp`:

Test cases:
1. All custom controls have accessibility implementations
2. Accessibility names are non-empty and meaningful
3. Accessibility roles are appropriate
4. Folder expand/collapse state changes are reported
5. File selection state changes are reported
6. Focus management: Tab cycles through zones
7. Keyboard navigation: all actions reachable without mouse
8. Theme contrast ratios meet WCAG AA
9. Scalable UI: elements at 200% scale do not overlap
10. Reduced motion: animations are disabled when preference is set

**Acceptance criteria:**
- All tests pass
- Accessibility coverage for all custom controls
- At least 15 test assertions

## Files Created/Modified

| File | Action |
|---|---|
| `src/ui/FileTreeCtrl.cpp` | Modified (accessibility) |
| `src/ui/Toolbar.cpp` | Modified (accessibility) |
| `src/ui/StatusBar.cpp` | Modified (accessibility) |
| `src/ui/CustomChrome.cpp` | Modified (accessibility) |
| `src/ui/ThemeGallery.cpp` | Modified (accessibility) |
| `src/ui/ThemeAwareWindow.h` | Modified (scale factor) |
| `src/platform/PlatformAbstraction.h` | Modified (system preferences) |
| `src/platform/MacPlatform.mm` | Modified (accessibility detection) |
| `src/platform/WinPlatform.cpp` | Modified (accessibility detection) |
| `src/platform/LinuxPlatform.cpp` | Modified (accessibility detection) |
| `src/CMakeLists.txt` | Modified |
| `tests/unit/test_accessibility.cpp` | Created |
| `docs/v1_docs/accessibility_audit.md` | Created |

## Dependencies

- wxWidgets 3.2.9 (wxAccessible)
- Platform accessibility APIs (MSAA/UIA, VoiceOver, AT-SPI)
- Phase 24 keyboard navigation (as foundation)
- Phase 08 ThemeEngine (for contrast calculations)

## Estimated Complexity

**Medium** -- wxWidgets provides basic accessibility infrastructure via wxAccessible. The main effort is in creating accessibility implementations for all custom controls and testing with actual screen readers. Platform-specific preference detection requires platform code.
