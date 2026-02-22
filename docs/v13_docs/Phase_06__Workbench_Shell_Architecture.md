# Phase 06 -- Workbench Shell Architecture

## Objective

Redesign the top-level layout from the current three-zone model (Sidebar 256px | Content flex | StatusBar 24px) into a VS Code-style five-zone workbench shell: Activity Bar (48px) | Primary Sidebar (resizable) | Editor Area (flex) | Secondary Sidebar (resizable) | with Panel Area (bottom, resizable) and Status Bar (22px). The shell manages zone visibility toggling, drag-to-resize between all zones, layout state persistence, and layout presets (Default, Zen, Presentation, Compact).

## Prerequisites

- Phase 01 (Design System Foundation) -- for `ComponentSizeResolver`, `SpacingGrid`, `DesignTokenRegistry`.
- Phase 03 (Theme Engine V2) -- for per-surface scoped tokens.
- Phase 04 (Animation Framework) -- for `TransitionManager` (zone show/hide animations).
- Phase 05 (Accessibility) -- for `FocusManager` zone registration, keyboard navigation.

## Deliverables

- `WorkbenchShell` -- new top-level container replacing `LayoutManager`'s current layout logic.
- `ZoneManager` -- tracks visibility, size, and constraints for each zone.
- `ResizeHandle` -- generalized resize bar widget for all zone boundaries.
- `LayoutPresetManager` -- named layout configurations.
- `LayoutStatePersistence` -- save/restore zone sizes and visibility.
- Catch2 test target: `test_workbench_shell`.

## Estimated Complexity

Very High -- this is the most architecturally significant phase, restructuring the entire window layout.

---

## Tasks

### Task 1: Define the Zone Model

**Description:** Create the data model that represents the five workbench zones, their constraints, and their relationships.

**Key Implementation Details:**
- Enum: `WorkbenchZone { kActivityBar, kPrimarySidebar, kEditorArea, kSecondarySidebar, kPanelArea, kStatusBar }`.
- Struct: `ZoneConfig { WorkbenchZone zone; int default_size; int min_size; int max_size; bool resizable; bool collapsible; bool visible; int current_size; }`.
- Default configurations:
  - `kActivityBar`: 48px, fixed, not resizable, always visible.
  - `kPrimarySidebar`: 256px default, 180-600px range, resizable, collapsible.
  - `kEditorArea`: flex (fills remaining), min 200px, not collapsible.
  - `kSecondarySidebar`: 256px default, 180-400px range, resizable, collapsible, hidden by default.
  - `kPanelArea`: 200px default, 100-600px range, resizable from top edge, collapsible, hidden by default.
  - `kStatusBar`: 22px, fixed, not resizable, always visible.
- Class: `ZoneManager` in `src/ui/ZoneManager.h`
- Method: `auto config(WorkbenchZone zone) const -> const ZoneConfig&`.
- Method: `void set_visible(WorkbenchZone zone, bool visible)`.
- Method: `void set_size(WorkbenchZone zone, int size)` -- clamps to min/max.
- Method: `void toggle(WorkbenchZone zone)`.

**Files Affected:**
- `src/ui/ZoneManager.h` (new)
- `src/ui/ZoneManager.cpp` (new)

**Acceptance Criteria:**
- All 6 zones have valid default configurations.
- `set_size` clamps to min/max bounds.
- `toggle` flips visibility.
- `kEditorArea` cannot be hidden.

**Dependencies:** None.

---

### Task 2: Implement the Generalized ResizeHandle Widget

**Description:** Create a single reusable resize handle widget that works for all zone boundaries. Replaces the current `SplitterBar` which only works between sidebar and content.

**Key Implementation Details:**
- Class: `ResizeHandle` in `src/ui/ResizeHandle.h`
- Enum: `ResizeOrientation { kHorizontal, kVertical }` -- horizontal = left/right drag, vertical = up/down drag.
- Constructor: `ResizeHandle(wxWindow* parent, ResizeOrientation orientation, core::ThemeEngine& theme, std::function<void(int delta)> on_resize)`.
- Hit zone: 6px (compact), 8px (default), 10px (comfortable) from `SpacingGrid`.
- Visual: 1px line centered in hit zone, color from `ThemeColorToken::BorderLight`.
- Hover: line brightens to `AccentPrimary` with opacity animation.
- Double-click: reset zone to default size.
- Cursor: `wxCURSOR_SIZEWE` for horizontal, `wxCURSOR_SIZENS` for vertical.
- Snap behavior: when dragging near the default size (within 10px), snap to default.
- Publishes zone size changes via `on_resize` callback.

