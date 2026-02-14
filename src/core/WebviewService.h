#pragma once

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

/// Options for creating a webview panel.
struct WebviewOptions
{
    bool enable_scripts{false};
    bool enable_forms{false};
    bool retain_context_when_hidden{false};
    std::vector<std::string> local_resource_roots;
};

/// A webview panel that can display HTML content (mirrors VS Code's WebviewPanel).
class WebviewPanel
{
public:
    WebviewPanel(std::string view_type, std::string title, WebviewOptions options);
    virtual ~WebviewPanel() = default;

    [[nodiscard]] auto view_type() const -> const std::string&;
    [[nodiscard]] auto title() const -> const std::string&;
    [[nodiscard]] auto options() const -> const WebviewOptions&;
    [[nodiscard]] auto html() const -> const std::string&;
    [[nodiscard]] auto is_visible() const -> bool;

    void set_title(const std::string& new_title);
    void set_html(const std::string& html_content);
    void reveal();
    void dispose();

    /// Post a message to the webview.
    void post_message(const std::string& message_json);

    /// Listener for messages from the webview.
    using MessageListener = std::function<void(const std::string& message_json)>;
    auto on_did_receive_message(MessageListener listener) -> std::size_t;

    /// Listener for disposal.
    using DisposeListener = std::function<void()>;
    auto on_did_dispose(DisposeListener listener) -> std::size_t;

private:
    std::string view_type_;
    std::string title_;
    WebviewOptions options_;
    std::string html_;
    bool visible_{true};
    bool disposed_{false};

    std::vector<std::pair<std::size_t, MessageListener>> message_listeners_;
    std::vector<std::pair<std::size_t, DisposeListener>> dispose_listeners_;
    std::size_t next_listener_id_{0};
};

/// Service that manages all webview panels.
class WebviewService
{
public:
    WebviewService() = default;

    /// Create a new webview panel.
    auto create_panel(const std::string& view_type,
                      const std::string& title,
                      WebviewOptions options = {}) -> WebviewPanel*;

    /// Get a panel by view type.
    [[nodiscard]] auto get_panel(const std::string& view_type) -> WebviewPanel*;

    /// Get all active panel view types.
    [[nodiscard]] auto panel_view_types() const -> std::vector<std::string>;

    /// Remove a panel.
    void remove_panel(const std::string& view_type);

private:
    std::unordered_map<std::string, std::unique_ptr<WebviewPanel>> panels_;
};

} // namespace markamp::core
