# Phase 05 -- Accessibility And Keyboard Navigation Foundation

## Objective

Build a comprehensive accessibility layer that ensures MarkAmp Studio is fully operable by keyboard-only users, screen reader users, and users with various visual or motor impairments. This phase upgrades the existing `FocusManager` and `AccessibilityModel` from data-only models into fully wired runtime systems with focus ring rendering, screen reader announcements, tab order management across all zones, ARIA-equivalent semantic roles, keyboard-only mode detection, and high contrast mode integration.

## Prerequisites

- Phase 01 (Design System Foundation) -- for `LayoutMetrics::min_hit_target()`, focus ring width, high contrast theme.
- Phase 03 (Theme Engine V2) -- for `ThemeColorToken::FocusRingColor`, high contrast mode support.
- Phase 04 (Animation Framework) -- for `FocusRingAnimator`, reduced-motion setting.

## Deliverables

- Upgraded `FocusManager` with full zone traversal and item-level focus.
- `ScreenReaderBridge` -- platform-specific screen reader announcement API.
- `AccessibilityController` -- top-level coordinator wiring all accessibility features.
- `KeyboardModeDetector` -- auto-detects keyboard-only usage.
- Focus ring rendering on all interactive controls.
- Skip-to-content navigation landmark.
- Focus trapping for modal dialogs and palettes.
- Catch2 test target: `test_accessibility`.

## Estimated Complexity

High -- requires platform-specific APIs (macOS Accessibility, Windows UI Automation) and integration across all controls.

---

## Tasks

### Task 1: Upgrade FocusManager to Support Item-Level Focus

**Description:** Extend the existing `FocusManager` singleton to track not just which zone has focus, but which specific item within that zone is focused. This enables fine-grained keyboard navigation within zones (e.g., which tab in the tab bar, which item in the activity bar).

**Key Implementation Details:**
- Add to `FocusManager`:
  - `void set_item_index(int index)` -- set focused item within current zone.
  - `auto item_index() const -> int` -- current item index (-1 = zone-level).
  - `void advance_item(FocusDirection dir)` -- move to next/prev item within zone.
  - `void set_item_count(FocusZoneId zone, int count)` -- set how many items a zone has.
- Zone-specific item counting:
  - `kActivityBar`: count of bar items.
  - `kSidebar`: count of visible tree/list items.
  - `kEditorArea`: 1 (the editor itself, or tab count for tab-level focus).
  - `kBottomPanel`: count of panel tabs.
  - `kStatusBar`: count of clickable status items.
  - `kBreadcrumb`: count of breadcrumb segments.
- `advance_item` wraps within zone bounds or delegates to zone-level `advance()` on overflow.

**Files Affected:**
- `src/ui/FocusManager.h` (modify)
- `src/ui/FocusManager.cpp` (modify)

**Acceptance Criteria:**
- Setting `item_index(2)` in the ActivityBar zone and querying returns 2.
- `advance_item(kForward)` at the last item moves to the next zone.
- Item counts are correctly maintained per zone.

**Dependencies:** None.

---

### Task 2: Implement Focus Ring Rendering in All Controls

**Description:** Wire the `FocusRingAnimator` (from Phase 04) to draw focus rings around the focused item in every interactive control. Each control reports its focused item's bounds to the focus ring renderer.

**Key Implementation Details:**
- Each control implements a method: `auto GetFocusedItemBounds() const -> std::optional<wxRect>`.
- Controls that need focus ring:
  - `ActivityBar`: rectangle around focused icon.
  - `TabBar`: rectangle around focused tab.
  - `Toolbar`: rectangle around focused button.
  - `StatusBarPanel`: rectangle around focused clickable item.
  - `BreadcrumbBar`: rectangle around focused segment.
  - `FileTreeCtrl`: rectangle around focused tree row.
  - `CommandPalette`: rectangle around focused result.
  - Bottom panel tabs.
- `FocusRingAnimator` is owned by the top-level `LayoutManager` and renders as an overlay.
- On focus change events, `LayoutManager` queries the active control for bounds and animates the ring.

