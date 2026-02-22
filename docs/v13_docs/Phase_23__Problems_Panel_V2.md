# Phase 23: Problems Panel V2

## Overview

Evolve the existing `ProblemsPanel` from a flat list view into a fully-featured diagnostics panel with tree-view grouping by file, severity icons, multi-dimensional filtering, quick-fix integration, click-to-navigate, collapsible groups, sorting controls, and deep integration with the status bar and editor gutter. This phase builds on the existing `DiagnosticsService`, `CodeActionProvider`, and `ProblemItem` model.

## Prerequisites

- Phase 22 (Output Panel V2) -- shares LogLevel infrastructure and panel header patterns
- Phase 21 (Integrated Terminal Panel) -- shares bottom panel integration patterns
- Existing `ProblemsPanel` at `/Users/ryanrentfro/code/markamp/src/ui/ProblemsPanel.h`
- Existing `DiagnosticsService` at `/Users/ryanrentfro/code/markamp/src/core/DiagnosticsService.h`
- Existing `CodeActionProvider` at `/Users/ryanrentfro/code/markamp/src/core/CodeActionProvider.h`
- Existing `EditorPanel` diagnostic indicators at `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.h`

## Target Files

| Action | File |
|--------|------|
| Modify | `/Users/ryanrentfro/code/markamp/src/ui/ProblemsPanel.h` |
| Modify | `/Users/ryanrentfro/code/markamp/src/ui/ProblemsPanel.cpp` |
| Create | `/Users/ryanrentfro/code/markamp/src/ui/ProblemsTreeModel.h` |
| Create | `/Users/ryanrentfro/code/markamp/src/ui/ProblemsTreeModel.cpp` |
| Modify | `/Users/ryanrentfro/code/markamp/src/core/DiagnosticsService.h` |
| Modify | `/Users/ryanrentfro/code/markamp/src/core/DiagnosticsService.cpp` |
| Modify | `/Users/ryanrentfro/code/markamp/src/core/Events.h` |
| Modify | `/Users/ryanrentfro/code/markamp/src/ui/StatusBarPanel.h` |
| Modify | `/Users/ryanrentfro/code/markamp/src/ui/StatusBarPanel.cpp` |
| Modify | `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp` |
| Modify | `/Users/ryanrentfro/code/markamp/src/ui/FileTreeCtrl.cpp` |
| Modify | `/Users/ryanrentfro/code/markamp/src/CMakeLists.txt` |
| Create | `/Users/ryanrentfro/code/markamp/tests/unit/test_problems_panel_v2.cpp` |

## Tasks

### Task 1: Design Problems Tree Data Model

**Title:** Create ProblemsTreeModel with file-grouped, severity-sorted diagnostic tree

**Description:** Create a standalone testable model that transforms flat diagnostic data from `DiagnosticsService` into a two-level tree: file nodes at level 1, individual diagnostic items at level 2. The model supports filtering, sorting, and tracks collapse state per file node.

**Implementation Details:**
Create `/Users/ryanrentfro/code/markamp/src/ui/ProblemsTreeModel.h`:
```cpp
#pragma once
#include "core/DiagnosticsService.h"
#include <string>
#include <vector>

namespace markamp::ui
{

struct ProblemFileNode
{
    std::string file_uri;
    std::string display_name; // basename or relative path
    int error_count{0};
    int warning_count{0};
    int info_count{0};
    int hint_count{0};
    bool collapsed{false};
    std::vector<ProblemItem> items;
};

enum class ProblemSortMode : uint8_t
{
    kSeverity,  // Errors first, then warnings, info, hints
    kFile,      // Alphabetical by file path
    kPosition,  // By line number within each file
};

class ProblemsTreeModel
{
public:
    void set_diagnostics(const core::DiagnosticsService& service);
    void set_severity_filter(core::DiagnosticSeverity min_severity);
    void set_source_filter(const std::string& source);
    void set_sort_mode(ProblemSortMode mode);

    [[nodiscard]] auto file_nodes() const -> const std::vector<ProblemFileNode>&;
    [[nodiscard]] auto total_errors() const -> int;
    [[nodiscard]] auto total_warnings() const -> int;
    [[nodiscard]] auto total_info() const -> int;
    [[nodiscard]] auto total_items() const -> int;
    [[nodiscard]] auto visible_row_count() const -> int;
    [[nodiscard]] auto sources() const -> std::vector<std::string>;

    void toggle_collapse(const std::string& file_uri);
    void collapse_all();
    void expand_all();

private:
    std::vector<ProblemFileNode> nodes_;
    core::DiagnosticSeverity min_severity_{core::DiagnosticSeverity::kHint};
    std::string source_filter_;
    ProblemSortMode sort_mode_{ProblemSortMode::kSeverity};

    void rebuild();
    void apply_filters();
    void apply_sort();
};

} // namespace markamp::ui
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/ProblemsTreeModel.h` (create)
- `/Users/ryanrentfro/code/markamp/src/ui/ProblemsTreeModel.cpp` (create)

