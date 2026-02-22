# Phase 25: Build And Task Runner Panel

## Overview

Implement a Build and Task Runner panel that provides build output display with ANSI colors, compiler error parsing and navigation, task configuration (tasks.json equivalent), a task runner with task list, build/rebuild/clean actions, compiler selection, build target selection, parallel build progress tracking, and integration with CMake and Make. This panel turns MarkAmp Studio into a viable C++ development environment.

## Prerequisites

- Phase 21 (Integrated Terminal Panel) -- ANSI parser and PTY process infrastructure
- Phase 22 (Output Panel V2) -- colorized output rendering and log level filtering
- Phase 23 (Problems Panel V2) -- compiler error to diagnostic pipeline
- Phase 24 (Debug Console Panel) -- shares bottom panel integration patterns
- Existing `DiagnosticsService` for publishing compiler diagnostics
- Existing `OutputChannelService` for build output channels

## Target Files

| Action | File |
|--------|------|
| Create | `/Users/ryanrentfro/code/markamp/src/core/BuildService.h` |
| Create | `/Users/ryanrentfro/code/markamp/src/core/BuildService.cpp` |
| Create | `/Users/ryanrentfro/code/markamp/src/core/TaskRunner.h` |
| Create | `/Users/ryanrentfro/code/markamp/src/core/TaskRunner.cpp` |
| Create | `/Users/ryanrentfro/code/markamp/src/core/CompilerErrorParser.h` |
| Create | `/Users/ryanrentfro/code/markamp/src/core/CompilerErrorParser.cpp` |
| Create | `/Users/ryanrentfro/code/markamp/src/core/TaskConfig.h` |
| Create | `/Users/ryanrentfro/code/markamp/src/core/TaskConfig.cpp` |
| Create | `/Users/ryanrentfro/code/markamp/src/ui/BuildPanel.h` |
| Create | `/Users/ryanrentfro/code/markamp/src/ui/BuildPanel.cpp` |
| Create | `/Users/ryanrentfro/code/markamp/src/ui/TaskListPanel.h` |
| Create | `/Users/ryanrentfro/code/markamp/src/ui/TaskListPanel.cpp` |
| Modify | `/Users/ryanrentfro/code/markamp/src/core/Events.h` |
| Modify | `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.h` |
| Modify | `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp` |
| Modify | `/Users/ryanrentfro/code/markamp/src/ui/StatusBarPanel.h` |
| Modify | `/Users/ryanrentfro/code/markamp/src/ui/StatusBarPanel.cpp` |
| Modify | `/Users/ryanrentfro/code/markamp/src/CMakeLists.txt` |
| Create | `/Users/ryanrentfro/code/markamp/tests/unit/test_build_panel.cpp` |
| Create | `/Users/ryanrentfro/code/markamp/tests/unit/test_compiler_error_parser.cpp` |
| Create | `/Users/ryanrentfro/code/markamp/tests/unit/test_task_runner.cpp` |

## Tasks

### Task 1: Define Build and Task Events

**Title:** Declare EventBus events for build lifecycle, task execution, and progress

**Implementation Details:**
Add to `/Users/ryanrentfro/code/markamp/src/core/Events.h`:
```cpp
MARKAMP_DECLARE_EVENT_WITH_FIELDS(BuildStartedEvent)
    std::string build_id;
    std::string target;
    std::string configuration; // "debug", "release"
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(BuildProgressEvent)
    std::string build_id;
    int completed{0};
    int total{0};
    std::string current_file;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(BuildFinishedEvent)
    std::string build_id;
    bool success{false};
    int error_count{0};
    int warning_count{0};
    double elapsed_seconds{0.0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(BuildOutputEvent)
    std::string build_id;
    std::string line;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(TaskStartedEvent)
    std::string task_name;
    std::string task_group;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(TaskFinishedEvent)
    std::string task_name;
    bool success{false};
    int exit_code{0};
MARKAMP_DECLARE_EVENT_END;
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/core/Events.h`

**Acceptance Criteria:**
- All six event types compile and are publishable
- Build events include a `build_id` for tracking parallel builds
- Progress event includes file-level granularity
- Finished event includes aggregate error/warning counts and elapsed time

**Dependencies:** None

---

### Task 2: Implement Compiler Error Parser

**Title:** Parse GCC, Clang, and MSVC compiler output into structured diagnostics

**Description:** Create a parser that recognizes compiler error/warning output patterns from GCC, Clang, and MSVC and converts them into `Diagnostic` structures that can be published to `DiagnosticsService`.

