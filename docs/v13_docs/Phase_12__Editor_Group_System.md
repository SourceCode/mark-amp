# Phase 12: Editor Group System

## Objective

Implement a multi-pane editor group system that allows splitting the editor area into multiple groups, each with its own tab bar, breadcrumb bar, and editor instance. This is the foundational architectural change required for a professional IDE layout -- enabling side-by-side file comparison, reference viewing while editing, and workspace arrangements matching VSCode's split editor behavior. The current `SplitView` handles editor/preview splits; this phase adds editor/editor splits in arbitrary grid configurations.

## Prerequisites

- Phase 11 (Tab Bar Overhaul) completed -- especially cross-group drag events (Task 10) and split context menu items (Task 8)
- Existing `SplitView` at `src/ui/SplitView.h` / `.cpp` providing editor/preview split
- Existing `LayoutManager` orchestrating sidebar | content area | status bar layout
- `EditorPanel` at `src/ui/EditorPanel.h` with full Scintilla editor functionality
- `TabBar` at `src/ui/TabBar.h` with all Phase 11 enhancements
- `BreadcrumbBar` at `src/ui/BreadcrumbBar.h`
- `EventBus` for inter-component communication

## Deliverables

A complete `EditorGroupManager` system with 25+ tasks covering split creation, grid layouts, group focus management, tab-to-group drag, keyboard navigation, and per-group state.

---

## Task 1: EditorGroup Data Model

**Title:** Define the EditorGroup struct and tree structure

**Description:** Create the core data model representing an editor group. Each group contains a tab bar, an editor panel, and a breadcrumb bar. Groups are arranged in a binary tree where internal nodes represent splits (horizontal or vertical) and leaves represent actual editor groups.

**Implementation Details:**
- Create `src/ui/EditorGroupManager.h`:
  ```cpp
  enum class SplitOrientation { Horizontal, Vertical };

  struct EditorGroupNode {
      int id{0};
      SplitOrientation split{SplitOrientation::Horizontal};
      double split_ratio{0.5};
      std::unique_ptr<EditorGroupNode> first_child;
      std::unique_ptr<EditorGroupNode> second_child;
      // Leaf data (null for internal nodes)
      std::unique_ptr<EditorGroupLeaf> leaf;
  };

  struct EditorGroupLeaf {
      int group_id{0};
      TabBar* tab_bar{nullptr};
      BreadcrumbBar* breadcrumb{nullptr};
      EditorPanel* editor{nullptr};
      wxPanel* container{nullptr};
      std::vector<std::string> open_files;
      std::string active_file;
      bool is_focused{false};
  };
  ```
- Internal nodes have children but no leaf; leaves have no children
- Use incrementing group IDs starting from 1

**Files Affected:**
- `src/ui/EditorGroupManager.h` (new)

**Acceptance Criteria:**
- Data model supports binary tree of splits
- Leaf nodes hold all necessary UI component pointers
- Can represent 1x1, 1x2, 2x1, 2x2, 1+2 layouts
- Group IDs are unique and incrementing

**Dependencies:** None

---

## Task 2: EditorGroupManager Class Shell

**Title:** Create the EditorGroupManager class with core API

**Description:** Implement the `EditorGroupManager` class that owns the group tree and provides the public API for split operations, group focus, and file routing.

**Implementation Details:**
- Create `src/ui/EditorGroupManager.cpp`:
  ```cpp
  class EditorGroupManager : public ThemeAwareWindow {
  public:
      EditorGroupManager(wxWindow* parent, core::ThemeEngine& theme_engine,
                         core::EventBus& event_bus, core::Config* config,
                         core::IMermaidRenderer* mermaid, core::IMathRenderer* math);

      // Split operations
      void SplitActiveGroupRight();
      void SplitActiveGroupDown();
      void SplitGroup(int group_id, SplitOrientation orientation);
      void CloseGroup(int group_id);

      // Focus
      void SetFocusedGroup(int group_id);
      [[nodiscard]] auto GetFocusedGroupId() const -> int;
      [[nodiscard]] auto GetGroupCount() const -> int;

      // File routing
      void OpenFileInGroup(int group_id, const std::string& path);
      void OpenFileInFocusedGroup(const std::string& path);

      // Navigation
      void FocusNextGroup();
      void FocusPreviousGroup();
      void FocusGroupByIndex(int index);

      // Queries
      [[nodiscard]] auto GetEditorForGroup(int group_id) -> EditorPanel*;
      [[nodiscard]] auto GetFocusedEditor() -> EditorPanel*;
      [[nodiscard]] auto GetAllGroupIds() const -> std::vector<int>;
  };
  ```
