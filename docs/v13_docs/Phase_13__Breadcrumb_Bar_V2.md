# Phase 13: Breadcrumb Bar V2

## Objective

Overhaul the existing `BreadcrumbBar` from a simple static text label (currently a single `wxStaticText` showing a concatenated path string) into a fully interactive, segment-based navigation component. The current implementation at `src/ui/BreadcrumbBar.cpp` is 163 lines and uses a single label with Unicode separators. This phase replaces it with individually clickable segments, dropdown pickers for each segment, symbol breadcrumbs (class/method hierarchy), keyboard navigation, and proper custom rendering via `wxGraphicsContext`.

## Prerequisites

- Existing `BreadcrumbBar` at `src/ui/BreadcrumbBar.h` / `.cpp`
- `ThemeEngine` with font and color tokens
- `EventBus` for navigation events
- `EditorPanel::GetHeadingSymbols()` providing heading hierarchy
- `FileTreeCtrl` with file tree data for sibling discovery
- Phase 12 complete (editor groups -- breadcrumbs scoped per group)

## Deliverables

A fully interactive, custom-painted breadcrumb bar with 25+ tasks covering segment rendering, dropdown pickers, symbol navigation, keyboard nav, and path operations.

---

## Task 1: Custom-Painted Breadcrumb Segments

**Title:** Replace wxStaticText with custom wxGraphicsContext segment rendering

**Description:** Rewrite the breadcrumb bar from a single `wxStaticText` label to a fully custom-painted panel using `wxGraphicsContext`. Each path segment is a discrete rendered element with individual hit-testing, hover states, and click handling.

**Implementation Details:**
- Change `BreadcrumbBar` to extend `ThemeAwareWindow` instead of `wxPanel`
- Remove `wxStaticText* label_` member
- Add `struct BreadcrumbSegment { std::string text; wxRect bounds; bool is_hovered; bool is_active; SegmentType type; }`
- Add `enum class SegmentType { Workspace, Folder, File, Symbol }`
- Add `std::vector<BreadcrumbSegment> segments_`
- Implement `OnPaint()` to draw each segment with:
  - Text in theme font (10pt, semibold for last segment)
  - Separator chevron between segments
  - Hover highlight (lighter background)
  - Active/clicked state (accent underline)
- Set background style to `wxBG_STYLE_PAINT` and min height to 24px

**Files Affected:**
- `src/ui/BreadcrumbBar.h` (complete rewrite of members)
- `src/ui/BreadcrumbBar.cpp` (replace label with custom paint)

**Acceptance Criteria:**
- Segments render individually with proper spacing
- Each segment has independent hover state
- Separator chevrons drawn between segments
- Last segment (filename) rendered in semibold
- Theme colors applied correctly

**Dependencies:** None

---

## Task 2: Full File Path Breadcrumbs (Workspace to File)

**Title:** Display complete path from workspace root to current file

**Description:** Show the full hierarchy from workspace root through all intermediate folders to the current file. When the path is longer than 3 intermediate folders, collapse middle segments into "..." that can be expanded on click.

**Implementation Details:**
- `void SetFilePath(const std::string& full_path, const std::string& workspace_root)`:
  1. Compute relative path from workspace root
  2. Split into segments: `[workspace_name, folder1, folder2, ..., filename]`
  3. If more than 5 segments, collapse middle: `[workspace, folder1, ..., folderN-1, filename]`
  4. Store collapsed state and full segments list
- Collapsed "..." segment: on click, expand to show all intermediate folders
- Workspace root segment shows the project name (last component of workspace path)

**Files Affected:**
- `src/ui/BreadcrumbBar.h` (update `SetFilePath` signature, add collapse state)
- `src/ui/BreadcrumbBar.cpp` (implement path splitting and collapse logic)

**Acceptance Criteria:**
- Full path displayed from workspace root
- Paths with >5 segments collapse middle sections
- "..." segment is clickable to expand
- Workspace name shown as first segment
- Path updates when active file changes

**Dependencies:** Task 1

---

## Task 3: Segment Icons by Type

**Title:** Add type-appropriate icons to each breadcrumb segment

