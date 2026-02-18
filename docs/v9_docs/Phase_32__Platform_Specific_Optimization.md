# Phase 32: Platform-Specific Optimization

## Overview
MacPlatform.mm (Objective-C++), WinPlatform.cpp, and LinuxPlatform.cpp exist behind PlatformAbstraction.h. Platform-specific features (native title bar integration, system tray, file dialogs, notifications, keychain, appearance settings) are minimally implemented. This phase completes platform-native integration for macOS, Windows, and Linux.

## Prerequisites
- Phase 01 (Core infrastructure)
- Phase 03 (Theme engine for native appearance)
- Phase 31 (Accessibility for platform accessibility APIs)

## Tasks

### Task 1: Wire macOS Native Title Bar Integration
**Files:** `src/platform/MacPlatform.mm`, `src/ui/CustomChrome.cpp`
**Description:** macOS native title bar with traffic lights (close/minimize/maximize). Toolbar area integrated with wxWidgets frame. Full-screen support.
**Acceptance Criteria:**
- Native traffic lights positioned correctly
- Toolbar area blends with title bar
- Full-screen mode (green button) works
- Title bar transparent with content behind
- Drag area for window movement
- Tab bar integrated with title bar space

### Task 2: Wire macOS System Services
**Files:** `src/platform/MacPlatform.mm`
**Description:** macOS system services: Keychain for credentials, NSUserNotificationCenter for notifications, NSAppearance for dark/light mode, Spotlight metadata.
**Acceptance Criteria:**
- Keychain: store/retrieve credentials
- Notifications: native notification center
- Dark mode: detect and follow system appearance
- Spotlight: write document metadata for system search
- Handoff: support Handoff between Apple devices
- Services menu: "Open in MarkAmp" for .md files

### Task 3: Wire macOS Trackpad Gestures
**Files:** `src/platform/MacPlatform.mm`, `src/ui/CanvasPanel.cpp`
**Description:** macOS trackpad gestures: pinch to zoom, two-finger scroll, three-finger swipe for navigation, force touch for quick look.
**Acceptance Criteria:**
- Pinch to zoom in editor and canvas
- Two-finger scroll with momentum
- Three-finger swipe: back/forward navigation
- Force touch on link: preview popup
- Gesture events translated to application events
- Gesture sensitivity configurable

### Task 4: Wire macOS Menu Bar Integration
**Files:** `src/platform/MacPlatform.mm`, `src/ui/MainFrame.cpp`
**Description:** macOS menu bar follows HIG: application menu with About, Preferences, Quit. Window menu with Minimize, Zoom, Bring All to Front.
**Acceptance Criteria:**
- Application menu: About MarkAmp, Preferences (Cmd+,), Services, Hide, Quit
- File menu: standard macOS items
- Edit menu: standard macOS items with Emoji & Symbols
- Window menu: Minimize, Zoom, Bring All to Front
- Help menu: Search field
- Dynamic menus update on context

### Task 5: Wire Windows Native Integration
**Files:** `src/platform/WinPlatform.cpp`, `src/ui/CustomChrome.cpp`
**Description:** Windows native integration: custom title bar with Windows 11 snap layouts, taskbar progress, jump list, and Windows Hello for credential storage.
**Acceptance Criteria:**
- Custom title bar compatible with Windows 11 snap
- Snap layouts: hover maximize button shows grid
- Taskbar progress: show during export, sync
- Jump list: recent files in Start menu
- Windows Hello: credential storage
- Title bar context menu: standard Windows options

### Task 6: Wire Windows System Integration
**Files:** `src/platform/WinPlatform.cpp`
**Description:** Windows system features: Windows Credential Manager, Toast notifications, dark mode detection, file associations, Windows Search indexer.
**Acceptance Criteria:**
- Credential Manager: store/retrieve credentials
- Toast notifications: Windows 10/11 notifications
- Dark mode: detect via registry/API
- File associations: .md, .markamp-nb, .markamp-board
- Windows Search: index document metadata
- System tray: minimize to tray option

