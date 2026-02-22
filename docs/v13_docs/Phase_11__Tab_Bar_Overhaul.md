# Phase 11: Tab Bar Overhaul

## Objective

Transform the existing `TabBar` from a functional but basic tab strip into a professional-grade editor tab system rivaling VSCode's tab bar. The current implementation (1378 lines in `TabBar.cpp`) already supports close buttons, drag reorder, pinned tabs, context menus, fade-in animation, modified dot indicators, group color tinting, and overflow scrolling. This phase adds file-type icons, preview tab semantics, configurable tab sizing modes, tab groups with explicit color assignments, an overflow dropdown, cross-group drag, saving spinner animation, and enhanced tooltip formatting.

## Prerequisites

- Phase 01-10 (prior UI foundation phases) completed
- Existing `TabBar` class at `src/ui/TabBar.h` / `src/ui/TabBar.cpp`
- `ThemeEngine` providing all color tokens (`AccentPrimary`, `AccentSecondary`, `TextMain`, `TextMuted`, etc.)
- `EventBus` with existing tab events (`TabSwitchedEvent`, `TabCloseRequestEvent`, `TabSaveRequestEvent`, `TabDuplicateRequestEvent`)
- `core::FileNode` type for file metadata

## Deliverables

Upgraded `TabBar` with 25 tasks covering file-type icons, preview tabs, tab size modes, tab groups, overflow dropdown, cross-group drag, saving spinner, and rich tooltips.

---

## Task 1: File-Type Icon Registry

**Title:** Create a file extension to icon mapping registry

**Description:** Build a registry that maps file extensions (`.cpp`, `.h`, `.py`, `.js`, `.md`, `.json`, `.yaml`, `.ts`, `.css`, `.html`, `.xml`, `.rs`, `.go`, `.java`, `.rb`, `.sh`, `.toml`, `.cmake`, `.txt`) to small 14x14 icon drawing routines using `wxGraphicsContext`. Each icon should be a recognizable glyph (e.g., C++ files get a blue "C++" text, Markdown gets a purple "M" with down arrow, Python gets a green snake shape, JSON gets curly braces).

**Implementation Details:**
- Create `FileTypeIconRegistry` class in `src/ui/FileTypeIconRegistry.h` / `.cpp`
- Use a `std::unordered_map<std::string, FileTypeIconId>` mapping extensions to enum IDs
- Each icon rendered via dedicated `Draw*Icon(wxGraphicsContext&, double x, double y, double size, const wxColour& tint)` methods
- Color tints sourced from `ThemeEngine` syntax tokens (`SyntaxKeyword`, `SyntaxString`, `SyntaxType`, etc.)
- Provide a `DrawFileIcon(wxGraphicsContext&, const std::string& filename, double x, double y, double size)` facade method
- Fallback to generic file icon (page glyph) for unknown extensions

**Files Affected:**
- `src/ui/FileTypeIconRegistry.h` (new)
- `src/ui/FileTypeIconRegistry.cpp` (new)
- `CMakeLists.txt` (add to `add_executable` and `source_group`)

**Acceptance Criteria:**
- Registry returns correct icon ID for all 20+ supported extensions
- Unknown extensions get fallback icon
- Icons render at 14x14 with theme-appropriate colors
- Unit test verifies mapping coverage for all registered extensions

**Dependencies:** None

---

## Task 2: Integrate File-Type Icons into Tab Rendering

**Title:** Draw file-type icon in each tab before the filename text

**Description:** Modify `TabBar::DrawTab()` to render the file-type icon to the left of the display name. The icon occupies 14x14 pixels with 4px right margin before the text. For pinned tabs, the icon appears after the pin indicator stripe. Adjust text area width calculations to account for the icon.

**Implementation Details:**
- Add `FileTypeIconRegistry* icon_registry_` member to `TabBar`
- In `DrawTab()`, after computing `text_x`, insert icon draw call:
  ```cpp
  constexpr int kFileIconSize = 14;
  constexpr int kFileIconMargin = 4;
  icon_registry_->DrawFileIcon(gc, tab.file_path, text_x, tab_y + (tab_h - kFileIconSize) / 2, kFileIconSize);
  text_x += kFileIconSize + kFileIconMargin;
  text_max_w -= kFileIconSize + kFileIconMargin;
  ```
- Pass `FileTypeIconRegistry&` in `TabBar` constructor

**Files Affected:**
- `src/ui/TabBar.h` (add member, update constructor, add constants)
- `src/ui/TabBar.cpp` (modify `DrawTab()`, update constructor)
- `src/ui/LayoutManager.cpp` (pass registry to TabBar constructor)

**Acceptance Criteria:**
- Each tab displays file-type icon matching its extension
- Icons vertically centered in the tab
- Text layout does not overlap with icon
- Pinned tabs show pin stripe, then icon, then filename