**Description:** Each segment gets a small icon preceding its text: workspace icon for the root, folder icon for directories, and file-type icon for the filename (reusing `FileTypeIconRegistry` from Phase 11).

**Implementation Details:**
- In `OnPaint()`, before drawing segment text, draw the icon:
  ```cpp
  constexpr int kSegmentIconSize = 12;
  switch (segment.type) {
      case SegmentType::Workspace:
          DrawWorkspaceIcon(gc, x, y, kSegmentIconSize);
          break;
      case SegmentType::Folder:
          DrawFolderIcon(gc, x, y, kSegmentIconSize);
          break;
      case SegmentType::File:
          icon_registry_->DrawFileIcon(gc, filename, x, y, kSegmentIconSize);
          break;
      case SegmentType::Symbol:
          DrawSymbolIcon(gc, x, y, kSegmentIconSize, symbol_kind);
          break;
  }
  ```
- Add `FileTypeIconRegistry*` dependency
- Icon rendering: simple geometric shapes (folder = folder glyph, workspace = cube/box)

**Files Affected:**
- `src/ui/BreadcrumbBar.h` (add icon registry dependency)
- `src/ui/BreadcrumbBar.cpp` (add icon drawing in paint)

**Acceptance Criteria:**
- Each segment type has a distinct icon
- Icons properly sized at 12x12
- File segments use extension-specific icons
- Icons vertically centered with text

**Dependencies:** Task 1, Phase 11 Task 1

---

## Task 4: Symbol Breadcrumbs (Class/Method Hierarchy)

**Title:** Add symbol hierarchy segments after the file path

**Description:** After the file path breadcrumbs, display symbol hierarchy based on the cursor position. For Markdown files this means heading hierarchy (H1 > H2 > H3). For code files (C++, Python, etc.) this means namespace > class > function. The symbol breadcrumbs update as the cursor moves.

**Implementation Details:**
- After file path segments, add a visual separator (vertical bar or thicker chevron)
- Append symbol segments from `EditorPanel::GetHeadingSymbols()`:
  ```cpp
  auto symbols = editor->GetHeadingSymbols();
  // Find the heading hierarchy at the current cursor line
  for (const auto& sym : GetActiveSymbolChain(symbols, cursor_line)) {
      segments_.push_back({sym.text, {}, false, false, SegmentType::Symbol});
  }
  ```
- `GetActiveSymbolChain()`: walks the heading list to find the chain of headings containing the cursor line
- Symbol segments use different icon (section sign for headings, braces for classes)
- Subscribe to cursor position change events to update symbols

**Files Affected:**
- `src/ui/BreadcrumbBar.h` (add symbol segment support, cursor tracking)
- `src/ui/BreadcrumbBar.cpp` (implement symbol chain extraction and rendering)

**Acceptance Criteria:**
- Symbol breadcrumbs appear after file path
- Heading hierarchy updates as cursor moves through document
- Visual separator distinguishes file path from symbol path
- Symbol segments have distinct icons
- Works for Markdown heading hierarchy

**Dependencies:** Tasks 1, 2

---

## Task 5: Dropdown Picker for Folder Segments

**Title:** Click a folder segment to show dropdown of siblings

**Description:** Clicking on a folder segment in the breadcrumb shows a dropdown popup listing all sibling folders at that level. Selecting a sibling navigates to that folder (opening the file tree to that location). The dropdown also shows files at that level.

**Implementation Details:**
- On segment click, determine the folder path up to that segment
- Query the file tree for children of the parent folder
- Create a `wxMenu` or custom popup showing:
  - Folders first (with folder icons, sorted alphabetically)
  - Files second (with file-type icons, sorted alphabetically)
  - Current item checked/highlighted
- Selecting a folder: navigate file tree to that folder and expand it
- Selecting a file: open it in the editor

**Files Affected:**
- `src/ui/BreadcrumbBar.h` (add dropdown method)
- `src/ui/BreadcrumbBar.cpp` (implement dropdown popup)
- `src/ui/LayoutManager.cpp` (provide file tree query callback)

