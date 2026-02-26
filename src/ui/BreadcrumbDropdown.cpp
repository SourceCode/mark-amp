#include "ui/BreadcrumbDropdown.h"

#include "core/Events.h"
#include "ui/IconManager.h"
#include "ui/TypographyScale.h"
#include "ui/accessibility/AccessibilityController.h"

#include <wx/dcbuffer.h>
#include <wx/display.h>

namespace markamp::ui
{

const core::ThemeColorToken BreadcrumbDropdown::kBgColorToken = core::ThemeColorToken::BgPanel;
const core::ThemeColorToken BreadcrumbDropdown::kHoverColorToken =
    core::ThemeColorToken::ControlBgHover;
const core::ThemeColorToken BreadcrumbDropdown::kBorderColorToken =
    core::ThemeColorToken::BorderLight;
const core::ThemeColorToken BreadcrumbDropdown::kTextColorToken = core::ThemeColorToken::TextMain;

wxBEGIN_EVENT_TABLE(BreadcrumbDropdown,
                    wxPopupTransientWindow) EVT_PAINT(BreadcrumbDropdown::OnPaint)
    EVT_MOTION(BreadcrumbDropdown::OnMouseMove) EVT_LEAVE_WINDOW(BreadcrumbDropdown::OnMouseLeave)
        EVT_LEFT_DOWN(BreadcrumbDropdown::OnMouseLeftDown) EVT_LEFT_UP(
            BreadcrumbDropdown::OnMouseLeftUp) EVT_KEY_DOWN(BreadcrumbDropdown::OnKeyDown)
            EVT_SET_FOCUS(BreadcrumbDropdown::OnSetFocus)
                EVT_KILL_FOCUS(BreadcrumbDropdown::OnKillFocus) wxEND_EVENT_TABLE()

