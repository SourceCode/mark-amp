# Phase 19: Run and Debug Panel

## Objective

Build a run and debug panel that provides launch configuration management, program execution, debug controls, breakpoint management, call stack viewing, variable inspection, and debug console output. The codebase currently has no run/debug infrastructure. This phase introduces the complete debug experience from scratch, targeting C++ debugging via LLDB/GDB integration and extensible to other languages via DAP (Debug Adapter Protocol).

## Prerequisites

- Phase 14 Task 5 (breakpoint indicators in gutter)
- Phase 14 Task 23 (conditional breakpoint dialog)
- `EditorPanel` with breakpoint toggle support
- `OutputPanel` at `src/ui/OutputPanel.h` for console output
- `EventBus` and `ThemeEngine`
- `SidebarMode` enum (needs `kDebug` entry if not present)
- Bottom panel notebook for debug console output

## Deliverables

A complete run and debug system with 25+ tasks covering launch configs, debug controls, breakpoints, call stack, variables, and debug console.

---

## Task 1: Debug Sidebar Panel Shell

**Title:** Create the Run and Debug sidebar panel

**Description:** Create the main sidebar panel for run/debug functionality with sections for launch configuration, breakpoints, call stack, and variables.

**Implementation Details:**
- Create `src/ui/RunDebugPanel.h` / `.cpp`:
  ```cpp
  class RunDebugPanel : public wxPanel {
  public:
      RunDebugPanel(wxWindow* parent, core::ThemeEngine& theme_engine,
                    core::EventBus& event_bus);
  private:
      wxPanel* launch_config_area_;     // Configuration selector
      wxPanel* debug_controls_area_;    // Play/pause/step buttons
      wxScrolledWindow* variables_area_; // Variable inspector
      wxScrolledWindow* callstack_area_; // Call stack view
      wxScrolledWindow* breakpoints_area_; // Breakpoints list
  };
  ```
- Add `kDebug` to `SidebarMode` enum if not present
- Register with `SidebarPanelRegistry`
- Panel sections are collapsible

**Files Affected:**
- `src/ui/RunDebugPanel.h` (new)
- `src/ui/RunDebugPanel.cpp` (new)
- `src/ui/SidebarMode.h` (add kDebug if needed)
- `src/ui/LayoutManager.cpp` (register panel)
- `CMakeLists.txt`

**Acceptance Criteria:**
- Panel visible when debug sidebar selected
- All sections present and collapsible
- Registered in sidebar panel registry
- Proper theming

**Dependencies:** None

---

## Task 2: Launch Configuration Data Model

**Title:** Define launch configuration schema

**Description:** Define the data model for launch configurations that specify how to build, run, and debug programs.

**Implementation Details:**
- Create `src/core/LaunchConfig.h`:
  ```cpp
  struct LaunchConfig {
      std::string name;           // "Debug MarkAmp"
      std::string type;           // "cppdbg", "python", "node"
      std::string request;        // "launch" or "attach"
      std::string program;        // path to executable
      std::vector<std::string> args; // command line arguments
      std::string cwd;            // working directory
      std::map<std::string, std::string> env; // environment variables
      std::string pre_launch_task; // build task to run first
      bool stop_on_entry{false};
      int port{0};                // for attach mode

      // C++ specific
      std::string debugger;       // "lldb" or "gdb"
      std::string mi_mode;        // "lldb" or "gdb"
  };
  ```
- Store configurations in `.markamp/launch.json`:
  ```json
  {
      "version": "0.2.0",
      "configurations": [
          {
              "name": "Debug MarkAmp",
              "type": "cppdbg",
              "request": "launch",
              "program": "${workspaceFolder}/build/debug/markamp",
              "args": [],
              "cwd": "${workspaceFolder}",
              "preLaunchTask": "cmake-build-debug"
          }
      ]
  }
  ```
- Variable substitution: `${workspaceFolder}`, `${file}`, `${fileBasename}`

**Files Affected:**
- `src/core/LaunchConfig.h` (new)
- `src/core/LaunchConfig.cpp` (new -- JSON parsing)
- `CMakeLists.txt`

