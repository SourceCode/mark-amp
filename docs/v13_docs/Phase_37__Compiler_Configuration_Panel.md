# Phase 37: Compiler Configuration Panel

## Overview

Build a Compiler Configuration Panel that allows users to configure C++ compilation
settings through a visual interface. MarkAmp aims to be a viable C++ IDE, so this phase
adds compiler selection, flags management, include/library path configuration, standard
selection, optimization and sanitizer options, and profile save/load. This is entirely
new functionality -- the existing codebase has no compiler configuration UI, no build
system integration, and no project settings. The panel integrates with the
`SettingsCatalog` for persistence and the `CommandRegistry` for discoverability.

## Existing Code References

| Component | File | Relevance |
|-----------|------|-----------|
| Config | `/Users/ryanrentfro/code/markamp/src/core/Config.h` | Persistence backend |
| SettingsCatalog | `/Users/ryanrentfro/code/markamp/src/core/SettingsCatalog.h` | Register compiler settings |
| WorkspaceSettings | `/Users/ryanrentfro/code/markamp/src/core/WorkspaceSettings.h` | Per-workspace compiler config |
| ConfigProfileManager | `/Users/ryanrentfro/code/markamp/src/core/ConfigProfile.h` | Save/load compiler profiles |
| CommandRegistry | `/Users/ryanrentfro/code/markamp/src/core/CommandRegistry.h` | Register compiler commands |
| EventBus | `/Users/ryanrentfro/code/markamp/src/core/EventBus.h` | Emit compiler config changes |
| ThemeEngine | `/Users/ryanrentfro/code/markamp/src/core/ThemeEngine.h` | Panel theming |

## Dependencies

- Phase 38 (Build System Integration) directly consumes compiler configuration from this phase.
- Phase 36 (Settings UI Overhaul) provides the settings UI patterns reused here.
- Phase 40 (Workspace Configuration) provides workspace-level overrides for compiler settings.

---

## Tasks

### Task 1: CompilerConfig Data Model

**Title:** Define the compiler configuration data model

**Description:** Create the data structures that represent a complete compiler configuration:
compiler path, standard, flags, paths, optimization, and sanitizers.

**Implementation Details:**
- Create `/Users/ryanrentfro/code/markamp/src/core/CompilerConfig.h`:
```cpp
namespace markamp::core {
struct CompilerConfig {
    std::string compiler_path;           // e.g., "/usr/bin/clang++"
    std::string compiler_name;           // "clang++", "g++", "cl.exe"
    std::string compiler_version;        // "17.0.0"
    std::string cpp_standard;            // "c++14", "c++17", "c++20", "c++23", "c++26"
    std::string optimization_level;      // "O0", "O1", "O2", "O3", "Os", "Oz"
    std::string warning_level;           // "none", "default", "all", "extra", "pedantic"
    std::vector<std::string> include_paths;
    std::vector<std::string> library_paths;
    std::vector<std::string> defines;          // e.g., "DEBUG=1", "NDEBUG"
    std::vector<std::string> custom_flags;     // e.g., "-fno-exceptions"
    std::vector<std::string> linker_flags;
    bool enable_asan{false};
    bool enable_ubsan{false};
    bool enable_tsan{false};
    bool enable_msan{false};
    bool enable_coverage{false};
    bool position_independent{false};
    std::string output_type;             // "executable", "static_lib", "shared_lib", "object"
    std::string target_arch;             // "x86_64", "arm64", "wasm32"
    std::string toolchain_file;          // For CMake cross-compilation
};
}
```
- Add serialization to/from JSON for persistence.
- Add validation: check compiler_path exists, standard is valid, flags are well-formed.

**Files Affected:**
- New: `/Users/ryanrentfro/code/markamp/src/core/CompilerConfig.h`
- New: `/Users/ryanrentfro/code/markamp/src/core/CompilerConfig.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/CMakeLists.txt`

**Acceptance Criteria:**
- `CompilerConfig` stores all compilation parameters.
- JSON serialization round-trips correctly.
- Validation rejects invalid compiler paths and standards.
- Default values produce a usable configuration.

**Dependencies:** None (foundational).

---

### Task 2: Compiler Auto-Detection

**Title:** Auto-detect installed compilers on the system

**Description:** Scan the system for installed C++ compilers and populate a list of
available options.