**Acceptance Criteria:**
- Diagnostics are grouped by file URI into `ProblemFileNode` entries
- Per-file severity counts are computed correctly
- Filtering by severity removes files that have no matching diagnostics
- Filtering by source (e.g., "markdownlint") shows only that source's diagnostics
- Sorting by severity puts error-files first, then warning-files
- Sorting by position arranges items by line number within each file
- `visible_row_count()` accounts for collapsed file nodes

**Dependencies:** None

---

### Task 2: Add Diagnostic Events to EventBus

**Title:** Declare events for diagnostic changes, navigation, and quick-fix triggers

**Description:** Add events that the problems panel publishes when the user navigates to a problem, requests a quick fix, or when diagnostic counts change.

**Implementation Details:**
Add to `/Users/ryanrentfro/code/markamp/src/core/Events.h`:
```cpp
MARKAMP_DECLARE_EVENT_WITH_FIELDS(DiagnosticsChangedEvent)
    int total_errors{0};
    int total_warnings{0};
    int total_info{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(NavigateToProblemEvent)
    std::string file_uri;
    int line{0};
    int character{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(QuickFixRequestedEvent)
    std::string file_uri;
    int line{0};
    std::string diagnostic_code;
MARKAMP_DECLARE_EVENT_END;
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/core/Events.h`

**Acceptance Criteria:**
- All three event types compile and are publishable
- `DiagnosticsChangedEvent` includes aggregate counts
- `NavigateToProblemEvent` includes full location info
- `QuickFixRequestedEvent` includes the diagnostic code for targeted fix lookup

**Dependencies:** None

---

### Task 3: Implement Severity Icon Rendering

**Title:** Draw severity icons (error=red circle, warning=yellow triangle, info=blue circle, hint=green lightbulb)

**Description:** Implement icon drawing functions for each diagnostic severity level using `wxGraphicsContext`. These icons are 14x14px and are drawn inline next to diagnostic messages in the tree view.

**Implementation Details:**
```cpp
namespace markamp::ui
{

void DrawErrorIcon(wxGraphicsContext& gc, double x, double y, double size, const core::Theme& theme);
void DrawWarningIcon(wxGraphicsContext& gc, double x, double y, double size, const core::Theme& theme);
void DrawInfoIcon(wxGraphicsContext& gc, double x, double y, double size, const core::Theme& theme);
void DrawHintIcon(wxGraphicsContext& gc, double x, double y, double size, const core::Theme& theme);
void DrawSeverityIcon(wxGraphicsContext& gc, core::DiagnosticSeverity severity,
                      double x, double y, double size, const core::Theme& theme);

} // namespace markamp::ui
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/ProblemsPanel.h`
- `/Users/ryanrentfro/code/markamp/src/ui/ProblemsPanel.cpp`

**Acceptance Criteria:**
- Error icon: filled red circle with white X
- Warning icon: filled yellow triangle with black exclamation mark
- Info icon: filled blue circle with white "i"
- Hint icon: filled green lightbulb outline
- Icons scale correctly at different DPI settings
- Colors are derived from theme tokens, not hardcoded

**Dependencies:** None

---

### Task 4: Implement Tree View Rendering