**Implementation Details:**
Create `/Users/ryanrentfro/code/markamp/src/core/CompilerErrorParser.h`:
```cpp
#pragma once
#include "DiagnosticsService.h"
#include <optional>
#include <regex>
#include <string>
#include <vector>

namespace markamp::core
{

enum class CompilerType : uint8_t { kGcc, kClang, kMsvc, kAuto };

struct ParsedCompilerError
{
    std::string file_path;
    int line{0};
    int column{0};
    DiagnosticSeverity severity{DiagnosticSeverity::kError};
    std::string message;
    std::string error_code;
    std::vector<std::string> context_lines; // "note:" lines that follow
};

class CompilerErrorParser
{
public:
    explicit CompilerErrorParser(CompilerType type = CompilerType::kAuto);

    [[nodiscard]] auto parse_line(const std::string& line) -> std::optional<ParsedCompilerError>;
    [[nodiscard]] auto parse_output(const std::string& full_output) -> std::vector<ParsedCompilerError>;

    void set_compiler_type(CompilerType type);
    [[nodiscard]] auto detect_compiler_type(const std::string& output) -> CompilerType;

    [[nodiscard]] auto to_diagnostics(const std::vector<ParsedCompilerError>& errors)
        -> std::vector<std::pair<std::string, std::vector<Diagnostic>>>;

private:
    CompilerType type_;
    static const std::regex gcc_error_regex_;
    static const std::regex clang_error_regex_;
    static const std::regex msvc_error_regex_;
    static const std::regex cmake_error_regex_;

    [[nodiscard]] auto parse_gcc_line(const std::string& line) -> std::optional<ParsedCompilerError>;
    [[nodiscard]] auto parse_clang_line(const std::string& line) -> std::optional<ParsedCompilerError>;
    [[nodiscard]] auto parse_msvc_line(const std::string& line) -> std::optional<ParsedCompilerError>;
};

} // namespace markamp::core
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/core/CompilerErrorParser.h` (create)
- `/Users/ryanrentfro/code/markamp/src/core/CompilerErrorParser.cpp` (create)

**Acceptance Criteria:**
- Parses GCC format: `file.cpp:10:5: error: message`
- Parses Clang format: `file.cpp:10:5: error: message [-Wflag]`
- Parses MSVC format: `file.cpp(10): error C2065: message`
- Parses CMake error format: `CMake Error at CMakeLists.txt:10`
- Auto-detection guesses compiler from output patterns
- "note:" lines are attached to the preceding error as context
- Handles multi-line error messages (caret + context)
- `to_diagnostics()` groups errors by file for batch DiagnosticsService update

**Dependencies:** None

---

### Task 3: Implement Task Configuration Schema

**Title:** Define and parse tasks.json-equivalent configuration

**Description:** Create a task configuration system that reads task definitions from a `.markamp/tasks.json` file in the workspace root. Tasks define shell commands, groups (build/test/deploy), presentation options, and problem matchers.

**Implementation Details:**
Create `/Users/ryanrentfro/code/markamp/src/core/TaskConfig.h`:
```cpp
#pragma once
#include <string>
#include <vector>
#include <optional>

namespace markamp::core
{

struct TaskDefinition
{
    std::string name;
    std::string label;       // Display label
    std::string command;     // Shell command to execute
    std::string group;       // "build", "test", "deploy", "none"
    std::string type;        // "shell", "process"
    std::string working_directory;
    std::vector<std::string> args;
    bool is_default{false};
    bool run_on_save{false};
    bool clear_before_run{true};

    struct Presentation
    {
        bool reveal{true};       // Show output panel
        bool focus{false};       // Focus output panel
        bool echo_command{true}; // Print command before execution
        std::string panel;       // "shared", "dedicated", "new"
    };
    Presentation presentation;

    std::string problem_matcher; // "gcc", "clang", "msvc", "none"
};

class TaskConfig
{
public:
    TaskConfig() = default;

    [[nodiscard]] auto load_from_file(const std::string& path) -> bool;
    [[nodiscard]] auto load_from_string(const std::string& json) -> bool;

    [[nodiscard]] auto tasks() const -> const std::vector<TaskDefinition>&;
    [[nodiscard]] auto find_task(const std::string& name) const -> const TaskDefinition*;
    [[nodiscard]] auto tasks_in_group(const std::string& group) const -> std::vector<const TaskDefinition*>;
    [[nodiscard]] auto default_build_task() const -> const TaskDefinition*;
    [[nodiscard]] auto default_test_task() const -> const TaskDefinition*;

    void add_task(TaskDefinition task);
    void remove_task(const std::string& name);
    [[nodiscard]] auto save_to_file(const std::string& path) const -> bool;

    // Templates
    [[nodiscard]] static auto cmake_build_template() -> TaskDefinition;
    [[nodiscard]] static auto make_build_template() -> TaskDefinition;
    [[nodiscard]] static auto ctest_template() -> TaskDefinition;

private:
    std::vector<TaskDefinition> tasks_;
};

} // namespace markamp::core
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/core/TaskConfig.h` (create)
- `/Users/ryanrentfro/code/markamp/src/core/TaskConfig.cpp` (create)

