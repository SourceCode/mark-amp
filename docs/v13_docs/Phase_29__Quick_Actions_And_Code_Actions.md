# Phase 29: Quick Actions And Code Actions

## Overview

Implement a complete code actions system with gutter lightbulb indicators, quick-fix menus on click, Cmd+. keyboard shortcut for auto-fix, code action categories (Quick Fix, Refactor, Source Action), code action preview, preferred code actions, and integration with the existing `CodeActionProvider`. This phase brings the editor's inline diagnostic remediation to parity with VS Code's code action system.

## Prerequisites

- Phase 23 (Problems Panel V2) -- diagnostic navigation and quick-fix integration
- Phase 28 (Context Menu System V2) -- themed menu system for code action menus
- Existing `CodeActionProvider` at `/Users/ryanrentfro/code/markamp/src/core/CodeActionProvider.h`
- Existing `CodeIntelligenceTypes.h` with `CodeActionInfo` struct
- Existing `EditorPanel` with `ShowQuickFixLightbulb()` and `PeekProblem()` methods
- Existing `DiagnosticsService` and `DiagnosticsErrorHelp`

## Target Files

| Action | File |
|--------|------|
| Modify | `/Users/ryanrentfro/code/markamp/src/core/CodeActionProvider.h` |
| Modify | `/Users/ryanrentfro/code/markamp/src/core/CodeActionProvider.cpp` |
| Create | `/Users/ryanrentfro/code/markamp/src/core/CodeActionTypes.h` |
| Create | `/Users/ryanrentfro/code/markamp/src/ui/CodeActionMenu.h` |
| Create | `/Users/ryanrentfro/code/markamp/src/ui/CodeActionMenu.cpp` |
| Create | `/Users/ryanrentfro/code/markamp/src/ui/LightbulbWidget.h` |
| Create | `/Users/ryanrentfro/code/markamp/src/ui/LightbulbWidget.cpp` |
| Create | `/Users/ryanrentfro/code/markamp/src/ui/CodeActionPreview.h` |
| Create | `/Users/ryanrentfro/code/markamp/src/ui/CodeActionPreview.cpp` |
| Modify | `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.h` |
| Modify | `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp` |
| Modify | `/Users/ryanrentfro/code/markamp/src/core/Events.h` |
| Modify | `/Users/ryanrentfro/code/markamp/src/core/CodeIntelligenceTypes.h` |
| Modify | `/Users/ryanrentfro/code/markamp/src/CMakeLists.txt` |
| Create | `/Users/ryanrentfro/code/markamp/tests/unit/test_code_actions_v2.cpp` |

## Tasks

### Task 1: Define Code Action Categories and Types

**Title:** Create comprehensive code action type definitions

**Description:** Define the full taxonomy of code actions: Quick Fix (diagnostic fix), Refactor (restructure code), Source Action (organize, format), and their display/execution metadata.

**Implementation Details:**
Create `/Users/ryanrentfro/code/markamp/src/core/CodeActionTypes.h`:
```cpp
#pragma once
#include <string>
#include <vector>
#include <optional>

namespace markamp::core
{

enum class CodeActionKind : uint8_t
{
    kQuickFix,     // Fix a diagnostic problem
    kRefactor,     // Restructure code
    kSourceAction, // Organize, format, etc.
};

struct TextEdit
{
    int start_line{0};
    int start_char{0};
    int end_line{0};
    int end_char{0};
    std::string new_text;
};

struct CodeAction
{
    std::string title;
    CodeActionKind kind{CodeActionKind::kQuickFix};
    bool is_preferred{false}; // Auto-apply candidate
    bool is_disabled{false};
    std::string disabled_reason;
    std::string diagnostic_code;       // Source diagnostic
    std::string diagnostic_message;    // For context display
    std::vector<TextEdit> edits;       // Changes to apply
    std::string command;               // Optional command to execute after edits

    // For preview
    [[nodiscard]] auto preview_text(const std::string& original) const -> std::string;
};

struct CodeActionSet
{
    std::vector<CodeAction> quick_fixes;
    std::vector<CodeAction> refactors;
    std::vector<CodeAction> source_actions;
    [[nodiscard]] auto total_count() const -> int;
    [[nodiscard]] auto has_preferred() const -> bool;
    [[nodiscard]] auto preferred_action() const -> const CodeAction*;
};

} // namespace markamp::core
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/core/CodeActionTypes.h` (create)

