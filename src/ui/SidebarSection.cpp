#include "ui/SidebarSection.h"

#include "core/Config.h"
#include "core/ThemeEngine.h"
#include "ui/DesignSystemContext.h"
#include "ui/IconManager.h"
#include "ui/TypographyScale.h"
#include "ui/accessibility/AccessibilityController.h"

#include <wx/dcbuffer.h>

namespace markamp::ui
{

wxBEGIN_EVENT_TABLE(SidebarSection::SectionHeader, ThemeAwareWindow)
    EVT_PAINT(SidebarSection::SectionHeader::OnPaint)
        EVT_SIZE(SidebarSection::SectionHeader::OnSize)
            EVT_LEFT_DOWN(SidebarSection::SectionHeader::OnLeftDown)
                EVT_LEFT_UP(SidebarSection::SectionHeader::OnLeftUp)
                    EVT_MOTION(SidebarSection::SectionHeader::OnMotion)
                        EVT_LEAVE_WINDOW(SidebarSection::SectionHeader::OnLeave)
                            EVT_SET_FOCUS(SidebarSection::SectionHeader::OnSetFocus)
                                EVT_KILL_FOCUS(SidebarSection::SectionHeader::OnKillFocus)
                                    EVT_KEY_DOWN(SidebarSection::SectionHeader::OnKeyDown)
                                        wxEND_EVENT_TABLE()

