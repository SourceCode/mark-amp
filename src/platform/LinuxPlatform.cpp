#include "LinuxPlatform.h"

#ifdef __linux__
#include <gdk/gdk.h>
#include <gtk/gtk.h>
#endif

namespace markamp::platform
{

#if defined(__linux__)
// Factory for Linux
auto create_platform() -> std::unique_ptr<PlatformAbstraction>
{
    return std::make_unique<LinuxPlatform>();
}
#endif

void LinuxPlatform::set_frameless_window_style(wxFrame* frame)
{
    // Removes default frame style for custom chrome
    frame->SetWindowStyleFlag(wxBORDER_NONE | wxRESIZE_BORDER | wxCLIP_CHILDREN);
}

auto LinuxPlatform::begin_native_drag([[maybe_unused]] wxFrame* frame,
                                      [[maybe_unused]] const wxPoint& mouse_screen_pos) -> bool
{
#ifdef __linux__
    // Use GDK for native window move on X11/Wayland
    GtkWidget* widget = static_cast<GtkWidget*>(frame->GetHandle());
    if (widget != nullptr)
    {
        GdkWindow* gdk_window = gtk_widget_get_window(widget);
        if (gdk_window != nullptr)
        {
            gdk_window_begin_move_drag(gdk_window,
                                       GDK_BUTTON_PRIMARY,
                                       mouse_screen_pos.x,
                                       mouse_screen_pos.y,
                                       GDK_CURRENT_TIME);
            return true;
        }
    }
#endif
    return false;
}

auto LinuxPlatform::begin_native_resize([[maybe_unused]] wxFrame* frame,
                                        [[maybe_unused]] ResizeEdge edge) -> bool
{
#ifdef __linux__
    GtkWidget* widget = static_cast<GtkWidget*>(frame->GetHandle());
    if (widget == nullptr)
    {
        return false;
    }

    GdkWindow* gdk_window = gtk_widget_get_window(widget);
    if (gdk_window == nullptr)
    {
        return false;
    }

    // Map ResizeEdge to GDK edge
    GdkWindowEdge gdk_edge = GDK_WINDOW_EDGE_SOUTH_EAST;
    switch (edge)
    {
        case ResizeEdge::Top:
            gdk_edge = GDK_WINDOW_EDGE_NORTH;
            break;
        case ResizeEdge::Bottom:
            gdk_edge = GDK_WINDOW_EDGE_SOUTH;
            break;
        case ResizeEdge::Left:
            gdk_edge = GDK_WINDOW_EDGE_WEST;
            break;
        case ResizeEdge::Right:
            gdk_edge = GDK_WINDOW_EDGE_EAST;
            break;
        case ResizeEdge::TopLeft:
            gdk_edge = GDK_WINDOW_EDGE_NORTH_WEST;
            break;
        case ResizeEdge::TopRight:
            gdk_edge = GDK_WINDOW_EDGE_NORTH_EAST;
            break;
        case ResizeEdge::BottomLeft:
            gdk_edge = GDK_WINDOW_EDGE_SOUTH_WEST;
            break;
        case ResizeEdge::BottomRight:
            gdk_edge = GDK_WINDOW_EDGE_SOUTH_EAST;
            break;
        default:
            return false;
    }

    gdk_window_begin_resize_drag(gdk_window, gdk_edge, GDK_BUTTON_PRIMARY, 0, 0, GDK_CURRENT_TIME);
    return true;
#else
    return false;
#endif
}

auto LinuxPlatform::is_maximized(const wxFrame* frame) const -> bool
{
    return frame->IsMaximized();
}

void LinuxPlatform::toggle_maximize(wxFrame* frame)
{
    frame->Maximize(!frame->IsMaximized());
}

void LinuxPlatform::enter_fullscreen(wxFrame* frame)
{
    frame->ShowFullScreen(true);
}

void LinuxPlatform::exit_fullscreen(wxFrame* frame)
{
    frame->ShowFullScreen(false);
}

// ── Accessibility ──

auto LinuxPlatform::is_high_contrast() const -> bool
{
#ifdef __linux__
    // Check GTK settings for high-contrast theme
    GtkSettings* settings = gtk_settings_get_default();
    if (settings != nullptr)
    {
        gchar* theme_name = nullptr;
        g_object_get(settings, "gtk-theme-name", &theme_name, nullptr);
        if (theme_name != nullptr)
        {
            const bool is_hc = (g_str_has_prefix(theme_name, "HighContrast") != 0) ||
                               (g_str_has_prefix(theme_name, "Adwaita-hc") != 0);
            g_free(theme_name);
            return is_hc;
        }
    }
#endif
    return false;
}

auto LinuxPlatform::prefers_reduced_motion() const -> bool
{
#ifdef __linux__
    // GTK does not have a direct "prefers-reduced-motion" API.
    // Check the environment variable as a fallback.
    const char* env = g_getenv("GTK_ENABLE_ANIMATIONS");
    if (env != nullptr && g_strcmp0(env, "0") == 0)
    {
        return true;
    }

    // Also check the gtk-enable-animations setting
    GtkSettings* settings = gtk_settings_get_default();
    if (settings != nullptr)
    {
        gboolean animations_enabled = TRUE;
        g_object_get(settings, "gtk-enable-animations", &animations_enabled, nullptr);
        return animations_enabled == FALSE;
    }
#endif
    return false;
}

void LinuxPlatform::announce_to_screen_reader([[maybe_unused]] wxWindow* window,
                                              [[maybe_unused]] const wxString& message)
{
    if (window == nullptr)
    {
        return;
    }

#ifdef __linux__
    // Use ATK (Accessibility Toolkit) to announce to screen readers like Orca.
    // Set the accessible name on the underlying GtkWidget so AT-SPI2 picks it up.
    GtkWidget* widget = static_cast<GtkWidget*>(window->GetHandle());
    if (widget != nullptr)
    {
        AtkObject* atk_obj = gtk_widget_get_accessible(widget);
        if (atk_obj != nullptr)
        {
            atk_object_set_name(atk_obj, message.utf8_str().data());
        }
    }
#endif

    // Also set wxWidgets accessible name as a cross-platform fallback.
    window->SetName(message);
#if wxUSE_ACCESSIBILITY
    wxAccessible::NotifyEvent(wxACC_EVENT_OBJECT_NAMECHANGE, window, wxOBJID_CLIENT, wxACC_SELF);
#endif
}

// ── System Appearance ──

auto LinuxPlatform::is_dark_mode() const -> bool
{
#ifdef __linux__
    // Check if the GTK theme prefers dark mode
    GtkSettings* settings = gtk_settings_get_default();
    if (settings != nullptr)
    {
        gboolean prefer_dark = FALSE;
        g_object_get(settings, "gtk-application-prefer-dark-theme", &prefer_dark, nullptr);
        if (prefer_dark == TRUE)
        {
            return true;
        }

        // Fallback: check if theme name contains "dark"
        gchar* theme_name = nullptr;
        g_object_get(settings, "gtk-theme-name", &theme_name, nullptr);
        if (theme_name != nullptr)
        {
            // Case-insensitive check for "dark" in theme name
            gchar* lower = g_utf8_strdown(theme_name, -1);
            const bool is_dark = (g_strstr_len(lower, -1, "dark") != nullptr);
            g_free(lower);
            g_free(theme_name);
            return is_dark;
        }
    }
#endif
    return false;
}

// ── Display ──

auto LinuxPlatform::get_content_scale_factor() const -> double
{
#ifdef __linux__
    GdkDisplay* display = gdk_display_get_default();
    if (display != nullptr)
    {
        GdkMonitor* monitor = gdk_display_get_primary_monitor(display);
        if (monitor != nullptr)
        {
            return static_cast<double>(gdk_monitor_get_scale_factor(monitor));
        }
    }
#endif
    return 1.0;
}

// ── Window Effects ──

void LinuxPlatform::enable_vibrancy([[maybe_unused]] wxFrame* frame, [[maybe_unused]] bool enable)
{
    // Intentional no-op: Linux window compositors (X11 / Wayland) do not
    // expose a standardized blur-behind / vibrancy API.
    // Compositor-specific extensions (e.g. KWin's _KDE_NET_WM_BLUR_BEHIND_REGION,
    // or picom's dual_kawase blur) could be supported in a future phase.
}

void LinuxPlatform::enable_surface_blur([[maybe_unused]] wxFrame* frame,
                                        [[maybe_unused]] bool enable)
{
    // Intentional no-op: same reasoning as enable_vibrancy.
    // Surface blur would require compositor-specific X11 atoms or
    // Wayland protocol extensions that are not universally available.
}

} // namespace markamp::platform