### Task 7: Wire Linux Desktop Integration
**Files:** `src/platform/LinuxPlatform.cpp`
**Description:** Linux desktop integration: GTK/GDK for system theme, D-Bus notifications, Secret Service for credentials, XDG for file associations.
**Acceptance Criteria:**
- GTK theme detection: colors from system theme
- D-Bus notifications: desktop notifications
- Secret Service API: credential storage
- XDG: proper file associations and MIME types
- Desktop file: .desktop entry for application launcher
- System tray: via StatusNotifierItem (SNI)

### Task 8: Wire Cross-Platform File Dialogs
**Files:** `src/platform/PlatformAbstraction.h`, `src/ui/MainFrame.cpp`
**Description:** File open/save dialogs use native platform dialogs: NSOpenPanel (macOS), IFileDialog (Windows), GtkFileChooserDialog (Linux).
**Acceptance Criteria:**
- File open: native dialog with file type filter
- File save: native dialog with default name
- Folder picker: native folder dialog
- Recent places shown in dialog
- Filter: Markdown (*.md), All Files (*)
- Dialog remembers last directory

### Task 9: Wire Platform-Specific Keyboard Handling
**Files:** `src/core/ShortcutManager.cpp`, `src/platform/PlatformAbstraction.h`
**Description:** Keyboard shortcuts follow platform conventions: Cmd on macOS, Ctrl on Windows/Linux. Platform-specific shortcuts for standard actions.
**Acceptance Criteria:**
- macOS: Cmd+C/V/X/Z, Cmd+S, Cmd+Q
- Windows/Linux: Ctrl+C/V/X/Z, Ctrl+S, Alt+F4
- Platform modifier detection: Cmd vs Ctrl
- Dead keys and input methods handled
- Key repeat rates from system settings
- IME support for CJK input

### Task 10: Wire Platform-Specific Font Handling
**Files:** `src/core/ThemeEngine.cpp`, `src/platform/PlatformAbstraction.h`
**Description:** Font selection uses platform-appropriate defaults and font enumeration.
**Acceptance Criteria:**
- macOS default: SF Mono (code), SF Pro (UI)
- Windows default: Cascadia Mono (code), Segoe UI (UI)
- Linux default: DejaVu Sans Mono (code), system sans-serif (UI)
- Font fallback chain for missing glyphs
- CJK font support on all platforms
- Font enumeration for settings panel

### Task 11: Wire Native Clipboard Integration
**Files:** `src/core/ClipboardService.cpp`, `src/platform/PlatformAbstraction.h`
**Description:** Clipboard integration with platform-specific formats: rich text, images, file paths, HTML.
**Acceptance Criteria:**
- macOS: NSPasteboard with UTIs
- Windows: OLE clipboard with CF_* formats
- Linux: X11/Wayland clipboard selection
- Copy as rich text preserves formatting
- Image paste works from all sources
- File drop from file manager

### Task 12: Wire Platform-Specific Path Handling
**Files:** `src/core/SafePath.cpp`, `src/platform/PlatformAbstraction.h`
**Description:** Path handling respects platform conventions: case sensitivity (macOS/Windows insensitive, Linux sensitive), path separators, max path length.
**Acceptance Criteria:**
- macOS: case-insensitive comparison, '/' separator
- Windows: case-insensitive, '\\' or '/' separator, max 260 chars (or extended)
- Linux: case-sensitive, '/' separator
- Path normalization per platform
- Long path support on Windows (\\?\\ prefix)
- Symlink resolution per platform

### Task 13: Wire Platform Auto-Update
**Files:** `src/platform/PlatformAbstraction.h`, `src/app/MarkAmpApp.cpp`
**Description:** Auto-update mechanism: check for updates on startup, download in background, install on next restart.
**Acceptance Criteria:**
- Update check on startup (configurable interval)
- Background download of update
- "Update Available" notification with changelog
- "Install on Restart" or "Install Now"
- Rollback: keep previous version
- Update channel: stable, beta (configurable)

