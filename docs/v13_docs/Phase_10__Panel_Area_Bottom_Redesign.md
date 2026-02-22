# Phase 10 -- Panel Area (Bottom) Redesign

## Objective

Redesign the bottom panel area from its current `wxNotebook`-based implementation (Output, Problems, Walkthrough) into a production-quality panel system with a custom tab bar, maximize/restore toggle, drag-to-resize from the top edge, panel-specific action icons in each panel header, badge indicators on tabs (error count on Problems, etc.), the ability to close individual panel tabs, drag panels between the bottom and sidebar, and horizontal split support within the panel area. This redesign prepares the panel area for future additions: integrated Terminal, Debug Console, and additional extension-contributed panels.

## Prerequisites

- Phase 01 (Design System Foundation) -- for spacing, typography, component sizes.
- Phase 02 (Icon Library) -- for panel tab icons, action icons.
- Phase 03 (Theme Engine V2) -- for `panel.*` and `panelTitle.*` scoped tokens.
- Phase 04 (Animation Framework) -- for show/hide animations, tab transitions.
- Phase 05 (Accessibility) -- for panel focus zone, keyboard navigation.
- Phase 06 (Workbench Shell) -- for panel area zone, resize handle.
- Phase 08 (Primary Sidebar) -- for shared components (PanelHeader, ThemedScrollbar).

## Deliverables

- `PanelTabBar` -- custom tab bar for bottom panels with badges, close buttons.
- `PanelContainer` -- container managing panel content with action headers.
- `PanelSplitter` -- horizontal split within the panel area.
- Panel maximize/restore functionality.
- Panel drag between bottom and sidebar.
- Future-ready panel contribution API.
- Catch2 test target: `test_panel_area`.

## Estimated Complexity

High -- replaces wxNotebook with custom implementation and adds significant new interaction patterns.

---

## Tasks

### Task 1: Design the Panel Data Model

**Description:** Create a data model representing bottom panel tabs, their content, badges, and configuration.

**Key Implementation Details:**
- Struct: `PanelTabConfig { std::string id; std::string title; std::string icon_name; BadgeState badge; int badge_count; bool closable; bool reorderable; int order; }`.
- Class: `PanelAreaModel` in `src/ui/PanelAreaModel.h`
- Methods:
  - `void register_panel(const PanelTabConfig& config)`.
  - `void unregister_panel(const std::string& id)`.
  - `void set_active(const std::string& id)`.
  - `auto active_panel() const -> std::string`.
  - `auto panels() const -> std::vector<PanelTabConfig>` -- sorted by order.
  - `void set_badge(const std::string& id, BadgeState state, int count)`.
  - `void set_order(const std::string& id, int order)`.
  - `void set_visible(const std::string& id, bool visible)`.
- Default panels: Output, Problems, Walkthrough (plus future: Terminal, Debug Console).
- Model publishes events on change: `PanelTabsChanged`, `PanelBadgeChanged`.

**Files Affected:**
- `src/ui/PanelAreaModel.h` (new)
- `src/ui/PanelAreaModel.cpp` (new)

**Acceptance Criteria:**
- Register/unregister/reorder panels.
- Badge updates publish events.
- Panels sorted by order.

**Dependencies:** None.

---

### Task 2: Implement the PanelTabBar Widget

**Description:** Create a custom-drawn tab bar for the bottom panel area, replacing the `wxNotebook` tab strip. Tabs show icons, labels, badges, and close buttons.

**Key Implementation Details:**
- Class: `PanelTabBar` in `src/ui/PanelTabBar.h`
- Height: 30px.
- Each tab (left to right):
  - Icon (14px, from icon registry).
  - Label text (caption font).
  - Badge (if present): pill shape with count.
  - Close button (14px "X", visible on hover).
- Active tab: highlighted background, accent-colored bottom border (2px).
- Inactive tab: muted foreground, transparent background.
- Right side of tab bar:
  - "Maximize" / "Restore" icon button.
  - "Close Panel" icon button (hides the entire panel area).
  - "More Actions" ellipsis dropdown.
