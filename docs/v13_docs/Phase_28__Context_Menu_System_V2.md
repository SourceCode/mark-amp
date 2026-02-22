# Phase 28: Context Menu System V2

## Overview

Implement a comprehensive context menu system with a fluent `ContextMenuBuilder` API, hierarchical sub-menus, icon support, keyboard shortcut hints, dynamic item generation, disabled item tooltips, and context menus for every interactive surface in the application: editor, tab bar, file tree, terminal, breadcrumb bar, status bar, and gutter. This replaces ad-hoc `wxMenu` creation with a unified, testable, theme-aware menu system.

## Prerequisites

- Phase 27 (Panel Header Action Bars) -- icon library and action patterns
- Existing `ContextMenuModel` at `/Users/ryanrentfro/code/markamp/src/ui/ContextMenuModel.h`
- Existing right-click handlers in TabBar, EditorPanel, FileTreeCtrl
- Existing `ThemeAwareWindow` and `ThemeEngine`

## Target Files

| Action | File |
|--------|------|
| Create | `/Users/ryanrentfro/code/markamp/src/ui/ContextMenuBuilder.h` |
| Create | `/Users/ryanrentfro/code/markamp/src/ui/ContextMenuBuilder.cpp` |
| Create | `/Users/ryanrentfro/code/markamp/src/ui/ThemedContextMenu.h` |
| Create | `/Users/ryanrentfro/code/markamp/src/ui/ThemedContextMenu.cpp` |
| Modify | `/Users/ryanrentfro/code/markamp/src/ui/ContextMenuModel.h` |
| Modify | `/Users/ryanrentfro/code/markamp/src/ui/ContextMenuModel.cpp` |
| Modify | `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp` |
| Modify | `/Users/ryanrentfro/code/markamp/src/ui/TabBar.cpp` |
| Modify | `/Users/ryanrentfro/code/markamp/src/ui/FileTreeCtrl.cpp` |
| Modify | `/Users/ryanrentfro/code/markamp/src/ui/TerminalPanel.cpp` |
| Modify | `/Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp` |
| Modify | `/Users/ryanrentfro/code/markamp/src/ui/StatusBarPanel.cpp` |
| Modify | `/Users/ryanrentfro/code/markamp/src/CMakeLists.txt` |
| Create | `/Users/ryanrentfro/code/markamp/tests/unit/test_context_menu_v2.cpp` |

## Tasks

### Task 1: Design ContextMenuBuilder Fluent API

**Title:** Create a builder class for constructing context menus declaratively

**Description:** Implement a `ContextMenuBuilder` with a fluent API that allows method chaining to construct menus with items, separators, sub-menus, icons, shortcuts, and dynamic generators.

**Implementation Details:**
Create `/Users/ryanrentfro/code/markamp/src/ui/ContextMenuBuilder.h`:
```cpp
#pragma once
#include "ContextMenuModel.h"
#include <functional>
#include <string>
#include <vector>
#include <memory>

namespace markamp::ui
{

class ContextMenuBuilder
{
public:
    ContextMenuBuilder() = default;

    auto item(const std::string& action_id, const std::string& label) -> ContextMenuBuilder&;
    auto item(const std::string& action_id, const std::string& label,
              const std::string& shortcut) -> ContextMenuBuilder&;
    auto separator() -> ContextMenuBuilder&;
    auto sub_menu(const std::string& label) -> ContextMenuBuilder&;
    auto end_sub_menu() -> ContextMenuBuilder&;
    auto icon(PanelHeaderIconId icon_id) -> ContextMenuBuilder&;
    auto enabled(bool is_enabled) -> ContextMenuBuilder&;
    auto enabled_when(std::function<bool()> predicate) -> ContextMenuBuilder&;
    auto visible(bool is_visible) -> ContextMenuBuilder&;
    auto checked(bool is_checked) -> ContextMenuBuilder&;
    auto category(MenuActionCategory cat) -> ContextMenuBuilder&;
    auto disabled_tooltip(const std::string& tooltip) -> ContextMenuBuilder&;

    // Dynamic item generation
    auto dynamic(std::function<std::vector<ContextMenuItem>()> generator) -> ContextMenuBuilder&;

    // Callback for when an action is selected
    auto on_action(const std::string& action_id,
                   std::function<void()> callback) -> ContextMenuBuilder&;

    [[nodiscard]] auto build() -> ContextMenuModel;
    [[nodiscard]] auto build_callbacks() -> std::unordered_map<std::string, std::function<void()>>;

private:
    struct BuilderItem
    {
        ContextMenuItem item;
        PanelHeaderIconId icon{};
        bool has_icon{false};
        std::string disabled_tooltip;
        std::function<bool()> enabled_predicate;
        std::vector<BuilderItem> children; // For sub-menus
    };

    std::vector<BuilderItem> items_;
    std::vector<BuilderItem*> sub_menu_stack_;
    std::unordered_map<std::string, std::function<void()>> callbacks_;
    std::vector<std::function<std::vector<ContextMenuItem>()>> generators_;

    auto current_items() -> std::vector<BuilderItem>&;
};

} // namespace markamp::ui
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/ContextMenuBuilder.h` (create)
- `/Users/ryanrentfro/code/markamp/src/ui/ContextMenuBuilder.cpp` (create)