**Acceptance Criteria:**
- Three code action kinds: QuickFix, Refactor, SourceAction
- `CodeAction` includes edits, preferred flag, and optional command
- `CodeActionSet` groups actions by category
- `preview_text()` generates a diff preview of what the action would change
- `is_preferred` marks the auto-apply candidate (at most one per diagnostic)
- `is_disabled` with `disabled_reason` for context-dependent unavailability

**Dependencies:** None

---

### Task 2: Extend CodeActionProvider

**Title:** Evolve CodeActionProvider to return categorized CodeActionSet results

**Description:** Update the existing `CodeActionProvider` to return `CodeActionSet` instead of the raw `CodeActionInfo` vector. Add new action providers for: organize imports (reference link cleanup), format document, and source-level actions.

**Implementation Details:**
Modify `/Users/ryanrentfro/code/markamp/src/core/CodeActionProvider.h`:
```cpp
class CodeActionProvider
{
public:
    explicit CodeActionProvider(DiagnosticsService& diagnostics_service);

    // New: Returns categorized action set
    [[nodiscard]] auto provide_action_set(const std::string& content,
                                           int start_line, int start_char,
                                           int end_line, int end_char,
                                           const std::string& document_uri = {}) const
        -> CodeActionSet;

    // Legacy: Still returns flat list for backward compatibility
    [[nodiscard]] auto provide_actions(/* existing signature */) const
        -> std::vector<CodeActionInfo>;

    // Register extension action providers
    using ActionProvider = std::function<std::vector<CodeAction>(
        const std::string& content, int line, const std::string& uri)>;
    void register_provider(CodeActionKind kind, ActionProvider provider);

private:
    // ... existing + new methods ...
    std::vector<std::pair<CodeActionKind, ActionProvider>> extension_providers_;

    // New built-in actions
    [[nodiscard]] static auto get_organize_links_action(const std::string& content)
        -> std::vector<CodeAction>;
    [[nodiscard]] static auto get_format_document_action(const std::string& content)
        -> std::vector<CodeAction>;
    [[nodiscard]] static auto get_remove_unused_references_action(const std::string& content)
        -> std::vector<CodeAction>;
};
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/core/CodeActionProvider.h`
- `/Users/ryanrentfro/code/markamp/src/core/CodeActionProvider.cpp`

**Acceptance Criteria:**
- `provide_action_set()` returns actions grouped by category
- Extension providers can be registered for each action kind
- Built-in actions include: fix heading hierarchy, convert link type, organize reference links, remove trailing whitespace, format document
- Legacy `provide_actions()` continues to work
- Preferred action is automatically determined (diagnostic quick-fix is preferred)

**Dependencies:** Task 1

---

### Task 3: Implement Lightbulb Gutter Widget

**Title:** Render a lightbulb icon in the editor gutter for lines with code actions

**Description:** Create a `LightbulbWidget` that renders a yellow lightbulb icon in the editor gutter margin next to lines that have available code actions. The lightbulb appears when the cursor is on a line with diagnostics or when hovering over a line with available actions.

**Implementation Details:**
Create `/Users/ryanrentfro/code/markamp/src/ui/LightbulbWidget.h`:
```cpp
#pragma once
#include "core/CodeActionTypes.h"
#include "core/ThemeEngine.h"
#include <wx/dc.h>

namespace markamp::ui
{

class LightbulbWidget
{
public:
    explicit LightbulbWidget(core::ThemeEngine& theme_engine);

    void set_visible(bool visible, int line = -1);
    void set_action_kind(core::CodeActionKind kind);

    [[nodiscard]] auto is_visible() const -> bool;
    [[nodiscard]] auto line() const -> int;

    void paint(wxDC& dc, int x, int y, int size);

    static constexpr int kSize = 14;

private:
    core::ThemeEngine& theme_engine_;
    bool visible_{false};
    int line_{-1};
    core::CodeActionKind kind_{core::CodeActionKind::kQuickFix};
};

} // namespace markamp::ui
```