- Start with single root group (equivalent to current single-editor behavior)
- Manage `EditorGroupNode` root tree

**Files Affected:**
- `src/ui/EditorGroupManager.h` (add class definition)
- `src/ui/EditorGroupManager.cpp` (new)
- `CMakeLists.txt` (add to build)

**Acceptance Criteria:**
- Manager initializes with one default group
- All public methods have implementations (even if some are stubs for later tasks)
- GetGroupCount returns 1 initially
- GetFocusedGroupId returns the default group ID

**Dependencies:** Task 1

---

## Task 3: Single Group Creation (Leaf Factory)

**Title:** Implement factory method for creating a single editor group

**Description:** Create a method that instantiates all UI components for a single editor group: a container panel, a tab bar, a breadcrumb bar, and an editor panel. Wire them together with proper sizer layout.

**Implementation Details:**
- `auto CreateGroupLeaf(wxWindow* parent, int group_id) -> std::unique_ptr<EditorGroupLeaf>`:
  ```cpp
  auto leaf = std::make_unique<EditorGroupLeaf>();
  leaf->group_id = group_id;
  leaf->container = new wxPanel(parent);
  auto* sizer = new wxBoxSizer(wxVERTICAL);
  leaf->tab_bar = new TabBar(leaf->container, theme_engine_, event_bus_);
  leaf->breadcrumb = new BreadcrumbBar(leaf->container, theme_engine_, event_bus_);
  leaf->editor = new EditorPanel(leaf->container, theme_engine_, event_bus_);
  sizer->Add(leaf->tab_bar, 0, wxEXPAND);
  sizer->Add(leaf->breadcrumb, 0, wxEXPAND);
  sizer->Add(leaf->editor, 1, wxEXPAND);
  leaf->container->SetSizer(sizer);
  ```
- Each group's tab bar operates independently with its own tab list
- Forward `FileTypeIconRegistry` to each tab bar

**Files Affected:**
- `src/ui/EditorGroupManager.h` (add factory method)
- `src/ui/EditorGroupManager.cpp` (implement factory)

**Acceptance Criteria:**
- Factory creates a complete, functional editor group
- Tab bar, breadcrumb, and editor properly laid out vertically
- All components receive correct theme engine and event bus references
- Container panel is sizer-managed for proper resizing

**Dependencies:** Task 2

---

## Task 4: Horizontal Split (Split Right)

**Title:** Implement splitting the active group horizontally (side-by-side)

**Description:** When `SplitActiveGroupRight()` is called, the current group's leaf node is replaced by an internal node with horizontal split, containing the original group as the left child and a new empty group as the right child.

**Implementation Details:**
- In `SplitGroup(group_id, Horizontal)`:
  1. Find the leaf node by group_id in the tree
  2. Create a new internal node
  3. Move the existing leaf to `first_child`
  4. Create a new leaf for `second_child`
  5. Replace the old leaf's position in the tree with the new internal node
  6. If the active file is open, open it in the new group too (or leave new group empty per VSCode behavior)
- Create `wxSplitterWindow` or manual layout to display both leaves side by side
- Default split ratio: 0.5

**Files Affected:**
- `src/ui/EditorGroupManager.cpp` (implement split logic)

**Acceptance Criteria:**
- Splitting creates two side-by-side editor groups
- Original group retains its open files and state
- New group is empty (or opens the same file per config)
- Splitter divider is draggable to resize
- Both groups have independent tab bars

**Dependencies:** Task 3

---

## Task 5: Vertical Split (Split Down)

**Title:** Implement splitting the active group vertically (top/bottom)

**Description:** Identical to horizontal split but with vertical orientation. The original group becomes the top child and a new group is created as the bottom child.

**Implementation Details:**
- Reuse the same `SplitGroup()` method with `SplitOrientation::Vertical`
- Layout uses vertical sizer arrangement
- Divider is horizontal (dragged up/down)

**Files Affected:**
- `src/ui/EditorGroupManager.cpp` (extend split logic for vertical)

**Acceptance Criteria:**
- Vertical split creates top/bottom editor groups
- Divider is horizontal and draggable
- Both groups function independently
- Can combine horizontal and vertical splits for grid layouts

