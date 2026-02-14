#include "WebviewHostPanel.h"

namespace markamp::ui
{

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

auto WebviewHostPanel::active_html() const -> std::string
{
    if (service_ == nullptr || active_panel_type_.empty())
    {
        return {};
    }
    auto* panel = service_->get_panel(active_panel_type_);
    return panel != nullptr ? panel->html() : std::string{};
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
    auto* panel = service_->get_panel(active_panel_type_);
    return panel != nullptr && panel->options().enable_scripts;
}

} // namespace markamp::ui
