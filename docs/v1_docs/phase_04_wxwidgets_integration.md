# Phase 04: wxWidgets Integration and Basic Window

## Objective

Create a functioning wxWidgets application that displays a basic window with correct initialization, event loop, and clean shutdown. This phase establishes the wxWidgets application lifecycle, validates the framework integration, and produces a visible window on all three platforms.

## Prerequisites

- Phase 01 (Project Scaffolding and CMake Setup)
- Phase 03 (Dependency Management with vcpkg)

## Deliverables

1. wxWidgets application class (`MarkAmpApp`) with proper `OnInit()` and `OnExit()`
2. Main frame window (`MainFrame`) with correct sizing, positioning, and close handling
3. Basic application icon setup
4. Event table and event handling foundation
5. Verified window display on at least one platform

## Tasks

### Task 4.1: Implement MarkAmpApp application class

Create `/Users/ryanrentfro/code/markamp/src/app/MarkAmpApp.h` and `/Users/ryanrentfro/code/markamp/src/app/MarkAmpApp.cpp`:

**MarkAmpApp.h:**
```cpp
#pragma once
#include <wx/wx.h>

namespace markamp::app {

class MarkAmpApp : public wxApp
{
public:
    bool OnInit() override;
    int OnExit() override;

    // Application-wide settings
    static constexpr int kDefaultWidth = 1280;
    static constexpr int kDefaultHeight = 800;
    static constexpr int kMinWidth = 800;
    static constexpr int kMinHeight = 600;
};

} // namespace markamp::app
```

**MarkAmpApp.cpp:**
- Implement `OnInit()`:
  - Set the application name and vendor: `SetAppName("MarkAmp")`, `SetVendorName("MarkAmp")`
  - Create and show a `MainFrame` instance
  - Set it as the top window
  - Return true on success
- Implement `OnExit()`:
  - Clean up any application-wide resources
  - Return base class `OnExit()`
- Register the app with `wxIMPLEMENT_APP(markamp::app::MarkAmpApp)`

**Acceptance criteria:**
- Application class compiles and links against wxWidgets
- `wxIMPLEMENT_APP` macro is used correctly
- No memory leaks on startup/shutdown

### Task 4.2: Implement basic MainFrame window

Create `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.h` and `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`:

**MainFrame.h:**
```cpp
#pragma once
#include <wx/wx.h>

namespace markamp::ui {

class MainFrame : public wxFrame
{
public:
    MainFrame(const wxString& title, const wxPoint& pos, const wxSize& size);

private:
    // Event handlers
    void OnClose(wxCloseEvent& event);
    void OnSize(wxSizeEvent& event);

    wxDECLARE_EVENT_TABLE();
};

} // namespace markamp::ui
```

**MainFrame.cpp:**
- Constructor:
  - Call base `wxFrame` constructor with `wxDEFAULT_FRAME_STYLE` for now (custom chrome comes in Phase 06)
  - Set minimum size to `kMinWidth x kMinHeight`
  - Center on screen
  - Set background color to a dark color (placeholder for theme)
- `OnClose`:
  - Confirm close if there are unsaved changes (placeholder -- just close for now)
  - Call `Destroy()`
- `OnSize`:
  - Trigger layout update (placeholder)
  - Skip event to allow default processing

**Acceptance criteria:**
- Window appears on screen with correct default size (1280x800)
- Window is centered on the screen
- Window can be resized down to minimum size
- Closing the window terminates the application cleanly
- No warnings or errors in console

### Task 4.3: Update main.cpp to use wxWidgets entry point

Replace `/Users/ryanrentfro/code/markamp/src/main.cpp` content:

The `wxIMPLEMENT_APP` macro in `MarkAmpApp.cpp` replaces the need for a traditional `main()`. Update `main.cpp` to either:

**Option A**: Remove `main.cpp` entirely and rely on `wxIMPLEMENT_APP` in `MarkAmpApp.cpp`

**Option B**: Keep `main.cpp` as a thin wrapper that includes `MarkAmpApp.h` and uses `wxIMPLEMENT_APP_NO_MAIN` with a manual `main()` that calls `wxEntry()`. This is useful for pre-initialization work.

Choose Option B for flexibility:
```cpp
#include "app/MarkAmpApp.h"
#include <wx/wx.h>

wxIMPLEMENT_APP_NO_MAIN(markamp::app::MarkAmpApp);

auto main(int argc, char* argv[]) -> int
{
    return wxEntry(argc, argv);
}
```

**Acceptance criteria:**
- Application starts via `main()` and enters the wxWidgets event loop
- The `MarkAmpApp::OnInit()` is called and creates the window

### Task 4.4: Set up application icon

Create placeholder icon resources:
- `/Users/ryanrentfro/code/markamp/resources/icons/markamp.png` (256x256 PNG, can be a simple colored square placeholder)
- `/Users/ryanrentfro/code/markamp/resources/icons/markamp.ico` (Windows icon)
- `/Users/ryanrentfro/code/markamp/resources/icons/markamp.icns` (macOS icon)