**Acceptance Criteria:**
- Parses JSON task definitions with all fields
- Missing optional fields use sensible defaults
- Template tasks provide working CMake and Make configurations
- `save_to_file()` round-trips correctly
- Validation rejects tasks with empty name or command
- File I/O uses `std::error_code` overloads per CLAUDE.md

**Dependencies:** None

---

### Task 4: Implement Task Runner Service

**Title:** Execute tasks as PTY processes with output capture and lifecycle management

**Description:** Create a `TaskRunner` service that executes task definitions as child processes, captures their output, publishes events, and manages task lifecycle (start/stop/restart).

**Implementation Details:**
Create `/Users/ryanrentfro/code/markamp/src/core/TaskRunner.h`:
```cpp
#pragma once
#include "EventBus.h"
#include "PtyProcess.h"
#include "TaskConfig.h"
#include "CompilerErrorParser.h"
#include "DiagnosticsService.h"
#include "OutputChannelService.h"
#include <memory>
#include <string>
#include <unordered_map>

namespace markamp::core
{

struct RunningTask
{
    std::string name;
    std::unique_ptr<PtyProcess> process;
    std::string output;
    std::chrono::steady_clock::time_point start_time;
    CompilerErrorParser error_parser;
    std::string problem_matcher;
};

class TaskRunner
{
public:
    TaskRunner(EventBus& event_bus,
               DiagnosticsService& diagnostics,
               OutputChannelService& output_channels);

    auto run_task(const TaskDefinition& task) -> bool;
    void stop_task(const std::string& task_name);
    void restart_task(const std::string& task_name);
    [[nodiscard]] auto is_running(const std::string& task_name) const -> bool;
    [[nodiscard]] auto running_tasks() const -> std::vector<std::string>;

    void stop_all();

private:
    EventBus& event_bus_;
    DiagnosticsService& diagnostics_;
    OutputChannelService& output_channels_;
    std::unordered_map<std::string, std::unique_ptr<RunningTask>> tasks_;

    void on_task_output(const std::string& task_name, const std::string& data);
    void on_task_exit(const std::string& task_name, int exit_code);
    void parse_and_publish_errors(const std::string& task_name, const std::string& line);
};

} // namespace markamp::core
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/core/TaskRunner.h` (create)
- `/Users/ryanrentfro/code/markamp/src/core/TaskRunner.cpp` (create)

**Acceptance Criteria:**
- Tasks execute as PTY processes (for ANSI color support)
- Output is captured and published via `BuildOutputEvent`
- Compiler errors are parsed in real-time and published to `DiagnosticsService`
- Task start/finish events are published with timing info
- `stop_task()` sends SIGTERM and cleans up
- Multiple tasks can run simultaneously
- Constructor injection of all services per CLAUDE.md

**Dependencies:** Task 1, Task 2, Task 3, Phase 21 Task 4 (PtyProcess)

---

### Task 5: Implement BuildService

**Title:** High-level build orchestration service with CMake and Make integration

**Description:** Create a `BuildService` that wraps the `TaskRunner` with build-specific logic: CMake configure/build/clean sequences, build target selection, compiler selection, and build configuration management.