**Acceptance Criteria:**
- Clicking any folder segment shows a dropdown
- Dropdown lists siblings at that level
- Folders and files separated and sorted
- Current item highlighted
- Selecting a file opens it
- Selecting a folder navigates the file tree

**Dependencies:** Task 1

---

## Task 6: Dropdown Picker for Symbol Segments

**Title:** Click a symbol segment to show sibling symbols

**Description:** Clicking on a symbol segment shows all symbols at the same level. For Markdown, clicking an H2 shows all H2 headings under the parent H1. Selecting one navigates the editor to that heading.

**Implementation Details:**
- On symbol segment click, determine the parent symbol and level
- Query heading symbols for all siblings at that level under the same parent
- Show popup with:
  - Each sibling heading/symbol with its icon
  - Current heading highlighted
  - Line numbers as secondary text
- Selecting a symbol calls `editor->GoToHeading(line)`

**Files Affected:**
- `src/ui/BreadcrumbBar.cpp` (implement symbol dropdown)

**Acceptance Criteria:**
- Clicking symbol segment shows sibling symbols
- Siblings filtered to same level under same parent
- Selecting navigates to that symbol's line
- Current symbol highlighted in dropdown

**Dependencies:** Task 4

---

## Task 7: Keyboard Navigation Through Segments

**Title:** Enable Left/Right arrow keys to navigate between segments

**Description:** When the breadcrumb bar is focused, Left/Right arrows move focus between segments. Enter opens the dropdown for the focused segment. Escape defocuses the breadcrumb bar. Home/End jump to first/last segment.

**Implementation Details:**
- Add `int focused_segment_index_{-1}` member
- Handle `wxEVT_KEY_DOWN` in breadcrumb bar:
  - Left: `focused_segment_index_ = max(0, focused - 1)`
  - Right: `focused_segment_index_ = min(count - 1, focused + 1)`
  - Enter: open dropdown for focused segment
  - Escape: `focused_segment_index_ = -1`, return focus to editor
  - Home: focus first segment
  - End: focus last segment
- Draw focus ring (2px dashed accent border) around focused segment
- Breadcrumb bar focusable via Cmd+Shift+. shortcut

**Files Affected:**
- `src/ui/BreadcrumbBar.h` (add focus state, key handler)
- `src/ui/BreadcrumbBar.cpp` (implement keyboard navigation, focus ring drawing)

**Acceptance Criteria:**
- Arrow keys navigate between segments
- Focused segment has visible focus ring
- Enter opens dropdown for focused segment
- Escape returns focus to editor
- Home/End shortcuts work
- Breadcrumb bar accessible via keyboard shortcut

**Dependencies:** Task 1

---

## Task 8: Breadcrumb Scrolling for Long Paths

**Title:** Implement horizontal scrolling when breadcrumbs overflow

**Description:** When the total breadcrumb width exceeds the bar width, implement horizontal scrolling. Show a fade gradient at the edges and scroll to ensure the active (last) segment is visible.

**Implementation Details:**
- Add `int breadcrumb_scroll_offset_{0}` member
- In `RecalculateSegmentBounds()`, compute total width
- If total width > client width, enable scrolling:
  - Auto-scroll to make the last segment (filename) visible
  - Draw fade gradient at left edge when scrolled right
  - Mouse wheel scrolls horizontally
  - Left/Right keyboard navigation auto-scrolls to keep focused segment visible
- Scroll animation: smooth 100ms transition using a timer

**Files Affected:**
- `src/ui/BreadcrumbBar.h` (add scroll state)
- `src/ui/BreadcrumbBar.cpp` (implement scrolling, fade gradients)

**Acceptance Criteria:**
- Long paths scroll horizontally
- Active/last segment always visible
- Fade gradient at overflow edges
- Mouse wheel scrolls breadcrumbs
- Keyboard navigation auto-scrolls
- Smooth scroll animation

**Dependencies:** Task 1

---

## Task 9: Quick Pick on Click

**Title:** Implement quick-pick file navigation on workspace segment click

**Description:** Clicking the workspace (first) breadcrumb segment opens a quick-pick panel similar to CommandPalette, listing recently opened files and frequently edited files for fast navigation.