**Title:** Replace wxListCtrl with custom-drawn tree view for problems display

**Description:** Replace the flat `wxListCtrl* list_ctrl_` with a custom-drawn owner-draw tree view. File nodes render as expandable/collapsible rows with the file icon, file name, and severity summary. Child diagnostic items render with severity icon, message, line number, and source.

**Implementation Details:**
The panel becomes a custom-painted `ThemeAwareWindow` subclass (replacing the `wxPanel` base with `wxListCtrl` child). Each row is 22px tall. File nodes have a disclosure triangle. Diagnostic items are indented 20px.

```cpp
void ProblemsPanel::OnPaint(wxPaintEvent& event)
{
    wxAutoBufferedPaintDC dc(this);
    int y = -scroll_offset_;
    for (const auto& node : tree_model_.file_nodes())
    {
        DrawFileNode(dc, node, y);
        y += kRowHeight;
        if (!node.collapsed)
        {
            for (const auto& item : node.items)
            {
                DrawProblemItem(dc, item, y);
                y += kRowHeight;
            }
        }
    }
}
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/ProblemsPanel.h`
- `/Users/ryanrentfro/code/markamp/src/ui/ProblemsPanel.cpp`

**Acceptance Criteria:**
- File nodes show: disclosure triangle, file icon, file name, severity counts
- Diagnostic items show: severity icon, message text, line:col, source tag
- Rows are 22px tall with 4px padding
- File nodes are clickable to expand/collapse
- Scrollbar reflects total visible row count
- Theme colors are applied to all elements
- Empty state shows "No problems detected" centered message

**Dependencies:** Task 1, Task 3

---

### Task 5: Implement Severity Filter Toggle Buttons

**Title:** Add four severity toggle buttons in the panel header

**Description:** Add toggle buttons for Error, Warning, Info, and Hint in the problems panel header bar. Each button shows the count of diagnostics at that severity level and can be toggled to show/hide that severity. The buttons use the same visual pattern as Output Panel V2 filter buttons.

**Implementation Details:**
```cpp
struct SeverityFilterButton
{
    core::DiagnosticSeverity severity;
    wxRect rect;
    bool is_active{true};
    int count{0};
};
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/ProblemsPanel.h`
- `/Users/ryanrentfro/code/markamp/src/ui/ProblemsPanel.cpp`

**Acceptance Criteria:**
- Four toggle buttons in panel header: Errors (red), Warnings (yellow), Info (blue), Hints (green)
- Each shows the count of diagnostics at that level
- Toggling a button immediately updates the tree view
- Multiple levels can be active simultaneously
- Default: all four levels are shown
- Count updates in real-time as diagnostics change

**Dependencies:** Task 1, Task 4

---

### Task 6: Implement Source Filter Dropdown

**Title:** Add a dropdown to filter diagnostics by source (e.g., "markdownlint", "mermaid")

**Description:** Add a dropdown in the panel header that lists all diagnostic sources currently present. Selecting a source filters the tree to show only diagnostics from that source. "All Sources" shows everything.

**Implementation Details:**
```cpp
auto ProblemsTreeModel::sources() const -> std::vector<std::string>
{
    std::set<std::string> unique_sources;
    for (const auto& node : nodes_)
    {
        for (const auto& item : node.items)
        {
            if (!item.source.empty())
            {
                unique_sources.insert(item.source);
            }
        }
    }
    return {unique_sources.begin(), unique_sources.end()};
}
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/ProblemsPanel.h`
- `/Users/ryanrentfro/code/markamp/src/ui/ProblemsPanel.cpp`
- `/Users/ryanrentfro/code/markamp/src/ui/ProblemsTreeModel.cpp`

**Acceptance Criteria:**
- Dropdown lists all unique diagnostic sources plus "All Sources"
- Selecting a source filters the tree immediately
- Source list updates when diagnostics change
- Filter persists until explicitly cleared
- "All Sources" clears the source filter

**Dependencies:** Task 1

---

### Task 7: Implement Quick Fix on Hover

**Title:** Show quick-fix lightbulb icon on hover with click-to-apply