**Implementation Details:**
Create `/Users/ryanrentfro/code/markamp/src/core/BuildService.h`:
```cpp
#pragma once
#include "EventBus.h"
#include "TaskRunner.h"
#include <string>
#include <vector>

namespace markamp::core
{

enum class BuildSystem : uint8_t { kCMake, kMake, kCustom };

struct BuildTarget
{
    std::string name;
    std::string description;
    bool is_default{false};
};

struct BuildConfiguration
{
    std::string name;             // "debug", "release", "relwithdebinfo"
    std::string cmake_preset;     // CMake preset name
    std::string build_directory;
    std::string compiler;         // "clang++", "g++", "cl.exe"
    int parallel_jobs{0};         // 0 = auto-detect
};

class BuildService
{
public:
    BuildService(EventBus& event_bus, TaskRunner& task_runner);

    // Build actions
    auto build(const std::string& target = {}) -> bool;
    auto rebuild() -> bool;
    auto clean() -> bool;
    auto configure() -> bool;

    // Configuration
    void set_build_system(BuildSystem system);
    void set_configuration(const BuildConfiguration& config);
    void set_workspace_root(const std::string& path);

    [[nodiscard]] auto detect_build_system() const -> BuildSystem;
    [[nodiscard]] auto available_targets() const -> std::vector<BuildTarget>;
    [[nodiscard]] auto available_presets() const -> std::vector<std::string>;
    [[nodiscard]] auto current_configuration() const -> const BuildConfiguration&;
    [[nodiscard]] auto is_building() const -> bool;

    void stop_build();

private:
    EventBus& event_bus_;
    TaskRunner& task_runner_;
    BuildSystem build_system_{BuildSystem::kCMake};
    BuildConfiguration config_;
    std::string workspace_root_;

    [[nodiscard]] auto cmake_build_command() const -> std::string;
    [[nodiscard]] auto cmake_clean_command() const -> std::string;
    [[nodiscard]] auto cmake_configure_command() const -> std::string;
    [[nodiscard]] auto make_build_command() const -> std::string;
    [[nodiscard]] auto detect_parallel_jobs() const -> int;
};

} // namespace markamp::core
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/core/BuildService.h` (create)
- `/Users/ryanrentfro/code/markamp/src/core/BuildService.cpp` (create)

**Acceptance Criteria:**
- `detect_build_system()` checks for CMakeLists.txt and Makefile in workspace root
- `build()` constructs and executes the correct build command
- `rebuild()` runs clean then build
- `configure()` runs CMake configure step
- `available_targets()` parses CMake target list or Makefile targets
- `available_presets()` reads CMakePresets.json
- `detect_parallel_jobs()` returns `sysctl -n hw.ncpu` on macOS
- Build can be cancelled with `stop_build()`

**Dependencies:** Task 4

---

### Task 6: Implement BuildPanel UI Component

**Title:** Create the main BuildPanel with output display and action toolbar

**Description:** Build the `BuildPanel` as a `ThemeAwareWindow` that displays build output with ANSI colors, shows a progress bar during builds, and provides build action buttons (Build, Rebuild, Clean, Configure, Stop).

**Implementation Details:**
Create `/Users/ryanrentfro/code/markamp/src/ui/BuildPanel.h`:
```cpp
#pragma once
#include "ThemeAwareWindow.h"
#include "core/BuildService.h"
#include "core/EventBus.h"
#include "ui/OutputPanelRenderer.h"
#include <wx/timer.h>

namespace markamp::ui
{

class BuildPanel : public ThemeAwareWindow
{
public:
    BuildPanel(wxWindow* parent,
               core::ThemeEngine& theme_engine,
               core::EventBus& event_bus,
               core::BuildService& build_service);

    void Clear();

    static constexpr int kActionBarHeight = 32;
    static constexpr int kProgressBarHeight = 4;

protected:
    void OnThemeChanged(const core::Theme& new_theme) override;

private:
    core::EventBus& event_bus_;
    core::BuildService& build_service_;
    OutputPanelRenderer renderer_;

    // Build output
    struct BuildOutputLine
    {
        std::string text;
        std::string ansi_text;
        bool is_error{false};
        bool is_warning{false};
    };
    std::vector<BuildOutputLine> output_lines_;
    int scroll_offset_{0};
    bool auto_scroll_{true};

    // Progress
    bool build_active_{false};
    int progress_completed_{0};
    int progress_total_{0};
    std::string current_file_;
    double elapsed_seconds_{0.0};

    // Summary
    int error_count_{0};
    int warning_count_{0};

    // Subscriptions
    core::Subscription build_started_sub_;
    core::Subscription build_progress_sub_;
    core::Subscription build_finished_sub_;
    core::Subscription build_output_sub_;

    void OnPaint(wxPaintEvent& event);
    void OnSize(wxSizeEvent& event);
    void OnMouseWheel(wxMouseEvent& event);
    void OnMouseDown(wxMouseEvent& event);

    void DrawActionBar(wxDC& dc);
    void DrawProgressBar(wxDC& dc);
    void DrawOutput(wxDC& dc);
    void DrawSummary(wxDC& dc);

    // Action handlers
    void OnBuild();
    void OnRebuild();
    void OnClean();
    void OnConfigure();
    void OnStop();
};

} // namespace markamp::ui
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/BuildPanel.h` (create)
- `/Users/ryanrentfro/code/markamp/src/ui/BuildPanel.cpp` (create)

**Acceptance Criteria:**
- Action bar at the top with Build, Rebuild, Clean, Configure, Stop buttons
- Progress bar below action bar shows build progress (thin 4px bar)
- Output area renders build output with ANSI colors
- Error lines are highlighted with a subtle red background
- Warning lines are highlighted with a subtle yellow background
- Summary bar at the bottom shows: error count, warning count, build time
- Clicking on an error line navigates to the file:line in the editor
- Build button changes to Stop button when a build is active

