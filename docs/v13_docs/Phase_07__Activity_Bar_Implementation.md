# Phase 07 -- Activity Bar Implementation

## Objective

Build a production-quality activity bar that serves as the primary navigation rail for the entire application. This phase takes the existing `ActivityBar` (which has basic click-to-select and emoji-based icons) and transforms it into a feature-complete component with SVG vector icons, animated badge indicators, keyboard navigation, drag-to-reorder, context menus, a bottom section for settings/account, and full accessibility semantics.

## Prerequisites

- Phase 01 (Design System Foundation) -- for density-aware sizing, spacing grid, typography.
- Phase 02 (Icon Library) -- for SVG icon rendering via `IconDrawer`.
- Phase 03 (Theme Engine V2) -- for `activityBar.*` scoped tokens.
- Phase 04 (Animation Framework) -- for badge animations, tooltip fades, hover effects.
- Phase 05 (Accessibility) -- for focus ring, keyboard navigation, screen reader annotations.
- Phase 06 (Workbench Shell) -- for activity bar zone placement.

## Deliverables

- Fully redesigned `ActivityBar` with SVG icons, badges, drag reorder, context menus.
- `ActivityBarModel` V2 with support for dynamic item registration by extensions.
- Badge animation system with count/dot/urgent variants.
- Bottom section with settings gear and account icon.
- Catch2 test target: `test_activity_bar_v2`.

## Estimated Complexity

Medium-High -- significant rendering and interaction work on a single, well-scoped component.

---

## Tasks

### Task 1: Redesign ActivityBarModel for Dynamic Items

**Description:** Extend the existing `ActivityBarModel` to support dynamic item registration, ordering, visibility control, and badge state. Extensions should be able to contribute activity bar items at runtime.

**Key Implementation Details:**
- Struct: `ActivityBarItemConfig { std::string id; std::string icon_name; std::string tooltip; int order; bool visible; bool is_bottom_section; BadgeState badge; }`.
- Enum: `BadgeState { kNone, kDot, kCount, kUrgent }`.
- Struct: `Badge { BadgeState state; int count; wxColour color; }`.
- Class updates to `ActivityBarModel`:
  - `void register_item(const ActivityBarItemConfig& config)`.
  - `void unregister_item(const std::string& id)`.
  - `void set_order(const std::string& id, int order)`.
  - `void set_visible(const std::string& id, bool visible)`.
  - `void set_badge(const std::string& id, const Badge& badge)`.
  - `auto items() const -> std::vector<ActivityBarItemConfig>` -- returns sorted by order.
  - `auto top_items() const -> std::vector<ActivityBarItemConfig>` -- non-bottom items.
  - `auto bottom_items() const -> std::vector<ActivityBarItemConfig>` -- settings, account.
- Default items registered on startup: Explorer, Search, Source Control, Run/Debug, Extensions, Graph, AI, plus Settings (bottom) and Account (bottom).

**Files Affected:**
- `src/ui/ActivityBarModel.h` (modify)
- `src/ui/ActivityBarModel.cpp` (modify)

**Acceptance Criteria:**
- Default items are registered in the correct order.
- Extensions can register new items that appear in the bar.
- `set_visible(false)` hides an item without removing it.
- `set_badge` updates the badge state.

**Dependencies:** None.

---

### Task 2: Implement SVG Icon Rendering in ActivityBar

**Description:** Replace the current Unicode `icon_char` rendering with SVG icons from the `IconDrawer`.

**Key Implementation Details:**
- In `ActivityBar::OnPaint`:
  - For each item, use `icon_drawer_->draw(gc, item.icon_name, icon_rect, fg_color)`.
  - Active item: use `ThemeColorToken::AccentPrimary` for icon color.
  - Inactive item: use `resolve("activityBar.foreground")` for icon color.
  - Hovered item: use a lighter variant of the foreground color.
- Icon size from `ComponentSizeResolver::resolve(kActivityBarSlot).icon_size`.
- Remove the `icon_char` field usage from rendering (keep as fallback per Phase 02 Task 23).

**Files Affected:**
- `src/ui/ActivityBar.cpp` (modify -- replace icon rendering in OnPaint)

**Acceptance Criteria:**
- All activity bar items display SVG icons.
- Active item icon is colored with the accent color.
- Icons are crisp at all DPI scales.

**Dependencies:** Phase 02 Task 14.

---

### Task 3: Implement Active Indicator Bar

**Description:** Render a visual indicator on the active item: a 2px-wide colored bar on the left edge of the active item slot, matching VS Code's active indicator style.