**Dependencies:** Task 4

---

## Task 6: Editor Grid Layouts (2x2, 1+2, etc.)

**Title:** Implement preset grid layout configurations

**Description:** Provide predefined layout presets that split into common arrangements: 2 columns, 2 rows, 2x2 grid, and 1+2 (one left column, two stacked on right).

**Implementation Details:**
- Add `enum class EditorGridLayout { Single, TwoColumns, TwoRows, Grid2x2, LeftPlusTwoRight }`
- Add `void SetGridLayout(EditorGridLayout layout)`:
  - `Single`: close all but focused group
  - `TwoColumns`: ensure root is a horizontal split with two leaves
  - `TwoRows`: ensure root is a vertical split with two leaves
  - `Grid2x2`: root horizontal, each child vertical (4 leaves)
  - `LeftPlusTwoRight`: root horizontal, left is leaf, right is vertical with 2 leaves
- Redistribute existing open files across groups (round-robin)
- Add layout preset selector to toolbar or command palette

**Files Affected:**
- `src/ui/EditorGroupManager.h` (add enum, method)
- `src/ui/EditorGroupManager.cpp` (implement layout logic)
- `src/ui/CommandPalette.cpp` (register layout commands)

**Acceptance Criteria:**
- All 5 layouts achievable via API
- Switching layouts redistributes files appropriately
- Layout presets accessible from command palette
- Grid2x2 creates exactly 4 editor groups
- LeftPlusTwoRight creates 3 groups (1 left, 2 right)

**Dependencies:** Tasks 4, 5

---

## Task 7: Group Focus Indicator

**Title:** Draw visual focus indicator on the active editor group

**Description:** The currently focused group must be visually distinguished. Draw a 2px accent-colored border around the focused group's container and dim non-focused groups slightly.

**Implementation Details:**
- In each group's container paint event, check `is_focused`:
  ```cpp
  if (leaf->is_focused) {
      gc.SetPen(wxPen(theme_engine_.color(ThemeColorToken::AccentPrimary), 2));
      gc.DrawRectangle(0, 0, width, height);
  }
  ```
- Non-focused groups: reduce tab bar opacity to 80% or apply subtle dimming
- Focus changes when user clicks in a group's editor, tab bar, or breadcrumb
- Add focus indicator to the group's tab bar as well (accent underline at bottom)

**Files Affected:**
- `src/ui/EditorGroupManager.cpp` (add focus drawing logic)
- `src/ui/TabBar.h` / `.cpp` (add `SetGroupFocused(bool)` for tab bar dimming)

**Acceptance Criteria:**
- Focused group has visible accent border
- Non-focused groups are subtly dimmed
- Focus changes on click anywhere in a group
- Only one group is focused at a time
- Focus indicator uses theme accent color

**Dependencies:** Task 3

---

## Task 8: Group Header Bar with Actions

**Title:** Add a thin header bar above each group's tab bar with group controls

**Description:** Each editor group gets a 20px header bar showing the group number and action buttons: "Split Right", "Split Down", "Close Group", and a maximize/restore toggle.

**Implementation Details:**
- Create a `GroupHeaderBar` widget (20px height) rendered above each group's tab bar
- Left side: "Group N" label in 9pt font
- Right side: icon buttons for actions
  - Split right: "|" icon
  - Split down: "--" icon
  - Maximize: "[]" icon (toggles to restore "[_]" when maximized)
  - Close: "x" icon (disabled if only one group)
- Wire buttons to `EditorGroupManager` methods
- Omit header bar when only one group exists (clean single-editor look)

**Files Affected:**
- `src/ui/GroupHeaderBar.h` (new)
- `src/ui/GroupHeaderBar.cpp` (new)
- `src/ui/EditorGroupManager.cpp` (integrate header into group layout)
- `CMakeLists.txt` (add to build)

**Acceptance Criteria:**
- Header visible above each group's tab bar when multiple groups exist
- Hidden in single-group mode
- All four action buttons functional
- Close button disabled for last remaining group
- Maximize expands one group to fill entire editor area

**Dependencies:** Tasks 4, 5

---

## Task 9: Drag Tab to Create New Group

**Title:** Handle tab drag-out events to create new editor groups