**Implementation Details:**
- Create `CompilerDetector` in `/Users/ryanrentfro/code/markamp/src/core/CompilerDetector.h`.
- Detection strategies:
  - macOS: check `/usr/bin/clang++`, `xcrun --find clang++`, Homebrew paths.
  - Linux: check `/usr/bin/g++`, `/usr/bin/clang++`, `/usr/local/bin/`.
  - Windows: query MSVC via `vswhere`, check `PATH` for cl.exe, g++.
- For each found compiler, extract version: `clang++ --version`, `g++ --version`.
- Return `vector<DetectedCompiler>` with `{path, name, version, supported_standards}`.
- Cache detection results; re-scan on explicit request.

**Files Affected:**
- New: `/Users/ryanrentfro/code/markamp/src/core/CompilerDetector.h`
- New: `/Users/ryanrentfro/code/markamp/src/core/CompilerDetector.cpp`

**Acceptance Criteria:**
- Detects clang++ on macOS.
- Detects g++ on Linux.
- Extracts compiler version.
- Returns supported C++ standards for each compiler.
- Handles missing compilers gracefully (empty list, no errors).

**Dependencies:** None.

---

### Task 3: Compiler Selection Dropdown

**Title:** Build the compiler selection UI with auto-detected options

**Description:** Create a dropdown that lists auto-detected compilers and allows
selecting one. Shows the compiler name, version, and path.

**Implementation Details:**
- Create `CompilerConfigPanel` as a `wxPanel`:
  `/Users/ryanrentfro/code/markamp/src/ui/CompilerConfigPanel.h` and `.cpp`.
- Constructor: `CompilerConfigPanel(wxWindow*, ThemeEngine&, EventBus&, Config&)`.
- Compiler dropdown (`wxChoice`): each entry shows "clang++ 17.0.0 (/usr/bin/clang++)".
- "Auto-detect" button re-scans the system.
- "Custom..." option allows entering a compiler path manually (file browser dialog).
- Show compiler version and supported standards below the dropdown.

**Files Affected:**
- New: `/Users/ryanrentfro/code/markamp/src/ui/CompilerConfigPanel.h`
- New: `/Users/ryanrentfro/code/markamp/src/ui/CompilerConfigPanel.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/CMakeLists.txt`

**Acceptance Criteria:**
- Dropdown lists all detected compilers.
- Selecting a compiler updates the CompilerConfig.
- "Auto-detect" re-scans and refreshes the list.
- "Custom..." opens a file browser for manual path entry.
- Version and standards are displayed for the selected compiler.

**Dependencies:** Tasks 1, 2.

---

### Task 4: C++ Standard Selector

**Title:** Build C++ standard selection UI

**Description:** A dropdown for selecting the C++ language standard, filtered to only
show standards supported by the selected compiler.

**Implementation Details:**
- Options: C++11, C++14, C++17, C++20, C++23, C++26.
- Gray out standards not supported by the selected compiler.
- Default: highest supported standard.
- Show a tooltip for each: "C++23 - requires GCC 12+ or Clang 16+".
- Changing the standard may invalidate some flags (show a warning).

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/CompilerConfigPanel.cpp` (standard dropdown)

**Acceptance Criteria:**
- Dropdown shows all C++ standards.
- Unsupported standards are grayed out.
- Default is the highest supported standard.
- Changing the compiler updates available standards.
- Tooltips explain requirements.

**Dependencies:** Task 3.

---

### Task 5: Compiler Flags Editor

**Title:** Build the compiler flags editor with common flags as checkboxes

**Description:** A two-part flags editor: checkboxes for common flags and a text input
for custom flags.

**Implementation Details:**
- Common flags as checkboxes (grouped):
  - Warnings: `-Wall`, `-Wextra`, `-Wpedantic`, `-Werror`.
  - Debug: `-g`, `-g3`.
  - Language: `-fno-exceptions`, `-fno-rtti`, `-fmodules`.
  - Code generation: `-fPIC`, `-fPIE`, `-march=native`.
- Custom flags text input below: free-form text for additional flags.
- Show the resulting full command-line preview at the bottom:
  `clang++ -std=c++23 -O2 -Wall -Wextra -g ...`
- Validate custom flags (warn on unrecognized flags).

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/CompilerConfigPanel.cpp` (flags editor)

**Acceptance Criteria:**
- Common flags are available as checkboxes.
- Custom flags can be entered as free text.
- Full command preview shows the complete flag set.
- Duplicate flags are detected and warned.
- Unknown flags show a warning.

**Dependencies:** Task 3.

---

### Task 6: Include Paths List Editor

**Title:** Build an editable list for include paths (-I flags)

**Description:** A list editor for managing include directories with add, remove,
reorder, and browse capabilities.

