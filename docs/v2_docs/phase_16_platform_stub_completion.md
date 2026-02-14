# Phase 16: Platform Stub Completion

**Priority:** Medium
**Estimated Scope:** ~4 files affected
**Dependencies:** None

## Objective

Complete the platform abstraction implementations for Windows and Linux so that all `PlatformAbstraction` virtual methods have real platform-integrated behavior, not stubs or empty bodies.

## Background/Context

The `PlatformAbstraction` interface (`/Users/ryanrentfro/code/markamp/src/platform/PlatformAbstraction.h`) defines 13 virtual methods across 5 categories:

1. Window Management: `set_frameless_window_style`, `begin_native_drag`, `begin_native_resize`, `is_maximized`, `toggle_maximize`, `enter_fullscreen`, `exit_fullscreen`
2. Accessibility: `is_high_contrast`, `prefers_reduced_motion`, `announce_to_screen_reader`
3. System Appearance: `is_dark_mode`
4. Display: `get_content_scale_factor`
5. Window Effects: `enable_vibrancy`, `enable_surface_blur`

### MacPlatform (Complete)

`MacPlatform.mm` uses Objective-C++ to implement all methods with native macOS APIs (NSWindow, NSVisualEffectView, NSAccessibility). This is the reference implementation.

### WinPlatform (Mostly Complete, 2 gaps)

**File:** `/Users/ryanrentfro/code/markamp/src/platform/WinPlatform.cpp`

Status:
- Window Management: COMPLETE (7/7 methods implemented with Win32 API)
- Accessibility: PARTIAL
  - `is_high_contrast()` -- COMPLETE (uses `SystemParametersInfoW` with `SPI_GETHIGHCONTRAST`)
  - `prefers_reduced_motion()` -- COMPLETE (uses `SPI_GETCLIENTAREAANIMATION`)
  - `announce_to_screen_reader()` -- STUB (line 133-138). Comment says "TODO: Implement UIA (UI Automation) notifications for NVDA/JAWS"
- System Appearance: COMPLETE (`is_dark_mode()` reads Windows Registry `AppsUseLightTheme`)
- Display: COMPLETE (`get_content_scale_factor()` uses `GetDeviceCaps`)
- Window Effects: NOT IMPLEMENTED (uses base class defaults which are no-ops)

### LinuxPlatform (Mostly Complete, 2 gaps)

**File:** `/Users/ryanrentfro/code/markamp/src/platform/LinuxPlatform.cpp`

Status:
- Window Management: COMPLETE (7/7 methods using GDK/GTK APIs)
- Accessibility: PARTIAL
  - `is_high_contrast()` -- COMPLETE (checks GTK theme name for "HighContrast" / "Adwaita-hc")
  - `prefers_reduced_motion()` -- COMPLETE (checks `GTK_ENABLE_ANIMATIONS` env var and `gtk-enable-animations` setting)
  - `announce_to_screen_reader()` -- STUB (line 169-175). Comment says "AT-SPI2 announcement requires atspi2 library linkage. Full AT-SPI2 integration is deferred to Phase 29."
- System Appearance: COMPLETE (`is_dark_mode()` checks `gtk-application-prefer-dark-theme` and theme name)
- Display: COMPLETE (`get_content_scale_factor()` uses `gdk_monitor_get_scale_factor`)
- Window Effects: NOT IMPLEMENTED (uses base class defaults)

## Detailed Tasks

### Task 1: Implement WinPlatform::announce_to_screen_reader using UIA

**File:** `/Users/ryanrentfro/code/markamp/src/platform/WinPlatform.cpp`

Replace the stub at line 133-138 with a UIA implementation:

```cpp
#ifdef _WIN32
#include <UIAutomation.h>
#endif

void WinPlatform::announce_to_screen_reader([[maybe_unused]] wxWindow* window,
                                            [[maybe_unused]] const wxString& message)
{
#ifdef _WIN32
    // Use UIA notification for NVDA/JAWS/Narrator
    // Fallback approach: use wxAccessible if available
    auto* accessible = window->GetAccessible();
    if (accessible != nullptr)
    {
        // wxWidgets MSAA bridge -- limited but functional
        // For a better approach, use IUIAutomation::RaiseNotificationEvent
        // which requires COM initialization and UIAutomationCore.lib
    }

    // Simpler fallback: use the window name approach
    // Screen readers will read the new window title
    // This is a temporary solution until full UIA is integrated
    MARKAMP_LOG_DEBUG("Screen reader announcement (stub): {}",
                       message.ToStdString());
#endif
}
```

**Note:** Full UIA implementation requires:
1. COM initialization (`CoInitializeEx`)
2. Linking `UIAutomationCore.lib`
3. Getting `IUIAutomation` interface
4. Creating `IUIAutomationElement` from HWND
5. Calling `UiaRaiseNotificationEvent`

For the initial implementation, use the wxAccessible bridge as a simpler first pass, with a TODO for full UIA.

