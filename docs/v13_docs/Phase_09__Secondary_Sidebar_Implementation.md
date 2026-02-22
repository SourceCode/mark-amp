# Phase 09 -- Secondary Sidebar Implementation

## Objective

Implement a fully independent secondary sidebar on the right side of the editor area. The secondary sidebar can host any panel that the primary sidebar supports (Outline, Search, Graph, etc.) but operates independently -- it has its own panel selection, its own resize behavior, its own collapsed state, and its own scroll positions. This mirrors VS Code's secondary sidebar feature, providing users with a two-panel workflow (e.g., Explorer on the left, Outline on the right).

## Prerequisites

- Phase 06 (Workbench Shell) -- for `kSecondarySidebar` zone, `ZoneManager`, `ResizeHandle`.
- Phase 07 (Activity Bar) -- for activity bar item mapping to sidebar panels.
- Phase 08 (Primary Sidebar) -- for shared components (`PanelHeader`, `SidebarSection`, `ThemedScrollbar`).

## Deliverables

- Fully functional secondary sidebar zone.
- Mini activity bar or tab strip for secondary sidebar panel selection.
- Independent panel registry, scroll state, and width persistence.
- Panel drag between primary and secondary sidebars.
- Catch2 test target: `test_secondary_sidebar`.

## Estimated Complexity

Medium -- leverages shared components from Phase 08, primarily integration and wiring work.

---

## Tasks

### Task 1: Implement Secondary Sidebar Panel Registry

**Description:** Create an independent `SidebarPanelRegistry` for the secondary sidebar. It can register the same panel types as the primary sidebar but maintains its own panel instances.

**Key Implementation Details:**
- The existing `secondary_panel_registry_` in `LayoutManager` becomes the backing store.
- Register default panels: Outline, Search, Graph (configurable).
- Each panel in the secondary registry gets its own independent instance (not shared with primary).
- Factory functions create separate wxPanel instances when panels are first accessed.
- Method: `auto secondary_sidebar_registry() -> SidebarPanelRegistry&`.

**Files Affected:**
- `src/ui/LayoutManager.h` (modify -- ensure secondary registry is properly initialized)
- `src/ui/LayoutManager.cpp` (modify -- register panels in secondary registry)

**Acceptance Criteria:**
- Secondary sidebar has its own panel instances.
- Switching the primary sidebar's panel does not affect the secondary sidebar.
- All panel types can be registered in both registries.

**Dependencies:** Phase 08 Task 3.

---

### Task 2: Implement Mini Tab Strip for Panel Selection

**Description:** Create a compact tab strip at the top of the secondary sidebar for switching between its hosted panels. Unlike the primary sidebar which uses the activity bar, the secondary sidebar uses a compact horizontal tab strip.

**Key Implementation Details:**
- Class: `MiniTabStrip` in `src/ui/MiniTabStrip.h`
- Height: 28px.
- Each tab: icon (16px) + short label, horizontally arranged.
- Active tab: underline indicator (2px) using accent color.
- Inactive tabs: muted foreground.
- Click to switch panels.
- Right-click: context menu with "Close Panel", "Move to Primary Sidebar".
- Overflow: if too many tabs, show a "..." overflow dropdown.
- Background: `resolve("sidebar.background")`.

**Files Affected:**
- `src/ui/MiniTabStrip.h` (new)
- `src/ui/MiniTabStrip.cpp` (new)

**Acceptance Criteria:**
- Tab strip shows all registered secondary panels.
- Clicking a tab switches the panel content.
- Active tab has an underline indicator.
- Overflow dropdown appears when tabs do not fit.

**Dependencies:** Phase 02, Phase 01.

---

### Task 3: Wire Secondary Sidebar into WorkbenchShell

**Description:** Populate the `kSecondarySidebar` zone in the `WorkbenchShell` with the mini tab strip and panel content area.

**Key Implementation Details:**
- Zone layout (top to bottom):
  1. `MiniTabStrip` (28px).
  2. Panel content area (flex fill, from secondary registry).
- Left border: 1px `ThemeColorToken::BorderDark`.
- Background: `resolve("sidebar.background")` (shared with primary by default, but independently overridable).
- Initially hidden (secondary sidebar is off by default).
- Toggle via: `Ctrl+Alt+B`, Command Palette "View: Toggle Secondary Sidebar".