- Tab interaction: click to activate, middle-click to close, drag to reorder.
- Overflow: horizontal scroll with left/right chevrons when tabs exceed available width.
- Background: `resolve("panelTitle.inactiveBackground")`.
- Border-bottom: 1px `ThemeColorToken::BorderDark`.

**Files Affected:**
- `src/ui/PanelTabBar.h` (new)
- `src/ui/PanelTabBar.cpp` (new)

**Acceptance Criteria:**
- Tabs render with icons, labels, and badges.
- Active tab is visually distinct.
- Close button works on individual tabs.
- Overflow scrolling works when many tabs exist.

**Dependencies:** Task 1, Phase 02, Phase 01.

---

### Task 3: Implement the PanelContainer Widget

**Description:** Create a container that hosts the active panel's content with a per-panel action toolbar at the top.

**Key Implementation Details:**
- Class: `PanelContainer` in `src/ui/PanelContainer.h`
- Layout (top to bottom):
  1. `PanelTabBar` (30px).
  2. Panel-specific action toolbar (28px, optional -- only shown if panel has actions).
  3. Panel content (flex fill).
- The action toolbar is configured per panel:
  - Output: channel selector dropdown, "Clear" button, "Scroll Lock" toggle.
  - Problems: severity filter icons (Error, Warning, Info, Hint), "Collapse All".
  - Walkthrough: no actions.
  - Terminal (future): "New Terminal", "Split Terminal", "Kill Terminal".
  - Debug Console (future): "Clear Console", "Evaluate Expression" input.
- Content area uses `ThemedScrollbar` from Phase 08.

**Files Affected:**
- `src/ui/PanelContainer.h` (new)
- `src/ui/PanelContainer.cpp` (new)

**Acceptance Criteria:**
- Container shows tab bar + action toolbar + content.
- Switching tabs changes action toolbar and content.
- Action toolbar buttons trigger correct actions.

**Dependencies:** Tasks 1, 2; Phase 08 Task 5.

---

### Task 4: Implement Panel Maximize/Restore

**Description:** Allow the user to maximize the panel area to fill the editor space, and restore it to its previous size.

**Key Implementation Details:**
- Maximize button in `PanelTabBar` right section.
- On maximize:
  - Panel area height expands to fill the space between the toolbar/tabbar and the status bar.
  - Editor area is effectively hidden (height reduced to 0 + toolbar + tabbar).
  - Sidebars remain visible.
  - Maximize icon changes to "Restore" icon.
- On restore:
  - Panel area returns to its previous height.
  - Editor area re-expands.
- Double-click the resize handle (top edge) toggles maximize.
- Keyboard shortcut: `Ctrl+Shift+M` to toggle (when panel is focused).
- Animation: 250ms resize transition.

**Files Affected:**
- `src/ui/PanelContainer.cpp` (modify -- wire maximize button)
- `src/ui/ZoneManager.cpp` (modify -- add maximize/restore state)
- `src/ui/WorkbenchShell.cpp` (modify -- handle maximized panel layout)

**Acceptance Criteria:**
- Maximize fills the panel to editor height.
- Restore returns to previous height.
- Double-click on resize handle toggles.
- Animation is smooth.

**Dependencies:** Task 3, Phase 06 Task 12.

---

### Task 5: Implement Badge Indicators on Panel Tabs

**Description:** Show real-time badge indicators on panel tabs reflecting their content state.

**Key Implementation Details:**
- Problems tab: shows error count + warning count badge (e.g., "3" in red pill).
  - Subscribe to `DiagnosticsChangedEvent`.
  - Badge color: error = `ThemeColorToken::ErrorColor`, warning = `ThemeColorToken::SuccessColor` (if only warnings).
- Output tab: dot indicator when new output is available and panel is not active.
  - Subscribe to `OutputChannelChangedEvent`.
  - Clear dot when panel is activated.
- Terminal tab (future): dot when terminal has new output.
- Badge updates trigger `PanelTabBar::Refresh()`.