**Description:** When the user hovers over a diagnostic item that has an available quick fix (from `CodeActionProvider`), show a lightbulb icon on the right side of the row. Clicking the lightbulb opens a popup menu with available fixes. Single fixes can also be applied with Cmd+. when the item is selected.

**Implementation Details:**
```cpp
void ProblemsPanel::OnMouseMove(wxMouseEvent& event)
{
    auto hit = HitTestRow(event.GetPosition());
    if (hit.is_diagnostic_item)
    {
        auto actions = code_action_provider_->provide_actions(
            /* content */, hit.item.line, 0, hit.item.line, 999, hit.item.file_uri);
        if (!actions.empty())
        {
            hovered_quickfix_row_ = hit.row_index;
            Refresh();
        }
    }
}
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/ProblemsPanel.h`
- `/Users/ryanrentfro/code/markamp/src/ui/ProblemsPanel.cpp`

**Acceptance Criteria:**
- Lightbulb icon appears on hover for items with available quick fixes
- Clicking lightbulb shows popup menu of available code actions
- Selecting a code action applies the fix and refreshes diagnostics
- Cmd+. shortcut applies the preferred (first) code action
- Lightbulb disappears when mouse leaves the row
- Code actions are fetched from the existing `CodeActionProvider`

**Dependencies:** Task 4

---

### Task 8: Implement Navigate to Problem Location

**Title:** Double-click or Enter navigates to the problem's file and line

**Description:** When the user double-clicks a diagnostic item or presses Enter on a selected item, navigate the editor to the file and line/column of the diagnostic. If the file is not open, open it first.

**Implementation Details:**
```cpp
void ProblemsPanel::NavigateToProblem(const ProblemItem& item)
{
    event_bus_.publish(core::events::NavigateToProblemEvent{
        .file_uri = item.file_uri,
        .line = item.line,
        .character = item.character
    });
}
```
The `LayoutManager` subscribes to `NavigateToProblemEvent` and opens the file, then sets the cursor position.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/ProblemsPanel.cpp`
- `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`

**Acceptance Criteria:**
- Double-click on a diagnostic item opens the file and navigates to the line
- Enter key on the selected item does the same
- If the file is already open, the existing tab is activated
- Cursor is placed at the exact line and character position
- The editor scrolls to make the line visible with context

**Dependencies:** Task 4

---

### Task 9: Implement Problem Count in Status Bar

**Title:** Show error and warning counts in the status bar with click-to-open

**Description:** Display the total error and warning counts in the status bar. The error count is shown with a red icon and the warning count with a yellow icon. Clicking the status bar item opens or focuses the problems panel.

**Implementation Details:**
Subscribe to `DiagnosticsChangedEvent` in `StatusBarPanel`:
```cpp
diag_sub_ = event_bus_.subscribe<core::events::DiagnosticsChangedEvent>(
    [this](const auto& e)
    {
        set_problem_counts(e.total_errors, e.total_warnings);
        RebuildItems();
        Refresh();
    });
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/StatusBarPanel.h`
- `/Users/ryanrentfro/code/markamp/src/ui/StatusBarPanel.cpp`

**Acceptance Criteria:**
- Status bar shows error icon + count and warning icon + count
- Zero counts are shown as "0" (not hidden)
- Error count is red, warning count is yellow
- Clicking the item opens/toggles the problems panel
- Counts update in real-time as diagnostics change
- Tooltip shows "X errors, Y warnings" full text

**Dependencies:** Task 2

---

### Task 10: Implement File Decorations for Problem Files

**Title:** Show error/warning indicators on files in the file tree

**Description:** Files that have diagnostics should show a severity decoration in the `FileTreeCtrl`. Files with errors get a red dot, files with only warnings get a yellow dot. The decoration updates in real-time as diagnostics change.

**Implementation Details:**
Subscribe to `DiagnosticsChangedEvent` in `FileTreeCtrl`:
```cpp
void FileTreeCtrl::UpdateDiagnosticDecorations(const core::DiagnosticsService& service)
{
    for (const auto& uri : service.uris())
    {
        auto severity = highest_severity(service.get(uri));
        SetItemDecoration(uri, severity);
    }
}
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/FileTreeCtrl.h`
- `/Users/ryanrentfro/code/markamp/src/ui/FileTreeCtrl.cpp`

**Acceptance Criteria:**
- Files with errors show a red dot decoration in the file tree
- Files with warnings (no errors) show a yellow dot
- Files with only info/hints show no decoration (to reduce noise)
- Decorations update when diagnostics change
- Decorations are removed when diagnostics for a file are cleared
- Decoration is drawn after the file name text, before the right edge

**Dependencies:** Task 2

---

### Task 11: Implement Collapse/Expand All Controls

**Title:** Add collapse-all and expand-all buttons in the panel header

**Description:** Add two icon buttons in the problems panel header: collapse-all (double-up chevron) and expand-all (double-down chevron). These affect all file nodes in the tree simultaneously.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/ProblemsPanel.h`
- `/Users/ryanrentfro/code/markamp/src/ui/ProblemsPanel.cpp`

