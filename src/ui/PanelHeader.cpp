#include "ui/PanelHeader.h"

#include "core/EventBus.h"
#include "core/Events.h"
#include "ui/BreadcrumbBar.h"
#include "ui/ComponentSizeResolver.h"
#include "ui/DesignSystemContext.h"
#include "ui/IconManager.h"
#include "ui/TypographyScale.h"
#include "ui/accessibility/AccessibilityController.h"

#include <wx/dataobj.h>
#include <wx/dcbuffer.h>
#include <wx/dnd.h>
#include <wx/sizer.h>

namespace markamp::ui
{

wxBEGIN_EVENT_TABLE(PanelHeader, ThemeAwareWindow) EVT_PAINT(PanelHeader::OnPaint)
    EVT_SIZE(PanelHeader::OnSize) EVT_LEFT_DOWN(PanelHeader::OnMouseLeftDown)
        EVT_LEFT_UP(PanelHeader::OnMouseLeftUp) EVT_RIGHT_UP(PanelHeader::OnMouseRightUp)
            EVT_MOTION(PanelHeader::OnMouseMotion) EVT_LEAVE_WINDOW(PanelHeader::OnMouseLeave)
                EVT_SET_FOCUS(PanelHeader::OnSetFocus) EVT_KILL_FOCUS(PanelHeader::OnKillFocus)
                    wxEND_EVENT_TABLE()

