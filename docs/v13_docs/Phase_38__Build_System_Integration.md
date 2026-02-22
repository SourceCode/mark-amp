# Phase 38: Build System Integration

## Overview

Integrate build system support into MarkAmp to enable compiling, building, and running
C++ projects directly from the IDE. This phase covers CMake integration (the primary
build system for MarkAmp itself), Make support, custom build commands, a build output
panel with error parsing, click-to-navigate on errors, build progress, and configuration
profiles. This is new functionality that consumes the compiler configuration from Phase 37
and provides the foundation for a complete C++ development workflow.

## Existing Code References

| Component | File | Relevance |
|-----------|------|-----------|
| CompilerConfig | Phase 37 | Compiler settings consumed by build system |
| WorkspaceService | `/Users/ryanrentfro/code/markamp/src/core/WorkspaceService.h` | Workspace folder access |
| OutputPanel | `/Users/ryanrentfro/code/markamp/src/ui/OutputPanel.h` | Existing output panel (may need enhancement) |
| ProblemsPanel | `/Users/ryanrentfro/code/markamp/src/ui/ProblemsPanel.h` | Problem display panel |
| EventBus | `/Users/ryanrentfro/code/markamp/src/core/EventBus.h` | Build events |
| CommandRegistry | `/Users/ryanrentfro/code/markamp/src/core/CommandRegistry.h` | Build commands |

## Dependencies

- Phase 37 (Compiler Configuration) provides the compiler settings.
- Phase 40 (Workspace Configuration) provides workspace-level build settings.

---

## Tasks

### Task 1: BuildService Core

**Title:** Create the centralized build service that executes build operations

**Description:** Build a `BuildService` that manages build processes, tracks their
state, captures output, and emits progress events.

**Implementation Details:**
- Create `/Users/ryanrentfro/code/markamp/src/core/BuildService.h` and `.cpp`.
- Constructor: `BuildService(EventBus&, Config&)`.
- Build states: Idle, Configuring, Building, Succeeded, Failed, Cancelled.
- Methods: `start_build(BuildTask)`, `cancel_build()`, `is_building()`, `last_result()`.
- `BuildTask` struct: `string command`, `string working_directory`,
  `vector<string> environment`, `string label`.
- Process execution: use `wxProcess` or `std::system` with output capture.
- Stream stdout/stderr to EventBus in real-time: `BuildOutputEvent{ string line, OutputType type }`.
- On completion: `BuildCompletedEvent{ int exit_code, double elapsed_ms, int errors, int warnings }`.

**Files Affected:**
- New: `/Users/ryanrentfro/code/markamp/src/core/BuildService.h`
- New: `/Users/ryanrentfro/code/markamp/src/core/BuildService.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/src/core/Events.h` (build events)
- Modified: `/Users/ryanrentfro/code/markamp/CMakeLists.txt`

**Acceptance Criteria:**
- BuildService can execute a build command asynchronously.
- Output is streamed line-by-line via events.
- Build can be cancelled.
- Completion event includes exit code and timing.
- Only one build can run at a time.

**Dependencies:** None (foundational).

---

### Task 2: CMake Integration - Detection

**Title:** Auto-detect CMake projects and available presets

**Description:** Detect CMakeLists.txt in the workspace root and parse available
configure/build presets from CMakePresets.json.

**Implementation Details:**
- Create `CMakeIntegration` in `/Users/ryanrentfro/code/markamp/src/core/CMakeIntegration.h`.
- On workspace open, check for `CMakeLists.txt` at the root.
- Parse `CMakePresets.json` and `CMakeUserPresets.json` for:
  - Configure presets: `{name, displayName, generator, cacheVariables}`.
  - Build presets: `{name, displayName, configurePreset, targets}`.
- If no presets file, detect build directories: `build/`, `build/debug/`, `build/release/`.
- Emit `CMakeProjectDetectedEvent{ bool has_presets, vector<string> configure_presets }`.

**Files Affected:**
- New: `/Users/ryanrentfro/code/markamp/src/core/CMakeIntegration.h`
- New: `/Users/ryanrentfro/code/markamp/src/core/CMakeIntegration.cpp`

