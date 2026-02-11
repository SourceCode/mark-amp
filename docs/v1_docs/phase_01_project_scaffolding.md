# Phase 01: Project Scaffolding and CMake Setup

## Objective

Establish the foundational C++26 project structure, CMake build system with presets, and compiler configuration for all three target platforms (macOS, Windows, Linux). By the end of this phase, the project compiles an empty `main.cpp` that returns 0, using C++26 standard features, on all supported compilers.

## Prerequisites

- None (this is the first phase)

## Deliverables

1. Complete directory structure matching the project convention from the master plan
2. Root `CMakeLists.txt` with C++26 standard enforcement
3. `CMakePresets.json` with debug, release, and CI configurations
4. Placeholder `main.cpp` that compiles and runs
5. `.gitignore` configured for C++/CMake projects
6. Source subdirectory `CMakeLists.txt` files for modular builds

## Tasks

### Task 1.1: Create the root directory structure

Create all directories listed in the master plan file structure convention. Every directory must exist even if it will be populated in later phases.

**Directories to create:**
```
src/
src/app/
src/core/
src/ui/
src/rendering/
src/platform/
resources/
resources/themes/
resources/fonts/
resources/icons/
resources/sample_files/
tests/
tests/unit/
tests/integration/
scripts/
```

**Acceptance criteria:**
- All directories exist
- `ls -R` from project root shows the complete tree

### Task 1.2: Create the root CMakeLists.txt

Create `/Users/ryanrentfro/code/markamp/CMakeLists.txt` with:

- `cmake_minimum_required(VERSION 3.30)`
- `project(MarkAmp VERSION 1.0.0 LANGUAGES CXX)`
- C++26 standard enforcement: `set(CMAKE_CXX_STANDARD 26)`, `set(CMAKE_CXX_STANDARD_REQUIRED ON)`, `set(CMAKE_CXX_EXTENSIONS OFF)`
- Export compile commands for tooling: `set(CMAKE_EXPORT_COMPILE_COMMANDS ON)`
- Build type defaulting to Debug if not specified
- Platform detection variables (MARKAMP_PLATFORM_MACOS, MARKAMP_PLATFORM_WINDOWS, MARKAMP_PLATFORM_LINUX)
- `add_subdirectory(src)` and `add_subdirectory(tests)` (tests gated behind a `MARKAMP_BUILD_TESTS` option defaulting to ON)
- Compiler warning flags: `-Wall -Wextra -Wpedantic -Werror` for GCC/Clang, `/W4 /WX` for MSVC
- Sanitizer options: `MARKAMP_ENABLE_ASAN`, `MARKAMP_ENABLE_UBSAN`, `MARKAMP_ENABLE_TSAN` (all OFF by default, mutually exclusive with TSAN)
- Version info passed as compile definitions: `MARKAMP_VERSION_MAJOR`, `MARKAMP_VERSION_MINOR`, `MARKAMP_VERSION_PATCH`

**Acceptance criteria:**
- `cmake -S . -B build` succeeds without errors
- Compile commands JSON is generated in the build directory

### Task 1.3: Create CMakePresets.json

Create `/Users/ryanrentfro/code/markamp/CMakePresets.json` with the following presets:

**Configure presets:**
- `debug` -- Debug build, tests ON, ASAN ON, UBSAN ON
- `release` -- Release build, tests OFF, no sanitizers
- `ci-debug` -- Same as debug but suitable for CI (explicit generator)
- `ci-release` -- Same as release but suitable for CI

**Build presets:**
- `debug-build` referencing `debug` configure preset
- `release-build` referencing `release` configure preset

**Test presets:**
- `debug-test` referencing `debug` configure preset, using CTest

All presets should use the Ninja generator where available, falling back to platform defaults.

**Acceptance criteria:**
- `cmake --preset debug` configures successfully
- `cmake --build --preset debug-build` builds successfully
- Preset list is visible via `cmake --list-presets`

### Task 1.4: Create the src/CMakeLists.txt

Create `/Users/ryanrentfro/code/markamp/src/CMakeLists.txt` that:

- Defines the `markamp` executable target
- Currently only includes `main.cpp`
- Sets up source groups for IDE organization (app, core, ui, rendering, platform)
- Includes placeholder comments for where future source files will be added
- Links against platform libraries conditionally (e.g., Cocoa framework on macOS)

