#include "ui/SidebarHeader.h"

#include "core/EventBus.h"
#include "core/Events.h"
#include "ui/BreadcrumbBar.h"
#include "ui/ComponentSizeResolver.h"
#include "ui/DesignSystemContext.h"
#include "ui/IconManager.h"
#include "ui/TypographyScale.h"
#include "ui/accessibility/AccessibilityController.h"

#include <wx/dcbuffer.h>
#include <wx/graphics.h>

namespace markamp::ui
{

wxBEGIN_EVENT_TABLE(SidebarHeader, ThemeAwareWindow) EVT_PAINT(SidebarHeader::OnPaint)
    EVT_SIZE(SidebarHeader::OnSize) EVT_LEFT_DOWN(SidebarHeader::OnMouseLeftDown)
        EVT_LEFT_UP(SidebarHeader::OnMouseLeftUp) EVT_MOTION(SidebarHeader::OnMouseMotion)
            EVT_LEAVE_WINDOW(SidebarHeader::OnMouseLeave) EVT_SET_FOCUS(SidebarHeader::OnSetFocus)
                EVT_KILL_FOCUS(SidebarHeader::OnKillFocus) wxEND_EVENT_TABLE()

                    SidebarHeader::SidebarHeader(wxWindow* parent,
                                                 DesignSystemContext& ds,
                                                 IconManager& icon_manager,
                                                 core::EventBus& event_bus)
    : ThemeAwareWindow(parent,
                       ds.theme,
                       wxID_ANY,
                       wxDefaultPosition,
                       wxDefaultSize,
                       wxBORDER_NONE | wxFULL_REPAINT_ON_RESIZE)
    , ds_(ds)
    , icon_manager_(icon_manager)
    , event_bus_(event_bus)
{
    SetBackgroundStyle(wxBG_STYLE_PAINT);

    breadcrumb_ = new BreadcrumbBar(this, ds, event_bus_);
    breadcrumb_->Hide();

    UpdateMetrics();
}

void SidebarHeader::set_title(const std::string& title)
{
    if (title_ != title)
    {
        title_ = title;
        Refresh();
    }
}

void SidebarHeader::set_display_mode(SidebarHeaderMode mode)
{
    if (mode_ != mode)
    {
        mode_ = mode;
        if (mode_ == SidebarHeaderMode::kBreadcrumb)
        {
            breadcrumb_->Show();
        }
        else
        {
            breadcrumb_->Hide();
        }

        // Post size event to update layout
        wxSizeEvent evt(GetSize(), GetId());
        wxPostEvent(this, evt);
        Refresh();
    }
}

void SidebarHeader::set_breadcrumb(const std::vector<std::string>& path)
{
    std::string full_path;
    for (const auto& segment : path)
    {
        if (!full_path.empty())
            full_path += "/";
        full_path += segment;
    }
    if (breadcrumb_ != nullptr)
    {
        breadcrumb_->SetFilePath(full_path, "");
    }
}

void SidebarHeader::UpdateMetrics()
{
    // Use PanelHeader metric for now as they are visually similar
    auto metrics = ds_.component_sizes.resolve(ComponentKind::kPanelHeader);
    height_ = metrics.height;
    SetMinSize(wxSize(-1, height_));
    SetMaxSize(wxSize(-1, height_));
}

void SidebarHeader::OnThemeChanged(const core::Theme& /*new_theme*/)
{
    UpdateMetrics();
    Refresh();
}

void SidebarHeader::OnSize(wxSizeEvent& event)
{
    // Recalculate hit rects
    wxSize size = GetClientSize();

    int icon_size = 16;
    int right_padding = 10;

    collapse_all_rect_ =
        wxRect(size.x - right_padding - icon_size, (height_ - icon_size) / 2, icon_size, icon_size);

    if (breadcrumb_ != nullptr)
    {
        breadcrumb_->SetSize(10, 0, collapse_all_rect_.x - 10, height_);
    }

    event.Skip();
}

void SidebarHeader::OnPaint(wxPaintEvent& /*event*/)
{
    wxAutoBufferedPaintDC dc(this);

    // Background
    auto& current_theme = ds_.theme;
    dc.SetBackground(wxBrush(current_theme.color(core::ThemeColorToken::SidebarBg)));
    dc.Clear();

    wxSize size = GetClientSize();

    if (mode_ == SidebarHeaderMode::kTitle)
    {
        // Title Text
        dc.SetFont(ds_.typography.font(TypeSlot::kBodyStrong));
        dc.SetTextForeground(current_theme.color(core::ThemeColorToken::SidebarFg));

        wxSize extent = dc.GetTextExtent(title_);

        int left_padding = 20; // VS Code style indentation
        dc.DrawText(title_, left_padding, (height_ - extent.y) / 2);
    }

    // Contextual Action Icons (e.g. Collapse All)
    // Only drawn when not empty
    if (!title_.empty())
    {
        // Hover background
        if (is_hovering_collapse_all_ || is_pressed_collapse_all_)
        {
            wxColour bg_color = is_pressed_collapse_all_
                                    ? current_theme.color(core::ThemeColorToken::ControlBgPressed)
                                    : current_theme.color(core::ThemeColorToken::ControlBgHover);

            dc.SetPen(*wxTRANSPARENT_PEN);
            dc.SetBrush(wxBrush(bg_color));

            // Inflate rect slightly for the hover background
            wxRect bg_rect = collapse_all_rect_;
            bg_rect.Inflate(4, 4);
            dc.DrawRoundedRectangle(bg_rect, 4.0);
        }

        auto icon_color = is_hovering_collapse_all_
                              ? current_theme.color(core::ThemeColorToken::SidebarFg)
                              : current_theme.color(core::ThemeColorToken::SidebarFg);

        icon_manager_.draw_icon(dc,
                                "action-collapse-all",
                                collapse_all_rect_.x,
                                collapse_all_rect_.y,
                                wxSize(16, 16),
                                icon_color);
    }

    if (HasFocus())
    {
        auto client_size = GetClientSize();
        dc.SetPen(wxPen(current_theme.color(core::ThemeColorToken::FocusRingColor)));
        dc.SetBrush(*wxTRANSPARENT_BRUSH);
        dc.DrawRectangle(0, 0, client_size.x, height_);
    }
}

void SidebarHeader::OnMouseLeftDown(wxMouseEvent& event)
{
    if (collapse_all_rect_.Contains(event.GetPosition()))
    {
        is_pressed_collapse_all_ = true;
        RefreshRect(collapse_all_rect_.Inflate(4));
    }
    event.Skip();
}

void SidebarHeader::OnMouseLeftUp(wxMouseEvent& event)
{
    if (is_pressed_collapse_all_)
    {
        is_pressed_collapse_all_ = false;
        if (collapse_all_rect_.Contains(event.GetPosition()))
        {
            // Improvement 94: Dispatch collapse-all command via EventBus
            core::events::CommandExecutedEvent collapse_evt;
            collapse_evt.command_id = "sidebar.collapseAll";
            collapse_evt.source = "sidebar_header";
            event_bus_.publish(collapse_evt);
        }
        RefreshRect(collapse_all_rect_.Inflate(4));
    }
    event.Skip();
}

void SidebarHeader::OnMouseMotion(wxMouseEvent& event)
{
    bool hover_collapse = collapse_all_rect_.Contains(event.GetPosition());

    if (hover_collapse != is_hovering_collapse_all_)
    {
        is_hovering_collapse_all_ = hover_collapse;
        SetControlCursor(is_hovering_collapse_all_ ? ControlCursorType::kHand
                                                   : ControlCursorType::kArrow);
        RefreshRect(collapse_all_rect_.Inflate(4));
    }

    event.Skip();
}

void SidebarHeader::OnMouseLeave(wxMouseEvent& event)
{
    if (is_hovering_collapse_all_ || is_pressed_collapse_all_)
    {
        is_hovering_collapse_all_ = false;
        is_pressed_collapse_all_ = false;
        SetControlCursor(ControlCursorType::kArrow);
        RefreshRect(collapse_all_rect_.Inflate(4));
    }
    event.Skip();
}

void SidebarHeader::OnSetFocus(wxFocusEvent& event)
{
    Refresh();
    event.Skip();
}

void SidebarHeader::OnKillFocus(wxFocusEvent& event)
{
    Refresh();
    event.Skip();
}

} // namespace markamp::ui
