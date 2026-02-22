# Phase 33: Outline Panel

## Overview

Build a full-featured document Outline Panel as a sidebar view that shows the hierarchical
structure of the current document: headings (H1-H6) for Markdown, and code symbols
(functions, classes, variables) for code files. The panel supports sorting, filtering,
follow-cursor, click-to-navigate, collapse/expand, and feeds data to the BreadcrumbBar.
The existing codebase has two parallel outline implementations: `OutlineService` (with
`Outline.h` data types, `OutlineBuilder`, caching, and breadcrumb path) and
`OutlinePanelController` (a separate V4-era controller with its own `OutlineNode`). This
phase unifies them into a single panel with a wxWidgets tree view.

## Existing Code References

| Component | File | Status |
|-----------|------|--------|
| OutlineService | `/Users/ryanrentfro/code/markamp/src/core/OutlineService.h` | Full API: get_outline, flatten, search, breadcrumb path |
| Outline data types | `/Users/ryanrentfro/code/markamp/src/core/Outline.h` | OutlineNode, OutlineEntry, DocumentOutline, OutlineBuilder |
| OutlinePanelController | `/Users/ryanrentfro/code/markamp/src/core/OutlinePanelController.h` | Parallel implementation, owns VaultService dependency |
| BreadcrumbBar | `/Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.h` | Displays heading path, segment click callback |
| EventBus | `/Users/ryanrentfro/code/markamp/src/core/EventBus.h` | CursorPositionChangedEvent, EditorContentChangedEvent |

## Dependencies

- Phase 31 (Command Palette V2) @ mode uses the same symbol data.
- Phase 32 (Go-To System) consumes outline data for navigation.

---

## Tasks

### Task 1: Unify OutlineService and OutlinePanelController

**Title:** Merge the two parallel outline implementations into a single service

**Description:** `OutlineService` and `OutlinePanelController` both parse headings and
build outline trees but use different data structures and dependency injection patterns.
Merge `OutlinePanelController` functionality into `OutlineService` and deprecate the
controller.

**Implementation Details:**
- Move `OutlinePanelController::build_outline(markdown)` capability into `OutlineService`.
  The existing `OutlineBuilder::build_from_ast` and `build_from_headings` cover this.
- `OutlinePanelController::update_for_document(document_id)` should become a method on
  `OutlineService` that triggers `on_content_changed` with the document's content.
- Replace all `OutlinePanelController` references throughout the codebase with
  `OutlineService` calls.
