# Phase 24: Debug Console Panel

## Overview

Implement a Debug Console panel that provides a REPL-style interface for expression evaluation, variable inspection, and debug output viewing. The console displays colored output (stdout=white, stderr=red, debug=blue), supports object/array tree expansion, clickable stack trace links, command history navigation, and multi-line input mode. This panel is essential for debugging workflows and extension development.

## Prerequisites

- Phase 21 (Integrated Terminal Panel) -- shares ANSI parsing and buffer infrastructure
- Phase 22 (Output Panel V2) -- shares log level and colorized rendering patterns
- Phase 23 (Problems Panel V2) -- shares tree expansion UI patterns
- Existing `EventBus` and `Events.h`
- Existing `ThemeAwareWindow` base class

## Target Files

| Action | File |
|--------|------|
| Create | `/Users/ryanrentfro/code/markamp/src/core/DebugConsoleService.h` |
| Create | `/Users/ryanrentfro/code/markamp/src/core/DebugConsoleService.cpp` |
| Create | `/Users/ryanrentfro/code/markamp/src/ui/DebugConsolePanel.h` |
| Create | `/Users/ryanrentfro/code/markamp/src/ui/DebugConsolePanel.cpp` |
| Create | `/Users/ryanrentfro/code/markamp/src/ui/ConsoleOutputRenderer.h` |
| Create | `/Users/ryanrentfro/code/markamp/src/ui/ConsoleOutputRenderer.cpp` |
| Create | `/Users/ryanrentfro/code/markamp/src/core/ExpressionEvaluator.h` |
| Create | `/Users/ryanrentfro/code/markamp/src/core/ExpressionEvaluator.cpp` |
| Modify | `/Users/ryanrentfro/code/markamp/src/core/Events.h` |
| Modify | `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.h` |
| Modify | `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp` |
| Modify | `/Users/ryanrentfro/code/markamp/src/core/PluginContext.h` |
| Modify | `/Users/ryanrentfro/code/markamp/src/app/MarkAmpApp.cpp` |
| Modify | `/Users/ryanrentfro/code/markamp/src/CMakeLists.txt` |
| Create | `/Users/ryanrentfro/code/markamp/tests/unit/test_debug_console.cpp` |

## Tasks

### Task 1: Define Debug Console Events

**Title:** Declare EventBus events for debug console I/O and state changes

**Description:** Add event types for console input, console output (with output type), expression evaluation requests/results, and console clear.

**Implementation Details:**
Add to `/Users/ryanrentfro/code/markamp/src/core/Events.h`:
```cpp
MARKAMP_DECLARE_EVENT_WITH_FIELDS(DebugConsoleOutputEvent)
    std::string text;
    enum class OutputType : uint8_t { kStdout, kStderr, kDebug, kSystem };
    OutputType type{OutputType::kStdout};
    std::string source; // e.g., "extension:markdown-lint"
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(DebugConsoleInputEvent)
    std::string expression;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(DebugConsoleEvalResultEvent)
    std::string expression;
    std::string result;
    bool is_error{false};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT(DebugConsoleClearEvent);
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/core/Events.h`

**Acceptance Criteria:**
- All four event types compile and are publishable
- `OutputType` enum differentiates stdout, stderr, debug, and system messages
- Events follow the existing macro conventions
- No existing events are broken

**Dependencies:** None

---

### Task 2: Implement DebugConsoleService

**Title:** Core service for debug console state, history, and expression routing

**Description:** Create the `DebugConsoleService` that manages console output history, input command history, and routes expression evaluation requests. The service stores structured output entries (not raw text) with metadata about output type, timestamp, and source.

