# Phase 08 -- Primary Sidebar Overhaul

## Objective

Transform the primary sidebar from a basic panel-switching container into a feature-complete, VS Code-quality sidebar with a professional header (workspace name + action icons), panel-specific toolbars, smooth panel transition animations, collapsible sections within panels, themed scrollbars, resize snap behavior, loading skeletons, and full accessibility. The sidebar is the primary information panel and must support the growing number of panel types (Explorer, Search, Source Control, Extensions, Graph, AI, etc.) with consistent UX patterns.

## Prerequisites

- Phase 01 (Design System Foundation) -- for spacing, typography, component sizes.
- Phase 02 (Icon Library) -- for action icons in panel headers.
- Phase 03 (Theme Engine V2) -- for `sidebar.*` scoped tokens.
- Phase 04 (Animation Framework) -- for panel transitions, skeleton loading.
- Phase 05 (Accessibility) -- for sidebar focus navigation, screen reader.
- Phase 06 (Workbench Shell) -- for sidebar zone placement.
- Phase 07 (Activity Bar) -- for sidebar mode switching.

## Deliverables

- `SidebarHeader` -- workspace name and global sidebar actions.
- `PanelHeader` -- per-panel header with title and contextual action icons.
- `SidebarSection` -- collapsible section container within panels.
- `ThemedScrollbar` -- custom-painted scrollbar matching the theme.
- Smooth panel transition animations.
- Loading skeleton states for all panels.
- Catch2 test target: `test_primary_sidebar`.

## Estimated Complexity

High -- touches multiple panels and introduces several new shared components.

---

## Tasks

### Task 1: Implement the SidebarHeader Component

**Description:** Create a header that appears at the very top of the primary sidebar, showing the workspace/project name and global sidebar action icons (collapse all, open folder).

**Key Implementation Details:**
- Class: `SidebarHeader` in `src/ui/SidebarHeader.h`
- Height: 35px from `ComponentSizeResolver::resolve(kPanelHeader).height`.
- Content (left to right):
  - Workspace name text (bold, using `TypographyScale::font(kBodyStrong)`).
  - Spacer.
  - Action icons: "Collapse All" (`"collapse-all"` icon), "Refresh" (`"refresh"` icon), ellipsis menu (`"ellipsis"` icon).
- Workspace name truncated with ellipsis if too long.
- Clicking the workspace name opens a file dialog to change workspace.
- Each action icon: 20px, hover highlight, tooltip.
- Background: `resolve("sidebarSectionHeader.background")`.
- Bottom border: 1px `ThemeColorToken::BorderLight`.

**Files Affected:**
- `src/ui/SidebarHeader.h` (new)
- `src/ui/SidebarHeader.cpp` (new)

**Acceptance Criteria:**
- Header shows the workspace name.
- Action icons are interactive with hover state.
- Long workspace names are truncated.

**Dependencies:** Phase 02, Phase 01.

---

### Task 2: Implement the PanelHeader Component

**Description:** Create a reusable panel header that appears at the top of each sidebar panel, showing the panel title and panel-specific action icons.

**Key Implementation Details:**
- Class: `PanelHeader` in `src/ui/PanelHeader.h`
- Replaces the current `SidebarToolbar` (which only has text title and basic actions).
- Height: 28px.
- Content (left to right):
  - Collapse/expand chevron (`"chevron-down"` / `"chevron-right"`).
  - Panel title text (semi-bold, uppercase, small font).
  - Badge (if applicable, e.g., item count).
  - Spacer.
  - Action icons (panel-specific).
- Clicking the chevron or title collapses/expands the panel content.
- Clicking an action icon triggers its callback.
- Each panel provides its own set of action icons via a configuration struct.
- Example actions per panel:
  - Explorer: New File, New Folder, Collapse All, Refresh.
  - Search: Clear, Toggle Regex, Toggle Case, Toggle Word.
  - Extensions: Filter, Sort, Refresh.

**Files Affected:**
- `src/ui/PanelHeader.h` (new)
- `src/ui/PanelHeader.cpp` (new)