**Files Affected:**
- `src/ui/PanelContainer.cpp` (modify -- subscribe to events, update badges)
- `src/ui/PanelTabBar.cpp` (modify -- render badges)

**Acceptance Criteria:**
- Problems tab shows error count.
- Output tab shows a dot when inactive with new content.
- Badges clear when the relevant panel is activated.

**Dependencies:** Tasks 1, 2.

---

### Task 6: Implement Close Individual Panel Tabs

**Description:** Allow users to close individual panel tabs, removing them from the panel area. Closed panels can be reopened via Command Palette.

**Key Implementation Details:**
- Close button on each tab (visible on hover).
- Middle-click on tab to close.
- When closed: `PanelAreaModel::unregister_panel(id)`.
- If the closed panel was active: activate the next panel.
- If no panels remain: hide the panel area (set zone visibility to false).
- Reopening: Command Palette "View: Open Output Panel", "View: Open Problems Panel", etc.
- Minimum: at least one panel must remain (prevent closing the last one; or allow and collapse the panel area).

**Files Affected:**
- `src/ui/PanelTabBar.cpp` (modify -- wire close button)
- `src/ui/PanelContainer.cpp` (modify -- handle tab removal)
- `src/ui/MainFrame.cpp` (modify -- register reopen commands)

**Acceptance Criteria:**
- Closing a tab removes it.
- Closing the last tab hides the panel area.
- Reopening via Command Palette works.
- Close button is themed and accessible.

**Dependencies:** Tasks 2, 3.

---

### Task 7: Implement Panel Tab Drag Reorder

**Description:** Allow users to reorder panel tabs by dragging within the tab bar.

**Key Implementation Details:**
- Drag initiation: mouse down on tab + move > 5px.
- During drag:
  - Show the dragged tab at cursor with ghost effect (50% opacity).
  - Show insertion indicator between tabs.
  - Other tabs animate to make room.
- On drop:
  - Update `PanelAreaModel::set_order()`.
  - Persist tab order in Config.
- Tab reorder animation: 150ms, ease_out_quad.

**Files Affected:**
- `src/ui/PanelTabBar.cpp` (modify -- add drag reorder)
- `src/ui/PanelAreaModel.cpp` (modify -- persist order)

**Acceptance Criteria:**
- Dragging tabs reorders them.
- Other tabs shift smoothly.
- Order persists across restarts.

**Dependencies:** Task 2.

---

### Task 8: Implement Panel Drag to Sidebar

**Description:** Allow users to drag a panel tab from the bottom panel area to the primary or secondary sidebar, converting it from a bottom panel to a sidebar panel.

**Key Implementation Details:**
- Drag from `PanelTabBar` can target sidebar zones.
- During drag outside the tab bar:
  - Show ghost tab at cursor.
  - Highlight sidebar zones as drop targets.
- On drop in sidebar:
  - Remove panel from `PanelAreaModel`.
  - Register in the target sidebar's `SidebarPanelRegistry`.
  - The panel's content adapts to vertical layout.
- Reverse: drag from sidebar panel header to bottom panel tab bar.
- Panels that do not support sidebar mode (e.g., Terminal with fixed height) refuse the drop.

**Files Affected:**
- `src/ui/PanelTabBar.cpp` (modify -- drag to external target)
- `src/ui/WorkbenchShell.cpp` (modify -- drop target handling)
- `src/ui/LayoutManager.cpp` (modify -- manage panel registry transfers)

**Acceptance Criteria:**
- Dragging a panel tab to the sidebar moves it.
- Panel content adapts to the new container.
- Dragging back to the bottom panel works.

**Dependencies:** Tasks 2, 7; Phase 08 Task 15.

---

### Task 9: Implement Horizontal Panel Split

**Description:** Allow the panel area to display two panels side by side (horizontal split). This is useful for viewing Output and Problems simultaneously.

**Key Implementation Details:**
- Split activation:
  - Drag a tab to the right half of the panel area to create a split.
  - Or use a "Split Panel" action icon in the panel header.
  - Or Command Palette: "View: Split Panel Area".
