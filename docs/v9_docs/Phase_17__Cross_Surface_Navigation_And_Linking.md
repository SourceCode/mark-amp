# Phase 17: Cross-Surface Navigation and Linking

## Overview
SurfaceLink, SurfaceLinkRouter, NavigationService, and SurfaceTransitionCoordinator exist with rich type definitions (SurfaceKind, EntityKind, LinkAnchor, TransitionSnapshot) but are not wired to actual surface transitions. Users cannot navigate from an editor wiki-link to a canvas object, or from a graph node to a notebook cell. This phase completes cross-surface navigation.

## Prerequisites
- Phase 06 (Workbench navigation)
- Phase 11 (Canvas workbench shell)
- Phase 15 (Notebook system)
- Phase 16 (Knowledge graph)

## Tasks

### Task 1: Wire SurfaceLinkRouter Resolution
**Files:** `src/core/SurfaceLink.cpp`, `src/core/SurfaceLink.h`
**Description:** SurfaceLinkRouter has resolve methods but they are not connected to actual surface discovery. Wire resolution for each SurfaceKind: Editor, Preview, Canvas, Graph, Notebook.
**Acceptance Criteria:**
- `resolve(link)` returns success with target surface for valid links
- Editor links resolve to file path + line/column
- Canvas links resolve to board ID + object ID
- Notebook links resolve to notebook path + cell index
- Graph links resolve to document node ID
- Failed resolution returns LinkFailureReason

### Task 2: Wire NavigationService Back/Forward History
**Files:** `src/ui/NavigationService.cpp`, `src/ui/NavigationService.h`
**Description:** NavigationService has per-pane history and global timeline. Wire Cmd+[ (back) and Cmd+] (forward) to navigate between surfaces, preserving scroll position and selection state.
**Acceptance Criteria:**
- Every navigation creates a NavigationEntry in history
- Cmd+[ navigates back to previous surface/position
- Cmd+] navigates forward
- History stores: surface kind, entity, scroll position, cursor/selection
- History depth: 50 entries per pane
- `NavigationEntryAddedEvent` emitted

### Task 3: Wire SurfaceTransitionCoordinator
**Files:** `src/ui/SurfaceTransitionCoordinator.cpp`, `src/ui/SurfaceTransitionCoordinator.h`
**Description:** SurfaceTransitionCoordinator has begin_transition, commit, and cancel. Wire it to capture and restore TransitionSnapshot (scroll, cursor, zoom, active object) during surface switches.
**Acceptance Criteria:**
- `begin_transition()` captures current surface state
- `commit()` activates target surface and restores target state
- `cancel()` returns to original surface state
- Snapshot includes: scroll position, cursor, zoom level, selection
- Transition completes within 100ms

### Task 4: Wire Editor-to-Canvas Navigation
**Files:** `src/ui/EditorPanel.cpp`, `src/ui/CanvasWorkspacePanel.cpp`
**Description:** Wiki-links in editor can reference canvas objects: `[[board:Board Name/Object Label]]`. Clicking navigates to canvas mode, opens the board, and selects the object.
**Acceptance Criteria:**
- Wiki-link syntax `[[board:...]]` recognized in editor
- Click resolves to board + object via SurfaceLinkRouter
- Canvas mode activates with board loaded
- Target object selected and centered in viewport
- Navigation history entry created

### Task 5: Wire Canvas-to-Editor Navigation
**Files:** `src/ui/CanvasWorkspacePanel.cpp`, `src/ui/EditorPanel.cpp`
**Description:** Canvas objects can reference documents. Double-clicking a StickyNote or TextBox with a `[[document]]` reference opens the document in the editor.
**Acceptance Criteria:**
- StickyNote/TextBox content scanned for wiki-links
- Double-click on wiki-link in canvas object opens editor
- Editor scrolls to referenced section if anchor specified
- Navigation history entry created
- `SurfaceLinkNavigatedEvent` emitted

### Task 6: Wire Graph-to-Editor Navigation
**Files:** `src/ui/GraphViewPanel.cpp`, `src/ui/EditorPanel.cpp`
**Description:** Double-clicking a graph node opens the corresponding document in the editor. If already in split view, opens in the adjacent pane.
**Acceptance Criteria:**
- Double-click graph node opens document in editor
- Split view: opens in adjacent pane if available
- Editor scrolls to top of document
- Navigation history entry created
- Selected node stays highlighted in graph (if visible)