**Key Implementation Details:**
- In `OnPaint`, for the active item:
  - Draw a rounded rectangle (2px wide, 16px tall) on the left edge of the item slot.
  - Color: `resolve("activityBar.activeBorder")` (falls back to `AccentPrimary`).
  - Vertically centered in the slot.
- Animate the indicator movement when the active item changes using `AnimationTimeline`:
  - Interpolate the indicator's y-position from old item to new item over 150ms with ease_out_cubic.

**Files Affected:**
- `src/ui/ActivityBar.cpp` (modify -- add indicator rendering and animation)

**Acceptance Criteria:**
- Active item has a left-side indicator bar.
- Indicator slides smoothly when switching items.
- Indicator color follows the theme token.

**Dependencies:** Phase 04 Task 3.

---

### Task 4: Implement Badge Rendering

**Description:** Render badge indicators on activity bar items showing counts (e.g., "3" errors on Problems) or notification dots.

**Key Implementation Details:**
- Badge position: top-right of the icon slot.
- Badge types:
  - `kNone`: no badge rendered.
  - `kDot`: 6px filled circle.
  - `kCount`: rounded pill with count text (min-width 16px, height 14px).
  - `kUrgent`: same as kCount but with pulsing animation.
- Badge background: `resolve("activityBar.badge.background")` (falls back to `AccentPrimary`).
- Badge text: `resolve("activityBar.badge.foreground")` (falls back to white).
- Count text font: `TypographyScale::font(kCaption)` at small size (9pt).
- Count display: show number for 1-99, show "99+" for > 99.
- Urgent badge: subtle scale pulse animation (1.0 to 1.15 to 1.0, looping every 2s).

**Files Affected:**
- `src/ui/ActivityBar.cpp` (modify -- add badge rendering in OnPaint)

**Acceptance Criteria:**
- Setting badge count > 0 shows a count badge.
- Setting badge state to kDot shows a dot.
- Urgent badges pulse.
- Counts over 99 show "99+".

**Dependencies:** Task 1, Phase 04 Task 3.

---

### Task 5: Implement Badge Animation on Change

**Description:** When a badge count changes, animate the badge to draw attention.

**Key Implementation Details:**
- When badge count increases: scale-up animation (1.0 -> 1.3 -> 1.0, 200ms, ease_out_back).
- When badge count decreases to 0: fade-out animation (1.0 -> 0.0, 150ms, ease_in_quad).
- When badge appears (kNone -> kDot/kCount): pop-in animation (0.0 -> 1.2 -> 1.0, 250ms, ease_out_back).

**Files Affected:**
- `src/ui/ActivityBar.cpp` (modify -- trigger animations on badge change)

**Acceptance Criteria:**
- Badge pops in when appearing.
- Badge scales on count increase.
- Badge fades out when cleared.

**Dependencies:** Tasks 4, Phase 04 Task 3.

---

### Task 6: Implement Keyboard Navigation

**Description:** Full keyboard navigation within the activity bar: Up/Down arrow keys to move between items, Enter to select, Home/End to jump to first/last item.

**Key Implementation Details:**
- The existing `OnKeyDown` handler in ActivityBar needs enhancement:
  - `Up`: move focus to previous visible item (wrapping from top to bottom items).
  - `Down`: move focus to next visible item.
  - `Enter` / `Space`: activate the focused item (fire `ActivityBarSelectionEvent`).
  - `Home`: focus first top item.
  - `End`: focus last bottom item.
- Focus index tracks across both top and bottom item lists.
- Use `IndexedControlState` from `ControlState.h` for tracking.
- Announce focused item via screen reader on focus change.

**Files Affected:**
- `src/ui/ActivityBar.cpp` (modify -- enhance OnKeyDown, OnSetFocus, OnKillFocus)

**Acceptance Criteria:**
- Pressing Down repeatedly cycles through all visible items.
- Enter activates the focused item and switches the sidebar.
- Screen reader announces each item as it receives focus.

**Dependencies:** Task 1, Phase 05 Task 11.

---

### Task 7: Implement Tooltip with Rich Content

**Description:** Show a tooltip on hover that displays the item name and any badge information. Replace the current basic tooltip with a custom-drawn tooltip popup.

