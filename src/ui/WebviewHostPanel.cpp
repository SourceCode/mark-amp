#include "WebviewHostPanel.h"

#include <wx/sizer.h>

namespace markamp::ui
{

// ── Data-only constructor (for tests) ──

WebviewHostPanel::WebviewHostPanel()
    : wxPanel()
{
}

// ── UI constructor ──

WebviewHostPanel::WebviewHostPanel(wxWindow* parent, core::WebviewService* service)
    : wxPanel(parent, wxID_ANY)
    , service_(service)
{
    CreateLayout();
    RefreshContent();
}

void WebviewHostPanel::CreateLayout()
{
    auto* sizer = new wxBoxSizer(wxVERTICAL);

    html_window_ = new wxHtmlWindow(
        this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHW_SCROLLBAR_AUTO | wxBORDER_NONE);
    sizer->Add(html_window_, 1, wxEXPAND);

    SetSizer(sizer);
}

void WebviewHostPanel::RefreshContent()
{
    if (html_window_ == nullptr)
    {
        return;
    }

    auto html = active_html();
    if (!html.empty())
    {
        html_window_->SetPage(wxString(html));
    }
    else
    {
        html_window_->SetPage("<html><body><p>No content</p></body></html>");
    }
}

void WebviewHostPanel::ApplyTheme(const wxColour& bg_colour, const wxColour& /*fg_colour*/)
{
    SetBackgroundColour(bg_colour);
    Refresh();
}

// ── Data-layer API (unchanged for test compatibility) ──

void WebviewHostPanel::set_service(core::WebviewService* service)
{
    service_ = service;
}

void WebviewHostPanel::set_active_panel(const std::string& view_type)
{
    active_panel_type_ = view_type;
}

auto WebviewHostPanel::active_panel_type() const -> const std::string&
{
    return active_panel_type_;
}

auto WebviewHostPanel::active_html() -> std::string
{
    if (service_ == nullptr || active_panel_type_.empty())
    {
        return {};
    }
    auto* panel = service_->get_panel(active_panel_type_);
    if (panel == nullptr)
    {
        return {};
    }
    return panel->html();
}

void WebviewHostPanel::post_message(const std::string& message_json)
{
    if (service_ == nullptr || active_panel_type_.empty())
    {
        return;
    }
    auto* panel = service_->get_panel(active_panel_type_);
    if (panel != nullptr)
    {
        panel->post_message(message_json);
    }
}

auto WebviewHostPanel::scripts_enabled() const -> bool
{
    if (service_ == nullptr || active_panel_type_.empty())
    {
        return false;
    }
    auto* mutable_service = const_cast<core::WebviewService*>(service_);
    auto* panel = mutable_service->get_panel(active_panel_type_);
    return panel != nullptr && panel->options().enable_scripts;
}

} // namespace markamp::ui