**Files Affected:**
- `src/ui/ActivityBar.h` / `.cpp` (modify -- add GetFocusedItemBounds)
- `src/ui/TabBar.h` / `.cpp` (modify)
- `src/ui/Toolbar.h` / `.cpp` (modify)
- `src/ui/StatusBarPanel.h` / `.cpp` (modify)
- `src/ui/BreadcrumbBar.h` / `.cpp` (modify)
- `src/ui/FileTreeCtrl.h` / `.cpp` (modify)
- `src/ui/LayoutManager.cpp` (modify -- wire FocusRingAnimator)

**Acceptance Criteria:**
- Pressing Tab cycles focus through zones with a visible focus ring.
- Arrow keys within a zone move the focus ring between items.
- Focus ring smoothly animates between positions (Phase 04 dependency).

**Dependencies:** Phase 04 Task 19, Task 1.

---

### Task 3: Implement Keyboard-Only Mode Detector

**Description:** Detect when the user is navigating exclusively by keyboard (no mouse clicks) and activate keyboard-only mode, which shows focus rings and other keyboard affordances. Hide focus rings when the mouse is used for interaction.

**Key Implementation Details:**
- Class: `KeyboardModeDetector` in `src/ui/KeyboardModeDetector.h`
- Listens to all input events at the `MainFrame` level.
- Rules:
  - Any keyboard event (Tab, arrow, Enter, Escape) sets `keyboard_mode = true`.
  - Any mouse click or mouse move sets `keyboard_mode = false`.
  - Mouse wheel does NOT exit keyboard mode (user may scroll while keyboard navigating).
- Publishes `KeyboardModeChanged { bool keyboard_mode }` via EventBus.
- Focus rings are only rendered in keyboard mode (mouse users do not need them).
- Add a forced-on mode for accessibility: "Always Show Focus Ring" setting.

**Files Affected:**
- `src/ui/KeyboardModeDetector.h` (new)
- `src/ui/KeyboardModeDetector.cpp` (new)
- `src/core/Events.h` (modify -- add event)
- `src/ui/MainFrame.cpp` (modify -- install detector)

**Acceptance Criteria:**
- Focus ring appears when user presses Tab.
- Focus ring hides when user clicks the mouse.
- "Always Show Focus Ring" setting overrides the auto-detection.

**Dependencies:** Task 2.

---

### Task 4: Implement Screen Reader Bridge -- macOS

**Description:** Create a platform-specific bridge to macOS VoiceOver that announces focus changes, state changes, and live region updates.

**Key Implementation Details:**
- Class: `MacScreenReaderBridge` in `src/platform/MacScreenReaderBridge.h` (Objective-C++)
- Implements `IScreenReaderBridge` interface.
- Method: `void announce(const std::string& message, bool assertive)` -- uses `NSAccessibilityPostNotification` with `NSAccessibilityAnnouncementRequestedNotification`.
- Method: `void set_role(wxWindow* window, ControlRole role)` -- maps `ControlRole` to `NSAccessibilityRole` values.
- Method: `void set_label(wxWindow* window, const std::string& label)` -- sets `NSAccessibilityTitleAttribute`.
- Method: `void set_value(wxWindow* window, const std::string& value)` -- sets `NSAccessibilityValueAttribute`.
- Method: `void set_expanded(wxWindow* window, bool expanded)`.
- Method: `void notify_focus_change(wxWindow* new_focus)`.
- Mapping: `ControlRole::kButton` -> `NSAccessibilityButtonRole`, `kTreeItem` -> `NSAccessibilityRowRole`, etc.

**Files Affected:**
- `src/platform/IScreenReaderBridge.h` (new -- interface)
- `src/platform/MacScreenReaderBridge.h` (new)
- `src/platform/MacScreenReaderBridge.mm` (new -- Objective-C++)

**Acceptance Criteria:**
- VoiceOver reads the announced text.
- Controls are identified by their role when focused.
- Focus change triggers VoiceOver focus announcement.

**Dependencies:** None.

---

### Task 5: Implement Screen Reader Bridge -- Windows (Stub)

**Description:** Create a stub implementation for Windows UI Automation. Full implementation deferred; this ensures the interface compiles on all platforms.