**Files Affected:**
- `src/ui/LayoutManager.cpp` (modify -- populate secondary sidebar zone)
- `src/ui/WorkbenchShell.cpp` (modify -- ensure zone content is rendered)

**Acceptance Criteria:**
- Secondary sidebar appears to the right of the editor.
- Tab strip and panel content render correctly.
- Toggle command shows/hides with animation.

**Dependencies:** Tasks 1, 2; Phase 06 Task 11.

---

### Task 4: Implement Independent Resize Behavior

**Description:** The secondary sidebar has its own resize handle on its left edge, independent from the primary sidebar. The resize handle is already placed by WorkbenchShell (Phase 06 Task 14); this task ensures the resize behavior works correctly.

**Key Implementation Details:**
- Resize handle between editor area and secondary sidebar.
- Width range: 180-400px (from `ZoneConfig`).
- Snap-to-default: 256px.
- Collapse-on-drag: same behavior as primary sidebar (drag past 120px below minimum).
- Width persisted separately: `layout.secondary_sidebar.width`.
- Resizing the secondary sidebar does not affect the primary sidebar (editor area absorbs the change).

**Files Affected:**
- `src/ui/ZoneManager.cpp` (ensure secondary sidebar config is correct)
- `src/ui/WorkbenchShell.cpp` (verify resize handle wiring)

**Acceptance Criteria:**
- Secondary sidebar resizes independently.
- Resizing shrinks/grows the editor area, not the primary sidebar.
- Width is persisted and restored.

**Dependencies:** Phase 06 Tasks 2, 14.

---

### Task 5: Implement Panel Selection Synchronization with Activity Bar

**Description:** When the secondary sidebar is visible, the activity bar should indicate which panels are hosted where. Optionally, add a visual cue (e.g., a small right-side dot) on activity bar items that are in the secondary sidebar.

**Key Implementation Details:**
- Each activity bar item can be annotated with its sidebar location: `kPrimary`, `kSecondary`, or `kBoth` (if duplicated).
- Items in the secondary sidebar show a small dot indicator on the right side of the icon.
- Clicking an activity bar item that is assigned to the secondary sidebar focuses the secondary sidebar.
- Ctrl+Click on an activity bar item moves it between primary and secondary.
- This is optional and can be disabled in settings.

**Files Affected:**
- `src/ui/ActivityBar.cpp` (modify -- add secondary sidebar dot indicator)
- `src/ui/ActivityBarModel.h` (modify -- add sidebar_location field)

**Acceptance Criteria:**
- Items in the secondary sidebar show a small dot on the right.
- Ctrl+Click moves a panel between sidebars.
- Visual indicator is themed.

**Dependencies:** Phase 07 Task 1.

---

### Task 6: Implement Panel Drag-Drop Between Sidebars

**Description:** Allow users to drag a panel from the mini tab strip or panel header between the primary and secondary sidebars.

**Key Implementation Details:**
- Drag source: `MiniTabStrip` tab or `PanelHeader` in secondary sidebar.
- Drag target: primary sidebar zone or secondary sidebar zone.
- During drag:
  - Show ghost panel at cursor.
  - Highlight the target sidebar zone.
  - Show an insertion indicator.
- On drop:
  - Unregister panel from source registry.
  - Register panel in target registry.
  - Activate the panel in the target sidebar.
- If the secondary sidebar becomes empty after the last panel is dragged away, collapse it.

**Files Affected:**
- `src/ui/MiniTabStrip.cpp` (modify -- add drag initiation)
- `src/ui/PanelHeader.cpp` (modify -- add drag initiation for secondary sidebar panels)
- `src/ui/WorkbenchShell.cpp` (modify -- handle drop targets)
- `src/ui/LayoutManager.cpp` (modify -- manage panel registry moves)

**Acceptance Criteria:**
- Dragging a tab from secondary to primary moves the panel.
- Empty secondary sidebar collapses automatically.
- Panel state (scroll position, filter) is preserved during move.

**Dependencies:** Phase 08 Task 15.

---

### Task 7: Implement Independent Scroll State

**Description:** Each panel in the secondary sidebar maintains its own scroll position, independent of the same panel type in the primary sidebar.