**Acceptance Criteria:**
- Collapse-all button collapses all file nodes, showing only file-level rows
- Expand-all button expands all file nodes, showing all diagnostic items
- Buttons are in the panel header action bar area
- Keyboard shortcut: Cmd+Shift+[ to collapse all, Cmd+Shift+] to expand all
- Buttons use theme-aware icon rendering

**Dependencies:** Task 4

---

### Task 12: Implement Sort Controls

**Title:** Add sort-mode cycling button (severity/file/position)

**Description:** Add a sort button in the panel header that cycles through sort modes: sort by severity (errors first), sort by file name (alphabetical), sort by position (line number). The current sort mode is indicated by the button icon.

**Implementation Details:**
```cpp
void ProblemsPanel::CycleSortMode()
{
    auto current = tree_model_.sort_mode();
    auto next = static_cast<ProblemSortMode>((static_cast<int>(current) + 1) % 3);
    tree_model_.set_sort_mode(next);
    Refresh();
}
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/ProblemsPanel.h`
- `/Users/ryanrentfro/code/markamp/src/ui/ProblemsPanel.cpp`

**Acceptance Criteria:**
- Sort button cycles: Severity -> File -> Position -> Severity
- Severity sort: files with errors first, then warnings, then info
- File sort: alphabetical by file path
- Position sort: by line number (lowest first) within each file
- Sort mode is indicated by button tooltip
- Sort applies immediately on click

**Dependencies:** Task 1

---

### Task 13: Implement Multiline Problem Descriptions

**Title:** Support multiline diagnostic messages with expand/collapse

**Description:** Some diagnostics have long messages or related information that spans multiple lines. Display the first line in the tree row, and show a "..." expander that reveals the full message on click. Related diagnostic information (e.g., "defined here" cross-references) is shown as indented sub-items.

**Implementation Details:**
```cpp
struct ProblemItem
{
    // ... existing fields ...
    std::vector<std::string> related_messages; // "Defined at file.md:10"
    bool is_expanded{false};
};
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/ProblemsPanel.h`
- `/Users/ryanrentfro/code/markamp/src/ui/ProblemsPanel.cpp`
- `/Users/ryanrentfro/code/markamp/src/ui/ProblemsTreeModel.h`

**Acceptance Criteria:**
- Long messages truncate with "..." in the tree row
- Clicking "..." or pressing Right arrow expands to show full message
- Related information is shown as sub-items with a link icon
- Clicking a related item navigates to that location
- Expanded state is preserved during tree rebuilds (identified by diagnostic ID)

**Dependencies:** Task 4

---

### Task 14: Implement Related Information Links

**Title:** Render clickable related diagnostic locations

**Description:** Diagnostics can have related information pointing to other file locations (e.g., "declared here", "imported from"). Display these as clickable links below the diagnostic message, each with a file:line reference that navigates when clicked.

**Implementation Details:**
Extend `Diagnostic` to include related information:
```cpp
struct DiagnosticRelatedInfo
{
    std::string message;
    std::string uri;
    DiagnosticPosition position;
};

struct Diagnostic
{
    // ... existing fields ...
    std::vector<DiagnosticRelatedInfo> related_information;
};
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/core/DiagnosticsService.h`
- `/Users/ryanrentfro/code/markamp/src/ui/ProblemsPanel.cpp`