**Dependencies:** Task 1

---

## Task 3: Preview Tab Mode (Single-Click Italic)

**Title:** Implement preview tab semantics for single-click file opens

**Description:** When a file is single-clicked in the FileTreeCtrl, it opens as a "preview" tab. Preview tabs display their name in italic font, are ephemeral (replaced when another file is single-clicked), and convert to permanent tabs on double-click or edit. Only one preview tab exists at a time. The concept mirrors VSCode's preview mode.

**Implementation Details:**
- Add `bool is_preview{false}` to `TabInfo` struct
- Add `void AddPreviewTab(const std::string& file_path, const std::string& display_name)` method
- `AddPreviewTab()` closes any existing preview tab before adding the new one
- `void PromotePreviewTab(const std::string& file_path)` converts preview to permanent
- In `DrawTab()`, apply `wxFONTSTYLE_ITALIC` to font when `is_preview == true`
- Wire `OnEditorChange` to auto-promote on first edit
- Add `TabPreviewPromotedEvent` to Events.h
- Wire FileTreeCtrl single-click to `AddPreviewTab`, double-click to `AddTab` (permanent)

**Files Affected:**
- `src/ui/TabBar.h` (add `is_preview` field, new methods)
- `src/ui/TabBar.cpp` (implement preview logic, modify `DrawTab()`)
- `src/core/Events.h` (add `TabPreviewPromotedEvent`)
- `src/ui/LayoutManager.cpp` (wire preview tab behavior)

**Acceptance Criteria:**
- Single-click in file tree opens preview tab (italic name)
- Only one preview tab exists at a time
- Double-click in file tree opens permanent tab
- Editing content promotes preview to permanent
- Preview tabs closed when another single-click opens a different file
- Promoted tab shows normal (non-italic) font

**Dependencies:** None

---

## Task 4: Tab Size Mode - Shrink Mode

**Title:** Implement shrinking tab width mode

**Description:** In shrink mode, all tabs reduce their width proportionally so that all tabs fit within the visible tab bar width without scrolling. When the total natural width of all tabs exceeds the bar width, each tab shrinks to `available_width / tab_count`, clamped to `kMinTabWidth`. If all tabs at minimum width still overflow, fall back to scroll mode.

**Implementation Details:**
- Add `enum class TabSizeMode { Shrink, Scroll, Fixed }` to `TabBar.h`
- Add `TabSizeMode tab_size_mode_{TabSizeMode::Scroll}` member
- Add `void SetTabSizeMode(TabSizeMode mode)` setter
- In `RecalculateTabRects()`, when mode is `Shrink`:
  ```cpp
  int total_natural = sum of all tab target_widths;
  int available = GetClientSize().GetWidth();
  if (total_natural > available) {
      int shrunk = available / static_cast<int>(tabs_.size());
      shrunk = std::max(shrunk, kMinTabWidth);
      // assign shrunk width to each tab
  }
  ```
- Add setting to config for default tab size mode

**Files Affected:**
- `src/ui/TabBar.h` (add enum, member, setter)
- `src/ui/TabBar.cpp` (modify `RecalculateTabRects()`)

**Acceptance Criteria:**
- In shrink mode, all tabs visible without scrolling when possible
- Tabs never shrink below `kMinTabWidth` (100px)
- When minimum width still causes overflow, scroll offset is respected
- Mode persists across tab additions and removals

**Dependencies:** None

---

## Task 5: Tab Size Mode - Fixed Width Mode

**Title:** Implement fixed-width tab sizing

**Description:** In fixed mode, all tabs render at the same width regardless of filename length. The fixed width is configurable (default: 150px). Long filenames are truncated with ellipsis.

**Implementation Details:**
- Add `int fixed_tab_width_{150}` member
- Add `void SetFixedTabWidth(int width)` setter
- In `RecalculateTabRects()`, when mode is `Fixed`, set all `target_width = fixed_tab_width_`
- Ensure ellipsis truncation in `DrawTab()` works correctly with uniform widths

**Files Affected:**
- `src/ui/TabBar.h` (add member, setter)
- `src/ui/TabBar.cpp` (modify `RecalculateTabRects()`)

**Acceptance Criteria:**
- All tabs render at exactly the configured fixed width
- Long filenames properly truncated with ellipsis
- Fixed width configurable between `kMinTabWidth` and `kMaxTabWidth`

**Dependencies:** Task 4

---

## Task 6: Middle-Click Close Enhancement

**Title:** Ensure middle-click close works with visual feedback

**Description:** The existing `OnMiddleDown` handler already closes tabs on middle-click. Enhance it with a brief flash animation (the tab background flashes red for 100ms before closing) to provide visual confirmation of the close action.

