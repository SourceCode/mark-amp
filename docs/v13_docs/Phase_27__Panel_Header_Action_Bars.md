# Phase 27: Panel Header Action Bars

## Overview

Implement a standardized panel header action bar system that provides contextual action buttons at the top of every sidebar and bottom panel. Each panel defines its own set of actions (icons with tooltips, callbacks, and enabled states), rendered through a shared `PanelHeaderBar` widget. This brings all panels to feature parity with professional IDEs where every panel header offers quick-access actions.

## Prerequisites

- Phase 21-25 (All panel implementations) -- panels exist to receive header bars
- Phase 26 (Toolbar Redesign) -- establishes icon drawing patterns
- Existing panels: OutputPanel, ProblemsPanel, TerminalPanel, FileTreeCtrl, SearchPanel, BuildPanel, DebugConsolePanel
- Existing `ThemeAwareWindow` base class

## Target Files

| Action | File |
|--------|------|
| Create | `/Users/ryanrentfro/code/markamp/src/ui/PanelHeaderBar.h` |
| Create | `/Users/ryanrentfro/code/markamp/src/ui/PanelHeaderBar.cpp` |
| Create | `/Users/ryanrentfro/code/markamp/src/ui/PanelHeaderIcons.h` |
| Create | `/Users/ryanrentfro/code/markamp/src/ui/PanelHeaderIcons.cpp` |
| Modify | `/Users/ryanrentfro/code/markamp/src/ui/FileTreeCtrl.h` |
| Modify | `/Users/ryanrentfro/code/markamp/src/ui/FileTreeCtrl.cpp` |
| Modify | `/Users/ryanrentfro/code/markamp/src/ui/SearchPanel.h` |
| Modify | `/Users/ryanrentfro/code/markamp/src/ui/SearchPanel.cpp` |
| Modify | `/Users/ryanrentfro/code/markamp/src/ui/OutputPanel.h` |
| Modify | `/Users/ryanrentfro/code/markamp/src/ui/OutputPanel.cpp` |
| Modify | `/Users/ryanrentfro/code/markamp/src/ui/ProblemsPanel.h` |
| Modify | `/Users/ryanrentfro/code/markamp/src/ui/ProblemsPanel.cpp` |
| Modify | `/Users/ryanrentfro/code/markamp/src/ui/TerminalPanel.h` |
| Modify | `/Users/ryanrentfro/code/markamp/src/ui/TerminalPanel.cpp` |
| Modify | `/Users/ryanrentfro/code/markamp/src/ui/BuildPanel.h` |
| Modify | `/Users/ryanrentfro/code/markamp/src/ui/BuildPanel.cpp` |
| Modify | `/Users/ryanrentfro/code/markamp/src/ui/DebugConsolePanel.h` |
| Modify | `/Users/ryanrentfro/code/markamp/src/ui/DebugConsolePanel.cpp` |
| Modify | `/Users/ryanrentfro/code/markamp/src/CMakeLists.txt` |
| Create | `/Users/ryanrentfro/code/markamp/tests/unit/test_panel_header_bar.cpp` |

## Tasks

### Task 1: Define PanelHeaderAction Data Structure

**Title:** Create the PanelHeaderAction struct for declarative action definitions

**Description:** Define a data structure that each panel uses to declare its header actions. Actions have an icon identifier, tooltip text, callback function, enabled state, toggle state, and optional keyboard shortcut.