**Dependencies:** Task 4, Task 5, Phase 22 Task 11 (OutputPanelRenderer)

---

### Task 7: Implement Compiler Error Navigation

**Title:** Click on parsed errors to navigate to file:line in the editor

**Description:** When the compiler error parser identifies an error with a file:line, that line in the build output becomes a clickable link. Clicking it opens the file and navigates to the error line, identical to the terminal link detection behavior.

**Implementation Details:**
```cpp
void BuildPanel::OnMouseDown(wxMouseEvent& event)
{
    auto line_index = HitTestLine(event.GetPosition());
    if (line_index >= 0 && output_lines_[line_index].is_error)
    {
        auto parsed = error_parser_.parse_line(output_lines_[line_index].text);
        if (parsed.has_value())
        {
            event_bus_.publish(core::events::NavigateToProblemEvent{
                .file_uri = parsed->file_path,
                .line = parsed->line,
                .character = parsed->column
            });
        }
    }
}
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/BuildPanel.cpp`

**Acceptance Criteria:**
- Error lines with file:line references are styled as links (underline on hover)
- Clicking navigates to the file and line in the editor
- If the file is already open, the existing tab is activated
- Navigation works for GCC, Clang, and MSVC error formats
- Cmd+Click also works (consistent with terminal link behavior)

**Dependencies:** Task 2, Task 6

---

### Task 8: Implement Task List Panel

**Title:** Create a sidebar panel showing available tasks from tasks.json

**Description:** Build a `TaskListPanel` that displays all configured tasks grouped by category (Build, Test, Deploy, Other). Each task shows its name, command, and a play button to run it.

**Implementation Details:**
Create `/Users/ryanrentfro/code/markamp/src/ui/TaskListPanel.h`:
```cpp
#pragma once
#include "ThemeAwareWindow.h"
#include "core/TaskConfig.h"
#include "core/TaskRunner.h"
#include "core/EventBus.h"

namespace markamp::ui
{

class TaskListPanel : public ThemeAwareWindow
{
public:
    TaskListPanel(wxWindow* parent,
                  core::ThemeEngine& theme_engine,
                  core::EventBus& event_bus,
                  core::TaskConfig& config,
                  core::TaskRunner& runner);

    void RefreshTaskList();

    static constexpr int kRowHeight = 28;
    static constexpr int kGroupHeaderHeight = 24;

protected:
    void OnThemeChanged(const core::Theme& new_theme) override;

private:
    core::EventBus& event_bus_;
    core::TaskConfig& config_;
    core::TaskRunner& runner_;

    struct TaskRow
    {
        std::string name;
        std::string label;
        std::string group;
        bool is_running{false};
        bool is_hovered{false};
        wxRect rect;
        wxRect play_rect;
    };

    struct GroupSection
    {
        std::string name;
        bool collapsed{false};
        std::vector<TaskRow> tasks;
    };

    std::vector<GroupSection> groups_;
    int scroll_offset_{0};

    void OnPaint(wxPaintEvent& event);
    void OnMouseDown(wxMouseEvent& event);
    void OnMouseMove(wxMouseEvent& event);
    void OnRightDown(wxMouseEvent& event);

    void RunTask(const std::string& name);
    void StopTask(const std::string& name);
    void EditTask(const std::string& name);
};

} // namespace markamp::ui
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/TaskListPanel.h` (create)
- `/Users/ryanrentfro/code/markamp/src/ui/TaskListPanel.cpp` (create)

**Acceptance Criteria:**
- Tasks are grouped by: Build, Test, Deploy, Other
- Each group has a collapsible header
- Each task shows: play icon, label, and running indicator
- Clicking play runs the task
- Running tasks show a spinner and a stop button
- Double-click opens the task definition in an editor
- Right-click shows context menu: Run, Stop, Edit, Delete

**Dependencies:** Task 3, Task 4

---

### Task 9: Implement Compiler Selection Dropdown

**Title:** Build panel dropdown for selecting the active compiler

**Description:** Add a dropdown in the build panel action bar that lists detected compilers (gcc, clang, msvc) and allows switching between them. The selection updates the `BuildConfiguration` and persists in config.

**Implementation Details:**
Detect compilers by checking standard paths:
```cpp
auto BuildService::detect_compilers() const -> std::vector<std::string>
{
    std::vector<std::string> compilers;
    // Check: /usr/bin/clang++, /usr/bin/g++, /usr/local/bin/g++-13, etc.
    // Also check: which clang++, which g++
    return compilers;
}
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/core/BuildService.h`
- `/Users/ryanrentfro/code/markamp/src/core/BuildService.cpp`
- `/Users/ryanrentfro/code/markamp/src/ui/BuildPanel.cpp`