**Key Implementation Details:**
- Since panels in the secondary sidebar are separate instances (Task 1), they naturally have independent scroll state.
- Ensure scroll position is persisted per panel per sidebar:
  - `sidebar.secondary.<mode>.scroll_position`.
- Restore scroll position when switching panels in the secondary sidebar.

**Files Affected:**
- `src/ui/LayoutManager.cpp` (modify -- persist secondary sidebar scroll state)

**Acceptance Criteria:**
- Scrolling Search results in the secondary sidebar does not affect the primary sidebar's Search.
- Scroll position persists across panel switches.
- Scroll position persists across app restarts.

**Dependencies:** Phase 08 Task 24.

---

### Task 8: Implement Default Panel Assignment

**Description:** Define which panels appear in the secondary sidebar by default and allow user customization.

**Key Implementation Details:**
- Default secondary sidebar panels: Outline, Search (can be configured).
- User can add/remove panels via:
  - Context menu on `MiniTabStrip`: "Add Panel..." with a submenu of available panels.
  - Drag-drop from primary sidebar.
  - Command Palette: "View: Move [Panel] to Secondary Sidebar".
- Configuration stored in Config:
  - `layout.secondary_sidebar.panels` -- JSON array of panel mode IDs.
  - `layout.secondary_sidebar.active_panel` -- currently active panel ID.

**Files Affected:**
- `src/ui/LayoutManager.cpp` (modify -- load/save secondary sidebar panel list)

**Acceptance Criteria:**
- Default panels appear in the secondary sidebar on first launch.
- User-added panels persist across restarts.
- Removing all panels collapses the secondary sidebar.

**Dependencies:** Tasks 1, 2.

---

### Task 9: Implement "Open in Secondary Sidebar" Command

**Description:** Add Command Palette commands for managing the secondary sidebar.

**Key Implementation Details:**
- Commands:
  - "View: Toggle Secondary Sidebar" (`Ctrl+Alt+B`).
  - "View: Move Active Panel to Secondary Sidebar" -- moves the current primary sidebar panel.
  - "View: Move Active Panel to Primary Sidebar" -- moves the current secondary sidebar panel.
  - "View: Focus Secondary Sidebar" (`Ctrl+Alt+0`).
  - "View: Open Outline in Secondary Sidebar".
  - "View: Open Search in Secondary Sidebar".

**Files Affected:**
- `src/ui/MainFrame.cpp` (modify -- register commands)

**Acceptance Criteria:**
- All commands work as described.
- Toggle command shows/hides the sidebar with animation.
- Move commands transfer the panel between sidebars.

**Dependencies:** Tasks 3, 6.

---

### Task 10: Implement Linked Scroll Mode (Optional)

**Description:** Optionally link the scroll position of a panel in the primary and secondary sidebars (e.g., two different views of the same file tree scrolling together). This is an advanced feature for power users.

**Key Implementation Details:**
- Toggle via panel header action icon: "Link Scroll" (`"lock"` icon).
- When enabled, scrolling one panel scrolls the other.
- Only works for panels of the same type in both sidebars.
- Uses EventBus to publish scroll events between linked panels.
- Default: off.

**Files Affected:**
- `src/ui/PanelHeader.cpp` (modify -- add link scroll action)
- `src/core/Events.h` (modify -- add LinkedScrollEvent)

**Acceptance Criteria:**
- Enabling linked scroll syncs the two panels.
- Disabling stops the sync.
- Only same-type panels can be linked.

**Dependencies:** Task 7.

---

### Task 11: Implement Secondary Sidebar in Zen Mode

**Description:** Ensure the secondary sidebar is correctly hidden in Zen Mode and restored when exiting.

**Key Implementation Details:**
- Zen Mode hides the secondary sidebar (already covered by Phase 06 Task 15).
- The secondary sidebar's state (visibility, active panel) is saved before entering Zen Mode.
- On exit, the state is restored.
- Verify this works with the two-sidebar layout presets.

**Files Affected:**
- `src/ui/LayoutPresetManager.cpp` (verify Zen Mode includes secondary sidebar handling)

**Acceptance Criteria:**
- Zen Mode hides the secondary sidebar.
- Exiting Zen Mode restores the secondary sidebar if it was visible before.

**Dependencies:** Phase 06 Task 15.

---

### Task 12: Implement Secondary Sidebar in Layout Presets