**Implementation Details:**
Create `/Users/ryanrentfro/code/markamp/src/core/DebugConsoleService.h`:
```cpp
#pragma once
#include "EventBus.h"
#include "LogLevel.h"
#include <chrono>
#include <string>
#include <variant>
#include <vector>

namespace markamp::core
{

struct ConsoleEntry
{
    enum class Type : uint8_t { kOutput, kInput, kResult, kError, kSystem };
    Type type{Type::kOutput};
    std::string text;
    std::string source;
    std::chrono::system_clock::time_point timestamp;
    LogLevel level{LogLevel::kInfo};
};

struct ConsoleObjectValue
{
    std::string key;
    std::string display_value;
    std::string type_name;
    bool expandable{false};
    std::vector<ConsoleObjectValue> children;
};

class DebugConsoleService
{
public:
    explicit DebugConsoleService(EventBus& event_bus);

    void write_stdout(const std::string& text, const std::string& source = {});
    void write_stderr(const std::string& text, const std::string& source = {});
    void write_debug(const std::string& text, const std::string& source = {});
    void write_system(const std::string& text);

    void submit_expression(const std::string& expression);
    void add_result(const std::string& expression, const std::string& result, bool is_error);

    [[nodiscard]] auto entries() const -> const std::vector<ConsoleEntry>&;
    [[nodiscard]] auto entry_count() const -> std::size_t;

    void clear();

    // Command history
    void push_history(const std::string& command);
    [[nodiscard]] auto history_at(int index) const -> const std::string&;
    [[nodiscard]] auto history_size() const -> int;

    // Log level filter
    void set_min_level(LogLevel level);
    [[nodiscard]] auto min_level() const -> LogLevel;
    [[nodiscard]] auto filtered_entries() const -> std::vector<const ConsoleEntry*>;

    static constexpr int kMaxHistory = 500;
    static constexpr int kMaxEntries = 50000;

private:
    EventBus& event_bus_;
    std::vector<ConsoleEntry> entries_;
    std::vector<std::string> command_history_;
    LogLevel min_level_{LogLevel::kDebug};

    Subscription output_sub_;
    Subscription input_sub_;
    Subscription eval_sub_;
    Subscription clear_sub_;

    void add_entry(ConsoleEntry entry);
    void trim_entries();
};

} // namespace markamp::core
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/core/DebugConsoleService.h` (create)
- `/Users/ryanrentfro/code/markamp/src/core/DebugConsoleService.cpp` (create)

**Acceptance Criteria:**
- Service stores structured entries with type, text, source, and timestamp
- Command history stores up to 500 entries
- Output history is capped at 50,000 entries (oldest removed first)
- `filtered_entries()` returns entries at or above the minimum log level
- `clear()` removes all entries and publishes `DebugConsoleClearEvent`
- Constructor injection of `EventBus` per CLAUDE.md convention

**Dependencies:** Task 1

---

### Task 3: Implement Expression Evaluator

**Title:** Extensible expression evaluation framework for the debug console

**Description:** Create an `ExpressionEvaluator` that routes evaluation requests to registered handlers. Built-in handlers support simple expressions like variable inspection, config queries, and system info. Extensions can register custom evaluation handlers.

**Implementation Details:**
Create `/Users/ryanrentfro/code/markamp/src/core/ExpressionEvaluator.h`:
```cpp
#pragma once
#include <functional>
#include <optional>
#include <string>
#include <vector>

namespace markamp::core
{

struct EvalResult
{
    std::string value;
    std::string type_name;
    bool is_error{false};
    bool is_expandable{false};
    std::vector<std::pair<std::string, std::string>> properties; // key-value pairs
};

using EvalHandler = std::function<std::optional<EvalResult>(const std::string& expression)>;

class ExpressionEvaluator
{
public:
    ExpressionEvaluator() = default;

    void register_handler(const std::string& prefix, EvalHandler handler);
    [[nodiscard]] auto evaluate(const std::string& expression) -> EvalResult;

    // Built-in commands
    void register_built_in_commands();

private:
    std::vector<std::pair<std::string, EvalHandler>> handlers_;

    [[nodiscard]] auto evaluate_built_in(const std::string& expression) -> std::optional<EvalResult>;
};

} // namespace markamp::core
```