**Implementation Details:**
- Add `int closing_tab_index_{-1}` and `wxTimer close_flash_timer_` members
- In `OnMiddleDown()`, set `closing_tab_index_` and start a 100ms timer
- In `DrawTab()`, if `closing_tab_index_ == current_index`, draw background with `ErrorColor` tint
- On timer fire, publish `TabCloseRequestEvent` and reset state
- Also apply same flash to close button click

**Files Affected:**
- `src/ui/TabBar.h` (add flash members)
- `src/ui/TabBar.cpp` (modify `OnMiddleDown`, `DrawTab`, add timer handler)

**Acceptance Criteria:**
- Middle-click shows brief red flash on tab before close
- Close button click also shows flash
- Flash duration is 100ms
- Tab correctly removed after flash completes

**Dependencies:** None

---

## Task 7: Explicit Tab Groups with Color Assignment

**Title:** Implement user-assignable tab groups with named colors

**Description:** Replace the current implicit directory-based group tinting with explicit, user-assignable tab groups. Users can assign tabs to named groups (e.g., "API", "Tests", "UI") via the context menu. Each group gets a distinct color from a palette. Groups persist across sessions.

**Implementation Details:**
- Add `std::string group_name` to `TabInfo` struct (empty = no group)
- Add `struct TabGroup { std::string name; int color_index; }` and `std::vector<TabGroup> tab_groups_`
- Add context menu item "Move to Group >" with submenu listing existing groups + "New Group..."
- Add `void AssignTabToGroup(const std::string& file_path, const std::string& group_name)`
- Add `void RemoveTabFromGroup(const std::string& file_path)`
- Modify `DrawTab()` to use explicit group color when `group_name` is set (instead of directory-based)
- Draw a small 3px color bar at the bottom of grouped tabs
- Add group color in the tab tooltip
- Persist group assignments to config

**Files Affected:**
- `src/ui/TabBar.h` (add group structures, methods)
- `src/ui/TabBar.cpp` (implement group assignment, modify `DrawTab`, modify `ShowTabContextMenu`)
- `src/core/Config.h` / `.cpp` (persist tab groups)

**Acceptance Criteria:**
- Right-click menu shows "Move to Group" submenu
- New groups can be created with custom names
- Group color bar visible at bottom of grouped tabs
- Tabs can be removed from groups
- Group assignments persist across application restarts

**Dependencies:** None

---

## Task 8: Enhanced Tab Context Menu

**Title:** Add all VSCode-equivalent context menu actions

**Description:** Extend the existing context menu (which already has Close, Close Others, Close All, Close Saved, Close to Left, Close to Right, Save, Save As, Copy Path, Copy Relative Path, Reveal in Finder, Pin/Unpin, Duplicate) with "Split Right" and "Split Down" actions that fire events to create new editor groups.

**Implementation Details:**
- Add `kContextSplitRight = 15` and `kContextSplitDown = 16` constants
- Add menu items after "Duplicate Tab" separator:
  ```cpp
  menu.AppendSeparator();
  menu.Append(kContextSplitRight, "Split Right");
  menu.Append(kContextSplitDown, "Split Down");
  ```
- Create `TabSplitRightRequestEvent` and `TabSplitDownRequestEvent` in Events.h
- Publish the appropriate event with the file path
- Events will be consumed by the editor group system (Phase 12)

**Files Affected:**
- `src/ui/TabBar.cpp` (add menu items, handle events)
- `src/core/Events.h` (add split request events)

**Acceptance Criteria:**
- "Split Right" and "Split Down" appear in context menu
- Clicking them publishes the correct event with file path
- Menu items are disabled if editor group system is not available (graceful degradation)

**Dependencies:** None (Phase 12 will wire the handlers)

---

## Task 9: Tab Overflow Dropdown

**Title:** Implement overflow dropdown showing list of all open tabs

**Description:** When tabs overflow the visible area, replace the simple chevron indicator with a clickable dropdown button. Clicking it shows a popup menu listing all open tabs with their file-type icons, modified indicators, and group color dots. Selecting a tab from the dropdown activates and scrolls to it.

**Implementation Details:**
- Add `wxRect overflow_button_rect_` member for hit testing
- In `OnPaint()`, replace the chevron drawing with a proper button area (20px wide) showing a "v" glyph with tab count badge
- Add `void OnOverflowClick()` method that creates a `wxMenu` with all tab paths
- Each menu item text: `[icon] filename  (parent_dir)` with modified dot if applicable
- Bind menu selection to `SetActiveTab()` + scroll
- Hit test the overflow button in `OnMouseDown()`
- Show overflow button only when tabs actually overflow

**Files Affected:**
- `src/ui/TabBar.h` (add overflow button rect, method)
- `src/ui/TabBar.cpp` (modify paint, add overflow menu logic)