**Implementation Details:**
- Create a reusable `PathListEditor` widget:
  - Shows paths in a list view.
  - "Add" button with folder browser dialog.
  - "Remove" button for selected item.
  - Up/Down buttons for reordering.
  - Manual text entry for pasting paths.
- Validate: check that each path exists on disk. Show a warning icon for missing paths.
- Support drag-and-drop of folders onto the list to add them.
- Each path shows an icon: green check for existing, red X for missing.

**Files Affected:**
- New: `/Users/ryanrentfro/code/markamp/src/ui/PathListEditor.h`
- New: `/Users/ryanrentfro/code/markamp/src/ui/PathListEditor.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/CompilerConfigPanel.cpp` (embed include paths)

**Acceptance Criteria:**
- Include paths are shown in a list.
- Add/Remove/Reorder buttons work.
- Browse button opens a folder dialog.
- Missing paths show a warning icon.
- Drag-and-drop adds folders.

**Dependencies:** Task 3.

---

### Task 7: Library Paths List Editor

**Title:** Build an editable list for library paths (-L flags)

**Description:** Reuse the `PathListEditor` from Task 6 for library search directories.

**Implementation Details:**
- Instance `PathListEditor` for library paths.
- Add library-specific validation: check for common library files (.a, .so, .dylib, .lib).
- Show the library path list in a separate collapsible section.
- Default paths: system library paths (/usr/lib, /usr/local/lib).

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/CompilerConfigPanel.cpp` (library paths)

**Acceptance Criteria:**
- Library paths are shown in a list.
- Reuses PathListEditor from Task 6.
- Missing paths show a warning.
- System default paths are pre-populated.

**Dependencies:** Task 6.

---

### Task 8: Define Macros Editor

**Title:** Build an editor for preprocessor macro definitions (-D flags)

**Description:** An editor for defining preprocessor macros with name and optional value.

**Implementation Details:**
- Show macros as a list of key=value pairs.
- "Add" shows a dialog with: Name input, Value input (optional, empty = defined without value).
- Common macros as quick-add buttons: DEBUG, NDEBUG, _DEBUG.
- Display: "DEBUG=1", "NDEBUG" (no value), "VERSION=\"1.0.0\"" (quoted string).
- Validate macro names: alphanumeric + underscore, must start with letter or underscore.
- Show the resulting `-D` flags in the command preview.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/CompilerConfigPanel.cpp` (macros editor)

**Acceptance Criteria:**
- Macros are shown as a list.
- Add dialog accepts name and optional value.
- Common macros are quick-addable.
- Invalid macro names are rejected.
- Command preview shows `-D` flags.

**Dependencies:** Task 3.

---

### Task 9: Optimization Level Selector

**Title:** Build optimization level selector with visual descriptions

**Description:** A segmented control or dropdown for selecting optimization levels
with descriptions of what each level does.

**Implementation Details:**
- Segmented control with options: O0, O1, O2, O3, Os, Oz.
- Below the control, show a description:
  - O0: "No optimization (fastest compile, best debug experience)"
  - O1: "Basic optimizations"
  - O2: "Full optimizations (recommended for release)"
  - O3: "Aggressive optimizations (may increase binary size)"
  - Os: "Optimize for size"
  - Oz: "Aggressively optimize for size (Clang only)"
- Gray out Oz when using GCC.
- Default: O0 for debug, O2 for release.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/CompilerConfigPanel.cpp` (optimization selector)

**Acceptance Criteria:**
- All optimization levels are selectable.
- Descriptions explain each level.
- Compiler-specific options are grayed out when unavailable.
- Default matches the build configuration context.

**Dependencies:** Task 3.

---

### Task 10: Warning Level Configuration

**Title:** Build warning level configuration with granular control

**Description:** Provide preset warning levels and granular per-warning control.

**Implementation Details:**
- Preset levels: "None", "Default", "All (-Wall)", "Extra (-Wall -Wextra)",
  "Pedantic (-Wall -Wextra -Wpedantic)", "Errors (-Wall -Wextra -Werror)".
- Selecting a preset checks/unchecks the corresponding flags in the flags editor (Task 5).
- Advanced: expandable section listing specific warning categories with toggle:
  `-Wshadow`, `-Wformat`, `-Wconversion`, `-Wsign-compare`, `-Wnon-virtual-dtor`, etc.
- Per-warning override: "Disable", "Warning", "Error" radio for each specific warning.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/CompilerConfigPanel.cpp` (warning config)

**Acceptance Criteria:**
- Preset levels work and update the flags editor.
- Advanced section shows per-warning controls.
- Per-warning overrides generate correct flag strings.
- Presets and advanced controls stay synchronized.