**Description:** When a tab is dragged out of its tab bar (Phase 11 Task 10 publishes `TabDragOutEvent`), determine the drop zone and create a new editor group at that position. The drop zone is determined by which half/quarter of the editor area the cursor is over.

**Implementation Details:**
- Subscribe to `TabDragOutEvent` in `EditorGroupManager`
- During drag, show visual drop zone indicators:
  - Mouse in left 25% of current group: split left
  - Mouse in right 25%: split right
  - Mouse in top 25%: split above
  - Mouse in bottom 25%: split below
  - Mouse in center 50%: move tab to existing group
- Draw semi-transparent overlay showing the proposed split zone
- On drop:
  1. Remove tab from source group's tab bar
  2. Create new group at the indicated position
  3. Open the file in the new group
- If source group has no more tabs, close it

**Files Affected:**
- `src/ui/EditorGroupManager.cpp` (subscribe to drag events, implement drop zone logic)
- `src/ui/EditorGroupManager.h` (add drag state tracking)

**Acceptance Criteria:**
- Dragging tab out of tab bar shows drop zone overlay
- Dropping in edge zones creates a new split
- Dropping in center of existing group moves tab there
- Source group closes when last tab is dragged out
- Drop zone indicators clearly show proposed layout change

**Dependencies:** Phase 11 Task 10

---

## Task 10: Keyboard Shortcuts for Group Navigation

**Title:** Implement Cmd+1/2/3/... for focusing editor groups

**Description:** Add keyboard shortcuts to quickly switch focus between editor groups. Cmd+1 focuses group 1, Cmd+2 focuses group 2, etc. Also add Cmd+K Cmd+Left/Right/Up/Down for directional focus movement.

**Implementation Details:**
- Add events to `Events.h`:
  ```cpp
  MARKAMP_DECLARE_EVENT(FocusEditorGroupEvent, int group_index);
  MARKAMP_DECLARE_EVENT(FocusEditorGroupDirectionEvent, Direction direction);
  ```
- Subscribe in `EditorGroupManager`:
  - `FocusEditorGroupEvent(N)` -> `FocusGroupByIndex(N)`
  - Directional focus: traverse the tree to find the nearest group in the given direction
- Register shortcuts in `MainFrame` key handler:
  - Cmd+1 through Cmd+9: focus group N
  - Cmd+K, Cmd+Left: focus group to the left
  - Cmd+K, Cmd+Right: focus group to the right

**Files Affected:**
- `src/core/Events.h` (add events)
- `src/ui/EditorGroupManager.cpp` (subscribe and handle)
- `src/ui/MainFrame.cpp` (register keyboard shortcuts)

**Acceptance Criteria:**
- Cmd+1 through Cmd+9 focus the Nth group (if it exists)
- Directional navigation moves focus to the nearest group in that direction
- Focus indicator updates immediately on keyboard navigation
- Shortcuts listed in command palette

**Dependencies:** Task 7

---

## Task 11: Maximize/Restore Single Group

**Title:** Implement group maximize that temporarily hides all other groups

**Description:** Double-clicking a group's header bar or pressing a shortcut maximizes that group to fill the entire editor area. All other groups are hidden but their state is preserved. Pressing the shortcut again or clicking the restore button returns to the previous layout.

**Implementation Details:**
- Add `bool is_maximized_{false}` and `int maximized_group_id_{-1}` to manager
- `void MaximizeGroup(int group_id)`:
  1. Store current tree layout dimensions
  2. Hide all group containers except the target
  3. Resize target to fill entire area
  4. Set `is_maximized_ = true`
- `void RestoreGroups()`:
  1. Show all group containers
  2. Restore saved dimensions
  3. Set `is_maximized_ = false`
- Toggle behavior: if already maximized, restore

**Files Affected:**
- `src/ui/EditorGroupManager.h` (add maximize state)
- `src/ui/EditorGroupManager.cpp` (implement maximize/restore)

**Acceptance Criteria:**
- Maximized group fills entire editor area
- Other groups hidden but state preserved
- Restore returns to exact previous layout
- Toggle behavior works correctly
- Maximize/restore animates smoothly (200ms transition)

**Dependencies:** Task 8

---

## Task 12: Close Group

**Title:** Implement closing an editor group

**Description:** Closing a group removes it from the tree and its parent split node is replaced by the sibling group. If the closed group has modified files, prompt for save.

