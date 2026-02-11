# Phase 02: Code Quality Tooling Pipeline

## Objective

Establish a comprehensive code quality pipeline including static analysis (clang-tidy, cppcheck), code formatting (clang-format), include hygiene (include-what-you-use), and runtime sanitizers (ASAN, UBSAN, TSAN). Every subsequent phase must pass this quality gate before being considered complete.

## Prerequisites

- Phase 01 (Project Scaffolding and CMake Setup)

## Deliverables

1. `.clang-tidy` configuration file with strict rulesets
2. `.clang-format` configuration file matching project style
3. CMake integration for all quality tools
4. Shell scripts for running format checks, linting, and full analysis
5. Pre-commit quality gate documentation

## Tasks

### Task 2.1: Create .clang-format configuration

Create `/Users/ryanrentfro/code/markamp/.clang-format` with these rules:

- `BasedOnStyle: LLVM`
- `IndentWidth: 4`
- `ColumnLimit: 100`
- `UseTab: Never`
- `BreakBeforeBraces: Allman` (opening braces on new lines for functions/classes)
- `AllowShortFunctionsOnASingleLine: Empty`
- `AllowShortIfStatementsOnASingleLine: Never`
- `AllowShortLoopsOnASingleLine: false`
- `AlwaysBreakTemplateDeclarations: Yes`
- `PointerAlignment: Left` (e.g., `int* ptr`)
- `NamespaceIndentation: None`
- `FixNamespaceComments: true`
- `SortIncludes: CaseSensitive`
- `IncludeBlocks: Regroup`
- Include categories ordered: project headers, library headers, system headers
- `SpaceAfterCStyleCast: false`
- `SpacesInParentheses: false`
- `Cpp11BracedListStyle: true`

**Acceptance criteria:**
- Running `clang-format --dry-run --Werror src/main.cpp` either passes or identifies formatting issues
- The style is consistent and readable

### Task 2.2: Create .clang-tidy configuration

Create `/Users/ryanrentfro/code/markamp/.clang-tidy` with strict checks:

**Enabled check groups:**
- `bugprone-*`
- `cert-*`
- `cppcoreguidelines-*`
- `hicpp-*`
- `misc-*`
- `modernize-*`
- `performance-*`
- `portability-*`
- `readability-*`

**Disabled checks (too noisy or incompatible with C++26):**
- `modernize-use-trailing-return-type` (allow both styles)
- `cppcoreguidelines-avoid-magic-numbers` (too strict for color values)
- `readability-magic-numbers` (same reason)
- `hicpp-signed-bitwise` (common with wxWidgets)
- `misc-non-private-member-variables-in-classes` (allow for POD-like structs)

**Additional settings:**
- `WarningsAsErrors: ''` (initially do not block, tighten over time)
- `HeaderFilterRegex: 'src/.*'` (only analyze project headers)
- `CheckOptions` for naming conventions:
  - Classes: `CamelCase`
  - Functions: `camelCase`
  - Variables: `camelCase`
  - Constants: `kCamelCase`
  - Namespaces: `lower_case`
  - Enum members: `kCamelCase`

**Acceptance criteria:**
- `clang-tidy src/main.cpp` runs without crashing
- Configuration file is valid YAML

### Task 2.3: Create cppcheck configuration

Create `/Users/ryanrentfro/code/markamp/.cppcheck` (or a suppressions file `cppcheck-suppressions.txt`) with:

- Enable all checks: `--enable=all`
- Suppress `missingIncludeSystem` (system includes may not be found)
- Suppress `unmatchedSuppression` (avoid circular issues)
- Set C++ standard: `--std=c++26` (or closest supported)
- Inline suppression support enabled

Also add cppcheck support to CMake by creating a `cmake/CppCheck.cmake` module that:
- Finds the cppcheck executable
- Sets `CMAKE_CXX_CPPCHECK` when found
- Configures the suppressions file path

**Acceptance criteria:**
- `cppcheck --project=build/compile_commands.json` runs without internal errors
- Suppressions file is loaded correctly

### Task 2.4: Configure sanitizers in CMake

Update `/Users/ryanrentfro/code/markamp/CMakeLists.txt` to add a `cmake/Sanitizers.cmake` module that:

- Defines function `markamp_configure_sanitizers(target_name)`
- When `MARKAMP_ENABLE_ASAN` is ON: adds `-fsanitize=address -fno-omit-frame-pointer`
- When `MARKAMP_ENABLE_UBSAN` is ON: adds `-fsanitize=undefined`
- When `MARKAMP_ENABLE_TSAN` is ON: adds `-fsanitize=thread`
- Validates that ASAN/UBSAN and TSAN are not enabled simultaneously (fatal error)
- Applies flags to both compile and link options of the target
- On MSVC, uses `/fsanitize=address` for ASAN (UBSAN/TSAN not natively supported, skip with warning)

**Acceptance criteria:**
- Debug preset builds with ASAN+UBSAN enabled
- Running the executable under ASAN produces no errors for the placeholder main.cpp
- Attempting to enable ASAN+TSAN simultaneously produces a CMake error

### Task 2.5: Configure include-what-you-use (iwyu) in CMake

Create `/Users/ryanrentfro/code/markamp/cmake/IWYU.cmake` module that:

- Finds the `include-what-you-use` executable
- When found and `MARKAMP_ENABLE_IWYU` option is ON:
  - Sets `CMAKE_CXX_INCLUDE_WHAT_YOU_USE` to the iwyu executable path
  - Adds mapping files for wxWidgets and standard library if needed
- When not found: prints a status message but does not error

**Acceptance criteria:**
- When iwyu is installed, it runs during compilation and reports include suggestions
- When iwyu is not installed, the build proceeds normally

### Task 2.6: Create format checking script

Create `/Users/ryanrentfro/code/markamp/scripts/format.sh` (executable, bash):

```bash
#!/usr/bin/env bash
# Format all C++ source files, or check formatting in CI mode.
# Usage: ./scripts/format.sh [--check]
```

The script should:
- Find all `.h`, `.cpp` files under `src/` and `tests/`
- If `--check` flag is provided: run `clang-format --dry-run --Werror` on each file, exit 1 on failure
- If no flag: run `clang-format -i` on each file (in-place formatting)
- Report count of files processed

**Acceptance criteria:**
- `./scripts/format.sh` formats all source files in place
- `./scripts/format.sh --check` returns 0 when all files are formatted, 1 otherwise

### Task 2.7: Create lint checking script

Create `/Users/ryanrentfro/code/markamp/scripts/lint.sh` (executable, bash):

```bash
#!/usr/bin/env bash
# Run clang-tidy and cppcheck on the project.
# Usage: ./scripts/lint.sh [--fix]
```

The script should:
- Require a `build/compile_commands.json` to exist (error with message if not)
- Run `clang-tidy` on all source files with the project's `.clang-tidy` config
- If `--fix` is provided, pass `--fix` to clang-tidy
- Run `cppcheck` with the compile commands database
- Report summary of warnings/errors from each tool
- Exit with non-zero if any tool reports errors

**Acceptance criteria:**
- `./scripts/lint.sh` runs both tools and produces a combined report
- Exit code reflects whether issues were found

### Task 2.8: Create cmake modules directory and CMake quality integration

Create `/Users/ryanrentfro/code/markamp/cmake/` directory and populate it with:

- `cmake/Sanitizers.cmake` (from Task 2.4)
- `cmake/CppCheck.cmake` (from Task 2.3)
- `cmake/IWYU.cmake` (from Task 2.5)
- `cmake/CompilerWarnings.cmake` -- extracted from root CMakeLists.txt, defines `markamp_set_warnings(target_name)` function

Update root `CMakeLists.txt` to:
- Add `cmake/` to `CMAKE_MODULE_PATH`
- Include all cmake modules
- Apply warnings and sanitizers to the `markamp` target

**Acceptance criteria:**
- `cmake --preset debug` configures with all quality tools detected
- Building with debug preset applies all warnings and sanitizers
- CMake modules are cleanly separated and reusable

## Files Created/Modified

| File | Action |
|---|---|
| `.clang-format` | Created |
| `.clang-tidy` | Created |
| `cppcheck-suppressions.txt` | Created |
| `cmake/Sanitizers.cmake` | Created |
| `cmake/CppCheck.cmake` | Created |
| `cmake/IWYU.cmake` | Created |
| `cmake/CompilerWarnings.cmake` | Created |
| `scripts/format.sh` | Created |
| `scripts/lint.sh` | Created |
| `CMakeLists.txt` | Modified (add cmake module path, include modules) |

## Dependencies

- clang-format 18+ (matching clang version)
- clang-tidy 18+
- cppcheck 2.x
- include-what-you-use (optional, latest compatible with clang version)

## Estimated Complexity

**Medium** -- Standard tooling configuration but requires careful tuning of clang-tidy checks to avoid false positives with wxWidgets patterns.
