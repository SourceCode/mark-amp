#include "Toolbar.h"

#include "core/Events.h"
#include "core/Logger.h"

#include <wx/dcbuffer.h>
#include <wx/graphics.h>

#include <array>
#include <cmath>

namespace markamp::ui
{

namespace
{
constexpr int kButtonPadH = 8;
constexpr int kButtonPadV = 8; // 8E: was 6
constexpr int kIconSize = 14;
constexpr int kButtonGap = 8;    // 8E: was 4
constexpr int kRightMargin = 16; // 8E: was 12
constexpr int kLeftMargin = 16;  // 8E: was 12
constexpr double kFontSizeLabel = 11.0;
} // namespace

Toolbar::Toolbar(wxWindow* parent, core::ThemeEngine& theme_engine, core::EventBus& event_bus)
    : ThemeAwareWindow(parent, theme_engine)
    , event_bus_(event_bus)
{
    SetMinSize(wxSize(-1, kHeight));
    SetMaxSize(wxSize(-1, kHeight));

    SetBackgroundStyle(wxBG_STYLE_PAINT);

    // --- Left buttons: SRC, SPLIT, VIEW, FOCUS ---
    left_buttons_.push_back({{}, "SRC", false, false, 0});
    left_buttons_.push_back({{}, "SPLIT", true, false, 1}); // default active
    left_buttons_.push_back({{}, "VIEW", false, false, 2});
    left_buttons_.push_back({{}, "FOCUS", false, false, 6}); // focus mode

    // --- Right buttons: Save, THEMES, Settings ---
    right_buttons_.push_back({{}, "", false, false, 3});       // Save (icon only)
    right_buttons_.push_back({{}, "THEMES", false, false, 4}); // Palette + label
    right_buttons_.push_back({{}, "", false, false, 5});       // Settings (icon only)

    // --- Events ---
    Bind(wxEVT_PAINT, &Toolbar::OnPaint, this);
    Bind(wxEVT_MOTION, &Toolbar::OnMouseMove, this);
    Bind(wxEVT_LEAVE_WINDOW, &Toolbar::OnMouseLeave, this);
    Bind(wxEVT_LEFT_DOWN, &Toolbar::OnMouseDown, this);
    Bind(wxEVT_LEFT_UP,
         [this](wxMouseEvent& /*evt*/)
         {
             pressed_button_index_ = -1;
             Refresh();
         });
    Bind(wxEVT_SIZE, &Toolbar::OnSize, this);

    // Subscribe to view mode changes from external sources
    view_mode_sub_ = event_bus_.subscribe<core::events::ViewModeChangedEvent>(
        [this](const core::events::ViewModeChangedEvent& evt) { SetActiveViewMode(evt.mode); });

    // Subscribe to focus mode changes
    focus_mode_sub_ = event_bus_.subscribe<core::events::FocusModeChangedEvent>(
        [this](const core::events::FocusModeChangedEvent& evt)
        {
            focus_mode_active_ = evt.active;
            if (left_buttons_.size() > 3)
            {
                left_buttons_[3].is_active = evt.active;
            }
            Refresh();
        });

    // R5 Fix 20: Save flash timer — resets flash after 400ms
    save_flash_timer_.Bind(wxEVT_TIMER,
                           [this](wxTimerEvent& /*evt*/)
                           {
                               save_flash_active_ = false;
                               Refresh();
                           });

    RecalculateButtonRects();
}

// ═══════════════════════════════════════════════════════
// Active Mode
// ═══════════════════════════════════════════════════════

void Toolbar::SetActiveViewMode(core::events::ViewMode mode)
{
    active_mode_ = mode;

    // Update active states
    if (left_buttons_.size() >= 3)
    {
        left_buttons_[0].is_active = (mode == core::events::ViewMode::Editor);
        left_buttons_[1].is_active = (mode == core::events::ViewMode::Split);
        left_buttons_[2].is_active = (mode == core::events::ViewMode::Preview);
    }

    Refresh();
}

auto Toolbar::GetActiveViewMode() const -> core::events::ViewMode
{
    return active_mode_;
}

void Toolbar::SetOnThemeGalleryClick(ThemeGalleryCallback callback)
{
    on_theme_gallery_click_ = std::move(callback);
}

// ═══════════════════════════════════════════════════════
// Button Rects
// ═══════════════════════════════════════════════════════

void Toolbar::RecalculateButtonRects()
{
    // Measure text widths using a temporary DC
    wxClientDC dc(this);
    auto font = wxFont(wxFontInfo(kFontSizeLabel).Family(wxFONTFAMILY_SWISS).Bold());
    dc.SetFont(font);

    int x = kLeftMargin;
    int btn_height = kHeight - 2 * kButtonPadV;

    // Left buttons
    for (auto& btn : left_buttons_)
    {
        int text_width = 0;
        if (!btn.label.empty())
        {
            wxSize ts = dc.GetTextExtent(btn.label);
            text_width = ts.GetWidth();
        }
        int btn_width = kIconSize + kButtonPadH * 2 + (text_width > 0 ? text_width + 6 : 0);
        btn.rect = wxRect(x, kButtonPadV, btn_width, btn_height);
        x += btn_width + kButtonGap;
    }

    // Right buttons — position from right edge
    int client_width = GetClientSize().GetWidth();
    if (client_width <= 0)
    {
        client_width = 800; // fallback
    }

    int rx = client_width - kRightMargin;
    for (int i = static_cast<int>(right_buttons_.size()) - 1; i >= 0; --i)
    {
        auto& btn = right_buttons_[static_cast<size_t>(i)];
        int text_width = 0;
        if (!btn.label.empty())
        {
            wxSize ts = dc.GetTextExtent(btn.label);
            text_width = ts.GetWidth();
        }
        int btn_width = kIconSize + kButtonPadH * 2 + (text_width > 0 ? text_width + 6 : 0);
        rx -= btn_width;
        btn.rect = wxRect(rx, kButtonPadV, btn_width, btn_height);
        rx -= kButtonGap;
    }
}

void Toolbar::OnSize(wxSizeEvent& event)
{
    RecalculateButtonRects();
    Refresh();
    event.Skip();
}

// ═══════════════════════════════════════════════════════
// Mouse
// ═══════════════════════════════════════════════════════

void Toolbar::OnMouseMove(wxMouseEvent& event)
{
    wxPoint pos = event.GetPosition();
    bool changed = false;
    bool any_hovered = false;

    // R5 Fix 1: Tooltip text per button
    static const std::array<std::string, 4> kLeftTooltips = {"Editor Only (Ctrl+1)",
                                                             "Split View (Ctrl+2)",
                                                             "Preview Only (Ctrl+3)",
                                                             "Focus Mode (Ctrl+K)"};
    static const std::array<std::string, 3> kRightTooltips = {
        "Save (Ctrl+S)", "Themes", "Settings"};

    for (size_t idx = 0; idx < left_buttons_.size(); ++idx)
    {
        auto& btn = left_buttons_[idx];
        bool was = btn.is_hovered;
        btn.is_hovered = btn.rect.Contains(pos);
        if (was != btn.is_hovered)
        {
            changed = true;
        }
        if (btn.is_hovered)
        {
            any_hovered = true;
            if (idx < kLeftTooltips.size())
            {
                SetToolTip(kLeftTooltips[idx]);
            }
        }
    }
    for (size_t idx = 0; idx < right_buttons_.size(); ++idx)
    {
        auto& btn = right_buttons_[idx];
        bool was = btn.is_hovered;
        btn.is_hovered = btn.rect.Contains(pos);
        if (was != btn.is_hovered)
        {
            changed = true;
        }
        if (btn.is_hovered)
        {
            any_hovered = true;
            if (idx < kRightTooltips.size())
            {
                SetToolTip(kRightTooltips[idx]);
            }
        }
    }

    if (!any_hovered)
    {
        UnsetToolTip();
    }

    // R5 Fix 2: Hand cursor on button hover
    SetCursor(any_hovered ? wxCursor(wxCURSOR_HAND) : wxNullCursor);

    if (changed)
    {
        Refresh();
    }
}

void Toolbar::OnMouseLeave(wxMouseEvent& /*event*/)
{
    for (auto& btn : left_buttons_)
    {
        btn.is_hovered = false;
    }
    for (auto& btn : right_buttons_)
    {
        btn.is_hovered = false;
    }
    // R5 Fix 2: Restore default cursor
    SetCursor(wxNullCursor);
    UnsetToolTip();
    Refresh();
}

void Toolbar::OnMouseDown(wxMouseEvent& event)
{
    wxPoint pos = event.GetPosition();

    // Check view mode buttons
    for (size_t i = 0; i < left_buttons_.size(); ++i)
    {
        if (left_buttons_[i].rect.Contains(pos))
        {
            // R17 Fix 3: Track pressed button for visual feedback
            pressed_button_index_ = static_cast<int>(i);
            pressed_is_left_ = true;
            Refresh();

            // Focus mode button (index 3)
            if (i == 3)
            {
                event_bus_.publish(core::events::FocusModeChangedEvent{!focus_mode_active_});
                return;
            }

            core::events::ViewMode mode{};
            switch (i)
            {
                case 0:
                    mode = core::events::ViewMode::Editor;
                    break;
                case 1:
                    mode = core::events::ViewMode::Split;
                    break;
                case 2:
                    mode = core::events::ViewMode::Preview;
                    break;
                default:
                    return;
            }

            if (mode != active_mode_)
            {
                event_bus_.publish(core::events::ViewModeChangedEvent{mode});
            }
            return;
        }
    }

    // Right buttons — Save, Themes, Settings
    for (size_t i = 0; i < right_buttons_.size(); ++i)
    {
        if (right_buttons_[i].rect.Contains(pos))
        {
            // R17 Fix 3: Track pressed button
            pressed_button_index_ = static_cast<int>(i);
            pressed_is_left_ = false;
            Refresh();

            if (i == 0)
            {
                // R5 Fix 20: Save button — flash green and publish save event
                event_bus_.publish(core::events::TabSaveRequestEvent{});
                save_flash_active_ = true;
                save_flash_timer_.StartOnce(400);
                Refresh();
            }
            else if (i == 1 && on_theme_gallery_click_)
            {
                on_theme_gallery_click_();
            }
            else if (i == 2)
            {
                // R17 Fix 4: Settings button opens settings panel
                event_bus_.publish(core::events::ActivityBarSelectionEvent(
                    core::events::ActivityBarItem::Settings));
            }
            return;
        }
    }
}

// ═══════════════════════════════════════════════════════
// Painting
// ═══════════════════════════════════════════════════════

void Toolbar::OnPaint(wxPaintEvent& /*event*/)
{
    wxAutoBufferedPaintDC dc(this);
    const auto& t = theme();

    dc.SetBackground(wxBrush(wxColour(t.colors.bg_panel.to_rgba_string())));
    dc.Clear();

    // 8D: subtle gradient overlay — lighter at bottom
    {
        auto base_col = wxColour(t.colors.bg_panel.to_rgba_string());
        auto lighter = base_col.ChangeLightness(103);
        int bar_h = GetClientSize().GetHeight();
        int bar_w = GetClientSize().GetWidth();
        for (int row = 0; row < bar_h - 1; ++row)
        {
            const double frac =
                static_cast<double>(row) / static_cast<double>(std::max(bar_h - 1, 1));
            auto lerp = [](int from, int to, double ratio) -> unsigned char
            {
                return static_cast<unsigned char>(
                    std::clamp(static_cast<int>(from + ratio * (to - from)), 0, 255));
            };
            dc.SetPen(wxPen(wxColour(lerp(base_col.Red(), lighter.Red(), frac),
                                     lerp(base_col.Green(), lighter.Green(), frac),
                                     lerp(base_col.Blue(), lighter.Blue(), frac)),
                            1));
            dc.DrawLine(0, row, bar_w, row);
        }
    }

    // 8B: soft bottom border — BorderLight at 40% alpha
    {
        auto border_col = wxColour(t.colors.border_light.to_rgba_string());
        dc.SetPen(wxPen(wxColour(border_col.Red(), border_col.Green(), border_col.Blue(), 102), 1));
    }
    int bottom_y = GetClientSize().GetHeight() - 1;
    dc.DrawLine(0, bottom_y, GetClientSize().GetWidth(), bottom_y);

    // Draw buttons using wxGraphicsContext
    auto gc_ptr = wxGraphicsContext::Create(dc);
    if (gc_ptr == nullptr)
    {
        return;
    }
    auto& gc = *gc_ptr;

    for (int idx = 0; idx < static_cast<int>(left_buttons_.size()); ++idx)
    {
        DrawButton(gc, left_buttons_[static_cast<size_t>(idx)], t);

        // R17 Fix 1: Active button underline — 2px accent line beneath active view mode buttons
        if (left_buttons_[static_cast<size_t>(idx)].is_active && idx < 3)
        {
            auto accent = wxColour(t.colors.accent_primary.to_rgba_string());
            gc.SetPen(gc.CreatePen(wxGraphicsPenInfo(accent).Width(2.0)));
            gc.SetBrush(*wxTRANSPARENT_BRUSH);
            double ux = left_buttons_[static_cast<size_t>(idx)].rect.GetX() + 2;
            double uw = left_buttons_[static_cast<size_t>(idx)].rect.GetWidth() - 4;
            double uy = left_buttons_[static_cast<size_t>(idx)].rect.GetBottom();
            gc.StrokeLine(ux, uy, ux + uw, uy);
        }
    }

    // R17 Fix 5: Vertical separator between left and right button groups
    if (!left_buttons_.empty() && !right_buttons_.empty())
    {
        int sep_x =
            (left_buttons_.back().rect.GetRight() + right_buttons_.front().rect.GetLeft()) / 2;
        auto sep_col = wxColour(t.colors.border_light.to_rgba_string());
        gc.SetPen(gc.CreatePen(wxGraphicsPenInfo(sep_col).Width(1.0)));
        gc.StrokeLine(sep_x, 8, sep_x, GetClientSize().GetHeight() - 8);
    }

    for (const auto& btn : right_buttons_)
    {
        DrawButton(gc, btn, t);
    }

    delete gc_ptr;
}

void Toolbar::DrawButton(wxGraphicsContext& gc, const ButtonInfo& btn, const core::Theme& t) const
{
    const auto& c = t.colors;
    double rx = btn.rect.GetX();
    double ry = btn.rect.GetY();
    double rw = btn.rect.GetWidth();
    double rh = btn.rect.GetHeight();

    // R17 Fix 2: Dim save button when no file is modified (icon_type 3 = save)
    // (Opacity handled in text color below — skip background for non-flash clean save)

    // Background
    // R5 Fix 20: Green flash for save button after save
    if (save_flash_active_ && btn.icon_type == 3)
    {
        gc.SetBrush(gc.CreateBrush(wxBrush(wxColour(50, 205, 50, 80)))); // lime green flash
        gc.SetPen(*wxTRANSPARENT_PEN);
        gc.DrawRoundedRectangle(rx, ry, rw, rh, 4.0);
    }
    else if (btn.is_active)
    {
        auto bg = c.accent_primary.with_alpha(0.20f).to_rgba_string();
        gc.SetBrush(gc.CreateBrush(wxBrush(wxColour(bg))));
        gc.SetPen(*wxTRANSPARENT_PEN);
        gc.DrawRoundedRectangle(rx, ry, rw, rh, 4.0);
    }
    else if (btn.is_hovered)
    {
        auto bg = c.text_main.with_alpha(0.08f).to_rgba_string();
        gc.SetBrush(gc.CreateBrush(wxBrush(wxColour(bg))));
        gc.SetPen(*wxTRANSPARENT_PEN);
        gc.DrawRoundedRectangle(rx, ry, rw, rh, 4.0);
    }

    // Text/icon color
    wxColour text_color;
    if (btn.is_active)
    {
        text_color = wxColour(c.accent_primary.to_rgba_string());
    }
    else if (btn.is_hovered)
    {
        text_color = wxColour(c.text_main.to_rgba_string());
    }
    else
    {
        text_color = wxColour(c.text_muted.to_rgba_string());
    }

    gc.SetPen(gc.CreatePen(wxGraphicsPenInfo(text_color).Width(1.5)));
    gc.SetBrush(*wxTRANSPARENT_BRUSH);

    // Draw icon (centered vertically, left-aligned in button)
    double icon_x = rx + kButtonPadH;
    double icon_y = ry + (rh - kIconSize) / 2.0;

    switch (btn.icon_type)
    {
        case 0:
            DrawCodeIcon(gc, icon_x, icon_y, kIconSize);
            break;
        case 1:
            DrawColumnsIcon(gc, icon_x, icon_y, kIconSize);
            break;
        case 2:
            DrawEyeIcon(gc, icon_x, icon_y, kIconSize);
            break;
        case 3:
            DrawSaveIcon(gc, icon_x, icon_y, kIconSize);
            break;
        case 4:
            DrawPaletteIcon(gc, icon_x, icon_y, kIconSize);
            break;
        case 5:
            DrawGearIcon(gc, icon_x, icon_y, kIconSize);
            break;
        case 6:
            DrawFocusIcon(gc, icon_x, icon_y, kIconSize);
            break;
        default:
            break;
    }

    // Draw label
    if (!btn.label.empty())
    {
        auto font = wxFont(wxFontInfo(kFontSizeLabel).Family(wxFONTFAMILY_SWISS).Bold());
        gc.SetFont(font, text_color);
        double text_x = icon_x + kIconSize + 6;
        double text_y = ry + (rh - kFontSizeLabel - 2) / 2.0;
        gc.DrawText(btn.label, text_x, text_y);
    }
}

// ═══════════════════════════════════════════════════════
// Icon Drawing
// ═══════════════════════════════════════════════════════

void Toolbar::DrawCodeIcon(wxGraphicsContext& gc, double x, double y, double size) const
{
    // </>  brackets
    double mid_y = y + size / 2.0;
    double left_x = x + 2;
    double right_x = x + size - 2;

    // Left angle bracket <
    auto path = gc.CreatePath();
    path.MoveToPoint(left_x + 4, y + 2);
    path.AddLineToPoint(left_x, mid_y);
    path.AddLineToPoint(left_x + 4, y + size - 2);
    gc.StrokePath(path);

    // Right angle bracket >
    auto path2 = gc.CreatePath();
    path2.MoveToPoint(right_x - 4, y + 2);
    path2.AddLineToPoint(right_x, mid_y);
    path2.AddLineToPoint(right_x - 4, y + size - 2);
    gc.StrokePath(path2);

    // Slash /
    auto path3 = gc.CreatePath();
    path3.MoveToPoint(x + size * 0.55, y + 2);
    path3.AddLineToPoint(x + size * 0.45, y + size - 2);
    gc.StrokePath(path3);
}

void Toolbar::DrawColumnsIcon(wxGraphicsContext& gc, double x, double y, double size) const
{
    // Two vertical rectangles
    double gap = 2;
    double col_w = (size - gap) / 2.0;

    gc.SetPen(*wxTRANSPARENT_PEN);
    auto pen_color = gc.CreatePen(
        wxGraphicsPenInfo(wxColour(theme().colors.accent_primary.to_rgba_string())).Width(0));

    // Use the current pen color for fill
    wxColour fill_color;
    if (left_buttons_.size() > 1 && left_buttons_[1].is_active)
    {
        fill_color = wxColour(theme().colors.accent_primary.to_rgba_string());
    }
    else if (left_buttons_.size() > 1 && left_buttons_[1].is_hovered)
    {
        fill_color = wxColour(theme().colors.text_main.to_rgba_string());
    }
    else
    {
        fill_color = wxColour(theme().colors.text_muted.to_rgba_string());
    }

    gc.SetBrush(gc.CreateBrush(wxBrush(fill_color)));
    gc.DrawRoundedRectangle(x, y + 1, col_w, size - 2, 1.5);
    gc.DrawRoundedRectangle(x + col_w + gap, y + 1, col_w, size - 2, 1.5);
    gc.SetBrush(*wxTRANSPARENT_BRUSH);
}

void Toolbar::DrawEyeIcon(wxGraphicsContext& gc, double x, double y, double size) const
{
    // Stylized eye shape
    double mid_x = x + size / 2.0;
    double mid_y = y + size / 2.0;

    auto path = gc.CreatePath();
    // Upper arc
    path.MoveToPoint(x + 1, mid_y);
    path.AddCurveToPoint(x + size * 0.25, y + 2, x + size * 0.75, y + 2, x + size - 1, mid_y);
    // Lower arc
    path.AddCurveToPoint(
        x + size * 0.75, y + size - 2, x + size * 0.25, y + size - 2, x + 1, mid_y);
    gc.StrokePath(path);

    // Pupil (filled circle)
    wxColour fill;
    if (left_buttons_.size() > 2 && left_buttons_[2].is_active)
    {
        fill = wxColour(theme().colors.accent_primary.to_rgba_string());
    }
    else if (left_buttons_.size() > 2 && left_buttons_[2].is_hovered)
    {
        fill = wxColour(theme().colors.text_main.to_rgba_string());
    }
    else
    {
        fill = wxColour(theme().colors.text_muted.to_rgba_string());
    }
    gc.SetBrush(gc.CreateBrush(wxBrush(fill)));
    gc.DrawEllipse(mid_x - 2.5, mid_y - 2.5, 5, 5);
    gc.SetBrush(*wxTRANSPARENT_BRUSH);
}

void Toolbar::DrawSaveIcon(wxGraphicsContext& gc, double x, double y, double size) const
{
    // Floppy disk shape
    gc.SetBrush(*wxTRANSPARENT_BRUSH);
    gc.DrawRoundedRectangle(x + 1, y + 1, size - 2, size - 2, 2.0);

    // Inner rectangle (disk label)
    auto path = gc.CreatePath();
    path.MoveToPoint(x + 3, y + size * 0.55);
    path.AddLineToPoint(x + size - 3, y + size * 0.55);
    path.AddLineToPoint(x + size - 3, y + size - 3);
    path.AddLineToPoint(x + 3, y + size - 3);
    path.CloseSubpath();
    gc.StrokePath(path);

    // Top notch
    auto path2 = gc.CreatePath();
    path2.MoveToPoint(x + 4, y + 1);
    path2.AddLineToPoint(x + 4, y + 4);
    path2.AddLineToPoint(x + size - 4, y + 4);
    path2.AddLineToPoint(x + size - 4, y + 1);
    gc.StrokePath(path2);
}

void Toolbar::DrawPaletteIcon(wxGraphicsContext& gc, double x, double y, double size) const
{
    // Simple palette shape — circle with dots
    gc.SetBrush(*wxTRANSPARENT_BRUSH);
    gc.DrawEllipse(x + 1, y + 1, size - 2, size - 2);

    // Color dots
    wxColour dot_color;
    if (right_buttons_.size() > 1 && right_buttons_[1].is_hovered)
    {
        dot_color = wxColour(theme().colors.accent_primary.to_rgba_string());
    }
    else
    {
        dot_color = wxColour(theme().colors.text_muted.to_rgba_string());
    }
    gc.SetBrush(gc.CreateBrush(wxBrush(dot_color)));
    gc.DrawEllipse(x + 3, y + 3, 2.5, 2.5);
    gc.DrawEllipse(x + 7, y + 3, 2.5, 2.5);
    gc.DrawEllipse(x + 3, y + 8, 2.5, 2.5);
    gc.SetBrush(*wxTRANSPARENT_BRUSH);
}

void Toolbar::DrawGearIcon(wxGraphicsContext& gc, double x, double y, double size) const
{
    // Simplified gear — circle with notches
    double cx = x + size / 2.0;
    double cy = y + size / 2.0;
    double outer_r = size / 2.0 - 1;
    double inner_r = outer_r * 0.55;

    // Outer circle
    gc.SetBrush(*wxTRANSPARENT_BRUSH);
    gc.DrawEllipse(cx - outer_r, cy - outer_r, outer_r * 2, outer_r * 2);

    // Inner circle
    gc.DrawEllipse(cx - inner_r, cy - inner_r, inner_r * 2, inner_r * 2);

    // Tick marks (gear teeth) at 0, 60, 120, 180, 240, 300 degrees
    for (int i = 0; i < 6; ++i)
    {
        double angle = static_cast<double>(i) * M_PI / 3.0;
        double x1 = cx + inner_r * std::cos(angle);
        double y1 = cy + inner_r * std::sin(angle);
        double x2 = cx + outer_r * std::cos(angle);
        double y2 = cy + outer_r * std::sin(angle);

        auto path = gc.CreatePath();
        path.MoveToPoint(x1, y1);
        path.AddLineToPoint(x2, y2);
        gc.StrokePath(path);
    }
}

void Toolbar::DrawFocusIcon(wxGraphicsContext& gc, double x, double y, double size) const
{
    // Center-align icon: 3 horizontal lines, center one wider
    double cx = x + size / 2.0;
    double line_h = size * 0.15;
    double short_w = size * 0.5;
    double long_w = size * 0.8;

    // Determine fill color based on state
    wxColour fill_color;
    if (left_buttons_.size() > 3 && left_buttons_[3].is_active)
    {
        fill_color = wxColour(theme().colors.accent_primary.to_rgba_string());
    }
    else if (left_buttons_.size() > 3 && left_buttons_[3].is_hovered)
    {
        fill_color = wxColour(theme().colors.text_main.to_rgba_string());
    }
    else
    {
        fill_color = wxColour(theme().colors.text_muted.to_rgba_string());
    }

    gc.SetBrush(gc.CreateBrush(wxBrush(fill_color)));
    gc.SetPen(*wxTRANSPARENT_PEN);

    // Top line (short, centered)
    gc.DrawRoundedRectangle(cx - short_w / 2, y + 2, short_w, line_h, 1.0);
    // Middle line (longer, centered)
    gc.DrawRoundedRectangle(cx - long_w / 2, y + size / 2 - line_h / 2, long_w, line_h, 1.0);
    // Bottom line (short, centered)
    gc.DrawRoundedRectangle(cx - short_w / 2, y + size - 2 - line_h, short_w, line_h, 1.0);

    gc.SetBrush(*wxTRANSPARENT_BRUSH);
}

// ═══════════════════════════════════════════════════════
// Theme
// ═══════════════════════════════════════════════════════

void Toolbar::OnThemeChanged(const core::Theme& new_theme)
{
    ThemeAwareWindow::OnThemeChanged(new_theme);
    Refresh();
}

} // namespace markamp::ui