Built-in commands:
- `.help` -- list available commands
- `.clear` -- clear the console
- `.config <key>` -- query a config value
- `.version` -- show MarkAmp version
- `.env <name>` -- read an environment variable
- `.theme` -- show current theme name and token count

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/core/ExpressionEvaluator.h` (create)
- `/Users/ryanrentfro/code/markamp/src/core/ExpressionEvaluator.cpp` (create)

**Acceptance Criteria:**
- Built-in commands (prefixed with `.`) work without any registered handlers
- Extension-registered handlers receive expressions matching their prefix
- If no handler matches, a "No evaluator available" error result is returned
- Evaluation is synchronous (async evaluation is a future enhancement)
- Handler registration is idempotent (re-registering replaces the old handler)

**Dependencies:** None

---

### Task 4: Implement Console Output Renderer

**Title:** Custom renderer for colorized console output with tree expansion

**Description:** Create a console output renderer that draws structured console entries with type-appropriate colors, tree-expandable objects, and clickable stack trace links.

**Implementation Details:**
Create `/Users/ryanrentfro/code/markamp/src/ui/ConsoleOutputRenderer.h`:
```cpp
#pragma once
#include "core/DebugConsoleService.h"
#include "core/ThemeEngine.h"
#include <wx/dc.h>

namespace markamp::ui
{

class ConsoleOutputRenderer
{
public:
    explicit ConsoleOutputRenderer(core::ThemeEngine& theme_engine);

    void paint_entry(wxDC& dc, const core::ConsoleEntry& entry,
                     int x, int y, int width, const wxFont& font);

    [[nodiscard]] auto entry_height(const core::ConsoleEntry& entry,
                                     int width, const wxFont& font) -> int;

    [[nodiscard]] auto stdout_color() -> wxColour;
    [[nodiscard]] auto stderr_color() -> wxColour;
    [[nodiscard]] auto debug_color() -> wxColour;
    [[nodiscard]] auto system_color() -> wxColour;
    [[nodiscard]] auto input_color() -> wxColour;
    [[nodiscard]] auto result_color() -> wxColour;

private:
    core::ThemeEngine& theme_engine_;
};

} // namespace markamp::ui
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/ConsoleOutputRenderer.h` (create)
- `/Users/ryanrentfro/code/markamp/src/ui/ConsoleOutputRenderer.cpp` (create)

**Acceptance Criteria:**
- stdout entries render in white/primary foreground color
- stderr entries render in red/error color
- debug entries render in blue/info color
- system entries render in dimmed foreground
- Input entries show a `>` prompt prefix in accent color
- Result entries show a `<` prefix in result color
- Multi-line entries calculate correct height for virtual scrolling

**Dependencies:** Task 2

---

### Task 5: Implement DebugConsolePanel UI Component

**Title:** Create the main DebugConsolePanel with output area and input line

**Description:** Build the `DebugConsolePanel` as a `ThemeAwareWindow` with two regions: a scrollable output area at the top and a single-line (or multi-line) input field at the bottom. The output area uses virtual scrolling for performance. The input field has auto-complete and syntax highlighting for expressions.

**Implementation Details:**
Create `/Users/ryanrentfro/code/markamp/src/ui/DebugConsolePanel.h`:
```cpp
#pragma once
#include "ThemeAwareWindow.h"
#include "core/DebugConsoleService.h"
#include "core/ExpressionEvaluator.h"
#include "ConsoleOutputRenderer.h"
#include <wx/textctrl.h>
#include <wx/timer.h>

namespace markamp::ui
{

class DebugConsolePanel : public ThemeAwareWindow
{
public:
    DebugConsolePanel(wxWindow* parent,
                      core::ThemeEngine& theme_engine,
                      core::EventBus& event_bus,
                      core::DebugConsoleService& service,
                      core::ExpressionEvaluator& evaluator);