**Files Affected:**
- `src/ui/ResizeHandle.h` (new)
- `src/ui/ResizeHandle.cpp` (new)

**Acceptance Criteria:**
- Dragging horizontally resizes the adjacent zones.
- Double-click resets to default size.
- Snap-to-default works within 10px threshold.
- Cursor changes on hover.

**Dependencies:** Phase 01 Task 5, Phase 04 Task 10.

---

### Task 3: Implement the WorkbenchShell Container

**Description:** Create the new top-level container that arranges all zones in the correct layout. This replaces the current `LayoutManager::CreateLayout()` logic.

**Key Implementation Details:**
- Class: `WorkbenchShell` in `src/ui/WorkbenchShell.h`
- Inherits from `ThemeAwareWindow`.
- Layout (horizontal): `[ActivityBar] [ResizeHandle] [PrimarySidebar] [ResizeHandle] [EditorArea] [ResizeHandle] [SecondarySidebar]`.
- Layout (vertical, within the right 4 zones): `[HorizontalRow] / [ResizeHandle] / [PanelArea]`.
- Layout (overall): `[WorkbenchBody] / [StatusBar]`.
- Constructor: `WorkbenchShell(wxWindow* parent, ZoneManager& zones, core::ThemeEngine& theme, core::EventBus& bus, DesignTokenRegistry& tokens)`.
- Method: `auto zone_panel(WorkbenchZone zone) -> wxPanel*` -- returns the container for a zone (to be populated by higher-level code).
- Method: `void relayout()` -- recalculate all positions based on current ZoneManager sizes.
- Uses `wxBoxSizer` for the horizontal row and another for the vertical (body + panel + status).
- Each zone is a plain `wxPanel` that serves as the parent for zone-specific content.

**Files Affected:**
- `src/ui/WorkbenchShell.h` (new)
- `src/ui/WorkbenchShell.cpp` (new)

**Acceptance Criteria:**
- Shell renders all 6 zones in the correct positions.
- Hiding a zone (e.g., secondary sidebar) causes adjacent zones to fill the space.
- Hiding the panel area makes the editor area taller.
- StatusBar always appears at the bottom.

**Dependencies:** Tasks 1, 2.

---

### Task 4: Implement Zone Visibility Toggling with Animation

**Description:** When a zone is shown or hidden, animate its size from zero to default (or vice versa) using the `TransitionManager`.

**Key Implementation Details:**
- Showing a sidebar: animate width from 0 to `ZoneConfig::default_size` over 250ms with `ease_out_cubic`.
- Hiding a sidebar: animate width from current to 0 over 200ms with `ease_in_cubic`.
- Showing the panel area: animate height from 0 to default.
- Hiding the panel area: animate height to 0.
- During animation, `relayout()` is called on each frame to update positions.
- After animation completes, update `ZoneManager::set_visible()`.
- Adjacent `ResizeHandle` widgets are hidden when their zone is collapsed.

**Files Affected:**
- `src/ui/WorkbenchShell.cpp` (modify -- add animation to visibility toggles)

**Acceptance Criteria:**
- Toggling the sidebar shows a smooth expand/collapse animation.
- Toggling the panel area shows a smooth slide up/down.
- Reduced motion: instant show/hide.
- ResizeHandles disappear when their zone is hidden.

**Dependencies:** Task 3, Phase 04 Task 5.

---

### Task 5: Implement Layout Presets

**Description:** Create named layout configurations that users can switch between. Each preset defines which zones are visible and their sizes.

**Key Implementation Details:**
- Class: `LayoutPresetManager` in `src/ui/LayoutPresetManager.h`
- Struct: `LayoutPreset { std::string name; std::map<WorkbenchZone, ZoneConfig> overrides; }`.
- Built-in presets:
  - "Default": primary sidebar visible (256px), panel hidden, secondary sidebar hidden.
  - "Zen Mode": all sidebars and panel hidden, activity bar hidden, status bar hidden, editor full screen.
  - "Presentation": similar to Zen but status bar visible, larger fonts (comfortable density).
  - "Two Sidebars": primary and secondary sidebars visible.
  - "Panel Focus": panel area maximized (60% of window height), sidebars collapsed.
  - "Compact": compact density, primary sidebar narrow (180px), panel hidden.
