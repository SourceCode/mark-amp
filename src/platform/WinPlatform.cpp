#include "WinPlatform.h"

namespace markamp::platform
{

#if defined(_WIN32)
// Factory for Windows
auto create_platform() -> std::unique_ptr<PlatformAbstraction>
{
    return std::make_unique<WinPlatform>();
}
#endif

void WinPlatform::set_frameless_window_style(wxFrame* frame)
{
    // Stub: removes default frame style
    frame->SetWindowStyleFlag(wxBORDER_NONE | wxRESIZE_BORDER | wxCLIP_CHILDREN);
}

auto WinPlatform::begin_native_drag([[maybe_unused]] wxFrame* frame,
                                    [[maybe_unused]] const wxPoint& mouse_screen_pos) -> bool
{
#ifdef _WIN32
    // Use Win32 API for native window move
    auto* hwnd = static_cast<HWND>(frame->GetHandle());
    if (hwnd != nullptr)
    {
        ReleaseCapture();
        SendMessage(hwnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);
        return true;
    }
#endif
    return false;
}

auto WinPlatform::begin_native_resize([[maybe_unused]] wxFrame* frame,
                                      [[maybe_unused]] ResizeEdge edge) -> bool
{
#ifdef _WIN32
    auto* hwnd = static_cast<HWND>(frame->GetHandle());
    if (hwnd == nullptr)
    {
        return false;
    }

    // Map ResizeEdge to Win32 hit-test codes
    WPARAM hit_test = HTCLIENT;
    switch (edge)
    {
        case ResizeEdge::Top:
            hit_test = HTTOP;
            break;
        case ResizeEdge::Bottom:
            hit_test = HTBOTTOM;
            break;
        case ResizeEdge::Left:
            hit_test = HTLEFT;
            break;
        case ResizeEdge::Right:
            hit_test = HTRIGHT;
            break;
        case ResizeEdge::TopLeft:
            hit_test = HTTOPLEFT;
            break;
        case ResizeEdge::TopRight:
            hit_test = HTTOPRIGHT;
            break;
        case ResizeEdge::BottomLeft:
            hit_test = HTBOTTOMLEFT;
            break;
        case ResizeEdge::BottomRight:
            hit_test = HTBOTTOMRIGHT;
            break;
        default:
            return false;
    }

    ReleaseCapture();
    SendMessage(hwnd, WM_NCLBUTTONDOWN, hit_test, 0);
    return true;
#else
    return false;
#endif
}

auto WinPlatform::is_maximized(const wxFrame* frame) const -> bool
{
    return frame->IsMaximized();
}

void WinPlatform::toggle_maximize(wxFrame* frame)
{
    frame->Maximize(!frame->IsMaximized());
}

void WinPlatform::enter_fullscreen(wxFrame* frame)
{
    frame->ShowFullScreen(true);
}

void WinPlatform::exit_fullscreen(wxFrame* frame)
{
    frame->ShowFullScreen(false);
}

// ── Accessibility ──

auto WinPlatform::is_high_contrast() const -> bool
{
#ifdef _WIN32
    HIGHCONTRAST hc{};
    hc.cbSize = sizeof(HIGHCONTRAST);
    if (SystemParametersInfoW(SPI_GETHIGHCONTRAST, sizeof(hc), &hc, 0) != 0)
    {
        return (hc.dwFlags & HCF_HIGHCONTRASTON) != 0;
    }
#endif
    return false;
}

auto WinPlatform::prefers_reduced_motion() const -> bool
{
#ifdef _WIN32
    BOOL animation_enabled = TRUE;
    if (SystemParametersInfoW(SPI_GETCLIENTAREAANIMATION, 0, &animation_enabled, 0) != 0)
    {
        return animation_enabled == FALSE;
    }
#endif
    return false;
}

void WinPlatform::announce_to_screen_reader([[maybe_unused]] wxWindow* window,
                                            [[maybe_unused]] const wxString& message)
{
    // TODO: Implement UIA (UI Automation) notifications for NVDA/JAWS
    // This requires linking to UIAutomationCore.lib and using
    // UiaRaiseNotificationEvent or IAccessible2 interfaces.
}

// ── System Appearance ──

auto WinPlatform::is_dark_mode() const -> bool
{
#ifdef _WIN32
    // Check Windows Registry for Apps dark mode preference
    HKEY h_key{};
    if (RegOpenKeyExW(HKEY_CURRENT_USER,
                      L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                      0,
                      KEY_READ,
                      &h_key) == ERROR_SUCCESS)
    {
        DWORD value = 1;
        DWORD size = sizeof(value);
        if (RegQueryValueExW(h_key,
                             L"AppsUseLightTheme",
                             nullptr,
                             nullptr,
                             reinterpret_cast<LPBYTE>(&value),
                             &size) == ERROR_SUCCESS)
        {
            RegCloseKey(h_key);
            return value == 0; // 0 = dark mode, 1 = light mode
        }
        RegCloseKey(h_key);
    }
#endif
    return false;
}

// ── Display ──

auto WinPlatform::get_content_scale_factor() const -> double
{
#ifdef _WIN32
    // Use the primary monitor's DPI
    HDC hdc = GetDC(nullptr);
    if (hdc != nullptr)
    {
        int dpi = GetDeviceCaps(hdc, LOGPIXELSX);
        ReleaseDC(nullptr, hdc);
        return static_cast<double>(dpi) / 96.0;
    }
#endif
    return 1.0;
}

} // namespace markamp::platform