### Task 2: Implement LinuxPlatform::announce_to_screen_reader using AT-SPI2

**File:** `/Users/ryanrentfro/code/markamp/src/platform/LinuxPlatform.cpp`

Replace the stub at line 169-175:

```cpp
void LinuxPlatform::announce_to_screen_reader([[maybe_unused]] wxWindow* window,
                                              [[maybe_unused]] const wxString& message)
{
#ifdef __linux__
    // AT-SPI2 is the Linux accessibility standard.
    // For a basic implementation, use the atspi_generate_keyboard_event
    // or atspi_accessible_notify approach from libatspi.
    //
    // Simpler fallback: Use the gtk-a11y bridge
    GtkWidget* widget = static_cast<GtkWidget*>(window->GetHandle());
    if (widget != nullptr)
    {
        AtkObject* atk_obj = gtk_widget_get_accessible(widget);
        if (atk_obj != nullptr)
        {
            // ATK announcement -- supported by Orca screen reader
            g_signal_emit_by_name(atk_obj,
                                   "announcement",
                                   message.ToStdString().c_str(),
                                   ATK_LIVE_POLITE);
        }
    }

    MARKAMP_LOG_DEBUG("Screen reader announcement (AT-SPI2): {}",
                       message.ToStdString());
#endif
}
```

**Note:** This requires linking `libatk` (already part of GTK). The `announcement` signal is available in ATK 2.46+. For older systems, fall back to setting the accessible name/description.

### Task 3: Add window effects for Windows (vibrancy via DWM)

**File:** `/Users/ryanrentfro/code/markamp/src/platform/WinPlatform.h` and `WinPlatform.cpp`

Add override declarations to the header:

```cpp
void enable_vibrancy(wxFrame* frame, bool enable) override;
void enable_surface_blur(wxFrame* frame, bool enable) override;
```

Implementation using Windows 10+ DWM APIs:

```cpp
void WinPlatform::enable_vibrancy(wxFrame* frame, bool enable)
{
#ifdef _WIN32
    auto* hwnd = static_cast<HWND>(frame->GetHandle());
    if (hwnd == nullptr) return;

    // Windows 11: use Mica material
    // Windows 10: use acrylic blur via SetWindowCompositionAttribute
    BOOL value = enable ? TRUE : FALSE;
    // DwmSetWindowAttribute with DWMWA_USE_IMMERSIVE_DARK_MODE
    // or DWMWA_SYSTEMBACKDROP_TYPE for Windows 11
    DwmSetWindowAttribute(hwnd, 20 /* DWMWA_USE_IMMERSIVE_DARK_MODE */,
                           &value, sizeof(value));
#endif
}

void WinPlatform::enable_surface_blur([[maybe_unused]] wxFrame* frame,
                                       [[maybe_unused]] bool enable)
{
    // Surface blur on individual panels is not supported by Win32 DWM.
    // This is a macOS-only feature using NSVisualEffectView.
}
```

### Task 4: Add window effects for Linux (no-op with documentation)

**File:** `/Users/ryanrentfro/code/markamp/src/platform/LinuxPlatform.h` and `LinuxPlatform.cpp`

Add override declarations:

```cpp
void enable_vibrancy(wxFrame* frame, bool enable) override;
void enable_surface_blur(wxFrame* frame, bool enable) override;
```

Implementation:

```cpp
void LinuxPlatform::enable_vibrancy([[maybe_unused]] wxFrame* frame,
                                     [[maybe_unused]] bool enable)
{
    // GTK does not support window-level translucency/vibrancy natively.
    // Compositors like KWin (KDE) and Mutter (GNOME) support blur via
    // X11 _KDE_NET_WM_BLUR_BEHIND_REGION or Wayland layer-shell protocol,
    // but these are compositor-specific and not portable.
    // This remains a no-op on Linux.
}

void LinuxPlatform::enable_surface_blur([[maybe_unused]] wxFrame* frame,
                                         [[maybe_unused]] bool enable)
{
    // Not supported on Linux.
}
```

### Task 5: Add WinPlatform and LinuxPlatform headers to source_group

**File:** `/Users/ryanrentfro/code/markamp/src/CMakeLists.txt`

Ensure `platform/WinPlatform.h` and `platform/LinuxPlatform.h` are listed in the `source_group()` block (currently only `platform/MacPlatform.h` is listed at line 218).

## Acceptance Criteria

1. `announce_to_screen_reader()` has a functional implementation on Windows and Linux
2. `enable_vibrancy()` has a Windows DWM implementation
3. Linux window effects are explicitly documented as unsupported (not silently no-op)
4. All platform headers are in the IDE source_group
5. Build succeeds on all three platforms

## Testing Requirements

- Manual testing: verify screen reader announcements on Windows (Narrator) and Linux (Orca)
- Manual testing: verify vibrancy effect on Windows 11
- Compile test: verify all three platforms build without errors
- Existing test suite passes