### Task 7: Wire Editor-to-Notebook Navigation
**Files:** `src/ui/EditorPanel.cpp`, `src/core/NotebookCellManager.cpp`
**Description:** Wiki-links can reference notebook cells: `[[notebook:Notebook Name#Cell 3]]`. Clicking opens the notebook and scrolls to the referenced cell.
**Acceptance Criteria:**
- Wiki-link syntax `[[notebook:...]]` recognized
- Click resolves to notebook path + cell index
- Notebook mode activates with notebook loaded
- Scrolls to referenced cell
- Navigation history entry created

### Task 8: Wire Breadcrumb Cross-Surface Trail
**Files:** `src/ui/BreadcrumbBar.cpp`, `src/ui/NavigationService.cpp`
**Description:** BreadcrumbBar shows the navigation trail across surfaces. Each breadcrumb shows surface icon + entity name. Clicking any breadcrumb navigates back to that point.
**Acceptance Criteria:**
- Breadcrumbs show: [Surface Icon] [Entity Name]
- Editor: file icon + filename
- Canvas: canvas icon + board name + object name
- Graph: graph icon + "Global" or document name
- Notebook: notebook icon + notebook name
- Click any breadcrumb navigates to that entry

### Task 9: Wire "Navigate To" Quick Picker
**Files:** `src/ui/CommandPalette.cpp`, `src/core/SurfaceLink.cpp`
**Description:** Command palette command "Navigate To" shows a unified picker of all navigable entities across all surfaces: documents, canvas boards, canvas objects, notebook files, graph nodes.
**Acceptance Criteria:**
- "Navigate To" (Cmd+P) shows unified entity list
- Results categorized by surface with icon
- Fuzzy search across all entity names
- Most recently visited entities ranked higher
- Selecting entity navigates to appropriate surface

### Task 10: Wire Cross-Surface Peek
**Files:** `src/ui/EditorPanel.cpp`, `src/ui/CanvasWorkspacePanel.cpp`
**Description:** Hovering over a cross-surface link shows a peek popup with preview of the target: document preview, canvas object thumbnail, notebook cell preview.
**Acceptance Criteria:**
- Hover over wiki-link shows peek popup after 500ms
- Document link: shows first 10 lines of target
- Canvas link: shows thumbnail of target object
- Notebook link: shows cell source and last output
- Escape or mouse-out dismisses peek
- Click peek opens full navigation

### Task 11: Wire Split-Surface View
**Files:** `src/ui/SplitView.cpp`, `src/ui/LayoutManager.cpp`
**Description:** Split view can show different surfaces side by side: editor + canvas, editor + graph, notebook + editor. Each pane maintains independent navigation history.
**Acceptance Criteria:**
- Split view supports heterogeneous surfaces
- Each pane has independent navigation history
- Drag entity to opposite pane opens it there
- Synced scroll available for editor + preview
- "Open to Side" command places in adjacent pane

### Task 12: Wire Link Validation
**Files:** `src/core/SurfaceLink.cpp`, `src/core/BacklinkIndex.cpp`
**Description:** Validate all cross-surface links on workspace open and file save. Broken links (target deleted, renamed, moved) flagged with visual indicator and listed in Problems panel.
**Acceptance Criteria:**
- Broken links underlined with warning color
- Hover shows "Target not found: [path]"
- Problems panel lists all broken links
- "Fix Broken Link" action shows suggestions (fuzzy match)
- Link validation runs on save (debounced 2 seconds)

### Task 13: Wire Link Refactoring (Rename Propagation)
**Files:** `src/core/BacklinkIndex.cpp`, `src/core/WikiLinkParser.cpp`
**Description:** When a document is renamed, all wiki-links pointing to it are updated automatically. Applies across all surfaces: editor files, canvas objects, notebook cells.
**Acceptance Criteria:**
- File rename triggers backlink update scan
- All `[[ ]]` references to old name updated to new name
- Canvas object text content updated
- Notebook cell source updated
- Confirmation dialog shows all changes before applying
- `LinksRefactoredEvent` emitted with count