- Split layout:
  - Left panel + ResizeHandle (horizontal) + Right panel.
  - Each side has its own panel content area.
  - Tab bar shows all tabs; active tab per side is indicated differently (underline color).
- Unsplit:
  - Drag the last panel out of one side.
  - Or double-click the split resize handle.
- Maximum: 2 panels side by side (no further splitting).

**Files Affected:**
- `src/ui/PanelContainer.h` / `.cpp` (modify -- add split support)
- `src/ui/PanelAreaModel.h` / `.cpp` (modify -- track split state)

**Acceptance Criteria:**
- Splitting shows two panels side by side.
- Each side independently selects its active panel.
- Unsplitting merges back to single panel.

**Dependencies:** Tasks 3, 7; Phase 06 Task 2.

---

### Task 10: Replace wxNotebook with PanelContainer

**Description:** Remove the `wxNotebook` from `LayoutManager` and replace it with the new `PanelContainer`.

**Key Implementation Details:**
- Remove `bottom_panel_notebook_` from `LayoutManager`.
- Create `PanelContainer` in its place.
- Migrate `OutputPanel`, `ProblemsPanel`, and `WalkthroughPanel` to register with `PanelAreaModel`.
- Ensure panel content is correctly parented to the `PanelContainer` content area.
- Wire theme changes to the new container.

**Files Affected:**
- `src/ui/LayoutManager.h` (modify -- replace wxNotebook* with PanelContainer*)
- `src/ui/LayoutManager.cpp` (modify -- replace CreateBottomPanelHost)

**Acceptance Criteria:**
- Bottom panels work identically to before (Output, Problems, Walkthrough).
- Tab switching works.
- Theme changes propagate.
- No reference to wxNotebook remains in LayoutManager.

**Dependencies:** Tasks 1, 2, 3.

---

### Task 11: Implement Panel Action Toolbar for Output Panel

**Description:** Configure the Output panel's action toolbar with channel selector and clear/lock buttons.

**Key Implementation Details:**
- Actions:
  - Channel selector dropdown (reusing existing `OutputPanel::channel_selector_`).
  - "Clear" button (`"clear"` icon) -- clears the active channel.
  - "Scroll Lock" toggle (`"lock"` / `"unlock"` icon) -- toggles auto-scroll.
  - "Word Wrap" toggle -- toggles text wrapping in the output.
- Actions are rendered in the `PanelContainer`'s action toolbar slot for the Output panel.

**Files Affected:**
- `src/ui/OutputPanel.h` / `.cpp` (modify -- expose actions configuration)
- `src/ui/PanelContainer.cpp` (modify -- configure Output panel actions)

**Acceptance Criteria:**
- Channel selector works.
- Clear button clears the output.
- Scroll lock toggle works.

**Dependencies:** Task 3.

---

### Task 12: Implement Panel Action Toolbar for Problems Panel

**Description:** Configure the Problems panel's action toolbar with severity filters and collapse controls.

**Key Implementation Details:**
- Actions:
  - Error toggle (`"error-circle"` icon) -- show/hide errors.
  - Warning toggle (`"warning-triangle"` icon) -- show/hide warnings.
  - Info toggle (`"info-circle"` icon) -- show/hide info.
  - "Collapse All" button (`"collapse-all"` icon).
  - "Group by File" toggle.
- Active filters highlighted with accent background.
- Filter state persisted in Config.

**Files Affected:**
- `src/ui/ProblemsPanel.h` / `.cpp` (modify -- expose filter actions)
- `src/ui/PanelContainer.cpp` (modify -- configure Problems panel actions)

**Acceptance Criteria:**
- Severity filter toggles filter the list.
- Collapse All collapses all file groups.
- Filter state persists.

**Dependencies:** Task 3.

---

### Task 13: Implement Panel Area Show/Hide Animation

**Description:** When the panel area is shown or hidden, animate the resize smoothly.

**Key Implementation Details:**
- Show: animate height from 0 to `ZoneConfig::default_size` over 250ms.
- Hide: animate height from current to 0 over 200ms.
- Already partially covered by Phase 06 Task 4; verify and polish.
- Ensure the editor area resizes smoothly during the animation.
- Tab bar content should not flash during the animation.