**Description:** Add the secondary sidebar to relevant layout presets and create a "Two Sidebars" preset.

**Key Implementation Details:**
- "Default": secondary sidebar hidden.
- "Two Sidebars": primary (256px, Explorer) + secondary (256px, Outline) visible.
- "Compact": secondary sidebar hidden.
- "Panel Focus": secondary sidebar hidden.
- Custom presets capture secondary sidebar state.

**Files Affected:**
- `src/ui/LayoutPresetManager.cpp` (modify)

**Acceptance Criteria:**
- "Two Sidebars" preset shows both sidebars.
- "Default" hides the secondary sidebar.

**Dependencies:** Phase 06 Task 5.

---

### Task 13: Implement Secondary Sidebar Accessibility

**Description:** Ensure the secondary sidebar is fully accessible with correct focus zone registration, keyboard navigation, and screen reader support.

**Key Implementation Details:**
- Register `FocusZoneId::kSecondarySidebar` in `FocusManager`.
- Tab traversal order: ... EditorArea -> SecondarySidebar -> PanelArea ...
- Within the secondary sidebar:
  - Tab strip tabs are focusable.
  - Panel content items follow the same keyboard patterns as primary sidebar.
- Screen reader announces "Secondary Sidebar" when focus enters the zone.
- `MiniTabStrip` tabs: role `kTab`, labels from panel names.

**Files Affected:**
- `src/ui/MiniTabStrip.cpp` (modify -- add accessibility)
- `src/ui/LayoutManager.cpp` (modify -- register focus zone)

**Acceptance Criteria:**
- Tab key traversal includes the secondary sidebar when visible.
- Screen reader announces the zone name.
- Tab strip tabs are keyboard navigable.

**Dependencies:** Phase 05 Tasks 1, 8.

---

### Task 14: Implement Panel Content Height Constraints

**Description:** When both primary and secondary sidebars are visible and the window is narrow, ensure that both sidebars and the editor area maintain usable widths.

**Key Implementation Details:**
- Constraint validation in `WorkbenchShell`:
  - Total width = activity_bar + primary_sidebar + editor_area + secondary_sidebar.
  - If total exceeds window width: shrink secondary sidebar first, then primary.
  - If still too narrow: collapse secondary sidebar entirely.
  - If still too narrow: collapse primary sidebar.
  - Editor area minimum: 200px.
- Show a warning notification if the window is too narrow for the two-sidebar layout.

**Files Affected:**
- `src/ui/WorkbenchShell.cpp` (modify -- enhance constraint validation)

**Acceptance Criteria:**
- Narrow windows collapse the secondary sidebar first.
- Very narrow windows collapse both sidebars.
- Editor area never goes below 200px.

**Dependencies:** Phase 06 Task 18.

---

### Task 15: Implement Secondary Sidebar Empty State

**Description:** When the secondary sidebar is visible but has no panels assigned, show an empty state with guidance.

**Key Implementation Details:**
- Empty state content:
  - Icon: `"split-vertical"` at 48px.
  - Text: "No panels in secondary sidebar".
  - Action: "Drag a panel here or use the context menu to add one".
  - Button: "Add Panel..." that opens a panel picker.
- Empty state is centered in the secondary sidebar area.

**Files Affected:**
- `src/ui/LayoutManager.cpp` (modify -- show empty state when no panels)

**Acceptance Criteria:**
- Empty secondary sidebar shows the guidance message.
- "Add Panel" button opens a picker.
- Adding a panel replaces the empty state.

**Dependencies:** Phase 08 Task 17.

---

### Task 16: Implement Panel Duplication Detection

**Description:** Warn users when they try to add a panel that already exists in the other sidebar. Allow it but show an informational notification.

**Key Implementation Details:**
- When adding a panel to the secondary sidebar that exists in the primary:
  - Show notification: "Search panel is also open in the primary sidebar. They operate independently."
  - Proceed with the add (independent instances).
- When moving a panel (not duplicating):
  - Remove from source, add to target -- no warning needed.

**Files Affected:**
- `src/ui/LayoutManager.cpp` (modify -- add duplication check on panel add)

**Acceptance Criteria:**
- Duplicating a panel shows an informational notification.
- Moving a panel does not show a warning.
- Both instances operate independently.