The lightbulb icon changes based on action kind:
- Quick Fix: yellow lightbulb (classic)
- Refactor: blue lightbulb with wrench
- Source Action: lightbulb outline (not filled)

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/LightbulbWidget.h` (create)
- `/Users/ryanrentfro/code/markamp/src/ui/LightbulbWidget.cpp` (create)

**Acceptance Criteria:**
- Lightbulb renders in the gutter margin at the correct line
- Yellow for quick fixes (diagnostic-triggered)
- Blue for refactoring actions
- Outline for source actions
- Lightbulb appears when cursor moves to a line with actions
- Lightbulb disappears when cursor leaves the line
- Size is 14x14px, matching gutter icon size

**Dependencies:** Task 1

---

### Task 4: Implement Code Action Menu

**Title:** Themed popup menu for selecting from available code actions

**Description:** Create a code action menu that displays available actions grouped by category, with action descriptions, diagnostic context, and preferred action indication. The menu opens when clicking the lightbulb or pressing Cmd+.

**Implementation Details:**
Create `/Users/ryanrentfro/code/markamp/src/ui/CodeActionMenu.h`:
```cpp
#pragma once
#include "core/CodeActionTypes.h"
#include "core/ThemeEngine.h"
#include "ThemedContextMenu.h"
#include <functional>

namespace markamp::ui
{

class CodeActionMenu
{
public:
    using ActionSelectedCallback = std::function<void(const core::CodeAction& action)>;

    CodeActionMenu(wxWindow* parent,
                   core::ThemeEngine& theme_engine,
                   const core::CodeActionSet& actions,
                   ActionSelectedCallback callback);

    void ShowAt(const wxPoint& position);

private:
    wxWindow* parent_;
    core::ThemeEngine& theme_engine_;
    core::CodeActionSet actions_;
    ActionSelectedCallback callback_;

    [[nodiscard]] auto BuildMenu() -> ContextMenuModel;
};

} // namespace markamp::ui
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/CodeActionMenu.h` (create)
- `/Users/ryanrentfro/code/markamp/src/ui/CodeActionMenu.cpp` (create)

**Acceptance Criteria:**
- Menu groups actions by category: Quick Fix, Refactor, Source Action
- Category headers are shown as non-clickable labels
- Preferred action is shown at the top with a star indicator
- Each action shows its title and, for quick fixes, the diagnostic message
- Clicking an action triggers the callback with the selected action
- Keyboard navigation works (Up/Down, Enter to select, Escape to cancel)
- Menu uses the themed context menu renderer from Phase 28

**Dependencies:** Task 1, Phase 28 Task 2

---

### Task 5: Implement Cmd+. Quick Fix Shortcut

**Title:** Wire Cmd+. to trigger quick fix at cursor position

**Description:** When the user presses Cmd+. (or Ctrl+. on Linux/Windows), compute available code actions at the current cursor position. If there is exactly one preferred action, apply it immediately. If there are multiple actions, show the code action menu.

**Implementation Details:**
In `EditorPanel::OnKeyDown()`:
```cpp
if (event.CmdDown() && event.GetKeyCode() == '.')
{
    auto line = GetCursorLine();
    auto col = GetCursorColumn();
    auto content = GetContent();
    auto actions = code_action_provider_->provide_action_set(
        content, line, col, line, col, current_file_uri_);

    if (actions.total_count() == 0) return;

    if (actions.has_preferred() && actions.total_count() == 1)
    {
        ApplyCodeAction(*actions.preferred_action());
    }
    else
    {
        ShowCodeActionMenu(actions);
    }
}
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp`

**Acceptance Criteria:**
- Cmd+. triggers code action computation at cursor position
- Single preferred action is auto-applied without menu
- Multiple actions show the code action menu
- No actions shows a subtle "No code actions available" indicator
- Action application updates the editor content immediately
- Diagnostics are refreshed after action application

**Dependencies:** Task 2, Task 4

---

### Task 6: Implement Lightbulb Click Handler

**Title:** Clicking the gutter lightbulb opens the code action menu

**Description:** When the user clicks the lightbulb icon in the gutter, open the code action menu at the lightbulb's position showing all available actions for that line.

**Implementation Details:**
In `EditorPanel`, detect clicks in the gutter lightbulb margin:
```cpp
void EditorPanel::OnGutterClick(int line, int margin)
{
    if (margin == kCodeActionMargin && lightbulb_.is_visible() && lightbulb_.line() == line)
    {
        auto content = GetContent();
        auto actions = code_action_provider_->provide_action_set(
            content, line, 0, line, 999, current_file_uri_);
        ShowCodeActionMenu(actions);
    }
}
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp`

**Acceptance Criteria:**
- Clicking the lightbulb opens the code action menu
- Menu appears at the lightbulb's position (not the mouse position)
- Actions are computed for the entire line
- Clicking outside the lightbulb on the gutter does not trigger the menu

**Dependencies:** Task 3, Task 4

---

### Task 7: Implement Code Action Preview

**Title:** Show a diff preview of what a code action will change before applying

**Description:** When hovering over a code action in the menu (or pressing a preview key), show a side-by-side or inline diff preview of the changes the action would make.

**Implementation Details:**
Create `/Users/ryanrentfro/code/markamp/src/ui/CodeActionPreview.h`:
```cpp
#pragma once
#include "core/CodeActionTypes.h"
#include "core/ThemeEngine.h"
#include <wx/popupwin.h>