- Method: `void apply(const std::string& preset_name, ZoneManager& zones)`.
- Method: `void register_preset(const LayoutPreset& preset)`.
- Method: `auto all_presets() const -> std::vector<std::string>`.
- Method: `auto save_as_preset(const std::string& name, const ZoneManager& zones) -> LayoutPreset` -- captures current state.
- Command Palette: "View: Layout Preset" with sub-choices.
- Keyboard: `Ctrl+K Ctrl+Z` for Zen Mode (matching VS Code).

**Files Affected:**
- `src/ui/LayoutPresetManager.h` (new)
- `src/ui/LayoutPresetManager.cpp` (new)
- `src/ui/MainFrame.cpp` (modify -- register commands and shortcut)

**Acceptance Criteria:**
- Applying "Zen Mode" hides all chrome except the editor.
- Applying "Default" restores the standard layout.
- Custom presets can be saved and restored.
- Zen mode toggle works with Ctrl+K Ctrl+Z.

**Dependencies:** Tasks 1, 3.

---

### Task 6: Implement Layout State Persistence

**Description:** Save and restore the complete layout state (zone sizes, visibility, active preset) across application restarts.

**Key Implementation Details:**
- Serialized state includes:
  - Each zone's current size.
  - Each zone's visibility.
  - Active preset name (or "custom" if manually configured).
  - Secondary sidebar mode.
  - Panel area active tab.
- Stored in `Config` under `layout.*` keys:
  - `layout.primary_sidebar.width`
  - `layout.primary_sidebar.visible`
  - `layout.secondary_sidebar.width`
  - `layout.secondary_sidebar.visible`
  - `layout.panel.height`
  - `layout.panel.visible`
  - `layout.preset`
- Load in `WorkbenchShell` constructor.
- Save on every zone resize/toggle (debounced at 500ms to avoid excessive writes).

**Files Affected:**
- `src/ui/WorkbenchShell.cpp` (modify -- load/save state)
- `src/core/Config.h` (modify -- ensure layout keys exist)

**Acceptance Criteria:**
- Resizing the sidebar and restarting preserves the sidebar width.
- Toggling the panel and restarting preserves the panel visibility.
- Corrupted config values are handled gracefully (fall back to defaults).

**Dependencies:** Tasks 1, 3.

---

### Task 7: Migrate LayoutManager to Use WorkbenchShell

**Description:** Refactor `LayoutManager` to use `WorkbenchShell` as its layout engine instead of managing zones directly. `LayoutManager` becomes a higher-level orchestrator that populates zone containers with the correct child widgets.

**Key Implementation Details:**
- `LayoutManager::CreateLayout()` now:
  1. Creates `WorkbenchShell` as its child.
  2. Populates `zone_panel(kActivityBar)` with `ActivityBar`.
  3. Populates `zone_panel(kPrimarySidebar)` with the sidebar panel registry content.
  4. Populates `zone_panel(kEditorArea)` with `TabBar` + `BreadcrumbBar` + `SplitView`.
  5. Populates `zone_panel(kPanelArea)` with the bottom panel notebook.
  6. Populates `zone_panel(kStatusBar)` with `StatusBarPanel`.
  7. Populates `zone_panel(kSecondarySidebar)` with secondary sidebar content.
- Remove `LayoutManager`'s direct `body_sizer_`, `main_sizer_`, `splitter_` management.
- `toggle_sidebar()` delegates to `ZoneManager::toggle(kPrimarySidebar)`.
- `ShowBottomPanel()` delegates to `ZoneManager::toggle(kPanelArea)`.
- `ToggleSecondarySidebar()` delegates to `ZoneManager::toggle(kSecondarySidebar)`.

**Files Affected:**
- `src/ui/LayoutManager.h` (modify -- replace zone management with WorkbenchShell delegation)
- `src/ui/LayoutManager.cpp` (modify -- major refactor of CreateLayout)

**Acceptance Criteria:**
- All existing LayoutManager functionality works through WorkbenchShell.
- No direct sizer manipulation remains in LayoutManager for zone layout.
- Sidebar toggle, panel toggle, secondary sidebar toggle all work.

