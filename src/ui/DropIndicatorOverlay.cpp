#include "DropIndicatorOverlay.h"

#include "core/ThemeEngine.h"
#include "core/ThemeTokens.h"

#include <wx/dcbuffer.h>
#include <wx/graphics.h>

namespace markamp::ui
{

DropIndicatorOverlay::DropIndicatorOverlay(wxWindow* parent, core::ThemeEngine& theme_engine)
    : ThemeAwareWindow(
          parent, theme_engine, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE)
{
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    Bind(wxEVT_PAINT, &DropIndicatorOverlay::on_paint, this);
    Hide();
}

void DropIndicatorOverlay::show_indicator(const wxRect& target_rect, DropDirection direction)
{
    target_rect_ = target_rect;
    direction_ = direction;
    is_showing_ = true;

    wxRect overlay_rect;
    switch (direction)
    {
        case DropDirection::kLeft:
            overlay_rect =
                wxRect(target_rect.x, target_rect.y, kIndicatorThickness, target_rect.height);
            break;
        case DropDirection::kRight:
            overlay_rect = wxRect(target_rect.GetRight() - kIndicatorThickness,
                                  target_rect.y,
                                  kIndicatorThickness,
                                  target_rect.height);
            break;
        case DropDirection::kTop:
            overlay_rect =
                wxRect(target_rect.x, target_rect.y, target_rect.width, kIndicatorThickness);
            break;
        case DropDirection::kBottom:
            overlay_rect = wxRect(target_rect.x,
                                  target_rect.GetBottom() - kIndicatorThickness,
                                  target_rect.width,
                                  kIndicatorThickness);
            break;
        case DropDirection::kBetween:
            overlay_rect =
                wxRect(target_rect.x, target_rect.y - 1, target_rect.width, kIndicatorThickness);
            break;
        case DropDirection::kCenter:
        case DropDirection::kNone:
        default:
            overlay_rect = target_rect;
            break;
    }

    SetPosition(overlay_rect.GetPosition());
    SetSize(overlay_rect.GetSize());
    Show();
    Refresh();
}

void DropIndicatorOverlay::hide_indicator()
{
    is_showing_ = false;
    direction_ = DropDirection::kNone;
    Hide();
}

void DropIndicatorOverlay::on_paint(wxPaintEvent& /*event*/)
{
    wxAutoBufferedPaintDC pdc(this);
    auto gc = std::unique_ptr<wxGraphicsContext>(wxGraphicsContext::Create(pdc));
    if (!gc)
    {
        return;
    }

    const auto rect = GetClientRect();
    const auto accent = theme_engine().color(core::ThemeColorToken::AccentPrimary);

    if (direction_ == DropDirection::kCenter)
    {
        // Semi-transparent fill for full-area drop.
        gc->SetBrush(wxBrush(wxColour(accent.Red(), accent.Green(), accent.Blue(), 40)));
        gc->SetPen(wxPen(accent, 2));
        gc->DrawRoundedRectangle(0, 0, rect.width, rect.height, FromDIP(4));
    }
    else
    {
        // Solid line indicator.
        gc->SetBrush(wxBrush(accent));
        gc->SetPen(*wxTRANSPARENT_PEN);
        gc->DrawRectangle(0, 0, rect.width, rect.height);
    }
}

} // namespace markamp::ui