**Dependencies:** Task 5.

---

### Task 11: Sanitizer Options

**Title:** Build sanitizer toggle section with descriptions

**Description:** Provide toggles for Address Sanitizer, Undefined Behavior Sanitizer,
Thread Sanitizer, and Memory Sanitizer.

**Implementation Details:**
- Sanitizer checkboxes:
  - AddressSanitizer (ASan): detect buffer overflows, use-after-free.
  - UndefinedBehaviorSanitizer (UBSan): detect undefined behavior.
  - ThreadSanitizer (TSan): detect data races (mutually exclusive with ASan).
  - MemorySanitizer (MSan): detect uninitialized reads (Clang only).
  - Code coverage: `-fprofile-instr-generate -fcoverage-mapping`.
- Mutual exclusion: ASan and TSan cannot be enabled simultaneously. Show an error.
- MSan is grayed out for GCC.
- Show a note about runtime performance impact.
- Generate the corresponding compiler and linker flags.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/CompilerConfigPanel.cpp` (sanitizers)

**Acceptance Criteria:**
- All sanitizer options are toggleable.
- ASan + TSan mutual exclusion is enforced.
- MSan is grayed out for GCC.
- Generated flags are correct for each sanitizer.
- Performance impact note is displayed.

**Dependencies:** Task 3.

---

### Task 12: Output Format Selector

**Title:** Build output format selector (executable, library, object)

**Description:** Select the output type for the compilation target.

**Implementation Details:**
- Dropdown: "Executable", "Static Library (.a/.lib)", "Shared Library (.so/.dylib/.dll)",
  "Object Files (.o/.obj)".
- Selecting a format updates linker flags accordingly.
- Static library: uses `ar` instead of linker.
- Shared library: adds `-shared` flag and `-fPIC`.
- Show the expected output file extension for the current platform.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/CompilerConfigPanel.cpp` (output selector)

**Acceptance Criteria:**
- All output formats are selectable.
- Linker flags update based on format.
- Expected output extension is shown.
- Shared library auto-enables -fPIC.

**Dependencies:** Task 3.

---

### Task 13: Target Architecture Selector

**Title:** Build target architecture selector with cross-compilation support

**Description:** Select the target CPU architecture and configure cross-compilation.

**Implementation Details:**
- Architecture options: "Native" (default), "x86_64", "arm64", "armv7", "wasm32".
- "Native" uses the host architecture.
- Cross-compilation:
  - Shows additional fields: Sysroot path, Toolchain file path.
  - Validates that the cross-compiler is available.
  - Shows a warning: "Cross-compilation requires appropriate toolchain".
- For wasm32: auto-suggest Emscripten toolchain.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/CompilerConfigPanel.cpp` (architecture)

**Acceptance Criteria:**
- Architecture options are selectable.
- "Native" uses host architecture.
- Cross-compilation shows sysroot and toolchain fields.
- Wasm32 suggests Emscripten.
- Validation checks cross-compiler availability.

**Dependencies:** Task 3.

---

### Task 14: Compiler Profile Save/Load

**Title:** Save and load compiler configuration profiles

**Description:** Allow saving the current compiler configuration as a named profile
and loading it later. Profiles are separate from the general settings profiles.

**Implementation Details:**
- Create `CompilerProfileManager` extending the profile concept:
  - Save: serializes `CompilerConfig` to JSON in `~/.markamp/compiler_profiles/`.
  - Load: deserializes and applies.
  - Built-in profiles: "Debug", "Release", "Release with Debug Info", "Sanitizer Check".
- Profile selector dropdown at the top of the CompilerConfigPanel.
- "Save As..." creates a new named profile.
- "Delete" removes a custom profile (built-in profiles cannot be deleted).
- Switching profiles updates all fields in the panel.

**Files Affected:**
- New: `/Users/ryanrentfro/code/markamp/src/core/CompilerProfileManager.h`
- New: `/Users/ryanrentfro/code/markamp/src/core/CompilerProfileManager.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/CompilerConfigPanel.cpp` (profile dropdown)

**Acceptance Criteria:**
- Profiles can be saved, loaded, and deleted.
- Built-in profiles exist for Debug, Release, RelWithDebInfo, Sanitizer.
- Profile dropdown shows all available profiles.
- Switching profiles updates the panel.
- Custom profiles persist across sessions.

**Dependencies:** Task 1.

---

### Task 15: Command Preview Panel

**Title:** Show the full compiler command line generated from current settings

**Description:** A read-only preview at the bottom of the CompilerConfigPanel showing
the exact command that would be executed.

**Implementation Details:**
- Build the full command: `clang++ -std=c++23 -O2 -Wall -Wextra -I/usr/include ...`
- Show in a monospace text area.
- "Copy Command" button copies to clipboard.
- Update in real-time as any setting changes.
- Color-code parts: compiler = blue, standard = green, flags = default, paths = orange.
- Line-wrap for long commands or show with `\` continuation.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/CompilerConfigPanel.cpp` (command preview)