                                            SidebarSection::SectionHeader::SectionHeader(
                                                SidebarSection* parent,
                                                DesignSystemContext& ds,
                                                IconManager& icon_manager,
                                                const std::string& title)
    : ThemeAwareWindow(parent,
                       ds.theme,
                       wxID_ANY,
                       wxDefaultPosition,
                       wxDefaultSize,
                       wxBORDER_NONE | wxFULL_REPAINT_ON_RESIZE)
    , section_parent_(parent)
    , ds_(ds)
    , icon_manager_(icon_manager)
    , title_(title)
{
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    UpdateMetrics();
}

void SidebarSection::SectionHeader::UpdateMetrics()
{
    // Minimal height for section header
    height_ = 22; // Hardcoded default, can map to TypeSlot::kLabel metrics
    SetMinSize(wxSize(-1, height_));
    SetMaxSize(wxSize(-1, height_));
}

void SidebarSection::SectionHeader::OnThemeChanged(const core::Theme& /*new_theme*/)
{
    UpdateMetrics();
    Refresh();
}

void SidebarSection::SectionHeader::OnSize(wxSizeEvent& event)
{
    event.Skip();
}

void SidebarSection::SectionHeader::OnPaint(wxPaintEvent& /*event*/)
{
    wxAutoBufferedPaintDC dc(this);
    auto& current_theme = ds_.theme;

    dc.SetBackground(wxBrush(current_theme.color(core::ThemeColorToken::SidebarBg)));
    dc.Clear();

    int icon_size = 16;
    int left_padding = 4;

    // Draw Chevron
    auto chevron_color = is_hovered_ ? current_theme.color(core::ThemeColorToken::SidebarFg)
                                     : current_theme.color(core::ThemeColorToken::SidebarFg);

    std::string icon_name = section_parent_->is_expanded() ? "chevron-down" : "chevron-right";

    icon_manager_.draw_icon(dc,
                            icon_name,
                            left_padding,
                            (height_ - icon_size) / 2,
                            wxSize(icon_size, icon_size),
                            chevron_color);

    // Draw string
    dc.SetFont(ds_.typography.font(TypeSlot::kBodyStrong));
    dc.SetTextForeground(chevron_color); // Matches chevron color for hover interactions

    wxSize extent = dc.GetTextExtent(title_);
    dc.DrawText(title_, left_padding + icon_size + 4, (height_ - extent.y) / 2);

    if (HasFocus())
    {
        wxSize size = GetClientSize();
        dc.SetPen(wxPen(current_theme.color(core::ThemeColorToken::FocusRingColor)));
        dc.SetBrush(*wxTRANSPARENT_BRUSH);
        dc.DrawRectangle(0, 0, size.x, height_);
    }
}

void SidebarSection::SectionHeader::OnLeftDown(wxMouseEvent& event)
{
    is_pressed_ = true;
    event.Skip();
}

void SidebarSection::SectionHeader::OnLeftUp(wxMouseEvent& event)
{
    if (is_pressed_)
    {
        is_pressed_ = false;
        section_parent_->ToggleExpanded();
        Refresh();
    }
    event.Skip();
}

void SidebarSection::SectionHeader::OnMotion(wxMouseEvent& event)
{
    if (!is_hovered_)
    {
        is_hovered_ = true;
        SetControlCursor(ControlCursorType::kHand);
        Refresh();
    }
    event.Skip();
}

void SidebarSection::SectionHeader::OnLeave(wxMouseEvent& event)
{
    if (is_hovered_)
    {
        is_hovered_ = false;
        is_pressed_ = false;
        SetControlCursor(ControlCursorType::kArrow);
        Refresh();
    }
    event.Skip();
}

void SidebarSection::SectionHeader::OnSetFocus(wxFocusEvent& event)
{
    accessibility::AccessibilityController::get().announce_focus(
        title_, "Tree Item", section_parent_->is_expanded() ? "Expanded" : "Collapsed");
    Refresh();
    event.Skip();
}

void SidebarSection::SectionHeader::OnKillFocus(wxFocusEvent& event)
{
    Refresh();
    event.Skip();
}

void SidebarSection::SectionHeader::OnKeyDown(wxKeyEvent& event)
{
    if (event.GetKeyCode() == WXK_SPACE || event.GetKeyCode() == WXK_RETURN)
    {
        section_parent_->ToggleExpanded();
        Refresh();
    }
    else
    {
        event.Skip();
    }
}

// ----------------------------------------------------------------------------
// SidebarSection Implementation
// ----------------------------------------------------------------------------

SidebarSection::SidebarSection(wxWindow* parent,
                               DesignSystemContext& ds,
                               IconManager& icon_manager,
                               core::EventBus& /*event_bus*/,
                               core::Config* config,
                               const std::string& title,
                               const std::string& persistence_id)
    : ThemeAwareWindow(parent, ds.theme, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE)
    , ds_(ds)
    , icon_manager_(icon_manager)
    , config_(config)
    , persistence_id_(persistence_id)
{
    if (config_ && !persistence_id_.empty())
    {
        is_expanded_ = config_->get_bool("sidebar.section." + persistence_id_ + ".expanded", true);
    }

    main_sizer_ = new wxBoxSizer(wxVERTICAL);
    header_ = new SectionHeader(this, ds_, icon_manager_, title);
    main_sizer_->Add(header_, 0, wxEXPAND | wxALL, 0);
    SetSizer(main_sizer_);
}

void SidebarSection::set_content(wxWindow* content_window)
{
    if (content_)
    {
        main_sizer_->Detach(content_);
        content_->Destroy();
    }

    content_ = content_window;

    if (content_)
    {
        content_->Reparent(this);
        main_sizer_->Add(content_, 1, wxEXPAND | wxALL, 0);
        content_->Show(is_expanded_);
    }

    Layout();
}

void SidebarSection::set_expanded(bool expanded)
{
    if (is_expanded_ != expanded)
    {
        ToggleExpanded();
    }
}

void SidebarSection::ToggleExpanded()
{
    is_expanded_ = !is_expanded_;

    if (config_ && !persistence_id_.empty())
    {
        config_->set("sidebar.section." + persistence_id_ + ".expanded", is_expanded_);
        (void)config_->save();
    }

    if (content_)
    {
        content_->Show(is_expanded_);

        // Ensure parent layout dynamically reflows
        if (GetParent())
        {
            GetParent()->Layout();
            GetParent()->Refresh();
        }

        if (header_)
        {
            accessibility::AccessibilityController::get().notify_state_change(
                header_->get_title(), is_expanded_ ? "Expanded" : "Collapsed");
        }
    }
}

void SidebarSection::OnThemeChanged(const core::Theme& /*new_theme*/)
{
    Refresh();
}

void SidebarSection::OnSize(wxSizeEvent& event)
{
    event.Skip();
}

} // namespace markamp::ui
