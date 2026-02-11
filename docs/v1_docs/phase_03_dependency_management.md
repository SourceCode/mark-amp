# Phase 03: Dependency Management with vcpkg

## Objective

Set up vcpkg as the package manager for all third-party dependencies. Configure the vcpkg manifest (`vcpkg.json`) with all required libraries, create vcpkg integration with CMake, and verify that all dependencies can be built and linked on all three target platforms.

## Prerequisites

- Phase 01 (Project Scaffolding and CMake Setup)

## Deliverables

1. `vcpkg.json` manifest with all project dependencies declared
2. `vcpkg-configuration.json` for registry and baseline pinning
3. CMake toolchain integration for vcpkg
4. Verified builds of all dependencies on the development platform
5. Triplet configurations for cross-platform builds

## Tasks

### Task 3.1: Initialize vcpkg submodule or bootstrap

Choose one of two approaches (submodule is recommended for reproducibility):

**Option A (Submodule):**
- Add vcpkg as a git submodule at `/Users/ryanrentfro/code/markamp/external/vcpkg`
- Run the bootstrap script to build the vcpkg executable

**Option B (System vcpkg):**
- Document the requirement for vcpkg to be installed system-wide
- Set `VCPKG_ROOT` environment variable

For either option, update `CMakeLists.txt` to detect and use vcpkg:
```cmake
if(DEFINED ENV{VCPKG_ROOT})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
elseif(EXISTS "${CMAKE_SOURCE_DIR}/external/vcpkg/scripts/buildsystems/vcpkg.cmake")
    set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/external/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
endif()
```

**Acceptance criteria:**
- vcpkg is available and functional
- CMake detects the vcpkg toolchain file

### Task 3.2: Create vcpkg.json manifest

Create `/Users/ryanrentfro/code/markamp/vcpkg.json` with all required dependencies:

```json
{
    "name": "markamp",
    "version-semver": "1.0.0",
    "description": "Cross-platform Markdown viewer and editor with retro-futuristic UI",
    "homepage": "https://github.com/markamp/markamp",
    "dependencies": [
        {
            "name": "wxwidgets",
            "version>=": "3.2.9"
        },
        {
            "name": "nlohmann-json",
            "version>=": "3.11.0"
        },
        {
            "name": "md4c",
            "version>=": "0.5.0"
        },
        {
            "name": "catch2",
            "version>=": "3.5.0"
        },
        {
            "name": "fmt",
            "version>=": "10.0.0"
        },
        {
            "name": "spdlog",
            "version>=": "1.12.0"
        }
    ],
    "overrides": [],
    "builtin-baseline": ""
}
```

Note: The `builtin-baseline` will be set to the actual vcpkg commit hash during setup. `fmt` is included as a fallback for `std::format` on compilers with incomplete C++26 support. `spdlog` provides structured logging.

**Acceptance criteria:**
- `vcpkg.json` is valid JSON
- `vcpkg install` (or CMake configure) resolves all dependencies
- All listed packages are available in vcpkg

### Task 3.3: Create vcpkg-configuration.json

Create `/Users/ryanrentfro/code/markamp/vcpkg-configuration.json`:

```json
{
    "default-registry": {
        "kind": "git",
        "repository": "https://github.com/microsoft/vcpkg",
        "baseline": "<commit-hash>"
    },
    "registries": []
}
```

The baseline should be pinned to a recent stable vcpkg commit to ensure reproducible builds.

**Acceptance criteria:**
- Configuration file exists and is valid
- vcpkg uses the pinned baseline during dependency resolution

### Task 3.4: Update CMakePresets.json for vcpkg

Update `/Users/ryanrentfro/code/markamp/CMakePresets.json` to include vcpkg toolchain in all configure presets:

- Add `"toolchainFile"` pointing to the vcpkg CMake toolchain
- Or add `"cacheVariables": { "CMAKE_TOOLCHAIN_FILE": "..." }`
- Ensure both submodule and system vcpkg paths are handled