**Implementation Details:**
Create in `/Users/ryanrentfro/code/markamp/src/ui/PanelHeaderBar.h`:
```cpp
#pragma once
#include "ThemeAwareWindow.h"
#include <functional>
#include <string>
#include <vector>

namespace markamp::ui
{

enum class PanelHeaderIconId : uint8_t
{
    kNewFile, kNewFolder, kCollapseAll, kExpandAll, kRefresh, kFilter,
    kTerminal, kClear, kLockScroll, kUnlockScroll, kOpenInEditor,
    kNewTerminal, kSplitTerminal, kKillTerminal, kScrollToBottom,
    kSearch, kRegex, kCaseSensitive, kWholeWord,
    kQuickFixAll, kSortBySeverity, kSortByFile, kSortByPosition,
    kBuild, kRebuild, kClean, kConfigure, kStop,
    kTimestamps, kWordWrap, kLogLevelFilter,
    kOverflowMenu,
};

struct PanelHeaderAction
{
    PanelHeaderIconId icon;
    std::string tooltip;
    std::string shortcut_hint;
    std::function<void()> callback;
    std::function<bool()> is_enabled; // Dynamic enablement
    bool is_toggle{false};
    std::function<bool()> is_toggled; // Current toggle state
    bool visible{true};
};

} // namespace markamp::ui
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/PanelHeaderBar.h` (create)

**Acceptance Criteria:**
- `PanelHeaderAction` struct captures all action metadata
- Dynamic enablement via `is_enabled` callback (called before each paint)
- Toggle state via `is_toggled` callback
- Icon enum covers all needed icons across all panels
- Shortcut hint is displayed in the tooltip

**Dependencies:** None

---

### Task 2: Implement PanelHeaderBar Widget

**Title:** Shared panel header bar widget with title and action buttons

**Description:** Create a reusable `PanelHeaderBar` widget that renders a title label on the left and action buttons on the right. The bar is 28px tall, uses theme colors, and supports any number of actions with overflow handling.

**Implementation Details:**
```cpp
class PanelHeaderBar : public ThemeAwareWindow
{
public:
    PanelHeaderBar(wxWindow* parent,
                   core::ThemeEngine& theme_engine,
                   const std::string& title);

    void set_title(const std::string& title);
    void set_actions(std::vector<PanelHeaderAction> actions);
    void update_action_states(); // Re-query enabled/toggled states

    static constexpr int kHeight = 28;
    static constexpr int kIconSize = 16;
    static constexpr int kIconPadding = 4;
    static constexpr int kTitlePaddingLeft = 8;

protected:
    void OnThemeChanged(const core::Theme& new_theme) override;

private:
    std::string title_;
    std::vector<PanelHeaderAction> actions_;

    struct ActionButton
    {
        wxRect rect;
        int action_index{0};
        bool is_hovered{false};
    };
    std::vector<ActionButton> buttons_;
    int hovered_button_{-1};

    void OnPaint(wxPaintEvent& event);
    void OnMouseDown(wxMouseEvent& event);
    void OnMouseMove(wxMouseEvent& event);
    void OnMouseLeave(wxMouseEvent& event);
    void OnSize(wxSizeEvent& event);

    void RecalculateButtonRects();
    void ShowOverflowMenu(const std::vector<int>& overflow_indices);
};
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/PanelHeaderBar.h`
- `/Users/ryanrentfro/code/markamp/src/ui/PanelHeaderBar.cpp` (create)

**Acceptance Criteria:**
- Title renders left-aligned in the panel header font
- Action buttons render right-aligned with 16x16 icons and 4px padding
- Hover state shows a subtle background highlight
- Disabled actions render in dimmed color and do not respond to clicks
- Toggle actions show a pressed/active visual state when toggled on
- Overflow actions (when panel is too narrow) are collected into a "..." menu
- Bar height is exactly 28px
- Background uses `bg_panel_header` theme token

**Dependencies:** Task 1

---

### Task 3: Implement Panel Header Icon Library

**Title:** Draw all panel header icons using wxGraphicsContext

**Description:** Create a centralized icon drawing library for all panel header action icons. Each icon is a `wxGraphicsContext` drawing function that renders at a given position and size, using theme colors.