namespace markamp::ui
{

class CodeActionPreview : public wxPopupTransientWindow
{
public:
    CodeActionPreview(wxWindow* parent,
                      core::ThemeEngine& theme_engine,
                      const core::CodeAction& action,
                      const std::string& original_content);

    void ShowAt(const wxPoint& position);

    static constexpr int kMaxWidth = 500;
    static constexpr int kMaxHeight = 300;
    static constexpr int kPadding = 8;

private:
    core::ThemeEngine& theme_engine_;

    struct DiffLine
    {
        enum class Type { kContext, kRemoved, kAdded };
        Type type;
        std::string text;
        int line_number{0};
    };

    std::vector<DiffLine> diff_lines_;

    void ComputeDiff(const core::CodeAction& action, const std::string& original);
    void OnPaint(wxPaintEvent& event);
};

} // namespace markamp::ui
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/CodeActionPreview.h` (create)
- `/Users/ryanrentfro/code/markamp/src/ui/CodeActionPreview.cpp` (create)

**Acceptance Criteria:**
- Preview shows removed lines in red background and added lines in green background
- Context lines (unchanged) shown in normal colors
- Preview appears to the right of the code action menu
- Preview updates as the user navigates between actions
- Max preview size is 500x300px with scrolling for larger diffs
- Preview shows line numbers for context

**Dependencies:** Task 1

---

### Task 8: Implement Apply Code Action Logic

**Title:** Apply TextEdit changes from a code action to the editor content

**Description:** Implement the logic to apply a `CodeAction`'s `TextEdit` vector to the editor content. Edits must be applied in reverse order (bottom-to-top) to preserve line number validity.

**Implementation Details:**
```cpp
void EditorPanel::ApplyCodeAction(const core::CodeAction& action)
{
    auto* stc = GetStyledTextCtrl();
    stc->BeginUndoAction();

    // Sort edits in reverse order (bottom-to-top, right-to-left)
    auto sorted_edits = action.edits;
    std::sort(sorted_edits.begin(), sorted_edits.end(),
        [](const auto& a, const auto& b)
        {
            return std::tie(b.start_line, b.start_char) < std::tie(a.start_line, a.start_char);
        });

    for (const auto& edit : sorted_edits)
    {
        auto start_pos = stc->PositionFromLine(edit.start_line) + edit.start_char;
        auto end_pos = stc->PositionFromLine(edit.end_line) + edit.end_char;
        stc->SetTargetStart(start_pos);
        stc->SetTargetEnd(end_pos);
        stc->ReplaceTarget(edit.new_text);
    }

    stc->EndUndoAction();

    // Execute post-edit command if any
    if (!action.command.empty())
    {
        ExecuteCommand(action.command);
    }
}
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp`

**Acceptance Criteria:**
- Edits are applied as a single undo group
- Reverse-order application preserves line numbers
- After application, cursor is placed at the end of the first edit
- Content change triggers diagnostic refresh
- Undo (Cmd+Z) reverts all edits in one step
- Post-edit commands execute after all edits are applied

**Dependencies:** Task 1

---

### Task 9: Implement Code Action from Diagnostic Hover

**Title:** Show quick fix actions in the diagnostic hover popup

**Description:** When the user hovers over a diagnostic squiggle and a hover popup shows the diagnostic message, include a "Quick Fix" link or button that opens the code action menu for that diagnostic.

**Implementation Details:**
In the diagnostic hover popup:
```cpp
struct DiagnosticHoverContent
{
    std::string message;
    std::string source;
    std::string code;
    std::string fix_hint; // From DiagnosticsErrorHelp
    bool has_quick_fix{false};
};
```
Add a "Quick Fix..." link at the bottom of the hover that, when clicked, triggers Cmd+. behavior for that specific diagnostic.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp`

