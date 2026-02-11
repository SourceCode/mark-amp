# Platform Notes – MarkAmp

## macOS

### Build Requirements

- **Xcode Command Line Tools** (or full Xcode)
- **CMake 3.30+** via Homebrew: `brew install cmake`
- **vcpkg** (bundled in `external/vcpkg`)
- **Fonts**: JetBrains Mono and Rajdhani are embedded in the application bundle

### Build Instructions

```bash
cmake --preset macos-release
cmake --build build --config Release
```

### Platform-Specific Features

- **Native fullscreen**: Uses `NSWindow toggleFullScreen:` for smooth macOS animations
- **Window drag**: Uses `performWindowDragWithEvent:` for native feel
- **Dark mode detection**: Queries `NSAppearance.effectiveAppearance`
- **Content scale factor**: Reports via `NSScreen.backingScaleFactor` (2.0 on Retina)
- **High contrast**: Queries `accessibilityDisplayShouldIncreaseContrast`
- **Reduced motion**: Queries `accessibilityDisplayShouldReduceMotion`
- **Screen reader**: Posts `NSAccessibilityAnnouncementRequestedNotification`
- **Menu bar**: Full File/Edit/View/Window/Help menu bar (required by macOS HIG)

### Known Quirks

- Custom chrome hides standard traffic light buttons and uses custom controls
- `titlebarAppearsTransparent` is set to YES for seamless chrome
- Objective-C++ (`.mm`) files use ARC-compatible patterns with `@autoreleasepool`

---

## Windows

### Build Requirements

- **Visual Studio 2022** (C++26 support)
- **CMake 3.30+**
- **vcpkg** (bundled in `external/vcpkg`)

### Build Instructions

```cmd
cmake --preset windows-release
cmake --build build --config Release
```

### Platform-Specific Features

- **Native drag**: Uses `WM_NCLBUTTONDOWN` with `HTCAPTION` for title bar drag
- **Native resize**: Maps `ResizeEdge` to `HT*` hit-test constants for edge resizing
- **DPI awareness**: Per-monitor DPI via `GetDeviceCaps(LOGPIXELSX)` (V2 planned)
- **High contrast**: Queries `SystemParametersInfoW(SPI_GETHIGHCONTRAST)`
- **Reduced motion**: Queries `SystemParametersInfoW(SPI_GETCLIENTAREAANIMATION)`
- **Dark mode**: Reads `HKCU\Software\Microsoft\Windows\CurrentVersion\Themes\Personalize\AppsUseLightTheme`

### Known Quirks

- UIA (UI Automation) screen reader support is stubbed — requires `UIAutomationCore.lib` linkage
- Code signing must be configured to avoid SmartScreen warnings
- Long path support (`>260 chars`) requires `\\?\` prefix on Win32 API calls

### Required Dependencies

- Windows 10+ (64-bit)
- MSVC v19.38+ (C++26 features)

---

## Linux

### Build Requirements

- **GCC 14+** or **Clang 18+** (C++26 support)
- **CMake 3.30+**
- **vcpkg** (bundled in `external/vcpkg`)
- **System packages**: `libgtk-3-dev`, `libwebkit2gtk-4.0-dev` (for wxWidgets GTK3 backend)

### Build Instructions

```bash
cmake --preset linux-release
cmake --build build --config Release
```

### Platform-Specific Features

- **Native drag**: Uses `gdk_window_begin_move_drag()` on both X11 and Wayland
- **Native resize**: Uses `gdk_window_begin_resize_drag()` with GDK edge constants
- **High contrast**: Checks GTK theme name for "HighContrast" or "Adwaita-hc"
- **Reduced motion**: Checks `GTK_ENABLE_ANIMATIONS` env var and `gtk-enable-animations` setting
- **Dark mode**: Queries `gtk-application-prefer-dark-theme` and theme name for "dark"
- **Content scale factor**: Via `gdk_monitor_get_scale_factor()` for display-server-aware scaling
- **Desktop integration**: `.desktop` file at `packaging/linux/markamp.desktop`

### Known Quirks

- Wayland vs X11: custom chrome drag may behave differently; wxWidgets abstracts most differences
- AT-SPI2 screen reader support is stubbed — requires `libatspi2` linkage for full integration
- File dialogs use native GTK dialogs automatically via wxWidgets
- Font fallbacks: if JetBrains Mono is not available, system falls back to DejaVu Sans Mono / Noto Sans Mono

### Required System Packages (Ubuntu 22.04)

```bash
sudo apt install build-essential cmake libgtk-3-dev libwebkit2gtk-4.0-dev \
    fonts-jetbrains-mono fonts-noto libfontconfig-dev
```

### Required System Packages (Fedora 38+)

```bash
sudo dnf install cmake gcc-c++ gtk3-devel webkit2gtk4.0-devel \
    jetbrains-mono-fonts google-noto-sans-mono-fonts fontconfig-devel
```

---

## Performance Characteristics

| Metric         | macOS (M1)     | Windows 10     | Ubuntu 22.04   |
| -------------- | -------------- | -------------- | -------------- |
| Cold start     | Target < 500ms | Target < 500ms | Target < 500ms |
| Theme switch   | Target < 50ms  | Target < 50ms  | Target < 50ms  |
| Scroll FPS     | 60 fps         | 60 fps         | 60 fps         |
| Mermaid render | Target < 2s    | Target < 2s    | Target < 2s    |

---

## Missing Features (All Platforms)

- Touch Bar support (macOS) — not implemented, low priority
- Windows Snap Layouts — partial support via `WM_GETMINMAXINFO` (stubbed)
- Jump List / Recent Files (Windows) — deferred to Phase 29
- AT-SPI2 full support (Linux) — stubbed, requires additional library linkage