**Acceptance Criteria:**
- Each panel shows its own header with title and actions.
- Clicking the chevron collapses the panel content.
- Action icons have hover states and tooltips.

**Dependencies:** Phase 02 Task 12.

---

### Task 3: Implement SidebarSection Component

**Description:** Create a collapsible section container that panels can use to organize their content into groups. For example, the Explorer panel has sections: "Open Editors", "Files", "Outline".

**Key Implementation Details:**
- Class: `SidebarSection` in `src/ui/SidebarSection.h`
- Constructor: `SidebarSection(wxWindow* parent, const std::string& title, bool initially_expanded = true)`.
- Contains a `PanelHeader` at the top and a content area below.
- When collapsed, only the header is visible (content height = 0).
- Collapse/expand uses `TransitionManager::start("panel-hide" / "panel-show")` for smooth animation.
- Sections within a panel are stacked vertically.
- Drag-to-reorder sections (future enhancement stub).
- Each section can have its own action icons.

**Files Affected:**
- `src/ui/SidebarSection.h` (new)
- `src/ui/SidebarSection.cpp` (new)

**Acceptance Criteria:**
- Sections collapse and expand with smooth animation.
- Collapsed sections show only the header.
- Multiple sections stack correctly.

**Dependencies:** Tasks 2, Phase 04 Task 5.

---

### Task 4: Implement Smooth Panel Transition Animation

**Description:** When the user switches sidebar modes (e.g., Explorer to Search), animate the outgoing panel fading out and the incoming panel fading in, creating a smooth visual transition.

**Key Implementation Details:**
- Transition sequence:
  1. Cross-fade: outgoing panel opacity 1.0 -> 0.0 over 150ms.
  2. Simultaneously, incoming panel opacity 0.0 -> 1.0 over 150ms.
  3. Optionally, slight vertical slide: outgoing slides up 8px, incoming slides down 8px.
- Use `TransitionManager` for orchestration.
- During transition, both panels are temporarily visible (overlapping).
- After transition, the old panel is hidden (`wxWindow::Hide()`).
- Replaces the current `sidebar_transition_timer_` / `sidebar_transition_alpha_` system.

**Files Affected:**
- `src/ui/LayoutManager.cpp` (modify -- replace panel switching logic)

**Acceptance Criteria:**
- Switching panels shows a smooth cross-fade.
- No flicker or blank frame during the transition.
- Reduced motion: instant switch, no animation.

**Dependencies:** Phase 04 Task 5.

---

### Task 5: Implement Themed Custom Scrollbar

**Description:** Create a custom-painted scrollbar for sidebar panels that matches the theme, replacing the native OS scrollbar which may not integrate visually with the dark theme.

**Key Implementation Details:**
- Class: `ThemedScrollbar` in `src/ui/ThemedScrollbar.h`
- Orientation: vertical (sidebar content scrolls vertically).
- Width: 10px (hover), 6px (idle), animated transition.
- Track color: `resolve("scrollbar.track")` (usually transparent or very subtle).
- Thumb color: `resolve("scrollbar.thumb")`.
- Thumb hover: `resolve("scrollbar.thumbHover")`.
- Thumb min height: 30px.
- Behavior: auto-hide when not scrolling (fade out after 1.5s inactivity).
- Smooth scroll tracking: thumb position interpolated during scroll.
- Overlays on content (does not take up layout space).
- Mouse interaction: click track to page, drag thumb to scroll.

**Files Affected:**
- `src/ui/ThemedScrollbar.h` (new)
- `src/ui/ThemedScrollbar.cpp` (new)

**Acceptance Criteria:**
- Scrollbar renders with theme colors.
- Auto-hides when not scrolling.
- Expands on hover.
- Dragging thumb scrolls the content.

**Dependencies:** Phase 03, Phase 04.

---

### Task 6: Implement Sidebar Skeleton Loading State

**Description:** When a panel's content is loading asynchronously (e.g., file tree scanning, extension list loading), show a skeleton loading placeholder.