**Key Implementation Details:**
- Class: `WinScreenReaderBridge` in `src/platform/WinScreenReaderBridge.h`
- Implements `IScreenReaderBridge`.
- All methods are no-ops with `MARKAMP_LOG_DEBUG("WinScreenReaderBridge: not implemented")`.
- Future: will use Windows UI Automation Provider API (`IRawElementProviderSimple`).

**Files Affected:**
- `src/platform/WinScreenReaderBridge.h` (new)
- `src/platform/WinScreenReaderBridge.cpp` (new)

**Acceptance Criteria:**
- Compiles on Windows.
- No crashes when methods are called.
- Log messages indicate not-yet-implemented.

**Dependencies:** Task 4 (interface definition).

---

### Task 6: Implement Screen Reader Bridge -- Linux (Stub)

**Description:** Stub implementation for Linux AT-SPI. Same pattern as Windows stub.

**Key Implementation Details:**
- Class: `LinuxScreenReaderBridge` in `src/platform/LinuxScreenReaderBridge.h`
- All methods are no-ops.
- Future: will use AT-SPI2 D-Bus API.

**Files Affected:**
- `src/platform/LinuxScreenReaderBridge.h` (new)
- `src/platform/LinuxScreenReaderBridge.cpp` (new)

**Acceptance Criteria:**
- Compiles on Linux.
- No crashes.

**Dependencies:** Task 4 (interface definition).

---

### Task 7: Create the AccessibilityController

**Description:** Build the top-level coordinator that wires all accessibility features together. It listens to focus change events, state change events, and delegates to the screen reader bridge and focus ring animator.

**Key Implementation Details:**
- Class: `AccessibilityController` in `src/ui/AccessibilityController.h`
- Constructor: `AccessibilityController(core::EventBus& bus, FocusManager& fm, IScreenReaderBridge& sr, AccessibilityModel& model)`.
- Subscribes to:
  - `FocusZoneChanged` -- announces zone label to screen reader.
  - `FocusItemChanged` -- announces item label and role.
  - `SidebarModeChanged` -- announces new sidebar panel name.
  - `TabSwitched` -- announces tab file name.
  - `NotificationShown` -- announces notification message (assertive for errors).
  - `ContentChanged` -- debounced announcement of word count changes.
- Method: `void register_control(const std::string& id, wxWindow* window, ControlRole role, const std::string& label)`.
- Method: `void update_state(const std::string& id, const std::string& state)` -- e.g., "expanded", "selected", "modified".

**Files Affected:**
- `src/ui/AccessibilityController.h` (new)
- `src/ui/AccessibilityController.cpp` (new)

**Acceptance Criteria:**
- Switching zones announces the zone name via screen reader.
- Tab switch announces the file name.
- Error notifications are announced assertively.

**Dependencies:** Tasks 1, 4.

---

### Task 8: Implement Tab Order Registration

**Description:** Each control registers its focusable items with the `FocusManager` so that Tab traversal covers every interactive element in the correct order.

**Key Implementation Details:**
- Zone order (matching VSCode): ActivityBar -> Sidebar -> EditorArea (tabs, then editor) -> BottomPanel -> StatusBar.
- Within each zone:
  - ActivityBar: top-to-bottom item order.
  - Sidebar: panel header actions, then tree/list items top-to-bottom.
  - EditorArea: tab bar tabs left-to-right, then breadcrumb segments, then editor itself.
  - BottomPanel: panel tabs left-to-right, then panel content.
  - StatusBar: left items then right items.
- Each control calls `FocusManager::set_item_count(zone, count)` when its item count changes.
- Dynamically updated when sidebar changes mode, tabs are added/removed, etc.

**Files Affected:**
- `src/ui/ActivityBar.cpp` (modify -- register item count on CreateItems)
- `src/ui/TabBar.cpp` (modify -- update count on AddTab/RemoveTab)
- `src/ui/StatusBarPanel.cpp` (modify -- register items)
- `src/ui/BreadcrumbBar.cpp` (modify)
- `src/ui/LayoutManager.cpp` (modify -- update zone visibility on panel show/hide)

**Acceptance Criteria:**
- Pressing Tab repeatedly visits: ActivityBar -> Sidebar header -> File tree -> Tab bar -> Breadcrumb -> Editor -> Bottom panel tabs -> Status bar -> back to ActivityBar.
- Hidden zones (e.g., hidden sidebar) are skipped.