- Mark `OutlinePanelController` as deprecated, then remove after all consumers are migrated.
- Resolve the duplicate `OutlineNode` definitions (one in `Outline.h`, one in
  `OutlinePanelController.h`) by using only the `Outline.h` version.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/core/OutlineService.h` (add update_for_document)
- Modified: `/Users/ryanrentfro/code/markamp/src/core/OutlineService.cpp`
- Deprecated: `/Users/ryanrentfro/code/markamp/src/core/OutlinePanelController.h`
- Deprecated: `/Users/ryanrentfro/code/markamp/src/core/OutlinePanelController.cpp`
- Modified: All files referencing OutlinePanelController.

**Acceptance Criteria:**
- Only one `OutlineNode` definition exists in the codebase (in `Outline.h`).
- `OutlineService` handles all outline operations previously split between two classes.
- No code references `OutlinePanelController` after migration.
- All existing outline-related tests pass.
- `OutlineService::update_for_document` retrieves content and rebuilds outline.

**Dependencies:** None (foundational unification).

---

### Task 2: Create OutlinePanel wxPanel

**Title:** Build the wxWidgets OutlinePanel as a sidebar view

**Description:** Create the visual outline panel using `wxTreeCtrl` (or custom-drawn tree)
that displays the document structure hierarchy.

**Implementation Details:**
- Create `/Users/ryanrentfro/code/markamp/src/ui/OutlinePanel.h` and `.cpp`.
- Constructor: `OutlinePanel(wxWindow* parent, ThemeEngine&, EventBus&, OutlineService&)`.
- Use `wxTreeCtrl` with custom item data to store `OutlineEntry` references.
- Tree root is invisible; top-level items are H1 headings.
- Each item shows: heading icon (by level), heading text, line number (dimmed).
- Subscribe to outline change events to rebuild the tree.
- Implement `ApplyTheme()` for full theme integration.

**Files Affected:**
- New: `/Users/ryanrentfro/code/markamp/src/ui/OutlinePanel.h`
- New: `/Users/ryanrentfro/code/markamp/src/ui/OutlinePanel.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/CMakeLists.txt`

**Acceptance Criteria:**
- OutlinePanel appears as a sidebar panel.
- Tree displays headings in correct hierarchy (H1 > H2 > H3...).
- Each item shows heading text and line number.
- Icons differentiate heading levels.
- Panel is themed consistently.

**Dependencies:** Task 1 (unified OutlineService).

---

### Task 3: Heading Level Icons

**Title:** Create distinct icons for each heading level (H1-H6) and symbol types

**Description:** Design and render icons that visually distinguish heading levels and
future code symbol types.

**Implementation Details:**
- For headings: H1 = large bold "H" icon, H2 = medium bold, H3-H6 progressively smaller.
- Alternative: use numeric indicators "1"-"6" in colored circles.
- For future code symbols: function = "f()" icon, class = "C" icon, variable = "x" icon.
- Create `OutlineIconProvider` that returns `wxBitmap` for each `SymbolKind` enum value.
- Support HiDPI rendering (1x and 2x).
- Icons should use theme colors.

**Files Affected:**
- New: `/Users/ryanrentfro/code/markamp/src/ui/OutlineIconProvider.h`
- New: `/Users/ryanrentfro/code/markamp/src/ui/OutlineIconProvider.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/OutlinePanel.cpp` (use icons)

**Acceptance Criteria:**
- Each heading level has a visually distinct icon.
- Icons render correctly on standard and HiDPI displays.
- Icons use theme-appropriate colors.
- SymbolKind enum covers: Heading, Function, Class, Variable, Namespace, Interface.

**Dependencies:** Task 2.

---

### Task 4: Sort by Position vs Name

**Title:** Add toggle to sort outline entries by document position or alphabetically

**Description:** Provide a sort toggle in the outline panel toolbar. Default is "by position"
(document order). Alternative is "by name" (alphabetical).

**Implementation Details:**
- Add a toolbar at the top of `OutlinePanel` with sort toggle button.
- Sort modes: `kByPosition` (default, uses `source_line` order), `kByName` (alphabetical
  by `text`, case-insensitive).
- When sort mode changes, rebuild the tree with the new ordering.
- Persist sort preference in Config: `"outline.sortMode"`.
- The tree hierarchy is flattened for alphabetical sort (all items at the same level).

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/OutlinePanel.h` (sort mode, toolbar)
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/OutlinePanel.cpp`

**Acceptance Criteria:**
- Default sort is by document position.
- Clicking the sort toggle switches to alphabetical.
- Alphabetical sort shows all headings at the same level.
- Sort preference persists across sessions.
- Toggle icon indicates the current sort mode.

**Dependencies:** Task 2.

---

### Task 5: Filter/Search Within Outline

**Title:** Add a search bar to filter outline entries

**Description:** Add a search input at the top of the outline panel that filters the
tree to show only entries matching the query.

**Implementation Details:**
- Add a `wxSearchCtrl` above the tree in `OutlinePanel`.
- On input change, call `OutlineService::search_headings(root_id, query)` to get
  matching entries.
- Rebuild the tree showing only matching entries (preserving hierarchy: show a match
  and all its ancestors).
- Highlight the matching portion of the heading text.
- Debounce search input by 200ms.
- Escape in the search field clears the filter.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/OutlinePanel.h` (search ctrl)
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/OutlinePanel.cpp`

**Acceptance Criteria:**
- Typing in the search bar filters the outline tree.
- Matching entries and their ancestors are shown.
- Non-matching entries are hidden.
- Matching text is highlighted in the tree items.
- Clearing the search (Escape or clear button) restores the full tree.
- Search is debounced to avoid excessive rebuilds.

**Dependencies:** Task 2.

---

### Task 6: Follow Cursor (Auto-Reveal)

**Title:** Automatically reveal and highlight the current heading as the cursor moves

**Description:** As the user moves the cursor in the editor, the outline panel should
auto-reveal the heading that contains the cursor position.

**Implementation Details:**
- Subscribe to `CursorPositionChangedEvent` via EventBus.
- On cursor move, call `OutlineService::get_heading_at_line(root_id, line)`.
- Find the corresponding tree item and call `wxTreeCtrl::SelectItem` + `EnsureVisible`.
- Throttle updates to every 100ms to avoid excessive tree operations during scrolling.
- Provide a "Follow Cursor" toggle button in the outline toolbar.
- Persist preference: `"outline.followCursor"` (default: true).

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/OutlinePanel.h` (follow cursor state)
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/OutlinePanel.cpp` (event subscription)

**Acceptance Criteria:**
- Moving the cursor in the editor highlights the corresponding outline item.
- Auto-reveal scrolls the outline tree to show the active item.
- Updates are throttled to avoid performance issues.
- "Follow Cursor" toggle disables auto-reveal when off.
- Preference persists across sessions.

**Dependencies:** Task 2.

---

### Task 7: Click to Navigate

**Title:** Clicking an outline entry navigates the editor to that heading

**Description:** Single-clicking an outline entry scrolls the editor to the heading's
line position.

**Implementation Details:**
- Handle `wxEVT_TREE_SEL_CHANGED` on the `wxTreeCtrl`.
- Retrieve the `OutlineEntry` from the selected item's data.
- Call `OutlineService::scroll_to_heading(block_id)` which emits
  `OutlineScrollToEvent`.
- `EditorPanel` subscribes to `OutlineScrollToEvent` and calls
  `wxStyledTextCtrl::GotoLine`.
- Ensure the heading appears near the top of the viewport (not centered).
- Push a `NavigationEntry` via `NavigationService`.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/OutlinePanel.cpp` (selection handler)
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp` (OutlineScrollToEvent handler)

**Acceptance Criteria:**
- Clicking an outline item scrolls the editor to that heading.
- The heading appears near the top of the viewport.
- Navigation history is updated.
- Double-clicking does not trigger a second scroll.
- Selection in the outline is visually highlighted.

**Dependencies:** Task 2, Phase 32 Task 1 (NavigationService).

---

### Task 8: Collapse/Expand Levels

**Title:** Implement collapse/expand for heading hierarchy with toolbar controls

**Description:** Allow collapsing and expanding heading groups. Add toolbar buttons for
"Collapse All" and "Expand All", plus per-item collapse via tree disclosure triangles.

**Implementation Details:**
- `wxTreeCtrl` provides built-in collapse/expand via disclosure triangles.
- Add toolbar buttons: "Collapse All" (`wxTreeCtrl::CollapseAll`) and "Expand All"
  (`wxTreeCtrl::ExpandAll`).
- Add "Collapse to Level N" submenu: Level 1 (show only H1), Level 2 (show H1+H2), etc.
- Persist collapse state per document in a `std::unordered_map<std::string, std::set<int>>`.
- Restore collapse state when switching between documents.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/OutlinePanel.h` (collapse state storage)
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/OutlinePanel.cpp` (toolbar, state management)

**Acceptance Criteria:**
- Tree items have disclosure triangles for collapse/expand.
- "Collapse All" collapses all tree items.
- "Expand All" expands all tree items.
- "Collapse to Level 2" shows H1 and H2, hides H3+.
- Collapse state is remembered per document.

**Dependencies:** Task 2.

---

### Task 9: Outline Panel in Sidebar

**Title:** Integrate OutlinePanel into the sidebar panel system

**Description:** Register the OutlinePanel as a sidebar view that can be shown/hidden
and positioned alongside the file tree and search panels.

**Implementation Details:**
- Register `OutlinePanel` with the `LayoutManager` as a sidebar view with ID "outline".
- Add a View menu entry: "View > Outline" with toggle behavior.
- Add a command: `view.showOutline` registered in `CommandRegistry`.
- The outline panel should be in a tab alongside the file tree or in a separate
  sidebar section (configurable).
- Show a placeholder message when no document is open: "Open a document to see its outline."

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.h` (register outline panel)
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`
- Modified: View menu construction code.

**Acceptance Criteria:**
- OutlinePanel appears in the sidebar.
- "View > Outline" toggles the panel.
- Panel shows a placeholder when no document is open.
- Panel is restored on application restart if it was visible.
- Panel can be repositioned within the sidebar.

**Dependencies:** Task 2.

---

### Task 10: Breadcrumb Integration (Outline Feeds Breadcrumbs)

**Title:** Wire OutlineService breadcrumb data to BreadcrumbBar

**Description:** Connect `OutlineService::get_breadcrumb_path` to `BreadcrumbBar::SetHeadingPath`
so that the breadcrumb bar always shows the heading ancestry of the cursor position.

**Implementation Details:**
- Subscribe to `CursorPositionChangedEvent` in the component that manages the breadcrumb bar.
- On cursor move, call `OutlineService::get_breadcrumb_path(root_id, line)`.
- Convert the `vector<OutlineEntry>` to `vector<string>` (heading texts).
- Call `BreadcrumbBar::SetHeadingPath(headings)`.
- Throttle updates to every 150ms.
- When cursor is outside any heading, show only the file path (clear heading path).

**Files Affected:**
- Modified: the component managing BreadcrumbBar (likely MainFrame or EditorPanel).
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp` (ensure SetHeadingPath works)