    void Clear();
    void SetLogLevel(core::LogLevel level);
    void ToggleMultiLineInput();

    static constexpr int kInputHeight = 28;
    static constexpr int kMultiLineInputHeight = 100;
    static constexpr int kMaxInputHistory = 500;

protected:
    void OnThemeChanged(const core::Theme& new_theme) override;

private:
    core::EventBus& event_bus_;
    core::DebugConsoleService& service_;
    core::ExpressionEvaluator& evaluator_;
    ConsoleOutputRenderer renderer_;

    // Output area
    int scroll_offset_{0};
    bool auto_scroll_{true};

    // Input
    wxTextCtrl* input_field_{nullptr};
    bool multi_line_mode_{false};
    int history_index_{-1};

    // Event subscriptions
    core::Subscription output_sub_;
    core::Subscription clear_sub_;

    void OnPaint(wxPaintEvent& event);
    void OnSize(wxSizeEvent& event);
    void OnMouseWheel(wxMouseEvent& event);
    void OnInputKeyDown(wxKeyEvent& event);
    void OnInputEnter(wxCommandEvent& event);

    void SubmitExpression();
    void HistoryUp();
    void HistoryDown();
    void AutoScrollToBottom();

    [[nodiscard]] auto CalculateOutputHeight() -> int;
    [[nodiscard]] auto VisibleEntryRange() -> std::pair<int, int>;
};

} // namespace markamp::ui
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/DebugConsolePanel.h` (create)
- `/Users/ryanrentfro/code/markamp/src/ui/DebugConsolePanel.cpp` (create)

**Acceptance Criteria:**
- Panel has scrollable output area and input field separated by a thin line
- Output area uses virtual scrolling (only renders visible entries)
- Input field has a `>` prompt indicator
- Pressing Enter submits the expression
- Focus automatically goes to input field when panel is shown
- Panel subscribes to debug console events for live updates

**Dependencies:** Task 2, Task 3, Task 4

---

### Task 6: Implement REPL Input with History Navigation

**Title:** Command history navigation with up/down arrows in the input field

**Description:** The input field supports navigating through command history using Up and Down arrow keys, similar to a shell. When navigating history, the current (unsaved) input is preserved and restored when returning to the bottom of the history stack.

**Implementation Details:**
```cpp
void DebugConsolePanel::OnInputKeyDown(wxKeyEvent& event)
{
    if (event.GetKeyCode() == WXK_UP)
    {
        if (history_index_ == -1)
        {
            saved_input_ = input_field_->GetValue().ToStdString();
        }
        if (history_index_ < service_.history_size() - 1)
        {
            ++history_index_;
            input_field_->SetValue(service_.history_at(history_index_));
            input_field_->SetInsertionPointEnd();
        }
    }
    else if (event.GetKeyCode() == WXK_DOWN)
    {
        if (history_index_ > 0)
        {
            --history_index_;
            input_field_->SetValue(service_.history_at(history_index_));
        }
        else if (history_index_ == 0)
        {
            history_index_ = -1;
            input_field_->SetValue(saved_input_);
        }
        input_field_->SetInsertionPointEnd();
    }
    else
    {
        event.Skip();
    }
}
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/DebugConsolePanel.cpp`

**Acceptance Criteria:**
- Up arrow navigates to the previous command in history
- Down arrow navigates to the next command (toward most recent)
- Current unsaved input is preserved when entering history
- Returning to the bottom restores the unsaved input
- History wraps: pressing Up at the oldest entry stays there
- Duplicate consecutive commands are not stored twice
- History persists across console clears but not across sessions

**Dependencies:** Task 5

---

### Task 7: Implement Variable Inspection on Hover

**Title:** Show variable value tooltip when hovering over identifiers in output

**Description:** When the user hovers over an identifier-like token in the console output, attempt to evaluate it and show the result in a tooltip popup. This works for simple identifiers and dotted property paths.

**Implementation Details:**
```cpp
void DebugConsolePanel::OnOutputMouseMove(wxMouseEvent& event)
{
    auto word = GetWordAtPosition(event.GetPosition());
    if (!word.empty() && word != last_hover_word_)
    {
        last_hover_word_ = word;
        auto result = evaluator_.evaluate(word);
        if (!result.is_error)
        {
            ShowValueTooltip(event.GetPosition(), word, result);
        }
    }
}
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/DebugConsolePanel.cpp`

**Acceptance Criteria:**
- Hovering over a word in the output shows its evaluated value
- Tooltip appears after a 500ms hover delay
- Tooltip shows the type name and value
- Tooltip disappears when the mouse moves away
- Invalid identifiers do not show a tooltip (no error popup)
- Hover evaluation is debounced (max 1 per 300ms)

**Dependencies:** Task 3, Task 5

---

### Task 8: Implement Colored Output Types

**Title:** Render stdout, stderr, and debug output with distinct colors

**Description:** Configure the console output renderer to use theme-derived colors for each output type. Stdout uses the primary foreground, stderr uses the error color, debug uses the info/blue color, and system messages use a dimmed foreground.

**Implementation Details:**
Color mapping:
```
stdout  -> theme.fg_primary (white in dark themes, black in light)
stderr  -> theme.fg_error (red)
debug   -> theme.fg_info (blue)
system  -> theme.fg_dimmed (gray)
input   -> theme.fg_accent (purple/blue)
result  -> theme.fg_success (green)
error   -> theme.fg_error (red, italic)
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/ConsoleOutputRenderer.cpp`

**Acceptance Criteria:**
- Each output type has a visually distinct color
- Colors adapt to the active theme (dark vs light)
- stderr entries have a subtle red background highlight in addition to red text
- System messages are visually de-emphasized (dimmed, smaller font optional)
- Input prompts show `>` in accent color
- Result prompts show `<` in success color

**Dependencies:** Task 4

---

### Task 9: Implement Object and Array Expansion

**Title:** Tree-view expansion for structured values in console output

**Description:** When an expression evaluates to an object or array, display it in a collapsed form initially (e.g., `{Object(3)}` or `[Array(5)]`). Clicking the disclosure triangle expands it to show key-value pairs, each of which may be further expandable.

**Implementation Details:**
```cpp
struct ExpandableEntry
{
    int entry_index;
    bool is_expanded{false};
    core::ConsoleObjectValue root;
};