**Dependencies:** Task 1.

---

### Task 9: Implement Focus Trapping for Modals

**Description:** When a modal dialog or overlay (Command Palette, theme picker, first-run wizard) is open, trap focus within it so Tab/Shift+Tab cycles only through the modal's controls.

**Key Implementation Details:**
- In `FocusManager`:
  - `void push_trap(FocusZoneId zone)` -- enters focus trap mode for the specified zone.
  - `void pop_trap()` -- exits focus trap, restores previous focus.
- When a trap is active, `advance()` wraps within the trap zone only.
- `CommandPalette`: on show, `push_trap(kCommandPalette)` (add new zone ID).
- On close: `pop_trap()` restores focus to where it was before.
- `ShortcutOverlay`, `FirstRunWizard`, and any future dialogs use the same pattern.
- Pressing Escape always exits the trap and closes the modal.

**Files Affected:**
- `src/ui/FocusManager.h` (modify -- add push_trap/pop_trap, add kCommandPalette zone)
- `src/ui/FocusManager.cpp` (modify)
- `src/ui/CommandPalette.cpp` (modify -- push/pop trap on show/hide)

**Acceptance Criteria:**
- With Command Palette open, Tab cycles only through palette items.
- Closing the palette restores focus to the previously focused element.
- Nested traps work (e.g., palette inside a dialog).

**Dependencies:** Task 1.

---

### Task 10: Implement Skip-to-Content Landmark

**Description:** Add a "Skip to Content" mechanism that allows keyboard users to jump directly to the editor area, bypassing the activity bar, sidebar, and toolbar. This is the web equivalent of a skip navigation link.

**Key Implementation Details:**
- When the user presses `Ctrl+F6` (VS Code convention) or a configurable shortcut:
  - Focus jumps directly to `FocusZoneId::kEditorArea`.
  - Screen reader announces "Editor area focused".
- When focus is in the editor and the user presses `Escape`:
  - Focus returns to the previously focused zone (using `FocusManager::restore()`).
- Also add `Ctrl+0`: focus sidebar, `Ctrl+Shift+E`: focus explorer, matching VS Code shortcuts.

**Files Affected:**
- `src/ui/MainFrame.cpp` (modify -- register keyboard shortcuts)
- `src/core/ShortcutManager.cpp` (modify -- add shortcuts)

**Acceptance Criteria:**
- Ctrl+F6 jumps focus to the editor from any zone.
- Screen reader announces the zone transition.
- Escape from editor returns focus to the previous zone.

**Dependencies:** Tasks 1, 7.

---

### Task 11: Annotate ActivityBar with Accessibility Semantics

**Description:** Add ARIA-equivalent semantic annotations to ActivityBar items so screen readers can identify each item's role, label, and state.

**Key Implementation Details:**
- In `ActivityBar::CreateItems()`, register each item with `AccessibilityController`:
  - Role: `ControlRole::kTab` (activity bar items behave like tabs).
  - Label: item label text (e.g., "Explorer", "Search", "Extensions").
  - State: "selected" for active item, "badge: N" if badge count > 0.
- On active item change, update the state announcement.
- On badge change, announce "N notifications on Extensions" (polite, not assertive).

**Files Affected:**
- `src/ui/ActivityBar.cpp` (modify -- register with AccessibilityController)

**Acceptance Criteria:**
- VoiceOver reads "Explorer, tab, selected" when the Explorer item is focused.
- Changing active item announces the new selection.
- Badge count is included in the item description.

**Dependencies:** Tasks 7, 8.

---

### Task 12: Annotate TabBar with Accessibility Semantics

**Description:** Add accessibility annotations to TabBar tabs.

**Key Implementation Details:**
- Each tab: role `kTab`, label = display_name, state = "modified" if modified, "pinned" if pinned.
- Tab list container: role `kToolbar` (tab strip acts as a toolbar of tabs).
- Close button on each tab: role `kButton`, label "Close [filename]".
- On tab switch: announce "Switched to [filename]".

**Files Affected:**
- `src/ui/TabBar.cpp` (modify)

**Acceptance Criteria:**
- Focusing a tab reads its filename, modified state, and pinned state.
- Close button is independently focusable and labeled.