**Acceptance Criteria:**
- Full command is displayed in monospace.
- Command updates in real-time.
- "Copy" button copies to clipboard.
- Parts are color-coded.
- Long commands are properly wrapped.

**Dependencies:** Tasks 3, 4, 5, 6, 7, 8, 9, 11, 12, 13.

---

### Task 16: Compiler Config Settings Catalog Registration

**Title:** Register all compiler settings in the SettingsCatalog

**Description:** Register compiler configuration entries in `SettingsCatalog` so they
appear in the general settings UI and can be searched.

**Implementation Details:**
- Register entries: `compiler.path`, `compiler.standard`, `compiler.optimizationLevel`,
  `compiler.warningLevel`, `compiler.enableAsan`, `compiler.enableUbsan`,
  `compiler.enableTsan`, `compiler.includePaths`, `compiler.libraryPaths`,
  `compiler.defines`, `compiler.customFlags`, `compiler.outputType`,
  `compiler.targetArch`, `compiler.toolchainFile`.
- Group: "Compiler". Subgroup: "General", "Paths", "Sanitizers", "Output".
- Each entry has proper type, default, description.
- Scope: `ConfigScope::kWorkspace` (compiler config is typically per-project).

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/core/SettingsCatalog.cpp` (register compiler settings)

**Acceptance Criteria:**
- All compiler settings appear in the Settings UI under "Compiler".
- Settings are searchable.
- Types and defaults are correct.
- Scope is workspace-level.

**Dependencies:** Task 1, SettingsCatalog.

---

### Task 17: Compiler Config Panel Integration

**Title:** Integrate the CompilerConfigPanel into the application

**Description:** Wire the CompilerConfigPanel into the sidebar or a dedicated view
accessible from the View menu.

**Implementation Details:**
- Add "View > Compiler Configuration" menu entry.
- Register `compiler.openConfig` command in CommandRegistry.
- The panel can appear as a sidebar view or a dialog.
- Emit `CompilerConfigChangedEvent` when settings change.
- Other components (e.g., build system, Phase 38) subscribe to this event.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp` (panel registration)
- Modified: `/Users/ryanrentfro/code/markamp/src/core/Events.h` (CompilerConfigChangedEvent)
- Modified: View menu construction code.

**Acceptance Criteria:**
- "View > Compiler Configuration" opens the panel.
- Command is registered and searchable.
- Config changes emit events.
- Panel is accessible from the command palette.

**Dependencies:** Task 3.

---

### Task 18: Compiler Config Validation

**Title:** Validate the complete compiler configuration

**Description:** Run a comprehensive validation check on the current configuration
and report all issues.

**Implementation Details:**
- Validation checks:
  - Compiler path exists and is executable.
  - Selected standard is supported by the compiler.
  - Include paths exist.
  - Library paths exist.
  - No mutually exclusive sanitizers are enabled.
  - Custom flags are syntactically valid.
  - Target architecture is compatible with the compiler.
- Show a validation summary: green check for valid, yellow warning, red error.
- "Validate" button runs all checks and shows results.
- Validation runs automatically on profile load and compiler change.

**Files Affected:**
- New: `/Users/ryanrentfro/code/markamp/src/core/CompilerConfigValidator.h`
- New: `/Users/ryanrentfro/code/markamp/src/core/CompilerConfigValidator.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/CompilerConfigPanel.cpp` (validation display)

**Acceptance Criteria:**
- All validation checks are implemented.
- Results are displayed with severity icons.
- Auto-validation runs on config changes.
- "Validate" button triggers manual check.
- Green/yellow/red summary indicator is visible.

**Dependencies:** Task 1.

---

### Task 19: Compiler Config Theme Integration

**Title:** Full theme support for the compiler config panel

**Description:** Apply theme tokens to all compiler configuration UI elements.

**Implementation Details:**
- Reuse settings theme tokens where appropriate.
- Add specific tokens: `compiler.previewBackground`, `compiler.validationSuccess`,
  `compiler.validationWarning`, `compiler.validationError`.