**Implementation Details:**
- On workspace segment click, show a `wxPopupTransientWindow`:
  - Search input at top
  - List of recent files (last 20)
  - As user types, filter list by filename
  - Each entry shows file-type icon + relative path
  - Enter opens the selected file
  - Escape closes the picker
- Reuse CommandPalette's fuzzy matching logic for filtering

**Files Affected:**
- `src/ui/BreadcrumbBar.h` (add quick-pick method)
- `src/ui/BreadcrumbBar.cpp` (implement quick-pick popup)
- `src/ui/BreadcrumbQuickPick.h` (new -- popup widget)
- `src/ui/BreadcrumbQuickPick.cpp` (new)

**Acceptance Criteria:**
- Workspace segment click shows quick-pick popup
- Recent files listed with icons
- Typing filters the list
- Enter opens selected file
- Escape closes without action
- Fuzzy matching works for partial filenames

**Dependencies:** Task 5

---

## Task 10: Copy Path from Breadcrumb

**Title:** Right-click breadcrumb to copy path

**Description:** Right-clicking any segment in the breadcrumb shows a context menu with path operations: "Copy Full Path", "Copy Relative Path", "Copy Path to This Folder", "Copy Filename".

**Implementation Details:**
- Bind `wxEVT_RIGHT_DOWN` to `OnRightClick()`
- Hit-test to determine which segment was right-clicked
- Show context menu:
  ```cpp
  wxMenu menu;
  menu.Append(1, "Copy Full Path");
  menu.Append(2, "Copy Relative Path");
  menu.Append(3, "Copy Path to This Folder");
  menu.Append(4, "Copy Filename");
  menu.AppendSeparator();
  menu.Append(5, "Reveal in Finder");
  menu.Append(6, "Open in Terminal");
  ```
- "Copy Path to This Folder": copies the path up to and including the clicked segment
- "Open in Terminal": opens a terminal at the clicked folder's location

**Files Affected:**
- `src/ui/BreadcrumbBar.h` (add context menu handler)
- `src/ui/BreadcrumbBar.cpp` (implement context menu)

**Acceptance Criteria:**
- Right-click shows context menu
- All copy operations put correct path on clipboard
- "Copy Path to This Folder" works for folder segments
- "Reveal in Finder" opens the correct location
- "Open in Terminal" opens terminal at the folder

**Dependencies:** Task 1

---

## Task 11: Reveal in Explorer from Breadcrumb

**Title:** Navigate file tree to match breadcrumb segment

**Description:** Add "Reveal in Explorer" to the breadcrumb context menu that expands the file tree sidebar to show and highlight the clicked segment's location.

**Implementation Details:**
- Construct the path up to the clicked segment
- Publish a `RevealInExplorerEvent(path)` on the EventBus
- FileTreeCtrl subscribes and calls `ExpandAncestors()` + `SetActiveFileId()` to reveal the node
- Ensure sidebar is visible when revealing

**Files Affected:**
- `src/ui/BreadcrumbBar.cpp` (publish reveal event)
- `src/core/Events.h` (add `RevealInExplorerEvent` if not present)
- `src/ui/FileTreeCtrl.cpp` (subscribe and handle reveal)

**Acceptance Criteria:**
- "Reveal in Explorer" opens sidebar if closed
- File tree expands to show the target path
- Target node is highlighted/selected
- Works for both files and folders

**Dependencies:** Task 10

---

## Task 12: Separator Chevron Styling

**Title:** Render themed chevron separators between segments

**Description:** Replace the current Unicode ">" character separators with custom-drawn chevron arrows using `wxGraphicsContext`. Chevrons should be small (8x8), use `TextMuted` color, and animate slightly on hover (rotate/darken).

**Implementation Details:**
- In `OnPaint()`, between each segment:
  ```cpp
  constexpr int kChevronSize = 8;
  auto muted = theme_engine_.color(ThemeColorToken::TextMuted);
  gc.SetPen(wxPen(muted, 1.5));
  // Draw ">" chevron
  int cx = separator_x + kChevronSize / 2;
  int cy = bar_height / 2;
  gc.StrokeLine(cx - 3, cy - 3, cx + 1, cy);
  gc.StrokeLine(cx + 1, cy, cx - 3, cy + 3);
  ```