**Key Implementation Details:**
- Class: `ActivityBarTooltip` in `src/ui/ActivityBarTooltip.h` (lightweight popup wxWindow).
- Content: icon name (bold) + badge text (if any, e.g., "3 errors").
- Show after 500ms delay (using `TooltipAnimator` from Phase 04).
- Position: to the right of the hovered item, vertically centered.
- Fade in over 150ms.
- Dismiss immediately on mouse leave or click.
- Background: `resolve("editorHoverWidget.background")`.
- Border: `ThemeColorToken::BorderLight`, 1px.
- Shadow: `ElevationLevel::kMedium`.

**Files Affected:**
- `src/ui/ActivityBarTooltip.h` (new)
- `src/ui/ActivityBarTooltip.cpp` (new)
- `src/ui/ActivityBar.cpp` (modify -- use tooltip)

**Acceptance Criteria:**
- Hovering an item shows a tooltip after 500ms delay.
- Tooltip contains the item name.
- Tooltip has a shadow and themed background.

**Dependencies:** Phase 04 Task 17.

---

### Task 8: Implement Drag-to-Reorder

**Description:** Allow users to drag activity bar items to reorder them. The existing drag infrastructure (`drag_index_`, `drag_target_index_`) needs to be connected to visual feedback and model updates.

**Key Implementation Details:**
- On mouse down + move (>5px threshold): enter drag mode.
- During drag:
  - Draw the dragged item at the cursor position with 50% opacity.
  - Draw a drop indicator line at the insertion point.
  - Highlight the target slot.
- On drop:
  - Update `ActivityBarModel::set_order()` for the dragged item.
  - Animate items shifting to their new positions (150ms, ease_out_quad).
- Bottom-section items cannot be reordered into the top section (and vice versa).
- Persist the custom order in Config.

**Files Affected:**
- `src/ui/ActivityBar.cpp` (modify -- implement drag feedback and model update)
- `src/ui/ActivityBarModel.cpp` (modify -- persist order)

**Acceptance Criteria:**
- Dragging an item shows it at the cursor with a ghost appearance.
- Dropping between items reorders them.
- Bottom items cannot be dragged to the top section.
- Order persists across restarts.

**Dependencies:** Task 1, Phase 04 Task 2.

---

### Task 9: Implement Context Menu

**Description:** Right-clicking an activity bar item shows a context menu with options to hide the item, reset order, and toggle badge visibility.

**Key Implementation Details:**
- Enhance existing `OnRightClick` handler:
- Context menu items:
  - "Hide [Item Name]" -- hides the item from the bar.
  - "Show All" -- restores all hidden items.
  - "Reset Order" -- restores default order.
  - "---" (separator)
  - Per-item toggle for visible items in the bar.
- Use `wxMenu` for the context menu.
- "Hide" sets `ActivityBarModel::set_visible(id, false)`.
- At least 2 top items must remain visible (prevent hiding all).

**Files Affected:**
- `src/ui/ActivityBar.cpp` (modify -- enhance OnRightClick)

**Acceptance Criteria:**
- Right-clicking shows the context menu.
- "Hide" removes the item from the bar.
- "Show All" restores hidden items.
- Cannot hide all items (minimum 2 enforced).

**Dependencies:** Task 1.

---

### Task 10: Implement Bottom Section Separator

**Description:** Render a visual separator between the top items and bottom items (Settings, Account) in the activity bar.

**Key Implementation Details:**
- Draw a 1px horizontal line between the last top item and the first bottom item.
- Color: `resolve("activityBar.foreground")` at 20% opacity.
- Spacing: 8px above and below the separator (`SpacingGrid::scaled(kSm)`).
- Bottom items are aligned to the bottom of the bar; top items are aligned to the top.
- The separator flexes vertically between the two groups.

**Files Affected:**
- `src/ui/ActivityBar.cpp` (modify -- add separator rendering in OnPaint)

**Acceptance Criteria:**
- Separator is visible between top and bottom groups.
- Separator is correctly positioned when the window is resized.
- Separator respects theme opacity.

**Dependencies:** Task 1.

---

### Task 11: Implement Settings Gear Item (Bottom)

**Description:** Add a Settings gear icon as a persistent bottom item that opens the settings panel.

**Key Implementation Details:**
- Registered as a bottom-section item in `ActivityBarModel` with id `"settings"`.
- Icon: `"gear"` from the icon registry.
- Click action: publishes `ActivityBarSelectionEvent` with `ActivityBarItem::Settings`.
- Sidebar switches to Settings panel.
- Tooltip: "Settings (Ctrl+,)".
- No badge by default.