**Key Implementation Details:**
- Use `SkeletonRenderer` from Phase 04.
- Explorer panel loading: show tree-like skeleton (indented lines).
- Search panel loading: show result-like skeleton (icon + text lines).
- Extensions panel loading: show card-like skeletons.
- Each panel's `GetOrCreate` factory shows the skeleton on first render, then replaces with real content.
- Skeleton is displayed for a minimum of 200ms to prevent flash.

**Files Affected:**
- `src/ui/FileTreeCtrl.cpp` (modify -- show skeleton during scan)
- `src/ui/ExtensionsBrowserPanel.cpp` (modify -- show skeleton during load)
- `src/ui/LayoutManager.cpp` (modify -- trigger skeleton on panel creation)

**Acceptance Criteria:**
- Opening the sidebar shows a skeleton before content loads.
- Skeleton matches the expected content layout.
- Skeleton is visible for at least 200ms.

**Dependencies:** Phase 04 Task 6.

---

### Task 7: Implement Explorer Panel Section Layout

**Description:** Reorganize the Explorer panel into VS Code-style sections: "Open Editors", "Files", "Outline", "Timeline".

**Key Implementation Details:**
- "Open Editors" section: lists currently open files with close buttons.
- "Files" section: the existing `FileTreeCtrl` file tree.
- "Outline" section: heading hierarchy of the active document (from `EditorPanel::GetHeadingSymbols()`).
- "Timeline" section: placeholder for git commit history (stub).
- Each section uses `SidebarSection` for collapse/expand.
- Default: "Open Editors" collapsed, "Files" expanded, "Outline" collapsed.
- Section expand state persisted in Config.

**Files Affected:**
- `src/ui/LayoutManager.cpp` (modify -- restructure explorer panel as sections)
- `src/ui/SidebarPanelRegistry.cpp` (modify -- register sections)

**Acceptance Criteria:**
- Explorer shows all four sections.
- Sections collapse independently.
- Open Editors list shows currently open files.
- Outline shows heading hierarchy of the active file.

**Dependencies:** Tasks 3.

---

### Task 8: Implement Explorer Panel Action Icons

**Description:** Add contextual action icons to the Explorer panel header and file tree items.

**Key Implementation Details:**
- Panel header actions:
  - "New File" (`"add"` icon) -- creates a new untitled file.
  - "New Folder" (`"add"` icon variant) -- creates a new folder in the workspace.
  - "Collapse All" (`"collapse-all"` icon) -- collapses all tree nodes.
  - "Refresh" (`"refresh"` icon) -- re-scans the workspace directory.
- Tree item hover actions (appear on hover):
  - File items: "Open" (click row), "Rename" (pencil icon), "Delete" (trash icon).
  - Folder items: "New File", "New Folder", "Collapse" / "Expand".
- Action icons are 16px, positioned at the right edge of the row.
- Only visible on hover (or focus for keyboard users).

**Files Affected:**
- `src/ui/FileTreeCtrl.h` / `.cpp` (modify -- add hover action icons)
- `src/ui/PanelHeader.cpp` (modify -- configure explorer actions)

**Acceptance Criteria:**
- Header shows New File, New Folder, Collapse All, Refresh icons.
- Hovering a tree item reveals inline action icons.
- Clicking New File creates a new file input in the tree.

**Dependencies:** Tasks 2, Phase 02.

---

### Task 9: Implement Search Panel Enhancements

**Description:** Enhance the Search sidebar panel with result count badges, match highlighting, and action icons.

**Key Implementation Details:**
- Panel header actions: "Clear Results" (`"clear"` icon), "Toggle Regex" (`"regex"` icon), "Toggle Case" (`"case"` icon), "Toggle Word" (`"word"` icon).
- Result list: show file name with match count badge, expandable to show individual match lines.
- Match lines: highlight the matched text with `ThemeColorToken::EditorFindHit`.
- Click a match to navigate to the line in the editor.
- Result count in panel title: "SEARCH: 42 results in 8 files".
- Replace functionality: input field below search, "Replace All" button.

**Files Affected:**
- `src/ui/LayoutManager.cpp` (modify -- configure search panel actions)
- Search panel implementation (may need new `SearchResultsPanel` class)

**Acceptance Criteria:**
- Search results show file + match count.
- Match text is highlighted.
- Clicking navigates to the match in the editor.