**Acceptance Criteria:**
- Related information is stored in the `Diagnostic` struct
- Related info renders as clickable links in the problems panel
- Clicking a related info link navigates to that file:line
- Related info is indented under the parent diagnostic
- Link text shows "filename:line - message" format

**Dependencies:** Task 8, Task 13

---

### Task 15: Implement Autofix All Button

**Title:** Apply all available quick fixes in the workspace with one click

**Description:** Add an "Autofix All" button in the problems panel header that iterates through all diagnostics, finds available quick fixes, and applies them in batch. Show a progress indicator and summary of applied fixes.

**Implementation Details:**
```cpp
void ProblemsPanel::AutofixAll()
{
    int fixed = 0;
    for (const auto& node : tree_model_.file_nodes())
    {
        for (const auto& item : node.items)
        {
            auto actions = code_action_provider_->provide_actions(
                /* content */, item.line, 0, item.line, 999, item.file_uri);
            for (const auto& action : actions)
            {
                if (action.is_preferred)
                {
                    // Apply action.edits
                    ++fixed;
                    break;
                }
            }
        }
    }
    // Show toast: "Applied N fixes"
}
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/ProblemsPanel.h`
- `/Users/ryanrentfro/code/markamp/src/ui/ProblemsPanel.cpp`

**Acceptance Criteria:**
- "Autofix All" button in panel header (wrench icon)
- Only applies "preferred" code actions (one per diagnostic)
- Shows progress during batch application
- Summary toast shows "Applied N fixes" after completion
- If no fixes are available, shows "No autofixes available"
- Confirmation dialog if more than 10 fixes would be applied

**Dependencies:** Task 7

---

### Task 16: Implement Keyboard Navigation

**Title:** Full keyboard navigation in the problems tree

**Description:** Support arrow key navigation, Enter to navigate to problem, Space to toggle collapse, and Tab/Shift+Tab to move between filter controls and the tree.

**Implementation Details:**
```cpp
void ProblemsPanel::OnKeyDown(wxKeyEvent& event)
{
    switch (event.GetKeyCode())
    {
        case WXK_UP: SelectPreviousRow(); break;
        case WXK_DOWN: SelectNextRow(); break;
        case WXK_LEFT: CollapseOrParent(); break;
        case WXK_RIGHT: ExpandOrChild(); break;
        case WXK_RETURN: NavigateToSelected(); break;
        case WXK_SPACE: ToggleSelectedCollapse(); break;
    }
}
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/ProblemsPanel.cpp`

**Acceptance Criteria:**
- Up/Down arrows move selection between visible rows
- Left arrow collapses a file node or moves to parent
- Right arrow expands a file node or moves to first child
- Enter navigates to the selected diagnostic's file location
- Space toggles collapse on file nodes
- Home/End jump to first/last row
- Selected row has visible focus ring

**Dependencies:** Task 4

---

### Task 17: Implement DiagnosticsService Event Publishing

**Title:** Wire DiagnosticsService to publish DiagnosticsChangedEvent

**Description:** Modify `DiagnosticsService` to accept an `EventBus` reference and publish `DiagnosticsChangedEvent` whenever diagnostics are added, removed, or cleared. This drives all downstream updates (status bar, file tree decorations, problems panel).

**Implementation Details:**
```cpp
class DiagnosticsService
{
public:
    DiagnosticsService() = default;
    explicit DiagnosticsService(EventBus& event_bus);
    // ... existing API ...
private:
    EventBus* event_bus_{nullptr};
    void publish_change();
};
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/core/DiagnosticsService.h`
- `/Users/ryanrentfro/code/markamp/src/core/DiagnosticsService.cpp`
- `/Users/ryanrentfro/code/markamp/src/app/MarkAmpApp.cpp`

**Acceptance Criteria:**
- `DiagnosticsChangedEvent` is published on `set()`, `remove()`, and `clear()`
- Event includes aggregate error, warning, and info counts
- EventBus is optional (null-checked) for backward compatibility
- Existing tests continue to pass with the no-arg constructor
- Event is published after the change is committed (not before)