**Dependencies:** Tasks 3, 4, 5, 6.

---

### Task 8: Implement Activity Bar Zone Placement

**Description:** Ensure the `ActivityBar` is correctly placed in the `kActivityBar` zone of the shell, with proper sizing and theme integration.

**Key Implementation Details:**
- `ActivityBar` fills the full height of `zone_panel(kActivityBar)`.
- Width is fixed at `ComponentSizeResolver::resolve(kActivityBarSlot).height` (48px default).
- Activity bar should have its own background color: `resolve("activityBar.background")`.
- A 1px right border separates it from the primary sidebar: `ThemeColorToken::BorderDark`.
- The activity bar is the leftmost element in the horizontal layout.

**Files Affected:**
- `src/ui/LayoutManager.cpp` (modify -- place ActivityBar in shell zone)
- `src/ui/WorkbenchShell.cpp` (modify -- ensure activity bar zone has correct constraints)

**Acceptance Criteria:**
- Activity bar is the leftmost strip in the window.
- Its width is fixed and does not resize.
- Background color follows the scoped theme token.

**Dependencies:** Tasks 3, 7.

---

### Task 9: Implement Primary Sidebar Zone Placement

**Description:** Ensure the primary sidebar content (explorer, search, extensions, etc.) is correctly placed in the `kPrimarySidebar` zone.

**Key Implementation Details:**
- The zone panel contains:
  1. `SidebarToolbar` at the top (contextual header with title and actions).
  2. Active sidebar panel content (from `SidebarPanelRegistry`).
- Sidebar mode switching (triggered by `ActivityBar` selection) swaps the panel content.
- Zone has a right border of 1px using `ThemeColorToken::BorderDark`.
- Background: `resolve("sidebar.background")`.

**Files Affected:**
- `src/ui/LayoutManager.cpp` (modify)

**Acceptance Criteria:**
- Sidebar content fills the primary sidebar zone.
- Switching activity bar items changes the sidebar content.
- Resize handle between sidebar and editor works correctly.

**Dependencies:** Tasks 3, 7.

---

### Task 10: Implement Editor Area Zone Placement

**Description:** Place the editor area content (toolbar, tab bar, breadcrumb bar, split view) in the `kEditorArea` zone.

**Key Implementation Details:**
- The editor area zone contains (top to bottom):
  1. `Toolbar` (40px).
  2. `TabBar` (36px).
  3. `BreadcrumbBar` (24px).
  4. `SplitView` / `EditorPanel` (flex fill).
- The editor area is the flex-fill zone (grows to fill remaining space).
- Background: `resolve("editor.background")`.

**Files Affected:**
- `src/ui/LayoutManager.cpp` (modify)

**Acceptance Criteria:**
- Editor area fills all remaining horizontal space.
- Toolbar, tab bar, breadcrumb, and editor stack vertically correctly.
- Hiding sidebars causes the editor area to expand.

**Dependencies:** Tasks 3, 7.

---

### Task 11: Implement Secondary Sidebar Zone Placement

**Description:** Place the secondary sidebar in the `kSecondarySidebar` zone. The secondary sidebar can host any panel that the primary sidebar can, but independently.

**Key Implementation Details:**
- Uses the existing `secondary_panel_registry_` from `LayoutManager`.
- Has its own `SidebarToolbar` header.
- Has its own mode selector (mini activity bar icons or tab strip at the top).
- Default mode: Search or Outline (configurable).
- Left border 1px using `ThemeColorToken::BorderDark`.
- Toggle: `Ctrl+Alt+B` or Command Palette "View: Toggle Secondary Sidebar".

**Files Affected:**
- `src/ui/LayoutManager.cpp` (modify -- populate secondary sidebar zone)
- `src/ui/WorkbenchShell.cpp` (modify -- ensure zone is wired)

**Acceptance Criteria:**
- Secondary sidebar appears to the right of the editor area.
- It can host any sidebar panel independently of the primary sidebar.
- Toggling it shows/hides with animation.

**Dependencies:** Tasks 3, 7.

---

### Task 12: Implement Panel Area Zone Placement

**Description:** Place the bottom panel area (Output, Problems, Walkthrough, and future Terminal/Debug Console) in the `kPanelArea` zone.