**Acceptance Criteria:**
- Overflow button appears only when tabs exceed visible width
- Clicking overflow shows dropdown with all tabs listed
- Each entry shows filename and parent directory
- Modified tabs show indicator in dropdown
- Selecting a tab activates it and scrolls into view
- Active tab is checked/highlighted in the dropdown

**Dependencies:** None

---

## Task 10: Tab Drag Between Editor Groups

**Title:** Implement cross-group tab dragging via EventBus

**Description:** Enable dragging a tab from one editor group's tab bar to another. When a tab is dragged past the edge of its tab bar, publish a `TabDragOutEvent` containing the file path and drop coordinates. The editor group system (Phase 12) will handle creating or moving to the target group.

**Implementation Details:**
- Extend existing drag logic in `OnMouseMove()` to detect when drag moves outside the tab bar bounds
- Add `wxRect GetScreenRect()` helper to determine tab bar screen coordinates
- When `event.GetPosition().y` exceeds tab bar bounds by more than 20px during drag:
  ```cpp
  core::events::TabDragOutEvent evt(tabs_[drag_tab_index_].file_path, event.GetPosition());
  event_bus_.publish(evt);
  ```
- Add `void AcceptDroppedTab(const std::string& file_path, const std::string& display_name)` for receiving drops
- Create `TabDragOutEvent` and `TabDropAcceptEvent` in Events.h

**Files Affected:**
- `src/ui/TabBar.h` (add `AcceptDroppedTab`, screen rect helper)
- `src/ui/TabBar.cpp` (extend drag detection in `OnMouseMove`)
- `src/core/Events.h` (add drag/drop events)

**Acceptance Criteria:**
- Dragging a tab outside the tab bar vertically publishes a drag-out event
- Event contains file path and screen coordinates
- Tab bar can accept dropped tabs from other groups
- Drag indicator shows during cross-group drag

**Dependencies:** None (Phase 12 will wire the handlers)

---

## Task 11: Modified Dot Indicator Enhancement

**Title:** Upgrade modified dot to themed, pulsing indicator

**Description:** Enhance the existing modified dot (6px filled circle) to pulse subtly when unsaved changes exist and to change color based on the time since last save (recent = accent, stale = warning after 5 minutes).

**Implementation Details:**
- Add `std::chrono::steady_clock::time_point last_save_time` to `TabInfo`
- In `DrawTab()`, compute time since save:
  ```cpp
  auto elapsed = std::chrono::steady_clock::now() - tab.last_save_time;
  bool stale = elapsed > std::chrono::minutes(5);
  wxColour dot_color = stale ? theme_engine().color(ThemeColorToken::WarningColor)
                             : theme_engine().color(ThemeColorToken::AccentSecondary);
  ```
- Add a gentle pulse animation by varying dot size between 5-7px on a 2-second cycle using a sine wave
- Pulse only when the tab is active and modified
- Update `last_save_time` when save events are received

**Files Affected:**
- `src/ui/TabBar.h` (add `last_save_time` to TabInfo)
- `src/ui/TabBar.cpp` (modify dot drawing in `DrawTab`, update save time)

**Acceptance Criteria:**
- Modified dot pulses gently (5-7px) on active modified tab
- Dot changes to warning color after 5 minutes without save
- Pulse animation is smooth at 60fps
- Save resets the timer and returns dot to accent color

**Dependencies:** None

---

## Task 12: Saving Spinner Animation

**Title:** Show a spinner animation on the tab while save is in progress

**Description:** When a file save is initiated, replace the modified dot with a spinning indicator (3-frame rotation) for the duration of the save operation. The spinner runs until a save-complete event is received.

**Implementation Details:**
- Add `bool is_saving{false}` and `int save_spinner_frame{0}` to `TabInfo`
- Subscribe to `TabSaveRequestEvent` to set `is_saving = true`
- Subscribe to `TabSaveCompleteEvent` to set `is_saving = false`
- Add `TabSaveCompleteEvent` to Events.h if not present
- In `DrawTab()`, when `is_saving`, draw a 3-frame spinning indicator:
  - Frame 0: "/" line
  - Frame 1: "--" line
  - Frame 2: "\\" line
- Use the existing `fade_timer_` (16ms / 60fps) to advance spinner frames every 4 ticks (15fps spinner)
- Spinner drawn at same position as modified dot

**Files Affected:**
- `src/ui/TabBar.h` (add saving state to TabInfo)
- `src/ui/TabBar.cpp` (draw spinner, handle save events)
- `src/core/Events.h` (add `TabSaveCompleteEvent` if needed)