                    BreadcrumbDropdown::BreadcrumbDropdown(wxWindow* parent,
                                                           DesignSystemContext& ds,
                                                           core::EventBus& event_bus,
                                                           const std::vector<DropdownItem>& items)
    : wxPopupTransientWindow(parent, wxBORDER_NONE | wxPU_CONTAINS_CONTROLS)
    , ds_(ds)
    , event_bus_(event_bus)
    , items_(items)
{
    SetBackgroundStyle(wxBG_STYLE_PAINT);

    // Calculate initial dimensions based on item count up to max visibly allowed limit.
    const int kVisibleItemCount = std::min(static_cast<int>(items_.size()), kMaxVisibleItems);
    const int kHeight = kVisibleItemCount * kItemHeight + 4; // Adding minimal padding

    int max_width = 300; // Base minimal width
    SetClientSize(wxSize(max_width, kHeight));
}

void BreadcrumbDropdown::OnDismiss()
{
    wxPopupTransientWindow::OnDismiss();
}

void BreadcrumbDropdown::OnPaint(wxPaintEvent& /*event*/)
{
    wxAutoBufferedPaintDC paint_dc(this);
    std::unique_ptr<wxGraphicsContext> gc(wxGraphicsContext::Create(paint_dc));

    if (!gc)
        return;

    const wxSize kSize = GetClientSize();
    RenderBackground(gc.get(), kSize.GetWidth(), kSize.GetHeight());
    RenderItems(paint_dc, gc.get(), kSize.GetWidth());
}

void BreadcrumbDropdown::RenderBackground(wxGraphicsContext* gc, int width, int height) const
{
    auto bg_color = ds_.theme.color(kBgColorToken);
    auto border_color = ds_.theme.color(kBorderColorToken);

    gc->SetBrush(wxBrush(bg_color));
    gc->SetPen(wxPen(border_color, 1));
    gc->DrawRoundedRectangle(0, 0, width, height, 4);
}

void BreadcrumbDropdown::RenderItems(wxPaintDC& dc, wxGraphicsContext* gc, int width)
{
    auto font = ds_.typography.font(TypeSlot::kBody);
    auto text_color = ds_.theme.color(kTextColorToken);
    auto hover_color = ds_.theme.color(kHoverColorToken);
    auto muted_color = ds_.theme.color(core::ThemeColorToken::TextMuted);

    gc->SetFont(font, text_color);

    const int kStartX = 12;
    const int kIconSize = 16;
    int current_y = 2; // initial padding map

    for (size_t i = 0; i < items_.size(); ++i)
    {
        auto& item = items_[i];
        item.bounds = wxRect(2, current_y, width - 4, kItemHeight);

        bool is_active =
            (static_cast<int>(i) == hovered_index_ || static_cast<int>(i) == focused_index_);

        if (is_active)
        {
            gc->SetBrush(wxBrush(hover_color));
            gc->SetPen(*wxTRANSPARENT_PEN);
            gc->DrawRoundedRectangle(item.bounds.GetX(),
                                     item.bounds.GetY(),
                                     item.bounds.GetWidth(),
                                     item.bounds.GetHeight(),
                                     4);
        }

        int text_x = kStartX;

        if (!item.icon_name.empty())
        {
            const int kIconY = current_y + (kItemHeight - kIconSize) / 2;
            IconManager::get().draw_icon(dc,
                                         item.icon_name,
                                         text_x,
                                         kIconY,
                                         wxSize(kIconSize, kIconSize),
                                         is_active ? text_color : muted_color);
            text_x += kIconSize + 8;
        }

        const wxString kWxText = wxString::FromUTF8(item.text);
        double text_w = 0.0;
        double text_h = 0.0;
        double descent = 0.0;
        double external_leading = 0.0;
        gc->GetTextExtent(kWxText, &text_w, &text_h, &descent, &external_leading);

        gc->SetFont(font, is_active ? text_color : muted_color);
        gc->DrawText(kWxText, text_x, current_y + (kItemHeight - text_h) / 2.0);

        current_y += kItemHeight;
    }
}

void BreadcrumbDropdown::SelectItem(int index)
{
    if (index >= 0 && index < static_cast<int>(items_.size()))
    {
        const auto& item = items_[static_cast<std::size_t>(index)];

        // Notify application
        core::events::BreadcrumbNavigateEvent nav_event;
        nav_event.root_id = item.full_path;
        event_bus_.publish(nav_event);

        Dismiss();
    }
}

void BreadcrumbDropdown::OnMouseMove(wxMouseEvent& event)
{
    const wxPoint kPos = event.GetPosition();
    const int kPrevHover = hovered_index_;
    hovered_index_ = -1;

    for (size_t i = 0; i < items_.size(); ++i)
    {
        if (items_[i].bounds.Contains(kPos))
        {
            hovered_index_ = static_cast<int>(i);
            focused_index_ = hovered_index_; // align sync focus tracking
            SetCursor(wxCursor(wxCURSOR_HAND));
            break;
        }
    }

    if (hovered_index_ == -1)
    {
        SetCursor(wxNullCursor);
    }

    if (kPrevHover != hovered_index_)
    {
        Refresh();
    }

    event.Skip();
}

void BreadcrumbDropdown::OnMouseLeave(wxMouseEvent& event)
{
    if (hovered_index_ != -1)
    {
        hovered_index_ = -1;
        SetCursor(wxNullCursor);
        Refresh();
    }
    event.Skip();
}

void BreadcrumbDropdown::OnMouseLeftDown(wxMouseEvent& event)
{
    event.Skip();
}

void BreadcrumbDropdown::OnMouseLeftUp(wxMouseEvent& event)
{
    if (hovered_index_ >= 0 && hovered_index_ < static_cast<int>(items_.size()))
    {
        SelectItem(hovered_index_);
    }
    event.Skip();
}

void BreadcrumbDropdown::OnKeyDown(wxKeyEvent& event)
{
    const int kKeyCode = event.GetKeyCode();
    const int kCount = static_cast<int>(items_.size());

    if (kCount == 0)
    {
        event.Skip();
        return;
    }

    bool handled = false;

    if (kKeyCode == WXK_UP)
    {
        focused_index_--;
        if (focused_index_ < 0)
        {
            focused_index_ = kCount - 1;
        }
        handled = true;
    }
    else if (kKeyCode == WXK_DOWN)
    {
        focused_index_++;
        if (focused_index_ >= kCount)
        {
            focused_index_ = 0;
        }
        handled = true;
    }
    else if (kKeyCode == WXK_RETURN || kKeyCode == WXK_SPACE)
    {
        SelectItem(focused_index_);
        handled = true;
    }
    else if (kKeyCode == WXK_ESCAPE)
    {
        Dismiss();
        handled = true;
    }

    if (handled)
    {
        accessibility::AccessibilityController::get().announce_focus(
            items_[static_cast<size_t>(focused_index_)].full_path, "Dropdown Item", "Selected");
        Refresh();
    }
    else
    {
        event.Skip();
    }
}

void BreadcrumbDropdown::OnSetFocus(wxFocusEvent& event)
{
    Refresh();
    event.Skip();
}

void BreadcrumbDropdown::OnKillFocus(wxFocusEvent& event)
{
    Refresh();
    event.Skip(); // Dropdown closure is implicitly handled via EVT_KILL_FOCUS on parent framework
}

} // namespace markamp::ui
