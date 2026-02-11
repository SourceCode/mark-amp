# MarkAmp Dependency Reference

## Overview

MarkAmp uses [vcpkg](https://vcpkg.io/) as its package manager. All dependencies are declared in
`vcpkg.json` and pinned via `vcpkg-configuration.json` for reproducible builds.

## Dependencies

| Package           | Version  | Purpose                                                              |
| ----------------- | -------- | -------------------------------------------------------------------- |
| **wxWidgets**     | ≥ 3.2.4  | Cross-platform native UI framework (core, base, html, xml, aui, stc) |
| **nlohmann-json** | ≥ 3.11.0 | JSON parsing/serialization for theme files                           |
| **md4c**          | ≥ 0.5.0  | C Markdown parser (CommonMark + GFM extensions)                      |
| **Catch2**        | ≥ 3.5.0  | C++ testing framework (test-only)                                    |
| **fmt**           | ≥ 10.0   | String formatting (fallback for `std::format` on older compilers)    |
| **spdlog**        | ≥ 1.12.0 | Structured logging                                                   |

## Setup Instructions

### 1. Initialize vcpkg

**Option A — Submodule (recommended):**

```bash
git submodule add https://github.com/microsoft/vcpkg.git external/vcpkg
cd external/vcpkg && ./bootstrap-vcpkg.sh  # macOS/Linux
cd external/vcpkg && bootstrap-vcpkg.bat   # Windows
```

**Option B — System install:**

```bash
export VCPKG_ROOT=~/vcpkg  # point to your vcpkg installation
```

### 2. Configure and Build

```bash
cmake --preset debug       # configures with vcpkg
cmake --build --preset debug-build
```

vcpkg will automatically download and build all dependencies on first configure.

## Platform-Specific Notes

### macOS

- Requires Xcode Command Line Tools: `xcode-select --install`
- wxWidgets builds against Cocoa framework natively

### Linux

- Requires GTK3 development headers:
  ```bash
  sudo apt install build-essential pkg-config libgtk-3-dev libwebkit2gtk-4.0-dev
  ```

### Windows

- Requires Visual Studio 2022 with C++ workload
- vcpkg uses the MSVC toolchain automatically

## Adding a New Dependency

1. Add the package to `vcpkg.json`:
   ```json
   { "name": "package-name", "version>=": "x.y.z" }
   ```
2. Create a `find_package()` call in `cmake/Dependencies.cmake`
3. Add the target to the `markamp_dependencies` interface library
4. Re-run `cmake --preset debug` to fetch the new package

## Updating the vcpkg Baseline

1. Update the commit hash in both `vcpkg.json` (`builtin-baseline`) and
   `vcpkg-configuration.json` (`baseline`)
2. Use `git log` in the vcpkg submodule to find a recent stable commit
3. Re-run configure to pick up updated package versions

## Troubleshooting

| Issue                      | Solution                                                       |
| -------------------------- | -------------------------------------------------------------- |
| `Could not find wxWidgets` | Ensure vcpkg toolchain is active; check `CMAKE_TOOLCHAIN_FILE` |
| GTK errors on Linux        | Install `libgtk-3-dev` and `libwebkit2gtk-4.0-dev`             |
| Slow first build           | vcpkg compiles from source — first configure takes 10-20 min   |
| MSVC link errors           | Ensure you're using the correct triplet (x64-windows)          |
