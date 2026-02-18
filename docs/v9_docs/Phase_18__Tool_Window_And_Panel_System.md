# Phase 18: Tool Window and Panel System

## Overview
ToolWindowHost (with TODO for JSON serialization at lines 160/166), PaneManager, and WorkspaceLayout exist but the tool window system is not fully functional. Panels like OutputPanel, ProblemsPanel, TreeViewHost, WebviewHostPanel, and BacklinksPanel exist as data-only stubs without proper wxPanel integration. This phase completes the dockable panel ecosystem.

## Prerequisites
- Phase 06 (Workbench navigation and activity bar)
- Phase 03 (Theme engine for panel theming)

## Tasks

### Task 1: Wire ToolWindowHost JSON Serialization
**Files:** `src/ui/ToolWindowHost.cpp`, `src/ui/ToolWindowHost.h`
**Description:** ToolWindowHost has TODOs at lines 160 and 166 for JSON serialization/deserialization. Complete save/restore of tool window state: which panels are open, their positions, sizes, and pinned state.
**Acceptance Criteria:**
- `save_layout()` serializes all panel states to JSON
- `restore_layout()` restores panels from JSON
- State includes: panel ID, position (bottom/right/left), size, visible, pinned
- Layout saved on window close, restored on open
- Default layout for first launch

### Task 2: Wire OutputPanel as wxPanel
**Files:** `src/ui/OutputPanel.cpp`, `src/ui/OutputPanel.h`
**Description:** OutputPanel exists but may not have full wxPanel rendering. Wire it as a proper wxPanel with scrollable text output, channel selector dropdown, and clear button.
**Acceptance Criteria:**
- OutputPanel inherits wxPanel with proper rendering
- Channel selector dropdown shows available output channels
- Scrollable text area with monospace font
- Auto-scroll to bottom on new output
- "Clear" button clears current channel
- Word wrap toggle

### Task 3: Wire ProblemsPanel as wxPanel
**Files:** `src/ui/ProblemsPanel.cpp`, `src/ui/ProblemsPanel.h`
**Description:** ProblemsPanel exists. Wire it to display diagnostics from DiagnosticsService: errors, warnings, info. Group by file with severity icons.
**Acceptance Criteria:**
- Problems grouped by file with severity count
- Columns: severity icon, message, file, line, source
- Click navigates to error location in editor
- Filter by severity: Error, Warning, Info
- Badge on tool window tab shows total count
- Real-time updates from DiagnosticsService

### Task 4: Wire TreeViewHost for Extension Trees
**Files:** `src/ui/TreeViewHost.cpp`, `src/ui/TreeViewHost.h`
**Description:** TreeViewHost exists for extension-contributed tree views. Wire it to render tree data from TreeDataProviderRegistry with expand/collapse, icons, and context menus.
**Acceptance Criteria:**
- TreeViewHost renders tree data from extensions
- Expand/collapse nodes with arrow icons
- Custom icons from extension
- Context menu on right-click (extension-provided actions)
- Refresh button to reload tree data
- Search/filter within tree

### Task 5: Wire WebviewHostPanel for Extension Webviews
**Files:** `src/ui/WebviewHostPanel.cpp`, `src/ui/WebviewHostPanel.h`
**Description:** WebviewHostPanel exists for extension-contributed HTML panels. Wire it to render HTML content from extensions with message passing.
**Acceptance Criteria:**
- WebviewHostPanel renders HTML content
- Message passing: extension to webview and back
- Content security policy enforced
- Webview state preserved across panel hide/show
- Loading indicator while content renders

### Task 6: Wire Panel Drag-and-Drop Repositioning
**Files:** `src/ui/ToolWindowHost.cpp`, `src/ui/PaneManager.cpp`
**Description:** Users can drag panel tabs to reposition: move between bottom/right/left areas, reorder tabs within an area, or float panels.
**Acceptance Criteria:**
- Drag panel tab to move between areas (bottom, right, left)
- Drag tab to reorder within area
- Drop indicator shows valid drop zones
- Floating panels supported (detach from dock)
- Double-click tab to maximize/restore panel