**Acceptance Criteria:**
- Fluent API supports chaining: `builder.item("cut", "Cut").shortcut("Cmd+X").separator()`
- Sub-menus can be nested: `builder.sub_menu("Refactor").item(...).end_sub_menu()`
- `build()` produces a valid `ContextMenuModel`
- Dynamic generators are called at build time and inject items
- Callbacks are stored and retrievable by action_id
- Empty builders produce empty models (not crashes)

**Dependencies:** None

---

### Task 2: Implement ThemedContextMenu Renderer

**Title:** Custom-drawn context menu with theme colors, icons, and shortcut hints

**Description:** Create a `ThemedContextMenu` class that renders context menus using the application theme instead of native OS menus. The themed menu supports icons next to labels, keyboard shortcut hints right-aligned, disabled item styling, and sub-menu indicators.

**Implementation Details:**
Create `/Users/ryanrentfro/code/markamp/src/ui/ThemedContextMenu.h`:
```cpp
#pragma once
#include "ContextMenuModel.h"
#include "PanelHeaderIcons.h"
#include "core/ThemeEngine.h"
#include <wx/popupwin.h>
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::ui
{

struct MenuRenderItem
{
    ContextMenuItem data;
    PanelHeaderIconId icon{};
    bool has_icon{false};
    std::string disabled_tooltip;
    wxRect rect;
    bool is_hovered{false};
    bool has_submenu{false};
    std::vector<MenuRenderItem> children;
};

class ThemedContextMenu : public wxPopupTransientWindow
{
public:
    ThemedContextMenu(wxWindow* parent,
                      core::ThemeEngine& theme_engine,
                      const ContextMenuModel& model,
                      std::unordered_map<std::string, std::function<void()>> callbacks);

    void ShowAt(const wxPoint& position);

    static constexpr int kRowHeight = 26;
    static constexpr int kIconColumnWidth = 24;
    static constexpr int kShortcutPadding = 40;
    static constexpr int kSubmenuArrowWidth = 16;
    static constexpr int kMinWidth = 180;
    static constexpr int kMaxWidth = 400;
    static constexpr int kSeparatorHeight = 9;
    static constexpr int kBorderRadius = 6;

private:
    core::ThemeEngine& theme_engine_;
    std::vector<MenuRenderItem> items_;
    std::unordered_map<std::string, std::function<void()>> callbacks_;
    int hovered_index_{-1};
    ThemedContextMenu* active_submenu_{nullptr};

    void OnPaint(wxPaintEvent& event);
    void OnMouseMove(wxMouseEvent& event);
    void OnMouseDown(wxMouseEvent& event);
    void OnMouseLeave(wxMouseEvent& event);
    void OnKeyDown(wxKeyEvent& event);

    void DrawItem(wxGraphicsContext& gc, const MenuRenderItem& item, const core::Theme& theme);
    void DrawSeparator(wxGraphicsContext& gc, int y, int width, const core::Theme& theme);
    void ShowSubmenu(int item_index);
    void CloseSubmenu();

    [[nodiscard]] auto CalculateMenuSize() -> wxSize;
};

} // namespace markamp::ui
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/ThemedContextMenu.h` (create)
- `/Users/ryanrentfro/code/markamp/src/ui/ThemedContextMenu.cpp` (create)