**Acceptance Criteria:**
- Launch config schema defined
- JSON parsing/serialization works
- Variable substitution functional
- Multiple configurations supported
- Default configuration auto-generated

**Dependencies:** None

---

## Task 3: Launch Configuration Selector

**Title:** Configuration dropdown in the debug panel

**Description:** Show a dropdown selector at the top of the debug panel for choosing which launch configuration to use. Include "Add Configuration..." option.

**Implementation Details:**
- Dropdown showing all configurations from `launch.json`
- Green play button next to dropdown to start debugging
- "Add Configuration..." option opens the JSON editor with templates
- Configuration templates for common setups:
  - C++ (LLDB)
  - C++ (GDB)
  - Python
  - Node.js
  - Attach to Process
- Selected configuration persists across sessions

**Files Affected:**
- `src/ui/RunDebugPanel.cpp` (implement configuration selector)

**Acceptance Criteria:**
- All configurations shown in dropdown
- Play button starts selected configuration
- "Add Configuration" opens editor
- Templates available for quick setup
- Selection persists

**Dependencies:** Task 2

---

## Task 4: Build Before Run

**Title:** Execute build task before launching debugger

**Description:** When a launch configuration specifies a `preLaunchTask`, run that build task first and only proceed to debug if the build succeeds.

**Implementation Details:**
- Build task execution:
  ```cpp
  void ExecutePreLaunchTask(const std::string& task_name, std::function<void(bool)> callback) {
      // Find task in tasks.json or use cmake preset
      auto command = ResolveBuildCommand(task_name);
      RunProcessAsync(command, [callback](int exit_code) {
          callback(exit_code == 0);
      });
  }
  ```
- Build output routed to the Output panel's "Build" channel
- On build failure: show error notification, do not launch debugger
- "Build & Run" vs "Run Without Building" options

**Files Affected:**
- `src/core/TaskRunner.h` (new)
- `src/core/TaskRunner.cpp` (new)
- `src/ui/RunDebugPanel.cpp` (wire pre-launch build)
- `CMakeLists.txt`

**Acceptance Criteria:**
- Pre-launch task executes before debug
- Build output visible in Output panel
- Build failure prevents debug launch
- Success proceeds to debug
- Run without building option available

**Dependencies:** Task 3

---

## Task 5: Debug Adapter Protocol Client

**Title:** Implement DAP client for debugger communication

**Description:** Create a Debug Adapter Protocol (DAP) client that communicates with debug adapters (like lldb-vscode or cppdbg) over stdin/stdout. This is the foundation for all debug operations.

**Implementation Details:**
- Create `src/core/DebugAdapterClient.h` / `.cpp`:
  ```cpp
  class DebugAdapterClient {
  public:
      void Initialize();
      void Launch(const LaunchConfig& config);
      void Attach(const LaunchConfig& config);
      void Disconnect();

      // Execution control
      void Continue();
      void Pause();
      void StepOver();
      void StepInto();
      void StepOut();
      void Terminate();

      // Breakpoints
      void SetBreakpoints(const std::string& file, const std::vector<int>& lines);
      void SetConditionalBreakpoint(const std::string& file, int line, const std::string& condition);

      // Inspection
      auto GetCallStack(int thread_id) -> std::vector<StackFrame>;
      auto GetVariables(int scope_id) -> std::vector<Variable>;
      auto Evaluate(const std::string& expression) -> std::string;

      // Events
      using StoppedCallback = std::function<void(int line, const std::string& reason)>;
      using OutputCallback = std::function<void(const std::string& category, const std::string& text)>;
      void SetOnStopped(StoppedCallback callback);
      void SetOnOutput(OutputCallback callback);
  };
  ```
- DAP communication: JSON-RPC over stdin/stdout pipes
- Launch debug adapter process as a child process
- Parse DAP protocol messages (Content-Length header + JSON body)

**Files Affected:**
- `src/core/DebugAdapterClient.h` (new)
- `src/core/DebugAdapterClient.cpp` (new)
- `CMakeLists.txt`