void ConsoleOutputRenderer::paint_expandable(wxDC& dc, const ExpandableEntry& entry,
                                              int x, int y, int width, int indent)
{
    DrawDisclosureTriangle(dc, x + indent, y, entry.is_expanded);
    DrawTypeTag(dc, x + indent + 16, y, entry.root.type_name);
    if (entry.is_expanded)
    {
        for (const auto& child : entry.root.children)
        {
            y += kRowHeight;
            DrawPropertyRow(dc, child, x, y, indent + 20);
        }
    }
}
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/DebugConsolePanel.h`
- `/Users/ryanrentfro/code/markamp/src/ui/DebugConsolePanel.cpp`
- `/Users/ryanrentfro/code/markamp/src/ui/ConsoleOutputRenderer.h`
- `/Users/ryanrentfro/code/markamp/src/ui/ConsoleOutputRenderer.cpp`

**Acceptance Criteria:**
- Objects display as `{Object(N)}` with a disclosure triangle
- Arrays display as `[Array(N)]` with a disclosure triangle
- Clicking the triangle toggles expansion
- Nested objects/arrays can be expanded recursively
- Property keys render in a distinct color (purple/accent)
- Property values are syntax-colored (strings=green, numbers=blue, booleans=orange)
- Max expansion depth is 10 levels

**Dependencies:** Task 4

---

### Task 10: Implement Clear Console

**Title:** Clear all console output with button and keyboard shortcut

**Description:** Add a clear button in the debug console panel header and support Cmd+K keyboard shortcut. Clearing removes all output entries but preserves command history.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/DebugConsolePanel.cpp`