- Chevrons between file/folder segments use standard ">"
- Chevron between file and symbol segments uses "::" or a vertical bar

**Files Affected:**
- `src/ui/BreadcrumbBar.cpp` (custom chevron drawing)

**Acceptance Criteria:**
- Chevrons are custom-drawn, not Unicode text
- Correct size (8x8) and color (TextMuted)
- File-to-symbol separator is visually distinct
- Chevrons anti-aliased via wxGraphicsContext

**Dependencies:** Task 1

---

## Task 13: Hover Tooltip on Segments

**Title:** Show full path tooltip on segment hover

**Description:** When hovering over a breadcrumb segment, show a tooltip with the full absolute path up to that segment.

**Implementation Details:**
- In `OnMouseMove()`, hit-test segments and set tooltip:
  ```cpp
  for (auto& seg : segments_) {
      if (seg.bounds.Contains(pos)) {
          SetToolTip(GetFullPathToSegment(seg));
          break;
      }
  }
  ```
- For folder segments: show the full directory path
- For symbol segments: show "filename:lineN - symbol name"

**Files Affected:**
- `src/ui/BreadcrumbBar.cpp` (add tooltip in mouse move)

**Acceptance Criteria:**
- Each segment shows appropriate tooltip on hover
- Folder segments show full directory path
- Symbol segments show file:line and symbol name
- Tooltip updates as mouse moves between segments

**Dependencies:** Task 1

---

## Task 14: Breadcrumb Bar Height Adjustment

**Title:** Support compact and expanded breadcrumb modes

**Description:** Allow the breadcrumb bar to switch between compact (24px, current) and expanded (32px) modes. Expanded mode shows more spacing and allows for a second line with additional info (file size, last modified date).

**Implementation Details:**
- Add `enum class BreadcrumbMode { Compact, Expanded }` and `BreadcrumbMode mode_{BreadcrumbMode::Compact}`
- Compact: 24px height, single line, tight spacing
- Expanded: 32px height, more padding, optional second line with file metadata
- Toggle via command palette or settings
- Update `SetMinSize()` and `SetMaxSize()` when mode changes

**Files Affected:**
- `src/ui/BreadcrumbBar.h` (add mode enum, setter)
- `src/ui/BreadcrumbBar.cpp` (adjust layout per mode)

**Acceptance Criteria:**
- Compact mode: 24px height, tight spacing
- Expanded mode: 32px height, more padding
- Toggle preserves all breadcrumb content
- Mode persists in settings

**Dependencies:** Task 1

---

## Task 15: Breadcrumb Event Integration

**Title:** Wire breadcrumb interactions to the application via EventBus

**Description:** Ensure all breadcrumb interactions (segment click, dropdown selection, keyboard navigation) publish appropriate events so the rest of the application can respond.

**Implementation Details:**
- Define events in `Events.h`:
  ```cpp
  MARKAMP_DECLARE_EVENT(BreadcrumbSegmentClickedEvent, std::string path, SegmentType type);
  MARKAMP_DECLARE_EVENT(BreadcrumbNavigateEvent, std::string target_path);
  MARKAMP_DECLARE_EVENT(BreadcrumbSymbolNavigateEvent, int target_line);
  ```
- Publish `BreadcrumbSegmentClickedEvent` on any segment click
- Publish `BreadcrumbNavigateEvent` when a file/folder is selected from dropdown
- Publish `BreadcrumbSymbolNavigateEvent` when a symbol is selected
- Subscribe in `LayoutManager` / `EditorGroupManager` to handle navigation

**Files Affected:**
- `src/core/Events.h` (add breadcrumb events)
- `src/ui/BreadcrumbBar.cpp` (publish events on interactions)
- `src/ui/LayoutManager.cpp` (subscribe to breadcrumb events)

**Acceptance Criteria:**
- All breadcrumb interactions publish events
- File navigation events correctly open files
- Symbol navigation events scroll editor to target line
- Events include enough context for handlers

**Dependencies:** Tasks 5, 6

---

## Task 16: Active Segment Highlight