**Acceptance Criteria:**
- Menu background uses `bg_popup` theme token with rounded corners
- Menu has a subtle drop shadow
- Items render with: icon (24px column), label, shortcut (right-aligned)
- Disabled items render in dimmed color and do not respond to clicks
- Hovered items have a subtle highlight background
- Separators render as thin lines with vertical padding
- Sub-menus show a right-arrow indicator and open on hover
- Menu dismisses on click outside or Escape key
- Keyboard navigation: Up/Down to move, Enter to select, Right for sub-menu, Left to close sub-menu

**Dependencies:** Phase 27 Task 3 (PanelHeaderIcons)

---

### Task 3: Extend ContextMenuModel with Sub-Menu Support

**Title:** Add hierarchical sub-menu support to the existing ContextMenuModel

**Description:** Extend `ContextMenuModel` and `ContextMenuItem` to support nested sub-menus, icon references, and disabled tooltips.

**Implementation Details:**
Update `/Users/ryanrentfro/code/markamp/src/ui/ContextMenuModel.h`:
```cpp
struct ContextMenuItem
{
    // ... existing fields ...
    PanelHeaderIconId icon{};
    bool has_icon{false};
    std::string disabled_tooltip;
    bool is_checked{false};
    bool has_submenu{false};
    std::vector<ContextMenuItem> children; // Sub-menu items
};
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/ContextMenuModel.h`
- `/Users/ryanrentfro/code/markamp/src/ui/ContextMenuModel.cpp`

**Acceptance Criteria:**
- `ContextMenuItem` supports `children` vector for sub-menus
- `has_submenu` is automatically set when `children` is non-empty
- `sorted_items()` includes sub-menu items in their parent's position
- `visible_items()` filters sub-menu items recursively
- Existing tests continue to pass with the extended struct

**Dependencies:** None

---

### Task 4: Implement Editor Context Menu

**Title:** Full context menu for the editor panel

**Description:** Implement a comprehensive editor right-click context menu with groups: Edit (Cut/Copy/Paste), Navigation (Go To Definition, Go To Line, Peek), Refactor (Extract, Rename), Format (Format Document, Sort Lines), and Selection (Select All, Expand Selection).

**Implementation Details:**
```cpp
void EditorPanel::ShowEditorContextMenu()
{
    auto builder = ContextMenuBuilder()
        .item("editor.cut", "Cut", "Cmd+X")
            .enabled(HasSelection())
            .icon(PanelHeaderIconId::kCut)
        .item("editor.copy", "Copy", "Cmd+C")
            .enabled(HasSelection())
        .item("editor.paste", "Paste", "Cmd+V")
        .separator()
        .sub_menu("Go To")
            .item("editor.goto_line", "Go to Line...", "Cmd+G")
            .item("editor.goto_symbol", "Go to Symbol...", "Cmd+Shift+O")
            .item("editor.goto_bracket", "Go to Matching Bracket", "Cmd+Shift+\\")
        .end_sub_menu()
        .sub_menu("Peek")
            .item("editor.peek_definition", "Peek Definition")
            .item("editor.peek_problem", "Peek Problem")
        .end_sub_menu()
        .separator()
        .sub_menu("Refactor")
            .item("editor.extract_heading", "Extract to Heading")
            .item("editor.convert_link", "Convert to Reference Link")
        .end_sub_menu()
        .separator()
        .item("editor.format_document", "Format Document", "Cmd+Shift+F")
        .item("editor.toggle_comment", "Toggle Comment", "Cmd+/")
        .separator()
        .item("editor.select_all", "Select All", "Cmd+A")
        .item("editor.expand_selection", "Expand Selection", "Cmd+Shift+Up");

    auto model = builder.build();
    auto callbacks = builder.build_callbacks();
    // Wire callbacks...

    ThemedContextMenu menu(this, theme_engine(), model, std::move(callbacks));
    menu.ShowAt(ScreenToClient(wxGetMousePosition()));
}
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp`

**Acceptance Criteria:**
- Editor right-click shows themed context menu
- Cut/Copy disabled when no selection
- Paste disabled when clipboard is empty
- Go To sub-menu with navigation options
- Refactor sub-menu with markdown-specific actions
- All menu items fire the correct editor methods
- Menu shows keyboard shortcut hints for all actions

**Dependencies:** Task 1, Task 2

---

### Task 5: Implement Tab Bar Context Menu

**Title:** Context menu for editor tabs

**Description:** Implement tab right-click menu with: Close, Close Others, Close All, Close Saved, Close to Left, Close to Right, Pin/Unpin, Split Right, Copy Path, Copy Relative Path, Reveal in File Explorer, Open Containing Folder.