**Key Implementation Details:**
- The panel area zone contains:
  1. Panel tab bar at the top (thin tab strip showing panel names).
  2. Active panel content below.
- Resize handle on the top edge for vertical resizing.
- Maximize toggle: double-clicking the top resize handle maximizes the panel to fill the editor area height.
- Background: `resolve("panel.background")`.
- Border-top: 1px `ThemeColorToken::BorderDark`.

**Files Affected:**
- `src/ui/LayoutManager.cpp` (modify -- populate panel zone with notebook)
- `src/ui/WorkbenchShell.cpp` (modify -- add maximize behavior to panel resize handle)

**Acceptance Criteria:**
- Bottom panel appears below the editor area.
- Dragging the top edge resizes the panel.
- Double-clicking maximizes/restores the panel.

**Dependencies:** Tasks 3, 7.

---

### Task 13: Implement Status Bar Zone Placement

**Description:** Ensure the `StatusBarPanel` is correctly placed at the very bottom of the workbench shell.

**Key Implementation Details:**
- Status bar fills the full width of the window.
- Fixed height: 22px from `ComponentSizeResolver`.
- No resize handle (not resizable).
- Background: `resolve("statusBar.background")`.
- In Zen Mode preset, status bar can be hidden.

**Files Affected:**
- `src/ui/LayoutManager.cpp` (modify)

**Acceptance Criteria:**
- Status bar spans full window width.
- Status bar is always at the very bottom.
- Zen Mode hides the status bar.

**Dependencies:** Tasks 3, 7.

---

### Task 14: Implement Drag-to-Resize for All Zone Boundaries

**Description:** Wire `ResizeHandle` widgets between every pair of adjacent resizable zones.

**Key Implementation Details:**
- Resize handles placed between:
  1. Activity Bar | Primary Sidebar (horizontal, resize sidebar).
  2. Primary Sidebar | Editor Area (horizontal, resize sidebar).
  3. Editor Area | Secondary Sidebar (horizontal, resize secondary sidebar).
  4. Editor Area + Sidebars | Panel Area (vertical, resize panel).
- Each handle's `on_resize` callback updates `ZoneManager::set_size()` and triggers `WorkbenchShell::relayout()`.
- Minimum sizes are enforced by `ZoneManager`.
- When a zone is at minimum and drag continues, collapse the zone (hide it).

**Files Affected:**
- `src/ui/WorkbenchShell.cpp` (modify -- create and position resize handles)

**Acceptance Criteria:**
- All four resize handles work independently.
- Dragging past minimum size collapses the zone.
- Snap-to-default works on all handles.

**Dependencies:** Tasks 2, 3.

---

### Task 15: Implement Zen Mode

**Description:** Implement a comprehensive Zen Mode that maximizes the editor area by hiding all surrounding chrome.

**Key Implementation Details:**
- When entering Zen Mode:
  1. Save current layout state as a snapshot.
  2. Hide: Activity Bar, Primary Sidebar, Secondary Sidebar, Panel Area, Status Bar.
  3. Hide: Toolbar, TabBar, BreadcrumbBar (within editor area).
  4. Optionally: center the editor content with max-width constraint (80ch).
  5. Optionally: enter fullscreen mode (platform-specific).
- When exiting Zen Mode:
  1. Restore the saved layout snapshot.
  2. Exit fullscreen if entered.
- Toggle: `Ctrl+K Ctrl+Z` or Command Palette "View: Toggle Zen Mode".
- Pressing `Escape` exits Zen Mode.
- Show a minimal floating widget in the top-right corner for exiting.

**Files Affected:**
- `src/ui/LayoutPresetManager.cpp` (modify -- implement Zen Mode preset)
- `src/ui/LayoutManager.cpp` (modify -- hide/show editor chrome)
- `src/ui/MainFrame.cpp` (modify -- fullscreen toggle)

**Acceptance Criteria:**
- Entering Zen Mode shows only the editor content.
- Exiting restores the exact previous layout.
- Escape key exits Zen Mode.
- Floating exit widget is visible and clickable.

**Dependencies:** Tasks 5, 7.

---

### Task 16: Implement Presentation Mode

**Description:** Implement Presentation Mode optimized for screen sharing and live demos.