**Acceptance Criteria:**
- Detects CMakeLists.txt in workspace root.
- Parses CMakePresets.json for configure and build presets.
- Detects existing build directories.
- Emits detection event.
- Handles missing files gracefully.

**Dependencies:** None.

---

### Task 3: CMake Integration - Configure

**Title:** Run CMake configure from the IDE

**Description:** Allow the user to run `cmake --preset <name>` or
`cmake -B build -S . <options>` from within MarkAmp.

**Implementation Details:**
- "Configure" command: `build.cmakeConfigure`.
- If presets exist, show a quick-pick to select a configure preset.
- If no presets, use default: `cmake -B build/debug -S . -DCMAKE_BUILD_TYPE=Debug`.
- Pass compiler config from Phase 37: `-DCMAKE_CXX_COMPILER=<path>`,
  `-DCMAKE_CXX_STANDARD=<std>`.
- Stream configure output to the build output panel.
- Parse CMake errors/warnings from output.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/core/CMakeIntegration.cpp` (configure)
- Modified: `/Users/ryanrentfro/code/markamp/src/core/BuildService.cpp` (execute configure)

**Acceptance Criteria:**
- "Configure" command runs CMake configuration.
- Preset selection works when presets exist.
- Default configuration works without presets.
- Compiler config from Phase 37 is passed to CMake.
- Output streams to the build panel.

**Dependencies:** Task 1, Task 2, Phase 37 Task 1.

---

### Task 4: CMake Integration - Build

**Title:** Run CMake build from the IDE

**Description:** Allow the user to run `cmake --build <dir>` from within MarkAmp.

**Implementation Details:**
- "Build" command: `build.cmakeBuild` (Cmd+B).
- Uses `cmake --build <build_dir> --preset <name>` if presets exist.
- Otherwise: `cmake --build build/debug -j$(nproc)`.
- Parallel jobs: configurable, default = CPU count.
- Build target selection: "All" (default) or specific target from CMake.
- Stream output with ANSI color support.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/core/CMakeIntegration.cpp` (build)

**Acceptance Criteria:**
- Cmd+B triggers a CMake build.
- Parallel build uses all CPU cores by default.
- Build output streams in real-time.
- Specific targets can be selected.

**Dependencies:** Task 1, Task 3.

---

### Task 5: Make Integration

**Title:** Detect and support Makefile-based projects

**Description:** Detect Makefile in the workspace and allow running make targets.

**Implementation Details:**
- Detect `Makefile`, `makefile`, `GNUmakefile` in workspace root.
- Parse make targets: `make -qp | grep -E '^[a-zA-Z].*:' | cut -d':' -f1`.
- Show available targets in a quick-pick.
- Default target: `make` (no arguments).
- Support `make clean`, `make install`, `make test`.

**Files Affected:**
- New: `/Users/ryanrentfro/code/markamp/src/core/MakeIntegration.h`
- New: `/Users/ryanrentfro/code/markamp/src/core/MakeIntegration.cpp`

**Acceptance Criteria:**
- Detects Makefile in workspace.
- Parses available targets.
- Default build runs `make`.
- Target selection via quick-pick.

**Dependencies:** Task 1.

---

### Task 6: Custom Build Command Editor

**Title:** Allow users to define custom build commands

**Description:** For projects not using CMake or Make, allow defining custom build
commands with variables.

**Implementation Details:**
- Create `BuildTaskEditor` dialog for defining custom commands.
- Fields: Label, Command, Working Directory, Environment Variables.
- Variable substitution: `${workspaceFolder}`, `${file}`, `${fileBasename}`,
  `${fileDirname}`, `${compilerPath}`.
- Save custom tasks in `.markamp/tasks.json`.
- Multiple tasks: build, clean, test, run.
- "Run Task" command shows a quick-pick of all defined tasks.

**Files Affected:**
- New: `/Users/ryanrentfro/code/markamp/src/ui/BuildTaskEditor.h`
- New: `/Users/ryanrentfro/code/markamp/src/ui/BuildTaskEditor.cpp`
- New: `/Users/ryanrentfro/code/markamp/src/core/BuildTaskManager.h`
- New: `/Users/ryanrentfro/code/markamp/src/core/BuildTaskManager.cpp`