**Acceptance Criteria:**
- Clear button (trash icon) in panel header
- Cmd+K clears all output
- Command history is preserved after clear
- Console shows empty state message after clear
- `DebugConsoleClearEvent` is published

**Dependencies:** Task 5

---

### Task 11: Implement Log Level Filter

**Title:** Filter console output by log level

**Description:** Add filter buttons (or a dropdown) in the panel header to filter visible entries by log level: Error, Warning, Info, Debug. This reuses the same `LogLevel` enum from Phase 22.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/DebugConsolePanel.h`
- `/Users/ryanrentfro/code/markamp/src/ui/DebugConsolePanel.cpp`

**Acceptance Criteria:**
- Log level filter buttons in panel header
- Filtering immediately hides/shows entries
- Filter state persists during the session
- Default: show all levels (Debug and above)
- Input and result entries are always shown regardless of filter

**Dependencies:** Task 5

---

### Task 12: Implement Stack Trace Link Detection

**Title:** Detect and render clickable file:line references in console output

**Description:** Scan console output for stack trace patterns (e.g., `at file.cpp:42`, `file.md:10:5`, `/path/to/file.ext:line`) and render them as clickable links. Clicking a link navigates to the file and line in the editor.

**Implementation Details:**
```cpp
struct StackTraceLink
{
    int start_offset;
    int end_offset;
    std::string file_path;
    int line{0};
    int column{0};
};

auto detect_stack_links(const std::string& text) -> std::vector<StackTraceLink>;
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/ConsoleOutputRenderer.h`
- `/Users/ryanrentfro/code/markamp/src/ui/ConsoleOutputRenderer.cpp`
- `/Users/ryanrentfro/code/markamp/src/ui/DebugConsolePanel.cpp`

**Acceptance Criteria:**
- Patterns like `file.cpp:42`, `file.md:10:5`, `/path/to/file.ext:42` are detected
- Links render with underline decoration in accent color
- Clicking a link opens the file and navigates to the line
- Hover shows the full resolved path as a tooltip
- Invalid paths (non-existent files) are detected but not styled as links

**Dependencies:** Task 4

---

### Task 13: Implement Copy Value Action

**Title:** Copy console entry values to clipboard

**Description:** Add a "Copy Value" action in the context menu and on hover for console entries. For expanded objects, "Copy Value" copies the full JSON representation. For text entries, it copies the raw text.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/DebugConsolePanel.cpp`

**Acceptance Criteria:**
- Right-click context menu includes "Copy Value"
- For text entries, copies the raw text
- For object/array entries, copies a formatted JSON representation
- For result entries, copies just the result value (not the expression)
- "Copy Expression" copies the input expression that produced the result

**Dependencies:** Task 5, Task 9

---

### Task 14: Implement Console Font Settings

**Title:** Configurable font for the debug console