### Task 7: Wire Panel Pin/Unpin Behavior
**Files:** `src/ui/ToolWindowHost.cpp`
**Description:** Panels can be pinned (always visible) or unpinned (auto-hide when focus leaves). Unpinned panels slide in on hover and slide out when focus moves.
**Acceptance Criteria:**
- Pin icon on each panel tab
- Pinned: panel stays visible
- Unpinned: panel auto-hides, appears on tab hover
- Auto-hide animation: slide in/out (200ms)
- Pin state persisted in layout

### Task 8: Wire Panel Size Constraints
**Files:** `src/ui/PaneManager.cpp`, `src/ui/LayoutManager.cpp`
**Description:** Each panel area has min/max size constraints. Resize handles between areas respect constraints. Double-click handle resets to default size.
**Acceptance Criteria:**
- Bottom area: min 100px, max 60% viewport height
- Side areas: min 150px, max 50% viewport width
- Resize handles between areas
- Double-click handle resets to default
- Size persisted in layout

### Task 9: Wire Panel Badge System
**Files:** `src/ui/ToolWindowHost.cpp`, `src/core/Events.h`
**Description:** Panel tabs show badges for unread content: Problems (error count), Output (new lines), Terminal (activity). Badges clear when panel is focused.
**Acceptance Criteria:**
- Badge renders as colored dot or number on tab
- Problems badge: error count (red), warning count (yellow)
- Output badge: "new" indicator when unfocused
- Badge clears when panel receives focus
- Badge animation on increment

### Task 10: Wire Terminal Panel (Stub Upgrade)
**Files:** `src/core/TerminalService.cpp`, `src/ui/ToolWindowHost.cpp`
**Description:** TerminalService is a 28-line stub. Upgrade it to a basic integrated terminal using platform process spawning. This is a shell panel with input and output.
**Acceptance Criteria:**
- Terminal panel shows shell (zsh/bash/powershell)
- Command input with history (up/down arrows)
- Output rendered in monospace with ANSI color support
- Working directory synced with workspace
- Multiple terminal instances supported
- "New Terminal" and "Kill Terminal" commands

### Task 11: Wire Panel Context Menu
**Files:** `src/ui/ToolWindowHost.cpp`
**Description:** Right-click on panel tab shows context menu: Close, Close Others, Close All, Move to Bottom/Right/Left, Pin/Unpin, Float.
**Acceptance Criteria:**
- Context menu on right-click of panel tab
- All actions functional
- "Close" removes panel from view (reopenable from View menu)
- "Move to" repositions panel area
- Menu items disabled when not applicable

### Task 12: Wire View Menu for Panel Visibility
**Files:** `src/ui/MainFrame.cpp`
**Description:** View menu and command palette list all available panels. Toggling opens/closes panels. Show which panels are currently visible.
**Acceptance Criteria:**
- "View: Toggle Output" shows/hides Output panel
- "View: Toggle Problems" shows/hides Problems panel
- "View: Toggle Terminal" shows/hides Terminal panel
- "View: Toggle Backlinks" shows/hides Backlinks panel
- Checkmark indicates currently visible panels