**Dependencies:** Task 1.

---

### Task 17: Implement Mini Tab Strip Drag Reorder

**Description:** Allow users to reorder tabs in the secondary sidebar's mini tab strip by dragging.

**Key Implementation Details:**
- Drag initiation: mouse down on tab + move > 5px threshold.
- During drag: show the tab at cursor position, show insertion indicator between tabs.
- On drop: reorder tabs in the panel list.
- Persist order in Config.

**Files Affected:**
- `src/ui/MiniTabStrip.cpp` (modify -- add drag reorder)

**Acceptance Criteria:**
- Dragging tabs reorders them.
- Order persists across restarts.

**Dependencies:** Task 2.

---

### Task 18: Implement Close Tab on Mini Tab Strip

**Description:** Each tab in the mini tab strip has a close button that removes the panel from the secondary sidebar.

**Key Implementation Details:**
- Close button: 14px "X" icon, visible on hover.
- Clicking close: removes the panel from the secondary registry.
- If the closed panel was active, activate the next panel.
- If no panels remain, collapse the secondary sidebar.
- Undo: show notification "Panel removed" with "Undo" button (re-adds the panel).

**Files Affected:**
- `src/ui/MiniTabStrip.cpp` (modify -- add close button)

**Acceptance Criteria:**
- Close button appears on hover.
- Closing removes the panel.
- Closing the last panel collapses the sidebar.
- Undo notification works.

**Dependencies:** Task 2.

---

### Task 19: Implement Secondary Sidebar Width Per Panel

**Description:** Just like the primary sidebar (Phase 08 Task 18), allow different panels in the secondary sidebar to have different preferred widths.

**Key Implementation Details:**
- Store `preferred_width` per panel mode in the secondary sidebar config.
- Switch width when switching panels.
- Config key: `layout.secondary_sidebar.width.<mode_name>`.

**Files Affected:**
- `src/ui/ZoneManager.cpp` (modify -- add secondary sidebar per-panel widths)

**Acceptance Criteria:**
- Different panels can have different widths.
- Width is restored when switching back to a panel.

**Dependencies:** Phase 08 Task 18.

---

### Task 20: Implement Event Coordination Between Sidebars

**Description:** Ensure certain events from the primary sidebar properly coordinate with the secondary sidebar and vice versa.

**Key Implementation Details:**
- File selection in primary Explorer: if Outline is in secondary sidebar, update the outline for the selected file.
- Tab switch in editor: update both sidebars' context-sensitive panels (Outline, Search results navigation).
- Workspace change: reinitialize both sidebar registries.
- Theme change: both sidebars update simultaneously.

**Files Affected:**
- `src/ui/LayoutManager.cpp` (modify -- add cross-sidebar event coordination)

**Acceptance Criteria:**
- Selecting a file in Explorer updates the secondary sidebar's Outline.
- Switching editor tabs updates context panels in both sidebars.
- Theme changes apply to both sidebars.

**Dependencies:** Tasks 1, 3.

---

### Task 21: Implement Sidebar Swap Command

**Description:** Add a command to swap the contents of the primary and secondary sidebars in one action.

**Key Implementation Details:**
- Command Palette: "View: Swap Sidebars".
- Swaps:
  - All registered panels.
  - Active panel selection.
  - Width preferences.
  - Scroll positions.
- Animated: both sidebars cross-fade their content.

**Files Affected:**
- `src/ui/MainFrame.cpp` (modify -- register command)
- `src/ui/LayoutManager.cpp` (modify -- implement swap logic)

**Acceptance Criteria:**
- Swap exchanges all content between sidebars.
- Animation shows the swap visually.
- State is preserved (scroll positions, expand states).

**Dependencies:** Tasks 1, 3.

---

### Task 22: Implement Side-by-Side Outline + Explorer Workflow

**Description:** Optimize the common workflow of having Explorer in the primary sidebar and Outline in the secondary sidebar. When the user opens a file in the Explorer, the Outline panel automatically shows the heading structure of that file.

**Key Implementation Details:**
- When the secondary sidebar contains the Outline panel:
  - Subscribe to `TabSwitchedEvent` in the Outline panel.
  - On tab switch, regenerate the heading outline from the new file.
  - Auto-scroll to the current heading (based on cursor position).