**Acceptance Criteria:**
- Breadcrumb bar shows the heading hierarchy at the cursor position.
- Moving from an H3 under H2 under H1 shows: "H1 > H2 > H3" in breadcrumbs.
- Cursor outside all headings clears the heading breadcrumbs.
- Updates are throttled for smooth performance.
- File path segments remain visible alongside heading segments.

**Dependencies:** Task 6 (follow cursor), BreadcrumbBar from existing codebase.

---

### Task 11: Stale Indicator When Document Changes

**Title:** Show a visual indicator when the outline is stale (being rebuilt)

**Description:** When the document content changes, the outline must be rebuilt. During
the rebuild period, show a subtle "updating..." indicator.

**Implementation Details:**
- When `OutlineService::on_content_changed` is called, set an `is_stale_` flag.
- The outline panel checks this flag and shows a spinner or dimmed overlay.
- After the outline is rebuilt and the tree is updated, clear the stale indicator.
- If rebuild takes > 200ms, show the indicator. For fast rebuilds, skip it (avoid flicker).
- Use a debounced content change handler (300ms) to avoid rebuilding on every keystroke.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/OutlinePanel.h` (stale state, timer)
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/OutlinePanel.cpp`

**Acceptance Criteria:**
- Rapid typing does not cause excessive outline rebuilds.
- A stale indicator appears for rebuilds taking > 200ms.
- The indicator disappears once the rebuild completes.
- Short rebuilds (<200ms) do not show/hide the indicator (no flicker).