**Key Implementation Details:**
- When entering Presentation Mode:
  1. Set density to comfortable.
  2. Increase editor font size by 4pt.
  3. Hide activity bar.
  4. Keep sidebar visible but narrow (200px).
  5. Keep status bar visible.
  6. Hide panel area.
  7. Optionally enter fullscreen.
- When exiting:
  1. Restore density, font size, and layout.
- Command Palette: "View: Toggle Presentation Mode".

**Files Affected:**
- `src/ui/LayoutPresetManager.cpp` (modify)

**Acceptance Criteria:**
- Presentation Mode increases readability.
- Fonts are noticeably larger.
- Exiting restores all previous settings.

**Dependencies:** Tasks 5, 7; Phase 01 Task 13.

---

### Task 17: Register Zone Focus Zones with Accessibility

**Description:** Update `FocusManager` zone registration to match the new workbench zones and ensure keyboard navigation traverses all visible zones.

**Key Implementation Details:**
- Map `WorkbenchZone` to `FocusZoneId`:
  - `kActivityBar` -> `FocusZoneId::kActivityBar`
  - `kPrimarySidebar` -> `FocusZoneId::kSidebar`
  - `kEditorArea` -> `FocusZoneId::kEditorArea` (further subdivided into tabs, breadcrumb, editor)
  - `kSecondarySidebar` -> new `FocusZoneId::kSecondarySidebar`
  - `kPanelArea` -> `FocusZoneId::kBottomPanel`
  - `kStatusBar` -> `FocusZoneId::kStatusBar`
- When a zone is hidden, disable it in `FocusManager`.
- When a zone is shown, enable it.

**Files Affected:**
- `src/ui/FocusManager.h` (modify -- add kSecondarySidebar)
- `src/ui/WorkbenchShell.cpp` (modify -- wire zone visibility to FocusManager)

**Acceptance Criteria:**
- Tab traversal skips hidden zones.
- Showing the secondary sidebar makes it Tab-reachable.
- Zone order matches the visual left-to-right, top-to-bottom order.

**Dependencies:** Phase 05 Tasks 1, 8.

---

### Task 18: Implement Zone Size Constraints Validation

**Description:** Ensure the total of all visible zone widths does not exceed the window width, and implement intelligent size redistribution when the window is resized.

**Key Implementation Details:**
- On window resize:
  1. Activity bar size is fixed.
  2. Status bar height is fixed.
  3. Remaining width = window width - activity bar - all resize handles.
  4. If primary sidebar + secondary sidebar + editor minimum > remaining width: collapse secondary sidebar first, then primary sidebar.
  5. Panel area height is clamped to not exceed (window height - toolbar - tabbar - breadcrumb - statusbar - min editor height).
- Publish `LayoutConstraintViolation` event if a zone cannot satisfy its minimum.

**Files Affected:**
- `src/ui/WorkbenchShell.cpp` (modify -- add constraint validation in relayout)
- `src/ui/ZoneManager.cpp` (modify -- add validation method)

**Acceptance Criteria:**
- Shrinking the window below sidebar + editor minimums collapses the secondary sidebar.
- Further shrinking collapses the primary sidebar.
- Editor area never goes below 200px wide.

**Dependencies:** Tasks 1, 3.

---

### Task 19: Implement Zone Border Rendering

**Description:** Render 1px borders between zones using theme tokens, ensuring clean visual separation.

**Key Implementation Details:**
- Borders drawn by `WorkbenchShell::OnPaint`:
  - Right border on Activity Bar.
  - Right border on Primary Sidebar (when visible).
  - Left border on Secondary Sidebar (when visible).
  - Top border on Panel Area (when visible).
  - Top border on Status Bar.
- Border color: `resolve("sidebarSectionHeader.border")` or `ThemeColorToken::BorderDark`.
- In high contrast mode: borders are 2px and use `ThemeColorToken::BorderLight`.

**Files Affected:**
- `src/ui/WorkbenchShell.cpp` (modify -- add border painting)

**Acceptance Criteria:**
- Borders appear between all adjacent visible zones.
- Border color changes with the theme.
- High contrast mode shows thicker borders.

**Dependencies:** Task 3; Phase 03.

---

### Task 20: Deprecate Old SplitterBar

**Description:** Mark the existing `SplitterBar` class as deprecated and schedule it for removal. All resize functionality is now handled by `ResizeHandle`.

