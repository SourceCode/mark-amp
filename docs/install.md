# Install Guide

> Prerequisites, dependencies, and build instructions for MarkAmp v2.3.16.

---

## Prerequisites

| Tool         | Minimum Version | Install (macOS)             | Install (Linux)           | Install (Windows)     |
| ------------ | --------------- | --------------------------- | ------------------------- | --------------------- |
| C++ Compiler | C++23 support   | Apple Clang 17+ (Xcode 16+) | GCC 14+ / Clang 17+       | MSVC 17.8+ (VS 2022)  |
| CMake        | 3.30+           | `brew install cmake`        | `apt install cmake`       | `choco install cmake` |
| Ninja        | latest          | `brew install ninja`        | `apt install ninja-build` | `choco install ninja` |
| Git          | latest          | `brew install git`          | `apt install git`         | `choco install git`   |
| pkg-config   | latest          | `brew install pkg-config`   | `apt install pkg-config`  | via vcpkg             |

## vcpkg Dependencies

MarkAmp uses vcpkg in **manifest mode** — dependencies are declared in `vcpkg.json` and automatically downloaded during CMake configuration. The vcpkg installation is bundled at `external/vcpkg/`.

### Dependency List

| Package       | Purpose                             |
| ------------- | ----------------------------------- |
| wxwidgets     | Cross-platform UI toolkit           |
| nlohmann-json | JSON parsing                        |
| md4c          | Markdown parsing                    |
| catch2        | Unit testing framework              |
| fmt           | String formatting                   |
| spdlog        | Logging                             |
| yaml-cpp      | YAML/config file parsing            |
| nanosvg       | SVG icon rendering                  |
| libzip        | ZIP/VSIX handling                   |
| openssl       | Cryptography (AES-256 encryption)   |
| cpp-httplib   | HTTP client                         |
| sqlite3       | Database with FTS5 full-text search |
| tracy         | Frame profiling                     |
| benchmark     | Google Benchmark                    |
| mimalloc      | High-performance memory allocator   |

## Platform-Specific Setup

### macOS

```bash
# Install Xcode command-line tools
xcode-select --install

# Install build tools
brew install cmake ninja pkg-config

# Clone and build
git clone https://github.com/markamp/markamp.git
cd markamp
cmake --preset debug
cmake --build build/debug -j$(sysctl -n hw.ncpu)
```

### Linux (Debian/Ubuntu)

```bash
# Install build dependencies
sudo apt update
sudo apt install -y build-essential cmake ninja-build pkg-config \
    libgtk-3-dev libwebkit2gtk-4.0-dev libgl1-mesa-dev

# Clone and build
git clone https://github.com/markamp/markamp.git
cd markamp
cmake --preset debug
cmake --build build/debug -j$(nproc)
```

### Windows

```powershell
# Install with Chocolatey
choco install cmake ninja git visualstudio2022-workload-nativedesktop

# Clone and build
git clone https://github.com/markamp/markamp.git
cd markamp
cmake --preset debug
cmake --build build/debug -j $env:NUMBER_OF_PROCESSORS
```

## Build Presets

| Preset             | Description                         | Tests | Output                    |
| ------------------ | ----------------------------------- | ----- | ------------------------- |
| `debug`            | Development build                   | ON    | `build/debug/`            |
| `release`          | Optimized release                   | OFF   | `build/release/`          |
| `release-static`   | Static linking for distribution     | OFF   | `build/release-static/`   |
| `debug-asan`       | AddressSanitizer + UBSan            | ON    | `build/debug-asan/`       |
| `debug-tsan`       | ThreadSanitizer                     | ON    | `build/debug-tsan/`       |
| `debug-lsan`       | LeakSanitizer                       | ON    | `build/debug-lsan/`       |
| `debug-clang-tidy` | Static analysis during build        | ON    | `build/debug-clang-tidy/` |
| `release-lto`      | Release with link-time optimization | OFF   | `build/release-lto/`      |
| `profile`          | RelWithDebInfo + Tracy profiling    | ON    | `build/profile/`          |

```bash
# Use any preset
cmake --preset <preset-name>
cmake --build build/<preset-name> -j$(sysctl -n hw.ncpu)
```

## Common Build Failures

| Error                              | Cause                       | Solution                                                  |
| ---------------------------------- | --------------------------- | --------------------------------------------------------- |
| `Cannot find wxWidgets`            | vcpkg not configured        | Ensure `VCPKG_ROOT` is set or `external/vcpkg/` exists    |
| `CMake version too old`            | CMake < 3.30                | Update CMake: `brew upgrade cmake`                        |
| `fatal error: 'wx/wx.h' not found` | wxWidgets headers missing   | Run `cmake --preset debug` again to trigger vcpkg install |
| `ninja: command not found`         | Missing Ninja               | Install: `brew install ninja`                             |
| Duplicate symbol errors            | Source files compiled twice | Check `src/CMakeLists.txt` for duplicate entries          |

## Verifying the Build

```bash
# Run the binary
./build/debug/markamp

# Run all tests
cd build/debug && ctest --output-on-failure

# Expected: 451 test targets
cd build/debug && ctest -N | tail -1
```