**Acceptance Criteria:**
- Spinner appears during save operation
- Spinner replaces modified dot
- Spinner disappears on save completion
- Spinner animates at ~15fps
- Multiple simultaneous saves show individual spinners per tab

**Dependencies:** None

---

## Task 13: Enhanced Tab Tooltips with Rich Formatting

**Title:** Implement rich tooltips showing full path, file size, and status

**Description:** Replace the current simple file path tooltip with a multi-line tooltip showing: full absolute path, relative path from workspace root, file size, last modified date, modified status, and language type.

**Implementation Details:**
- Override tooltip logic in `OnMouseMove()`:
  ```cpp
  std::string tooltip;
  tooltip += tab.file_path + "\n";
  if (!workspace_root_.empty()) {
      tooltip += "Relative: " + std::filesystem::relative(tab.file_path, workspace_root_).string() + "\n";
  }
  tooltip += "Language: " + GetLanguageName(tab.file_path) + "\n";
  tooltip += tab.is_modified ? "Status: Modified (unsaved)" : "Status: Saved";
  if (tab.is_pinned) tooltip += " | Pinned";
  if (tab.is_preview) tooltip += " | Preview";
  ```
- Add `GetLanguageName(const std::string& path)` helper based on extension
- Format file size if available (from FileBuffer metadata in LayoutManager)

**Files Affected:**
- `src/ui/TabBar.h` (add helper method)
- `src/ui/TabBar.cpp` (modify tooltip generation in `OnMouseMove`)

**Acceptance Criteria:**
- Tooltip shows full path on first line
- Relative path shown when workspace root is set
- Language name derived from extension
- Modified/saved status displayed
- Pinned and preview status shown when applicable
- Tooltip for close button still shows "Close tab"

**Dependencies:** Task 3 (preview mode)

---

## Task 14: Tab Group Header Bar

**Title:** Render group name header above grouped tab clusters

**Description:** When tabs are assigned to groups, render a thin (16px) header bar above each contiguous cluster of same-group tabs showing the group name and color. The header is clickable to collapse/expand the group.

**Implementation Details:**
- Add `bool show_group_headers_{true}` setting
- In `OnPaint()`, before drawing tabs, scan for group boundaries and draw headers
- Header: 16px tall, background is group color at 20% opacity, text is group name in 9pt font
- Clicking header toggles `is_group_collapsed` state
- Collapsed groups show only their header (tabs hidden)
- Adjust total tab bar height to accommodate headers: `kHeight + (num_visible_groups * 16)`

**Files Affected:**
- `src/ui/TabBar.h` (add group header logic, constants)
- `src/ui/TabBar.cpp` (add header rendering, collapse logic)

**Acceptance Criteria:**
- Group headers render above grouped tab clusters
- Header shows group name and color
- Click header to collapse/expand group
- Collapsed groups hide their tabs
- Tab bar height adjusts dynamically

**Dependencies:** Task 7

---

## Task 15: Tab Bar Background Action Area

**Title:** Add actions to empty tab bar area

**Description:** The empty area to the right of tabs (when not all space is used) should support: double-click to create new untitled file (already implemented), right-click for "New File" / "Reopen Closed Tab" context menu, and a "+" button at the right edge for new file creation.

**Implementation Details:**
- Add a "+" button drawn at the rightmost visible position (after last tab or at bar edge)
- Size: 24x24, centered vertically, shows "+" glyph with `TextMuted` color, lightens on hover
- Hit-test the "+" button in `OnMouseDown()` and create a new untitled file on click
- Add right-click context menu for empty area with "New File", "New Untitled File", "Reopen Closed Tab"
- Maintain a `std::vector<std::string> recently_closed_` stack (max 10 entries)
- "Reopen Closed Tab" pops from the stack and publishes a file open event

**Files Affected:**
- `src/ui/TabBar.h` (add recently closed stack, "+" button rect)
- `src/ui/TabBar.cpp` (add empty area context menu, "+" button drawing and hit test)

**Acceptance Criteria:**
- "+" button visible at end of tab strip
- Clicking "+" creates a new untitled file
- Right-click on empty area shows context menu
- "Reopen Closed Tab" restores the most recently closed file
- Recently closed list holds up to 10 entries

**Dependencies:** None

---

## Task 16: Tab Sorting Options

**Title:** Add tab sorting by name, path, type, and modified status

**Description:** Add ability to sort tabs alphabetically by display name, by full path, by file extension type, or with modified tabs first. Accessible via context menu "Sort Tabs By" submenu.

**Implementation Details:**
- Add `enum class TabSortOrder { Manual, ByName, ByPath, ByType, ModifiedFirst }`
- Add `TabSortOrder sort_order_{TabSortOrder::Manual}` member
- Add `void SortTabs(TabSortOrder order)` method that reorders `tabs_` vector
- Add "Sort Tabs By" submenu to the context menu shown on right-click in empty area:
  - "Name (A-Z)"
  - "Path (A-Z)"
  - "File Type"
  - "Modified First"