**Files Affected:**
- `src/ui/WorkbenchShell.cpp` (verify animation)
- `src/ui/PanelContainer.cpp` (ensure content does not flash)

**Acceptance Criteria:**
- Panel area slides up smoothly when shown.
- Panel area slides down smoothly when hidden.
- No flashing or layout jumps.

**Dependencies:** Phase 06 Task 4.

---

### Task 14: Implement Panel Area Height Persistence

**Description:** Persist the panel area height and maximize state across app restarts.

**Key Implementation Details:**
- Config keys:
  - `layout.panel.height` -- int.
  - `layout.panel.maximized` -- bool.
  - `layout.panel.active_tab` -- string.
  - `layout.panel.tab_order` -- JSON array.
- Load on startup.
- Save on resize, maximize, tab switch (debounced).

**Files Affected:**
- `src/ui/PanelContainer.cpp` (modify -- load/save state)

**Acceptance Criteria:**
- Panel height restored on restart.
- Active tab restored.
- Tab order restored.

**Dependencies:** Tasks 3, 7.

---

### Task 15: Implement Panel Contribution API for Extensions

**Description:** Allow extensions to register custom panels in the panel area.

**Key Implementation Details:**
- Add to `PluginContext`:
  - `auto register_panel(const PanelTabConfig& config, wxPanel* content) -> bool`.
  - `auto unregister_panel(const std::string& id) -> bool`.
- Extension-contributed panels appear after built-in panels in the tab bar.
- Maximum 10 extension panels.
- Extension panels have the same features as built-in panels (badges, close, reorder, drag).

**Files Affected:**
- `src/core/PluginContext.h` (modify -- add panel registration API)
- `src/ui/PanelAreaModel.cpp` (modify -- handle extension panels)

**Acceptance Criteria:**
- An extension can register a custom panel tab.
- The panel appears in the tab bar.
- Unregistering removes it.

**Dependencies:** Task 1.

---

### Task 16: Implement Terminal Panel Stub

**Description:** Add a "Terminal" panel tab as a placeholder for the future integrated terminal. Shows an informational message that the terminal is coming in a future version.

**Key Implementation Details:**
- Panel ID: `"terminal"`.
- Tab icon: `"terminal"`.
- Content: centered message "Integrated Terminal -- Coming Soon" with a terminal icon.
- Keyboard shortcut: `` Ctrl+` `` toggles the panel area and focuses the terminal tab.
- When the terminal is eventually implemented, this stub is replaced.

**Files Affected:**
- `src/ui/TerminalPanelStub.h` (new)
- `src/ui/TerminalPanelStub.cpp` (new)
- `src/ui/PanelAreaModel.cpp` (modify -- register terminal panel)
- `src/ui/MainFrame.cpp` (modify -- register shortcut)

**Acceptance Criteria:**
- Terminal tab appears in the panel tab bar.
- Content shows the "coming soon" message.
- `` Ctrl+` `` shortcut works.

**Dependencies:** Task 1.

---

### Task 17: Implement Debug Console Panel Stub

**Description:** Add a "Debug Console" panel tab as a placeholder for the future debug console.

**Key Implementation Details:**
- Panel ID: `"debug-console"`.
- Tab icon: `"debug-console"`.
- Content: centered message "Debug Console -- Coming Soon".
- Hidden by default; shown when a debug session is active (future).

**Files Affected:**
- `src/ui/DebugConsolePanelStub.h` (new)
- `src/ui/DebugConsolePanelStub.cpp` (new)

**Acceptance Criteria:**
- Debug Console tab exists in the model.
- Can be shown via Command Palette.

**Dependencies:** Task 1.

---

### Task 18: Implement Panel Tab Context Menu

**Description:** Right-clicking a panel tab shows a context menu with management options.