**Acceptance Criteria:**
- Diagnostic hover popup shows the message, source, and code
- If `DiagnosticsErrorHelp` has a fix hint, it is shown
- "Quick Fix (Cmd+.)" link appears at the bottom if code actions are available
- Clicking the link opens the code action menu
- Hover popup shows the fix hint from `DiagnosticsErrorHelp`

**Dependencies:** Task 2, Task 4

---

### Task 10: Implement Organize Imports Action

**Title:** Code action to organize and deduplicate reference links

**Description:** Add a "Source Action" that organizes reference-style links in a markdown document: sorts them alphabetically, removes unused references, and standardizes formatting.

**Implementation Details:**
```cpp
auto CodeActionProvider::get_organize_links_action(const std::string& content)
    -> std::vector<CodeAction>
{
    // Parse all reference links: [id]: url "title"
    // Check which are referenced in the document body
    // Sort alphabetically by reference ID
    // Remove unused references
    // Generate TextEdits to restructure the reference section
}
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/core/CodeActionProvider.cpp`

**Acceptance Criteria:**
- "Organize Reference Links" source action is available for markdown files
- Unused reference links are identified and removed
- Remaining references are sorted alphabetically
- Preview shows the before/after diff
- Action only appears when the document has reference links
- Action is non-destructive (does not remove referenced links)

**Dependencies:** Task 2

---

### Task 11: Implement Format Document Action

**Title:** Code action to format the current markdown document

**Description:** Add a "Source Action" that formats the document: normalizes heading spacing, ensures blank lines around code fences, standardizes list indentation, and normalizes emphasis markers.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/core/CodeActionProvider.cpp`

**Acceptance Criteria:**
- "Format Document" source action is available for all markdown files
- Normalizes heading spacing (blank line before, optional after)
- Ensures blank lines around code fences
- Standardizes list indentation (2 or 4 spaces, configurable)
- Normalizes emphasis markers (* vs _) per user preference
- Action can be triggered with Cmd+Shift+F

**Dependencies:** Task 2

---

### Task 12: Implement Extract Function/Variable Actions

**Title:** Code actions for extracting selected text into markdown constructs

**Description:** When text is selected, offer refactoring actions: "Extract to Heading" (wrap in a new heading), "Extract to Footnote" (convert to footnote reference), "Extract to Block Quote" (wrap in blockquote).

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/core/CodeActionProvider.cpp`

**Acceptance Criteria:**
- "Extract to Heading" creates a new heading with selected text
- "Extract to Footnote" converts selected text to a footnote reference + definition
- "Extract to Block Quote" wraps selection in `>` prefix
- Actions only appear when text is selected
- Each generates the correct TextEdits
- Preview shows the resulting change

**Dependencies:** Task 2

---

### Task 13: Implement Inline Variable Action

**Title:** Code action to inline a reference link into direct links

**Description:** When the cursor is on a reference link definition (e.g., `[id]: url`), offer an action to inline that reference into all usage sites and remove the definition.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/core/CodeActionProvider.cpp`

**Acceptance Criteria:**
- "Inline Reference Link" action appears on reference definitions
- All `[text][id]` usages are replaced with `[text](url)`
- The reference definition is removed
- Preview shows all affected locations
- Action is not offered for references used more than 10 times (to avoid large diffs)

**Dependencies:** Task 2

---

### Task 14: Implement Preferred Code Action Auto-Apply

**Title:** Auto-apply the preferred code action without showing a menu

**Description:** When Cmd+. is pressed and there is exactly one preferred code action (and no other actions), apply it immediately without showing the menu. Show a brief "Applied: [action title]" notification.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp`