**Acceptance Criteria:**
- DAP protocol messages sent/received correctly
- Launch and attach modes work
- Execution control commands work
- Breakpoint setting works
- Call stack and variables retrievable
- Events (stopped, output) delivered to callbacks

**Dependencies:** Task 2

---

## Task 6: Start/Stop/Restart Buttons

**Title:** Implement debug control buttons

**Description:** Show debug control buttons in a floating toolbar when debugging is active: Continue/Pause, Step Over, Step Into, Step Out, Restart, Stop.

**Implementation Details:**
- Debug toolbar: floating panel at top-center of editor area (above breadcrumbs):
  ```
  [Continue/Pause] [Step Over] [Step Into] [Step Out] [Restart] [Stop]
  ```
- Button states:
  - Not debugging: toolbar hidden
  - Running: Pause button active
  - Stopped/breakpoint: Continue button active, step buttons active
- Icon-based buttons with tooltips
- Keyboard shortcuts: F5 (Continue), F10 (Step Over), F11 (Step Into), Shift+F11 (Step Out), Shift+F5 (Stop)

**Files Affected:**
- `src/ui/DebugToolbar.h` (new)
- `src/ui/DebugToolbar.cpp` (new)
- `src/ui/LayoutManager.cpp` (show/hide debug toolbar)
- `CMakeLists.txt`

**Acceptance Criteria:**
- Toolbar appears when debugging starts
- Disappears when debugging stops
- All buttons functional
- Keyboard shortcuts work
- Correct button states based on debug state

**Dependencies:** Task 5

---

## Task 7: Breakpoints List Panel

**Title:** Display all breakpoints in the debug panel

**Description:** Show a list of all breakpoints across all files in the debug panel, with ability to enable/disable, edit conditions, and remove them.

**Implementation Details:**
- Breakpoints section in RunDebugPanel:
  ```
  BREAKPOINTS
  [x] src/ui/TabBar.cpp:42
  [x] src/ui/TabBar.cpp:150  (condition: idx > 0)
  [ ] src/core/EventBus.cpp:88  (disabled)
  [x] src/ui/EditorPanel.cpp:220  (log: "x = {x}")
  ```
- Checkbox: enable/disable without removing
- Click: navigate to breakpoint location
- Right-click: Edit Condition, Remove, Remove All
- Toggle all: master enable/disable checkbox
- Sync with editor gutter breakpoints (Phase 14 Task 5)

**Files Affected:**
- `src/ui/RunDebugPanel.cpp` (implement breakpoints list)

**Acceptance Criteria:**
- All breakpoints listed with file:line
- Conditional breakpoints show condition
- Log points show message
- Enable/disable via checkbox
- Click navigates to breakpoint
- Synchronized with editor gutter

**Dependencies:** Phase 14 Task 5

---

## Task 8: Call Stack View

**Title:** Display the call stack when stopped at a breakpoint

**Description:** Show the call stack (stack frames) when the debugger is paused, with the current frame highlighted and ability to navigate between frames.

**Implementation Details:**
- Call stack section in RunDebugPanel:
  ```
  CALL STACK
  > TabBar::DrawTab (TabBar.cpp:450)       [current frame]
    TabBar::OnPaint (TabBar.cpp:325)
    wxWindowBase::OnPaint (wxwidgets)
    ...
  ```
- Current frame: bold with accent indicator
- Click frame: navigate to that source location, update variables
- External/library frames: dimmed, collapsible
- Thread selector: if multi-threaded, show thread dropdown
- Frame limit: show first 50 frames, "Show More" for rest

**Files Affected:**
- `src/ui/RunDebugPanel.cpp` (implement call stack view)

**Acceptance Criteria:**
- Stack frames displayed when paused
- Current frame highlighted
- Click navigates to source
- Library frames dimmed
- Thread selection available
- Variables update on frame selection

**Dependencies:** Task 5

---

## Task 9: Variables View (Locals)

**Title:** Display local variables at current breakpoint

**Description:** Show local variables, their types, and values in a tree view. Support expanding complex types (structs, arrays, pointers).

