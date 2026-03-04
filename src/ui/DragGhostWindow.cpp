#include "DragGhostWindow.h"

#include "core/ThemeEngine.h"
#include "core/ThemeTokens.h"

#include <wx/dcbuffer.h>
#include <wx/graphics.h>

namespace markamp::ui
{

DragGhostWindow::DragGhostWindow(wxWindow* parent, core::ThemeEngine& theme_engine)
    : ThemeAwareWindow(parent,
                       theme_engine,
                       wxID_ANY,
                       wxDefaultPosition,
                       wxSize(kGhostWidth, kGhostHeight),
                       wxBORDER_NONE)
{
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    Bind(wxEVT_PAINT, &DragGhostWindow::on_paint, this);
    Hide();
}

void DragGhostWindow::configure_for_tab(const std::string& tab_title)
{
    type_ = DragPayloadType::kTab;
    label_ = tab_title;
    icon_name_.clear();
}

void DragGhostWindow::configure_for_file(const std::string& file_name, const std::string& icon_name)
{
    type_ = DragPayloadType::kFile;
    label_ = file_name;
    icon_name_ = icon_name;
}

void DragGhostWindow::configure_for_panel(const std::string& panel_title)
{
    type_ = DragPayloadType::kPanel;
    label_ = panel_title;
    icon_name_.clear();
}

void DragGhostWindow::move_to(int screen_x, int screen_y)
{
    SetPosition(wxPoint(screen_x + kCursorOffset, screen_y + kCursorOffset));
}

void DragGhostWindow::show_ghost()
{
    is_visible_ = true;
    Show();
    Refresh();
}

void DragGhostWindow::hide_ghost()
{
    is_visible_ = false;
    Hide();
}

void DragGhostWindow::on_paint(wxPaintEvent& /*event*/)
{
    wxAutoBufferedPaintDC pdc(this);
    auto gc = std::unique_ptr<wxGraphicsContext>(wxGraphicsContext::Create(pdc));
    if (!gc)
    {
        return;
    }

    const auto rect = GetClientRect();

    // Semi-transparent background with border
    gc->SetBrush(wxBrush(theme_engine().color(core::ThemeColorToken::BgPanel)));
    gc->SetPen(wxPen(theme_engine().color(core::ThemeColorToken::AccentPrimary), 1));
    gc->DrawRoundedRectangle(0, 0, rect.width, rect.height, FromDIP(4));

    // Label
    gc->SetFont(theme_engine().font(core::ThemeFontToken::UILabel),
                theme_engine().color(core::ThemeColorToken::TextMain));
    gc->DrawText(wxString::FromUTF8(label_), FromDIP(8), FromDIP(6));
}

} // namespace markamp::ui