**Dependencies:** Task 2.

---

### Task 12: Outline Context Menu

**Title:** Add right-click context menu on outline items

**Description:** Right-clicking an outline item shows options: "Copy Heading Text",
"Copy Link to Heading", "Collapse Children", "Expand Children", "Select All Under Heading".

**Implementation Details:**
- Handle `wxEVT_TREE_ITEM_RIGHT_CLICK` on the tree control.
- Menu items:
  - "Copy Heading Text": copy heading text to clipboard.
  - "Copy Link to Heading": copy `[[doc#heading]]` style link.
  - "Collapse Children": collapse all children of this item.
  - "Expand Children": expand all children.
  - "Select Content": select all text from this heading to the next heading in the editor.
- Show a separator between navigation and editing actions.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/OutlinePanel.cpp` (context menu)

**Acceptance Criteria:**
- Right-click shows a context menu with all listed options.
- "Copy Heading Text" copies the plain heading text.
- "Copy Link to Heading" copies a properly formatted link.
- "Collapse/Expand Children" works on the tree.
- "Select Content" selects text in the editor.

**Dependencies:** Task 2.

---

### Task 13: Outline Drag and Drop for Reordering

**Title:** Enable drag-and-drop reordering of headings in the outline panel

**Description:** Allow users to drag outline items to reorder sections in the document.
Dragging a heading reorders the corresponding content block in the source.

**Implementation Details:**
- Enable `wxTR_EDIT_LABELS` is not needed; use custom DnD with `wxTreeCtrl` and
  `wxDropSource` / `wxDropTarget`.
- When a heading is dragged, calculate the source range (from this heading to the next
  heading at the same or higher level).
- On drop, move the source text block to the new position in the document.
- Show a drop indicator line between items during drag.
- Emit `EditorContentChangedEvent` after the move to trigger outline rebuild.
- Support undo via the editor's undo stack.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/OutlinePanel.h` (DnD support)
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/OutlinePanel.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp` (content reordering)

**Acceptance Criteria:**
- Dragging a heading in the outline reorders the corresponding section in the document.
- Drop indicator shows the target position during drag.
- Undo reverses the reordering.
- Nested headings move with their parent.
- Outline rebuilds after reorder.

**Dependencies:** Task 2, Task 7 (editor interaction).

---

### Task 14: Heading Count Badge

**Title:** Show heading count statistics in the outline panel header

**Description:** Display the total heading count and per-level counts in the outline
panel's toolbar area.

**Implementation Details:**
- Use `DocumentOutline::total_heading_count` and `level_counts` array.
- Show in the panel header: "12 headings (3 H1, 4 H2, 5 H3)".
- Update on outline rebuild.
- Use dimmed text styling.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/OutlinePanel.h` (stats label)
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/OutlinePanel.cpp`

**Acceptance Criteria:**
- Heading count is displayed in the panel header.
- Per-level breakdown is shown.
- Counts update when the document changes.
- Shows "No headings" when the document has none.

**Dependencies:** Task 2.

---

### Task 15: Outline Empty State

**Title:** Show an appropriate empty state when the document has no headings

**Description:** When the current document contains no headings, show a helpful
message instead of an empty tree.

**Implementation Details:**
- Check `DocumentOutline::is_empty()` after outline rebuild.
- Show centered text: "No headings found in this document."
- Optionally show a hint: "Add Markdown headings (# H1, ## H2, etc.) to see
  the document structure here."
- When no document is open, show: "Open a document to view its outline."

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/OutlinePanel.cpp` (empty state)