**Description:** Allow independent font configuration for the debug console. Defaults to the same monospace font as the terminal. Input field uses the same font as the output area.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/DebugConsolePanel.h`
- `/Users/ryanrentfro/code/markamp/src/ui/DebugConsolePanel.cpp`
- `/Users/ryanrentfro/code/markamp/src/core/Config.cpp`

**Acceptance Criteria:**
- Font family and size configurable under `debug_console.font` in config
- Changes apply immediately
- Input field and output area use the same font
- Default: same as terminal font settings

**Dependencies:** Task 5

---

### Task 15: Implement Multi-Line Input Mode

**Title:** Toggle between single-line and multi-line input in the debug console

**Description:** Add a toggle (Shift+Enter or a button) that expands the input field to a multi-line editor. Multi-line mode allows entering multi-line expressions. Cmd+Enter submits the expression in multi-line mode. The input area expands from 28px to 100px.

**Implementation Details:**
```cpp
void DebugConsolePanel::ToggleMultiLineInput()
{
    multi_line_mode_ = !multi_line_mode_;
    if (multi_line_mode_)
    {
        input_field_->SetMinSize({-1, kMultiLineInputHeight});
        input_field_->SetWindowStyle(input_field_->GetWindowStyle() | wxTE_MULTILINE);
    }
    else
    {
        input_field_->SetMinSize({-1, kInputHeight});
    }
    Layout();
}
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/DebugConsolePanel.cpp`

**Acceptance Criteria:**
- Single-line mode: Enter submits, Shift+Enter toggles to multi-line
- Multi-line mode: Enter inserts newline, Cmd+Enter submits
- Toggle button in input area indicates current mode
- Input area resizes smoothly between modes
- Multi-line content is preserved when toggling modes
- Multi-line mode shows line numbers in the input area

**Dependencies:** Task 5

---

### Task 16: Implement Console.log Interception

**Title:** Route extension console.log calls to the debug console

**Description:** Extensions that call `console.log()` (or equivalent) should have their output routed to the debug console panel. Wire the `PluginOutputRouter` to also send output to `DebugConsoleService` in addition to the Output panel.

**Implementation Details:**
```cpp
class PluginConsoleRouter
{
public:
    PluginConsoleRouter(DebugConsoleService& console, PluginOutputRouter& output);

    void log(const std::string& plugin_id, const std::string& message);
    void warn(const std::string& plugin_id, const std::string& message);
    void error(const std::string& plugin_id, const std::string& message);
    void debug(const std::string& plugin_id, const std::string& message);
};
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/core/DebugConsoleService.h`
- `/Users/ryanrentfro/code/markamp/src/core/DebugConsoleService.cpp`
- `/Users/ryanrentfro/code/markamp/src/core/PluginManager.cpp`

**Acceptance Criteria:**
- Extension log/warn/error/debug calls appear in the debug console
- Each entry shows the extension ID as the source
- Log levels map correctly (log=Info, warn=Warning, error=Error, debug=Debug)
- Output goes to both the debug console and the extension's output channel

**Dependencies:** Task 2

---

### Task 17: Implement Console Context Menu

**Title:** Right-click context menu for the debug console

**Description:** Context menu with actions: Copy, Copy Value, Copy All, Select All, Clear Console, Filter by Level.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/DebugConsolePanel.cpp`

**Acceptance Criteria:**
- Context menu appears on right-click in the output area
- "Copy" copies selected text
- "Copy Value" copies the value of the clicked entry
- "Clear Console" clears all output
- "Filter by Level" submenu allows choosing minimum level

**Dependencies:** Task 5, Task 13

---

### Task 18: Integrate Debug Console into LayoutManager

**Title:** Wire DebugConsolePanel as a bottom panel option

**Description:** Add the debug console as a new bottom panel type in the `LayoutManager`, alongside Terminal, Output, and Problems.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.h`
- `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`
- `/Users/ryanrentfro/code/markamp/src/app/MarkAmpApp.cpp`

**Acceptance Criteria:**
- Debug Console appears as a tab in the bottom panel area
- Panel is lazily created on first access
- Keyboard shortcut Cmd+Shift+Y opens the debug console (VS Code convention)
- Layout state persists across sessions

**Dependencies:** Task 5

---

### Task 19: Wire DebugConsoleService into PluginContext

**Title:** Expose DebugConsoleService to extensions via PluginContext

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/core/PluginContext.h`
- `/Users/ryanrentfro/code/markamp/src/app/MarkAmpApp.cpp`

**Acceptance Criteria:**
- `PluginContext` includes a `DebugConsoleService*` pointer
- Pointer is populated during `MarkAmpApp::OnInit()`
- Extensions can write to the console and register evaluation handlers
- Null-check guard prevents crashes if service is unavailable