Add icon loading to `MainFrame` constructor:
```cpp
#ifdef __WXMSW__
    SetIcon(wxICON(markamp));
#else
    wxIcon icon;
    icon.LoadFile("resources/icons/markamp.png", wxBITMAP_TYPE_PNG);
    SetIcon(icon);
#endif
```

For Windows, create a resource file `src/markamp.rc`:
```rc
markamp ICON "resources/icons/markamp.ico"
```

Update CMakeLists.txt to include the `.rc` file on Windows.

**Acceptance criteria:**
- Application has a visible icon in the taskbar/dock
- Icon loads without errors on the development platform

### Task 4.5: Set up wxWidgets event handling foundation

Demonstrate the event system by adding basic menu and keyboard handling to `MainFrame`:

- Create a minimal menu bar with:
  - File menu: Quit (Ctrl+Q / Cmd+Q)
  - Help menu: About
- Implement `OnQuit` handler that closes the application
- Implement `OnAbout` handler that shows a `wxMessageBox` with:
  - "MarkAmp v1.0.0"
  - "A retro-futuristic Markdown editor"
  - wxICON_INFORMATION

- Use `wxDECLARE_EVENT_TABLE()` / `wxBEGIN_EVENT_TABLE()` macros
  OR dynamic event binding with `Bind()` (preferred for modern C++)

**Acceptance criteria:**
- Menu bar is visible with File and Help menus
- Ctrl+Q / Cmd+Q quits the application
- Help > About shows the dialog
- Events are properly handled without leaks

### Task 4.6: Verify DPI and high-DPI behavior

Add DPI awareness handling:

- On Windows: set DPI awareness in the application manifest or via `SetProcessDPIAware()`
- On macOS: ensure Retina display support (wxWidgets handles this automatically with proper build flags)
- On Linux: respect `GDK_SCALE` environment variable

Add a status message or logging that reports:
- Current DPI scale factor
- Logical vs physical screen resolution
- Per-monitor DPI if available

**Acceptance criteria:**
- Application renders sharply on high-DPI displays
- UI elements are not blurry or incorrectly sized on Retina/4K displays

### Task 4.7: Add basic logging infrastructure

Set up spdlog for application-wide logging:

Create `/Users/ryanrentfro/code/markamp/src/core/Logger.h` and `Logger.cpp`:

- Initialize a console logger and a rotating file logger
- Set log level from environment variable `MARKAMP_LOG_LEVEL` (default: info)
- Log file location: platform-appropriate app data directory
- Provide convenience macros or inline functions:
  - `MARKAMP_LOG_TRACE(...)`, `MARKAMP_LOG_DEBUG(...)`, `MARKAMP_LOG_INFO(...)`, etc.

Add logging calls to:
- `MarkAmpApp::OnInit()` -- log app version, platform, wxWidgets version
- `MainFrame` constructor -- log window creation with size
- `MainFrame::OnClose()` -- log shutdown

**Acceptance criteria:**
- Console shows log output when running in debug mode
- Log file is created in the appropriate directory
- Log level can be changed via environment variable

### Task 4.8: Write unit tests for app initialization

Create `/Users/ryanrentfro/code/markamp/tests/unit/test_app_init.cpp`:

- Test that `MarkAmpApp` can be instantiated (using wxWidgets test harness or manual setup)
- Test that `MainFrame` can be constructed with valid parameters
- Test minimum size constraints
- Test that the application reports correct version information

Note: wxWidgets GUI tests require special setup. Use `wxUIActionSimulator` or test the non-GUI components separately.

**Acceptance criteria:**
- Test file compiles and links
- Tests pass under CTest
- At least 3 meaningful assertions

## Files Created/Modified

| File | Action |
|---|---|
| `src/app/MarkAmpApp.h` | Created (replacing placeholder) |
| `src/app/MarkAmpApp.cpp` | Created |
| `src/ui/MainFrame.h` | Created (replacing placeholder) |
| `src/ui/MainFrame.cpp` | Created |
| `src/main.cpp` | Modified (wxWidgets entry point) |
| `src/core/Logger.h` | Created |
| `src/core/Logger.cpp` | Created |
| `src/markamp.rc` | Created (Windows resource) |
| `resources/icons/markamp.png` | Created (placeholder) |
| `resources/icons/markamp.ico` | Created (placeholder) |
| `resources/icons/markamp.icns` | Created (placeholder) |
| `src/CMakeLists.txt` | Modified (add source files, resource file) |
| `tests/unit/test_app_init.cpp` | Created |

## Dependencies

- wxWidgets 3.2.9 (core, base components)
- spdlog (logging)
- Platform display server (X11/Wayland on Linux, Cocoa on macOS, Win32 on Windows)

## Estimated Complexity

**High** -- wxWidgets application setup involves platform-specific nuances (icon handling, DPI, resource files). Getting a clean compile-link-run cycle across platforms requires careful CMake work.
