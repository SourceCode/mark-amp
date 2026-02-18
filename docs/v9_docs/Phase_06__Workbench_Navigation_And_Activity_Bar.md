# Phase 06: Workbench Navigation and Activity Bar

## Overview
ActivityBar exists (318 lines) but is not mounted into the main workbench flow. Sidebar mode switching is limited to Explorer/Extensions. The product vision requires VSCode-like activity bar with Miro-style mode switching across Explorer, Search, Notebooks, Canvas, Graph, and Extensions. This phase establishes the canonical navigation model.

## Prerequisites
- Phase 01 (EventBus hardening)
- Phase 03 (Theme engine for activity bar theming)

## Tasks

### Task 1: Mount ActivityBar as Permanent Left Rail
**Files:** `src/ui/LayoutManager.h`, `src/ui/LayoutManager.cpp`, `src/ui/ActivityBar.cpp`, `src/ui/ActivityBar.h`
**Description:** ActivityBar currently exists but is not part of the main layout. Mount it as a fixed-width (48px) left rail outside the sidebar content area. It should be visible in all modes, always the primary mode switch mechanism.
**Acceptance Criteria:**
- ActivityBar rendered as leftmost column in main layout
- 48px wide, full height from below custom chrome to above status bar
- Visible in all modes (editor, canvas, graph, etc.)
- Theme-aware: background, icon colors, hover/active states from tokens

### Task 2: Expand SidebarMode to Full Set
**Files:** `src/ui/LayoutManager.h`
**Description:** Expand `SidebarMode` enum to include: Explorer, Search, Notebooks, Canvas, Graph, Extensions, Settings. Ensure two-way sync: rail selection, menu commands, and internal mode changes always reflect one canonical active mode.
**Acceptance Criteria:**
- 7 modes defined in enum
- Single source of truth for active mode in LayoutManager
- Mode changes from any source (rail, menu, command, event) converge to same state
- `WorkbenchModeChangedEvent` emitted on every mode change

### Task 3: Implement Lazy Panel Registry
**Files:** `src/ui/LayoutManager.cpp`, `src/ui/LayoutManager.h`
**Description:** Replace one-off panel show/hide logic with a registry-based sidebar host. Panels are lazily created on first access, preserved when hidden, and restored when their mode is reactivated.
**Acceptance Criteria:**
- Each sidebar mode has a registered panel factory
- Panel created on first mode activation
- Switching away hides panel without destroying it
- Switching back restores panel state (scroll position, selection)

### Task 4: Replace Emoji Activity Icons with Glyph Pipeline
**Files:** `src/ui/ActivityBar.cpp`
**Description:** Activity bar currently uses emoji for icons. Replace with a consistent glyph/icon system (either drawn icons using wxDC or an icon font). Icons should be theme-aware with hover/active/pressed states.
**Acceptance Criteria:**
- Each mode has a distinct recognizable icon (not emoji)
- Icons use theme foreground color
- Hover: subtle background highlight
- Active: accent color with indicator bar
- Pressed: slight scale or depth effect

### Task 5: Add Keyboard Focus and Navigation
**Files:** `src/ui/ActivityBar.cpp`
**Description:** Activity bar items must be keyboard-navigable: focus with Tab, arrow keys to move between items, Enter/Space to activate. Add visible focus indicators.
**Acceptance Criteria:**
- Tab focuses the activity bar
- Up/Down arrows move between items
- Enter/Space activates the focused item
- Focus ring visible using theme focus_ring token
- Screen reader accessible labels

### Task 6: Add Badge System for Activity Bar Items
**Files:** `src/ui/ActivityBar.cpp`, `src/ui/ActivityBar.h`
**Description:** Activity bar items should support notification badges (e.g., search results count, extension updates count, problems count). Badges are small numbers or dots displayed on the icon.
**Acceptance Criteria:**
- Badge API: `ActivityBar::set_badge(item, count)` and `clear_badge(item)`
- Badge rendered as small circle with count (or dot for boolean)
- Badge color from theme `badge_bg` / `badge_fg` tokens
- Badges update in real-time as events occur

### Task 7: Wire Mode-Specific Navigation Events
**Files:** `src/core/Events.h`, `src/ui/LayoutManager.cpp`
**Description:** Add canonical show-mode events: `ShowSearchRequestEvent`, `ShowNotebooksRequestEvent`, `ShowCanvasRequestEvent`, `ShowGraphRequestEvent`. Route all mode activation through one event path in LayoutManager.
**Acceptance Criteria:**
- Each mode has a dedicated show-request event
- LayoutManager subscribes to all show-request events
- All mode activation paths (activity bar, menu, command palette) emit the same events
- Duplicate mode activation (already active) is a no-op

### Task 8: Persist Active Mode and Panel States
**Files:** `src/ui/LayoutManager.cpp`, `src/core/Config.cpp`
**Description:** Save the last active sidebar mode and per-panel state (scroll, selection) to config. On startup, restore the last active mode.
**Acceptance Criteria:**
- Last active mode saved on mode change
- Startup restores last mode (default: Explorer if none saved)
- Panel-specific state (tree expansion, scroll position) saved per mode
- Config key: `workbench.sidebar.lastMode`

### Task 9: Add Mode-Contextual Toolbar Actions
**Files:** `src/ui/Toolbar.cpp`, `src/ui/MainFrame.cpp`
**Description:** The toolbar should show different quick-action buttons depending on the active mode. Explorer shows file actions, Canvas shows canvas tools, Notebook shows cell actions, etc.
**Acceptance Criteria:**
- Toolbar actions change when mode switches
- Explorer mode: New File, New Folder, Refresh, Collapse All
- Canvas mode: New Board, Zoom to Fit, Center Selection
- Notebook mode: Add Cell, Run Cell, Run All
- Graph mode: Zoom to Fit, Filter, Refresh