**Dependencies:** Tasks 7, 8.

---

### Task 13: Annotate FileTreeCtrl with Accessibility Semantics

**Description:** Add tree navigation semantics to the file tree.

**Key Implementation Details:**
- Each tree item: role `kTreeItem`, label = filename.
- State: "expanded" or "collapsed" for folders, "selected" for selected item.
- Arrow keys: Up/Down navigate items, Left collapses, Right expands (matches `ArrowKeyBehavior::kVerticalList`).
- Screen reader announces level/depth for nested items.
- On expand/collapse: announce "Expanded [folder name]" or "Collapsed [folder name]".

**Files Affected:**
- `src/ui/FileTreeCtrl.cpp` (modify)

**Acceptance Criteria:**
- Tree items announce their name, role, and expand state.
- Arrow key navigation works correctly with focus management.
- Nested items announce their depth level.

**Dependencies:** Tasks 7, 8.

---

### Task 14: Annotate StatusBarPanel with Accessibility Semantics

**Description:** Add accessibility annotations to status bar segments.

**Key Implementation Details:**
- Status bar container: role `kStatus`.
- Each clickable item: role `kButton`, label = descriptive text (e.g., "Line 42, Column 7", "Git branch: main").
- Non-clickable items: role `kStatus`, label = display text.
- On stat changes (word count, cursor position): announce via polite live region (debounced, no more than once per 2 seconds).

**Files Affected:**
- `src/ui/StatusBarPanel.cpp` (modify)

**Acceptance Criteria:**
- Focusing a clickable status item reads its label and "button" role.
- Cursor position changes are debounce-announced.

**Dependencies:** Tasks 7, 8.

---

### Task 15: Annotate CommandPalette with Accessibility Semantics

**Description:** Add combobox/listbox accessibility semantics to the Command Palette.

**Key Implementation Details:**
- Input field: role `kTextInput`, label "Command Palette".
- Results list: role `kListItem` for each result.
- Announce result count on filter change: "5 results" (polite).
- Announce selected result label on arrow key navigation.
- On execute: announce "Executed: [command name]".

**Files Affected:**
- `src/ui/CommandPalette.cpp` (modify)

**Acceptance Criteria:**
- VoiceOver identifies the palette as a search input with results list.
- Arrow key navigation reads each result.
- Result count is announced on filter.

**Dependencies:** Tasks 7, 9.

---

### Task 16: Annotate BreadcrumbBar with Accessibility Semantics

**Description:** Add navigation semantics to breadcrumb segments.

**Key Implementation Details:**
- Container: role `kToolbar`.
- Each segment: role `kButton`, label = segment text.
- Separator chevrons: hidden from accessibility (decorative).
- On segment click: announce navigation target.

**Files Affected:**
- `src/ui/BreadcrumbBar.cpp` (modify)

**Acceptance Criteria:**
- Focusing a breadcrumb segment reads its text as a button.
- Chevrons are not announced by screen readers.

**Dependencies:** Tasks 7, 8.

---

### Task 17: Annotate Bottom Panel Tabs with Accessibility Semantics

**Description:** Add tab semantics to the bottom panel tab bar (Output, Problems, Walkthrough).

**Key Implementation Details:**
- Tab strip: role `kToolbar`.
- Each tab: role `kTab`, label = panel name.
- Badge on Problems tab: state includes error/warning count.
- Active tab: state "selected".
- On switch: announce "Switched to Problems panel, 3 errors".

**Files Affected:**
- `src/ui/LayoutManager.cpp` (modify -- annotate bottom panel notebook tabs)

**Acceptance Criteria:**
- Tab focus reads panel name and active state.
- Error count on Problems tab is announced.

**Dependencies:** Tasks 7, 8.

---

### Task 18: Implement High Contrast Mode Toggle

**Description:** Wire the existing `AccessibilityModel::high_contrast()` flag to the theme system, activating the high-contrast theme and adjusting all UI elements for maximum visibility.

**Key Implementation Details:**
- Command Palette: "View: Toggle High Contrast Mode".
- When enabled:
  1. Switch to the high-contrast theme (Phase 03).
  2. Set `AccessibilityModel::set_high_contrast(true)`.
  3. Increase all border widths to 2px.
  4. Show focus rings always (override keyboard-mode detection).
  5. Disable all animations (reduced-motion = true).
  6. Increase minimum contrast ratio requirement to 7:1 (AAA).
