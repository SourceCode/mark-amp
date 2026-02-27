#include "ui/ExplorerToolbar.h"

#include "core/ThemeEngine.h"
#include "ui/IconManager.h"

#include <wx/dcbuffer.h>

namespace markamp::ui
{

wxBEGIN_EVENT_TABLE(ExplorerToolbar, ThemeAwareWindow) EVT_PAINT(ExplorerToolbar::OnPaint)
    EVT_LEAVE_WINDOW(ExplorerToolbar::OnMouseLeave) EVT_MOTION(ExplorerToolbar::OnMouseMove)
        EVT_LEFT_DOWN(ExplorerToolbar::OnLeftDown) EVT_LEFT_UP(ExplorerToolbar::OnLeftUp)
            EVT_SIZE(ExplorerToolbar::OnSize) wxEND_EVENT_TABLE()

                ExplorerToolbar::ExplorerToolbar(wxWindow* parent,
                                                 core::ThemeEngine& theme_engine,
                                                 IconManager& icon_manager)
    : ThemeAwareWindow(parent,
                       theme_engine,
                       wxID_ANY,
                       wxDefaultPosition,
                       wxDefaultSize,
                       wxBORDER_NONE | wxFULL_REPAINT_ON_RESIZE)
    , icon_manager_(icon_manager)
{
    SetBackgroundStyle(wxBG_STYLE_PAINT);

    buttons_ = {{Action::kNewFile, "file-add", wxRect(), "New File"},
                {Action::kNewFolder, "folder-add", wxRect(), "New Folder"},
                {Action::kRefresh, "refresh", wxRect(), "Refresh Explorer"},
                {Action::kCollapseAll, "collapse-all", wxRect(), "Collapse Folders"},
                {Action::kFilter, "filter", wxRect(), "Filter Workspace"}};

    UpdateLayout();
}

void ExplorerToolbar::SetActionCallback(ActionCallback callback)
{
    callback_ = std::move(callback);
}

void ExplorerToolbar::UpdateLayout()
{
    const int icon_size = 16;
    const int padding = 4;
    const int spacing = 2;
    int current_x = 0;

    for (auto& btn : buttons_)
    {
        btn.bounds = wxRect(current_x, 0, icon_size + padding * 2, icon_size + padding * 2);
        current_x += btn.bounds.width + spacing;
    }

    SetMinSize(wxSize(current_x, icon_size + padding * 2));
}

int ExplorerToolbar::GetButtonHit(const wxPoint& pos) const
{
    for (size_t i = 0; i < buttons_.size(); ++i)
    {
        if (buttons_[i].bounds.Contains(pos))
        {
            return static_cast<int>(i);
        }
    }
    return -1;
}

void ExplorerToolbar::OnPaint(wxPaintEvent& /*event*/)
{
    wxAutoBufferedPaintDC dc(this);
    // The ExplorerToolbar has no background to blend into the header
    dc.SetBackground(*wxTRANSPARENT_BRUSH);
    dc.Clear();

    const int icon_size = 16;
    const auto active_fg = theme_engine().color(core::ThemeColorToken::SidebarFg);
    const auto hover_bg = theme_engine().color(core::ThemeColorToken::HoverBg);
    const auto inactive_fg = theme_engine().color(core::ThemeColorToken::TextMuted);

    for (size_t i = 0; i < buttons_.size(); ++i)
    {
        const auto& btn = buttons_[i];
        bool is_hovered = (static_cast<int>(i) == hovered_index_);

        if (is_hovered)
        {
            dc.SetPen(*wxTRANSPARENT_PEN);
            auto brush_color = hover_bg;
            if (static_cast<int>(i) == pressed_index_)
            {
                // Slightly darker/lighter when pressed
                int delta = 20;
                brush_color = wxColour(
                    static_cast<unsigned char>(std::clamp(hover_bg.Red() - delta, 0, 255)),
                    static_cast<unsigned char>(std::clamp(hover_bg.Green() - delta, 0, 255)),
                    static_cast<unsigned char>(std::clamp(hover_bg.Blue() - delta, 0, 255)));
            }
            dc.SetBrush(wxBrush(brush_color));
            dc.DrawRoundedRectangle(btn.bounds, 3.0);
        }

        auto icon_color = is_hovered ? active_fg : inactive_fg;
        int icon_x = btn.bounds.x + (btn.bounds.width - icon_size) / 2;
        int icon_y = btn.bounds.y + (btn.bounds.height - icon_size) / 2;

        icon_manager_.draw_icon(
            dc, btn.icon_name, icon_x, icon_y, wxSize(icon_size, icon_size), icon_color);
    }
}

void ExplorerToolbar::OnMouseLeave(wxMouseEvent& event)
{
    if (hovered_index_ != -1)
    {
        hovered_index_ = -1;
        pressed_index_ = -1;
        SetToolTip(nullptr);
        Refresh();
    }
    event.Skip();
}

void ExplorerToolbar::OnMouseMove(wxMouseEvent& event)
{
    int hit = GetButtonHit(event.GetPosition());
    if (hit != hovered_index_)
    {
        hovered_index_ = hit;
        if (hit != -1)
        {
            SetToolTip(buttons_[static_cast<std::size_t>(hit)].tooltip);
        }
        else
        {
            SetToolTip(nullptr);
        }
        Refresh();
    }
    event.Skip();
}

void ExplorerToolbar::OnLeftDown(wxMouseEvent& event)
{
    int hit = GetButtonHit(event.GetPosition());
    if (hit != -1)
    {
        pressed_index_ = hit;
        Refresh();
    }
    event.Skip();
}

void ExplorerToolbar::OnLeftUp(wxMouseEvent& event)
{
    int hit = GetButtonHit(event.GetPosition());
    if (hit != -1 && hit == pressed_index_)
    {
        if (callback_)
        {
            callback_(buttons_[static_cast<std::size_t>(hit)].action);
        }
    }
    pressed_index_ = -1;
    Refresh();
    event.Skip();
}

void ExplorerToolbar::OnSize(wxSizeEvent& event)
{
    event.Skip();
}

void ExplorerToolbar::OnThemeChanged(const core::Theme& /*new_theme*/)
{
    Refresh();
}

} // namespace markamp::ui
