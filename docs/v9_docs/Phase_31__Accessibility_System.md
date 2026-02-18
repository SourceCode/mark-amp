# Phase 31: Accessibility System

## Overview
AccessibilityManager exists in core with accessibility infrastructure. Test files test_accessibility.cpp exist. However, accessibility features are not comprehensively wired: screen reader announcements, keyboard navigation, focus management, high contrast support, and reduced motion are incomplete across all surfaces. This phase builds a complete accessibility layer.

## Prerequisites
- Phase 06 (Workbench navigation for focus management)
- Phase 07 (Editor for accessible editing)
- Phase 03 (Theme engine for high contrast)

## Tasks

### Task 1: Wire AccessibilityManager Core Framework
**Files:** `src/core/AccessibilityManager.cpp`, `src/core/AccessibilityManager.h`
**Description:** AccessibilityManager exists. Wire it as the central accessibility coordinator: announce events to screen readers, manage focus order, detect system accessibility settings.
**Acceptance Criteria:**
- Screen reader announcement API: `announce(text, priority)`
- Priority levels: polite (queue) and assertive (interrupt)
- System setting detection: VoiceOver (macOS), NVDA/JAWS (Windows), Orca (Linux)
- Focus management: track focused element, restore focus
- `AccessibilityModeChangedEvent` emitted on system setting change

### Task 2: Wire Keyboard Navigation for All Surfaces
**Files:** `src/ui/MainFrame.cpp`, `src/core/ShortcutManager.cpp`
**Description:** Every UI element must be reachable via keyboard. Wire comprehensive Tab order across all panels and surfaces.
**Acceptance Criteria:**
- Tab cycles through all interactive elements
- Shift+Tab cycles in reverse
- Tab order follows visual layout (top-to-bottom, left-to-right)
- Focus visible: focus ring on all focused elements
- Skip navigation: jump to main content area
- Focus trap: modals trap focus within

### Task 3: Wire Screen Reader Announcements for Editor
**Files:** `src/ui/EditorPanel.cpp`, `src/core/AccessibilityManager.cpp`
**Description:** Editor announces: cursor position, selected text, formatting at cursor, document structure navigation.
**Acceptance Criteria:**
- Cursor move: announce line number and content
- Selection: announce "X characters selected" or selection content
- Formatting: announce "heading level 2", "bold text", "code block"
- Save: announce "document saved"
- Error: announce diagnostic message at cursor
- All announcements configurable (verbosity level)

### Task 4: Wire Screen Reader Announcements for Canvas
**Files:** `src/ui/CanvasWorkspacePanel.cpp`, `src/core/AccessibilityManager.cpp`
**Description:** Canvas announces: selected object type and label, tool changes, canvas navigation.
**Acceptance Criteria:**
- Object select: "Selected: Sticky Note titled 'Idea 1'"
- Tool change: "Tool changed to Select"
- Object create: "Created Text Box"
- Object delete: "Deleted 3 objects"
- Zoom: "Zoom level 150%"
- Canvas navigation: object count and viewport description

### Task 5: Wire Screen Reader for Graph View
**Files:** `src/ui/GraphViewPanel.cpp`, `src/core/AccessibilityManager.cpp`
**Description:** Graph view announces: selected node, connection count, navigation between nodes.
**Acceptance Criteria:**
- Node select: "Document: filename.md, 5 connections"
- Node navigation: Tab cycles through connected nodes
- Filter change: "Showing 42 of 100 nodes"
- Cluster: "Cluster: 8 documents"
- Search: "3 nodes matching 'term'"

### Task 6: Wire Focus Ring Styling
**Files:** `src/core/ThemeEngine.cpp`, `src/ui/ThemeAwareWindow.cpp`
**Description:** All focusable elements show a visible focus ring. Ring uses theme accent color with sufficient contrast.
**Acceptance Criteria:**
- Focus ring: 2px solid accent color
- Offset: 2px from element boundary
- Contrast: minimum 3:1 against background
- Custom focus ring for canvas objects
- No focus ring on mouse click (keyboard-only focus indicator)
- Focus ring color from `focus_ring` theme token

