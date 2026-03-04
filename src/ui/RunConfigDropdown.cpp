#include "RunConfigDropdown.h"

#include <wx/dcbuffer.h>
#include <wx/graphics.h>
#include <wx/menu.h>

namespace markamp::ui
{

RunConfigDropdown::RunConfigDropdown(wxWindow* parent,
                                     core::ThemeEngine& theme_engine,
                                     core::RunConfigService& config_service)
    : ThemeAwareWindow(parent,
                       theme_engine,
                       wxID_ANY,
                       wxDefaultPosition,
                       wxSize(kDropdownWidth, kDropdownHeight))
    , config_service_(config_service)
{
    SetMinSize(wxSize(kDropdownWidth, kDropdownHeight));

    Bind(wxEVT_PAINT, &RunConfigDropdown::OnPaint, this);
    Bind(wxEVT_LEFT_DOWN, &RunConfigDropdown::OnMouseDown, this);
    Bind(wxEVT_MOTION, &RunConfigDropdown::OnMouseMove, this);
    Bind(wxEVT_LEAVE_WINDOW, &RunConfigDropdown::OnMouseLeave, this);

    // Set initial selection from service
    if (const auto* active = config_service_.active_configuration())
    {
        selected_name_ = active->name;
    }
    else if (!config_service_.configurations().empty())
    {
        selected_name_ = config_service_.configurations().front().name;
    }
}

auto RunConfigDropdown::selected_name() const -> const std::string&
{
    return selected_name_;
}

void RunConfigDropdown::set_on_selection(SelectionCallback callback)
{
    on_selection_ = std::move(callback);
}

void RunConfigDropdown::set_on_edit(EditCallback callback)
{
    on_edit_ = std::move(callback);
}

void RunConfigDropdown::OnThemeChanged(const core::Theme& /*new_theme*/)
{
    Refresh();
}

void RunConfigDropdown::OnPaint(wxPaintEvent& /*event*/)
{
    wxBufferedPaintDC paint_dc(this);
    auto graphics_ctx = std::unique_ptr<wxGraphicsContext>(wxGraphicsContext::Create(paint_dc));
    if (!graphics_ctx)
    {
        return;
    }

    const auto& colors = this->theme().colors;
    auto size = GetClientSize();

    // Background
    graphics_ctx->SetBrush(wxBrush(wxColour(colors.bg_input.to_rgba_string())));
    graphics_ctx->SetPen(wxPen(wxColour(colors.border_light.to_rgba_string())));
    graphics_ctx->DrawRoundedRectangle(0, 0, size.GetWidth(), size.GetHeight(), 4);

    // Label text
    wxFont label_font(9, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
    graphics_ctx->SetFont(label_font, wxColour(colors.text_muted.to_rgba_string()));

    const std::string display = selected_name_.empty() ? "No Configuration" : selected_name_;
    graphics_ctx->DrawText(display, 8, 4);

    // Dropdown chevron
    constexpr double kChevronSize = 6.0;
    const double chevron_x = size.GetWidth() - 16.0;
    const double chevron_y = (size.GetHeight() - kChevronSize) / 2.0;

    auto path = graphics_ctx->CreatePath();
    path.MoveToPoint(chevron_x, chevron_y);
    path.AddLineToPoint(chevron_x + kChevronSize, chevron_y);
    path.AddLineToPoint(chevron_x + kChevronSize / 2.0, chevron_y + kChevronSize);
    path.CloseSubpath();

    graphics_ctx->SetBrush(wxBrush(wxColour(colors.text_muted.to_rgba_string())));
    graphics_ctx->SetPen(*wxTRANSPARENT_PEN);
    graphics_ctx->FillPath(path);
}

void RunConfigDropdown::OnMouseDown(wxMouseEvent& /*event*/)
{
    ShowDropdownPopup();
}

void RunConfigDropdown::OnMouseMove(wxMouseEvent& /*event*/)
{
    // Hover state handled in popup
}

void RunConfigDropdown::OnMouseLeave(wxMouseEvent& /*event*/)
{
    // Reset hover state
}

void RunConfigDropdown::ShowDropdownPopup()
{
    wxMenu menu;

    // Recent configurations first
    auto recent = config_service_.recent_configurations();
    if (!recent.empty())
    {
        for (const auto& name : recent)
        {
            const int menu_id = static_cast<int>(menu.GetMenuItemCount()) + 1;
            auto* menu_item = menu.AppendCheckItem(menu_id, name);
            if (name == selected_name_)
            {
                menu_item->Check(true);
            }
            menu.Bind(
                wxEVT_MENU,
                [this, name](wxCommandEvent&)
                {
                    selected_name_ = name;
                    config_service_.set_active(name);
                    if (on_selection_)
                    {
                        on_selection_(name);
                    }
                    Refresh();
                },
                menu_id);
        }
        menu.AppendSeparator();
    }

    // All configurations
    for (const auto& cfg : config_service_.configurations())
    {
        // Skip if already in recent
        bool in_recent = false;
        for (const auto& rec : recent)
        {
            if (rec == cfg.name)
            {
                in_recent = true;
                break;
            }
        }
        if (in_recent)
        {
            continue;
        }

        const int menu_id = static_cast<int>(menu.GetMenuItemCount()) + 1;
        menu.Append(menu_id, cfg.name);
        menu.Bind(
            wxEVT_MENU,
            [this, name = cfg.name](wxCommandEvent&)
            {
                selected_name_ = name;
                config_service_.set_active(name);
                if (on_selection_)
                {
                    on_selection_(name);
                }
                Refresh();
            },
            menu_id);
    }

    // Edit Configurations...
    menu.AppendSeparator();
    const int edit_id = static_cast<int>(menu.GetMenuItemCount()) + 1;
    menu.Append(edit_id, "Edit Configurations...");
    menu.Bind(
        wxEVT_MENU,
        [this](wxCommandEvent&)
        {
            if (on_edit_)
            {
                on_edit_();
            }
        },
        edit_id);

    PopupMenu(&menu, 0, GetClientSize().GetHeight());
}

void RunConfigDropdown::CloseDropdownPopup()
{
    is_open_ = false;
}

} // namespace markamp::ui