**Title:** Visually highlight the currently relevant segments

**Description:** The file segment matching the active file and the symbol segment matching the current cursor position should be highlighted (bolder text, accent color, subtle underline).

**Implementation Details:**
- In `OnPaint()`, the last file segment and the deepest matching symbol segment are drawn with:
  - `TextMain` color instead of `TextMuted`
  - Semibold weight
  - 1px accent-colored underline
- Other segments: `TextMuted` color, regular weight
- Hovered segments: lighter background rectangle

**Files Affected:**
- `src/ui/BreadcrumbBar.cpp` (differentiate active segment rendering)

**Acceptance Criteria:**
- Active segments visually distinct from inactive ones
- Accent underline on active segments
- Hover state visible on all segments
- Clear visual hierarchy from left (dimmer) to right (brighter)

**Dependencies:** Task 1

---

## Task 17: Breadcrumb Accessibility

**Title:** Add accessibility support to breadcrumb bar

**Description:** Ensure the breadcrumb bar is accessible: proper WAI-ARIA-equivalent roles, keyboard focusable, screen reader text for each segment.

**Implementation Details:**
- Breadcrumb bar has role "navigation" with aria-label "Breadcrumb"
- Each segment has role "link" with accessible name "{segment_name}"
- Tab key enters breadcrumb bar focus, Left/Right navigates (from Task 7)
- Screen reader text: "Breadcrumb: workspace > folder > file > heading"
- Focus order: breadcrumb bar comes after toolbar, before editor

**Files Affected:**
- `src/ui/BreadcrumbBar.h` (add accessibility interface)
- `src/ui/BreadcrumbBar.cpp` (implement accessible names and roles)

**Acceptance Criteria:**
- Breadcrumb bar focusable via keyboard
- Screen reader announces segment names
- Navigation role set correctly
- Focus order logical within the window

**Dependencies:** Task 7

---

## Task 18: Go to File from Breadcrumb

**Title:** Cmd+P style file picker triggered from breadcrumb

**Description:** Pressing Cmd+Shift+. (or configurable shortcut) focuses the breadcrumb bar and opens the file quick-pick. This provides an alternative to the command palette for file navigation.

**Implementation Details:**
- Register keyboard shortcut Cmd+Shift+. in MainFrame
- Handler focuses the breadcrumb bar and opens the workspace segment's quick-pick (Task 9)
- If breadcrumb is already focused, the shortcut opens the dropdown for the focused segment

**Files Affected:**
- `src/ui/MainFrame.cpp` (register shortcut)
- `src/ui/BreadcrumbBar.cpp` (handle focus and quick-pick activation)

**Acceptance Criteria:**
- Shortcut focuses breadcrumb and opens file picker
- File picker allows navigation to any file in workspace
- Second press of shortcut (when already focused) opens segment dropdown

**Dependencies:** Task 9

---

## Task 19: Traversal Segment Rendering

**Title:** Render existing cross-surface traversal segments

**Description:** The `BreadcrumbBar` already has a `TraversalSegment` struct and `SetTraversalSegments()` method (from V8 Phase 12). Wire these into the new segment-based rendering so that cross-surface navigation breadcrumbs display properly.

**Implementation Details:**
- In `Rebuild()`, after file and symbol segments, add traversal segments:
  ```cpp
  for (const auto& ts : traversal_segments_) {
      segments_.push_back({
          ts.surface_label + ": " + ts.anchor_label,
          {}, false, false, SegmentType::Traversal
      });
  }
  ```
- Traversal segments use a distinct icon (link/arrow icon)
- Clicking a traversal segment publishes a `TraversalNavigateEvent` with the `nav_entry_index`

**Files Affected:**
- `src/ui/BreadcrumbBar.h` (add `SegmentType::Traversal`)
- `src/ui/BreadcrumbBar.cpp` (render traversal segments)

**Acceptance Criteria:**
- Traversal segments render after symbol segments
- Distinct icon for traversal type
- Clicking navigates via existing cross-surface navigation system
- Proper separator between symbol and traversal regions

**Dependencies:** Tasks 1, 4

---

## Task 20: Empty State Breadcrumb