**Acceptance Criteria:**
- Custom build tasks can be defined.
- Variable substitution works.
- Tasks are saved in `.markamp/tasks.json`.
- "Run Task" shows available tasks.
- Multiple task types are supported.

**Dependencies:** Task 1.

---

### Task 7: Build Output Panel with ANSI Colors

**Title:** Create the build output panel with colored terminal output

**Description:** A dedicated panel that displays build output with ANSI color rendering,
auto-scroll, and text selection.

**Implementation Details:**
- Create `BuildOutputPanel` as a bottom panel (or enhance existing `OutputPanel`).
- Use `wxStyledTextCtrl` or custom rendering for ANSI color support.
- Parse ANSI escape codes: standard 16 colors, bold, underline, reset.
- Auto-scroll to bottom as new output arrives.
- Text is selectable and copyable.
- "Clear" button clears the output.
- "Pin" button disables auto-scroll.
- Show build status in the panel header: "Building... (12s)" or "Build succeeded (34s)".

**Files Affected:**
- New: `/Users/ryanrentfro/code/markamp/src/ui/BuildOutputPanel.h`
- New: `/Users/ryanrentfro/code/markamp/src/ui/BuildOutputPanel.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/CMakeLists.txt`

**Acceptance Criteria:**
- Build output displays with ANSI colors.
- Auto-scroll follows new output.
- Text is selectable and copyable.
- Clear and Pin buttons work.
- Build timing is displayed.

**Dependencies:** Task 1.

---

### Task 8: Error/Warning Parser

**Title:** Parse compiler errors and warnings from build output

**Description:** Parse GCC, Clang, and MSVC error/warning output formats to extract
structured diagnostics.

**Implementation Details:**
- Create `BuildOutputParser` in `/Users/ryanrentfro/code/markamp/src/core/BuildOutputParser.h`.
- Parse formats:
  - GCC/Clang: `file.cpp:42:15: error: expected ';'`
  - MSVC: `file.cpp(42,15): error C2143: syntax error`
  - CMake: `CMake Error at CMakeLists.txt:17`
- Extract: file path, line, column, severity (error/warning/note), message, error code.
- Emit `BuildDiagnosticEvent` for each parsed diagnostic.
- Aggregate diagnostics: `{total_errors, total_warnings}`.

**Files Affected:**
- New: `/Users/ryanrentfro/code/markamp/src/core/BuildOutputParser.h`
- New: `/Users/ryanrentfro/code/markamp/src/core/BuildOutputParser.cpp`

**Acceptance Criteria:**
- Parses GCC/Clang error format correctly.
- Parses MSVC error format correctly.
- Parses CMake error format.
- Extracts file, line, column, severity, message.
- Aggregate counts are accurate.

**Dependencies:** Task 1.

---

### Task 9: Click-to-Navigate on Error Lines

**Title:** Make error file:line references clickable in the build output

**Description:** Error and warning lines in the build output should be clickable links
that navigate the editor to the error location.

**Implementation Details:**
- After parsing diagnostics (Task 8), apply link indicators to the output panel.
- Clicking a file:line reference: opens the file and navigates to the line/column.
- Hover shows a tooltip with the full error message.
- Use `wxStyledTextCtrl` hotspot styles for link rendering.
- Also populate the Problems Panel with parsed diagnostics.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/BuildOutputPanel.cpp` (link rendering)
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/ProblemsPanel.cpp` (diagnostic display)

**Acceptance Criteria:**
- File:line references in build output are clickable.
- Clicking navigates to the error location.
- Hover shows the error message.
- Problems Panel shows all diagnostics.
- Links are styled with underline and accent color.

**Dependencies:** Task 7, Task 8.

---

### Task 10: Build Progress Indicator

**Title:** Show build progress in the status bar and panel header

**Description:** Display build progress: elapsed time, current file being compiled,
and a progress bar for targets with known total.