**Implementation Details:**
- Variables section with expandable tree:
  ```
  VARIABLES
  Locals
    tab_index: int = 3
    > tab: TabInfo
      file_path: string = "/path/to/file.md"
      is_modified: bool = true
      > rect: wxRect
        x: int = 120
        y: int = 0
        width: int = 150
        height: int = 36
    scroll_offset_: int = 40
  ```
- Types shown in muted color after variable name
- Values shown in accent color
- Complex types expandable (lazy-loaded from debugger)
- Right-click: "Copy Value", "Copy Expression", "Add to Watch"
- Editable values: double-click to modify variable value

**Files Affected:**
- `src/ui/RunDebugPanel.cpp` (implement variables tree)

**Acceptance Criteria:**
- Local variables displayed with names, types, values
- Complex types expandable
- Values update on step/continue
- Copy and watch actions available
- Values editable
- Types properly formatted

**Dependencies:** Task 5

---

## Task 10: Watch Variables

**Title:** Allow adding custom watch expressions

**Description:** Add a "Watch" section where users can add custom expressions to evaluate at each breakpoint stop.

**Implementation Details:**
- Watch section below locals:
  ```
  WATCH
  tabs_.size()          = 5
  active_file_path_     = "/path/to/file.md"
  config_->get("theme") = "dark"
  + Add Expression...
  ```
- "Add Expression" input: type any expression to evaluate
- Expressions evaluated via DAP evaluate request
- Red text for invalid/unevaluable expressions
- Persist watch expressions per workspace
- Right-click: "Edit Expression", "Remove", "Copy Value"

**Files Affected:**
- `src/ui/RunDebugPanel.cpp` (implement watch section)

**Acceptance Criteria:**
- Custom expressions evaluable
- Values update on debug stops
- Invalid expressions show error
- Add/edit/remove expressions
- Persistent per workspace

**Dependencies:** Task 5

---

## Task 11: Debug Console Output

**Title:** Route debug output to a dedicated console panel

**Description:** Create a "Debug Console" tab in the bottom panel that shows debugger output, program stdout/stderr, and provides an interactive REPL for evaluating expressions during debugging.

**Implementation Details:**
- Add "Debug Console" tab to bottom panel notebook
- Output categories:
  - Debugger messages: gray
  - Program stdout: white
  - Program stderr: red
  - Evaluation results: green
- Interactive input at bottom:
  - Type expression and press Enter to evaluate
  - History: up/down arrows for previous expressions
  - Auto-complete for variable names
- Clear console button
- Copy output to clipboard

**Files Affected:**
- `src/ui/DebugConsolePanel.h` (new)
- `src/ui/DebugConsolePanel.cpp` (new)
- `src/ui/LayoutManager.cpp` (add to bottom panel)
- `CMakeLists.txt`

**Acceptance Criteria:**
- Debug output routed to console panel
- Color-coded by category
- Interactive expression evaluation
- History navigation
- Clear and copy functions

**Dependencies:** Task 5

---

## Task 12: Current Execution Line Indicator

**Title:** Highlight the current execution line in the editor

**Description:** When the debugger is stopped at a breakpoint or step, highlight the current execution line with a yellow/accent arrow indicator in the gutter and a full-width background highlight.

**Implementation Details:**
- In EditorPanel, when debug stops at a line:
  ```cpp
  void ShowExecutionLine(const std::string& file, int line) {
      // Add yellow arrow marker in gutter
      editor_->MarkerAdd(line, kMarkerCurrentExecution);
      // Set caret line background to execution highlight
      editor_->SetCaretLineBackground(execution_highlight_color);
      // Scroll to make line visible
      editor_->GotoLine(line);
      editor_->EnsureVisibleEnforcePolicy(line);
  }
  ```
- Yellow arrow (>) in gutter margin
- Full-width yellow/amber background tint
- Clear indicator when continuing execution

**Files Affected:**
- `src/ui/EditorPanel.h` (add execution line methods)
- `src/ui/EditorPanel.cpp` (implement execution line indicator)