**Acceptance Criteria:**
- Single preferred action is auto-applied on Cmd+.
- Brief notification shows "Applied: [title]" for 2 seconds
- Multiple actions (even with one preferred) still show the menu
- Auto-apply can be disabled in settings

**Dependencies:** Task 5

---

### Task 15: Implement Code Action Keyboard Navigation

**Title:** Navigate and trigger code actions without the mouse

**Description:** The code action menu supports full keyboard navigation: arrow keys to navigate, Enter to apply, Space to preview, Escape to cancel. Tab cycles between categories.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/CodeActionMenu.cpp`

**Acceptance Criteria:**
- Up/Down moves selection within a category
- Tab/Shift+Tab moves between categories
- Enter applies the selected action
- Space toggles the diff preview
- Escape closes the menu without applying
- Selection starts on the preferred action if one exists

**Dependencies:** Task 4

---

### Task 16: Wire Lightbulb to Cursor Position Changes

**Title:** Update lightbulb visibility when cursor moves between lines

**Description:** Subscribe to cursor position changes in the editor and recompute code action availability for the new line. Show/hide the lightbulb accordingly. Use debouncing (100ms) to avoid excessive computation during rapid cursor movement.

**Implementation Details:**
```cpp
void EditorPanel::OnCursorPositionChanged()
{
    lightbulb_debounce_timer_.StartOnce(100);
}