**Implementation Details:**
```cpp
void TabBar::ShowTabContextMenu(int tab_index)
{
    const auto& tab = tabs_[tab_index];
    auto builder = ContextMenuBuilder()
        .item("tab.close", "Close", "Cmd+W")
        .item("tab.close_others", "Close Others")
        .item("tab.close_all", "Close All")
        .item("tab.close_saved", "Close Saved")
        .item("tab.close_left", "Close to the Left")
        .item("tab.close_right", "Close to the Right")
        .separator()
        .item("tab.pin", tab.is_pinned ? "Unpin" : "Pin")
        .separator()
        .item("tab.split_right", "Split Right")
        .item("tab.duplicate", "Duplicate Tab")
        .separator()
        .item("tab.copy_path", "Copy Path")
        .item("tab.copy_relative", "Copy Relative Path")
        .item("tab.reveal", "Reveal in File Explorer")
        .item("tab.open_folder", "Open Containing Folder");

    // ... build and show
}
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/TabBar.cpp`

**Acceptance Criteria:**
- Right-click on a tab shows the context menu
- Close actions work correctly for the clicked tab (not the active tab)
- Pin/Unpin toggles based on current pin state
- Copy Path copies the full absolute path to clipboard
- Copy Relative Path copies the workspace-relative path
- Reveal opens the system file manager at the file's location
- Close to Left/Right close the correct set of tabs

**Dependencies:** Task 1, Task 2

---

### Task 6: Implement File Tree Context Menu

**Title:** Context menu for file tree items

**Description:** Implement file/folder right-click menu with: New File, New Folder, Rename, Delete, Copy Path, Copy Relative Path, Reveal in Finder, Open in Terminal, Open With, Duplicate.

**Implementation Details:**
```cpp
void FileTreeCtrl::ShowContextMenu(const wxPoint& pos)
{
    auto item = HitTest(pos);
    bool is_folder = IsFolder(item);
    bool is_file = !is_folder && item.IsOk();

    auto builder = ContextMenuBuilder()
        .item("tree.new_file", "New File", "Cmd+N")
        .item("tree.new_folder", "New Folder", "Cmd+Shift+N")
        .separator()
        .item("tree.rename", "Rename", "Enter")
            .enabled(is_file || is_folder)
        .item("tree.delete", "Delete", "Cmd+Backspace")
            .enabled(is_file || is_folder)
        .item("tree.duplicate", "Duplicate")
            .enabled(is_file)
        .separator()
        .item("tree.copy_path", "Copy Path")
            .enabled(is_file || is_folder)
        .item("tree.copy_relative", "Copy Relative Path")
            .enabled(is_file || is_folder)
        .separator()
        .item("tree.reveal", "Reveal in Finder")
            .enabled(is_file || is_folder)
        .item("tree.terminal", "Open in Terminal")
            .enabled(is_folder);

    // ... build and show
}
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/FileTreeCtrl.cpp`

**Acceptance Criteria:**
- Right-click on a file shows file-specific actions
- Right-click on a folder shows folder-specific actions
- Right-click on empty space shows New File/Folder only
- Delete shows confirmation dialog
- Rename inline editing starts on Enter or menu selection
- Open in Terminal creates a terminal in the folder's directory

**Dependencies:** Task 1, Task 2

---

### Task 7: Implement Terminal Context Menu

**Title:** Context menu for the terminal panel

**Description:** Implement terminal right-click menu with: Copy, Paste, Select All, Find, Clear Terminal, Split Terminal, Kill Process.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/TerminalPanel.cpp`

**Acceptance Criteria:**
- Copy is disabled when no selection
- Paste sends clipboard to the PTY
- Select All selects all terminal content
- Clear Terminal clears buffer and scrollback
- Kill Process shows confirmation before sending SIGTERM

**Dependencies:** Task 1, Task 2

---

### Task 8: Implement Breadcrumb Context Menu

**Title:** Context menu for breadcrumb bar segments

**Description:** Right-clicking a breadcrumb segment shows: Copy Path, Copy Relative Path, Open in Terminal, Reveal in Finder, Open File (for file segments), and a sub-menu listing sibling files/folders.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp`