**Acceptance criteria:**
- `cmake --preset debug` configures with vcpkg and finds all dependencies
- No manual `CMAKE_TOOLCHAIN_FILE` override needed

### Task 3.5: Create CMake find-and-link module for dependencies

Create `/Users/ryanrentfro/code/markamp/cmake/Dependencies.cmake` that:

- Uses `find_package()` for each dependency:
  - `find_package(wxWidgets REQUIRED COMPONENTS core base html xml aui stc)`
  - `find_package(nlohmann_json REQUIRED)`
  - `find_package(md4c REQUIRED)` (or manual find if vcpkg port uses different name)
  - `find_package(Catch2 REQUIRED)` (only when tests enabled)
  - `find_package(fmt REQUIRED)`
  - `find_package(spdlog REQUIRED)`
- Defines an interface library `markamp::dependencies` that aggregates all link targets
- Exports the dependency targets for use in `src/CMakeLists.txt`

**Acceptance criteria:**
- `cmake --preset debug` finds all packages without errors
- The aggregated dependency target can be linked against

### Task 3.6: Update src/CMakeLists.txt to link dependencies

Update `/Users/ryanrentfro/code/markamp/src/CMakeLists.txt` to:

- Include the Dependencies module
- Link the `markamp` executable against all required libraries
- Use `target_link_libraries(markamp PRIVATE ...)` with proper targets
- Add wxWidgets include directories and definitions via its CMake targets

**Acceptance criteria:**
- The project compiles and links with all dependencies
- No undefined symbol errors at link time

### Task 3.7: Create a dependency verification test

Create `/Users/ryanrentfro/code/markamp/tests/unit/test_dependencies.cpp` that:

- Includes headers from each dependency: `<wx/wx.h>`, `<nlohmann/json.hpp>`, `<md4c.h>`, `<fmt/format.h>`, `<spdlog/spdlog.h>`
- Has a simple Catch2 test case for each that verifies the library is usable:
  - nlohmann/json: parse a JSON string
  - md4c: call `md_parse` on a trivial markdown string
  - fmt: format a string
  - spdlog: create a logger

Update `tests/CMakeLists.txt` to build and register this test.

**Acceptance criteria:**
- `ctest --preset debug-test` runs the dependency test and it passes
- Each dependency's header is includable and its basic API is callable

### Task 3.8: Document dependency versions and platform notes

Create `/Users/ryanrentfro/code/markamp/docs/v1_docs/dependencies.md` documenting:

- Each dependency, its version, its purpose in MarkAmp
- Platform-specific build notes (e.g., wxWidgets needs GTK3-dev on Linux)
- How to add new dependencies
- How to update the vcpkg baseline
- Troubleshooting common build issues

**Acceptance criteria:**
- Document exists and is accurate
- A new developer can follow it to set up the build environment

## Files Created/Modified

| File | Action |
|---|---|
| `vcpkg.json` | Created |
| `vcpkg-configuration.json` | Created |
| `CMakePresets.json` | Modified (add vcpkg toolchain) |
| `CMakeLists.txt` | Modified (add vcpkg detection) |
| `cmake/Dependencies.cmake` | Created |
| `src/CMakeLists.txt` | Modified (link dependencies) |
| `tests/unit/test_dependencies.cpp` | Created |
| `tests/CMakeLists.txt` | Modified (add dependency test) |
| `docs/v1_docs/dependencies.md` | Created |

## Dependencies

- vcpkg (latest)
- All packages listed in vcpkg.json
- Platform build tools:
  - macOS: Xcode command line tools
  - Linux: `build-essential`, `pkg-config`, `libgtk-3-dev`, `libwebkit2gtk-4.0-dev`
  - Windows: Visual Studio 2022 with C++ workload

## Estimated Complexity

**Medium** -- vcpkg setup is well-documented, but wxWidgets can have platform-specific build complications, especially on Linux where GTK development headers are required.