void EditorPanel::OnLightbulbDebounce(wxTimerEvent&)
{
    auto line = GetCursorLine();
    auto content = GetContent();
    auto actions = code_action_provider_->provide_action_set(
        content, line, 0, line, 999, current_file_uri_);

    if (actions.total_count() > 0)
    {
        auto kind = actions.has_preferred() ? core::CodeActionKind::kQuickFix
                                             : core::CodeActionKind::kRefactor;
        lightbulb_.set_visible(true, line);
        lightbulb_.set_action_kind(kind);
    }
    else
    {
        lightbulb_.set_visible(false);
    }
    Refresh();
}
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp`

**Acceptance Criteria:**
- Lightbulb appears within 100ms of cursor settling on a line with actions
- Lightbulb disappears when cursor moves to a line without actions
- Lightbulb kind reflects the highest-priority action kind
- No lightbulb flicker during rapid cursor movement (debouncing)
- Lightbulb position tracks the gutter margin correctly during scrolling

**Dependencies:** Task 3

---

### Task 17: Define Code Action Events

**Title:** EventBus events for code action lifecycle

**Implementation Details:**
Add to Events.h:
```cpp
MARKAMP_DECLARE_EVENT_WITH_FIELDS(CodeActionAppliedEvent)
    std::string action_title;
    std::string file_uri;
    int line{0};
    int edit_count{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(CodeActionsAvailableEvent)
    std::string file_uri;
    int line{0};
    int action_count{0};
MARKAMP_DECLARE_EVENT_END;
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/core/Events.h`

**Acceptance Criteria:**
- `CodeActionAppliedEvent` published after every action application
- `CodeActionsAvailableEvent` published when lightbulb appears (for status bar)
- Events include the file URI and line for context

**Dependencies:** None

---

### Task 18: Update CMakeLists.txt

**Title:** Add all new code action source files to the build

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/CMakeLists.txt`

**Acceptance Criteria:**
- All new files in both `add_executable()` and `source_group()`
- Build succeeds

**Dependencies:** All create tasks

---

### Task 19: Write Unit Tests for CodeActionTypes

**Title:** Test code action data structures and preview generation

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/tests/unit/test_code_actions_v2.cpp` (create)

**Acceptance Criteria:**
- Tests verify `TextEdit` application logic
- Tests verify `CodeActionSet` grouping and preferred selection
- Tests verify `preview_text()` generates correct diffs
- All tests pass

**Dependencies:** Task 1

---

### Task 20: Write Unit Tests for Extended CodeActionProvider

**Title:** Test new action kinds and extension provider registration

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/tests/unit/test_code_actions_v2.cpp`

**Acceptance Criteria:**
- Tests verify `provide_action_set()` returns categorized actions
- Tests verify organize links action
- Tests verify format document action
- Tests verify extract actions on selected text
- Tests verify inline reference link action
- Tests verify extension provider registration and routing
- All tests pass

**Dependencies:** Task 2

---

### Task 21: Write Unit Tests for Lightbulb Positioning

**Title:** Test lightbulb visibility logic

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/tests/unit/test_code_actions_v2.cpp`

**Acceptance Criteria:**
- Tests verify lightbulb appears on lines with diagnostics
- Tests verify lightbulb disappears on lines without actions
- Tests verify lightbulb kind reflects action types
- Tests verify debounce prevents rapid toggling
- All tests pass

**Dependencies:** Task 3, Task 16

---

### Task 22: Write Integration Tests for Auto-Apply Flow

**Title:** Test Cmd+. auto-apply with single preferred action

**Description:** Verify that when there is exactly one preferred code action and no other actions, Cmd+. applies it immediately without showing a menu.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/tests/unit/test_code_actions_v2.cpp`

**Acceptance Criteria:**
- Tests verify single preferred action is auto-applied
- Tests verify multiple actions (with preferred) show menu instead
- Tests verify no-action scenario shows no-actions indicator
- Tests verify auto-applied action content is correct
- All tests pass

**Dependencies:** Task 5

---

### Task 23: Write Integration Tests for Multi-Edit Actions

**Title:** Test multi-edit code actions apply in correct reverse order

**Description:** Verify that code actions with multiple TextEdits apply in reverse order (bottom-to-top) to preserve line number validity during the edit sequence.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/tests/unit/test_code_actions_v2.cpp`

**Acceptance Criteria:**
- Tests verify edits applied bottom-to-top
- Tests verify edits within the same line applied right-to-left
- Tests verify resulting document content matches expected output
- Tests verify edits that span multiple lines work correctly
- All tests pass

**Dependencies:** Task 8

---

### Task 24: Write Integration Tests for Undo After Code Action

**Title:** Test that undo reverts all edits from a code action in one step

**Description:** Verify that after applying a code action with multiple edits, a single undo operation reverts all of them because they are wrapped in a single undo group.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/tests/unit/test_code_actions_v2.cpp`

**Acceptance Criteria:**
- Tests verify single Cmd+Z undoes all edits from a code action
- Tests verify redo (Cmd+Shift+Z) re-applies all edits
- Tests verify undo restores exact original content
- All tests pass

**Dependencies:** Task 8

---

### Task 25: Write Integration Tests for Extension Action Providers

**Title:** Test extension-registered code action providers

**Description:** Verify that extensions can register custom code action providers and that their actions appear in the code action menu alongside built-in actions.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/tests/unit/test_code_actions_v2.cpp`

**Acceptance Criteria:**
- Tests verify registered extension provider is called during provide_action_set()
- Tests verify extension actions appear in the correct category
- Tests verify extension actions can be applied like built-in actions
- Tests verify re-registering a provider replaces the old one
- All tests pass

**Dependencies:** Task 2

---

### Task 26: Documentation

**Title:** Document the code action system architecture

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/docs/architecture.md`

**Acceptance Criteria:**
- Code action types and categories documented
- CodeActionProvider extension API documented
- Lightbulb behavior documented
- Built-in markdown code actions listed
- Keyboard shortcuts documented

**Dependencies:** All previous tasks

## Testing Requirements

- Unit tests for code action types and TextEdit application
- Unit tests for all built-in code action providers
- Integration tests for Cmd+. auto-apply flow
- Integration tests for lightbulb positioning
- Manual testing with various markdown patterns

## Phase Completion Criteria

- Lightbulb icon appears in the gutter for lines with available code actions
- Clicking the lightbulb shows categorized code action menu
- Cmd+. auto-applies preferred action or shows menu
- Code action preview shows diff of proposed changes
- All built-in markdown actions (organize links, format, extract, inline) work
- Extension providers can register custom code actions
- All unit and integration tests pass