**Acceptance Criteria:**
- Empty documents show the appropriate message.
- Message text is helpful and actionable.
- No document open shows a different message.
- Switching from an empty document to one with headings shows the tree.

**Dependencies:** Task 2.

---

### Task 16: Outline Panel Toolbar

**Title:** Build the complete outline panel toolbar

**Description:** Assemble all toolbar buttons into a coherent toolbar at the top of the
outline panel.

**Implementation Details:**
- Toolbar items (left to right):
  1. Sort toggle (by position / by name)
  2. Collapse All button
  3. Expand All button
  4. Follow Cursor toggle
  5. Search/filter input (right side)
- Use small icon buttons (16x16) with tooltips.
- Toolbar uses theme colors.
- Toolbar layout responds to panel width.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/OutlinePanel.h` (toolbar construction)
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/OutlinePanel.cpp`

**Acceptance Criteria:**
- All toolbar buttons are present and functional.
- Tooltips describe each button's action.
- Active state is visually indicated (e.g., "Follow Cursor" button shows pressed state).
- Toolbar is themed.
- Search input is accessible via keyboard.

**Dependencies:** Tasks 4, 5, 6, 8.

---

### Task 17: Code Symbol Support via ISymbolProvider

**Title:** Wire ISymbolProvider for non-Markdown files

**Description:** When the current file is not Markdown (e.g., a code file), use the
`ISymbolProvider` interface (Phase 31 Task 4) to populate the outline with code symbols.