**Key Implementation Details:**
- Context menu items:
  - "Close" -- close this tab.
  - "Close Others" -- close all tabs except this one.
  - "Move to Sidebar" -- move this panel to the primary sidebar.
  - "Split Right" -- open this panel in a split.
  - "---" (separator)
  - Per-panel visibility toggles for all registered panels.

**Files Affected:**
- `src/ui/PanelTabBar.cpp` (modify -- add right-click handler and context menu)

**Acceptance Criteria:**
- Right-click shows the context menu.
- "Close" closes the tab.
- "Move to Sidebar" transfers the panel.
- Visibility toggles work.

**Dependencies:** Tasks 2, 6, 8.

---

### Task 19: Implement Panel Focus and Keyboard Navigation

**Description:** Ensure the panel area is fully keyboard-navigable.

**Key Implementation Details:**
- `FocusZoneId::kBottomPanel` is already registered.
- Within the panel area:
  - Tab key cycles through: panel tabs -> action toolbar buttons -> panel content.
  - Left/Right arrows navigate between tabs.
  - Enter activates a tab.
  - Ctrl+W closes the active tab.
- Focus ring renders around the focused element.
- Screen reader announces tab labels and badge information.

**Files Affected:**
- `src/ui/PanelTabBar.cpp` (modify -- add keyboard navigation)
- `src/ui/PanelContainer.cpp` (modify -- wire focus zone)

**Acceptance Criteria:**
- Tab/arrow key navigation works within the panel area.
- Focus ring is visible.
- Screen reader announces tab labels.

**Dependencies:** Phase 05 Tasks 1, 2, 17.

---

### Task 20: Implement Panel Notifications

**Description:** When a panel tab has new content and is not currently visible (panel area is hidden), show a notification or status bar indicator.

**Key Implementation Details:**
- When the panel area is hidden and a panel receives new content:
  - Show a brief toast notification: "New problems detected (3 errors)".
  - Update the status bar to show a clickable indicator.
- Clicking the notification or status bar indicator opens the panel area and activates the relevant tab.
- Do not show notifications for Output panel (too frequent).
- Only show for Problems (on new errors) and Debug Console (on exceptions).

**Files Affected:**
- `src/ui/PanelContainer.cpp` (modify -- detect hidden panel updates)
- `src/ui/StatusBarPanel.cpp` (modify -- show panel indicator)

**Acceptance Criteria:**
- New errors while panel is hidden trigger a notification.
- Clicking the notification opens the Problems panel.
- Output panel does not trigger notifications.

**Dependencies:** Tasks 5, 10.

---

### Task 21: Implement Panel Area Empty State

**Description:** When all panel tabs are closed, show an empty state in the panel area before collapsing it.

**Key Implementation Details:**
- Brief empty state (visible for 500ms before auto-collapsing):
  - Message: "All panels closed".
  - Link: "Reopen panels" to restore default panels.
- If the user quickly drags a tab back, the empty state is replaced.
- Auto-collapse timer: 500ms after last tab is closed.

**Files Affected:**
- `src/ui/PanelContainer.cpp` (modify -- add empty state)

**Acceptance Criteria:**
- Closing all tabs shows empty state briefly.
- Auto-collapse after 500ms.
- "Reopen panels" link works.

**Dependencies:** Task 6.

---

### Task 22: Implement Panel Area Responsive Behavior

**Description:** On small screens, the panel area adapts its layout.

**Key Implementation Details:**
- Below 600px window width: panel tab labels are hidden (icons only).
- Below 400px window width: panel area is force-collapsed.
- Panel split is disabled below 800px window width.
- Action toolbar collapses into a single overflow dropdown below 400px panel width.

**Files Affected:**
- `src/ui/PanelTabBar.cpp` (modify -- responsive tab labels)
- `src/ui/PanelContainer.cpp` (modify -- responsive action toolbar)

**Acceptance Criteria:**
- Small windows show icon-only tabs.
- Very small windows collapse the panel.
- Split is disabled in narrow windows.

**Dependencies:** Tasks 2, 9.

---

### Task 23: Theme Integration for Panel Area

**Description:** Apply all `panel.*` and `panelTitle.*` scoped tokens to the redesigned panel area.