**Implementation Details:**
- `void CloseGroup(int group_id)`:
  1. Find the leaf node in the tree
  2. Check all open files for modifications (prompt save)
  3. Close all tabs in the group
  4. Find the parent internal node
  5. Replace parent with the sibling child
  6. Destroy the closed group's UI components
  7. If the closed group was focused, focus the sibling
- Cannot close the last remaining group (button disabled)

**Files Affected:**
- `src/ui/EditorGroupManager.cpp` (implement close logic)

**Acceptance Criteria:**
- Closing group removes it from layout
- Sibling group expands to fill the space
- Modified files prompt for save before close
- Last group cannot be closed
- Focus moves to sibling after close

**Dependencies:** Task 8

---

## Task 13: Group-Specific Breadcrumbs

**Title:** Each editor group has its own breadcrumb bar

**Description:** Ensure that each editor group's breadcrumb bar independently shows the path of the file active in that group. Breadcrumb updates are scoped to the group that owns the editor.

**Implementation Details:**
- Each `EditorGroupLeaf` already has its own `BreadcrumbBar*`
- In the file open/switch handler for each group, update that group's breadcrumb:
  ```cpp
  std::vector<std::string> segments = ParsePathSegments(file_path, workspace_root);
  leaf->breadcrumb->SetFilePath(segments);
  ```
- Ensure cursor movement events update the heading path for the correct group
- Subscribe to per-editor cursor events with group ID context

**Files Affected:**
- `src/ui/EditorGroupManager.cpp` (wire breadcrumb updates per group)

**Acceptance Criteria:**
- Each group's breadcrumb shows the active file path for that group
- Breadcrumbs update independently when switching tabs within a group
- Heading breadcrumbs track the editor cursor within each group
- Clicking a breadcrumb segment navigates within the correct group

**Dependencies:** Task 3

---

## Task 14: Group-Level Tab Bar

**Title:** Ensure each group has independent tab management

**Description:** Each editor group operates an independent tab bar with its own set of open files. Tabs can exist in multiple groups simultaneously (showing the same file in two groups). Tab operations (close, pin, sort) are scoped to the group.

**Implementation Details:**
- Each `EditorGroupLeaf::tab_bar` manages its own `tabs_` vector
- When a file is opened in a group, add it to that group's tab bar only
- When a tab is switched in a group, load the file into that group's editor
- Files can be open in multiple groups simultaneously -- each group maintains independent cursor/scroll state
- Tab events include group ID for routing:
  ```cpp
  struct GroupTabSwitchedEvent { int group_id; std::string file_path; };
  ```

**Files Affected:**
- `src/core/Events.h` (add group-scoped tab events)
- `src/ui/EditorGroupManager.cpp` (route tab events per group)

**Acceptance Criteria:**
- Each group has its own independent tab list
- Same file can be open in multiple groups
- Tab operations (close, pin) only affect the group they belong to
- Tab switching loads the correct file in the correct group's editor
- Independent cursor and scroll positions per group

**Dependencies:** Task 3

---

## Task 15: Diff Editor in Split Group

**Title:** Open diff view as a special editor group mode

**Description:** Support opening a diff view between two files in a single editor group. The group switches to a special "diff mode" showing two editors side-by-side with change highlighting. Leverages the existing `DiffPanel` stub.

**Implementation Details:**
- Add `void OpenDiffInGroup(int group_id, const std::string& left_path, const std::string& right_path)`
- In diff mode, the group hides its tab bar and shows a diff header with file names
- Create a proper `DiffEditorPanel` that contains two `wxStyledTextCtrl` instances:
  - Left: original file (read-only)
  - Right: modified file (editable)
  - Synchronized scrolling
  - Change highlighting with colored gutters (green=added, red=deleted, blue=modified)
- Wire to existing `DiffPanel` at `src/ui/DiffPanel.h` and flesh out its implementation
- Add "Compare with..." command to file context menu

**Files Affected:**
- `src/ui/DiffPanel.h` (expand from stub to full implementation)
- `src/ui/DiffPanel.cpp` (implement dual-editor diff view)
- `src/ui/EditorGroupManager.cpp` (add diff mode support)
- `CMakeLists.txt` (ensure DiffPanel in build)

**Acceptance Criteria:**
- Diff view shows two files side-by-side in one group
- Changes highlighted with colored gutters
- Synchronized scrolling between the two sides
- Navigation between changes (next/previous change)
- Diff header shows both file names
- Can switch back to normal editor mode