- Color-coded command preview uses theme-derived colors.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/CompilerConfigPanel.cpp` (theme)

**Acceptance Criteria:**
- All panel elements use theme tokens.
- Theme switch updates colors.
- Command preview colors are theme-appropriate.

**Dependencies:** Task 3.

---

### Task 20: Compiler Config Persistence

**Title:** Persist compiler configuration per workspace

**Description:** Save and load compiler configuration from the workspace settings file.

**Implementation Details:**
- Store in `.markamp/compiler.json` within the workspace root.
- On workspace open, load compiler config.
- On change, save automatically (debounced 1s).
- Fall back to user-level defaults if no workspace config exists.
- Support `CompilerConfig` in `WorkspaceSettings` cascade.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/core/CompilerConfig.cpp` (persistence)
- Modified: workspace loading code.

**Acceptance Criteria:**
- Compiler config is saved per workspace.
- Config loads on workspace open.
- Changes auto-save.
- Missing workspace config falls back to user defaults.

**Dependencies:** Task 1.

---

### Task 21-25: Additional Tasks

**Task 21: Compiler Config Keyboard Shortcuts** - Register shortcuts for the panel (Cmd+Shift+K to open). Ensure navigation within the panel works with Tab/Shift+Tab.

**Task 22: Compiler Config Accessibility** - Screen reader labels for all controls. Accessible descriptions for optimization levels and sanitizer toggles.

**Task 23: Compiler Config Command Registration** - Register all compiler commands: `compiler.openConfig`, `compiler.detect`, `compiler.validate`, `compiler.switchProfile`, `compiler.copyCommand`.

**Task 24: Compiler Config Import/Export** - Export compiler config to JSON. Import from other projects. Support importing from CMakePresets.json format.

**Task 25: Unit Tests for Compiler Configuration** - Test file: `/Users/ryanrentfro/code/markamp/tests/unit/test_compiler_config.cpp`. Sections: CompilerConfig serialization, CompilerDetector (mock filesystem), CompilerConfigValidator, CompilerProfileManager (save/load/built-ins), command preview generation. At least 25 test cases.

---

### Task 26: Compiler Diagnostic Output Formatting

**Title:** Configure compiler diagnostic output format and rendering

**Description:** Allow users to control how compiler diagnostics are formatted and displayed,
including color schemes, caret rendering, template backtrace depth, and column limit for
diagnostic messages.

**Implementation Details:**
- Add diagnostic formatting fields to `CompilerConfig`:
  - `diagnostic_color_mode`: "auto", "always", "never" (maps to `-fcolor-diagnostics` / `-fno-color-diagnostics`).
  - `diagnostic_format`: "text", "sarif", "json" (maps to `-fdiagnostics-format=`).
  - `template_backtrace_limit`: int, default 10 (maps to `-ftemplate-backtrace-limit=N`).
  - `diagnostic_column_limit`: int, default 0 (unlimited) (maps to `-fmessage-length=N`).
  - `show_source_ranges`: bool (maps to `-fdiagnostics-show-ranges`).
  - `show_fix_its`: bool (maps to `-fdiagnostics-fixit-info`).
- Add a "Diagnostics" collapsible section in `CompilerConfigPanel`:
  - Color mode dropdown.
  - Output format dropdown (note: SARIF output can be consumed by ProblemsPanel).
  - Template backtrace depth spinner.
  - Fix-it display toggle.
- When SARIF format is selected, wire the output to the ProblemsPanel structured
  diagnostic viewer from Phase 38 Task 8.
- Update the command preview (Task 15) to include diagnostic flags.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/core/CompilerConfig.h` (diagnostic fields)
- Modified: `/Users/ryanrentfro/code/markamp/src/core/CompilerConfig.cpp` (serialization, validation)
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/CompilerConfigPanel.cpp` (diagnostics section)

**Acceptance Criteria:**
- Diagnostic color mode is configurable and produces correct flags.
- Output format selection generates the correct `-fdiagnostics-format=` flag.
- Template backtrace limit is configurable and validates range (0-100).
- Fix-it and source range toggles produce correct flags.
- SARIF output mode integrates with ProblemsPanel.
- Command preview reflects all diagnostic flags.

**Dependencies:** Task 1, Task 15.

---

### Task 27: Precompiled Header Configuration

**Title:** Configure precompiled header (PCH) generation and usage

**Description:** Provide UI for configuring precompiled headers to speed up compilation,
including PCH file selection, automatic PCH generation, and force-include settings.