**Dependencies:** Task 2

---

### Task 20: Update CMakeLists.txt

**Title:** Add all new debug console source files to the build

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/CMakeLists.txt`

**Acceptance Criteria:**
- All new `.h` and `.cpp` files in both `add_executable()` and `source_group()`
- Build succeeds with no linker errors

**Dependencies:** All create tasks

---

### Task 21: Write Unit Tests for DebugConsoleService

**Title:** Test console service entry management, history, and filtering

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/tests/unit/test_debug_console.cpp` (create)

**Acceptance Criteria:**
- Tests verify entry storage and retrieval
- Tests verify command history navigation
- Tests verify log level filtering
- Tests verify entry cap (50,000 max)
- Tests verify clear preserves history
- All tests pass

**Dependencies:** Task 2

---

### Task 22: Write Unit Tests for ExpressionEvaluator

**Title:** Test expression evaluation routing and built-in commands

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/tests/unit/test_debug_console.cpp`

**Acceptance Criteria:**
- Tests verify built-in `.help` command returns available commands
- Tests verify `.clear` command triggers clear
- Tests verify custom handler registration and routing
- Tests verify unhandled expressions return error result
- All tests pass

**Dependencies:** Task 3

---

### Task 23: Write Unit Tests for Stack Trace Detection

**Title:** Test file:line pattern detection in console output

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/tests/unit/test_debug_console.cpp`

**Acceptance Criteria:**
- Tests detect `file.cpp:42` patterns
- Tests detect `/path/to/file.ext:10:5` patterns
- Tests detect `at function (file.js:42:10)` patterns
- Tests do not false-positive on time strings like `14:30:00`
- Tests return correct file path, line, and column
- All tests pass

**Dependencies:** Task 12

---

### Task 24: Implement Input Auto-Complete

**Title:** Basic auto-complete for built-in commands in the input field

**Description:** Show an auto-complete popup for built-in commands when the user types a `.` prefix. The popup lists available commands with descriptions.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/DebugConsolePanel.cpp`

**Acceptance Criteria:**
- Typing `.` shows auto-complete popup with built-in commands
- Arrow keys navigate the popup
- Tab or Enter completes the selected command
- Escape closes the popup
- Popup is themed to match the application

**Dependencies:** Task 5, Task 3

---

### Task 25: Implement Console Output Timestamps

**Title:** Optional timestamps on console entries

**Description:** Add a toggle to show/hide timestamps on each console entry, identical to the Output Panel V2 timestamp feature.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/DebugConsolePanel.h`
- `/Users/ryanrentfro/code/markamp/src/ui/DebugConsolePanel.cpp`

**Acceptance Criteria:**
- Timestamp toggle button in panel header
- Timestamps format as `[HH:MM:SS.mmm]`
- Default: timestamps hidden
- Toggle applies immediately to all visible entries

**Dependencies:** Task 5

---

### Task 26: Documentation

**Title:** Document debug console architecture and extension API

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/docs/architecture.md`

**Acceptance Criteria:**
- Debug console service architecture documented
- Expression evaluation framework documented
- Extension API for console access documented
- Built-in commands listed and described

**Dependencies:** All previous tasks

## Testing Requirements

- Unit tests for DebugConsoleService entry management and history
- Unit tests for ExpressionEvaluator routing and built-in commands
- Unit tests for stack trace link detection
- Integration tests for event flow from service to UI
- Manual testing with expression evaluation and object expansion

## Phase Completion Criteria

- Debug console provides REPL-style input with command history
- Expression evaluation works with built-in commands and extension handlers
- Console output is color-coded by type (stdout/stderr/debug)
- Objects and arrays are expandable as tree views
- Stack trace links are clickable and navigate to file:line
- Multi-line input mode works with Cmd+Enter to submit
- All unit and integration tests pass