                        PanelHeader::PanelHeader(wxWindow* parent,
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

    breadcrumb_ = new BreadcrumbBar(this, ds);
    breadcrumb_->Hide();

    breadcrumb_sub_ = event_bus_.subscribe<core::events::SidebarBreadcrumbUpdateEvent>(
        [this](const core::events::SidebarBreadcrumbUpdateEvent& evt)
        {
            if (!panel_id_.empty() && evt.panel_id == panel_id_)
            {
                set_breadcrumb(evt.breadcrumb_path);
            }
        });

    main_sizer_ = new wxBoxSizer(wxVERTICAL);
    main_sizer_->AddSpacer(header_height_);
    SetSizer(main_sizer_);
    UpdateMetrics();
}

void PanelHeader::set_title(const std::string& title)
{
    if (title_ != title)
    {
        title_ = title;
        Refresh();
    }
}

void PanelHeader::set_panel_id(const std::string& panel_id)
{
    panel_id_ = panel_id;
}

void PanelHeader::set_display_mode(PanelHeaderMode mode)
{
    if (mode_ != mode)
    {
        mode_ = mode;
        if (mode_ == PanelHeaderMode::kBreadcrumb)
        {
            breadcrumb_->Show();
        }
        else
        {
            breadcrumb_->Hide();
        }

        wxSizeEvent evt(GetSize(), GetId());
        wxPostEvent(this, evt);
        Refresh();
    }
}

void PanelHeader::set_breadcrumb(const std::vector<std::string>& path)
{
    breadcrumb_->SetFilePath(path);
}

void PanelHeader::set_actions(const std::vector<ActionIcon>& actions)
{
    actions_ = actions;
    LayoutActions();
    Refresh();
}

void PanelHeader::set_toolbar(wxWindow* toolbar)
{
    if (toolbar_)
    {
        main_sizer_->Detach(toolbar_);
        toolbar_->Hide();
    }
    toolbar_ = toolbar;
    if (toolbar_)
    {
        toolbar_->Reparent(this);
        // We add no top padding since the spacer is already above it,
        // but we can add side/bottom padding.
        main_sizer_->Add(toolbar_, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 8);
        toolbar_->Show();
    }
    UpdateMetrics();
    Refresh();
}

void PanelHeader::UpdateMetrics()
{
    auto metrics = ds_.component_sizes.resolve(ComponentKind::kPanelHeader);
    header_height_ = metrics.height;

    if (main_sizer_ && main_sizer_->GetItemCount() > 0)
    {
        main_sizer_->GetItem(size_t(0))->SetMinSize(wxSize(-1, header_height_));
    }

    if (toolbar_)
    {
        main_sizer_->Layout();
        SetMinSize(wxSize(-1, main_sizer_->GetMinSize().y));
        SetMaxSize(wxSize(-1, main_sizer_->GetMinSize().y));
    }
    else
    {
        SetMinSize(wxSize(-1, header_height_));
        SetMaxSize(wxSize(-1, header_height_));
    }
    LayoutActions();
}

void PanelHeader::LayoutActions()
{
    action_rects_.clear();

    if (actions_.empty())
        return;

    wxSize size = GetClientSize();
    int icon_size = 16;
    int padding = 4;
    int x_offset = size.x - padding - icon_size; // Start from right

    for (auto it = actions_.rbegin(); it != actions_.rend(); ++it)
    {
        ActionRect ar;
        ar.action = *it;
        ar.rect = wxRect(x_offset, (header_height_ - icon_size) / 2, icon_size, icon_size);
        action_rects_.push_back(ar);

        x_offset -= (icon_size + padding);
    }
}

void PanelHeader::OnThemeChanged(const core::Theme& /*new_theme*/)
{
    UpdateMetrics();
    Refresh();
}

void PanelHeader::OnSize(wxSizeEvent& event)
{
    LayoutActions();

    if (breadcrumb_ != nullptr)
    {
        int right_margin = 0;
        if (!action_rects_.empty())
        {
            right_margin = GetClientSize().x - action_rects_.back().rect.x + 8;
        }
        breadcrumb_->SetSize(10, 0, GetClientSize().x - 10 - right_margin, header_height_);
    }

    event.Skip();
}

void PanelHeader::OnPaint(wxPaintEvent& /*event*/)
{
    wxAutoBufferedPaintDC dc(this);

    auto& current_theme = ds_.theme;
    dc.SetBackground(wxBrush(current_theme.color(core::ThemeColorToken::SidebarBg)));
    dc.Clear();

    if (mode_ == PanelHeaderMode::kTitle)
    {
        // Title Text - Uppercase, strong
        dc.SetFont(ds_.typography.font(TypeSlot::kBodyStrong));
        dc.SetTextForeground(current_theme.color(core::ThemeColorToken::SidebarFg));

        wxSize extent = dc.GetTextExtent(title_);

        int left_padding = 10;
        dc.DrawText(title_, left_padding, (header_height_ - extent.y) / 2);
    }

    // Actions
    for (const auto& ar : action_rects_)
    {
        if (ar.is_hovered || ar.is_pressed)
        {
            wxColour bg_color = ar.is_pressed
                                    ? current_theme.color(core::ThemeColorToken::ControlBgPressed)
                                    : current_theme.color(core::ThemeColorToken::ControlBgHover);

            dc.SetPen(*wxTRANSPARENT_PEN);
            dc.SetBrush(wxBrush(bg_color));

            wxRect bg_rect = ar.rect;
            bg_rect.Inflate(4, 4);
            dc.DrawRoundedRectangle(bg_rect, 4.0);
        }

        auto icon_color = ar.is_hovered ? current_theme.color(core::ThemeColorToken::SidebarFg)
                                        : current_theme.color(core::ThemeColorToken::SidebarFg);

        icon_manager_.draw_icon(
            dc, ar.action.icon_name, ar.rect.x, ar.rect.y, wxSize(16, 16), icon_color);
    }

    if (HasFocus())
    {
        dc.SetPen(wxPen(current_theme.color(core::ThemeColorToken::FocusRingColor)));
        dc.SetBrush(*wxTRANSPARENT_BRUSH);
        dc.DrawRectangle(0, 0, GetClientSize().x, header_height_);
    }
}

void PanelHeader::OnMouseLeftDown(wxMouseEvent& event)
{
    drag_start_pos_ = event.GetPosition();
    is_dragging_ = false;

    for (auto& ar : action_rects_)
    {
        if (ar.rect.Contains(event.GetPosition()))
        {
            ar.is_pressed = true;
            RefreshRect(ar.rect.Inflate(4));
            break;
        }
    }
    event.Skip();
}

void PanelHeader::OnMouseLeftUp(wxMouseEvent& event)
{
    for (auto& ar : action_rects_)
    {
        if (ar.is_pressed)
        {
            ar.is_pressed = false;
            if (ar.rect.Contains(event.GetPosition()))
            {
                core::events::PanelHeaderActionEvent action_event;
                action_event.action_id = ar.action.id;
                event_bus_.publish(action_event);
            }
            RefreshRect(ar.rect.Inflate(4));
        }
    }
    event.Skip();
}

void PanelHeader::OnMouseRightUp(wxMouseEvent& event)
{
    wxPoint pt = ClientToScreen(event.GetPosition());
    core::events::PanelContextMenuEvent ctx_event;
    ctx_event.panel_title = title_;
    ctx_event.screen_x = pt.x;
    ctx_event.screen_y = pt.y;
    event_bus_.publish(ctx_event);
    event.Skip();
}

void PanelHeader::OnMouseMotion(wxMouseEvent& event)
{
    if (event.Dragging() && event.LeftIsDown() && drag_start_pos_ != wxDefaultPosition &&
        !is_dragging_)
    {
        wxPoint current_pos = event.GetPosition();
        int dx = std::abs(current_pos.x - drag_start_pos_.x);
        int dy = std::abs(current_pos.y - drag_start_pos_.y);

        if (dx > 3 || dy > 3)
        {
            is_dragging_ = true;

            // Clear any hover states before dragging
            for (auto& ar : action_rects_)
            {
                ar.is_hovered = false;
                ar.is_pressed = false;
            }
            Refresh();
            Update();

            // R20 Fix 21: Use wxTextDataObject for easier cross-panel drop target
            std::string payload = "MARKAMP_PANEL:" + panel_id_;
            wxTextDataObject drag_data(payload);

            wxDropSource drag_source(this);
            drag_source.SetData(drag_data);
            drag_source.DoDragDrop(wxDrag_CopyOnly);

            drag_start_pos_ = wxDefaultPosition;
            is_dragging_ = false;
            return;
        }
    }

    bool any_hovered = false;
    bool requires_refresh = false;

    for (auto& ar : action_rects_)
    {
        bool is_currently_hovered = ar.rect.Contains(event.GetPosition());
        if (ar.is_hovered != is_currently_hovered)
        {
            ar.is_hovered = is_currently_hovered;
            if (ar.is_hovered)
                any_hovered = true;

            requires_refresh = true;
            RefreshRect(ar.rect.Inflate(4));
        }
    }

    if (requires_refresh)
    {
        SetControlCursor(any_hovered ? ControlCursorType::kHand : ControlCursorType::kArrow);
    }

    event.Skip();
}

void PanelHeader::OnMouseLeave(wxMouseEvent& event)
{
    bool requires_refresh = false;
    for (auto& ar : action_rects_)
    {
        if (ar.is_hovered || ar.is_pressed)
        {
            ar.is_hovered = false;
            ar.is_pressed = false;
            requires_refresh = true;
            RefreshRect(ar.rect.Inflate(4));
        }
    }

    if (requires_refresh)
    {
        SetControlCursor(ControlCursorType::kArrow);
    }
    event.Skip();
}

void PanelHeader::OnSetFocus(wxFocusEvent& event)
{
    Refresh();
    event.Skip();
}

void PanelHeader::OnKillFocus(wxFocusEvent& event)
{
    Refresh();
    event.Skip();
}

} // namespace markamp::ui