- Sorting preserves pinned tabs at the front
- After sorting, recalculate rects and refresh

**Files Affected:**
- `src/ui/TabBar.h` (add enum, member, method)
- `src/ui/TabBar.cpp` (implement sorting, add context menu submenu)

**Acceptance Criteria:**
- All four sort orders work correctly
- Pinned tabs remain at front regardless of sort
- Manual order is the default (user-arranged via drag)
- Sort applies immediately with visual update

**Dependencies:** None

---

## Task 17: Tab Close Animation

**Title:** Animate tab close with slide-out effect

**Description:** When a tab is closed, instead of instant removal, animate it sliding out by shrinking its width to 0 over 150ms. Adjacent tabs slide to fill the gap.

**Implementation Details:**
- Add `bool is_closing{false}` and `float close_progress{0.0F}` to `TabInfo`
- In `RemoveTab()`, instead of immediately erasing from vector, set `is_closing = true`
- Drive animation via `fade_timer_` (already running at 16ms):
  - Each tick: `close_progress += 0.1F` (reaches 1.0 in ~160ms)
  - When `close_progress >= 1.0F`, erase from vector
- In `RecalculateTabRects()`, multiply width by `(1.0F - close_progress)` for closing tabs
- In `DrawTab()`, apply proportional opacity during close

**Files Affected:**
- `src/ui/TabBar.h` (add close animation fields to TabInfo)
- `src/ui/TabBar.cpp` (modify `RemoveTab`, `RecalculateTabRects`, `DrawTab`, fade timer)

**Acceptance Criteria:**
- Closing tab shrinks to 0 width over ~150ms
- Adjacent tabs smoothly slide to fill the gap
- Tab text and icon fade out during animation
- Multiple simultaneous closes animate independently
- Tab fully removed from data structure when animation completes

**Dependencies:** None

---

## Task 18: Tab Unsaved Changes Indicator on Close

**Title:** Confirm before closing modified tabs

**Description:** When a close action targets a modified tab, show a confirmation dialog with "Save", "Don't Save", and "Cancel" options. Apply to all close methods: close button, middle-click, context menu close, close others, close all.

**Implementation Details:**
- Add `bool ConfirmCloseIfModified(const std::string& file_path)` method
- Before publishing `TabCloseRequestEvent`, check `is_modified`:
  ```cpp
  if (tabs_[idx].is_modified) {
      wxMessageDialog dlg(this, "Save changes to " + tabs_[idx].display_name + "?",
                          "Unsaved Changes", wxYES_NO | wxCANCEL | wxICON_WARNING);
      dlg.SetYESNOLabels("Save", "Don't Save");
      int result = dlg.ShowModal();
      if (result == wxID_YES) { /* save first */ }
      else if (result == wxID_CANCEL) { return false; }
  }
  ```
- For batch close operations (Close All, Close Others), iterate and confirm each modified tab
- Add "Close All Saved" as a shortcut that skips confirmation (already exists as `CloseSavedTabs`)

**Files Affected:**
- `src/ui/TabBar.h` (add confirmation method)
- `src/ui/TabBar.cpp` (add confirmation logic before close events)

**Acceptance Criteria:**
- Closing a modified tab shows save/don't save/cancel dialog
- "Save" saves the file then closes
- "Don't Save" closes without saving
- "Cancel" aborts the close
- Batch operations confirm each modified tab individually
- Unmodified tabs close without confirmation

**Dependencies:** None

---

## Task 19: Tab Keyboard Navigation

**Title:** Add keyboard shortcuts for tab management

**Description:** Add keyboard shortcuts: Ctrl+Tab / Ctrl+Shift+Tab for cycling (already wired in LayoutManager), Ctrl+W for close active tab, Ctrl+Shift+T for reopen closed tab, Ctrl+1-9 for jump to Nth tab, Alt+Left/Right for move tab position.

**Implementation Details:**
- Subscribe to keyboard shortcut events in TabBar:
  - `TabCloseActiveEvent` -> close active tab (with confirmation)
  - `TabReopenClosedEvent` -> pop from recently_closed_ and open
  - `TabJumpToIndexEvent(int index)` -> activate Nth tab
  - `TabMoveLeftEvent` / `TabMoveRightEvent` -> swap active tab with neighbor
- Add these events to `Events.h`
- Register shortcuts in `CommandPalette` and `ShortcutEditor`

**Files Affected:**
- `src/ui/TabBar.h` (add subscription members)
- `src/ui/TabBar.cpp` (implement handlers)
- `src/core/Events.h` (add events)
- `src/ui/CommandPalette.cpp` (register commands)