**Implementation Details:**
Create `/Users/ryanrentfro/code/markamp/src/ui/PanelHeaderIcons.h`:
```cpp
#pragma once
#include "core/Theme.h"
#include <wx/graphics.h>

namespace markamp::ui
{

class PanelHeaderIcons
{
public:
    static void Draw(wxGraphicsContext& gc, PanelHeaderIconId icon,
                     double x, double y, double size,
                     const core::Theme& theme, bool enabled = true,
                     bool toggled = false);

private:
    static void DrawNewFile(wxGraphicsContext& gc, double x, double y, double s, const wxColour& c);
    static void DrawNewFolder(wxGraphicsContext& gc, double x, double y, double s, const wxColour& c);
    static void DrawCollapseAll(wxGraphicsContext& gc, double x, double y, double s, const wxColour& c);
    static void DrawExpandAll(wxGraphicsContext& gc, double x, double y, double s, const wxColour& c);
    static void DrawRefresh(wxGraphicsContext& gc, double x, double y, double s, const wxColour& c);
    static void DrawFilter(wxGraphicsContext& gc, double x, double y, double s, const wxColour& c);
    static void DrawTerminal(wxGraphicsContext& gc, double x, double y, double s, const wxColour& c);
    static void DrawClear(wxGraphicsContext& gc, double x, double y, double s, const wxColour& c);
    static void DrawLock(wxGraphicsContext& gc, double x, double y, double s, const wxColour& c);
    static void DrawUnlock(wxGraphicsContext& gc, double x, double y, double s, const wxColour& c);
    static void DrawOpenInEditor(wxGraphicsContext& gc, double x, double y, double s, const wxColour& c);
    static void DrawPlus(wxGraphicsContext& gc, double x, double y, double s, const wxColour& c);
    static void DrawSplit(wxGraphicsContext& gc, double x, double y, double s, const wxColour& c);
    static void DrawKill(wxGraphicsContext& gc, double x, double y, double s, const wxColour& c);
    static void DrawScrollBottom(wxGraphicsContext& gc, double x, double y, double s, const wxColour& c);
    static void DrawSearch(wxGraphicsContext& gc, double x, double y, double s, const wxColour& c);
    static void DrawRegex(wxGraphicsContext& gc, double x, double y, double s, const wxColour& c);
    static void DrawCaseSensitive(wxGraphicsContext& gc, double x, double y, double s, const wxColour& c);
    static void DrawOverflow(wxGraphicsContext& gc, double x, double y, double s, const wxColour& c);
    // ... additional icon methods
};

} // namespace markamp::ui
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/PanelHeaderIcons.h` (create)
- `/Users/ryanrentfro/code/markamp/src/ui/PanelHeaderIcons.cpp` (create)

**Acceptance Criteria:**
- Every `PanelHeaderIconId` enum value has a corresponding drawing function
- Icons render at 16x16 default size, scalable to any size
- Disabled state renders with 40% opacity
- Toggled state uses accent color instead of default foreground
- Icons are visually consistent (same stroke width, same visual weight)
- All icons are drawn procedurally (no bitmap dependencies)

**Dependencies:** Task 1

---

### Task 4: Implement Explorer Panel Actions

**Title:** Add header actions to the file tree / explorer panel

**Description:** Wire the following actions into the `FileTreeCtrl` panel header: New File, New Folder, Collapse All, Refresh, Filter, Open Terminal Here.

**Implementation Details:**
```cpp
auto FileTreeCtrl::BuildHeaderActions() -> std::vector<PanelHeaderAction>
{
    return {
        {.icon = PanelHeaderIconId::kNewFile,
         .tooltip = "New File",
         .shortcut_hint = "Cmd+N",
         .callback = [this] { CreateNewFile(); }},
        {.icon = PanelHeaderIconId::kNewFolder,
         .tooltip = "New Folder",
         .callback = [this] { CreateNewFolder(); }},
        {.icon = PanelHeaderIconId::kCollapseAll,
         .tooltip = "Collapse All",
         .callback = [this] { CollapseAll(); }},
        {.icon = PanelHeaderIconId::kRefresh,
         .tooltip = "Refresh Explorer",
         .callback = [this] { RefreshTree(); }},
        {.icon = PanelHeaderIconId::kFilter,
         .tooltip = "Filter Files",
         .is_toggle = true,
         .is_toggled = [this] { return filter_active_; },
         .callback = [this] { ToggleFilter(); }},
        {.icon = PanelHeaderIconId::kTerminal,
         .tooltip = "Open Terminal Here",
         .callback = [this] { OpenTerminalHere(); }},
    };
}
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/FileTreeCtrl.h`
- `/Users/ryanrentfro/code/markamp/src/ui/FileTreeCtrl.cpp`