**Implementation Details:**
- Check the file type of the current document.
- If Markdown, use `OutlineService` (existing behavior).
- If code, query the registered `ISymbolProvider` for symbols.
- Display code symbols with appropriate icons (function, class, variable).
- Hierarchy: Namespace > Class > Function/Variable.
- If no symbol provider is registered for the file type, show a message:
  "No outline available for this file type."

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/OutlinePanel.cpp` (multi-provider support)
- Modified: `/Users/ryanrentfro/code/markamp/src/core/ISymbolProvider.h` (if created in Phase 31)

**Acceptance Criteria:**
- Markdown files show heading outline.
- Code files show symbol outline (when provider exists).
- Files without a provider show an informative message.
- Symbol hierarchy is correctly nested.

**Dependencies:** Phase 31 Task 4 (ISymbolProvider).

---

### Task 18: Outline Decoration Badges

**Title:** Add decoration badges for TODO, FIXME, and bookmark markers in headings

**Description:** If a heading contains TODO or FIXME markers, or the user has bookmarked
a section, show a small badge next to the heading in the outline.

**Implementation Details:**
- During outline build, scan heading text and the first few lines under each heading
  for TODO, FIXME, HACK, NOTE markers.
- Display a small colored badge: TODO = blue, FIXME = red, NOTE = green.
- Bookmarked sections show a bookmark icon (user can right-click > "Bookmark" an outline item).
- Store bookmarks in a `std::set<std::string>` keyed by heading block_id.
- Persist bookmarks per document.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/OutlinePanel.h` (badge rendering, bookmarks)
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/OutlinePanel.cpp`

**Acceptance Criteria:**
- Headings containing TODO show a blue badge.
- Headings containing FIXME show a red badge.
- Bookmarked headings show a bookmark icon.
- Bookmarks persist across sessions for the same document.
- Badges update when the document content changes.

**Dependencies:** Task 2.

---

### Task 19: Minimap Outline Overlay

**Title:** Show outline heading positions as markers on the editor minimap

**Description:** If the minimap is enabled, overlay heading markers at the corresponding
vertical positions to show the document's structure at a glance.

**Implementation Details:**
- After outline rebuild, compute the relative position of each heading in the document.
- Draw small horizontal lines or colored markers on the minimap at those positions.
- Different heading levels use different marker widths or colors.
- Click on a marker in the minimap navigates to that heading.

**Files Affected:**
- Modified: The minimap rendering component (if it exists, otherwise this is deferred).
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/OutlinePanel.cpp` (provide position data)

**Acceptance Criteria:**
- Heading markers appear on the minimap.
- Different heading levels are visually distinguishable.
- Clicking a marker scrolls to the heading.
- Markers update when the outline changes.

**Dependencies:** Task 2, minimap component.

---

### Task 20: Outline Event Integration

**Title:** Emit and consume all outline-related events

**Description:** Define and wire all events needed for outline synchronization:
outline rebuild, active heading change, heading clicked, outline visibility toggle.

**Implementation Details:**
- Events (add to Events.h if not present):
  - `OutlineRebuiltEvent{ std::string root_id; int heading_count; }`
  - `OutlineActiveHeadingChangedEvent{ std::string root_id; int line; std::string heading_text; }`
  - `OutlineScrollToEvent{ std::string block_id; int line; }`
  - `OutlineVisibilityChangedEvent{ bool visible; }`
- OutlineService emits `OutlineRebuiltEvent` after rebuild.
- OutlineService emits `OutlineActiveHeadingChangedEvent` on cursor move.
- OutlinePanel emits `OutlineScrollToEvent` on item click.
- LayoutManager emits `OutlineVisibilityChangedEvent` on panel toggle.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/core/Events.h` (outline events)
- Modified: `/Users/ryanrentfro/code/markamp/src/core/OutlineService.cpp` (emit events)
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/OutlinePanel.cpp` (emit/subscribe)

**Acceptance Criteria:**
- All four outline events are declared and used.
- OutlinePanel subscribes to `OutlineRebuiltEvent` to refresh the tree.
- BreadcrumbBar subscribes to `OutlineActiveHeadingChangedEvent`.
- EditorPanel subscribes to `OutlineScrollToEvent`.
- No circular event loops.

**Dependencies:** Tasks 2, 6, 7.

---

### Task 21: Outline Theme Integration

**Title:** Full theme support for the outline panel

**Description:** Apply theme tokens to all outline panel visual elements.

**Implementation Details:**
- Theme tokens: `outline.background`, `outline.foreground`, `outline.activeItemBackground`,
  `outline.activeItemForeground`, `outline.headingLevelColors[1-6]`,
  `outline.searchHighlight`, `outline.toolbarBackground`, `outline.separator`.