**Acceptance Criteria:**
- Dropdown lists all detected C++ compilers with version info
- Selecting a compiler updates the build configuration
- Selection persists across sessions
- If no compilers are detected, dropdown shows "No compilers found"
- Default is the system default compiler

**Dependencies:** Task 5, Task 6

---

### Task 10: Implement Build Target Selector

**Title:** Dropdown for selecting the CMake build target

**Description:** For CMake projects, parse the available targets and present them in a dropdown. For Make projects, parse Makefile targets. The selected target is passed to the build command.

**Implementation Details:**
```cpp
auto BuildService::available_targets() const -> std::vector<BuildTarget>
{
    if (build_system_ == BuildSystem::kCMake)
    {
        // Parse: cmake --build build/debug --target help
        // Or read CMake cache for target list
    }
    else if (build_system_ == BuildSystem::kMake)
    {
        // Parse: make -pn | grep "^[a-zA-Z].*:" (simplified)
    }
    return {};
}
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/core/BuildService.cpp`
- `/Users/ryanrentfro/code/markamp/src/ui/BuildPanel.cpp`

**Acceptance Criteria:**
- CMake targets are parsed from the project
- "All" is always available as the default target
- Selecting a target updates the build command
- Target list refreshes after cmake configure
- For Make, common targets (all, clean, install, test) are detected

**Dependencies:** Task 5

---

### Task 11: Implement Build Progress Tracking

**Title:** Track and display parallel build progress

**Description:** Parse build output to track progress. For CMake/ninja, parse the `[N/M]` progress format. For Make, estimate progress from file compilation counts. Display progress as a thin bar and percentage text.

**Implementation Details:**
```cpp
struct BuildProgressInfo
{
    int completed{0};
    int total{0};
    std::string current_file;
    double percentage() const { return total > 0 ? (100.0 * completed / total) : 0.0; }
};

auto parse_progress(const std::string& line) -> std::optional<BuildProgressInfo>
{
    // Ninja format: [42/100] Compiling src/core/EventBus.cpp
    // Make format: Compiling src/core/EventBus.cpp (heuristic)
    static const std::regex ninja_regex(R"(\[(\d+)/(\d+)\]\s*(.*))");
    // ...
}
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/core/BuildService.cpp`
- `/Users/ryanrentfro/code/markamp/src/ui/BuildPanel.cpp`

**Acceptance Criteria:**
- Ninja `[N/M]` progress format is parsed correctly
- Progress bar updates smoothly during build
- Current file name is displayed during compilation
- Progress percentage shows in the action bar
- Build without parseable progress shows an indeterminate animation

**Dependencies:** Task 6

---

### Task 12: Implement Error and Warning Count Summary

**Title:** Display error/warning counts during and after build

**Description:** Track error and warning counts as they are parsed from build output. Display a summary in the build panel footer and publish to the status bar.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/BuildPanel.cpp`
- `/Users/ryanrentfro/code/markamp/src/ui/StatusBarPanel.cpp`

**Acceptance Criteria:**
- Error count updates in real-time as errors are parsed
- Warning count updates in real-time
- Summary bar shows: "Build: 3 errors, 12 warnings (5.2s)"
- Status bar shows build status indicator during active builds
- Zero errors shows success indicator (green check)
- One or more errors shows failure indicator (red X)

**Dependencies:** Task 2, Task 6

---

### Task 13: Implement Build Time Display

**Title:** Show elapsed build time in the summary

**Description:** Track build start time, display elapsed time during build, and show total time after completion.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/BuildPanel.cpp`

**Acceptance Criteria:**
- Elapsed time displays during build (updated every second)
- Format: "0:05" for under a minute, "1:23" for over a minute, "1:23:45" for over an hour
- Total build time shows in the summary after build completes
- Timer stops when build is cancelled

**Dependencies:** Task 6

---

### Task 14: Implement Auto-Build on Save

**Title:** Optional automatic build trigger when a file is saved

**Description:** Add a configuration option to trigger the default build task automatically when any source file is saved. Debounce rapid saves (500ms). Only trigger if no build is currently running.

**Implementation Details:**
Subscribe to `FileSavedEvent` in `BuildService`:
```cpp
save_sub_ = event_bus_.subscribe<events::FileSavedEvent>(
    [this](const auto& e)
    {
        if (auto_build_enabled_ && !is_building())
        {
            auto_build_timer_.StartOnce(500); // Debounce
        }
    });
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/core/BuildService.h`
- `/Users/ryanrentfro/code/markamp/src/core/BuildService.cpp`
- `/Users/ryanrentfro/code/markamp/src/core/Config.cpp`