**Dependencies:** Task 3

---

## Task 16: Group Resize via Divider Drag

**Title:** Implement draggable dividers between editor groups

**Description:** The splitter between editor groups must be draggable. Implement visual divider bars (4px wide) with hover cursor change and drag-to-resize behavior.

**Implementation Details:**
- At each internal node in the group tree, render a divider bar
- Horizontal splits: vertical divider (drag left/right)
- Vertical splits: horizontal divider (drag up/down)
- Divider: 4px visual width, 12px hit area
- Cursor changes to `wxCURSOR_SIZEWE` or `wxCURSOR_SIZENS` on hover
- During drag, update `split_ratio` of the parent node and relayout
- Minimum group size: 150px width, 100px height
- Double-click divider to reset to 50/50

**Files Affected:**
- `src/ui/EditorGroupManager.cpp` (implement divider rendering and drag)

**Acceptance Criteria:**
- Dividers visible between all groups
- Drag resizes groups proportionally
- Minimum size constraints enforced
- Double-click resets to equal split
- Cursor changes on hover

**Dependencies:** Tasks 4, 5

---

## Task 17: Group State Persistence

**Title:** Save and restore editor group layout across sessions

**Description:** Persist the entire group tree structure, split ratios, open files per group, and active file per group to the config file. Restore on application startup.

**Implementation Details:**
- Serialize group tree to YAML:
  ```yaml
  editor_groups:
    type: split
    orientation: horizontal
    ratio: 0.5
    children:
      - type: leaf
        group_id: 1
        files: ["/path/a.md", "/path/b.md"]
        active: "/path/a.md"
      - type: leaf
        group_id: 2
        files: ["/path/c.md"]
        active: "/path/c.md"
  ```
- Deserialize on startup and recreate the group tree
- Handle missing files gracefully (skip with warning)

**Files Affected:**
- `src/ui/EditorGroupManager.h` (add serialize/deserialize methods)
- `src/ui/EditorGroupManager.cpp` (implement persistence)
- `src/core/Config.h` / `.cpp` (add editor group storage)

**Acceptance Criteria:**
- Group layout saved on application close
- Layout restored on startup with correct split ratios
- Open files restored per group
- Missing files skipped with log warning
- Active file per group correctly set

**Dependencies:** Task 6

---

## Task 18: Integrate EditorGroupManager into LayoutManager

**Title:** Replace single SplitView with EditorGroupManager in LayoutManager

**Description:** Modify `LayoutManager` to use `EditorGroupManager` as the primary content area component instead of the current single `SplitView`. The `SplitView` becomes one component within each editor group (handling editor/preview splits within a group).

**Implementation Details:**
- In `LayoutManager::CreateLayout()`, replace `split_view_` creation with `editor_group_manager_`:
  ```cpp
  editor_group_manager_ = new EditorGroupManager(
      content_panel_, theme_engine_, event_bus_, config_,
      mermaid_renderer_, math_renderer_);
  ```
- Route file open events through `EditorGroupManager::OpenFileInFocusedGroup()`
- Route tab events through the group manager
- Keep backward compatibility: single group behaves identically to current behavior
- Update all LayoutManager methods that interact with the editor (e.g., `GetActiveFilePath`, `GetActiveFileContent`)

**Files Affected:**
- `src/ui/LayoutManager.h` (replace SplitView* with EditorGroupManager*)
- `src/ui/LayoutManager.cpp` (rewire all editor interactions)

**Acceptance Criteria:**
- Single-group mode is visually and functionally identical to current behavior
- All existing LayoutManager public methods still work
- File open/close/save operations correctly routed through group manager
- Status bar updates from the focused group's editor
- No regressions in existing functionality

**Dependencies:** Tasks 2, 3

---

## Task 19: Group Focus Follows Mouse

**Title:** Auto-focus group when mouse enters its editor area

**Description:** Optionally (configurable), the focused group automatically changes when the mouse cursor enters a different group's editor area. This provides "focus follows mouse" behavior for users who prefer it.

**Implementation Details:**
- Add `bool focus_follows_mouse_{false}` setting
- In each editor group's container, handle `wxEVT_ENTER_WINDOW`:
  ```cpp
  if (focus_follows_mouse_) {
      SetFocusedGroup(leaf->group_id);
  }
  ```