**Dependencies:** Task 2

---

### Task 18: Implement Problems Panel Context Menu

**Title:** Right-click context menu for problems panel items

**Description:** Context menu with actions: Navigate to Problem, Copy Message, Copy Diagnostic Code, Quick Fix, Go to Related, Filter by Source, Collapse All, Expand All.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/ProblemsPanel.cpp`

**Acceptance Criteria:**
- Right-click on a diagnostic shows context menu
- "Navigate to Problem" opens the file at the diagnostic location
- "Copy Message" copies the diagnostic message to clipboard
- "Copy Diagnostic Code" copies the error code (e.g., "MD001")
- "Quick Fix" submenu shows available code actions
- "Filter by Source" filters the tree to this diagnostic's source
- Context menu items show keyboard shortcut hints

**Dependencies:** Task 7, Task 8

---

### Task 19: Implement Search/Filter in Problems Panel

**Title:** Add a text search/filter for diagnostic messages

**Description:** Add a search input in the panel header that filters the problems tree to show only diagnostics whose message, file path, or source matches the search query. This is a quick-filter, not a separate find dialog.

**Implementation Details:**
```cpp
void ProblemsTreeModel::set_text_filter(const std::string& query)
{
    text_filter_ = query;
    rebuild();
}
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/ProblemsPanel.h`
- `/Users/ryanrentfro/code/markamp/src/ui/ProblemsPanel.cpp`
- `/Users/ryanrentfro/code/markamp/src/ui/ProblemsTreeModel.h`
- `/Users/ryanrentfro/code/markamp/src/ui/ProblemsTreeModel.cpp`

**Acceptance Criteria:**
- Search input in the panel header (magnifying glass icon toggles it)
- Typing filters diagnostics in real-time (debounced 150ms)
- Matches against message, file path, source, and error code
- Case-insensitive search
- Empty query shows all diagnostics
- Match count displayed next to search input

**Dependencies:** Task 1

---

### Task 20: Implement ProblemsPanel as ThemeAwareWindow

**Title:** Migrate ProblemsPanel from wxPanel to ThemeAwareWindow base

**Description:** Change the ProblemsPanel base class from `wxPanel` to `ThemeAwareWindow` for consistent theme integration. This replaces the manual `ApplyTheme()` method with the automatic `OnThemeChanged()` callback.

**Implementation Details:**
```cpp
class ProblemsPanel : public ThemeAwareWindow
{
public:
    ProblemsPanel();  // Data-only constructor for tests
    ProblemsPanel(wxWindow* parent, core::ThemeEngine& theme_engine,
                  core::EventBus& event_bus, core::DiagnosticsService& service);
protected:
    void OnThemeChanged(const core::Theme& new_theme) override;
};
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/ProblemsPanel.h`
- `/Users/ryanrentfro/code/markamp/src/ui/ProblemsPanel.cpp`

**Acceptance Criteria:**
- ProblemsPanel inherits from `ThemeAwareWindow` instead of `wxPanel`
- `OnThemeChanged()` replaces `ApplyTheme()`
- Constructor accepts `ThemeEngine&` and `EventBus&`
- Data-only constructor still works for unit tests
- Theme changes update all panel colors immediately

**Dependencies:** None

---

### Task 21: Implement Editor Gutter Integration

**Title:** Show problem severity indicators in the editor gutter

**Description:** Wire the problems panel's diagnostic data to the editor's gutter, showing severity-colored markers (dots or icons) next to lines that have diagnostics. Clicking a gutter marker shows the diagnostic inline.

**Implementation Details:**
Subscribe to `DiagnosticsChangedEvent` in `EditorPanel` and update the `diagnostic_indicators_` vector. Use Scintilla markers for gutter display.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp`

**Acceptance Criteria:**
- Error lines show red marker in editor gutter
- Warning lines show yellow marker in editor gutter
- Clicking gutter marker shows diagnostic message inline (peek problem)
- Markers update when diagnostics change
- Markers are cleared when the file's diagnostics are removed