### Task 14: Wire Platform DPI/Scaling Support
**Files:** `src/ui/LayoutManager.cpp`, `src/platform/PlatformAbstraction.h`
**Description:** Proper DPI scaling on all platforms: high-DPI displays, mixed DPI multi-monitor, DPI change during runtime.
**Acceptance Criteria:**
- macOS Retina: 2x rendering
- Windows: per-monitor DPI awareness
- Linux: honor GDK_SCALE
- Canvas: high-DPI rendering with crisp lines
- Icons: provide 1x and 2x variants
- DPI change: re-layout and re-render

### Task 15: Wire Platform File Watching
**Files:** `src/core/VaultService.cpp`, `src/platform/PlatformAbstraction.h`
**Description:** File watching using platform-native APIs: FSEvents (macOS), ReadDirectoryChangesW (Windows), inotify (Linux).
**Acceptance Criteria:**
- macOS: FSEvents for directory monitoring
- Windows: ReadDirectoryChangesW
- Linux: inotify with watch limit handling
- Events: create, modify, delete, rename
- Coalescing: rapid changes produce single event
- Recursive watching supported

### Task 16: Wire Platform Crash Reporting
**Files:** `src/platform/PlatformAbstraction.h`, `src/app/MarkAmpApp.cpp`
**Description:** Platform-specific crash handling: signal handlers, exception handlers, crash dump generation.
**Acceptance Criteria:**
- macOS: Mach exception handler, crash report
- Windows: SetUnhandledExceptionFilter, minidump
- Linux: signal handler (SIGSEGV, SIGABRT)
- Crash dump written to `.markamp/crashes/`
- Crash report includes: stack trace, version, OS, memory
- "Report Crash" dialog on next launch

### Task 17: Wire Platform System Tray
**Files:** `src/platform/PlatformAbstraction.h`, `src/ui/MainFrame.cpp`
**Description:** System tray integration: minimize to tray, tray icon with status, context menu with quick actions.
**Acceptance Criteria:**
- Tray icon when minimized (configurable: always/never/when minimized)
- Tray context menu: Open, New Document, Recent Files, Quit
- Tray icon reflects sync status
- Double-click tray: restore window
- Notification from tray for important events
- Platform-specific tray API

### Task 18: Wire Platform Shell Integration
**Files:** `src/platform/PlatformAbstraction.h`
**Description:** Shell integration: "Open with MarkAmp" in OS file manager, protocol handler (markamp://), file type icons.
**Acceptance Criteria:**
- macOS: Finder context menu, Info.plist file types
- Windows: shell extension, registry entries
- Linux: .desktop file, MIME types
- Protocol handler: markamp:// URLs open in MarkAmp
- Custom file icons for .md and .markamp-* files
- Installation registers all associations

### Task 19: Wire Platform Build and Packaging
**Files:** `CMakeLists.txt`, `CMakePresets.json`
**Description:** Platform-specific build and packaging: .app bundle (macOS), NSIS/MSI installer (Windows), AppImage/Flatpak/Snap (Linux).
**Acceptance Criteria:**
- macOS: signed .app bundle with DMG
- Windows: NSIS installer with Start Menu entries
- Linux: AppImage for universal distribution
- Code signing on macOS and Windows
- Version embedding in all packages
- Automated packaging in CI

### Task 20: Add Platform Tests
**Files:** `tests/unit/test_cross_platform.cpp`
**Description:** Test platform abstractions: file handling, clipboard, notifications, credentials, and DPI scaling.
**Acceptance Criteria:**
- File operations cross-platform compatible
- Clipboard round-trip for text and images
- Credential store/retrieve
- Path normalization per platform
- DPI calculation correctness
- Font fallback correctness

## Testing Requirements
- File operations on all platforms
- Clipboard operations cross-platform
- Path handling: case sensitivity, separators, limits
- DPI/scaling calculations

## Phase Completion Criteria
- macOS: native title bar, Keychain, trackpad gestures, full-screen
- Windows: snap layouts, jump list, credential manager, toast notifications
- Linux: GTK theme, D-Bus notifications, Secret Service, XDG
- Cross-platform: file dialogs, keyboard, clipboard, DPI
- All tests pass