- Debounce by 100ms to prevent rapid focus switching
- Add toggle to settings panel and command palette

**Files Affected:**
- `src/ui/EditorGroupManager.h` (add setting)
- `src/ui/EditorGroupManager.cpp` (implement mouse enter handler)

**Acceptance Criteria:**
- When enabled, hovering over a group focuses it
- 100ms debounce prevents rapid switching
- When disabled, focus only changes on click
- Setting persists in config
- Discoverable in command palette

**Dependencies:** Task 7

---

## Task 20: Move File Between Groups

**Title:** Add command to move active file to another group

**Description:** Add "Move to Group N" commands that move the active file from the current group to a specified group. The file is removed from the source group's tab bar and opened in the target group.

**Implementation Details:**
- Add `void MoveFileToGroup(const std::string& path, int source_group, int target_group)`
- In source group: save editor state, close tab
- In target group: open file, restore editor state
- Add to command palette: "Move File to Group 1/2/3/..."
- Add keyboard shortcut: Cmd+K, Cmd+Shift+Left/Right to move to adjacent group

**Files Affected:**
- `src/ui/EditorGroupManager.h` (add move method)
- `src/ui/EditorGroupManager.cpp` (implement move logic)
- `src/ui/CommandPalette.cpp` (register commands)

**Acceptance Criteria:**
- File moved to target group with preserved editor state
- Source group closes if it was the last file
- Cursor position and scroll preserved after move
- Commands available in command palette
- Keyboard shortcut works for adjacent group movement

**Dependencies:** Task 14

---

## Task 21: Equal Size Groups

**Title:** Add command to equalize all group sizes

**Description:** Add a command that resets all split ratios to equal sizes, giving each group the same proportion of space.

**Implementation Details:**
- `void EqualizeGroupSizes()`: traverse the tree and set all `split_ratio` values to 0.5
- Add to command palette: "Editor: Equal Group Sizes"
- Animate the transition over 200ms

**Files Affected:**
- `src/ui/EditorGroupManager.cpp` (implement equalization)
- `src/ui/CommandPalette.cpp` (register command)

**Acceptance Criteria:**
- All groups become equal size
- Transition animates smoothly
- Works for any number of groups and split orientations
- Command accessible from palette

**Dependencies:** Task 16

---

## Task 22: Join Group (Merge Adjacent)

**Title:** Merge two adjacent groups into one

**Description:** Provide a "Join Group" command that merges the focused group with an adjacent group, combining their open files into a single tab bar.

**Implementation Details:**
- `void JoinGroupWith(int group_id, Direction direction)`:
  1. Find the adjacent group in the specified direction
  2. Move all open files from the adjacent group to the target
  3. Close the adjacent group (tree pruning)
  4. All files from both groups now in one tab bar

**Files Affected:**
- `src/ui/EditorGroupManager.h` (add join method)
- `src/ui/EditorGroupManager.cpp` (implement merge logic)

**Acceptance Criteria:**
- Two groups merge into one
- All files from both groups present in the merged tab bar
- Active file from the focused group remains active
- Tree correctly pruned (internal node removed)

**Dependencies:** Task 12

---

## Task 23: Group Navigation History

**Title:** Track group focus history for Cmd+K Cmd+Left/Right cycling

**Description:** Maintain a stack of recently focused groups so that Cmd+K Cmd+P (or similar shortcut) cycles through focus history, similar to Alt+Tab in window managers.

**Implementation Details:**
- Add `std::vector<int> focus_history_` stack
- On every focus change, push the previous group ID
- Limit history to 20 entries
- `void FocusPreviousFromHistory()`: pop and focus the previous group
- `void FocusNextFromHistory()`: move forward in history

**Files Affected:**
- `src/ui/EditorGroupManager.h` (add history tracking)
- `src/ui/EditorGroupManager.cpp` (implement history cycling)

**Acceptance Criteria:**
- Focus history records group focus changes
- Cycling through history works forward and backward
- History limited to 20 entries
- History clears when groups are closed

**Dependencies:** Task 10

---

## Task 24: Status Bar Group Indicator

**Title:** Show the focused group number in the status bar

**Description:** Add a segment to the status bar showing which editor group is currently focused (e.g., "Group 1 of 3"). Click the segment to show a quick-pick for switching groups.