**Implementation Details:**
- Status bar: show "Building... (target 5/12, 15s)" during build.
- Parse progress from build output: CMake shows `[  5/12] Building CXX object ...`.
- Update a progress bar or percentage indicator.
- Show the current file being compiled.
- On completion: "Build succeeded (34s, 0 errors, 2 warnings)".
- Failed build: "Build FAILED (12s, 3 errors, 1 warning)".

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/StatusBarPanel.cpp` (build progress)
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/BuildOutputPanel.cpp` (progress parsing)

**Acceptance Criteria:**
- Progress shows target count and elapsed time.
- Progress bar updates as targets complete.
- Current file name is displayed.
- Completion summary includes errors and warnings.
- Failed build is clearly indicated.

**Dependencies:** Task 1, Task 8.

---

### Task 11: Parallel Build Configuration

**Title:** Configure the number of parallel build jobs

**Description:** Allow users to set the number of parallel compilation jobs.

**Implementation Details:**
- Setting: `build.parallelJobs` (default: 0 = auto = CPU count).
- Auto-detect CPU count: `std::thread::hardware_concurrency()`.
- Show in build config: "Parallel Jobs: [Auto (8 cores)] [1] [2] [4] [8] [Custom]".
- Pass to CMake: `--parallel N` or Make: `-jN`.
- Register in SettingsCatalog under "Build" group.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/core/BuildService.cpp` (parallel config)
- Modified: SettingsCatalog registration code.

**Acceptance Criteria:**
- Default uses all CPU cores.
- Custom job count is configurable.
- Value is passed to build system.
- Setting is registered and searchable.

**Dependencies:** Task 1.

---

### Task 12: Build Configuration Profiles

**Title:** Support Debug/Release/RelWithDebInfo/MinSizeRel configurations

**Description:** Allow switching between build configurations with a single click.

**Implementation Details:**
- Build configuration selector in the status bar or build panel toolbar.
- Options: Debug, Release, RelWithDebInfo, MinSizeRel.
- Switching changes: `CMAKE_BUILD_TYPE`, optimization level, debug symbols.
- Store active configuration in workspace settings.
- Show current configuration in status bar.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/core/BuildService.h` (config profiles)
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/StatusBarPanel.cpp` (config display)

**Acceptance Criteria:**
- Build configuration selector is available.
- Switching configuration updates build parameters.
- Active configuration persists.
- Status bar shows current configuration.

**Dependencies:** Task 1, Task 3.

---

### Task 13: Build on Save Option

**Title:** Enable automatic build on file save

**Description:** When enabled, saving a file triggers an automatic build.

**Implementation Details:**
- Setting: `build.buildOnSave` (default: false).
- On `FileSavedEvent`, if enabled, trigger `BuildService::start_build()`.
- Debounce: wait 500ms after the last save before building (batch rapid saves).
- Only rebuild if the saved file is part of the build target.
- Show a subtle notification: "Auto-building..." in the status bar.
- If a build is already running, queue the new build.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/core/BuildService.cpp` (auto-build)
- Modified: SettingsCatalog registration.

**Acceptance Criteria:**
- Build triggers on save when enabled.
- Rapid saves are debounced.
- Non-source files do not trigger build.
- Status bar shows auto-build indicator.
- Queued builds wait for current build to finish.

**Dependencies:** Task 1.

---

### Task 14: Clean Build Command

**Title:** Implement clean build functionality

**Description:** Add commands for "Clean" (remove build artifacts), "Rebuild" (clean + build),
and "Clean Build Directory".

**Implementation Details:**
- Clean: `cmake --build <dir> --target clean` or `make clean`.
- Rebuild: clean followed by build.
- Clean Build Directory: delete the entire build directory.
- Show confirmation for "Clean Build Directory" (destructive).
- Commands: `build.clean`, `build.rebuild`, `build.cleanDirectory`.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/core/BuildService.cpp` (clean commands)
- Modified: `/Users/ryanrentfro/code/markamp/src/core/CMakeIntegration.cpp`

**Acceptance Criteria:**
- Clean removes build artifacts.
- Rebuild performs clean then build.
- "Clean Directory" deletes the build directory with confirmation.
- All commands are registered.

**Dependencies:** Task 1, Task 4.

---

### Task 15: Toolchain File Selection

**Title:** UI for selecting CMake toolchain files

**Description:** Allow users to browse and select CMake toolchain files for
cross-compilation.

**Implementation Details:**
- "Toolchain File" field in CompilerConfigPanel and BuildService.
- Browse button opens file dialog filtered to `*.cmake`.
- Recently used toolchain files are remembered.
- Pass to CMake: `-DCMAKE_TOOLCHAIN_FILE=<path>`.
- Validate that the toolchain file exists and is readable.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/CompilerConfigPanel.cpp` (toolchain UI)
- Modified: `/Users/ryanrentfro/code/markamp/src/core/CMakeIntegration.cpp` (pass toolchain)