**Files Affected:**
- `src/ui/ActivityBarModel.cpp` (modify -- register settings item)
- `src/ui/ActivityBar.cpp` (modify -- handle settings click)

**Acceptance Criteria:**
- Settings gear appears at the bottom of the activity bar.
- Clicking it switches to the Settings sidebar panel.
- Tooltip shows the keyboard shortcut.

**Dependencies:** Tasks 1, 10.

---

### Task 12: Implement Account Icon (Bottom)

**Description:** Add an Account/Profile icon as the bottommost item, for future user profile and sync features.

**Key Implementation Details:**
- Registered as a bottom-section item in `ActivityBarModel` with id `"account"`.
- Icon: `"account"` from the icon registry.
- Click action: opens a context menu with:
  - "Sign In..." (disabled -- future feature).
  - "Settings Sync..." (disabled -- future feature).
  - "Turn on Settings Sync..." (disabled -- future feature).
- Tooltip: "Account".
- Badge: dot indicator when sync is available (future).

**Files Affected:**
- `src/ui/ActivityBarModel.cpp` (modify)
- `src/ui/ActivityBar.cpp` (modify)

**Acceptance Criteria:**
- Account icon appears below the settings gear.
- Clicking shows a context menu (with disabled items).
- Future-proofed for sign-in and sync features.

**Dependencies:** Tasks 1, 10.

---

### Task 13: Implement Hover Effect

**Description:** Show a subtle background highlight when hovering over an activity bar item, using the design system tokens.

**Key Implementation Details:**
- On hover: draw a rounded rectangle background behind the icon.
- Color: `resolve("activityBar.foreground")` at 10% opacity (or `ThemeColorToken::HoverBg`).
- Corner radius: 4px.
- Animate opacity from 0 to target over 100ms on enter, from target to 0 on leave.
- Active item always shows its active background (no hover overlay needed).

**Files Affected:**
- `src/ui/ActivityBar.cpp` (modify -- add hover background in OnPaint)

**Acceptance Criteria:**
- Hovering an inactive item shows a subtle background highlight.
- Active item does not show an additional hover effect.
- Hover animates smoothly.

**Dependencies:** Phase 04 Task 3.

---

### Task 14: Implement Press Feedback

**Description:** Show a visual press effect when clicking an activity bar item.

**Key Implementation Details:**
- On mouse down: draw the background at higher opacity (20%) and scale the icon to 0.9x.
- On mouse up: restore normal appearance.
- Use `ControlStateTracker` from `ControlState.h` for state management.
- Duration: the scale change is instant (no animation on press; animation only on release back to 1.0x).

**Files Affected:**
- `src/ui/ActivityBar.cpp` (modify)

**Acceptance Criteria:**
- Clicking an item shows a brief press feedback.
- The icon scales down slightly on press.
- Release returns to normal.

**Dependencies:** Phase 01 (ControlState).

---

### Task 15: Implement Double-Click to Collapse Sidebar

**Description:** Double-clicking an already-active activity bar item collapses/expands the primary sidebar (matching VS Code behavior).

**Key Implementation Details:**
- The existing `OnDoubleClick` handler fires `SidebarToggleEvent`.
- Ensure it only toggles when double-clicking the already-active item.
- If double-clicking a non-active item: first click activates, second click (within double-click threshold) collapses.

**Files Affected:**
- `src/ui/ActivityBar.cpp` (modify -- refine OnDoubleClick logic)

**Acceptance Criteria:**
- Double-clicking the active item toggles the sidebar.
- Double-clicking a non-active item does NOT toggle the sidebar (it switches to that panel).
- Works correctly with keyboard (double-press Enter on active item).

**Dependencies:** Phase 06 Task 7.

---

### Task 16: Wire Badge Sources to Real Data

**Description:** Connect badge indicators to actual data sources: search result count, problems/diagnostic count, extension update count.

**Key Implementation Details:**
- Search badge: subscribe to `SearchResultsCountEvent`, show count on Search item.
- Problems badge: subscribe to `DiagnosticsChangedEvent`, show error+warning count on Problems/Source Control item.
- Extensions badge: subscribe to `ExtensionUpdatesAvailableEvent`, show count on Extensions item.
- Clear badge when count reaches 0.
- Update via `ActivityBarModel::set_badge()`.

**Files Affected:**
- `src/ui/ActivityBar.cpp` (modify -- subscribe to events)

**Acceptance Criteria:**
- Running a search updates the Search item badge with result count.
- Lint errors update the relevant item badge.
- Extension updates show on the Extensions item.