**Dependencies:** Tasks 2, 3.

---

### Task 10: Implement Extensions Panel Enhancements

**Description:** Enhance the Extensions sidebar panel with category filtering, sort options, and improved card layout.

**Key Implementation Details:**
- Panel header actions: "Filter" dropdown (`"filter"` icon), "Sort" dropdown (`"sort-asc"` icon), "Refresh" (`"refresh"` icon).
- Filter options: Installed, Enabled, Disabled, Updates Available.
- Sort options: Name, Rating, Install Count, Date.
- Extension cards show: icon, name, author, description preview, install/enable button.
- Update available badge on extension cards.
- "Install" button with loading state.

**Files Affected:**
- `src/ui/ExtensionsBrowserPanel.h` / `.cpp` (modify)
- `src/ui/ExtensionCard.h` / `.cpp` (modify)

**Acceptance Criteria:**
- Filter and sort dropdowns work.
- Extension cards display full information.
- Install button shows loading during installation.

**Dependencies:** Tasks 2, 3.

---

### Task 11: Implement Sidebar Resize Snap-to-Default

**Description:** When the user drags the sidebar resize handle near the default width (within 10px), the sidebar snaps to the default width. This provides a "home" position that is easy to return to.

**Key Implementation Details:**
- Already partially implemented in `ResizeHandle` (Phase 06 Task 2).
- Ensure the snap threshold is 10px.
- Show a subtle visual indicator (brief highlight of the border) when snapping.
- Double-click the resize handle to reset to default width (already in ResizeHandle).

**Files Affected:**
- `src/ui/ResizeHandle.cpp` (modify -- add snap visual feedback)

**Acceptance Criteria:**
- Dragging near 256px snaps to exactly 256px.
- A brief highlight indicates the snap.
- Double-click resets to 256px from any width.

**Dependencies:** Phase 06 Task 2.

---

### Task 12: Implement Sidebar Minimum Width Constraint

**Description:** Enforce minimum sidebar width and provide a "collapse on drag past minimum" behavior.

**Key Implementation Details:**
- Minimum width: 180px from `ZoneConfig`.
- When the user drags the sidebar handle below 120px (below minimum by 60px): collapse the sidebar entirely.
- Visual feedback: when width < 180px during drag, show the sidebar at reduced opacity (0.5) to indicate it will collapse on release.
- On release below threshold: animate collapse.
- On release above minimum: snap to minimum.

**Files Affected:**
- `src/ui/WorkbenchShell.cpp` (modify -- add collapse-on-release behavior)

**Acceptance Criteria:**
- Sidebar cannot be set to a width between 0 and 180px (it is either >= 180px or collapsed).
- Dragging below 120px and releasing collapses the sidebar.
- Visual feedback shows the pending collapse.

**Dependencies:** Phase 06 Tasks 3, 4.

---

### Task 13: Implement Panel-Specific Toolbar Integration

**Description:** Each sidebar panel can provide a custom toolbar that appears below the panel header but above the panel content. For example, Search provides a search input toolbar; Explorer provides a filter input.

**Key Implementation Details:**
- Each panel registers its toolbar configuration when created.
- Toolbar is rendered as a separate band between the panel header and the content area.
- Search toolbar: search input, regex toggle, case toggle, word toggle.
- Explorer toolbar: filter input for filtering the file tree.
- Extensions toolbar: search input for filtering extensions.
- Toolbar height: 32px.
- Toolbar visibility: toggleable per panel (e.g., explorer filter shown on Ctrl+F within sidebar).

**Files Affected:**
- `src/ui/SidebarSection.h` (modify -- support toolbar slot)
- `src/ui/LayoutManager.cpp` (modify -- configure per-panel toolbars)

**Acceptance Criteria:**
- Search panel shows its search toolbar.
- Explorer filter can be toggled with Ctrl+F when sidebar is focused.
- Toolbar is correctly themed.

**Dependencies:** Tasks 2, 3.

---

### Task 14: Implement Sidebar Panel Context Menu

**Description:** Right-clicking in the sidebar panel header shows a context menu for panel management.