**Acceptance Criteria:**
- Yellow arrow in gutter at execution line
- Background highlight across full line width
- Scrolls to execution line when stopped
- Clears when continuing
- Opens file if not already open

**Dependencies:** Phase 14 Task 1

---

## Task 13: Conditional Breakpoint Integration

**Title:** Wire conditional breakpoints to DAP

**Description:** Connect the conditional breakpoint UI (Phase 14 Task 23) to the DAP client so conditions are sent to the debugger.

**Implementation Details:**
- When breakpoints are set via the editor gutter, send to DAP:
  ```cpp
  void SyncBreakpoints(const std::string& file) {
      std::vector<DAPBreakpoint> breakpoints;
      for (const auto& bp : GetBreakpointsForFile(file)) {
          DAPBreakpoint dap_bp;
          dap_bp.line = bp.line;
          dap_bp.condition = bp.condition;
          dap_bp.log_message = bp.log_message;
          breakpoints.push_back(dap_bp);
      }
      dap_client_->SetBreakpoints(file, breakpoints);
  }
  ```
- Verified breakpoints: debugger responds with adjusted line numbers (e.g., moved to next valid line)
- Unverified breakpoints: shown as hollow circles
- Update editor gutter to reflect verified positions

**Files Affected:**
- `src/ui/RunDebugPanel.cpp` (sync breakpoints to DAP)
- `src/core/DebugAdapterClient.cpp` (send breakpoint requests)

**Acceptance Criteria:**
- Conditional breakpoints sent to debugger
- Log points sent as logMessage
- Verified breakpoints show solid dot
- Unverified show hollow dot
- Line adjustments reflected in editor

**Dependencies:** Phase 14 Task 23, Task 5

---

## Task 14: Run Without Debugging

**Title:** Support running the program without debugger attachment

**Description:** Add "Run Without Debugging" option that simply executes the program without debug adapter attachment, routing output to the Output panel.

**Implementation Details:**
- "Run Without Debugging" button (or Ctrl+F5):
  1. Execute pre-launch build task (if configured)
  2. Launch program as child process (no debug adapter)
  3. Route stdout/stderr to Output panel "Program Output" channel
  4. Show running indicator in status bar
  5. Stop button to terminate process
- Process management: track PID, handle signals

**Files Affected:**
- `src/core/ProcessRunner.h` (new -- generic process execution)
- `src/core/ProcessRunner.cpp` (new)
- `src/ui/RunDebugPanel.cpp` (add run without debug option)
- `CMakeLists.txt`

**Acceptance Criteria:**
- Program runs without debugger
- Output visible in Output panel
- Stop button terminates process
- Exit code shown on completion
- Ctrl+F5 shortcut

**Dependencies:** Task 4

---

## Task 15: Configuration Editor

**Title:** Provide UI for editing launch configurations

**Description:** When "Add Configuration" or "Edit Configuration" is selected, open the `launch.json` file in the editor with auto-complete support for known fields.

**Implementation Details:**
- Open `.markamp/launch.json` in a dedicated editor tab
- Provide JSON schema auto-complete (future):
  - Field name suggestions after "{"
  - Value suggestions for known fields (type, debugger, etc.)
- Template insertion: snippets for common configurations
- Validation: check required fields, highlight errors
- Create `.markamp/` directory and default `launch.json` if not exists

**Files Affected:**
- `src/ui/RunDebugPanel.cpp` (open config file)
- `src/core/LaunchConfig.cpp` (create default config)

**Acceptance Criteria:**
- Launch.json opens in editor
- Default config created if missing
- Templates available for quick setup
- Directory created if needed

**Dependencies:** Task 2

---

## Task 16: Compiler Output Routing

**Title:** Route compiler output to the Problems panel

**Description:** Parse compiler output (gcc/clang format) and route errors/warnings to the Problems panel with clickable navigation.

**Implementation Details:**
- Parse compiler output lines matching patterns:
  ```
  src/ui/TabBar.cpp:42:10: error: use of undeclared identifier 'foo'
  src/ui/TabBar.cpp:45:5: warning: unused variable 'x' [-Wunused-variable]
  ```