**Acceptance Criteria:**
- Right-click on a path segment shows context menu
- Copy Path copies the full path up to that segment
- Siblings sub-menu lists files/folders at the same level
- Clicking a sibling navigates to that file/folder

**Dependencies:** Task 1, Task 2

---

### Task 9: Implement Status Bar Context Menu

**Title:** Context menus for clickable status bar items

**Description:** Status bar items that are clickable (encoding, EOL mode, indent mode, language) should show a context menu or quick-pick when clicked, allowing the user to change the value.

**Implementation Details:**
```cpp
void StatusBarPanel::OnMouseDown(wxMouseEvent& event)
{
    for (const auto& item : right_items_)
    {
        if (item.bounds.Contains(event.GetPosition()) && item.is_clickable)
        {
            if (item.text == encoding_)
            {
                ShowEncodingPicker(item.bounds);
            }
            else if (item.text == eol_mode_)
            {
                ShowEolPicker(item.bounds);
            }
            // ...
        }
    }
}
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/StatusBarPanel.cpp`

**Acceptance Criteria:**
- Clicking encoding shows: UTF-8, UTF-16 LE, UTF-16 BE, ASCII, Latin-1
- Clicking EOL mode shows: LF, CRLF, CR
- Clicking indent mode shows: Spaces: 2, Spaces: 4, Tabs
- Clicking language shows available syntax modes
- Selected value updates immediately in the editor
- Picker popup is themed to match the application

**Dependencies:** Task 1, Task 2

---

### Task 10: Implement Gutter Context Menu

**Title:** Context menu for the editor gutter (line number area)

**Description:** Right-clicking the editor gutter shows: Add Breakpoint, Add Bookmark, Toggle Line Comment, Fold/Unfold Region, Copy Line Number.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp`

**Acceptance Criteria:**
- Right-click in the gutter area (left of line numbers) shows menu
- Add Breakpoint toggles a breakpoint marker on that line
- Add Bookmark adds a named bookmark
- Toggle Line Comment comments/uncomments the line
- Fold/Unfold only appears on foldable regions
- Copy Line Number copies the clicked line number

**Dependencies:** Task 1, Task 2

---

### Task 11: Implement Dynamic Menu Item Generation

**Title:** Support for dynamically generated menu items at display time

**Description:** Implement the dynamic generator feature in `ContextMenuBuilder` that allows menus to include items generated at display time. For example, the "Recent Files" sub-menu or "Active Terminals" sub-menu items are generated based on current state.

**Implementation Details:**
```cpp
auto builder = ContextMenuBuilder()
    .sub_menu("Open Recent")
    .dynamic([&recent_files]()
    {
        std::vector<ContextMenuItem> items;
        for (const auto& file : recent_files)
        {
            items.push_back({.action_id = "open:" + file, .label = filename(file)});
        }
        return items;
    })
    .end_sub_menu();
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/ContextMenuBuilder.cpp`

**Acceptance Criteria:**
- Dynamic generators are called during `build()`
- Generated items are inserted at the generator's position
- Empty generators produce no items (not empty sub-menus)
- Generators can produce items with icons and shortcuts
- Multiple generators can be in the same menu

**Dependencies:** Task 1

---

### Task 12: Implement Disabled Item Tooltips

**Title:** Show explanatory tooltips for disabled menu items

**Description:** When a menu item is disabled, hovering over it can show a tooltip explaining why it is disabled (e.g., "Copy: No text selected", "Paste: Clipboard is empty").

**Implementation Details:**
```cpp
builder.item("editor.copy", "Copy", "Cmd+C")
    .enabled(HasSelection())
    .disabled_tooltip("No text selected");
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/ThemedContextMenu.cpp`

**Acceptance Criteria:**
- Hovering over a disabled item for 500ms shows the disabled tooltip
- Tooltip appears near the item, not at the mouse cursor
- Tooltip uses a dimmed style to distinguish from action tooltips
- Enabled items do not show disabled tooltips

**Dependencies:** Task 2

---

### Task 13: Implement Keyboard Navigation in Menus

**Title:** Full keyboard navigation for themed context menus

**Description:** Support arrow key navigation, Enter to select, Escape to close, type-ahead to jump to items, and Right/Left for sub-menu navigation.

**Implementation Details:**
```cpp
void ThemedContextMenu::OnKeyDown(wxKeyEvent& event)
{
    switch (event.GetKeyCode())
    {
        case WXK_UP: MoveSelection(-1); break;
        case WXK_DOWN: MoveSelection(1); break;
        case WXK_RIGHT: OpenSelectedSubmenu(); break;
        case WXK_LEFT: CloseToParent(); break;
        case WXK_RETURN: ActivateSelected(); break;
        case WXK_ESCAPE: Dismiss(); break;
        default:
            // Type-ahead: jump to first item starting with typed character
            TypeAhead(event.GetUnicodeKey());
            break;
    }
}
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/ThemedContextMenu.cpp`

**Acceptance Criteria:**
- Up/Down arrow moves selection (skipping separators and disabled items)
- Right arrow opens sub-menu and selects first item
- Left arrow closes sub-menu and returns to parent
- Enter activates the selected item and closes the menu
- Escape closes the menu (and all sub-menus)
- Typing a character jumps to the next item starting with that character
- Selection wraps from last to first and vice versa

**Dependencies:** Task 2

---

### Task 14: Update CMakeLists.txt

**Title:** Add ContextMenuBuilder and ThemedContextMenu to the build

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/CMakeLists.txt`