**Title:** Show helpful empty state when no file is open

**Description:** When no file is open, the breadcrumb bar should show a helpful message like "Open a file to start" with a clickable action to open the file picker.

**Implementation Details:**
- When `file_segments_` is empty, render a single centered segment:
  - Text: "Open a file to start editing"
  - Color: `TextMuted`
  - Italic style
  - Click handler opens the command palette in file-open mode

**Files Affected:**
- `src/ui/BreadcrumbBar.cpp` (add empty state rendering)

**Acceptance Criteria:**
- Empty state message shown when no file is open
- Message is clickable and opens file picker
- Message styled differently from normal segments (italic, muted)

**Dependencies:** Task 1

---

## Task 21: Breadcrumb Drag Support

**Title:** Support dragging breadcrumb segments to other locations

**Description:** Allow dragging a file breadcrumb segment to the file tree (to reveal) or to another editor group's tab bar (to open the file there). This provides a drag-based navigation metaphor.

**Implementation Details:**
- On mouse down + drag on a file/folder segment:
  - Start a `wxDropSource` with `wxFileDataObject` containing the path
  - Set drag cursor to file icon
  - Drop targets: file tree (reveals), tab bars (opens file), desktop (copies path)
- For symbol segments: dragging copies the line reference ("file.md:42") as text

**Files Affected:**
- `src/ui/BreadcrumbBar.h` (add drag state tracking)
- `src/ui/BreadcrumbBar.cpp` (implement drag source)

**Acceptance Criteria:**
- File segments draggable with file icon cursor
- Drop on tab bar opens file in that group
- Drop on file tree reveals in explorer
- Symbol segment drag copies reference text
- Drag starts only after 5px threshold to avoid accidental drags

**Dependencies:** Task 1

---

## Task 22: Breadcrumb Configuration

**Title:** Add settings for breadcrumb visibility and behavior

**Description:** Add configuration options for the breadcrumb bar: show/hide, show file path only vs file+symbols, segment click behavior (dropdown vs navigate), and compact/expanded mode.

**Implementation Details:**
- Settings in config:
  ```yaml
  breadcrumb:
    visible: true
    show_symbols: true
    click_action: "dropdown"  # or "navigate"
    mode: "compact"  # or "expanded"
  ```
- Add `void LoadSettings(core::Config& config)` and `void SaveSettings(core::Config& config)`
- Toggle visibility via command palette: "View: Toggle Breadcrumbs"
- When hidden, breadcrumb bar collapses to 0 height

**Files Affected:**
- `src/ui/BreadcrumbBar.h` (add settings methods)
- `src/ui/BreadcrumbBar.cpp` (implement settings load/save)
- `src/core/Config.h` (add breadcrumb settings)

**Acceptance Criteria:**
- All settings load from and save to config
- Toggle visibility works from command palette
- Click action configurable between dropdown and direct navigation
- Settings persist across restarts

**Dependencies:** Tasks 1, 14

---

## Task 23: Symbol Update Debouncing

**Title:** Debounce symbol breadcrumb updates during typing

**Description:** When the user is actively typing, symbol breadcrumbs should not update on every keystroke. Debounce updates to 300ms after the last cursor movement to avoid visual noise.

**Implementation Details:**
- Add `wxTimer symbol_update_timer_` with 300ms interval
- On cursor position change event, restart the timer instead of immediately updating:
  ```cpp
  symbol_update_timer_.Stop();
  pending_cursor_line_ = cursor_line;
  symbol_update_timer_.Start(300, wxTIMER_ONE_SHOT);
  ```
- On timer fire, recalculate the symbol chain and update segments
- If the file path changes (not just cursor), update immediately (no debounce)

**Files Affected:**
- `src/ui/BreadcrumbBar.h` (add debounce timer, pending state)
- `src/ui/BreadcrumbBar.cpp` (implement debounced symbol updates)

**Acceptance Criteria:**
- Symbol breadcrumbs update 300ms after cursor stops moving
- No flicker during active typing
- File path changes update immediately
- Timer properly cleaned up on destruction

**Dependencies:** Task 4

---

## Task 24: Breadcrumb Bar Theme Integration

