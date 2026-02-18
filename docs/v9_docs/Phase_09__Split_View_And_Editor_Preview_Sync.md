# Phase 09: Split View and Editor-Preview Sync

## Overview
SplitView exists with basic split functionality but lacks workspace presets, synchronized navigation, and the paired-surface mode described in the v8 docs. The authoring experience depends on seamless editor-preview coordination. This phase makes the split workflow production-grade.

## Prerequisites
- Phase 07 (Editor improvements)
- Phase 08 (Preview rendering completeness)

## Tasks

### Task 1: Implement Split Workspace Presets
**Files:** `src/ui/SplitView.cpp`, `src/ui/SplitView.h`
**Description:** Add workspace presets: Editor Focus (90/10 split), Balanced (50/50), Review (30/70), Preview Focus (10/90). Presets switchable via command palette and toolbar.
**Acceptance Criteria:**
- Four presets with defined split ratios
- Animated transitions between presets (100ms, interruptible)
- Current preset persisted to config
- Keyboard shortcuts for each preset

### Task 2: Implement Synchronized Heading Breadcrumbs
**Files:** `src/ui/BreadcrumbBar.cpp`, `src/ui/EditorPanel.cpp`, `src/ui/PreviewPanel.cpp`
**Description:** In split mode, both editor and preview show the same heading breadcrumb path. Moving cursor in editor updates preview breadcrumb and vice versa.
**Acceptance Criteria:**
- Breadcrumb bar shows current heading path
- Editor cursor change updates breadcrumb
- Preview scroll updates breadcrumb
- Clicking breadcrumb in either pane scrolls both panes

### Task 3: Add Selection Mirroring
**Files:** `src/ui/EditorPanel.cpp`, `src/ui/PreviewPanel.cpp`
**Description:** When text is selected in the editor, the corresponding rendered content in the preview is highlighted (and vice versa for click-to-source).
**Acceptance Criteria:**
- Editor selection highlights corresponding preview content
- Preview click highlights corresponding editor text
- Highlight uses theme `editor_match_highlight` token
- Highlight is non-intrusive (subtle background, not selection)

### Task 4: Add Scroll Sync Mode Indicator
**Files:** `src/ui/StatusBarPanel.cpp`, `src/ui/SplitView.cpp`
**Description:** Show the current scroll sync mode in the status bar. Clicking cycles through modes: Proportional, Heading-Anchored, Cursor-Anchored, Off.
**Acceptance Criteria:**
- Status bar shows sync mode icon and label
- Click cycles through modes
- Mode change emits `ScrollSyncModeChangedEvent`
- Current mode persisted

### Task 5: Add Pin Preview Feature
**Files:** `src/ui/SplitView.cpp`, `src/ui/Toolbar.cpp`
**Description:** Add "Pin Preview" that freezes the preview content at the current state. The editor continues to be editable but the preview does not update until unpinned. Useful for referencing rendered content while editing a different section.
**Acceptance Criteria:**
- Pin button in toolbar toggles preview freeze
- Pinned preview shows "Pinned" indicator
- Editor changes do not update pinned preview
- Unpinning refreshes preview with current content

### Task 6: Add "Open in Side" Command
**Files:** `src/ui/MainFrame.cpp`, `src/ui/SplitView.cpp`
**Description:** Add command to open the current file's preview in the secondary sidebar (or a new split pane). This enables side-by-side comparison of two documents.
**Acceptance Criteria:**
- Command: "Open Preview to the Side"
- Opens a second preview pane to the right
- Each pane can show different files
- Independent scroll positions

### Task 7: Add Export Current Render
**Files:** `src/ui/PreviewPanel.cpp`, `src/core/ExportService.h`
**Description:** Add command to export the currently rendered preview as HTML or PDF. Uses the existing ExportService (272 lines).
**Acceptance Criteria:**
- Command: "Export Preview as HTML"
- Command: "Export Preview as PDF" (if PdfExporter available)
- Export includes all rendered styles
- File save dialog with appropriate extension

### Task 8: Add Split View Resize Snapping
**Files:** `src/ui/SplitView.cpp`, `src/ui/SplitterBar.cpp`
**Description:** When dragging the split view divider, snap to preset ratios (25%, 33%, 50%, 67%, 75%) with visual indicator. Hold Shift to disable snapping for free resize.
**Acceptance Criteria:**
- Divider snaps to 5 predefined ratios
- Visual marker at snap points during drag
- Shift key disables snapping
- Snap threshold: 10px

### Task 9: Add Adaptive Sync Throttling Under Load
**Files:** `src/ui/SplitView.cpp`, `src/ui/PreviewPanel.cpp`
**Description:** During rapid editing, throttle preview sync updates to prevent editor jank. Priority is always editor responsiveness. Preview update frequency adapts to typing speed.
**Acceptance Criteria:**
- During rapid typing: preview updates at most every 300ms
- During slow typing/pauses: preview updates within 50ms
- Editor typing latency never exceeds 16ms regardless of preview load
- Throttling transparent to user

