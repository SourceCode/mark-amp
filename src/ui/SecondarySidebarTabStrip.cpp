#include "ui/SecondarySidebarTabStrip.h"

#include "core/Config.h"
#include "core/ThemeEngine.h"

#include <wx/dcbuffer.h>
#include <wx/graphics.h>

namespace markamp::ui
{

wxBEGIN_EVENT_TABLE(SecondarySidebarTabStrip, ThemeAwareWindow)
    EVT_PAINT(SecondarySidebarTabStrip::OnPaint) EVT_SIZE(SecondarySidebarTabStrip::OnSize)
        EVT_LEFT_DOWN(SecondarySidebarTabStrip::OnMouseLeftDown)
            EVT_LEFT_UP(SecondarySidebarTabStrip::OnMouseLeftUp)
                EVT_MOTION(SecondarySidebarTabStrip::OnMouseMotion)
                    EVT_LEAVE_WINDOW(SecondarySidebarTabStrip::OnMouseLeave) wxEND_EVENT_TABLE()

                        SecondarySidebarTabStrip::SecondarySidebarTabStrip(
                            wxWindow* parent,
                            core::ThemeEngine& theme_engine,
                            [[maybe_unused]] DesignSystemContext& ds,
                            core::EventBus& event_bus,
                            [[maybe_unused]] core::Config* config)
    : ThemeAwareWindow(parent,
                       theme_engine,
                       wxID_ANY,
                       wxDefaultPosition,
                       wxSize(-1, 38),
                       wxFULL_REPAINT_ON_RESIZE | wxBORDER_NONE)
    , event_bus_(event_bus)
{
    SetBackgroundStyle(wxBG_STYLE_PAINT);
}

void SecondarySidebarTabStrip::AddTab(const std::string& mode,
                                      const std::string& icon,
                                      const std::string& tooltip)
{
    tabs_.push_back({mode, icon, tooltip, wxRect(), false, false});
    UpdateLayoutMetrics();
    Refresh();
}

void SecondarySidebarTabStrip::SetActiveMode(const std::string& mode)
{
    if (active_mode_ != mode)
    {
        active_mode_ = mode;
        Refresh();
    }
}

auto SecondarySidebarTabStrip::GetActiveMode() const -> std::string
{
    return active_mode_;
}

void SecondarySidebarTabStrip::UpdateLayoutMetrics()
{
    if (tabs_.empty())
        return;

    int current_x = 0;
    const int tab_width = 44; // Fixed width for each icon tab
    const int height = GetClientSize().GetHeight();

    for (auto& tab : tabs_)
    {
        tab.rect = wxRect(current_x, 0, tab_width, height);
        current_x += tab_width;
    }
}

void SecondarySidebarTabStrip::OnSize(wxSizeEvent& event)
{
    UpdateLayoutMetrics();
    event.Skip();
    Refresh();
}

void SecondarySidebarTabStrip::OnThemeChanged(const core::Theme& /*new_theme*/)
{
    Refresh();
}

int SecondarySidebarTabStrip::HitTest(const wxPoint& pos) const
{
    for (size_t i = 0; i < tabs_.size(); ++i)
    {
        if (tabs_[i].rect.Contains(pos))
        {
            return static_cast<int>(i);
        }
    }
    return -1;
}

void SecondarySidebarTabStrip::OnMouseMotion(wxMouseEvent& event)
{
    int hit = HitTest(event.GetPosition());
    if (hit != hover_index_)
    {
        hover_index_ = hit;
        if (hover_index_ >= 0 && hover_index_ < static_cast<int>(tabs_.size()))
        {
            SetToolTip(tabs_[static_cast<size_t>(hover_index_)].tooltip);
        }
        else
        {
            SetToolTip("");
        }
        Refresh();
    }
}

void SecondarySidebarTabStrip::OnMouseLeave(wxMouseEvent& /*event*/)
{
    if (hover_index_ != -1)
    {
        hover_index_ = -1;
        Refresh();
    }
    if (pressed_index_ != -1)
    {
        pressed_index_ = -1;
        Refresh();
    }
}

void SecondarySidebarTabStrip::OnMouseLeftDown(wxMouseEvent& event)
{
    pressed_index_ = HitTest(event.GetPosition());
    if (pressed_index_ != -1)
    {
        Refresh();
    }
}

void SecondarySidebarTabStrip::OnMouseLeftUp(wxMouseEvent& event)
{
    int hit = HitTest(event.GetPosition());
    if (hit != -1 && hit == pressed_index_)
    {
        const auto& mode = tabs_[static_cast<size_t>(hit)].mode;
        SetActiveMode(mode);
        event_bus_.publish(core::events::SecondarySidebarSelectionEvent{mode});
    }

    pressed_index_ = -1;
    Refresh();
}

void SecondarySidebarTabStrip::OnPaint(wxPaintEvent& /*event*/)
{
    wxAutoBufferedPaintDC dc(this);
    std::unique_ptr<wxGraphicsContext> gc(wxGraphicsContext::Create(dc));

    if (!gc)
        return;

    auto bg_color = theme_engine()
                        .resolve_token("sidebar.bg")
                        .value_or(theme_engine().color(core::ThemeColorToken::BgPanel))
                        .ChangeLightness(108);
    auto border_color = theme_engine()
                            .resolve_token("border.divider")
                            .value_or(theme_engine().color(core::ThemeColorToken::BorderLight));
    auto active_color = theme_engine()
                            .resolve_token("accent.primary")
                            .value_or(theme_engine().color(core::ThemeColorToken::AccentPrimary));
    auto hover_bg = theme_engine()
                        .resolve_token("list.hover.bg")
                        .value_or(theme_engine().color(core::ThemeColorToken::HoverBg));

    auto text_main = theme_engine()
                         .resolve_token("text.main")
                         .value_or(theme_engine().color(core::ThemeColorToken::TextMain));
    auto text_muted = theme_engine()
                          .resolve_token("text.muted")
                          .value_or(theme_engine().color(core::ThemeColorToken::TextMuted));

    wxRect client_rect = GetClientRect();

    // Fill background
    gc->SetBrush(wxBrush(bg_color));
    gc->SetPen(*wxTRANSPARENT_PEN);
    gc->DrawRectangle(client_rect.x, client_rect.y, client_rect.width, client_rect.height);

    // Draw bottom border
    gc->SetPen(wxPen(border_color));
    gc->StrokeLine(client_rect.x,
                   client_rect.y + client_rect.height - 1,
                   client_rect.x + client_rect.width,
                   client_rect.y + client_rect.height - 1);

    wxFont icon_font = theme_engine().font(core::ThemeFontToken::MonoRegular).Scaled(1.2f);
    gc->SetFont(icon_font, text_main);

    for (int i = 0; i < static_cast<int>(tabs_.size()); ++i)
    {
        const auto& tab = tabs_[static_cast<size_t>(i)];
        bool is_active = (tab.mode == active_mode_);
        bool is_hovered = (i == hover_index_);
        bool is_pressed = (i == pressed_index_);

        if (is_hovered && !is_active)
        {
            gc->SetBrush(wxBrush(hover_bg));
            gc->DrawRectangle(tab.rect.x, tab.rect.y, tab.rect.width, tab.rect.height);
        }

        if (is_active)
        {
            // Active text color
            gc->SetFont(icon_font, active_color);
            // active top or bottom border indicator (VS Code style top border for panel tabs, but
            // bottom border works well too)
            gc->SetPen(wxPen(active_color, 2));
            gc->StrokeLine(tab.rect.x,
                           tab.rect.y + tab.rect.height - 2,
                           tab.rect.x + tab.rect.width,
                           tab.rect.y + tab.rect.height - 2);
        }
        else
        {
            gc->SetFont(icon_font, is_hovered ? text_main : text_muted);
        }

        double text_width, text_height, descent, external_leading;
        gc->GetTextExtent(tab.icon, &text_width, &text_height, &descent, &external_leading);

        double draw_x = tab.rect.x + (tab.rect.width - text_width) / 2.0;
        double draw_y = tab.rect.y + (tab.rect.height - text_height) / 2.0;

        if (is_pressed)
        {
            draw_y += 1.0; // slight push effect
        }

        gc->DrawText(tab.icon, draw_x, draw_y);
    }
}

} // namespace markamp::ui