**Implementation Details:**
- Add PCH fields to `CompilerConfig`:
  - `pch_header_path`: string, path to the PCH header file (e.g., `pch.h`).
  - `pch_output_path`: string, path for the generated `.pch`/`.gch` file.
  - `enable_pch`: bool, default false.
  - `force_include_pch`: bool, default true (maps to `-include pch.h`).
- Add a "Precompiled Headers" collapsible section in `CompilerConfigPanel`:
  - "Enable PCH" toggle.
  - "PCH Header" file picker (browse for the header file).
  - "Force Include" toggle (auto-include the PCH in every translation unit).
  - Status indicator: show whether the PCH file exists and is up-to-date.
- Generate flags:
  - Clang: `-include-pch <path>` for usage, `-x c++-header` for generation.
  - GCC: implicit via `.gch` file alongside header.
  - MSVC: `/Yc` for create, `/Yu` for use, `/Fp` for output path.
- Add a "Regenerate PCH" button that triggers PCH compilation.
- Validate: warn if the PCH header includes files that change frequently.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/core/CompilerConfig.h` (PCH fields)
- Modified: `/Users/ryanrentfro/code/markamp/src/core/CompilerConfig.cpp` (PCH serialization, flags)
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/CompilerConfigPanel.cpp` (PCH section)

**Acceptance Criteria:**
- PCH can be enabled and configured through the panel.
- PCH header path is selectable via file browser.
- Compiler-appropriate flags are generated (Clang, GCC, MSVC).
- "Regenerate PCH" button triggers recompilation of the precompiled header.
- Status indicator shows PCH freshness.
- Force-include toggle adds `-include` flag to the command preview.

**Dependencies:** Task 1, Task 3, Task 15.

---

### Task 28: Link-Time Optimization Toggle

**Title:** Configure link-time optimization (LTO) settings

**Description:** Provide a toggle and configuration for link-time optimization (LTO),
including thin LTO vs full LTO mode selection and LTO cache settings.

**Implementation Details:**
- Add LTO fields to `CompilerConfig`:
  - `lto_mode`: "off", "full", "thin" (default "off").
  - `lto_jobs`: int, parallel LTO jobs (default 0 = auto).
  - `lto_cache_dir`: string, cache directory for ThinLTO (maps to `-flto-cache-dir=`).
- Add an "LTO" section within the optimization area of `CompilerConfigPanel`:
  - LTO mode segmented control: Off | Full | Thin.
  - Description for each mode:
    - Off: "No link-time optimization."
    - Full: "Monolithic LTO. Best optimization, slowest link time."
    - Thin: "Scalable LTO. Good optimization, faster link time (Clang only)."
  - Parallel jobs spinner (only visible when LTO is enabled).
  - Cache directory picker (only visible for Thin LTO).
- Gray out "Thin" when the selected compiler is GCC (GCC only supports full LTO).
- Generate flags:
  - Clang Full: `-flto=full`.
  - Clang Thin: `-flto=thin -flto-jobs=N`.
  - GCC Full: `-flto=N` (N = parallel jobs).
- Show a note: "LTO significantly increases link time. Recommended for release builds only."
- Integrate with Profile Manager (Task 14): Release profiles enable LTO by default.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/core/CompilerConfig.h` (LTO fields)
- Modified: `/Users/ryanrentfro/code/markamp/src/core/CompilerConfig.cpp` (LTO serialization, flags)
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/CompilerConfigPanel.cpp` (LTO section)
- Modified: `/Users/ryanrentfro/code/markamp/src/core/CompilerProfileManager.cpp` (Release profile LTO defaults)

**Acceptance Criteria:**
- LTO mode is selectable (Off, Full, Thin).
- Thin LTO is grayed out for GCC.
- Parallel LTO jobs are configurable.
- ThinLTO cache directory is configurable.
- Generated flags are correct for each compiler and LTO mode.
- Release profile includes LTO=thin by default (Clang) or LTO=full (GCC).
- Performance impact note is displayed.

**Dependencies:** Task 1, Task 9, Task 14.

---

### Task 29: Compiler Cache Integration (ccache/sccache)

**Title:** Detect and configure compiler caching tools (ccache, sccache)

**Description:** Detect installed compiler cache tools and provide configuration to wrap
the compiler invocation with a caching layer for faster incremental builds.

**Implementation Details:**
- Add cache fields to `CompilerConfig`:
  - `cache_tool`: "none", "ccache", "sccache" (default "none").
  - `cache_tool_path`: string, path to the cache binary.
  - `cache_max_size`: string, cache size limit (e.g., "10G").
  - `cache_dir`: string, custom cache directory.