- Extract: file path, line, column, severity, message
- Send to ProblemsPanel as diagnostic entries
- Click on problem: open file and navigate to line/column
- Clear problems on successful build

**Files Affected:**
- `src/core/CompilerOutputParser.h` (new)
- `src/core/CompilerOutputParser.cpp` (new)
- `src/ui/ProblemsPanel.cpp` (accept parsed diagnostics)
- `CMakeLists.txt`

**Acceptance Criteria:**
- Compiler errors parsed correctly (gcc/clang format)
- Errors and warnings show in Problems panel
- Click navigates to source location
- Problems cleared on successful build
- Supports multiple compiler output formats

**Dependencies:** Task 4

---

## Task 17: Debug State Management

**Title:** Track and publish debug session state

**Description:** Create a centralized debug state manager that tracks the current debug session state and publishes events for UI components to react to.

**Implementation Details:**
- Add `enum class DebugState { Inactive, Starting, Running, Paused, Stopped }`
- Create `DebugSessionManager` that tracks:
  ```cpp
  struct DebugSession {
      DebugState state{DebugState::Inactive};
      LaunchConfig config;
      int active_thread_id{0};
      int active_frame_id{0};
      std::string stopped_file;
      int stopped_line{0};
      std::string stopped_reason; // "breakpoint", "step", "exception"
  };
  ```
- Events:
  ```cpp
  MARKAMP_DECLARE_EVENT(DebugStateChangedEvent, DebugState state);
  MARKAMP_DECLARE_EVENT(DebugStoppedEvent, std::string file, int line, std::string reason);
  MARKAMP_DECLARE_EVENT(DebugOutputEvent, std::string category, std::string text);
  ```
- All UI components subscribe to these events

**Files Affected:**
- `src/core/DebugSessionManager.h` (new)
- `src/core/DebugSessionManager.cpp` (new)
- `src/core/Events.h` (add debug events)
- `CMakeLists.txt`

**Acceptance Criteria:**
- Debug state tracked centrally
- State change events published
- All UI components react to state changes
- Session data accessible for inspection

**Dependencies:** Task 5

---

## Task 18: Inline Variable Values

**Title:** Show variable values inline in the editor during debugging

**Description:** When paused at a breakpoint, show variable values inline after each variable reference in the source code, similar to VSCode's inline values.

**Implementation Details:**
- When stopped, for each variable in the current scope:
  - Find occurrences of the variable name in the visible lines
  - Show the value as a ghost text annotation after the reference:
    ```
    int tab_index = FindTabIndex(file_path);  // tab_index = 3
    if (tab_index >= 0) {                     // true
    ```
  - Use Scintilla annotations or end-of-line decorations
  - Gray/muted color, italic
- Toggle via settings: "Debug: Inline Values"
- Clear when continuing or ending debug session

**Files Affected:**
- `src/ui/EditorPanel.h` (add inline value display)
- `src/ui/EditorPanel.cpp` (implement inline values)

**Acceptance Criteria:**
- Variable values shown inline
- Values update on step/frame change
- Muted styling, not intrusive
- Togglable via settings
- Cleared on continue/stop

**Dependencies:** Tasks 5, 9

---

## Task 19: Exception Breakpoints

**Title:** Support breaking on exceptions

**Description:** Add the ability to break on thrown exceptions (caught and uncaught). Configurable in the breakpoints section.

**Implementation Details:**
- Add "Exception Breakpoints" section in breakpoints list:
  ```
  EXCEPTION BREAKPOINTS
  [x] C++ Exceptions (uncaught)
  [ ] C++ Exceptions (all)
  ```
- Send exception breakpoint filters to DAP:
  ```cpp
  dap_client_->SetExceptionBreakpoints({"uncaught", "caught"});
  ```
- When exception caught: show exception details in Variables view

**Files Affected:**
- `src/ui/RunDebugPanel.cpp` (add exception breakpoint UI)
- `src/core/DebugAdapterClient.cpp` (send exception breakpoint config)