**Key Implementation Details:**
- Add `[[deprecated("Use ResizeHandle from Phase 06")]]` to `SplitterBar` class.
- Remove `SplitterBar` from `LayoutManager`'s member list.
- Ensure no other code references `SplitterBar`.
- Do not delete the file yet (allow a deprecation period).

**Files Affected:**
- `src/ui/SplitterBar.h` (modify -- add deprecation attribute)
- `src/ui/LayoutManager.h` (modify -- remove SplitterBar* member)
- `src/ui/LayoutManager.cpp` (modify -- remove SplitterBar creation)

**Acceptance Criteria:**
- `SplitterBar` is not instantiated anywhere.
- Compilation produces a deprecation warning if any code references it.
- Resize between zones uses `ResizeHandle` exclusively.

**Dependencies:** Tasks 2, 14.

---

### Task 21: Window Size Restoration with New Layout

**Description:** Ensure window size and position are correctly saved and restored, including the new zone layout state.

**Key Implementation Details:**
- `MainFrame::saveWindowState()` now also saves zone sizes via `LayoutStatePersistence`.
- `MainFrame::restoreWindowState()` restores both window geometry and zone layout.
- Handle multi-monitor scenarios: if the saved position is on a disconnected monitor, center on the primary monitor.
- Handle DPI changes between sessions: scale zone sizes proportionally.

**Files Affected:**
- `src/ui/MainFrame.cpp` (modify)

**Acceptance Criteria:**
- Window opens at the same position and size as when last closed.
- Zone layout is preserved across restarts.
- Moving to a different DPI screen adjusts zone sizes proportionally.

**Dependencies:** Tasks 6, 7.

---

### Task 22: Event Bus Integration for Zone Changes

**Description:** Publish events when zones change visibility or size, so other components can react (e.g., editor reflowing, minimap adjusting).

**Key Implementation Details:**
- Events:
  - `ZoneVisibilityChanged { WorkbenchZone zone; bool visible; }`.
  - `ZoneSizeChanged { WorkbenchZone zone; int new_size; }`.
  - `LayoutPresetApplied { std::string preset_name; }`.
- Published by `ZoneManager` on every change.
- `EditorPanel` subscribes to reflow on editor area size change.
- `ActivityBar` subscribes to visibility changes to adjust its appearance.

**Files Affected:**
- `src/core/Events.h` (modify -- add zone events)
- `src/ui/ZoneManager.cpp` (modify -- publish events)

**Acceptance Criteria:**
- Toggling the sidebar publishes `ZoneVisibilityChanged`.
- Resizing the sidebar publishes `ZoneSizeChanged`.
- Applying a preset publishes `LayoutPresetApplied`.

**Dependencies:** Tasks 1, 3.

---

### Task 23: Remove Deprecated LayoutManager Layout Code

**Description:** Clean up the old three-zone layout code from `LayoutManager` now that `WorkbenchShell` handles all zone management.

**Key Implementation Details:**
- Remove from `LayoutManager`:
  - `body_sizer_`, `main_sizer_` (replaced by WorkbenchShell sizers).
  - Direct manipulation of `sidebar_panel_`, `content_panel_` sizers.
  - Old `UpdateSidebarSize()` method (replaced by ZoneManager).
  - Old sidebar animation state (replaced by Phase 04).
  - Old `sidebar_visible_`, `sidebar_width_`, `sidebar_current_width_` (replaced by ZoneManager).
- `LayoutManager` retains:
  - Child widget ownership (ActivityBar, TabBar, etc.).
  - File buffer management.
  - Event subscriptions for editor operations.
  - Sidebar mode switching (delegates panel swapping to WorkbenchShell zone panels).

**Files Affected:**
- `src/ui/LayoutManager.h` (modify -- significant member removal)
- `src/ui/LayoutManager.cpp` (modify -- significant code removal)

**Acceptance Criteria:**
- `LayoutManager` no longer manages sizer layout directly.
- All layout delegation goes through `WorkbenchShell` and `ZoneManager`.
- File operations (open, save, close) still work correctly.

**Dependencies:** Tasks 7 through 14.

---

### Task 24: Update CustomChrome Integration

**Description:** Ensure `CustomChrome` (title bar) works correctly with the new `WorkbenchShell` layout below it.