- Clicking a heading in the Outline navigates to that heading in the editor.
- Outline updates in real-time as the user edits (debounced).

**Files Affected:**
- `src/ui/LayoutManager.cpp` (modify -- wire outline to editor events)
- New or existing outline panel implementation.

**Acceptance Criteria:**
- Outline shows headings of the active file.
- Clicking a heading navigates to it.
- Real-time updates as the user types.

**Dependencies:** Task 20.

---

### Task 23: Performance Testing for Dual Sidebar Layout

**Description:** Ensure that running two sidebars with active panels does not degrade rendering performance.

**Key Implementation Details:**
- Profile paint times with both sidebars visible.
- Both sidebars should paint independently (no cross-invalidation).
- Target: each sidebar paints in under 2ms.
- File tree with 1000+ items should not cause lag in either sidebar.
- Throttle sidebar updates during editor typing (debounce at 200ms).

**Files Affected:**
- `src/ui/LayoutManager.cpp` (modify -- add debounce on sidebar content updates)

**Acceptance Criteria:**
- Both sidebars paint in under 2ms each.
- Typing in the editor does not cause sidebar rendering lag.
- 1000-item file tree scrolls smoothly in either sidebar.

**Dependencies:** All previous tasks.

---

### Task 24: Documentation for Two-Sidebar Workflow

**Description:** Add user-facing documentation explaining the two-sidebar workflow and how to configure it.

**Key Implementation Details:**
- Add a section to the user guide covering:
  - How to enable the secondary sidebar.
  - How to add/remove panels.
  - How to drag panels between sidebars.
  - Available keyboard shortcuts.
  - Layout presets that include the secondary sidebar.
- Add tooltip text to the secondary sidebar toggle command.

**Files Affected:**
- `docs/user_guide.md` (modify)

**Acceptance Criteria:**
- Documentation is clear and includes screenshots/diagrams.
- All keyboard shortcuts are documented.
- Layout preset descriptions mention secondary sidebar.

**Dependencies:** All previous tasks.

---

### Task 25: Catch2 Test Suite for Secondary Sidebar

**Description:** Write comprehensive tests for secondary sidebar functionality.

**Key Implementation Details:**
- Test file: `tests/unit/test_secondary_sidebar.cpp`
- Test target: `test_secondary_sidebar` in CMakeLists.txt
- Test sections:
  - Panel registry: independent of primary, register/unregister.
  - MiniTabStrip: tab creation, active tab, overflow.
  - Width persistence: per-panel width storage, restore on switch.
  - Panel duplication: warning when duplicating, independent instances.
  - Layout presets: "Two Sidebars" preset includes secondary.
  - Constraint validation: narrow windows collapse secondary first.
  - Empty state: shown when no panels, hidden when panel added.
  - Drag reorder: tab order preserved after drag.
  - Event coordination: file selection updates both sidebars.

**Files Affected:**
- `tests/unit/test_secondary_sidebar.cpp` (new)
- `CMakeLists.txt` (modify)

**Acceptance Criteria:**
- All tests pass.
- At least 30 test assertions.

**Dependencies:** All previous tasks.

---

## Files Created

| File | Type |
|------|------|
| `src/ui/MiniTabStrip.h` / `.cpp` | Tab strip component |
| `tests/unit/test_secondary_sidebar.cpp` | Test file |

## Files Modified

| File | Change |
|------|--------|
| `src/ui/LayoutManager.h` / `.cpp` | Secondary sidebar wiring, panel management |
| `src/ui/WorkbenchShell.cpp` | Zone population, constraint validation |
| `src/ui/ZoneManager.cpp` | Per-panel widths for secondary sidebar |
| `src/ui/ActivityBar.cpp` | Secondary sidebar dot indicator |
| `src/ui/ActivityBarModel.h` | sidebar_location field |
| `src/ui/PanelHeader.cpp` | Drag initiation, link scroll action |
| `src/ui/LayoutPresetManager.cpp` | Two Sidebars preset |
| `src/ui/FocusManager.h` / `.cpp` | Secondary sidebar focus zone |
| `src/ui/MainFrame.cpp` | Commands and shortcuts |
| `src/core/Events.h` | LinkedScrollEvent |
| `docs/user_guide.md` | Two-sidebar documentation |
| `CMakeLists.txt` | New sources, test target |