**Acceptance Criteria:**
- Exception breakpoint toggles available
- Breaks on uncaught exceptions
- Exception details visible
- Configurable caught/uncaught

**Dependencies:** Task 5

---

## Task 20: Debug Status Bar Integration

**Title:** Show debug status in the status bar

**Description:** Display debug session status in the status bar: "Debugging (paused at line 42)" with debug-specific coloring.

**Implementation Details:**
- Status bar changes during debugging:
  - Background color: orange/warning tint when debugging active
  - Left segment: "Debugging: {config_name}" or "Paused at {file}:{line}"
  - Right segment: thread info "Thread 1 of 3"
- Click on debug status: toggle between run and pause

**Files Affected:**
- `src/ui/StatusBarPanel.cpp` (add debug status display)

**Acceptance Criteria:**
- Status bar shows debug state
- Background tinted during debug
- Shows current stop location when paused
- Click toggles run/pause
- Clears on debug end

**Dependencies:** Task 17

---

## Task 21: Hover Variable Evaluation

**Title:** Show variable value on mouse hover during debugging

**Description:** When paused at a breakpoint, hovering over a variable in the editor shows a tooltip with its current value and type.

**Implementation Details:**
- On mouse hover during debug pause:
  1. Get the word under cursor
  2. Send DAP evaluate request for that expression
  3. Show tooltip with type and value:
     ```
     tab_index: int
     Value: 3
     ```
  4. For complex types: expandable tree in tooltip
- Delay: 500ms hover before evaluation
- Cancel: move mouse away cancels pending evaluation

**Files Affected:**
- `src/ui/EditorPanel.cpp` (enhance hover for debug evaluation)
- `src/core/DebugAdapterClient.cpp` (evaluate on hover)

**Acceptance Criteria:**
- Hover shows variable value tooltip
- Complex types expandable
- 500ms hover delay
- Cancels on mouse move
- Only active during debug pause

**Dependencies:** Task 5

---

## Task 22: Data Breakpoints

**Title:** Support data/watchpoint breakpoints

**Description:** Allow setting breakpoints that trigger when a specific memory address or variable value changes (hardware watchpoints).

**Implementation Details:**
- In variables view, right-click variable: "Break on Value Change"
- Sends DAP `dataBreakpoint` request
- Shows in breakpoints list: "data: tab_index (changed)"
- Triggers when the variable value is modified

**Files Affected:**
- `src/ui/RunDebugPanel.cpp` (add data breakpoint from variables view)
- `src/core/DebugAdapterClient.cpp` (send data breakpoint request)

**Acceptance Criteria:**
- Data breakpoints can be set from variable view
- Trigger on value change
- Listed in breakpoints panel
- Removable like regular breakpoints

**Dependencies:** Tasks 5, 9

---

## Task 23: Multi-Target Debugging

**Title:** Support debugging multiple processes simultaneously

**Description:** Allow launching multiple debug sessions simultaneously (e.g., a client and server). Each session gets its own section in the call stack view.

**Implementation Details:**
- Support multiple `DebugAdapterClient` instances
- Call stack view shows sessions:
  ```
  CALL STACK
  Session: Server (running)
    Thread 1
      > main() (server.cpp:10)
  Session: Client (paused)
    Thread 1
      > handleResponse() (client.cpp:42)
  ```
- Independent controls per session
- Compound launch configuration:
  ```json
  {
      "name": "Full Stack",
      "configurations": ["Debug Server", "Debug Client"]
  }
  ```

**Files Affected:**
- `src/core/DebugSessionManager.cpp` (support multiple sessions)
- `src/ui/RunDebugPanel.cpp` (multi-session call stack)

**Acceptance Criteria:**
- Multiple debug sessions simultaneously
- Independent controls per session
- Call stack grouped by session
- Compound launch configurations

**Dependencies:** Tasks 5, 8

---

## Task 24: Debug Panel Theme

**Title:** Full theme integration for debug panel

**Description:** Ensure all debug panel elements use theme colors correctly.

