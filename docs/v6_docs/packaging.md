# MarkAmp Packaging Guide — Phase 38

## Overview

MarkAmp uses CPack for multi-platform packaging. The packaging
configuration lives in `cmake/Packaging.cmake`.

## Supported Formats

| Platform | Formats       |
| -------- | ------------- |
| macOS    | DMG, ZIP      |
| Windows  | NSIS, ZIP     |
| Linux    | DEB, RPM, TGZ |

## Building Packages

```bash
# Quick build:
./scripts/build_packages.sh Release

# Manual:
cmake -B build/package -DCMAKE_BUILD_TYPE=Release
cmake --build build/package
cd build/package && cpack -C Release
```

## Configuration

Key CMake variables:

- `CPACK_PACKAGE_VERSION` — auto-set from `PROJECT_VERSION`
- `CPACK_GENERATOR` — platform-specific (auto-detected)
- `CPACK_BUNDLE_ICON` — macOS app icon

## Install Layout

```
bin/markamp                  # executable
share/markamp/               # resources (CSS, JS, HTML, icons)
share/doc/markamp/           # documentation (README, LICENSE, HISTORY)
```
