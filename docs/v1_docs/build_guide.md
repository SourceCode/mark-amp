# Build and Packaging Guide

Comprehensive instructions for building MarkAmp from source, running tests, and creating distributable packages for all three platforms.

## Prerequisites

### All Platforms

| Tool         | Version       | Purpose                        |
| ------------ | ------------- | ------------------------------ |
| CMake        | ≥ 3.30        | Build system                   |
| Ninja        | latest        | Build generator                |
| C++ compiler | C++23 support | Clang 17+, GCC 14+, MSVC 17.8+ |
| vcpkg        | (bundled)     | Dependency manager             |
| Git          | latest        | Source control                 |

### macOS

```bash
xcode-select --install        # Command-line tools
brew install cmake ninja       # Build tools
```

### Windows

```powershell
# Visual Studio 2022 with "Desktop development with C++" workload
choco install cmake ninja      # Or install via Visual Studio Installer
choco install nsis             # For installer creation (optional)
```

### Linux (Ubuntu/Debian)

```bash
sudo apt-get install -y \
    build-essential cmake ninja-build \
    libgtk-3-dev libwebkit2gtk-4.0-dev \
    libnotify-dev libsecret-1-dev \
    pkg-config
```

### Linux (Fedora)

```bash
sudo dnf install -y \
    gcc-c++ cmake ninja-build \
    gtk3-devel webkit2gtk4.0-devel \
    libnotify-devel libsecret-devel \
    pkgconf-pkg-config
```

---

## Building from Source

### Quick Start (macOS/Linux)

```bash
# Debug build + tests
./scripts/build_debug.sh

# Release build (optimized, no tests)
./scripts/build_release.sh

# Run all tests
./scripts/run_tests.sh

# Clean everything
./scripts/clean.sh
```

### Using CMake Presets

```bash
# Debug
cmake --preset debug
cmake --build --preset debug-build --parallel

# Release
cmake --preset release
cmake --build --preset release-build --parallel

# Release (static linking for distribution)
cmake --preset release-static
cmake --build --preset release-static-build --parallel
```

### Manual CMake

```bash
cmake -B build -G Ninja \
    -DCMAKE_BUILD_TYPE=Debug \
    -DMARKAMP_BUILD_TESTS=ON \
    -DCMAKE_TOOLCHAIN_FILE=external/vcpkg/scripts/buildsystems/vcpkg.cmake

cmake --build build --parallel
```

---

## Running Tests

```bash
# All tests
cd build && ctest --output-on-failure

# Specific test
ctest -R test_theme --output-on-failure

# With sanitizers
cmake -B build -DMARKAMP_ENABLE_ASAN=ON -DMARKAMP_ENABLE_UBSAN=ON
cmake --build build --parallel
cd build && ctest --output-on-failure
```

---

## Packaging

### macOS (.dmg)

```bash
./scripts/build_release.sh
./scripts/package_macos.sh
# Output: dist/MarkAmp-macOS.dmg
```

**Code signing** (requires Apple Developer ID):

```bash
export MARKAMP_DEVELOPER_ID="Developer ID Application: Your Name (TEAM_ID)"
./scripts/package_macos.sh
```

**Notarization** (required for Gatekeeper):

```bash
export MARKAMP_APPLE_ID="your@email.com"
export MARKAMP_APPLE_PASSWORD="app-specific-password"
export MARKAMP_TEAM_ID="TEAM_ID"
./scripts/package_macos.sh
```

### Mac App Store (.pkg)

```bash
export MARKAMP_MAS_APP_CERT="3rd Party Mac Developer Application: Your Name"
export MARKAMP_MAS_INSTALLER_CERT="3rd Party Mac Developer Installer: Your Name"
export MARKAMP_PROVISIONING_PROFILE="/path/to/profile.provisionprofile"
./scripts/package_macos_appstore.sh
# Output: dist/MarkAmp-macOS-AppStore.pkg
```

### Windows (Installer + Portable)

```powershell
.\scripts\build_release.sh   # Or cmake --build build/release
.\scripts\package_windows.ps1
# Output: dist/MarkAmp-<version>-Setup.exe
#         dist/MarkAmp-<version>-Windows-Portable.zip
```

### Windows (MSIX for Microsoft Store)

```powershell
.\scripts\package_windows_msix.ps1
# Output: dist/MarkAmp-<version>.msix
```

### Linux (.deb / AppImage / tarball)

```bash
./scripts/build_release.sh
./scripts/package_linux.sh
# Output: dist/markamp_<version>_amd64.deb
#         dist/MarkAmp-<version>-x86_64.AppImage
#         dist/MarkAmp-<version>-linux-x86_64.tar.gz
```

---

## CI/CD Pipeline

### Continuous Integration (`ci.yml`)

Runs on every push to `main`/`develop` and on pull requests.

| Step        | Details                                          |
| ----------- | ------------------------------------------------ |
| Platforms   | macOS, Windows, Ubuntu                           |
| Build types | Debug (with tests)                               |
| Caching     | vcpkg packages (keyed on `vcpkg.json` hash + OS) |
| Checks      | Build, CTest, clang-format                       |

### Release Pipeline (`release.yml`)

Triggered by pushing a version tag (`v*`).

1. Builds Release on all 3 platforms in parallel
2. Runs full test suite
3. Creates platform packages
4. Generates changelog from git history
5. Creates GitHub Release with all artifacts

---

## Release Process

```bash
# 1. Update version everywhere
./scripts/bump_version.sh 1.1.0

# 2. Push tag to trigger release pipeline
git push origin main --tags

# 3. GitHub Actions will:
#    - Build for macOS, Windows, Linux
#    - Create packages
#    - Publish GitHub Release with assets
```

### App Store Submissions

**Mac App Store:**

1. Build with `package_macos_appstore.sh`
2. Upload `.pkg` via Transporter or `xcrun altool`
3. Complete metadata in App Store Connect
4. Submit for review

**Microsoft Store:**

1. Build with `package_windows_msix.ps1`
2. Validate with Windows App Certification Kit
3. Upload to Microsoft Partner Center
4. Complete listing and submit

---

## Troubleshooting

### vcpkg fails to bootstrap

```bash
# Re-clone vcpkg
rm -rf external/vcpkg
git submodule update --init --recursive
```

### CMake can't find wxWidgets

```bash
# Ensure vcpkg installed it
ls vcpkg_installed/*/lib/libwx*
# If missing, re-run vcpkg install
./external/vcpkg/vcpkg install
```

### Ninja not found (macOS)

```bash
brew install ninja
# Or update CMakePresets.json to remove the hardcoded path
```

### Linux: missing GTK/WebKit headers

```bash
sudo apt-get install libgtk-3-dev libwebkit2gtk-4.0-dev
```

### Windows: MSVC not found

Ensure Visual Studio 2022 is installed with the "Desktop development with C++" workload, then run from a "Developer Command Prompt" or "x64 Native Tools Command Prompt."

### Build takes too long

```bash
# Use more parallel jobs
cmake --build build --parallel $(nproc)

# Or use ccache
sudo apt-get install ccache  # Linux
brew install ccache           # macOS
cmake -B build -DCMAKE_CXX_COMPILER_LAUNCHER=ccache
```