**Acceptance Criteria:**
- All new `.h` and `.cpp` files in both `add_executable()` and `source_group()`
- Build succeeds

**Dependencies:** All create tasks

---

### Task 15: Write Unit Tests for ContextMenuBuilder

**Title:** Test builder API and model generation

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/tests/unit/test_context_menu_v2.cpp` (create)

**Acceptance Criteria:**
- Tests verify fluent API produces correct model
- Tests verify sub-menu nesting
- Tests verify dynamic generation
- Tests verify enabled/disabled states
- Tests verify separator placement
- Tests verify empty builder produces empty model
- All tests pass

**Dependencies:** Task 1

---

### Task 16: Write Editor Context Menu Tests

**Title:** Test editor context menu item states and callbacks

**Description:** Verify editor right-click menu: Cut/Copy disabled without selection, Paste disabled with empty clipboard, Go To sub-menu items present, Refactor sub-menu items present.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/tests/unit/test_context_menu_v2.cpp`

**Acceptance Criteria:**
- Tests verify Cut/Copy disabled when no selection
- Tests verify Go To sub-menu contains Go to Line, Go to Symbol, Go to Bracket
- Tests verify all action callbacks fire correct editor methods
- All tests pass

**Dependencies:** Task 4

---

### Task 17: Write Tab Bar Context Menu Tests

**Title:** Test tab bar context menu for various tab states

**Description:** Verify tab context menu: Close variants work on the correct tab (not the active tab), Pin/Unpin toggles correctly, Copy Path copies the right path.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/tests/unit/test_context_menu_v2.cpp`

**Acceptance Criteria:**
- Tests verify Close Others closes all tabs except the right-clicked one
- Tests verify Pin label changes based on current pin state
- Tests verify Copy Path puts the absolute path on the clipboard
- All tests pass

**Dependencies:** Task 5

---

### Task 18: Write File Tree Context Menu Tests

**Title:** Test file tree context menu for file and folder targets

**Description:** Verify different menu items appear for files vs folders vs empty space right-clicks.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/tests/unit/test_context_menu_v2.cpp`

**Acceptance Criteria:**
- Tests verify Rename/Delete are disabled on empty space
- Tests verify Open in Terminal only appears for folders
- Tests verify Duplicate only appears for files
- All tests pass

**Dependencies:** Task 6

---

### Task 19: Write Terminal Context Menu Tests

**Title:** Test terminal context menu states

**Description:** Verify Copy is disabled without selection, Paste always enabled, Kill shows confirmation.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/tests/unit/test_context_menu_v2.cpp`

**Acceptance Criteria:**
- Tests verify Copy disabled when no terminal selection
- Tests verify all action IDs map to correct terminal methods
- All tests pass

**Dependencies:** Task 7

---

### Task 20: Write Breadcrumb Context Menu Tests

**Title:** Test breadcrumb right-click menu for path segments

**Description:** Verify Copy Path copies the correct partial path, siblings sub-menu lists correct files.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/tests/unit/test_context_menu_v2.cpp`

**Acceptance Criteria:**
- Tests verify Copy Path copies path up to the clicked segment
- Tests verify sibling items are generated correctly
- All tests pass

**Dependencies:** Task 8

---

### Task 21: Write Status Bar Picker Tests

**Title:** Test status bar item click actions and picker values