**Key Implementation Details:**
- Context menu items:
  - Panel-specific actions (e.g., "New File" in Explorer).
  - "Move to Secondary Sidebar" -- moves this panel to the right sidebar.
  - "Close Panel" -- hides this panel.
  - "Reset Sections" -- restores default section expand/collapse state.

**Files Affected:**
- `src/ui/PanelHeader.cpp` (modify -- add right-click handler)

**Acceptance Criteria:**
- Right-click shows the context menu.
- "Move to Secondary Sidebar" moves the panel.
- Context menu is themed.

**Dependencies:** Tasks 2, Phase 06 Task 11.

---

### Task 15: Implement Sidebar Panel Drag Between Primary and Secondary

**Description:** Allow users to drag panels between the primary and secondary sidebars.

**Key Implementation Details:**
- Drag initiated from the panel header.
- During drag: show a ghost panel indicator at the cursor.
- Drop targets: primary sidebar, secondary sidebar.
- On drop: move the panel to the target sidebar's registry.
- Update activity bar to reflect the move (panel icon shows in the correct sidebar's activity section).
- Animated panel insertion at the target.

**Files Affected:**
- `src/ui/PanelHeader.cpp` (modify -- drag initiation)
- `src/ui/WorkbenchShell.cpp` (modify -- drop target handling)
- `src/ui/LayoutManager.cpp` (modify -- panel registry management)

**Acceptance Criteria:**
- Dragging a panel header to the secondary sidebar moves it there.
- Dragging back to the primary sidebar restores it.
- Panel content and state are preserved during the move.

**Dependencies:** Tasks 2, Phase 06 Task 11.

---

### Task 16: Implement Sidebar Scroll Behavior

**Description:** Ensure sidebar panels scroll correctly with the themed scrollbar when content exceeds the visible area.

**Key Implementation Details:**
- Each panel content area is wrapped in a scroll container.
- `ThemedScrollbar` overlays the content.
- Scroll position is preserved when switching panels (each panel remembers its scroll position).
- Smooth scrolling via `ScrollAnimator` (Phase 04).
- Scroll-to-reveal: when a tree item is selected via keyboard, auto-scroll to make it visible.

**Files Affected:**
- `src/ui/SidebarSection.cpp` (modify -- add scroll container)
- `src/ui/FileTreeCtrl.cpp` (modify -- scroll-to-reveal)

**Acceptance Criteria:**
- Long file trees scroll with the themed scrollbar.
- Scroll position is preserved across panel switches.
- Keyboard navigation auto-scrolls to the focused item.

**Dependencies:** Tasks 5, Phase 04 Task 18.

---

### Task 17: Implement Empty State for Panels

**Description:** When a panel has no content (e.g., no workspace open in Explorer, no search results), show a helpful empty state with an icon, message, and action button.

**Key Implementation Details:**
- Explorer empty state: folder icon + "Open a Folder" text + "Open Folder" button.
- Search empty state: search icon + "Type to search" text.
- Extensions empty state: extension icon + "No extensions installed" + "Browse Extensions" button.
- Empty state is centered vertically and horizontally in the panel area.
- Text uses `TypographyScale::font(kSubtitle)`.
- Icon is 48px, muted color.

**Files Affected:**
- `src/ui/FileTreeCtrl.cpp` (modify -- add empty state)
- `src/ui/ExtensionsBrowserPanel.cpp` (modify -- add empty state)

**Acceptance Criteria:**
- Empty explorer shows the Open Folder prompt.
- Clicking the action button triggers the expected action.
- Empty state is correctly themed.

**Dependencies:** Phase 02.

---

### Task 18: Implement Sidebar Width Memory Per Panel

**Description:** Allow different sidebar panels to have different preferred widths, restored when switching panels.

**Key Implementation Details:**
- Store a `preferred_width` per `SidebarMode` in `ZoneManager`.
- When switching panels, if the new panel has a stored preferred width, resize the sidebar to that width.
- If no preference stored, use the current width.
- User can set preferred width by resizing the sidebar while a panel is active (auto-saved).
- Config key: `layout.sidebar.width.<mode_name>`.

**Files Affected:**
- `src/ui/ZoneManager.h` (modify -- per-panel width storage)
- `src/ui/ZoneManager.cpp` (modify)
- `src/ui/LayoutManager.cpp` (modify -- apply panel-specific width on switch)

**Acceptance Criteria:**
- Resizing the sidebar while Explorer is active saves the width for Explorer.
- Switching to Search restores Search's preferred width.
- Widths persist across restarts.

**Dependencies:** Phase 06 Task 1.

---

### Task 19: Implement Sidebar Header Breadcrumb Mode

**Description:** Optionally show the sidebar header as a breadcrumb trail instead of a static workspace name, showing the navigation path within the current panel.

**Key Implementation Details:**
- For Explorer: show current folder depth (e.g., "workspace > src > ui").
- For Search: show search scope (e.g., "workspace > *.cpp").
- Breadcrumb segments are clickable to navigate up.
- Toggle between breadcrumb and simple header via a setting.
- Default: simple header (VS Code style).

**Files Affected:**
- `src/ui/SidebarHeader.cpp` (modify -- add breadcrumb mode)

**Acceptance Criteria:**
- Breadcrumb mode shows the navigation path.
- Clicking a segment navigates to that level.
- Simple mode shows the workspace name.

**Dependencies:** Task 1.

---

### Task 20: Implement Sidebar Footer Status Area

**Description:** Add an optional footer at the bottom of the sidebar showing contextual status information (file count, selected item count, etc.).

**Key Implementation Details:**
- Height: 22px (same as status bar).
- Content varies by panel:
  - Explorer: "42 files, 8 folders".
  - Search: "12 results in 4 files".
  - Extensions: "8 installed, 2 updates".
- Text uses `TypographyScale::font(kCaption)`.
- Color: `resolve("sidebar.foreground")` at muted opacity.
- Can be hidden via setting.

**Files Affected:**
- `src/ui/SidebarFooter.h` (new)
- `src/ui/SidebarFooter.cpp` (new)

**Acceptance Criteria:**
- Footer shows contextual information per panel.
- Footer updates when content changes.
- Can be hidden via setting.

**Dependencies:** Phase 01.

---

### Task 21: Deprecate Old SidebarToolbar

**Description:** Mark the existing `SidebarToolbar` class as deprecated now that `PanelHeader` and `SidebarHeader` replace its functionality.

**Key Implementation Details:**
- Add `[[deprecated("Use PanelHeader and SidebarHeader from Phase 08")]]` to `SidebarToolbar`.
- Ensure no code creates new instances of `SidebarToolbar`.
- Keep the file for one version cycle.

**Files Affected:**
- `src/ui/SidebarToolbar.h` (modify -- add deprecation)

**Acceptance Criteria:**
- No new instances of SidebarToolbar are created.
- Deprecation warnings appear during compilation.

**Dependencies:** Tasks 1, 2.

---

### Task 22: Accessibility for Sidebar Components

**Description:** Ensure all new sidebar components are fully accessible.

**Key Implementation Details:**
- `SidebarHeader`: role `kToolbar`, label "Sidebar Header".
- `PanelHeader`: role `kButton` (collapse/expand behavior), label includes panel name and expand state.
- `SidebarSection`: role `kTreeItem` or `kTabPanel`, announce expand/collapse state.
- Action icons: role `kButton`, label from tooltip text.
- `ThemedScrollbar`: aria scrollbar semantics (value-now, value-min, value-max).
- Empty states: announce message text to screen reader on panel switch.

**Files Affected:**
- All new files from this phase (add accessibility annotations)

**Acceptance Criteria:**
- Screen reader correctly reads all sidebar elements.
- Keyboard navigation covers all interactive elements.
- Expand/collapse state changes are announced.

**Dependencies:** Phase 05 Task 7.

---

### Task 23: Sidebar Keyboard Shortcuts

**Description:** Add keyboard shortcuts for common sidebar operations.

**Key Implementation Details:**
- `Ctrl+Shift+E`: focus Explorer panel in sidebar.
- `Ctrl+Shift+F`: focus Search panel in sidebar.
- `Ctrl+Shift+X`: focus Extensions panel in sidebar.
- `Ctrl+Shift+G`: focus Graph/Source Control panel.
- `Ctrl+B`: toggle primary sidebar visibility.
- `Ctrl+\`: focus sidebar (if visible).
- Arrow keys: navigate within the active panel (tree items, list items, sections).
- Enter: activate/expand the focused item.
- Escape: return focus from sidebar to editor.

**Files Affected:**
- `src/ui/MainFrame.cpp` (modify -- register shortcuts)
- `src/core/ShortcutManager.cpp` (modify)

**Acceptance Criteria:**
- All listed shortcuts work.
- Focus moves correctly between sidebar and editor.

**Dependencies:** Phase 05 Task 20.

---

### Task 24: Sidebar Panel State Persistence

**Description:** Persist the expanded/collapsed state of sections, scroll positions, and filter text across panel switches and app restarts.

**Key Implementation Details:**
- Per-panel state saved to Config:
  - `sidebar.<mode>.sections.<section_name>.expanded` -- bool.
  - `sidebar.<mode>.scroll_position` -- int.
  - `sidebar.<mode>.filter_text` -- string.
- State saved on section collapse/expand, scroll stop, and filter change (debounced).
- State restored when the panel is activated.

**Files Affected:**
- `src/ui/SidebarSection.cpp` (modify -- persist expand state)
- `src/ui/LayoutManager.cpp` (modify -- persist scroll and filter)

**Acceptance Criteria:**
- Collapsing a section and switching panels preserves the collapsed state.
- Scroll position is restored on panel re-activation.
- State persists across app restarts.

**Dependencies:** Task 3.

---

### Task 25: Catch2 Test Suite for Primary Sidebar

**Description:** Write comprehensive tests for all sidebar components.

**Key Implementation Details:**
- Test file: `tests/unit/test_primary_sidebar.cpp`
- Test target: `test_primary_sidebar` in CMakeLists.txt
- Test sections:
  - `PanelHeader`: title, action icons, collapse state toggle.
  - `SidebarSection`: expand/collapse, nested sections.
  - `SidebarHeader`: workspace name display, truncation.
  - Panel transition: correct panel shown after switch.
  - Scroll position preservation.
  - Empty state: shown when no content, hidden when content loads.
  - Per-panel width memory.
  - State persistence: save and restore round-trip.
  - Context menu: move to secondary sidebar.

**Files Affected:**
- `tests/unit/test_primary_sidebar.cpp` (new)
- `CMakeLists.txt` (modify)

**Acceptance Criteria:**
- All tests pass.
- At least 35 test assertions.

**Dependencies:** All previous tasks.

---

## Files Created

| File | Type |
|------|------|
| `src/ui/SidebarHeader.h` / `.cpp` | Header component |
| `src/ui/PanelHeader.h` / `.cpp` | Panel header |
| `src/ui/SidebarSection.h` / `.cpp` | Collapsible section |
| `src/ui/ThemedScrollbar.h` / `.cpp` | Custom scrollbar |
| `src/ui/SidebarFooter.h` / `.cpp` | Footer status |
| `tests/unit/test_primary_sidebar.cpp` | Test file |

## Files Modified

| File | Change |
|------|--------|
| `src/ui/SidebarToolbar.h` | Deprecation attribute |
| `src/ui/LayoutManager.cpp` | Panel sections, transitions, state |
| `src/ui/FileTreeCtrl.h` / `.cpp` | Sections, hover actions, empty state |
| `src/ui/ExtensionsBrowserPanel.h` / `.cpp` | Skeleton, empty state, filter |
| `src/ui/ExtensionCard.h` / `.cpp` | Enhanced card layout |
| `src/ui/ResizeHandle.cpp` | Snap visual feedback |
| `src/ui/WorkbenchShell.cpp` | Collapse-on-drag behavior |
| `src/ui/ZoneManager.h` / `.cpp` | Per-panel width storage |
| `src/ui/MainFrame.cpp` | Sidebar shortcuts |
| `CMakeLists.txt` | New sources, test target |