**Acceptance criteria:**
- `src/CMakeLists.txt` exists and is included by the root CMakeLists.txt
- The executable target `markamp` is defined

### Task 1.5: Create the initial main.cpp

Create `/Users/ryanrentfro/code/markamp/src/main.cpp` with:

```cpp
#include <cstdlib>
#include <print>  // C++23/26 std::print if available, else fallback

auto main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[]) -> int
{
    std::println("MarkAmp v{}.{}.{} -- Starting...",
        MARKAMP_VERSION_MAJOR, MARKAMP_VERSION_MINOR, MARKAMP_VERSION_PATCH);
    return EXIT_SUCCESS;
}
```

If `<print>` is not available on the target compiler, use `<format>` with `<iostream>` as a fallback. Include a compile-time check:

```cpp
#if __has_include(<print>)
#include <print>
#else
#include <format>
#include <iostream>
#endif
```

**Acceptance criteria:**
- `main.cpp` compiles with C++26 flags
- Running the executable prints the version string
- Return code is 0

### Task 1.6: Create the tests/CMakeLists.txt

Create `/Users/ryanrentfro/code/markamp/tests/CMakeLists.txt` that:

- Is only included when `MARKAMP_BUILD_TESTS` is ON
- Sets up CTest with `enable_testing()`
- Has placeholder comments for where test targets will be added
- Includes a minimal "smoke test" that always passes (a simple test executable that returns 0)

**Acceptance criteria:**
- Test CMakeLists.txt exists and is conditionally included
- `ctest --preset debug-test` runs and reports 0 failures (or the smoke test passes)

### Task 1.7: Create .gitignore

Create `/Users/ryanrentfro/code/markamp/.gitignore` with entries for:

- `build/` and `out/` directories
- CMake generated files (`CMakeCache.txt`, `CMakeFiles/`, `cmake_install.cmake`)
- Compiled outputs (`*.o`, `*.obj`, `*.exe`, `*.app`, `*.dylib`, `*.so`, `*.dll`)
- IDE files (`.vscode/`, `.idea/`, `*.xcodeproj`, `*.xcworkspace`)
- vcpkg installed packages (`vcpkg_installed/`)
- OS files (`.DS_Store`, `Thumbs.db`)
- Compile commands symlink (`compile_commands.json` in root)

**Acceptance criteria:**
- `.gitignore` exists at project root
- Build artifacts are excluded from git tracking

### Task 1.8: Create placeholder source files

Create empty placeholder header files with include guards for the major modules to establish the file structure:

- `src/app/MarkAmpApp.h`
- `src/core/Theme.h`
- `src/core/ThemeEngine.h`
- `src/core/FileNode.h`
- `src/core/EventBus.h`
- `src/ui/MainFrame.h`
- `src/platform/PlatformAbstraction.h`

Each file should contain:
```cpp
#pragma once

// MarkAmp -- [Module Name]
// Phase XX will implement this module.

namespace markamp::[module] {

} // namespace markamp::[module]
```

**Acceptance criteria:**
- All placeholder headers exist at their expected paths
- Each contains the correct namespace and pragma once guard

## Files Created/Modified

| File | Action |
|---|---|
| `CMakeLists.txt` | Created |
| `CMakePresets.json` | Created |
| `src/CMakeLists.txt` | Created |
| `src/main.cpp` | Created |
| `tests/CMakeLists.txt` | Created |
| `.gitignore` | Created |
| `src/app/MarkAmpApp.h` | Created (placeholder) |
| `src/core/Theme.h` | Created (placeholder) |
| `src/core/ThemeEngine.h` | Created (placeholder) |
| `src/core/FileNode.h` | Created (placeholder) |
| `src/core/EventBus.h` | Created (placeholder) |
| `src/ui/MainFrame.h` | Created (placeholder) |
| `src/platform/PlatformAbstraction.h` | Created (placeholder) |

## Dependencies

- CMake 3.30+
- A C++26-capable compiler:
  - Clang 18+ (recommended for macOS/Linux)
  - GCC 14+ (Linux)
  - MSVC 19.40+ / Visual Studio 2022 17.10+ (Windows)
- Ninja build system (recommended, optional)

## Estimated Complexity

**Medium** -- Straightforward but requires careful CMake configuration for C++26 features across three compiler families.
