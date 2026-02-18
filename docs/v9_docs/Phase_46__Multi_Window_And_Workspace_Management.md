# Phase 46: Multi-Window and Workspace Management

## Overview
MainFrame, LayoutManager, PaneManager, and WorkspaceLayout exist but MarkAmp is a single-window application. Users cannot open multiple windows, detach tabs, or manage multiple workspaces simultaneously. This phase adds multi-window support and advanced workspace management.

## Prerequisites
- Phase 06 (Workbench navigation)
- Phase 18 (Tool window and panel system)
- Phase 20 (File management and workspace)

## Tasks

### Task 1: Wire Multi-Window Support
**Files:** `src/app/MarkAmpApp.cpp`, `src/ui/MainFrame.cpp`
**Description:** Support multiple MainFrame instances: each window has independent editor tabs, panels, and workspace view. Shared services (EventBus, Config, etc.) across windows.
**Acceptance Criteria:**
- "New Window" command creates new MainFrame
- Each window has independent tab set
- Shared services: EventBus, Config, PluginManager
- Independent: layout, active tab, panel state
- Window list in Window menu
- Close last window: quit application (macOS: keep app running)

### Task 2: Wire Tab Detachment
**Files:** `src/ui/TabBar.cpp`, `src/ui/MainFrame.cpp`
**Description:** Drag tab out of tab bar to create new window with that tab. Drag tab between windows to move files.
**Acceptance Criteria:**
- Drag tab out of window: creates new window with file
- Drag tab to another window: moves file to target
- Drag indicator shows during drag
- Tab state (scroll position, undo history) preserved
- Multi-file drag: drag multiple selected tabs
- Drop back: return tab to original window

### Task 3: Wire Split Editor Within Window
**Files:** `src/ui/SplitView.cpp`, `src/ui/LayoutManager.cpp`
**Description:** SplitView exists. Wire advanced split: split horizontally, vertically, up to 4 panes. Each pane has independent tabs.
**Acceptance Criteria:**
- Split right: Cmd+\\ creates vertical split
- Split down: Cmd+Shift+\\ creates horizontal split
- Up to 4 panes (2x2 grid)
- Each pane has independent tab set
- Focus indicated by accent border
- Close pane: tabs merge to adjacent pane

### Task 4: Wire Pane Management Commands
**Files:** `src/ui/MainFrame.cpp`, `src/ui/PaneManager.cpp`
**Description:** Commands for pane management: "Split Right", "Split Down", "Close Pane", "Move to Next Pane", "Focus Next Pane".
**Acceptance Criteria:**
- "Split Right" / "Split Down" split active pane
- "Close Pane" closes current pane
- "Move File to Next Pane" moves active tab
- "Focus Next Pane" (Cmd+K Cmd+Right) cycles focus
- "Join All Panes" merges all panes into one
- All commands with keyboard shortcuts

### Task 5: Wire Workspace Tabs
**Files:** `src/ui/MainFrame.cpp`, `src/core/WorkspaceService.cpp`
**Description:** Multiple workspaces can be open in one window as workspace tabs. Workspace tabs appear above editor tabs.
**Acceptance Criteria:**
- Workspace tab bar above editor area
- "Add Workspace" opens folder picker
- Click workspace tab switches context
- Each workspace: independent file tree, tabs, settings
- Shared: application settings, themes, extensions
- Workspace tab context menu: Close, Duplicate

### Task 6: Wire Zen Mode
**Files:** `src/ui/MainFrame.cpp`, `src/ui/LayoutManager.cpp`
**Description:** Zen mode: hide all chrome except the editor. Full-screen, centered editor with configurable width.
**Acceptance Criteria:**
- "Toggle Zen Mode" command (Cmd+K Z)
- Hide: menu bar, tab bar, status bar, sidebar, panels
- Editor centered with comfortable width (80 chars default)
- Full-screen entry
- Escape: exit zen mode
- Zen mode settings: line width, show line numbers, show minimap

### Task 7: Wire Focus Mode
**Files:** `src/ui/EditorPanel.cpp`, `src/ui/LayoutManager.cpp`
**Description:** Focus mode: dim everything except the current paragraph. Helps focus on writing.
**Acceptance Criteria:**
- "Toggle Focus Mode" command
- Current paragraph: full brightness
- Other paragraphs: dimmed (configurable opacity)
- Cursor movement updates focused paragraph
- Focus mode indicator in status bar
- Configurable: paragraph, sentence, or line level

### Task 8: Wire Window State Persistence
**Files:** `src/app/MarkAmpApp.cpp`, `src/ui/MainFrame.cpp`
**Description:** Window state persisted: position, size, maximized state, split configuration, open workspaces.
**Acceptance Criteria:**
- Window position and size saved on close
- Restored on next launch
- Multi-window: all windows saved
- Multi-monitor: windows open on correct monitor
- Maximized/full-screen state preserved
- Default: centered on primary monitor

### Task 9: Wire Recent Files Navigation
**Files:** `src/ui/CommandPalette.cpp`, `src/core/CommandHistory.cpp`
**Description:** Navigate recent files: Cmd+P shows recent files first, Ctrl+Tab shows tab switching overlay (VS Code style).
**Acceptance Criteria:**
- Ctrl+Tab: shows recently used tabs overlay
- Hold Ctrl: navigate with Tab/Shift+Tab
- Release Ctrl: switch to selected tab
- Cmd+P: recent files first in list
- Recent files across all panes
- History: 50 most recent files