**Acceptance Criteria:**
- Auto-build is configurable (default: off)
- 500ms debounce prevents rapid-fire builds
- Does not trigger if a build is already running
- Only triggers for files matching a configurable glob pattern
- Setting persists in config

**Dependencies:** Task 5

---

### Task 15: Implement Build Before Run Integration

**Title:** Ensure build succeeds before executing a run configuration

**Description:** When the user clicks Run (Phase 26), check if a build is needed and run the default build task first. If the build fails, abort the run and show the build errors. This is a pre-requisite hook that other features can register.

**Implementation Details:**
```cpp
class BuildService
{
public:
    using PreRunHook = std::function<bool()>; // return false to abort run
    auto build_before_run() -> bool; // Build, return true if successful
};
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/core/BuildService.h`
- `/Users/ryanrentfro/code/markamp/src/core/BuildService.cpp`

**Acceptance Criteria:**
- `build_before_run()` runs the default build task and blocks until completion
- Returns `true` if build succeeds (exit code 0, no errors)
- Returns `false` if build fails
- Caller (Run configuration) checks return value before proceeding
- Build output is visible in the build panel during the pre-run build

**Dependencies:** Task 5

---

### Task 16: Implement CMake Integration

**Title:** Deep CMake integration for configure, presets, and targets

**Description:** Add CMake-specific features: preset parsing from CMakePresets.json, automatic configure detection, target discovery, and CMake cache variable inspection.

**Implementation Details:**
```cpp
auto BuildService::available_presets() const -> std::vector<std::string>
{
    // Parse CMakePresets.json from workspace root
    auto presets_path = workspace_root_ + "/CMakePresets.json";
    // Read and parse JSON for configurePresets[].name and buildPresets[].name
}
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/core/BuildService.cpp`

**Acceptance Criteria:**
- CMakePresets.json is parsed for configure and build presets
- Preset selection is available in the build panel dropdown
- `cmake --build <dir> --target help` output is parsed for targets
- CMake configure step uses the selected preset
- Missing CMakePresets.json falls back to manual configuration

**Dependencies:** Task 5

---

### Task 17: Implement Make Integration

**Title:** Makefile target detection and build command generation

**Description:** For workspaces with a Makefile, detect available targets, support `make -j` for parallel builds, and parse Make-style output.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/core/BuildService.cpp`

**Acceptance Criteria:**
- Detects Makefile in workspace root
- Parses common targets: all, clean, install, test
- Uses `-j` flag with detected CPU count for parallel builds
- Make output is parsed for compiler errors (GCC/Clang format)

**Dependencies:** Task 5

---

### Task 18: Implement Custom Build Commands

**Title:** Support arbitrary build commands in task definitions

**Description:** Allow users to define custom build commands beyond CMake/Make, such as `cargo build`, `go build`, `npm run build`, etc. These commands are defined in tasks.json with a `"type": "shell"` entry.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/core/TaskConfig.h`
- `/Users/ryanrentfro/code/markamp/src/core/TaskRunner.cpp`

**Acceptance Criteria:**
- Custom commands execute in the workspace directory
- Output is captured and displayed in the build panel
- Error parsing uses the specified problem matcher or none
- Environment variables from tasks.json are passed to the process
- Shell expansion works (e.g., `$HOME`, `$(pwd)`)

**Dependencies:** Task 3, Task 4

---

### Task 19: Implement Task Templates

**Title:** Provide pre-built task templates for common build systems

**Description:** Offer task templates that the user can select when creating a new task: CMake Build, CMake Test, Make Build, Cargo Build, Go Build, npm Build, Custom Shell Command.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/core/TaskConfig.cpp`

**Acceptance Criteria:**
- At least 6 task templates are available
- Each template fills in reasonable defaults
- Templates adapt to detected project type where possible
- Template selection UI is accessible from the task list panel

**Dependencies:** Task 3

---

### Task 20: Implement Task Groups

**Title:** Group tasks by category (build/test/deploy) with group-level actions

**Description:** Tasks are assigned to groups. Groups can be run as a whole (e.g., "Run all test tasks"). The default build group task is what Cmd+B triggers.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/core/TaskRunner.h`
- `/Users/ryanrentfro/code/markamp/src/core/TaskRunner.cpp`
- `/Users/ryanrentfro/code/markamp/src/ui/TaskListPanel.cpp`