### Task 10: Add Live Preview Mode
**Files:** `src/ui/LivePreviewRenderer.cpp`, `src/ui/EditorPanel.cpp`
**Description:** LivePreviewRenderer exists but needs completion. In live preview mode, the editor shows rendered Markdown inline (like Obsidian's live preview): headings rendered large, bold/italic rendered, but cursor position shows source Markdown.
**Acceptance Criteria:**
- Live preview mode toggleable (ViewMode::LivePreview)
- Rendered styling applied to editor text (fonts, sizes)
- Cursor line shows raw Markdown source
- Lines away from cursor show rendered output
- Toggle via command palette and View menu

### Task 11: Add Split Direction Toggle
**Files:** `src/ui/SplitView.cpp`
**Description:** Allow toggling between horizontal split (editor left, preview right) and vertical split (editor top, preview bottom). Persist preference.
**Acceptance Criteria:**
- Command: "Toggle Split Direction"
- Horizontal and vertical layouts
- Smooth transition animation
- Direction persisted per workspace

### Task 12: Add Focus Mode Integration
**Files:** `src/ui/EditorPanel.cpp`, `src/ui/LayoutManager.cpp`
**Description:** Wire `FocusModeChangedEvent` to implement focus mode: hide sidebar, toolbar, status bar, and expand editor to full width. Preview still available in split mode.
**Acceptance Criteria:**
- Focus mode hides all chrome except editor and optional preview
- Toggle via Cmd+Shift+F / Ctrl+Shift+F
- Escape exits focus mode
- Sidebar and chrome restore to previous state

### Task 13: Add Typewriter Scroll Mode
**Files:** `src/ui/EditorPanel.cpp`
**Description:** In typewriter mode, the cursor line is always vertically centered in the editor. Useful for long writing sessions.
**Acceptance Criteria:**
- Typewriter mode keeps cursor line at vertical center
- Works with all scroll sync modes
- Toggle via setting and command palette
- Subtle vertical indicator for center line

### Task 14: Add Split View Keyboard Shortcuts
**Files:** `src/ui/MainFrame.cpp`, `src/core/ShortcutManager.cpp`
**Description:** Register keyboard shortcuts for all split view operations: toggle split, cycle presets, switch focus between panes, toggle sync mode.
**Acceptance Criteria:**
- Cmd+\ / Ctrl+\: toggle split
- Cmd+1 / Ctrl+1: focus editor pane
- Cmd+2 / Ctrl+2: focus preview pane
- Cmd+Shift+\ / Ctrl+Shift+\: toggle split direction
- All shortcuts in command palette

### Task 15: Add Split View State Persistence
**Files:** `src/ui/SplitView.cpp`, `src/core/Config.h`
**Description:** Persist: split ratio, direction, active preset, sync mode, focus mode state. Restore on startup.
**Acceptance Criteria:**
- All split view state saved on change
- Restored on startup
- Per-workspace persistence
- Defaults used when no saved state

### Task 16: Add Reveal in Editor / Reveal in Preview Commands
**Files:** `src/ui/MainFrame.cpp`, `src/ui/EditorPanel.cpp`, `src/ui/PreviewPanel.cpp`
**Description:** Add reciprocal reveal commands: from editor cursor, reveal the corresponding content in preview; from preview position, reveal the corresponding source in editor.
**Acceptance Criteria:**
- "Reveal in Preview": scroll preview to match editor cursor
- "Reveal in Editor": scroll editor to match preview position
- Both available in command palette and context menus
- Works even when scroll sync is Off

### Task 17: Add Split View Performance Tests
**Files:** `tests/unit/test_split_view.cpp`, `tests/unit/test_split_view_advanced.cpp`
**Description:** Test split view performance: preset switching, sync modes, rapid editing, and large document handling.
**Acceptance Criteria:**
- Preset switch: < 200ms including animation
- Sync update: < 16ms for proportional mode
- Large document (10k lines): no lag in editor during preview update
- 100 rapid edits: preview stable, editor responsive

### Task 18: Add Scroll Sync Accuracy Tests
**Files:** `tests/unit/test_live_preview.cpp`
**Description:** Test that scroll sync modes accurately map between editor positions and preview positions for various Markdown structures.
**Acceptance Criteria:**
- Proportional: editor scroll fraction matches preview scroll fraction within 5%
- Heading-anchored: correct heading visible in preview for each editor position
- Cursor-anchored: element at cursor line visible in preview viewport
- Tests with documents containing: headings, code blocks, tables, images

### Task 19: Add Split View Accessibility
**Files:** `tests/unit/test_accessibility.cpp`
**Description:** Verify split view keyboard navigation: focus moves between panes, keyboard shortcuts work in both panes, screen reader announces active pane.
**Acceptance Criteria:**
- Tab/Shift+Tab cycles between panes
- Keyboard shortcuts work regardless of focused pane
- Active pane visually indicated
- Screen reader announces "Editor pane" / "Preview pane"

### Task 20: Add View Menu Completeness
**Files:** `src/ui/MainFrame.cpp`
**Description:** Ensure all split view and editor/preview controls are accessible from the View menu with consistent shortcuts.
**Acceptance Criteria:**
- View > Split Editor: toggle split
- View > Split Direction: toggle direction
- View > Sync Mode: submenu with mode options
- View > Focus Mode: toggle
- View > Zen Mode: toggle
- All with keyboard shortcuts shown

## Testing Requirements
- All three scroll sync modes tested with various document types
- Split presets: all four presets verified
- Performance: editor stays responsive during preview updates
- Keyboard navigation: all pane operations keyboard accessible

## Phase Completion Criteria
- Split presets with animated transitions
- Three scroll sync modes working accurately
- Live preview mode functional
- Pin, focus, and typewriter modes
- All split operations accessible via keyboard, menu, and command palette
- All tests pass