**Acceptance Criteria:**
- Ctrl+W closes active tab (with confirmation if modified)
- Ctrl+Shift+T reopens last closed tab
- Ctrl+1 through Ctrl+9 activate Nth tab
- Alt+Left/Right moves active tab position
- All shortcuts discoverable in command palette

**Dependencies:** Task 15 (recently closed stack)

---

## Task 20: Pinned Tab Visual Refinement

**Title:** Upgrade pinned tab rendering to show icon-only compact mode

**Description:** Pinned tabs should render in a compact mode showing only the file-type icon (no filename text), similar to VSCode. The tab width shrinks to ~36px for pinned tabs. The pin emoji is removed and replaced by the accent stripe at the left edge. Unpinning restores full-width rendering.

**Implementation Details:**
- In `RecalculateTabRects()`, when `tab.is_pinned`:
  ```cpp
  constexpr int kPinnedTabWidth = 36;
  tab.target_width = kPinnedTabWidth;
  ```
- In `DrawTab()`, when pinned: draw only the file-type icon centered, no text, no close button
- Show close button only on hover for pinned tabs
- Keep the accent stripe indicator
- Tooltip shows full filename for pinned tabs

**Files Affected:**
- `src/ui/TabBar.h` (add constant)
- `src/ui/TabBar.cpp` (modify `RecalculateTabRects` and `DrawTab` for pinned tabs)

**Acceptance Criteria:**
- Pinned tabs render at 36px width with icon only
- No filename text in pinned mode
- Close button appears on hover only
- Accent stripe visible on left edge
- Tooltip shows full filename
- Unpinning restores normal tab width with text

**Dependencies:** Task 2 (file-type icons)

---

## Task 21: Tab Dirty State Persistence

**Title:** Persist modified tab state across application restart

**Description:** When the application is closed with unsaved tabs, record their paths and modified flag. On restart, reopen these tabs and mark them as modified so the user knows they have unsaved changes.

**Implementation Details:**
- Add `void SaveTabState()` method that writes tab info to config:
  ```cpp
  // In YAML config:
  // open_tabs:
  //   - path: /path/to/file.md
  //     is_pinned: true
  //     is_modified: false
  //     group: "API"
  ```
- Add `void RestoreTabState()` method called during startup
- Save tab state on application close and periodically (every 60 seconds)
- Include scroll position and cursor position per tab

**Files Affected:**
- `src/ui/TabBar.h` (add save/restore methods)
- `src/ui/TabBar.cpp` (implement serialization)
- `src/ui/LayoutManager.cpp` (call save/restore during lifecycle)
- `src/core/Config.h` / `.cpp` (add tab state storage)

**Acceptance Criteria:**
- Tab state saved on application close
- Tabs restored on next launch with correct order
- Pinned status, group assignments preserved
- Modified flag correctly restored
- Non-existent files on disk reported as errors on restore

**Dependencies:** Task 7 (groups), Task 3 (preview mode)

---

## Task 22: Tab Bar Scroll Buttons

**Title:** Add left/right scroll arrow buttons when tabs overflow

**Description:** In addition to mouse wheel scrolling, add visible scroll arrow buttons at the left and right edges of the tab bar when tabs overflow. Left arrow scrolls back, right arrow scrolls forward.

**Implementation Details:**
- Draw left arrow when `scroll_offset_ > 0` (20px wide button at x=0)
- Draw right arrow when tabs extend past visible width (20px wide button at right edge)
- Each arrow: triangle glyph, `TextMuted` color, lightens on hover
- Click scrolls by one tab width
- Hold-to-scroll with 200ms repeat rate
- Arrows rendered on top of the fade gradient

**Files Affected:**
- `src/ui/TabBar.h` (add scroll button rects, timer for hold-to-scroll)
- `src/ui/TabBar.cpp` (draw arrows, handle clicks, hold-to-scroll)

**Acceptance Criteria:**
- Left arrow visible when scrolled right
- Right arrow visible when tabs overflow
- Single click scrolls by one tab width
- Hold triggers continuous scrolling at 200ms intervals
- Arrows disappear when scrolling is not needed

**Dependencies:** None

---

## Task 23: Tab Drag Visual Feedback Enhancement

**Title:** Improve drag reorder visual with tab ghost and insertion line

**Description:** Enhance the existing drag reorder (already showing insertion line and ghost shadow) with: a semi-transparent ghost of the dragged tab following the cursor, a smooth insertion line animation, and haptic-like visual pulse when crossing tab boundaries.

**Implementation Details:**
- During drag, draw a semi-transparent copy of the dragged tab at the mouse position:
  ```cpp
  if (is_dragging_) {
      // Draw ghost tab at mouse X
      DrawTab(gc, tabs_[drag_tab_index_], theme); // at mouse position with 50% opacity
  }
  ```