### Task 13: Wire Panel Focus Navigation
**Files:** `src/ui/ToolWindowHost.cpp`, `src/core/ShortcutManager.cpp`
**Description:** Keyboard shortcuts to focus panels: Cmd+Shift+M (Problems), Cmd+` (Terminal), Cmd+Shift+U (Output). Escape returns focus to editor.
**Acceptance Criteria:**
- Panel focus shortcuts registered
- Shortcut opens panel if closed, focuses if open
- Escape from panel returns focus to editor
- Tab cycles between panel elements
- Ctrl+Tab cycles between open panels

### Task 14: Wire Panel Search
**Files:** `src/ui/ToolWindowHost.cpp`
**Description:** Output and Problems panels support Cmd+F to search within their content. Search highlights matches and navigates between them.
**Acceptance Criteria:**
- Cmd+F in panel opens search bar
- Search matches highlighted
- Navigate between matches with Enter/Shift+Enter
- Search closes on Escape
- Regex search supported

### Task 15: Wire Maximized Panel Mode
**Files:** `src/ui/LayoutManager.cpp`, `src/ui/ToolWindowHost.cpp`
**Description:** Double-clicking a panel tab maximizes it to fill the entire workspace area. Double-click again or press Escape restores original size.
**Acceptance Criteria:**
- Double-click panel tab maximizes panel
- All other panels and editor hidden
- Double-click again restores layout
- Escape restores layout
- Maximized state not persisted (always restores on reopen)

### Task 16: Wire Panel Theme Integration
**Files:** `src/ui/ToolWindowHost.cpp`, `src/ui/OutputPanel.cpp`, `src/ui/ProblemsPanel.cpp`
**Description:** All panels use theme tokens: background, text, borders, badges, severity colors, tab active/inactive states.
**Acceptance Criteria:**
- Panel background from `panel_bg` token
- Tab active from `tab_active_bg` token
- Severity icons: error red, warning yellow, info blue from theme
- Resize handles from `border_color` token
- Theme change updates all panels immediately

### Task 17: Wire Panel Notifications
**Files:** `src/core/NotificationService.cpp`, `src/ui/ToolWindowHost.cpp`
**Description:** NotificationService has TODO for action button callbacks (line 45). Wire notification actions: clicking "Show" on a notification opens the relevant panel and scrolls to content.
**Acceptance Criteria:**
- Notification "Show" action opens relevant panel
- Panel scrolls to relevant content
- "Show Problem" opens Problems panel at specific error
- "Show Output" opens Output panel at relevant line
- Action callbacks wired through NotificationService

### Task 18: Wire Panel Lifecycle Events
**Files:** `src/core/Events.h`, `src/ui/ToolWindowHost.cpp`
**Description:** Emit events for panel lifecycle: PanelOpenedEvent, PanelClosedEvent, PanelFocusedEvent, PanelResizedEvent. Extensions can react to panel state changes.
**Acceptance Criteria:**
- `PanelOpenedEvent` emitted with panel ID
- `PanelClosedEvent` emitted with panel ID
- `PanelFocusedEvent` emitted on focus change
- `PanelResizedEvent` emitted with new dimensions
- Events available to extensions via PluginContext

### Task 19: Wire Default Panel Layouts
**Files:** `src/ui/WorkspaceLayout.cpp`, `src/ui/LayoutManager.cpp`
**Description:** Define default panel layouts for each workspace mode: Editor (Output+Problems at bottom), Canvas (Inspector at right), Notebook (Variables at right), Graph (Backlinks at right).
**Acceptance Criteria:**
- Editor mode: Output and Problems in bottom area
- Canvas mode: Inspector in right area
- Notebook mode: Variables in right area
- Graph mode: Backlinks in right area
- User customizations override defaults

### Task 20: Add Panel System Tests
**Files:** `tests/unit/test_pane_manager.cpp`, `tests/unit/test_workspace_layout.cpp`
**Description:** Test panel system: serialization, drag-drop, pin/unpin, badges, and lifecycle events.
**Acceptance Criteria:**
- Layout serialization round-trip preserves all state
- Panel repositioning updates layout correctly
- Badge counts reflect real data
- Pin/unpin state changes persisted
- Panel focus navigation works correctly

## Testing Requirements
- Layout serialization: save/restore preserves all panel states
- Panel operations: open, close, move, resize, pin
- Badge system: counts update correctly
- Focus management: keyboard navigation between panels

## Phase Completion Criteria
- All tool window panels render as proper wxPanels
- Drag-and-drop repositioning functional
- Pin/unpin with auto-hide
- Badge system for unread content
- Layout persistence across sessions
- All tests pass