**Title:** Full theme support for breadcrumb rendering

**Description:** Ensure all breadcrumb visual elements respond to theme changes: segment text color, icon tints, separator color, hover background, focus ring, and background.

**Implementation Details:**
- Override `OnThemeChanged()`:
  ```cpp
  void BreadcrumbBar::OnThemeChanged(const core::Theme& new_theme) {
      ThemeAwareWindow::OnThemeChanged(new_theme);
      RecalculateSegmentBounds();
      Refresh();
  }
  ```
- Color mapping:
  - Background: `BgPanel`
  - Active segment text: `TextMain`
  - Inactive segment text: `TextMuted`
  - Hover background: `BgPanel.ChangeLightness(110)`
  - Focus ring: `AccentPrimary`
  - Separator: `TextMuted` at 60% opacity
  - Bottom border: `BorderLight`

**Files Affected:**
- `src/ui/BreadcrumbBar.h` (ensure `ThemeAwareWindow` base)
- `src/ui/BreadcrumbBar.cpp` (theme-aware rendering throughout)

**Acceptance Criteria:**
- All colors update on theme change
- No hardcoded colors in rendering code
- Visual consistency with TabBar, Toolbar, and StatusBar
- Dark and light themes both render correctly

**Dependencies:** Task 1

---

## Task 25: Breadcrumb Unit Tests

**Title:** Comprehensive test coverage for breadcrumb logic

**Description:** Write Catch2 unit tests covering breadcrumb segment generation, path splitting, symbol chain extraction, collapse logic, and hit-testing.

**Implementation Details:**
- Create `tests/unit/test_breadcrumb_bar.cpp`:
  - Test path splitting: verify segment count and content for various paths
  - Test collapse: verify "..." insertion for paths >5 segments
  - Test symbol chain: verify heading hierarchy extraction
  - Test hit-testing: verify correct segment identified for given coordinates
  - Test scroll: verify scroll offset calculation for long paths
  - Test empty state: verify empty state when no file set

**Files Affected:**
- `tests/unit/test_breadcrumb_bar.cpp` (new)
- `CMakeLists.txt` (add test target)

**Acceptance Criteria:**
- Path splitting covers: simple paths, deep paths, root-level files
- Collapse tested for 3, 5, 8, 12 segment paths
- Symbol chain tested for Markdown heading hierarchies
- All tests pass in CI

**Dependencies:** Tasks 1-4

---

## Estimated Complexity

| Area | Effort |
|------|--------|
| Custom Rendering (Task 1) | High |
| Full Path Display (Task 2) | Medium |
| Segment Icons (Task 3) | Medium |
| Symbol Breadcrumbs (Task 4) | High |
| Folder Dropdown (Task 5) | High |
| Symbol Dropdown (Task 6) | Medium |
| Keyboard Navigation (Task 7) | Medium |
| Scrolling (Task 8) | Medium |
| Quick Pick (Task 9) | High |
| Context Menu (Tasks 10-11) | Low |
| Chevron Styling (Task 12) | Low |
| Tooltips (Task 13) | Low |
| Height Modes (Task 14) | Low |
| Events (Task 15) | Medium |
| Active Highlight (Task 16) | Low |
| Accessibility (Task 17) | Medium |
| Go to File (Task 18) | Low |
| Traversal Segments (Task 19) | Medium |
| Empty State (Task 20) | Low |
| Drag Support (Task 21) | Medium |
| Configuration (Task 22) | Low |
| Debouncing (Task 23) | Low |
| Theme (Task 24) | Low |
| Tests (Task 25) | Medium |

## Files Created

- `src/ui/BreadcrumbQuickPick.h`
- `src/ui/BreadcrumbQuickPick.cpp`
- `tests/unit/test_breadcrumb_bar.cpp`

## Files Modified

- `src/ui/BreadcrumbBar.h` (major rewrite)
- `src/ui/BreadcrumbBar.cpp` (major rewrite)
- `src/core/Events.h`
- `src/ui/LayoutManager.cpp`
- `src/ui/MainFrame.cpp`
- `src/core/Config.h` / `src/core/Config.cpp`
- `CMakeLists.txt`