- Create `CompilerCacheDetector` logic (can be part of `CompilerDetector`):
  - Check for `ccache` and `sccache` in `PATH`.
  - Query cache stats: `ccache -s`, `sccache --show-stats`.
  - Extract: cache size, hit rate, max size.
- Add a "Compiler Cache" collapsible section in `CompilerConfigPanel`:
  - Cache tool dropdown: None, ccache, sccache.
  - Auto-detected tools are pre-populated.
  - Cache stats display: "Cache size: 2.3G / 10G, Hit rate: 78%".
  - "Clear Cache" button: `ccache -C` or `sccache --zero-stats`.
  - Max size configuration.
- When cache tool is enabled, prepend to compiler command:
  - `ccache clang++ ...` or use `CMAKE_CXX_COMPILER_LAUNCHER=ccache`.
- Update the command preview (Task 15) to show the cache wrapper.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/core/CompilerConfig.h` (cache fields)
- Modified: `/Users/ryanrentfro/code/markamp/src/core/CompilerConfig.cpp` (cache serialization)
- Modified: `/Users/ryanrentfro/code/markamp/src/core/CompilerDetector.h` (cache detection)
- Modified: `/Users/ryanrentfro/code/markamp/src/core/CompilerDetector.cpp` (cache detection logic)
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/CompilerConfigPanel.cpp` (cache section)

**Acceptance Criteria:**
- ccache and sccache are auto-detected on the system.
- Cache tool can be selected and configured.
- Cache stats (size, hit rate) are displayed and refreshed.
- "Clear Cache" button works for both tools.
- Compiler command preview shows the cache wrapper prefix.
- CMake integration passes `CMAKE_CXX_COMPILER_LAUNCHER` when cache tool is set.
- Missing cache tools are handled gracefully (grayed out option, no errors).

**Dependencies:** Task 1, Task 2, Task 15.

---

### Task 30: Cross-Compilation Sysroot Selector

**Title:** Provide UI for selecting and validating cross-compilation sysroots

**Description:** When cross-compilation is enabled (Task 13), provide a dedicated sysroot
selection and validation workflow including sysroot browsing, content inspection, and
automatic platform SDK detection.

**Implementation Details:**
- Add sysroot fields to `CompilerConfig`:
  - `sysroot_path`: string, path to the sysroot directory.
  - `sysroot_target_triple`: string, the target triple (e.g., "aarch64-linux-gnu").
- Add a "Sysroot" section that appears when target architecture is not "Native":
  - Sysroot path file browser (directory picker).
  - Target triple input with autocomplete:
    - Common triples: "aarch64-linux-gnu", "arm-linux-gnueabihf", "x86_64-w64-mingw32",
      "wasm32-unknown-emscripten".
  - "Detect SDKs" button:
    - macOS: scan `/Library/Developer/CommandLineTools/SDKs/` and Xcode SDK paths.
    - Linux: scan `/usr/` for cross-compilation prefixes.
    - Show detected SDKs in a list with version info.
  - Sysroot validation:
    - Check that the sysroot contains expected directories: `usr/include/`, `usr/lib/`.
    - Verify the sysroot matches the selected target architecture.
    - Show a green check or red X with diagnostic info.
  - Sysroot content inspector: expandable tree showing key directories and header presence.
- Generate flags:
  - `--sysroot=<path>` for GCC/Clang.
  - `--target=<triple>` for Clang.
  - `-DCMAKE_SYSROOT=<path>` for CMake integration.
- Store recent sysroots for quick switching.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/core/CompilerConfig.h` (sysroot fields)
- Modified: `/Users/ryanrentfro/code/markamp/src/core/CompilerConfig.cpp` (sysroot serialization, validation)
- Modified: `/Users/ryanrentfro/code/markamp/src/core/CompilerConfigValidator.h` (sysroot validation)
- Modified: `/Users/ryanrentfro/code/markamp/src/core/CompilerConfigValidator.cpp` (sysroot checks)
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/CompilerConfigPanel.cpp` (sysroot section)

**Acceptance Criteria:**
- Sysroot path is selectable via directory browser.
- Target triple input provides autocomplete for common triples.
- "Detect SDKs" finds installed platform SDKs on macOS and Linux.
- Sysroot validation checks directory structure and architecture match.
- Generated flags include `--sysroot` and `--target` correctly.
- Recent sysroots are stored for quick access.
- Sysroot section only appears when cross-compilation is active.

**Dependencies:** Task 1, Task 13, Task 18.