**Implementation Details:**
- Color mappings:
  - Panel background: `BgPanel`
  - Section headers: `TextMuted` semibold
  - Variable names: `TextMain`
  - Variable types: `SyntaxType`
  - Variable values: `SyntaxString`
  - Execution line highlight: `WarningColor` at 20% opacity
  - Breakpoint red: `ErrorColor`
  - Debug toolbar: `BgPanel` with `AccentPrimary` active buttons

**Files Affected:**
- `src/ui/RunDebugPanel.cpp` (theme all elements)
- `src/ui/DebugToolbar.cpp` (theme toolbar)
- `src/ui/DebugConsolePanel.cpp` (theme console)

**Acceptance Criteria:**
- All elements themed correctly
- Dark and light themes work
- Execution highlight visible but not overwhelming
- Debug toolbar clearly visible

**Dependencies:** Tasks 1-11

---

## Task 25: Debug Panel Accessibility

**Title:** Accessibility support for debug panel

**Description:** Ensure the debug panel is fully keyboard navigable and screen reader compatible.

**Implementation Details:**
- Tab order: config selector -> play button -> step buttons -> breakpoints list -> call stack -> variables
- Arrow keys within sections
- Screen reader: "Paused at TabBar.cpp line 42, breakpoint"
- Variable values announced: "tab_index, integer, value 3"
- Keyboard shortcuts for all debug actions (F5, F10, F11)

**Files Affected:**
- `src/ui/RunDebugPanel.cpp` (add accessibility)
- `src/ui/DebugToolbar.cpp` (add accessibility)

**Acceptance Criteria:**
- Full keyboard navigation
- Screen reader announcements
- Debug state changes announced
- Keyboard shortcuts for all actions

**Dependencies:** Tasks 1-12

---

## Estimated Complexity

| Area | Effort |
|------|--------|
| Panel Shell (Task 1) | Medium |
| Launch Config (Tasks 2-3) | Medium |
| Build Before Run (Task 4) | Medium |
| DAP Client (Task 5) | Very High |
| Debug Controls (Task 6) | Medium |
| Breakpoints List (Task 7) | Medium |
| Call Stack (Task 8) | High |
| Variables (Tasks 9-10) | High |
| Debug Console (Task 11) | High |
| Execution Line (Task 12) | Medium |
| Conditional BP (Task 13) | Medium |
| Run Without Debug (Task 14) | Medium |
| Config Editor (Task 15) | Low |
| Compiler Output (Task 16) | Medium |
| State Management (Task 17) | Medium |
| Inline Values (Task 18) | High |
| Exception BP (Task 19) | Medium |
| Status Bar (Task 20) | Low |
| Hover Eval (Task 21) | Medium |
| Data Breakpoints (Task 22) | Medium |
| Multi-Target (Task 23) | High |
| Theme (Task 24) | Low |
| Accessibility (Task 25) | Medium |

## Files Created

- `src/ui/RunDebugPanel.h`
- `src/ui/RunDebugPanel.cpp`
- `src/ui/DebugToolbar.h`
- `src/ui/DebugToolbar.cpp`
- `src/ui/DebugConsolePanel.h`
- `src/ui/DebugConsolePanel.cpp`
- `src/core/LaunchConfig.h`
- `src/core/LaunchConfig.cpp`
- `src/core/DebugAdapterClient.h`
- `src/core/DebugAdapterClient.cpp`
- `src/core/DebugSessionManager.h`
- `src/core/DebugSessionManager.cpp`
- `src/core/TaskRunner.h`
- `src/core/TaskRunner.cpp`
- `src/core/ProcessRunner.h`
- `src/core/ProcessRunner.cpp`
- `src/core/CompilerOutputParser.h`
- `src/core/CompilerOutputParser.cpp`

## Files Modified

- `src/ui/SidebarMode.h`
- `src/ui/LayoutManager.cpp`
- `src/ui/EditorPanel.h` / `.cpp`
- `src/ui/StatusBarPanel.cpp`
- `src/ui/ProblemsPanel.cpp`
- `src/core/Events.h`
- `CMakeLists.txt`