### Task 10: Add Secondary Sidebar Support
**Files:** `src/ui/LayoutManager.h`, `src/ui/LayoutManager.cpp`
**Description:** Add support for a right-side secondary sidebar (like VSCode's secondary sidebar). This can host properties panels, outline views, or inspector panels.
**Acceptance Criteria:**
- Secondary sidebar can be toggled via command or toolbar button
- Independent from primary sidebar mode
- Can host any registered panel
- Width adjustable via splitter

### Task 11: Add Sidebar Width Persistence and Limits
**Files:** `src/ui/LayoutManager.cpp`
**Description:** Sidebar width should be persisted and have reasonable min/max limits. Dragging the splitter resizes within limits.
**Acceptance Criteria:**
- Sidebar width saved to config on resize
- Restored on startup
- Min width: 200px, Max width: 600px
- Collapse: dragging below min width collapses sidebar

### Task 12: Add Panel Drag Reordering in Activity Bar
**Files:** `src/ui/ActivityBar.cpp`
**Description:** Allow users to drag activity bar items to reorder them. Persist the custom order.
**Acceptance Criteria:**
- Activity bar items can be dragged to new positions
- Drop indicator shown during drag
- Custom order persisted to config
- Reset to default order available via context menu

### Task 13: Add Activity Bar Context Menu
**Files:** `src/ui/ActivityBar.cpp`
**Description:** Right-click on activity bar shows context menu: Hide Item, Reset Order, Show All Items. Hidden items can be shown via settings.
**Acceptance Criteria:**
- Right-click shows context menu
- "Hide" removes item from bar (mode still accessible via command palette)
- "Show All" restores all items
- "Reset Order" restores default order
- Hidden items persisted to config

### Task 14: Wire Search Panel Into Sidebar Mode
**Files:** `src/ui/SearchPanel.cpp`, `src/ui/SearchPanel.h`
**Description:** SearchPanel is currently only 9 lines (empty). Implement a basic search sidebar panel that uses the existing SearchEngine (1008 lines) to search workspace content.
**Acceptance Criteria:**
- Search panel shows search input, results list, and replace input
- Results grouped by file with match previews
- Clicking a result opens the file at the match location
- Search supports regex and case-sensitive toggles

### Task 15: Wire Graph View Into Sidebar Mode
**Files:** `src/ui/GraphViewPanel.cpp`
**Description:** GraphViewPanel (359 lines) exists but is not wired as a sidebar mode. Register it with the panel registry and connect to the activity bar Graph mode.
**Acceptance Criteria:**
- Graph mode in activity bar activates GraphViewPanel in sidebar
- Graph shows document/backlink connections
- Clicking a node opens the document
- Filter bar for node types visible

### Task 16: Add Workspace Mode Status Bar Indicator
**Files:** `src/ui/StatusBarPanel.cpp`
**Description:** Status bar should show the current workspace mode (Editor, Canvas, Notebook, etc.) so users always know their context.
**Acceptance Criteria:**
- Mode indicator shown in left section of status bar
- Icon + text label matching current mode
- Clickable to open mode switcher
- Theme-aware styling

### Task 17: Add Quick Mode Switcher Overlay
**Files:** `src/ui/MainFrame.cpp`, `src/ui/CommandPalette.cpp`
**Description:** Add a quick mode switcher (similar to VSCode's Ctrl+Q): shows all modes in a popup list, type to filter, Enter to switch.
**Acceptance Criteria:**
- Keyboard shortcut (Ctrl+Shift+M or similar) opens mode picker
- List shows all modes with icons
- Type to filter modes
- Enter activates selected mode
- Escape dismisses

### Task 18: Add Mode Transition Animation
**Files:** `src/ui/LayoutManager.cpp`
**Description:** When switching between sidebar modes, add a subtle crossfade or slide animation (< 150ms). Animation should be interruptible and respect reduced-motion preference.
**Acceptance Criteria:**
- Mode switch has smooth visual transition (not instant snap)
- Duration: 100-150ms
- Interruptible: rapid switching doesn't queue animations
- Reduced-motion: instant switch when preference is set

### Task 19: Add Workbench Navigation Tests
**Files:** `tests/unit/test_layout.cpp`, `tests/unit/test_workbench_navigation.cpp`
**Description:** Comprehensive tests for: mode switching, panel lifecycle, state persistence, badge updates, keyboard navigation, and event routing.
**Acceptance Criteria:**
- Mode switching: all 7 modes activate correctly
- Panel lifecycle: lazy creation, preservation, restoration
- State persistence: save and restore across restart
- Badge system: set, update, clear badges
- Event routing: all activation paths converge

### Task 20: Add Activity Bar Accessibility Audit
**Files:** `tests/unit/test_accessibility.cpp`
**Description:** Verify that the activity bar meets accessibility requirements: keyboard traversable, screen reader labels, sufficient contrast, focus visible.
**Acceptance Criteria:**
- All activity bar items have accessible names
- Keyboard navigation reaches every item
- Focus ring visible against all theme backgrounds
- Badge counts announced by screen reader

## Testing Requirements
- Mode switching: all 7 modes, from any source (bar, menu, command, event)
- Panel lifecycle: create, hide, restore, destroy cycle
- State persistence: round-trip through save/load
- Keyboard navigation: Tab, arrows, Enter, Escape
- Theme switching: activity bar updates correctly

## Phase Completion Criteria
- Activity bar is the canonical workspace navigator
- 7 modes switch instantly without losing panel state
- Keyboard-first navigation works throughout
- Mode-contextual toolbar actions change appropriately
- All tests pass