**Key Implementation Details:**
- Panel background: `resolve("panel.background")`.
- Panel border: `resolve("panel.border")`.
- Tab active foreground: `resolve("panelTitle.activeForeground")`.
- Tab inactive foreground: `resolve("panelTitle.inactiveForeground")`.
- Tab active border: `resolve("panelTitle.activeBorder")`.
- Action toolbar background: `resolve("panel.background")`.
- Verify rendering in dark, light, and high-contrast themes.

**Files Affected:**
- `src/ui/PanelTabBar.cpp` (modify -- use scoped tokens)
- `src/ui/PanelContainer.cpp` (modify -- use scoped tokens)

**Acceptance Criteria:**
- Panel area colors change with the theme.
- All three base themes render correctly.
- High-contrast mode has visible borders.

**Dependencies:** Phase 03 Task 22.

---

### Task 24: Performance Optimization for Panel Switching

**Description:** Ensure panel switching is instant with no visible lag.

**Key Implementation Details:**
- Panels are lazily created on first activation (already in `SidebarPanelRegistry` pattern).
- Once created, panels are hidden (not destroyed) on deactivation.
- Switching tabs only changes visibility, no content re-creation.
- Panel content updates are debounced (Problems list refresh debounced at 200ms).
- Profile tab switch latency; target: under 5ms.

**Files Affected:**
- `src/ui/PanelContainer.cpp` (modify -- ensure lazy creation and reuse)

**Acceptance Criteria:**
- Tab switching completes in under 5ms.
- No content re-creation on switch.
- Lazy creation on first activation.

**Dependencies:** Task 10.

---

### Task 25: Catch2 Test Suite for Panel Area

**Description:** Write comprehensive tests for all panel area components.

**Key Implementation Details:**
- Test file: `tests/unit/test_panel_area.cpp`
- Test target: `test_panel_area` in CMakeLists.txt
- Test sections:
  - `PanelAreaModel`: register/unregister/reorder, set_badge, active panel switching.
  - `PanelTabBar`: tab creation, close, drag reorder, badge rendering.
  - Maximize/restore: state toggling, height preservation.
  - Badge sources: diagnostics changes update Problems badge.
  - Panel split: create/destroy split, independent panel selection.
  - Tab context menu: close, close others, move to sidebar.
  - Panel contribution API: extension registration.
  - Persistence: height, active tab, tab order save/restore.
  - Empty state: shown when all tabs closed.
  - Responsive: icon-only mode below width threshold.

**Files Affected:**
- `tests/unit/test_panel_area.cpp` (new)
- `CMakeLists.txt` (modify)

**Acceptance Criteria:**
- All tests pass.
- At least 40 test assertions.

**Dependencies:** All previous tasks.

---

## Files Created

| File | Type |
|------|------|
| `src/ui/PanelAreaModel.h` / `.cpp` | Panel data model |
| `src/ui/PanelTabBar.h` / `.cpp` | Custom tab bar |
| `src/ui/PanelContainer.h` / `.cpp` | Panel container |
| `src/ui/TerminalPanelStub.h` / `.cpp` | Terminal placeholder |
| `src/ui/DebugConsolePanelStub.h` / `.cpp` | Debug Console placeholder |
| `tests/unit/test_panel_area.cpp` | Test file |

## Files Modified

| File | Change |
|------|--------|
| `src/ui/LayoutManager.h` / `.cpp` | Replace wxNotebook with PanelContainer |
| `src/ui/OutputPanel.h` / `.cpp` | Expose action configuration |
| `src/ui/ProblemsPanel.h` / `.cpp` | Expose filter actions |
| `src/ui/ZoneManager.cpp` | Maximize/restore state |
| `src/ui/WorkbenchShell.cpp` | Maximized panel layout, drop targets |
| `src/ui/StatusBarPanel.cpp` | Panel notification indicator |
| `src/core/PluginContext.h` | Panel contribution API |
| `src/ui/MainFrame.cpp` | Panel commands, terminal shortcut |
| `CMakeLists.txt` | New sources, test target |