- Persist setting in Config under `accessibility.high_contrast`.
- Restore on app restart.

**Files Affected:**
- `src/ui/MainFrame.cpp` (modify -- register command)
- `src/ui/AccessibilityController.cpp` (modify -- wire high contrast toggling)

**Acceptance Criteria:**
- Toggling high contrast immediately switches to the high-contrast theme.
- Focus rings are visible at all times in high contrast mode.
- All text meets AAA contrast requirements.

**Dependencies:** Tasks 3, 7; Phase 03 Task 9.

---

### Task 19: Implement Reduced Complexity Mode

**Description:** Wire the existing `AccessibilityModel::reduced_complexity()` flag to simplify the UI for users who are overwhelmed by information-dense interfaces.

**Key Implementation Details:**
- Command Palette: "View: Toggle Reduced Complexity Mode".
- When enabled:
  1. Hide activity bar badges.
  2. Hide status bar segments beyond Line/Col and filename.
  3. Collapse bottom panel by default.
  4. Hide minimap.
  5. Reduce sidebar to single-panel mode (no panel switching).
  6. Larger font sizes (comfortable density forced).
- Persist in Config under `accessibility.reduced_complexity`.

**Files Affected:**
- `src/ui/MainFrame.cpp` (modify)
- `src/ui/AccessibilityController.cpp` (modify)
- `src/ui/LayoutManager.cpp` (modify -- respond to reduced complexity events)

**Acceptance Criteria:**
- Enabling reduced complexity hides non-essential UI elements.
- Density is forced to comfortable.
- Disabling restores the previous state.

**Dependencies:** Task 7.

---

### Task 20: Keyboard Shortcut for All Panel Operations

**Description:** Ensure every panel operation (show/hide/focus) has a keyboard shortcut, matching VS Code conventions where applicable.