**Acceptance Criteria:**
- Toolchain file can be browsed and selected.
- Recent toolchain files are available.
- Path is passed to CMake correctly.
- Invalid paths show a warning.

**Dependencies:** Task 3, Phase 37 Task 13.

---

### Task 16: Build System Detection Heuristic

**Title:** Auto-detect the appropriate build system for the workspace

**Description:** When a workspace is opened, determine which build system to use
based on project files present.

**Implementation Details:**
- Detection priority:
  1. CMakePresets.json -> CMake with presets
  2. CMakeLists.txt -> CMake
  3. Makefile/GNUmakefile -> Make
  4. build.ninja -> Ninja
  5. .markamp/tasks.json -> Custom tasks
  6. None -> Show "Configure Build System" prompt
- Emit `BuildSystemDetectedEvent{ BuildSystemType type, string config_file }`.
- Show detected build system in the status bar.

**Files Affected:**
- New: `/Users/ryanrentfro/code/markamp/src/core/BuildSystemDetector.h`
- New: `/Users/ryanrentfro/code/markamp/src/core/BuildSystemDetector.cpp`

**Acceptance Criteria:**
- Detects CMake, Make, Ninja, and custom tasks.
- Priority order is respected.
- Status bar shows detected build system.
- Missing build system shows configuration prompt.

**Dependencies:** Task 2, Task 5.

---

### Task 17: Build Keyboard Shortcuts

**Title:** Register all build system keyboard shortcuts

**Description:** Register intuitive shortcuts for build operations.

**Implementation Details:**
- Cmd+B: Build
- Cmd+Shift+B: Build with options (pick target)
- Cmd+Shift+K: Clean
- Cmd+Shift+R: Rebuild
- Ctrl+Shift+`: Toggle build output panel
- F7: Build (alternative)
- Shift+F7: Clean + Build
- All in "Build" category.

**Files Affected:**
- Modified: shortcut registration code.

**Acceptance Criteria:**
- All shortcuts are registered.
- No conflicts with existing shortcuts.
- Shortcuts appear in overlay and palette.

**Dependencies:** Task 4.

---

### Task 18: Build Output Panel Theme Integration

**Title:** Theme the build output panel

**Description:** Apply theme tokens to the build output panel and error decorations.

**Implementation Details:**
- Theme tokens: `build.outputBackground`, `build.outputForeground`, `build.errorForeground`,
  `build.warningForeground`, `build.successForeground`, `build.linkForeground`,
  `build.progressBar`, `build.headerBackground`.
- ANSI color mapping uses theme-appropriate colors.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/BuildOutputPanel.cpp` (theme)

**Acceptance Criteria:**
- All output panel elements use theme tokens.
- Error/warning text colors are theme-appropriate.
- ANSI colors map to theme colors.

**Dependencies:** Task 7.

---

### Task 19-20: Build History and Environment

**Task 19: Build History** - Track the last 20 builds with: timestamp, configuration, duration, exit code, error/warning count. Show in a "Build History" dropdown. Allow re-running a previous build.

**Task 20: Build Environment Variables** - Allow configuring environment variables for build processes. UI: key-value editor. Variables passed to the build process. Workspace-level and user-level variables.

---

### Task 21-25: Additional Tasks

**Task 21: Build System Event Wiring** - Wire BuildOutputEvent, BuildCompletedEvent, BuildDiagnosticEvent, BuildProgressEvent to all subscribers. Ensure EditorPanel marks error lines after build.

**Task 22: Build Target Picker** - Quick-pick dialog for selecting build targets. Parse available targets from CMake/Make. Show target dependencies.