**Key Implementation Details:**
- `MainFrame` layout (top to bottom): `CustomChrome` (40px) | `WorkbenchShell` (flex fill).
- `CustomChrome::SidebarToggle` button now toggles the primary sidebar via `ZoneManager`.
- Double-click on chrome still toggles maximize.
- Dragging the chrome still moves the window.

**Files Affected:**
- `src/ui/MainFrame.cpp` (modify -- ensure chrome + shell layout)
- `src/ui/CustomChrome.cpp` (modify -- sidebar toggle delegates to ZoneManager)

**Acceptance Criteria:**
- Title bar + workbench shell fill the entire frame.
- Sidebar toggle in chrome works with the new zone system.
- Window drag and maximize still work.

**Dependencies:** Task 7.

---

### Task 25: Catch2 Test Suite for Workbench Shell

**Description:** Write comprehensive unit tests for all workbench shell components.

**Key Implementation Details:**
- Test file: `tests/unit/test_workbench_shell.cpp`
- Test target: `test_workbench_shell` in CMakeLists.txt
- Test sections:
  - `ZoneManager`: default configs, set_size clamping, toggle visibility, editor non-collapsible.
  - `LayoutPresetManager`: apply preset, save custom preset, all_presets list.
  - Zone constraint validation: window too small collapses zones in correct order.
  - Layout state persistence: save and restore round-trip.
  - Event publication: visibility and size change events.
  - Focus zone mapping: hidden zones excluded from traversal.

**Files Affected:**
- `tests/unit/test_workbench_shell.cpp` (new)
- `CMakeLists.txt` (modify)

**Acceptance Criteria:**
- All tests pass.
- At least 35 test assertions.
- Tests do not require wxWidgets event loop.

**Dependencies:** All previous tasks.

---

## Dependency Graph

```
Task 1 (ZoneManager) ─────────────────────────────┐
Task 2 (ResizeHandle) ─────────────────────────────┤
Task 3 (WorkbenchShell) ── 1, 2 ───────────────────┤
Task 4 (Visibility animation) ── 3, Phase 04 ──────┤
Task 5 (Layout presets) ── 1, 3 ───────────────────┤
Task 6 (State persistence) ── 1, 3 ────────────────┤
Task 7 (LayoutManager migration) ── 3-6 ──────────┤
Task 8-13 (Zone placements) ── 3, 7 ──────────────┤
Task 14 (Resize wiring) ── 2, 3 ──────────────────┤
Task 15 (Zen Mode) ── 5, 7 ───────────────────────┤
Task 16 (Presentation Mode) ── 5, 7 ──────────────┤
Task 17 (Focus zone registration) ── Phase 05 ─────┤
Task 18 (Constraint validation) ── 1, 3 ──────────┤
Task 19 (Border rendering) ── 3 ──────────────────┤
Task 20 (Deprecate SplitterBar) ── 2, 14 ─────────┤
Task 21 (Window restoration) ── 6, 7 ─────────────┤
Task 22 (Zone events) ── 1, 3 ────────────────────┤
Task 23 (LayoutManager cleanup) ── 7-14 ──────────┤
Task 24 (CustomChrome) ── 7 ──────────────────────┤
Task 25 (Tests) ── all ───────────────────────────┘
```

## Files Created

| File | Type |
|------|------|
| `src/ui/ZoneManager.h` / `.cpp` | Zone model |
| `src/ui/ResizeHandle.h` / `.cpp` | Resize widget |
| `src/ui/WorkbenchShell.h` / `.cpp` | Shell layout |
| `src/ui/LayoutPresetManager.h` / `.cpp` | Layout presets |
| `tests/unit/test_workbench_shell.cpp` | Test file |

## Files Modified

| File | Change |
|------|--------|
| `src/ui/LayoutManager.h` / `.cpp` | Major refactor: delegate layout to WorkbenchShell |
| `src/ui/SplitterBar.h` | Deprecation attribute |
| `src/ui/FocusManager.h` / `.cpp` | Add kSecondarySidebar zone |
| `src/ui/MainFrame.cpp` | Layout preset commands, fullscreen |
| `src/ui/CustomChrome.cpp` | Zone-aware sidebar toggle |
| `src/core/Events.h` | Zone change events |
| `src/core/Config.h` | Layout persistence keys |
| `CMakeLists.txt` | New sources, test target |