**Dependencies:** Tasks 1, 4.

---

### Task 17: Implement Activity Bar Width Responsiveness

**Description:** When the window is very narrow, the activity bar can optionally collapse to a narrower mode or be completely hidden.

**Key Implementation Details:**
- Below 800px window width: activity bar shows icons at smaller size (20px instead of 24px).
- Below 600px window width: activity bar is hidden (replaced by a hamburger menu in the toolbar).
- Activity bar width adjusts via `ComponentSizeResolver` at compact density.
- A collapsed state stores the bar items in a dropdown menu.

**Files Affected:**
- `src/ui/ActivityBar.cpp` (modify -- responsive size)
- `src/ui/WorkbenchShell.cpp` (modify -- respond to window width)

**Acceptance Criteria:**
- Narrow windows show smaller activity bar icons.
- Very narrow windows hide the activity bar.
- Hamburger menu provides access to all items when bar is hidden.

**Dependencies:** Phase 06 Task 18.

---

### Task 18: Implement Extension-Contributed Items

**Description:** Allow extensions to register activity bar items via the `PluginContext` API.

**Key Implementation Details:**
- Add to `PluginContext`:
  - `auto register_activity_bar_item(const ActivityBarItemConfig& config) -> bool`.
  - `auto unregister_activity_bar_item(const std::string& id) -> bool`.
- Extension items appear after built-in items in the top section.
- Extension items can have custom icons (provided as SVG strings).
- Extension items can set badge state.
- Maximum 10 extension-contributed items (prevent bar overflow).

**Files Affected:**
- `src/core/PluginContext.h` (modify -- add activity bar API)
- `src/ui/ActivityBarModel.cpp` (modify -- handle extension items)

**Acceptance Criteria:**
- An extension can register a custom activity bar item.
- The item appears in the bar with its custom icon.
- Unregistering removes it from the bar.

**Dependencies:** Task 1.

---

### Task 19: Implement Activity Bar Overflow Handling

**Description:** When there are too many items to fit in the bar, show an overflow indicator that reveals hidden items in a dropdown.

**Key Implementation Details:**
- Calculate available slots: `(bar_height - bottom_section_height - separator_height) / slot_height`.
- If total top items > available slots:
  - Show an overflow chevron (`"chevron-down"` icon) as the last visible slot.
  - Clicking the chevron shows a dropdown menu with the overflow items.
  - Overflow items show their icon, label, and badge.
- Priority: built-in items before extension items for visibility.

**Files Affected:**
- `src/ui/ActivityBar.cpp` (modify -- add overflow detection and rendering)

**Acceptance Criteria:**
- With 20+ items, overflow chevron appears.
- Clicking the chevron shows a dropdown with hidden items.
- Selecting from the dropdown activates the item.

**Dependencies:** Task 1.

---

### Task 20: Persist Activity Bar Configuration

**Description:** Save the user's activity bar configuration (item order, hidden items, badge preferences) to Config.

**Key Implementation Details:**
- Config keys:
  - `activityBar.order` -- JSON array of item IDs in display order.
  - `activityBar.hidden` -- JSON array of hidden item IDs.
- Saved on every reorder or hide action (debounced).
- Loaded on startup to restore the configuration.

**Files Affected:**
- `src/ui/ActivityBarModel.cpp` (modify -- save/load from Config)

**Acceptance Criteria:**
- Custom item order persists across restarts.
- Hidden items remain hidden on restart.
- Corrupted config falls back to default order.

**Dependencies:** Task 1.

---

### Task 21: Implement Full Accessibility for Activity Bar

**Description:** Ensure the activity bar is fully accessible with correct ARIA roles, labels, states, and keyboard interaction patterns.