**Key Implementation Details:**
- Shortcuts to add/verify:
  - `Ctrl+B` -- toggle sidebar.
  - `Ctrl+J` -- toggle bottom panel.
  - `Ctrl+Shift+E` -- focus explorer.
  - `Ctrl+Shift+F` -- focus search.
  - `Ctrl+Shift+G` -- focus source control (future).
  - `Ctrl+Shift+D` -- focus run/debug (future).
  - `Ctrl+Shift+X` -- focus extensions.
  - `` Ctrl+` `` -- toggle terminal (future).
  - `Ctrl+Shift+M` -- focus problems panel.
  - `Ctrl+Shift+U` -- focus output panel.
  - `Ctrl+0` -- focus sidebar.
  - `Ctrl+1` through `Ctrl+9` -- focus editor group N.
  - `Ctrl+K Ctrl+S` -- open keyboard shortcuts editor.
- Register all in `MainFrame::RegisterDefaultShortcuts()`.

**Files Affected:**
- `src/ui/MainFrame.cpp` (modify)
- `src/core/ShortcutManager.cpp` (modify)

**Acceptance Criteria:**
- All listed shortcuts work and focus the correct zone/panel.
- Shortcuts are discoverable via Command Palette.
- No shortcut conflicts with existing bindings.

**Dependencies:** Tasks 1, 8.

---

### Task 21: Announce Region for Live Updates

**Description:** Create an invisible "announce region" that the screen reader monitors for changes. Content changes in this region are automatically read aloud, following the ARIA live region pattern.

**Key Implementation Details:**
- Class: `AnnounceRegion` in `src/ui/AnnounceRegion.h`
- On macOS: creates a hidden `NSAccessibilityElement` with `NSAccessibilityLiveRegionChangedNotification`.
- Method: `void set_polite(const std::string& text)` -- queued announcement (does not interrupt current speech).
- Method: `void set_assertive(const std::string& text)` -- interrupts current speech.
- Debouncing: polite announcements are debounced at 1 second minimum interval.
- Used for: word count changes, cursor position changes, search result counts, build status.

**Files Affected:**
- `src/ui/AnnounceRegion.h` (new)
- `src/ui/AnnounceRegion.cpp` (new)
- `src/platform/MacScreenReaderBridge.mm` (modify -- implement live region)

**Acceptance Criteria:**
- Polite announcements are read after current speech finishes.
- Assertive announcements interrupt.
- Debouncing prevents rapid-fire announcements.

**Dependencies:** Task 4.

---

### Task 22: Minimum Hit Target Enforcement

**Description:** Ensure all interactive elements meet the minimum 24x24px hit target (WCAG 2.5.5 Level AAA target size). Enforce this programmatically so controls cannot be rendered too small even in compact density.

**Key Implementation Details:**
- In `ComponentSizeResolver`, all `ComponentMetrics::height` values have a floor of 24px.
- In `LayoutMetrics::min_hit_target()`, return 24 regardless of density.
- Add a debug overlay (toggleable via "Developer: Show Hit Targets") that draws red borders around any element smaller than 24x24px.
- Controls that currently violate: close buttons on tabs (14px), breadcrumb separators (if clickable).
- Fix: increase touch targets for close buttons to 24px while keeping visual size at 14px (invisible padding).

**Files Affected:**
- `src/ui/ComponentSizeResolver.cpp` (modify -- enforce minimum)
- `src/ui/TabBar.cpp` (modify -- increase close button hit target)
- `src/ui/MainFrame.cpp` (modify -- register debug overlay command)

**Acceptance Criteria:**
- No interactive element has a hit target smaller than 24x24px.
- Close buttons on tabs have a 24px hit target despite 14px visual size.
- Debug overlay correctly identifies undersized targets.

**Dependencies:** Phase 01 Task 6.

---

### Task 23: Color Token Accessibility Metadata

**Description:** Annotate each foreground color token with its expected background token, enabling automated contrast ratio checking at runtime.

**Key Implementation Details:**
- Struct: `TokenPair { std::string fg_token; std::string bg_token; std::string context; }`.
- Registry: `std::vector<TokenPair>` defined in `src/ui/AccessibilityTokenPairs.h`.
- Pairs include all text-on-background combinations across all surfaces.
- `AccessibilityController` runs `ContrastValidator` on startup and after every theme change.
- Violations are logged at WARN level.
- In high-contrast mode, violations are logged at ERROR level.

**Files Affected:**
- `src/ui/AccessibilityTokenPairs.h` (new)
- `src/ui/AccessibilityController.cpp` (modify -- validate on theme change)

**Acceptance Criteria:**
- On startup with a compliant theme, no warnings.
- On a non-compliant theme, specific violations are logged.
- Token pairs cover all ~30 known fg/bg combinations.

**Dependencies:** Phase 03 Task 6.

---

### Task 24: Accessibility Audit Command

**Description:** Create a developer command that runs a comprehensive accessibility audit of the current UI state and produces a report.

**Key Implementation Details:**
- Command Palette: "Developer: Run Accessibility Audit".
- Audit checks:
  1. All focusable controls have accessible names (from `AccessibilityModel::unnamed_controls()`).
  2. All fg/bg token pairs meet WCAG AA contrast.
  3. All interactive elements meet 24x24px minimum hit target.
  4. Focus order covers all visible zones.
  5. No orphaned focus traps (trap stack is empty).
  6. Keyboard shortcuts do not conflict.
- Output: a panel or notification showing pass/fail for each check with details.
- Structured output for CI integration (JSON format to stdout when run with `--a11y-audit` flag).

**Files Affected:**
- `src/ui/AccessibilityAudit.h` (modify or new -- may extend existing `src/core/AccessibilityAudit.h`)
- `src/ui/AccessibilityAudit.cpp` (new)
- `src/ui/MainFrame.cpp` (modify -- register command)

**Acceptance Criteria:**
- Audit runs in under 1 second.
- Correctly identifies missing accessible names.
- Correctly identifies contrast violations.
- JSON output is parseable.

**Dependencies:** Tasks 7, 22, 23.

---

### Task 25: Catch2 Test Suite for Accessibility

**Description:** Write comprehensive unit tests for all accessibility components.

**Key Implementation Details:**
- Test file: `tests/unit/test_accessibility.cpp`
- Test target: `test_accessibility` in CMakeLists.txt
- Test sections:
  - `FocusManager`: zone traversal, item-level focus, push/pop trap, snapshot restore.
  - `KeyboardModeDetector`: keyboard events enable mode, mouse clicks disable.
  - `AccessibilityController`: event subscriptions produce correct announcements.
  - `AccessibilityModel`: unnamed controls detection, high contrast toggle.
  - Tab order: verify zone traversal visits all enabled zones.
  - Hit target: verify minimum sizes at all densities.
  - Token pairs: verify all pairs are defined.
- Use mock `IScreenReaderBridge` for testing announcements.

**Files Affected:**
- `tests/unit/test_accessibility.cpp` (new)
- `CMakeLists.txt` (modify)

**Acceptance Criteria:**
- All tests pass.
- At least 40 test assertions.
- Mock screen reader captures all expected announcements.

**Dependencies:** All previous tasks.

---

## Dependency Graph

```
Task 1 (FocusManager upgrade) ────────────────┐
Task 2 (Focus ring rendering) ── 1, Phase 04 ──┤
Task 3 (Keyboard mode detector) ── 2 ──────────┤
Task 4 (macOS ScreenReaderBridge) ──────────────┤
Task 5 (Windows stub) ── 4 ────────────────────┤
Task 6 (Linux stub) ── 4 ─────────────────────┤
Task 7 (AccessibilityController) ── 1, 4 ──────┤
Task 8 (Tab order registration) ── 1 ──────────┤
Task 9 (Focus trapping) ── 1 ─────────────────┤
Task 10 (Skip-to-content) ── 1, 7 ─────────────┤
Task 11-17 (Control annotations) ── 7, 8 ──────┤
Task 18 (High contrast) ── 3, 7 ───────────────┤
Task 19 (Reduced complexity) ── 7 ─────────────┤
Task 20 (Keyboard shortcuts) ── 1, 8 ──────────┤
Task 21 (Announce region) ── 4 ────────────────┤
Task 22 (Hit target enforcement) ── Phase 01 ──┤
Task 23 (Token pair metadata) ── Phase 03 ─────┤
Task 24 (Audit command) ── 7, 22, 23 ──────────┤
Task 25 (Tests) ── all ────────────────────────┘
```

## Files Created

| File | Type |
|------|------|
| `src/ui/KeyboardModeDetector.h` / `.cpp` | Keyboard detection |
| `src/platform/IScreenReaderBridge.h` | Interface |
| `src/platform/MacScreenReaderBridge.h` / `.mm` | macOS bridge |
| `src/platform/WinScreenReaderBridge.h` / `.cpp` | Windows stub |
| `src/platform/LinuxScreenReaderBridge.h` / `.cpp` | Linux stub |
| `src/ui/AccessibilityController.h` / `.cpp` | Coordinator |
| `src/ui/AnnounceRegion.h` / `.cpp` | Live region |
| `src/ui/AccessibilityTokenPairs.h` | Token pair registry |
| `src/ui/AccessibilityAudit.cpp` | Audit command |
| `tests/unit/test_accessibility.cpp` | Test file |

## Files Modified

| File | Change |
|------|--------|
| `src/ui/FocusManager.h` / `.cpp` | Item-level focus, trapping |
| `src/ui/ActivityBar.h` / `.cpp` | Focus bounds, a11y annotations |
| `src/ui/TabBar.h` / `.cpp` | Focus bounds, a11y annotations |
| `src/ui/Toolbar.h` / `.cpp` | Focus bounds |
| `src/ui/StatusBarPanel.h` / `.cpp` | Focus bounds, a11y annotations |
| `src/ui/BreadcrumbBar.h` / `.cpp` | Focus bounds, a11y annotations |
| `src/ui/FileTreeCtrl.h` / `.cpp` | Tree a11y, focus |
| `src/ui/CommandPalette.cpp` | Focus trapping, a11y |
| `src/ui/LayoutManager.cpp` | Focus ring, zone management |
| `src/ui/MainFrame.cpp` | Shortcuts, commands |
| `src/core/Events.h` | KeyboardModeChanged event |
| `src/core/ShortcutManager.cpp` | New shortcuts |
| `src/ui/ComponentSizeResolver.cpp` | Min hit target enforcement |
| `CMakeLists.txt` | New sources, test target |