**Dependencies:** Task 17

---

### Task 22: Update CMakeLists.txt

**Title:** Add ProblemsTreeModel source files to the build

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/CMakeLists.txt`

**Acceptance Criteria:**
- `ProblemsTreeModel.h` and `.cpp` are in both `add_executable()` and `source_group()`
- Build succeeds with no linker errors

**Dependencies:** Task 1

---

### Task 23: Write Unit Tests for ProblemsTreeModel

**Title:** Comprehensive tests for tree model filtering, sorting, and grouping

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/tests/unit/test_problems_panel_v2.cpp` (create)

**Acceptance Criteria:**
- Tests verify file grouping from flat diagnostic list
- Tests verify severity filtering hides/shows correct items
- Tests verify source filtering
- Tests verify all three sort modes produce correct ordering
- Tests verify collapse/expand state tracking
- Tests verify per-file severity counts
- All tests pass

**Dependencies:** Task 1

---

### Task 24: Write Tests for Problem Navigation

**Title:** Test NavigateToProblemEvent publishing on double-click

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/tests/unit/test_problems_panel_v2.cpp`

**Acceptance Criteria:**
- Tests verify `NavigateToProblemEvent` is published with correct file and position
- Tests verify `QuickFixRequestedEvent` is published with correct diagnostic code
- Tests verify keyboard navigation selects correct rows
- All tests pass

**Dependencies:** Task 8

---

### Task 25: Write Tests for Diagnostic Event Publishing

**Title:** Test DiagnosticsService publishes change events correctly

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/tests/unit/test_problems_panel_v2.cpp`

**Acceptance Criteria:**
- Tests verify `DiagnosticsChangedEvent` is published on `set()`
- Tests verify `DiagnosticsChangedEvent` is published on `remove()`
- Tests verify `DiagnosticsChangedEvent` is published on `clear()`
- Tests verify event includes correct aggregate counts
- Tests verify no event published when no-arg constructor is used
- All tests pass

**Dependencies:** Task 17

---

### Task 26: Implement Batch Diagnostic Operations

**Title:** Efficient batch update support for DiagnosticsService

**Description:** Add a batch update method that sets diagnostics for multiple URIs atomically with a single change event. This prevents cascading UI updates during a full-file lint pass.

**Implementation Details:**
```cpp
void DiagnosticsService::set_batch(
    const std::vector<std::pair<std::string, std::vector<Diagnostic>>>& batch)
{
    for (const auto& [uri, diags] : batch)
    {
        diagnostics_[uri] = diags;
    }
    publish_change(); // Single event for the entire batch
}
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/core/DiagnosticsService.h`
- `/Users/ryanrentfro/code/markamp/src/core/DiagnosticsService.cpp`

**Acceptance Criteria:**
- `set_batch()` updates multiple URIs atomically
- Only one `DiagnosticsChangedEvent` is published for the entire batch
- Batch update is at least 5x faster than individual `set()` calls for 50+ URIs
- Existing `set()` API continues to work unchanged

**Dependencies:** Task 17

---

### Task 27: Documentation

**Title:** Document Problems Panel V2 architecture and integration points

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/docs/architecture.md`

**Acceptance Criteria:**
- Tree model design is documented
- Filter/sort system is described
- Quick-fix integration flow is documented
- Status bar and file tree decoration integration is described

**Dependencies:** All previous tasks

## Testing Requirements

- Unit tests for ProblemsTreeModel grouping, filtering, and sorting
- Unit tests for DiagnosticsService event publishing
- Integration tests for navigate-to-problem flow
- Performance test: 1,000 diagnostics across 100 files renders in under 50ms
- Manual testing with real markdown lint diagnostics

## Phase Completion Criteria

- Problems panel displays a tree view grouped by file with severity icons
- Filtering by severity and source works with toggle buttons and dropdown
- Quick-fix lightbulb appears on hover for fixable diagnostics
- Double-click navigates to the problem location in the editor
- Status bar shows live error/warning counts
- File tree shows problem decorations
- Collapse/expand, sort, and search all function correctly
- All unit and integration tests pass