**Task 23: Build Command Registration** - Register all build commands in CommandRegistry: build.build, build.clean, build.rebuild, build.configure, build.selectTarget, build.cancel, build.showOutput, build.buildOnSave.

**Task 24: Build Notification Integration** - Show OS-level notification on build complete (when app is not focused). Configurable via `build.showNotifications` setting.

**Task 25: Unit Tests for Build System** - Test file: `/Users/ryanrentfro/code/markamp/tests/unit/test_build_system.cpp`. Sections: BuildOutputParser (GCC, Clang, MSVC, CMake formats), CMakeIntegration (preset parsing, detection), BuildTaskManager (task CRUD, variable substitution), BuildService (state machine). At least 30 test cases.

---

### Task 26: Build Dependency Graph Visualization

**Title:** Visualize the build dependency graph for CMake projects

**Description:** Parse the CMake dependency graph and render an interactive visualization
showing how targets depend on each other, allowing users to understand and navigate build
structure.

**Implementation Details:**
- Extract dependency information:
  - Run `cmake --graphviz=deps.dot <build_dir>` to get the Graphviz DOT file.
  - Alternatively, parse `CMakeCache.txt` and `cmake_install.cmake` for target info.
  - Parse the DOT file to extract nodes (targets) and edges (dependencies).
- Create `BuildDependencyGraph` data model in `/Users/ryanrentfro/code/markamp/src/core/BuildDependencyGraph.h`:
  - `struct GraphNode { string name; string type; vector<string> dependencies; }`.
  - `vector<GraphNode> nodes`.
  - Methods: `get_dependents(target)`, `get_transitive_deps(target)`, `topological_sort()`.
- Create `BuildGraphPanel` in `/Users/ryanrentfro/code/markamp/src/ui/BuildGraphPanel.h`:
  - Render nodes as boxes arranged in a layered layout (topological order left-to-right).
  - Edges as arrows between nodes.
  - Color-coded by target type: executable = blue, static lib = green, shared lib = orange,
    object lib = gray.
  - Click a node to highlight its dependency chain (upstream and downstream).
  - Double-click a node to navigate to the corresponding `CMakeLists.txt` `add_library`/`add_executable` line.
  - Zoom and pan support.
  - Search bar to find and focus a specific target.
- "Build > Show Dependency Graph" command: `build.showDependencyGraph`.
- Refresh the graph after `cmake --configure` completes.

**Files Affected:**
- New: `/Users/ryanrentfro/code/markamp/src/core/BuildDependencyGraph.h`
- New: `/Users/ryanrentfro/code/markamp/src/core/BuildDependencyGraph.cpp`
- New: `/Users/ryanrentfro/code/markamp/src/ui/BuildGraphPanel.h`
- New: `/Users/ryanrentfro/code/markamp/src/ui/BuildGraphPanel.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/src/core/CMakeIntegration.cpp` (graph extraction)
- Modified: `/Users/ryanrentfro/code/markamp/CMakeLists.txt`

**Acceptance Criteria:**
- DOT file is generated and parsed from CMake.
- Dependency graph renders with nodes and edges.
- Target types are color-coded.
- Clicking a node highlights its dependency chain.
- Double-clicking navigates to the CMakeLists.txt definition.
- Search finds and focuses a target.
- Graph refreshes after reconfiguration.

**Dependencies:** Task 2, Task 3.

---

### Task 27: Incremental Build Status Tracking

**Title:** Track and display incremental build status per source file

**Description:** Track which source files have changed since the last build and show
per-file build status indicators in the file tree and editor tabs.

**Implementation Details:**
- Create `IncrementalBuildTracker` in `/Users/ryanrentfro/code/markamp/src/core/IncrementalBuildTracker.h`:
  - Track file modification times vs last successful build time.
  - States per file: `UpToDate`, `Modified` (needs rebuild), `Building`, `Failed`, `Unknown`.
  - Subscribe to `FileSavedEvent` to mark files as `Modified`.
  - Subscribe to `BuildCompletedEvent` to mark built files as `UpToDate` or `Failed`.
  - Store the last build timestamp per file.
