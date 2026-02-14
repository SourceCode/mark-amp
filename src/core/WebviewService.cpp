#include "WebviewService.h"

#include <utility>

namespace markamp::core
{

// ── WebviewPanel ──

WebviewPanel::WebviewPanel(std::string view_type, std::string title, WebviewOptions options)
    : view_type_(std::move(view_type))
    , title_(std::move(title))
    , options_(std::move(options))
{
}

auto WebviewPanel::view_type() const -> const std::string&
{
    return view_type_;
}

auto WebviewPanel::title() const -> const std::string&
{
    return title_;
}

auto WebviewPanel::options() const -> const WebviewOptions&
{
    return options_;
}

auto WebviewPanel::html() const -> const std::string&
{
    return html_;
}

auto WebviewPanel::is_visible() const -> bool
{
    return visible_ && !disposed_;
}

void WebviewPanel::set_title(const std::string& new_title)
{
    title_ = new_title;
}

void WebviewPanel::set_html(const std::string& html_content)
{
    html_ = html_content;
}

void WebviewPanel::reveal()
{
    visible_ = true;
}

void WebviewPanel::dispose()
{
    disposed_ = true;
    visible_ = false;
    for (const auto& [id, listener] : dispose_listeners_)
    {
        listener();
    }
}

void WebviewPanel::post_message(const std::string& message_json)
{
    for (const auto& [id, listener] : message_listeners_)
    {
        listener(message_json);
    }
}

auto WebviewPanel::on_did_receive_message(MessageListener listener) -> std::size_t
{
    auto listener_id = next_listener_id_++;
    message_listeners_.emplace_back(listener_id, std::move(listener));
    return listener_id;
}

auto WebviewPanel::on_did_dispose(DisposeListener listener) -> std::size_t
{
    auto listener_id = next_listener_id_++;
    dispose_listeners_.emplace_back(listener_id, std::move(listener));
    return listener_id;
}

// ── WebviewService ──

auto WebviewService::create_panel(const std::string& view_type,
                                  const std::string& title,
                                  WebviewOptions options) -> WebviewPanel*
{
    auto panel = std::make_unique<WebviewPanel>(view_type, title, std::move(options));
    auto* raw_ptr = panel.get();
    panels_[view_type] = std::move(panel);
    return raw_ptr;
}

auto WebviewService::get_panel(const std::string& view_type) -> WebviewPanel*
{
    auto found = panels_.find(view_type);
    return found != panels_.end() ? found->second.get() : nullptr;
}

auto WebviewService::panel_view_types() const -> std::vector<std::string>
{
    std::vector<std::string> types;
    types.reserve(panels_.size());
    for (const auto& [vt, panel] : panels_)
    {
        types.push_back(vt);
    }
    return types;
}

void WebviewService::remove_panel(const std::string& view_type)
{
    panels_.erase(view_type);
}

} // namespace markamp::core