### Task 10: Wire Window Layout Templates
**Files:** `src/ui/WorkspaceLayout.cpp`
**Description:** Predefined window layouts: Writing (editor only), Research (editor + preview + backlinks), Development (editor + problems + output), Canvas (canvas full).
**Acceptance Criteria:**
- Layout templates in command palette: "Layout: ..."
- Writing: single editor, no panels
- Research: editor, preview split, backlinks panel
- Development: editor, output + problems at bottom
- Canvas: canvas workspace full
- Custom layouts saveable as named templates

### Task 11: Wire Pinned Tabs
**Files:** `src/ui/TabBar.cpp`
**Description:** Pin tabs: pinned tabs stay at left of tab bar, show only icon, cannot be closed accidentally.
**Acceptance Criteria:**
- Right-click tab: "Pin Tab"
- Pinned tabs: compact (icon only)
- Pinned tabs: always at left side
- Pinned tabs: close shows confirmation
- Unpin: right-click "Unpin Tab"
- Pinned state persisted

### Task 12: Wire Tab Groups
**Files:** `src/ui/TabBar.cpp`
**Description:** Group related tabs with colored borders. Name groups. Collapse/expand groups.
**Acceptance Criteria:**
- Select tabs, right-click: "Group Tabs"
- Group name and color
- Grouped tabs have colored top border
- Click group label: collapse/expand
- Drag tabs between groups
- Ungroup: right-click "Ungroup"

### Task 13: Wire Mini Map in Editor
**Files:** `src/ui/EditorPanel.cpp`
**Description:** Editor minimap: scaled-down view of entire document in right margin. Current viewport indicated. Click to navigate.
**Acceptance Criteria:**
- Minimap in right margin (toggleable)
- Shows full document at scale
- Viewport indicator as highlighted region
- Click to navigate to position
- Syntax highlighting in minimap (simplified)
- Minimap width configurable

### Task 14: Wire Breadcrumb Navigation
**Files:** `src/ui/BreadcrumbBar.cpp`
**Description:** BreadcrumbBar exists. Wire breadcrumb navigation: show file path + document structure (headings). Click any segment to navigate.
**Acceptance Criteria:**
- Breadcrumb: workspace > folder > file > heading
- Click segment: show children in dropdown
- File segment: show sibling files
- Heading segment: show sibling headings
- Navigate by clicking any breadcrumb
- Breadcrumb updates on cursor move

### Task 15: Wire Window Commands
**Files:** `src/ui/MainFrame.cpp`
**Description:** Register window commands: "New Window", "Close Window", "Split Right", "Split Down", "Toggle Zen Mode", "Toggle Focus Mode", "Toggle Minimap", "Close All Tabs".
**Acceptance Criteria:**
- All commands registered in command palette
- Keyboard shortcuts for common operations
- "Close All Tabs" confirms if unsaved
- "Reopen Closed Tab" (Cmd+Shift+T)
- Window commands in Window menu

### Task 16: Wire Window Theme Integration
**Files:** `src/ui/MainFrame.cpp`, `src/ui/TabBar.cpp`
**Description:** Multi-window and tab features use theme tokens: tab group colors, pinned tab indicators, zen mode background, focus mode dimming.
**Acceptance Criteria:**
- Tab group colors: from theme palette
- Pinned tab indicator: accent color
- Zen mode background: `editor_bg` with optional tinting
- Focus mode dimming: configurable opacity
- Minimap: theme-aware colors

### Task 17: Wire Session Management
**Files:** `src/core/WorkspaceService.cpp`, `src/app/MarkAmpApp.cpp`
**Description:** Named sessions: save and restore complete application state (all windows, all tabs, all splits, all panel states).
**Acceptance Criteria:**
- "Session: Save" saves all window states
- "Session: Restore" restores complete state
- Named sessions: multiple saved sessions
- Default session: auto-saved on quit, auto-restored on launch
- Session includes: windows, tabs, splits, panels, scroll positions
- Import/export sessions

### Task 18: Wire Side-by-Side Diff in Split
**Files:** `src/ui/SplitView.cpp`, `src/ui/DiffPanel.cpp`
**Description:** Split view supports diff mode: same file two versions side-by-side with synchronized scrolling.
**Acceptance Criteria:**
- "Compare with Saved" opens diff in split
- "Compare Files" picks two files for diff
- Synchronized scrolling between panes
- Diff highlighting: additions, deletions
- Navigate between changes
- Close diff: return to normal split

### Task 19: Wire Window Accessibility
**Files:** `src/ui/MainFrame.cpp`, `src/core/AccessibilityManager.cpp`
**Description:** Multi-window accessible: window switching announced, tab switching announced, pane focus announced.
**Acceptance Criteria:**
- Window switch: announce window title
- Tab switch: announce file name
- Pane focus: announce pane position (e.g., "Right pane")
- Zen mode: announce entry/exit
- Focus mode: announce active/inactive
- All pane operations keyboard-accessible

### Task 20: Add Window Management Tests
**Files:** `tests/unit/test_window_management.cpp`
**Description:** Test window management: multi-window state, tab operations, split view, session persistence.
**Acceptance Criteria:**
- Multi-window creation and destruction
- Tab pin/unpin/group operations
- Split view: create, navigate, close
- Session save/restore round-trip
- Zen mode toggle
- Layout template application

## Testing Requirements
- Multi-window state management
- Tab operations: pin, group, detach
- Split view: create, navigate, resize
- Session persistence round-trip

## Phase Completion Criteria
- Multi-window with independent tabs
- Tab detachment between windows
- Advanced split view (up to 4 panes)
- Zen mode and focus mode
- Pinned tabs and tab groups
- Session management
- All tests pass
