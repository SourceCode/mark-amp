#include "CustomChrome.h"

#include "core/EventBus.h"
#include "core/Events.h"
#include "core/Logger.h"
#include "core/ThemeEngine.h"

#include <wx/dcbuffer.h>
#include <wx/graphics.h>

namespace markamp::ui
{

// --- Fallback colour constants (used when no ThemeEngine is available) ---
namespace colours
{
const wxColour kChromeBg{20, 20, 30};          // dark navy chrome
const wxColour kTextMain{200, 200, 210};       // main text
const wxColour kTextMuted{100, 100, 120};      // muted / inactive text
const wxColour kTextVersion{80, 80, 100};      // version number text
const wxColour kAccentPrimary{0, 200, 255};    // cyan accent
const wxColour kGlowAccent{0, 200, 255, 40};   // semi-transparent glow
const wxColour kHoverBg{255, 255, 255, 25};    // white/10%
const wxColour kCloseHover{239, 68, 68, 204};  // red-500/80%
const wxColour kBevelDark{0, 0, 0, 51};        // black/20%
const wxColour kBevelLight{255, 255, 255, 13}; // white/5%
const wxColour kSeparator{60, 60, 80};         // separator line
const wxColour kFilePill{40, 40, 55};          // filename pill background
} // namespace colours

// --- Themed colour helpers ---

auto CustomChrome::chromeBg() const -> wxColour
{
    if (theme_engine_ != nullptr)
    {
        return theme_engine_->color(core::ThemeColorToken::BgHeader);
    }
    return colours::kChromeBg;
}

auto CustomChrome::textMain() const -> wxColour
{
    if (theme_engine_ != nullptr)
    {
        return theme_engine_->color(core::ThemeColorToken::TextMain);
    }
    return colours::kTextMain;
}

auto CustomChrome::textMuted() const -> wxColour
{
    if (theme_engine_ != nullptr)
    {
        return theme_engine_->color(core::ThemeColorToken::TextMuted);
    }
    return colours::kTextMuted;
}

auto CustomChrome::accentPrimary() const -> wxColour
{
    if (theme_engine_ != nullptr)
    {
        return theme_engine_->color(core::ThemeColorToken::AccentPrimary);
    }
    return colours::kAccentPrimary;
}

auto CustomChrome::hoverBg() const -> wxColour
{
    if (theme_engine_ != nullptr)
    {
        return theme_engine_->color(core::ThemeColorToken::HoverBg);
    }
    return colours::kHoverBg;
}

auto CustomChrome::borderLight() const -> wxColour
{
    if (theme_engine_ != nullptr)
    {
        return theme_engine_->color(core::ThemeColorToken::BorderLight);
    }
    return colours::kBevelLight;
}

auto CustomChrome::borderDark() const -> wxColour
{
    if (theme_engine_ != nullptr)
    {
        return theme_engine_->color(core::ThemeColorToken::BorderDark);
    }
    return colours::kBevelDark;
}

// --- Construction ---

CustomChrome::CustomChrome(wxWindow* parent,
                           markamp::core::EventBus* event_bus,
                           markamp::platform::PlatformAbstraction* platform,
                           markamp::core::ThemeEngine* theme_engine)
    : wxPanel(
          parent, wxID_ANY, wxDefaultPosition, wxSize(-1, kChromeHeight), wxFULL_REPAINT_ON_RESIZE)
    , event_bus_(event_bus)
    , platform_(platform)
    , theme_engine_(theme_engine)
{
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    SetMinSize(wxSize(-1, kChromeHeight));
    SetMaxSize(wxSize(-1, kChromeHeight));

    // Bind events
    Bind(wxEVT_PAINT, &CustomChrome::OnPaint, this);
    Bind(wxEVT_LEFT_DOWN, &CustomChrome::OnMouseDown, this);
    Bind(wxEVT_LEFT_UP, &CustomChrome::OnMouseUp, this);
    Bind(wxEVT_MOTION, &CustomChrome::OnMouseMove, this);
    Bind(wxEVT_LEFT_DCLICK, &CustomChrome::OnMouseDoubleClick, this);
    Bind(wxEVT_ENTER_WINDOW, &CustomChrome::OnMouseEnter, this);
    Bind(wxEVT_LEAVE_WINDOW, &CustomChrome::OnMouseLeave, this);

    // Subscribe to theme changes
    if (theme_engine_ != nullptr)
    {
        theme_subscription_ = theme_engine_->subscribe_theme_change(
            [this](const std::string& /*theme_id*/) { Refresh(); });
    }
}

void CustomChrome::set_filename(const std::string& filename)
{
    filename_ = filename;
    Refresh();
}

void CustomChrome::set_maximized(bool maximized)
{
    is_maximized_ = maximized;
    Refresh();
}

auto CustomChrome::hit_test(const wxPoint& local_pos) const -> HitZone
{
    // If using native controls, we don't hit-test the custom buttons
    if (platform_ != nullptr && platform_->uses_native_window_controls())
    {
        if (sidebar_rect_.Contains(local_pos))
        {
            return HitZone::SidebarToggle;
        }
        return HitZone::DragArea;
    }

    if (close_rect_.Contains(local_pos))
    {
        return HitZone::CloseButton;
    }
    if (maximize_rect_.Contains(local_pos))
    {
        return HitZone::MaximizeButton;
    }
    if (minimize_rect_.Contains(local_pos))
    {
        return HitZone::MinimizeButton;
    }
    if (sidebar_rect_.Contains(local_pos))
    {
        return HitZone::SidebarToggle;
    }
    return HitZone::DragArea;
}

void CustomChrome::computeLayout(int width)
{
    const int btn_y = (kChromeHeight - 24) / 2;
    const int right_edge = width - 4;

    // Check for native controls
    wxRect native_controls_rect;
    bool using_native_controls = false;
    if (platform_ != nullptr && platform_->uses_native_window_controls())
    {
        using_native_controls = true;
        auto* frame = getParentFrame();
        if (frame)
        {
            native_controls_rect = platform_->get_window_controls_rect(frame);
        }
    }

    if (using_native_controls)
    {
        // Zero out custom button rects so they aren't clickable/visible
        close_rect_ = wxRect();
        maximize_rect_ = wxRect();
        minimize_rect_ = wxRect();

        // Check if native controls are on the right (like Windows/Linux default, but native)
        // or left (like macOS default)
        const bool controls_on_left = (native_controls_rect.GetRight() < width / 2);

        if (controls_on_left)
        {
            // Native controls on left.
            // Sidebar toggle is on the right.
            // We need to make sure the logo/title doesn't overlap the native controls.
            // (Handled in OnPaint by checking native_controls_rect)

            // Sidebar toggle stays on the right
            sidebar_rect_ = wxRect(right_edge - kButtonWidth - 8, btn_y, kButtonWidth, 24);
        }
        else
        {
            // Native controls on right.
            // Sidebar toggle should be to the left of the native controls.
            int right_most =
                native_controls_rect.GetLeft() > 0 ? native_controls_rect.GetLeft() : right_edge;
            sidebar_rect_ = wxRect(right_most - kButtonWidth - 12, btn_y, kButtonWidth, 24);
        }
    }
    else
    {
        // Custom window control buttons (right-to-left)
        close_rect_ = wxRect(right_edge - kButtonWidth, btn_y, kButtonWidth, 24);
        maximize_rect_ = wxRect(close_rect_.GetLeft() - kButtonWidth, btn_y, kButtonWidth, 24);
        minimize_rect_ = wxRect(maximize_rect_.GetLeft() - kButtonWidth, btn_y, kButtonWidth, 24);

        // Sidebar toggle button is before the separator
        sidebar_rect_ =
            wxRect(minimize_rect_.GetLeft() - kButtonWidth - 8, btn_y, kButtonWidth, 24);
    }
}

// --- Paint ---

void CustomChrome::OnPaint(wxPaintEvent& /*event*/)
{
    wxAutoBufferedPaintDC dc(this);
    auto sz = GetClientSize();
    const int w = sz.GetWidth();
    const int h = sz.GetHeight();

    // Recompute layout
    computeLayout(w);

    // Background — gradient if configured, otherwise flat themed
    bool gradient_painted = false;
    if (theme_engine_ != nullptr)
    {
        const auto& gradient = theme_engine_->current_theme().title_bar_gradient;
        if (gradient.is_enabled())
        {
            auto start_color = core::Color::from_string(*gradient.start);
            auto end_color = core::Color::from_string(*gradient.end);
            if (start_color && end_color)
            {
                wxGraphicsContext* gc_ptr = wxGraphicsContext::Create(dc);
                if (gc_ptr != nullptr)
                {
                    gc_ptr->SetBrush(gc_ptr->CreateLinearGradientBrush(0,
                                                                       0,
                                                                       static_cast<wxDouble>(w),
                                                                       0,
                                                                       start_color->to_wx_colour(),
                                                                       end_color->to_wx_colour()));
                    gc_ptr->DrawRectangle(0, 0, w, h);
                    delete gc_ptr;
                    gradient_painted = true;
                }
            }
        }
    }
    if (!gradient_painted)
    {
        dc.SetBackground(wxBrush(chromeBg()));
        dc.Clear();
    }

    // Phase 4D+: Window Effects rendering
    if (theme_engine_ != nullptr)
    {
        const auto& effects = theme_engine_->current_theme().effects;

        // Edge glow — configurable color, width, and alpha
        if (effects.edge_glow)
        {
            auto glow_color = effects.edge_glow_color.to_wx_colour();
            const int glow_width = std::clamp(effects.edge_glow_width, 1, 4);
            const uint8_t glow_alpha = effects.edge_glow_alpha;

            for (int layer = 0; layer < glow_width; ++layer)
            {
                // Fade alpha outward: outermost layer = full alpha, inner layers dim
                auto layer_alpha =
                    static_cast<uint8_t>(glow_alpha * (glow_width - layer) / glow_width);
                wxColour layer_color(
                    glow_color.Red(), glow_color.Green(), glow_color.Blue(), layer_alpha);
                dc.SetPen(wxPen(layer_color, 1));
                dc.DrawLine(layer, layer, w - layer, layer);                 // top
                dc.DrawLine(layer, layer, layer, h - layer);                 // left
                dc.DrawLine(w - 1 - layer, layer, w - 1 - layer, h - layer); // right
                dc.DrawLine(layer, h - 1 - layer, w - layer, h - 1 - layer); // bottom
            }
        }

        // Inner shadow — concentric rects with decreasing alpha
        if (effects.inner_shadow)
        {
            const int radius = std::clamp(effects.inner_shadow_radius, 1, 8);
            const uint8_t base_alpha = effects.inner_shadow_alpha;

            for (int layer = 0; layer < radius; ++layer)
            {
                auto shadow_alpha = static_cast<uint8_t>(base_alpha * (radius - layer) / radius);
                wxColour shadow_color(0, 0, 0, shadow_alpha);
                dc.SetPen(wxPen(shadow_color, 1));
                dc.SetBrush(*wxTRANSPARENT_BRUSH);
                dc.DrawRectangle(layer, layer, w - 2 * layer, h - 2 * layer);
            }
        }

        // Vignette — radial gradient darkening at window edges
        if (effects.vignette)
        {
            wxGraphicsContext* gc_ptr = wxGraphicsContext::Create(dc);
            if (gc_ptr != nullptr)
            {
                const uint8_t strength = effects.vignette_strength;

                // Draw semi-transparent black rectangles along edges
                // Top vignette strip
                gc_ptr->SetBrush(gc_ptr->CreateLinearGradientBrush(0,
                                                                   0,
                                                                   0,
                                                                   static_cast<wxDouble>(h) / 6.0,
                                                                   wxColour(0, 0, 0, strength),
                                                                   wxColour(0, 0, 0, 0)));
                gc_ptr->DrawRectangle(0, 0, w, static_cast<wxDouble>(h) / 6.0);

                // Bottom vignette strip
                gc_ptr->SetBrush(
                    gc_ptr->CreateLinearGradientBrush(0,
                                                      static_cast<wxDouble>(h) * 5.0 / 6.0,
                                                      0,
                                                      static_cast<wxDouble>(h),
                                                      wxColour(0, 0, 0, 0),
                                                      wxColour(0, 0, 0, strength)));
                gc_ptr->DrawRectangle(
                    0, static_cast<wxDouble>(h) * 5.0 / 6.0, w, static_cast<wxDouble>(h) / 6.0);

                // Left vignette strip
                gc_ptr->SetBrush(gc_ptr->CreateLinearGradientBrush(0,
                                                                   0,
                                                                   static_cast<wxDouble>(w) / 8.0,
                                                                   0,
                                                                   wxColour(0, 0, 0, strength),
                                                                   wxColour(0, 0, 0, 0)));
                gc_ptr->DrawRectangle(0, 0, static_cast<wxDouble>(w) / 8.0, h);

                // Right vignette strip
                gc_ptr->SetBrush(
                    gc_ptr->CreateLinearGradientBrush(static_cast<wxDouble>(w) * 7.0 / 8.0,
                                                      0,
                                                      static_cast<wxDouble>(w),
                                                      0,
                                                      wxColour(0, 0, 0, 0),
                                                      wxColour(0, 0, 0, strength)));
                gc_ptr->DrawRectangle(
                    static_cast<wxDouble>(w) * 7.0 / 8.0, 0, static_cast<wxDouble>(w) / 8.0, h);

                delete gc_ptr;
            }
        }
    }

    // --- Left section: Logo dot + "MARKAMP v1.0.0" ---
    // Only show logo and title if NOT using native controls (per user request for macOS)
    if (platform_ == nullptr || !platform_->uses_native_window_controls())
    {
        const int logo_x = 16;
        const int logo_y = h / 2;
        drawLogoDot(dc, logo_x, logo_y);

        // "MARKAMP" title — themed
        wxFont titleFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
        dc.SetFont(titleFont);
        dc.SetTextForeground(textMain());
        const int text_x = logo_x + kLogoDotRadius + 10;
        const int text_y = (h - dc.GetTextExtent("M").GetHeight()) / 2;
        dc.DrawText("MARKAMP", text_x, text_y);

        // Version string
        auto title_extent = dc.GetTextExtent("MARKAMP");
        wxFont versionFont(8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
        dc.SetFont(versionFont);
        dc.SetTextForeground(textMuted());
        dc.DrawText("v" + std::to_string(MARKAMP_VERSION_MAJOR) + "." +
                        std::to_string(MARKAMP_VERSION_MINOR) + "." +
                        std::to_string(MARKAMP_VERSION_PATCH),
                    text_x + title_extent.GetWidth() + 6,
                    text_y + 2);
    }

    // --- Center section: Filename pill ---
    wxFont monoFont(10, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
    dc.SetFont(monoFont);
    auto fname_extent = dc.GetTextExtent(filename_);
    const int pill_w = fname_extent.GetWidth() + 20;
    const int pill_h = fname_extent.GetHeight() + 6;
    const int pill_x = (w - pill_w) / 2;
    const int pill_y = (h - pill_h) / 2;

    // Draw pill background — themed
    wxColour pillBg = theme_engine_ != nullptr
                          ? theme_engine_->color(core::ThemeColorToken::BgPanel)
                          : colours::kFilePill;
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.SetBrush(wxBrush(pillBg));
    dc.DrawRoundedRectangle(pill_x, pill_y, pill_w, pill_h, 4);

    // Draw filename text — themed
    dc.SetTextForeground(textMuted());
    dc.DrawText(filename_, pill_x + 10, pill_y + 3);

    // --- Right section: Sidebar toggle + separator + window controls ---
    drawSidebarToggle(dc, sidebar_rect_);

    // Separator line — themed
    wxColour sepColor = theme_engine_ != nullptr
                            ? theme_engine_->color(core::ThemeColorToken::BorderLight)
                            : colours::kSeparator;

    // Separator is always to the right of the sidebar toggle
    const int sep_x = sidebar_rect_.GetRight() + 6;
    dc.SetPen(wxPen(sepColor, 1));
    dc.DrawLine(sep_x, 8, sep_x, h - 8);

    // Window control buttons (only if not using native)
    if (platform_ == nullptr || !platform_->uses_native_window_controls())
    {
        drawWindowButton(dc, minimize_rect_, HitZone::MinimizeButton, "_");
        drawWindowButton(
            dc, maximize_rect_, HitZone::MaximizeButton, is_maximized_ ? "\u25A1" : "\u25A0");
        drawWindowButton(dc, close_rect_, HitZone::CloseButton, "\u00D7");
    }

    // --- Bottom bevel border ---
    drawBevelBorder(dc, w, h);
}

void CustomChrome::drawLogoDot(wxDC& dc, int x, int y)
{
    auto accent = accentPrimary();

    // Glow circle — semi-transparent accent
    dc.SetPen(*wxTRANSPARENT_PEN);
    wxColour glow(accent.Red(), accent.Green(), accent.Blue(), 40);
    dc.SetBrush(wxBrush(glow));
    dc.DrawCircle(x, y, kLogoDotRadius + 4);

    // Main dot — solid accent
    dc.SetBrush(wxBrush(accent));
    dc.DrawCircle(x, y, kLogoDotRadius);
}

void CustomChrome::drawWindowButton(wxDC& dc,
                                    const wxRect& rect,
                                    HitZone zone,
                                    const wxString& glyph)
{
    bool hovered = (hover_zone_ == zone);
    bool pressed = (pressed_zone_ == zone);

    dc.SetPen(*wxTRANSPARENT_PEN);
    if (zone == HitZone::CloseButton && hovered)
    {
        dc.SetBrush(wxBrush(colours::kCloseHover));
    }
    else if (hovered || pressed)
    {
        dc.SetBrush(wxBrush(hoverBg()));
    }
    else
    {
        dc.SetBrush(*wxTRANSPARENT_BRUSH);
    }
    dc.DrawRoundedRectangle(rect, 3);

    wxFont btnFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
    dc.SetFont(btnFont);

    if (zone == HitZone::CloseButton && hovered)
    {
        dc.SetTextForeground(*wxWHITE);
    }
    else if (hovered)
    {
        dc.SetTextForeground(textMain());
    }
    else
    {
        dc.SetTextForeground(textMuted());
    }

    auto extent = dc.GetTextExtent(glyph);
    dc.DrawText(glyph,
                rect.GetLeft() + (rect.GetWidth() - extent.GetWidth()) / 2,
                rect.GetTop() + (rect.GetHeight() - extent.GetHeight()) / 2);
}

void CustomChrome::drawSidebarToggle(wxDC& dc, const wxRect& rect)
{
    bool hovered = (hover_zone_ == HitZone::SidebarToggle);

    dc.SetPen(*wxTRANSPARENT_PEN);
    if (hovered)
    {
        dc.SetBrush(wxBrush(hoverBg()));
        dc.DrawRoundedRectangle(rect, 3);
    }

    dc.SetPen(wxPen(hovered ? textMain() : textMuted(), 2));
    const int cx = rect.GetLeft() + rect.GetWidth() / 2;
    const int cy = rect.GetTop() + rect.GetHeight() / 2;
    const int line_w = 10;

    dc.DrawLine(cx - line_w / 2, cy - 4, cx + line_w / 2, cy - 4);
    dc.DrawLine(cx - line_w / 2, cy, cx + line_w / 2, cy);
    dc.DrawLine(cx - line_w / 2, cy + 4, cx + line_w / 2, cy + 4);
}

void CustomChrome::drawBevelBorder(wxDC& dc, int width, int height)
{
    dc.SetPen(wxPen(borderLight(), 1));
    dc.DrawLine(0, height - 3, width, height - 3);

    dc.SetPen(wxPen(borderDark(), 1));
    dc.DrawLine(0, height - 2, width, height - 2);
    dc.DrawLine(0, height - 1, width, height - 1);
}

// --- Mouse event handlers ---

void CustomChrome::OnMouseDown(wxMouseEvent& event)
{
    auto zone = hit_test(event.GetPosition());
    pressed_zone_ = zone;

    if (zone == HitZone::DragArea)
    {
        auto* frame = getParentFrame();
        if (frame != nullptr && platform_ != nullptr)
        {
            auto screen_pos = ClientToScreen(event.GetPosition());
            if (platform_->begin_native_drag(frame, screen_pos))
            {
                return;
            }
        }

        dragging_ = true;
        drag_start_ = event.GetPosition();
        CaptureMouse();
    }

    Refresh();
}

void CustomChrome::OnMouseUp(wxMouseEvent& event)
{
    if (dragging_)
    {
        dragging_ = false;
        if (HasCapture())
        {
            ReleaseMouse();
        }
    }

    auto zone = hit_test(event.GetPosition());
    if (zone == pressed_zone_)
    {
        auto* frame = getParentFrame();
        switch (zone)
        {
            case HitZone::MinimizeButton:
                if (frame != nullptr)
                {
                    frame->Iconize(true);
                }
                break;

            case HitZone::MaximizeButton:
                if (frame != nullptr && platform_ != nullptr)
                {
                    platform_->toggle_maximize(frame);
                    is_maximized_ = platform_->is_maximized(frame);
                    Refresh();
                }
                break;

            case HitZone::CloseButton:
                if (frame != nullptr)
                {
                    frame->Close();
                }
                break;

            case HitZone::SidebarToggle:
            {
                if (event_bus_ != nullptr)
                {
                    markamp::core::events::SidebarToggleEvent evt;
                    evt.visible = true;
                    event_bus_->publish(evt);
                }
                MARKAMP_LOG_DEBUG("Sidebar toggle clicked");
                break;
            }

            default:
                break;
        }
    }

    pressed_zone_ = HitZone::None;
    Refresh();
}

void CustomChrome::OnMouseMove(wxMouseEvent& event)
{
    if (dragging_)
    {
        auto* frame = getParentFrame();
        if (frame != nullptr)
        {
            wxPoint screen_pos = ClientToScreen(event.GetPosition());
            auto new_pos = screen_pos - drag_start_;
            frame->Move(new_pos);
        }
        return;
    }

    auto zone = hit_test(event.GetPosition());
    if (zone != hover_zone_)
    {
        hover_zone_ = zone;
        if (zone != HitZone::DragArea && zone != HitZone::None)
        {
            SetCursor(wxCursor(wxCURSOR_HAND));
        }
        else
        {
            SetCursor(wxNullCursor);
        }
        Refresh();
    }
}

void CustomChrome::OnMouseDoubleClick(wxMouseEvent& event)
{
    auto zone = hit_test(event.GetPosition());
    if (zone == HitZone::DragArea)
    {
        auto* frame = getParentFrame();
        if (frame != nullptr && platform_ != nullptr)
        {
            platform_->toggle_maximize(frame);
            is_maximized_ = platform_->is_maximized(frame);
            Refresh();
        }
    }
}

void CustomChrome::OnMouseEnter([[maybe_unused]] wxMouseEvent& event)
{
    // Reset hover on re-enter
}

void CustomChrome::OnMouseLeave([[maybe_unused]] wxMouseEvent& event)
{
    if (hover_zone_ != HitZone::None)
    {
        hover_zone_ = HitZone::None;
        SetCursor(wxNullCursor);
        Refresh();
    }
}

auto CustomChrome::getParentFrame() -> wxFrame*
{
    wxWindow* parent = GetParent();
    while (parent != nullptr)
    {
        auto* frame = dynamic_cast<wxFrame*>(parent);
        if (frame != nullptr)
        {
            return frame;
        }
        parent = parent->GetParent();
    }
    return nullptr;
}

} // namespace markamp::ui