- Subscribe to `ThemeChangedEvent` and refresh all colors.
- Tree item colors should differentiate heading levels (optional: H1 = brighter, H6 = dimmer).

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/OutlinePanel.cpp` (ApplyTheme)
- Modified: Theme definitions.

**Acceptance Criteria:**
- All outline panel elements use theme tokens.
- Theme switch updates colors immediately.
- Heading level colors are distinguishable.
- High contrast theme maintains readability.

**Dependencies:** Task 2.

---

### Task 22: Outline Keyboard Navigation

**Title:** Full keyboard support for the outline panel

**Description:** When the outline panel is focused, support keyboard navigation:
arrow keys, Enter to navigate, Ctrl+Shift+O to focus the panel.

**Implementation Details:**
- Arrow Up/Down: move selection in the tree.
- Enter: navigate the editor to the selected heading.
- Left/Right: collapse/expand the selected item.
- Home/End: jump to first/last item.
- Ctrl+Shift+O: focus the outline panel (global shortcut).
- Escape: return focus to the editor.
- Type-to-filter: typing alphanumeric characters activates the search bar.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/OutlinePanel.cpp` (keyboard handlers)

**Acceptance Criteria:**
- All listed keyboard shortcuts work in the outline panel.
- Enter navigates the editor.
- Escape returns focus to the editor.
- Type-to-filter activates the search bar with the typed character.

**Dependencies:** Task 2.

---

### Task 23: Outline Panel Width Persistence

**Title:** Remember outline panel width across sessions

**Description:** When the user resizes the outline panel, persist the width in Config.

**Implementation Details:**
- On sash drag end (if using a splitter), save the width to `Config::set("outline.width", width)`.
- On startup, restore the width from Config.
- Default width: 250px. Min: 150px. Max: 500px.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/OutlinePanel.cpp` (width persistence)
- Modified: sidebar layout code.

**Acceptance Criteria:**
- Resizing the outline panel persists the width.
- Width is restored on next application start.
- Width is clamped to min/max bounds.

**Dependencies:** Task 9 (sidebar integration).

---

### Task 24: Outline Accessibility

**Title:** Ensure outline panel is fully accessible

**Description:** Add proper accessibility labels and roles to the outline panel for
screen readers.

**Implementation Details:**
- Set accessible name on the tree control: "Document Outline".
- Each tree item should have an accessible description: "Heading level N: text, line M".
- Toolbar buttons should have accessible labels.
- Search input should have an accessible label: "Filter outline".
- Use `wxAccessible` to provide role information.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/OutlinePanel.cpp` (accessibility)

**Acceptance Criteria:**
- VoiceOver (macOS) reads tree items correctly.
- Accessible names are set on all interactive elements.
- Screen reader can navigate the tree structure.
- Focus management works correctly with assistive technology.

**Dependencies:** Task 2.

---

### Task 25: Unit Tests for Outline Panel

**Title:** Comprehensive test suite for outline functionality

**Description:** Write Catch2 tests for the outline data model, service, and panel behavior.

**Implementation Details:**
- Test file: `/Users/ryanrentfro/code/markamp/tests/unit/test_outline_panel.cpp`
- Sections:
  - OutlineBuilder: build from headings, nested hierarchy, empty document.
  - OutlineService: get_outline, get_flat_outline, search_headings, get_breadcrumb_path, invalidation.
  - Sort: position order, alphabetical order.
  - Filter: substring match, empty query, no matches.
  - Follow cursor: heading_at_line for various cursor positions.
  - Stale detection: invalidation triggers stale flag.
  - Edge cases: single heading, all same level, deeply nested (H1>H2>H3>H4>H5>H6).

**Files Affected:**
- New: `/Users/ryanrentfro/code/markamp/tests/unit/test_outline_panel.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/CMakeLists.txt`

**Acceptance Criteria:**
- All test sections pass.
- Edge cases (empty, single, deep nesting) are covered.
- OutlineBuilder produces correct hierarchy from flat headings.
- Breadcrumb path computation is correct for all nesting scenarios.
- At least 30 test cases.

**Dependencies:** Task 1 (unified OutlineService).
