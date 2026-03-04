#include "LightbulbWidget.h"

#include <wx/brush.h>
#include <wx/pen.h>

namespace markamp::ui
{

void LightbulbWidget::SetVisible(bool visible, int line, core::CodeActionKind primary_kind)
{
    visible_ = visible;
    line_ = line;
    kind_ = primary_kind;
}

void LightbulbWidget::Hide()
{
    visible_ = false;
    line_ = -1;
}

auto LightbulbWidget::IsVisible() const -> bool
{
    return visible_;
}

auto LightbulbWidget::GetLine() const -> int
{
    return line_;
}

auto LightbulbWidget::GetKind() const -> core::CodeActionKind
{
    return kind_;
}

void LightbulbWidget::Paint(wxDC& device_context, int pos_x, int pos_y, int size) const
{
    if (!visible_)
    {
        return;
    }

    // Cache position for hit testing
    last_x_ = pos_x;
    last_y_ = pos_y;
    last_size_ = size;

    // Choose color based on the action kind
    wxColour bulb_color;
    switch (kind_)
    {
        case core::CodeActionKind::kQuickFix:
        {
            bulb_color = wxColour(255, 204, 0); // Yellow
            break;
        }
        case core::CodeActionKind::kRefactor:
        case core::CodeActionKind::kRefactorExtract:
        case core::CodeActionKind::kRefactorInline:
        {
            bulb_color = wxColour(68, 138, 255); // Blue
            break;
        }
        case core::CodeActionKind::kSource:
        case core::CodeActionKind::kSourceFixAll:
        {
            bulb_color = wxColour(160, 160, 160); // Grey
            break;
        }
    }

    // Draw the lightbulb: a filled circle as the bulb
    const int bulb_radius = size / 3;
    const int center_x = pos_x + size / 2;
    const int center_y = pos_y + size / 3;

    device_context.SetPen(wxPen(bulb_color.ChangeLightness(80), 1));
    device_context.SetBrush(wxBrush(bulb_color));
    device_context.DrawCircle(center_x, center_y, bulb_radius);

    // Draw the base (two small horizontal lines below the circle)
    const int base_y = center_y + bulb_radius + 1;
    const int base_half = bulb_radius / 2;
    device_context.SetPen(wxPen(bulb_color.ChangeLightness(70), 1));
    device_context.DrawLine(center_x - base_half, base_y, center_x + base_half, base_y);
    device_context.DrawLine(
        center_x - base_half + 1, base_y + 2, center_x + base_half - 1, base_y + 2);
}

auto LightbulbWidget::HitTest(int click_x, int click_y) const -> bool
{
    if (!visible_)
    {
        return false;
    }

    return click_x >= last_x_ && click_x <= last_x_ + last_size_ && click_y >= last_y_ &&
           click_y <= last_y_ + last_size_;
}

} // namespace markamp::ui