**Acceptance Criteria:**
- Tasks are visually grouped in the task list
- Group headers show the group name and task count
- "Run All in Group" runs all tasks in sequence
- Default build task in the "build" group is triggered by Cmd+B
- Default test task in the "test" group is triggered by Cmd+Shift+T

**Dependencies:** Task 3, Task 8

---

### Task 21: Integrate Build Panel into LayoutManager

**Title:** Wire BuildPanel and TaskListPanel into the layout

**Description:** Add the build panel as a bottom panel tab and the task list panel as a sidebar panel option.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.h`
- `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`
- `/Users/ryanrentfro/code/markamp/src/app/MarkAmpApp.cpp`

**Acceptance Criteria:**
- Build panel is a tab in the bottom panel area
- Task list panel is an option in the sidebar
- Both are lazily created on first access
- Cmd+B triggers build and reveals the build panel
- Layout state persists across sessions

**Dependencies:** Task 6, Task 8

---

### Task 22: Implement Status Bar Build Indicator

**Title:** Show build status in the status bar

**Description:** Display a build status indicator in the status bar: spinner during build, green check on success, red X on failure, with error/warning counts.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/StatusBarPanel.h`
- `/Users/ryanrentfro/code/markamp/src/ui/StatusBarPanel.cpp`

**Acceptance Criteria:**
- Spinner animation during active build
- Green check icon after successful build
- Red X icon after failed build
- Error and warning counts shown next to the icon
- Clicking the indicator opens the build panel
- Status clears after 30 seconds or on next build start

**Dependencies:** Task 1

---

### Task 23: Update CMakeLists.txt

**Title:** Add all new build and task runner source files to the build

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/CMakeLists.txt`

**Acceptance Criteria:**
- All new `.h` and `.cpp` files in both `add_executable()` and `source_group()`
- Build succeeds with no linker errors

**Dependencies:** All create tasks

---

### Task 24: Write Unit Tests for CompilerErrorParser

**Title:** Test GCC, Clang, and MSVC error format parsing

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/tests/unit/test_compiler_error_parser.cpp` (create)

**Acceptance Criteria:**
- Tests parse GCC error: `file.cpp:10:5: error: undeclared identifier`
- Tests parse GCC warning: `file.cpp:10:5: warning: unused variable [-Wunused-variable]`
- Tests parse Clang error with flag: `file.cpp:10:5: error: use of undeclared identifier 'x' [-Werror]`
- Tests parse MSVC error: `file.cpp(10): error C2065: 'x': undeclared identifier`
- Tests parse CMake error: `CMake Error at CMakeLists.txt:10`
- Tests verify context ("note:") lines attach to previous error
- Tests verify `to_diagnostics()` groups by file correctly
- All tests pass

**Dependencies:** Task 2

---

### Task 25: Write Unit Tests for TaskRunner and TaskConfig

**Title:** Test task configuration parsing and task execution lifecycle

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/tests/unit/test_task_runner.cpp` (create)

**Acceptance Criteria:**
- Tests verify JSON task parsing with all fields
- Tests verify default values for optional fields
- Tests verify task templates generate valid configurations
- Tests verify task group filtering
- Tests verify `find_task()` and `default_build_task()`
- Tests verify task start/stop lifecycle events
- All tests pass

**Dependencies:** Task 3, Task 4

---

### Task 26: Write Integration Tests for Build Panel

**Title:** Test build event flow from service through to panel

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/tests/unit/test_build_panel.cpp` (create)

**Acceptance Criteria:**
- Tests verify `BuildStartedEvent` triggers panel to show
- Tests verify `BuildProgressEvent` updates progress bar
- Tests verify `BuildFinishedEvent` shows summary with counts
- Tests verify compiler errors are published to DiagnosticsService
- All tests pass

**Dependencies:** Task 1, Task 5

---

### Task 27: Documentation

**Title:** Document build and task runner architecture

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/docs/architecture.md`

**Acceptance Criteria:**
- BuildService architecture documented
- TaskRunner lifecycle documented
- CompilerErrorParser format support documented
- tasks.json schema documented
- CMake and Make integration documented

**Dependencies:** All previous tasks

## Testing Requirements

- Unit tests for CompilerErrorParser covering GCC, Clang, MSVC, and CMake formats
- Unit tests for TaskConfig parsing and templates
- Integration tests for build event flow
- Manual testing with real CMake project builds
- Performance: build output rendering keeps up with fast parallel builds

## Phase Completion Criteria

- Build panel displays colorized build output with error/warning highlighting
- Compiler errors are parsed and published to the problems panel
- Clicking errors navigates to the file:line in the editor
- Task configuration supports CMake, Make, and custom commands
- Task list panel shows available tasks grouped by category
- Build progress is tracked and displayed
- All unit and integration tests pass