**Acceptance Criteria:**
- Six action buttons appear in the explorer panel header
- New File opens a name input at the selected location
- New Folder opens a folder name input
- Collapse All collapses the entire tree
- Refresh re-reads the file system
- Filter toggles a file name filter input
- Open Terminal Here opens a terminal in the selected folder's directory

**Dependencies:** Task 2, Task 3

---

### Task 5: Implement Search Panel Actions

**Title:** Add header actions to the search sidebar panel

**Description:** Wire actions: Clear All Results, Collapse Results, Toggle Regex, Toggle Case Sensitive, Toggle Whole Word, Open New Search Editor.

**Implementation Details:**
```cpp
auto SearchPanel::BuildHeaderActions() -> std::vector<PanelHeaderAction>
{
    return {
        {.icon = PanelHeaderIconId::kClear,
         .tooltip = "Clear Search Results",
         .callback = [this] { ClearResults(); },
         .is_enabled = [this] { return has_results(); }},
        {.icon = PanelHeaderIconId::kCollapseAll,
         .tooltip = "Collapse Results",
         .callback = [this] { CollapseAllResults(); }},
        {.icon = PanelHeaderIconId::kRegex,
         .tooltip = "Use Regular Expression",
         .is_toggle = true,
         .is_toggled = [this] { return regex_enabled_; },
         .callback = [this] { ToggleRegex(); }},
        {.icon = PanelHeaderIconId::kCaseSensitive,
         .tooltip = "Match Case",
         .is_toggle = true,
         .is_toggled = [this] { return case_sensitive_; },
         .callback = [this] { ToggleCaseSensitive(); }},
        {.icon = PanelHeaderIconId::kWholeWord,
         .tooltip = "Match Whole Word",
         .is_toggle = true,
         .is_toggled = [this] { return whole_word_; },
         .callback = [this] { ToggleWholeWord(); }},
        {.icon = PanelHeaderIconId::kOpenInEditor,
         .tooltip = "Open New Search Editor",
         .callback = [this] { OpenSearchEditor(); }},
    };
}
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/SearchPanel.h`
- `/Users/ryanrentfro/code/markamp/src/ui/SearchPanel.cpp`

**Acceptance Criteria:**
- Six action buttons in the search panel header
- Clear is disabled when there are no results
- Regex, Case, Whole Word are toggle buttons that reflect current search state
- Toggling search options immediately re-executes the search
- Open New Search Editor creates a new search tab

**Dependencies:** Task 2, Task 3

---

### Task 6: Implement Output Panel Actions

**Title:** Add header actions to the output panel

**Description:** Wire actions: Clear, Lock/Unlock Scroll, Word Wrap Toggle, Timestamps Toggle, Open in Editor.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/OutputPanel.h`
- `/Users/ryanrentfro/code/markamp/src/ui/OutputPanel.cpp`

**Acceptance Criteria:**
- Five action buttons in the output panel header (after the channel dropdown)
- Clear clears the active channel
- Lock/Unlock scroll icon changes based on current scroll lock state
- Word wrap toggle updates the display immediately
- Timestamps toggle shows/hides timestamps
- Open in Editor opens channel content in the editor

**Dependencies:** Task 2, Task 3

---

### Task 7: Implement Problems Panel Actions

**Title:** Add header actions to the problems panel

**Description:** Wire actions: Filter by Severity (toggle buttons), Collapse All, Expand All, Quick Fix All, Sort cycling.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/ProblemsPanel.h`
- `/Users/ryanrentfro/code/markamp/src/ui/ProblemsPanel.cpp`

**Acceptance Criteria:**
- Header shows severity filter toggles (Error, Warning, Info, Hint)
- Collapse All and Expand All buttons
- Quick Fix All button (wrench icon) applies all available fixes
- Sort cycling button changes sort mode on click