### Task 7: Wire High Contrast Mode
**Files:** `src/core/ThemeEngine.cpp`, `src/core/AccessibilityManager.cpp`
**Description:** Detect system high contrast mode. Override theme to increase contrast: minimum 4.5:1 for text, 3:1 for large text, borders on all elements.
**Acceptance Criteria:**
- System high contrast detected on all platforms
- Text contrast: minimum 4.5:1 ratio
- Large text: minimum 3:1 ratio
- Borders added to all interactive elements
- Icons: high contrast variants or outlines
- Focus ring: thicker (3px) and higher contrast

### Task 8: Wire Reduced Motion Support
**Files:** `src/core/AccessibilityManager.cpp`, `src/rendering/FxSafetyController.cpp`
**Description:** Detect system "Reduce Motion" setting. Disable all animations, transitions, and motion effects when active.
**Acceptance Criteria:**
- macOS: `NSWorkspace.accessibilityDisplayShouldReduceMotion`
- Windows: `SPI_GETCLIENTAREAANIMATION`
- Linux: `gtk-enable-animations`
- All FX effects disabled when reduce motion active
- Smooth scrolling replaced with instant scroll
- Panel transitions replaced with instant show/hide

### Task 9: Wire ARIA-Like Roles for Custom Controls
**Files:** `src/ui/ToolWindowHost.cpp`, `src/ui/TabBar.cpp`, `src/ui/ActivityBar.cpp`
**Description:** Custom-drawn controls (tool window tabs, tab bar, activity bar) expose role and state information to accessibility APIs.
**Acceptance Criteria:**
- Tab bar: role=tablist, each tab: role=tab, aria-selected
- Activity bar: role=toolbar, each item: role=button
- Tool window: role=tabpanel
- Tree view: role=tree, items: role=treeitem, aria-expanded
- File explorer: role=tree with proper nesting
- All states dynamic (update on interaction)

### Task 10: Wire Keyboard Shortcuts for Accessibility Actions
**Files:** `src/core/ShortcutManager.cpp`, `src/ui/MainFrame.cpp`
**Description:** Accessibility-specific shortcuts: increase/decrease font size, toggle high contrast, read current line, read selection, navigate headings.
**Acceptance Criteria:**
- Cmd+= / Cmd+-: increase/decrease font size
- Alt+H: navigate to next heading
- Alt+Shift+H: navigate to previous heading
- F6: cycle between major UI areas
- Ctrl+Alt+Space: read current line to screen reader
- All shortcuts documented in keyboard reference

### Task 11: Wire Color Blindness Support
**Files:** `src/core/ThemeEngine.cpp`, `src/core/AccessibilityManager.cpp`
**Description:** Color is never the only indicator. All color-coded information also has shape, text, or pattern indicator.
**Acceptance Criteria:**
- Git status: color + letter (M, A, D, U)
- Diagnostics: color + icon (error, warning, info)
- Task priority: color + label text
- Canvas objects: color + shape labels
- Search match: color + underline
- Review: color + position (star count or text label)

### Task 12: Wire Text Scaling Support
**Files:** `src/ui/LayoutManager.cpp`, `src/core/Config.h`
**Description:** UI text scales independently of editor text. System text size settings respected. All UI elements scale proportionally.
**Acceptance Criteria:**
- UI text size configurable: 80% to 200% of default
- System text size setting respected
- All panels resize proportionally
- Icons scale with text size
- Minimum touch target: 44x44px at any scale
- Text truncation with ellipsis (no overflow)

### Task 13: Wire Alternative Text for Images
**Files:** `src/ui/PreviewPanel.cpp`, `src/rendering/HtmlRenderer.cpp`
**Description:** Images in preview and canvas show alt text when: image fails to load, screen reader active, or on hover.
**Acceptance Criteria:**
- Markdown `![alt text](url)`: alt text shown on load failure
- Screen reader: announces alt text for all images
- Canvas images: alt text in accessibility tree
- Missing alt text: warning in diagnostics
- Placeholder for broken images includes descriptive text