### Task 14: Wire Navigation Status Bar
**Files:** `src/ui/StatusBarPanel.cpp`, `src/ui/NavigationService.cpp`
**Description:** Status bar shows current surface and navigation controls: back/forward buttons, surface indicator icon, and entity name.
**Acceptance Criteria:**
- Surface icon shows current active surface
- Entity name truncated with ellipsis (max 30 chars)
- Back/forward buttons enabled when history available
- Click surface icon shows surface switcher
- Tooltip shows full entity path

### Task 15: Wire "Open in Surface" Context Menu
**Files:** `src/ui/EditorPanel.cpp`, `src/ui/GraphViewPanel.cpp`, `src/ui/CanvasWorkspacePanel.cpp`
**Description:** Right-click on any entity shows "Open in [Surface]" options: Open in Editor, Show in Graph, Find on Canvas, Open in Notebook.
**Acceptance Criteria:**
- Context menu shows applicable surface options
- "Show in Graph" highlights node in graph view
- "Find on Canvas" searches all boards for references
- Options disabled if entity not present on that surface
- Opens in new pane if Cmd held

### Task 16: Wire Navigation Keyboard Shortcuts
**Files:** `src/ui/MainFrame.cpp`, `src/core/ShortcutManager.cpp`
**Description:** Register all navigation shortcuts: Cmd+[ (back), Cmd+] (forward), Cmd+P (navigate to), Cmd+Shift+E (editor), Cmd+Shift+G (graph), Cmd+Shift+C (canvas), Cmd+Shift+N (notebook).
**Acceptance Criteria:**
- All shortcuts registered in shortcut manager
- Shortcuts shown in command palette
- Shortcuts context-aware (only active when applicable)
- Shortcuts documented in keyboard shortcut overlay
- No conflicts with existing shortcuts

### Task 17: Wire Deep Link URL Scheme
**Files:** `src/core/SurfaceLink.cpp`, `src/app/MarkAmpApp.cpp`
**Description:** Support `markamp://` URL scheme for deep linking: `markamp://editor/path/to/file.md#heading`, `markamp://canvas/board-id/object-id`, `markamp://notebook/path/cell/3`.
**Acceptance Criteria:**
- URL scheme handler registered on application start
- `markamp://editor/...` opens file in editor
- `markamp://canvas/...` opens board and selects object
- `markamp://notebook/...` opens notebook at cell
- Invalid URLs show error notification

### Task 18: Wire Navigation Analytics
**Files:** `src/ui/NavigationService.cpp`, `src/core/OutputChannelService.cpp`
**Description:** Track navigation patterns: most navigated paths, surface transition frequency, broken link encounters. Surface in output channel for diagnostics.
**Acceptance Criteria:**
- Navigation events logged to "Navigation" output channel
- Surface transition counts tracked
- Most-visited entities tracked (top 20)
- Broken link navigation attempts counted
- Statistics available via command: "Navigation: Show Statistics"

### Task 19: Wire Navigation Accessibility
**Files:** `src/ui/NavigationService.cpp`, `src/core/AccessibilityManager.cpp`
**Description:** Screen reader announces surface transitions, breadcrumb trail, and navigation history. Focus management ensures keyboard users can navigate across surfaces.
**Acceptance Criteria:**
- Surface switch announced: "Navigated to Editor: filename.md"
- Breadcrumb trail readable by screen reader
- Back/forward accessible via keyboard
- Focus placed on primary content after transition
- Navigation history navigable via keyboard

### Task 20: Add Cross-Surface Navigation Tests
**Files:** `tests/unit/test_navigation_service.cpp`, `tests/unit/test_workbench_navigation.cpp`
**Description:** Test cross-surface navigation: link resolution, history management, transition coordination, and link validation.
**Acceptance Criteria:**
- Link resolution for all surface kinds
- History push/back/forward correctness
- Transition snapshot capture and restore
- Broken link detection accuracy
- Rename propagation across surfaces

## Testing Requirements
- Link resolution: all surface kinds, valid and invalid targets
- Navigation history: push, back, forward, depth limit
- Transition: snapshot capture, commit, cancel
- Link validation: broken link detection, rename propagation

## Phase Completion Criteria
- Cross-surface navigation functional for all surface pairs
- Navigation history with back/forward
- Breadcrumb trail across surfaces
- Link validation with broken link detection
- Rename propagation updates all references
- All tests pass