**Dependencies:** Task 2, Task 3

---

### Task 8: Implement Terminal Panel Actions

**Title:** Add header actions to the terminal panel

**Description:** Wire actions: New Terminal, Split Terminal, Kill Terminal, Scroll to Bottom, Clear.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/TerminalPanel.h`
- `/Users/ryanrentfro/code/markamp/src/ui/TerminalPanel.cpp`

**Acceptance Criteria:**
- Five action buttons in the terminal panel header
- New Terminal creates a new terminal session
- Split Terminal splits the current panel
- Kill Terminal sends SIGTERM to the active process
- Scroll to Bottom scrolls to the latest output
- Clear clears the terminal buffer and scrollback

**Dependencies:** Task 2, Task 3

---

### Task 9: Implement Build Panel Actions

**Title:** Add header actions to the build panel

**Description:** Wire actions: Build, Rebuild, Clean, Configure, Stop.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/BuildPanel.h`
- `/Users/ryanrentfro/code/markamp/src/ui/BuildPanel.cpp`

**Acceptance Criteria:**
- Five action buttons in the build panel header
- Build triggers the default build task
- Stop is only enabled during an active build
- Icons match the toolbar build controls

**Dependencies:** Task 2, Task 3

---

### Task 10: Implement Debug Console Actions

**Title:** Add header actions to the debug console panel

**Description:** Wire actions: Clear Console, Log Level Filter, Timestamps Toggle.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/DebugConsolePanel.h`
- `/Users/ryanrentfro/code/markamp/src/ui/DebugConsolePanel.cpp`

**Acceptance Criteria:**
- Three action buttons in the debug console header
- Clear removes all console output
- Log level filter cycles through min levels
- Timestamps toggle shows/hides entry timestamps

**Dependencies:** Task 2, Task 3

---

### Task 11: Implement Overflow Menu for Narrow Panels

**Title:** Overflow handling when panel header is too narrow for all actions

**Description:** When a panel is too narrow to display all action buttons, excess buttons are moved into an overflow menu (three dots "..." button at the right edge).

**Implementation Details:**
```cpp
void PanelHeaderBar::RecalculateButtonRects()
{
    int available_width = GetClientSize().GetWidth() - title_width_ - kTitlePaddingLeft;
    int total_needed = static_cast<int>(actions_.size()) * (kIconSize + kIconPadding * 2);

    if (total_needed > available_width)
    {
        int visible_count = available_width / (kIconSize + kIconPadding * 2) - 1; // -1 for overflow
        // First `visible_count` actions are visible, rest go to overflow
    }
}
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/PanelHeaderBar.cpp`

**Acceptance Criteria:**
- Overflow "..." button appears when actions do not fit
- Clicking "..." shows a popup menu with the hidden actions
- Menu items include icon, label, and shortcut hint
- Overflow recalculates on panel resize
- At minimum width, all actions except one go to overflow

**Dependencies:** Task 2

---

### Task 12: Implement Keyboard Shortcuts for Actions

**Title:** Register keyboard shortcuts for panel header actions

**Description:** Each panel header action with a `shortcut_hint` should have the corresponding keyboard shortcut registered, active only when that panel has focus.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/PanelHeaderBar.cpp`