### Task 14: Wire Accessible Table Navigation
**Files:** `src/ui/AVTablePanel.cpp`, `src/ui/TableEditorOverlay.cpp`
**Description:** Tables (Attribute View and Markdown tables) navigable with keyboard: Tab between cells, arrow keys within cell, announce row/column headers.
**Acceptance Criteria:**
- Tab moves to next cell
- Shift+Tab moves to previous cell
- Arrow keys navigate grid
- Header announced on column change
- Row number announced on row change
- Ctrl+Home: go to first cell

### Task 15: Wire Accessible Search
**Files:** `src/ui/SearchPanel.cpp`, `src/ui/CommandPalette.cpp`
**Description:** Search panel and command palette fully accessible: results count announced, current result announced, keyboard navigation through results.
**Acceptance Criteria:**
- Result count announced: "42 results found"
- Current result: "Result 3 of 42: filename.md line 15"
- Arrow keys navigate results with announcement
- Enter opens result
- Filter changes: new count announced
- Live region for updating results

### Task 16: Wire Status Bar Accessibility
**Files:** `src/ui/StatusBarPanel.cpp`, `src/core/AccessibilityManager.cpp`
**Description:** Status bar information accessible to screen readers. Important status changes announced.
**Acceptance Criteria:**
- All status bar items have accessible labels
- Git status announced on change
- Sync status announced on change
- Error count announced on change
- F6 cycle includes status bar
- Status bar items keyboard-activatable

### Task 17: Wire Accessible Notifications
**Files:** `src/ui/NotificationManager.cpp`, `src/core/AccessibilityManager.cpp`
**Description:** Notifications announced to screen reader. Actions accessible via keyboard.
**Acceptance Criteria:**
- Notification announced: "Notification: [title] - [message]"
- Action buttons keyboard-focusable
- Tab navigates between notification actions
- Escape dismisses notification
- Notification history accessible via command

### Task 18: Wire Accessibility Audit Tool
**Files:** `src/core/AccessibilityManager.cpp`, `src/ui/MainFrame.cpp`
**Description:** Built-in accessibility audit: "Accessibility: Run Audit" scans current view for issues: missing labels, low contrast, no keyboard access, missing alt text.
**Acceptance Criteria:**
- Audit scans current panel/view
- Issues categorized: critical, warning, info
- Results shown in Problems panel
- Each issue: element, issue, fix suggestion
- Audit runs automatically in debug mode
- Export audit results

### Task 19: Wire Accessibility Settings
**Files:** `src/ui/SettingsPanel.cpp`, `src/core/Config.h`
**Description:** Accessibility settings: screen reader mode, high contrast, reduced motion, text scale, focus ring style, announcement verbosity.
**Acceptance Criteria:**
- Screen reader mode: on/off/auto-detect
- High contrast: on/off/auto-detect
- Reduced motion: on/off/auto-detect
- Text scale: slider 80%-200%
- Announcement verbosity: minimal, standard, verbose
- Settings section: "Accessibility"

### Task 20: Add Accessibility Tests
**Files:** `tests/unit/test_accessibility.cpp`
**Description:** Test accessibility: screen reader announcements, keyboard navigation, focus management, high contrast compliance.
**Acceptance Criteria:**
- Screen reader announcements generated correctly
- Tab order covers all interactive elements
- Focus ring visible on all focusable elements
- High contrast ratios meet WCAG 2.1 AA
- Reduced motion disables all animations
- All custom controls have accessible roles

## Testing Requirements
- Keyboard navigation covers all UI elements
- Screen reader announcements for all interactions
- Color contrast meets WCAG 2.1 AA (4.5:1)
- Reduced motion disables all animation

## Phase Completion Criteria
- Screen reader support for all surfaces
- Full keyboard navigation
- High contrast mode
- Reduced motion support
- Focus management and focus rings
- Color-blind safe indicators
- All tests pass