**Description:** Verify encoding picker shows correct options, EOL picker shows LF/CRLF/CR, indent picker shows spaces/tabs options.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/tests/unit/test_context_menu_v2.cpp`

**Acceptance Criteria:**
- Tests verify encoding options include UTF-8, UTF-16 LE, UTF-16 BE
- Tests verify EOL options include LF, CRLF, CR
- Tests verify selecting an option publishes the correct event
- All tests pass

**Dependencies:** Task 9

---

### Task 22: Write Gutter Context Menu Tests

**Title:** Test editor gutter right-click menu

**Description:** Verify breakpoint toggle, bookmark add, comment toggle, and fold/unfold conditions.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/tests/unit/test_context_menu_v2.cpp`

**Acceptance Criteria:**
- Tests verify Fold/Unfold only appears on foldable lines
- Tests verify Add Breakpoint toggles a marker
- Tests verify Copy Line Number copies the correct number
- All tests pass

**Dependencies:** Task 10

---

### Task 23: Write Keyboard Navigation Tests

**Title:** Test arrow key, Enter, Escape, and type-ahead in themed menus

**Description:** Verify full keyboard navigation works correctly in the ThemedContextMenu.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/tests/unit/test_context_menu_v2.cpp`

**Acceptance Criteria:**
- Tests verify Up/Down skip separators and disabled items
- Tests verify Enter activates the selected item
- Tests verify Escape dismisses the menu
- Tests verify type-ahead jumps to matching items
- All tests pass

**Dependencies:** Task 13

---

### Task 24: Write Sub-Menu Behavior Tests

**Title:** Test sub-menu opening, closing, and nesting

**Description:** Verify sub-menus open on hover/right-arrow, close on left-arrow, and support nesting up to 3 levels deep.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/tests/unit/test_context_menu_v2.cpp`

**Acceptance Criteria:**
- Tests verify sub-menu opens when hovering over a parent item
- Tests verify Right arrow opens sub-menu
- Tests verify Left arrow closes sub-menu and returns focus to parent
- Tests verify up to 3 levels of nesting work correctly
- All tests pass

**Dependencies:** Task 2

---

### Task 25: Write Dynamic Item Generation Tests

**Title:** Test dynamic menu item generators

**Description:** Verify dynamic generators produce correct items, empty generators produce no items, and multiple generators work in the same menu.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/tests/unit/test_context_menu_v2.cpp`

**Acceptance Criteria:**
- Tests verify generator is called during build()
- Tests verify generated items appear at the correct position
- Tests verify empty generator produces no items (no empty sub-menu)
- Tests verify multiple generators in one menu all execute
- All tests pass

**Dependencies:** Task 11

---

### Task 26: Migrate Existing Context Menus

**Title:** Replace all ad-hoc wxMenu usage with ContextMenuBuilder

**Description:** Search for all `wxMenu` usage in the codebase and replace with the new `ContextMenuBuilder` and `ThemedContextMenu` system. This ensures all context menus are themed consistently.

**Files Affected:**
- All files currently using `wxMenu` for context menus
- `/Users/ryanrentfro/code/markamp/src/ui/TabBar.cpp`
- `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp`
- `/Users/ryanrentfro/code/markamp/src/ui/FileTreeCtrl.cpp`

**Acceptance Criteria:**
- No direct `wxMenu` usage remains for context menus
- All context menus use the themed renderer
- Existing functionality is preserved
- Menu appearance is consistent across all surfaces

**Dependencies:** Tasks 4-10

---

### Task 27: Documentation

**Title:** Document the context menu system V2

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/docs/architecture.md`

**Acceptance Criteria:**
- ContextMenuBuilder API documented with examples
- Per-surface menu inventories listed
- ThemedContextMenu rendering pipeline described
- Migration guide from wxMenu to ContextMenuBuilder

**Dependencies:** All previous tasks

## Testing Requirements

- Unit tests for ContextMenuBuilder API
- Unit tests for model generation and sub-menu nesting
- Integration tests for each interactive surface
- Keyboard navigation tests
- Theme switching with menus open

## Phase Completion Criteria

- Every interactive surface has a comprehensive, themed context menu
- ContextMenuBuilder fluent API is used consistently across the codebase
- Sub-menus, icons, shortcuts, and disabled tooltips all render correctly
- Keyboard navigation works in all menus
- No ad-hoc wxMenu usage remains
- All unit and integration tests pass