**Acceptance Criteria:**
- Shortcuts are active when the parent panel has focus
- Shortcuts do not conflict across panels (each panel's shortcuts only work when that panel is focused)
- Shortcut hints appear in action tooltips
- Shortcuts are registered with the shortcut system (visible in keyboard shortcut editor)

**Dependencies:** Task 2

---

### Task 13: Implement Tooltip for Actions

**Title:** Show tooltip on hover over panel header action buttons

**Description:** Display a themed tooltip when hovering over an action button. The tooltip shows the action name and keyboard shortcut. Tooltip appears after a 500ms delay and uses the application theme.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/PanelHeaderBar.cpp`

**Acceptance Criteria:**
- Tooltip appears after 500ms hover delay
- Tooltip shows action name and shortcut (e.g., "Clear Terminal (Cmd+K)")
- Tooltip uses theme colors (bg_tooltip, fg_tooltip)
- Tooltip disappears on mouse leave
- Disabled actions show tooltip with "(disabled)" suffix

**Dependencies:** Task 2

---

### Task 14: Implement Action State Updates

**Title:** Dynamic enabled/toggled state refresh on panel state changes

**Description:** Panel header actions need their enabled and toggled states refreshed when the panel state changes (e.g., scroll lock changes, filter changes, build starts/stops). Implement an update mechanism that queries the callbacks and repaints affected buttons.

**Implementation Details:**
```cpp
void PanelHeaderBar::update_action_states()
{
    bool needs_repaint = false;
    for (auto& btn : buttons_)
    {
        auto& action = actions_[btn.action_index];
        bool was_enabled = action.is_enabled ? action.is_enabled() : true;
        bool was_toggled = action.is_toggled ? action.is_toggled() : false;
        // Check if states changed, set needs_repaint
    }
    if (needs_repaint) Refresh();
}
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/PanelHeaderBar.cpp`

**Acceptance Criteria:**
- States are queried before each paint
- Changed states trigger minimal repaint (only affected buttons)
- No performance impact for panels with static actions
- Toggle buttons visually update immediately when state changes

**Dependencies:** Task 2

---

### Task 15: Update CMakeLists.txt

**Title:** Add PanelHeaderBar and PanelHeaderIcons to the build

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/CMakeLists.txt`

**Acceptance Criteria:**
- `PanelHeaderBar.h/.cpp` and `PanelHeaderIcons.h/.cpp` in both `add_executable()` and `source_group()`
- Build succeeds

**Dependencies:** Task 2, Task 3

---

### Task 16: Write Unit Tests for PanelHeaderBar

**Title:** Test action rendering, overflow, and state management

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/tests/unit/test_panel_header_bar.cpp` (create)

**Acceptance Criteria:**
- Tests verify button count matches action count
- Tests verify overflow triggers at narrow widths
- Tests verify disabled actions do not fire callbacks
- Tests verify toggle state is queried correctly
- All tests pass

**Dependencies:** Task 2

---

### Task 17: Write Explorer Panel Header Integration Tests

**Title:** Test FileTreeCtrl header action wiring

**Description:** Verify that all explorer panel header actions (New File, New Folder, Collapse All, Refresh, Filter, Open Terminal Here) are correctly wired to their panel methods and that enabled/toggled states reflect actual panel state.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/tests/unit/test_panel_header_bar.cpp`

**Acceptance Criteria:**
- Tests verify New File action triggers CreateNewFile()
- Tests verify Collapse All collapses the tree
- Tests verify Filter toggle state reflects filter_active_
- All tests pass

**Dependencies:** Task 4

---

### Task 18: Write Search Panel Header Integration Tests

**Title:** Test SearchPanel header action wiring

**Description:** Verify all search panel header actions (Clear, Collapse, Regex, Case Sensitive, Whole Word, Open Search Editor) are correctly wired.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/tests/unit/test_panel_header_bar.cpp`

**Acceptance Criteria:**
- Tests verify Clear is disabled with no results
- Tests verify Regex toggle updates search state
- Tests verify Case toggle updates search state
- All tests pass

**Dependencies:** Task 5

---

### Task 19: Write Output Panel Header Integration Tests

**Title:** Test OutputPanel header action wiring

**Description:** Verify all output panel header actions (Clear, Lock Scroll, Word Wrap, Timestamps, Open in Editor) are correctly wired.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/tests/unit/test_panel_header_bar.cpp`

**Acceptance Criteria:**
- Tests verify Clear clears the active channel
- Tests verify Lock Scroll toggle reflects auto_scroll state
- Tests verify Word Wrap toggle updates display
- All tests pass

**Dependencies:** Task 6

---

### Task 20: Write Problems Panel Header Integration Tests

**Title:** Test ProblemsPanel header action wiring

**Description:** Verify severity filter toggles, collapse/expand, quick fix all, and sort cycling actions.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/tests/unit/test_panel_header_bar.cpp`

**Acceptance Criteria:**
- Tests verify severity toggles filter the tree
- Tests verify Collapse All collapses all file nodes
- Tests verify Sort cycling changes sort mode
- All tests pass

**Dependencies:** Task 7

---

### Task 21: Write Terminal Panel Header Integration Tests

**Title:** Test TerminalPanel header action wiring

**Description:** Verify terminal panel header actions (New Terminal, Split, Kill, Scroll to Bottom, Clear) are correctly wired.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/tests/unit/test_panel_header_bar.cpp`

**Acceptance Criteria:**
- Tests verify New Terminal creates a new session
- Tests verify Kill sends termination signal
- Tests verify Clear clears the buffer
- All tests pass

**Dependencies:** Task 8

---

### Task 22: Write Build Panel Header Integration Tests

**Title:** Test BuildPanel header action wiring

**Description:** Verify build panel header actions (Build, Rebuild, Clean, Configure, Stop) are correctly wired and that Stop is disabled when no build is active.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/tests/unit/test_panel_header_bar.cpp`

**Acceptance Criteria:**
- Tests verify Build triggers default build task
- Tests verify Stop is disabled when idle
- Tests verify Stop is enabled during active build
- All tests pass

**Dependencies:** Task 9

---

### Task 23: Write Debug Console Header Integration Tests

**Title:** Test DebugConsolePanel header action wiring

**Description:** Verify debug console header actions (Clear, Log Level Filter, Timestamps) are correctly wired.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/tests/unit/test_panel_header_bar.cpp`

**Acceptance Criteria:**
- Tests verify Clear removes all console entries
- Tests verify Log Level filter cycles through levels
- Tests verify Timestamps toggle updates display
- All tests pass

**Dependencies:** Task 10

---

### Task 24: Implement Panel Header Theme Consistency

**Title:** Ensure all panel headers use identical styling and spacing

**Description:** Audit all nine panel header implementations to verify they use the same background color token, title font, icon size, padding values, and hover effects. Fix any inconsistencies.

**Files Affected:**
- All panel files modified in Tasks 4-10

**Acceptance Criteria:**
- All headers use `bg_panel_header` background token
- All headers are exactly 28px tall
- All icons are 16x16 with 4px padding
- Title font is the same across all headers
- Hover highlight color is consistent
- Divider line between header and content is 1px using `border_subtle`

**Dependencies:** Tasks 4-10

---

### Task 25: Implement Accessibility for Panel Headers

**Title:** Ensure panel header actions are keyboard accessible and screen reader compatible

**Description:** Each panel header button must be reachable via Tab navigation, activatable via Enter/Space, and expose its tooltip text as an accessible name for screen readers.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/PanelHeaderBar.cpp`

**Acceptance Criteria:**
- Tab focuses the first action button in the header
- Left/Right arrows navigate between buttons
- Enter/Space activates the focused button
- Escape returns focus to the panel content
- wxAccessible name is set to the button tooltip text
- Focus ring is visible on the focused button

**Dependencies:** Task 2

---

### Task 26: Documentation

**Title:** Document the panel header action bar system

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/docs/architecture.md`

**Acceptance Criteria:**
- PanelHeaderAction data model documented
- PanelHeaderBar widget usage documented
- Icon library catalog with visual descriptions
- Per-panel action inventories listed

**Dependencies:** All previous tasks

## Testing Requirements

- Unit tests for PanelHeaderBar layout and overflow logic
- Integration tests for each panel's action wiring
- Manual testing of all actions in all panels
- Theme switching with header bars active
- Narrow-panel overflow testing

## Phase Completion Criteria

- Every sidebar and bottom panel has a themed header bar with contextual actions
- Action icons are visually consistent and use theme colors
- Toggle actions correctly reflect panel state
- Overflow menus work at narrow widths
- Keyboard shortcuts fire panel actions when focused
- All unit and integration tests pass