- Integrate with `FileTreeCtrl`:
  - Show a small status icon next to each source file:
    - Green dot: up-to-date (built successfully).
    - Yellow dot: modified since last build.
    - Red dot: last build failed for this file.
    - Gray dot: unknown / never built.
  - Only show icons for source files (.cpp, .h, .c, .cxx) not all files.
- Integrate with `TabBar`:
  - Modified (needs rebuild) tabs show a subtle build indicator.
- Show summary in status bar: "3 files modified since last build".
- "Build Modified Files Only" command: filters the build to only recompile changed files
  (pass specific targets or rely on CMake's incremental build).

**Files Affected:**
- New: `/Users/ryanrentfro/code/markamp/src/core/IncrementalBuildTracker.h`
- New: `/Users/ryanrentfro/code/markamp/src/core/IncrementalBuildTracker.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/FileTreeCtrl.cpp` (status icons)
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/TabBar.cpp` (build indicator)
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/StatusBarPanel.cpp` (modified count)

**Acceptance Criteria:**
- File modification times are tracked against last build time.
- File tree shows per-file build status icons.
- Icons update on save (yellow) and build completion (green/red).
- Status bar shows count of modified files since last build.
- Tab bar shows a subtle build-needed indicator.
- Status resets correctly on successful build.

**Dependencies:** Task 1, Task 8.

---

### Task 28: Build Artifact Browser

**Title:** Browse and manage build artifacts (executables, libraries, object files)

**Description:** Provide a panel that lists all build artifacts produced by the last build,
with options to run, inspect, copy, or clean individual artifacts.

**Implementation Details:**
- Create `BuildArtifactBrowser` in `/Users/ryanrentfro/code/markamp/src/ui/BuildArtifactBrowser.h`:
  - Scan the build output directory for produced artifacts.
  - Categories: Executables, Static Libraries, Shared Libraries, Object Files.
  - For each artifact show: name, size, last modified time, full path.
- Artifact actions (context menu / toolbar):
  - "Run" (executables only): execute the binary with optional arguments dialog.
  - "Copy Path": copy the artifact's absolute path to clipboard.
  - "Reveal in Finder/Explorer": open the containing directory.
  - "Delete": remove the artifact with confirmation.
  - "Show Dependencies" (shared libraries): run `otool -L` (macOS), `ldd` (Linux), or
    `dumpbin /dependents` (Windows) to show library dependencies.
- Artifact details panel (click to expand):
  - File size, format (ELF/Mach-O/PE), architecture, linked libraries.
  - For executables: show symbols count (via `nm` or `objdump`).
- Refresh after each build completes.
- Register command: `build.showArtifacts`.

**Files Affected:**
- New: `/Users/ryanrentfro/code/markamp/src/ui/BuildArtifactBrowser.h`
- New: `/Users/ryanrentfro/code/markamp/src/ui/BuildArtifactBrowser.cpp`
- New: `/Users/ryanrentfro/code/markamp/src/core/ArtifactInspector.h`
- New: `/Users/ryanrentfro/code/markamp/src/core/ArtifactInspector.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/CMakeLists.txt`

**Acceptance Criteria:**
- Build artifacts are scanned and categorized after build.
- Executables, libraries, and object files are listed with metadata.
- "Run" executes an artifact and captures output.
- "Show Dependencies" displays linked libraries.
- "Delete" removes an artifact with confirmation.
- Panel refreshes automatically after build completion.
- Artifact sizes and timestamps are accurate.

**Dependencies:** Task 1, Task 4.

---

### Task 29: Build Log Export and History Comparison

**Title:** Export build logs and compare outputs between two builds

**Description:** Allow users to export the full build log to a file and compare the
output of two builds side-by-side to identify regressions or changes.

**Implementation Details:**
- Build log export:
  - "Export Build Log" button in the BuildOutputPanel toolbar.
  - Save formats: plain text (.log), ANSI-colored text (.log with escape codes),
    HTML (with color rendering).
  - Default filename: `build_<config>_<timestamp>.log`.
  - Include build metadata header: command, configuration, timestamp, duration,
    exit code, error/warning counts.
- Build history storage (extends Task 19):
  - Store the last 10 build logs on disk in `.markamp/build_logs/`.
  - Each log includes: full output, parsed diagnostics, timing, configuration.
  - Auto-prune logs older than 30 days or beyond the 10-log limit.
- Build comparison:
  - "Compare with Previous Build" command: `build.compareWithPrevious`.
  - Show a diff view between two build logs:
    - New errors/warnings highlighted in red.
    - Resolved errors/warnings highlighted in green.
    - Unchanged diagnostics shown in gray.
  - Summary: "+2 new errors, -1 resolved error, +3 new warnings".
  - Allow selecting any two builds from history for comparison.
  - Side-by-side panel layout with synchronized scrolling.

**Files Affected:**
- New: `/Users/ryanrentfro/code/markamp/src/core/BuildLogManager.h`
- New: `/Users/ryanrentfro/code/markamp/src/core/BuildLogManager.cpp`
- New: `/Users/ryanrentfro/code/markamp/src/ui/BuildComparisonPanel.h`
- New: `/Users/ryanrentfro/code/markamp/src/ui/BuildComparisonPanel.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/BuildOutputPanel.cpp` (export button)

**Acceptance Criteria:**
- Build log exports to text, ANSI, and HTML formats.
- Exported logs include metadata header.
- Last 10 build logs are persisted to disk.
- Build comparison shows a diff of diagnostics between two builds.
- New and resolved diagnostics are highlighted.
- Summary shows counts of new/resolved errors and warnings.
- Old logs are auto-pruned.

**Dependencies:** Task 1, Task 7, Task 8, Task 19.

---

### Task 30: Remote Build Support

**Title:** Configure and execute builds on remote machines via SSH

**Description:** Allow users to configure a remote build host and execute build commands
over SSH, streaming output back to the local IDE in real-time.

**Implementation Details:**
- Create `RemoteBuildConfig` in `/Users/ryanrentfro/code/markamp/src/core/RemoteBuildConfig.h`:
  - Fields: `host`, `port`, `user`, `identity_file`, `remote_workspace_path`,
    `sync_method` ("rsync", "scp"), `build_command`.
  - Serialization to JSON for persistence in `.markamp/remote_build.json`.
- Create `RemoteBuildService` in `/Users/ryanrentfro/code/markamp/src/core/RemoteBuildService.h`:
  - Pre-build: sync local workspace to remote via rsync:
    `rsync -avz --exclude 'build/' ./ user@host:remote_path/`.
  - Execute build command over SSH: `ssh user@host 'cd remote_path && cmake --build build'`.
  - Stream stdout/stderr back via the SSH connection.
  - Post-build: optionally sync artifacts back to local.
  - Use `wxProcess` for SSH process management.
- Remote build UI in CompilerConfigPanel or a separate dialog:
  - "Remote Build" toggle.
  - Host, port, user, identity file fields.
  - Remote workspace path.
  - "Test Connection" button: runs `ssh user@host echo ok`.
  - Sync options: exclude patterns, sync direction.
- Remote build progress: show sync progress, then build progress.
- Emit `RemoteBuildStartedEvent`, `RemoteBuildSyncEvent`, standard `BuildOutputEvent` and
  `BuildCompletedEvent` (so existing panels work seamlessly).

**Files Affected:**
- New: `/Users/ryanrentfro/code/markamp/src/core/RemoteBuildConfig.h`
- New: `/Users/ryanrentfro/code/markamp/src/core/RemoteBuildConfig.cpp`
- New: `/Users/ryanrentfro/code/markamp/src/core/RemoteBuildService.h`
- New: `/Users/ryanrentfro/code/markamp/src/core/RemoteBuildService.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/src/core/Events.h` (remote build events)
- Modified: `/Users/ryanrentfro/code/markamp/src/core/BuildService.cpp` (remote build delegation)

**Acceptance Criteria:**
- Remote build host is configurable with host, user, and identity file.
- "Test Connection" verifies SSH connectivity.
- Workspace syncs to remote before build via rsync.
- Build output streams back in real-time over SSH.
- Build artifacts can optionally sync back to local.
- Existing BuildOutputPanel and ProblemsPanel work with remote builds.
- Connection failures produce clear error messages.

**Dependencies:** Task 1, Task 7, Task 8.