**Implementation Details:**
- Add `void set_editor_group(int current, int total)` to `StatusBarPanel`
- Display in left items: "Group N/M" with accent color when multiple groups exist
- Click handler shows a simple popup menu listing all groups with their active file names
- Hide when only one group exists

**Files Affected:**
- `src/ui/StatusBarPanel.h` (add group setter)
- `src/ui/StatusBarPanel.cpp` (display group info, click handler)
- `src/ui/EditorGroupManager.cpp` (publish group changes to status bar)

**Acceptance Criteria:**
- Status bar shows "Group N/M" when multiple groups exist
- Hidden in single-group mode
- Click shows group list with active file names
- Selecting a group from the list focuses it

**Dependencies:** Task 7

---

## Task 25: Editor Group Event System

**Title:** Define complete event set for editor group lifecycle

**Description:** Create all necessary events for the editor group system to integrate with the rest of the application (command palette, keyboard shortcuts, plugins, etc.).

**Implementation Details:**
- Add to `Events.h`:
  ```cpp
  MARKAMP_DECLARE_EVENT(EditorGroupCreatedEvent, int group_id);
  MARKAMP_DECLARE_EVENT(EditorGroupClosedEvent, int group_id);
  MARKAMP_DECLARE_EVENT(EditorGroupFocusedEvent, int group_id);
  MARKAMP_DECLARE_EVENT(EditorGroupLayoutChangedEvent, int group_count);
  MARKAMP_DECLARE_EVENT(EditorGroupSplitEvent, int source_group_id, SplitOrientation orientation);
  MARKAMP_DECLARE_EVENT(MoveToGroupEvent, int target_group_id);
  MARKAMP_DECLARE_EVENT(MaximizeGroupEvent, int group_id);
  MARKAMP_DECLARE_EVENT(RestoreGroupsEvent);
  MARKAMP_DECLARE_EVENT(EqualizeGroupsEvent);
  ```
- All events published at appropriate lifecycle points in EditorGroupManager
- Subscribe to these events in CommandPalette, StatusBar, and other consumers

**Files Affected:**
- `src/core/Events.h` (add all group events)
- `src/ui/EditorGroupManager.cpp` (publish events at lifecycle points)

**Acceptance Criteria:**
- All lifecycle events published correctly
- Command palette can subscribe and react to group changes
- Status bar updates on group focus changes
- Plugin system can observe group events

**Dependencies:** Tasks 2-12

---

## Estimated Complexity

| Area | Effort |
|------|--------|
| Data Model (Task 1) | Low |
| Manager Shell (Task 2) | Medium |
| Group Factory (Task 3) | Medium |
| Horizontal/Vertical Split (Tasks 4-5) | High |
| Grid Layouts (Task 6) | High |
| Focus Indicator (Task 7) | Medium |
| Header Bar (Task 8) | Medium |
| Tab Drag to Group (Task 9) | High |
| Keyboard Navigation (Task 10) | Medium |
| Maximize/Restore (Task 11) | Medium |
| Close Group (Task 12) | Medium |
| Group Breadcrumbs (Task 13) | Low |
| Independent Tab Bars (Task 14) | Medium |
| Diff Editor (Task 15) | High |
| Divider Drag (Task 16) | Medium |
| State Persistence (Task 17) | Medium |
| LayoutManager Integration (Task 18) | High |
| Focus Follows Mouse (Task 19) | Low |
| Move File Between Groups (Task 20) | Medium |
| Equalize Sizes (Task 21) | Low |
| Join Groups (Task 22) | Medium |
| Navigation History (Task 23) | Low |
| Status Bar Indicator (Task 24) | Low |
| Event System (Task 25) | Medium |

## Files Created

- `src/ui/EditorGroupManager.h`
- `src/ui/EditorGroupManager.cpp`
- `src/ui/GroupHeaderBar.h`
- `src/ui/GroupHeaderBar.cpp`

## Files Modified

- `src/ui/LayoutManager.h`
- `src/ui/LayoutManager.cpp`
- `src/ui/DiffPanel.h`
- `src/ui/DiffPanel.cpp`
- `src/ui/StatusBarPanel.h`
- `src/ui/StatusBarPanel.cpp`
- `src/ui/TabBar.h` / `src/ui/TabBar.cpp`
- `src/core/Events.h`
- `src/core/Config.h` / `src/core/Config.cpp`
- `src/ui/CommandPalette.cpp`
- `src/ui/MainFrame.cpp`
- `CMakeLists.txt`