**Key Implementation Details:**
- Container role: `kToolbar`.
- Each item role: `kTab` (matching VS Code's `role="tab"` for activity bar items).
- Labels: item tooltip text.
- States: "selected" for active item.
- Badge announced as part of the item description: "Explorer, tab, selected" or "Extensions, tab, 3 updates available".
- Tab / arrow key navigation per Phase 05.
- Focus ring rendered per Phase 05.

**Files Affected:**
- `src/ui/ActivityBar.cpp` (modify -- register all items with AccessibilityController)

**Acceptance Criteria:**
- VoiceOver reads correct role, label, and state for each item.
- Arrow keys navigate between items with announcements.
- Badge information is included in the description.

**Dependencies:** Phase 05 Tasks 7, 11.

---

### Task 22: Implement Theme-Aware Rendering Polish

**Description:** Apply final rendering polish: anti-aliased edges, sub-pixel positioning, proper alpha blending for overlapping elements.

**Key Implementation Details:**
- Ensure `wxGraphicsContext` anti-aliasing is enabled.
- Icon rendering uses alpha blending for transparency.
- Badge pill shape uses proper rounded corners (half-height radius).
- Active indicator bar has rounded ends (2px radius).
- Hover background has proper alpha blending (no overdraw).
- Test all visual states across dark, light, and high-contrast themes.

**Files Affected:**
- `src/ui/ActivityBar.cpp` (modify -- rendering polish)

**Acceptance Criteria:**
- No aliasing artifacts on icons or indicators.
- Correct rendering in all three base themes.
- No overdraw or blending artifacts on hover+active states.

**Dependencies:** Tasks 2, 3, 4, 13.

---

### Task 23: Implement Activity Bar Vertical Scroll (Rare Case)

**Description:** For extremely small windows where even the overflow handling cannot accommodate the minimum items, allow the activity bar to scroll vertically.

**Key Implementation Details:**
- If window height < (min_visible_items * slot_height + bottom_section + separator):
  - Enable vertical scrolling within the top section.
  - Show a subtle scroll indicator (gradient fade) at top/bottom of the scrollable area.
  - Mouse wheel scrolls the items.
  - Scroll position is not persisted.

**Files Affected:**
- `src/ui/ActivityBar.cpp` (modify -- add scroll support)

**Acceptance Criteria:**
- Very short windows allow scrolling the activity bar items.
- Scroll indicators appear at the edges.
- Mouse wheel works for scrolling.

**Dependencies:** Task 19.

---

### Task 24: Performance Optimization

**Description:** Ensure the activity bar renders efficiently with no unnecessary repaints.

**Key Implementation Details:**
- Only repaint the changed item slot on hover/unhover (not the entire bar).
- Use `wxWindow::RefreshRect(item_rect)` for targeted repaints.
- Cache the icon bitmaps (via `IconCache`) so they are not re-rendered on every paint.
- Badge text measurement is cached and only recalculated when count changes.
- Profile paint time and ensure it stays under 1ms for the full bar.

**Files Affected:**
- `src/ui/ActivityBar.cpp` (modify)

**Acceptance Criteria:**
- Hovering does not repaint the entire bar (only the affected slot).
- Paint time < 1ms measured with `MARKAMP_LOG_DEBUG`.
- Icon bitmaps are retrieved from cache, not re-rendered.

**Dependencies:** Phase 02 Task 5.

---

### Task 25: Catch2 Test Suite for Activity Bar V2

**Description:** Write comprehensive tests for the redesigned activity bar.

**Key Implementation Details:**
- Test file: `tests/unit/test_activity_bar_v2.cpp`
- Test target: `test_activity_bar_v2` in CMakeLists.txt
- Test sections:
  - `ActivityBarModel`: register/unregister, set_order, set_visible, set_badge, top/bottom items.
  - Badge state: kNone/kDot/kCount/kUrgent transitions, count clamping at 99+.
  - Item ordering: default order, custom reorder, persist/restore.
  - Visibility: hide/show, minimum 2 items enforced.
  - Keyboard navigation: Up/Down/Enter/Home/End.
  - Context menu: hide item, show all, reset order.
  - Overflow: items exceeding capacity trigger overflow.
  - Extension items: register/unregister, maximum enforced.
  - Drag reorder: model state after reorder.
  - Accessibility: all items have names and roles.

**Files Affected:**
- `tests/unit/test_activity_bar_v2.cpp` (new)
- `CMakeLists.txt` (modify)

**Acceptance Criteria:**
- All tests pass.
- At least 45 test assertions.
- No dependency on wxWidgets rendering.

**Dependencies:** All previous tasks.

---

## Files Created

| File | Type |
|------|------|
| `src/ui/ActivityBarTooltip.h` / `.cpp` | Tooltip popup |
| `tests/unit/test_activity_bar_v2.cpp` | Test file |

## Files Modified

| File | Change |
|------|--------|
| `src/ui/ActivityBarModel.h` / `.cpp` | Dynamic items, badges, persistence |
| `src/ui/ActivityBar.h` / `.cpp` | Complete rendering + interaction rewrite |
| `src/core/PluginContext.h` | Activity bar extension API |
| `src/ui/WorkbenchShell.cpp` | Responsive activity bar sizing |
| `CMakeLists.txt` | New sources, test target |