- Animate insertion line width from 0 to 2px over 50ms when changing position
- Add a brief (50ms) scale-up pulse (102%) on the tab being swapped past
- Ensure ghost tab doesn't clip outside the tab bar

**Files Affected:**
- `src/ui/TabBar.cpp` (enhance drag drawing in `OnPaint` and `OnMouseMove`)

**Acceptance Criteria:**
- Ghost tab follows cursor during drag
- Ghost is semi-transparent (50% opacity)
- Insertion line animates smoothly
- Visual pulse on boundary crossing provides feedback
- Ghost clipped to tab bar bounds

**Dependencies:** None

---

## Task 24: Tab Context Menu - Clipboard Operations

**Title:** Add Copy Filename, Copy Relative Path to context menu

**Description:** The context menu already has "Copy Path" and "Copy Relative Path". Add "Copy Filename" (just the file name without path) to complete the clipboard operation set.

**Implementation Details:**
- Add `kContextCopyFilename = 17` constant
- Add menu item after "Copy Relative Path":
  ```cpp
  menu.Append(kContextCopyFilename, "Copy Filename");
  ```
- Handler extracts filename from path:
  ```cpp
  case kContextCopyFilename: {
      auto filename = std::filesystem::path(target_path).filename().string();
      if (wxTheClipboard->Open()) {
          wxTheClipboard->SetData(new wxTextDataObject(filename));
          wxTheClipboard->Close();
      }
      break;
  }
  ```

**Files Affected:**
- `src/ui/TabBar.cpp` (add menu item and handler)

**Acceptance Criteria:**
- "Copy Filename" appears in context menu
- Copies just the filename (e.g., "TabBar.cpp") to clipboard
- Works for all file types including paths with special characters

**Dependencies:** None

---

## Task 25: Tab Bar Accessibility

**Title:** Add ARIA-equivalent accessibility support to tab bar

**Description:** Ensure the tab bar is fully accessible: proper focus management, keyboard navigation, screen reader announcements for tab switches, and high-contrast mode support.

**Implementation Details:**
- Set `wxAccessible` overrides for the tab bar (or use `wxACCESSIBLE_ROLE_PAGETAB`)
- Tab bar accepts focus via Tab key
- Left/Right arrows navigate between tabs when focused
- Enter activates the focused tab
- Delete closes the focused tab
- Screen reader text: "{filename} tab {N} of {total}, {modified status}"
- In high-contrast mode (detected via system settings), use solid borders instead of subtle tints
- Add `int focused_tab_index_{-1}` for keyboard focus ring (distinct from active tab)
- Draw focus ring as 2px dashed border around focused tab

**Files Affected:**
- `src/ui/TabBar.h` (add focus tracking, accessible interface)
- `src/ui/TabBar.cpp` (implement keyboard focus, accessibility text, focus ring drawing)

**Acceptance Criteria:**
- Tab key moves focus to tab bar
- Arrow keys navigate between tabs
- Enter activates, Delete closes
- Focus ring visually distinct from active indicator
- Accessibility name and role set for screen readers
- High-contrast mode detected and applied

**Dependencies:** None

---

## Estimated Complexity

| Area | Effort |
|------|--------|
| File-Type Icon Registry (Tasks 1-2) | Medium |
| Preview Tabs (Task 3) | Medium |
| Tab Size Modes (Tasks 4-5) | Low |
| Middle-Click Flash (Task 6) | Low |
| Tab Groups (Tasks 7, 14) | High |
| Context Menu Enhancements (Tasks 8, 24) | Low |
| Overflow Dropdown (Task 9) | Medium |
| Cross-Group Drag (Task 10) | Medium |
| Modified Indicators (Tasks 11-12) | Medium |
| Tooltips (Task 13) | Low |
| Background Actions (Task 15) | Medium |
| Sorting (Task 16) | Low |
| Close Animation (Task 17) | Medium |
| Close Confirmation (Task 18) | Low |
| Keyboard Navigation (Task 19) | Medium |
| Pinned Tab Compact (Task 20) | Medium |
| State Persistence (Task 21) | Medium |
| Scroll Buttons (Task 22) | Low |
| Drag Enhancement (Task 23) | Medium |
| Accessibility (Task 25) | High |

## Files Created

- `src/ui/FileTypeIconRegistry.h`
- `src/ui/FileTypeIconRegistry.cpp`

## Files Modified

- `src/ui/TabBar.h`
- `src/ui/TabBar.cpp`
- `src/ui/LayoutManager.cpp`
- `src/core/Events.h`
- `src/core/Config.h` / `src/core/Config.cpp`
- `src/ui/CommandPalette.cpp`
- `CMakeLists.txt`
