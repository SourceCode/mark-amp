#pragma once

#include <functional>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

/// An output channel that extensions can write to (equivalent to VS Code's OutputChannel).
class OutputChannel
{
public:
    explicit OutputChannel(std::string name);

    [[nodiscard]] auto name() const -> const std::string&;
    [[nodiscard]] auto content() const -> std::string;

    void append(const std::string& text);
    void append_line(const std::string& text);
    void clear();
    void show();
    void hide();

    [[nodiscard]] auto is_visible() const -> bool;

    /// Listener for content changes.
    using ContentChangeListener = std::function<void(const OutputChannel& channel)>;
    auto on_content_change(ContentChangeListener listener) -> std::size_t;
    void remove_content_listener(std::size_t listener_id);

private:
    std::string name_;
    std::string content_;
    bool visible_{false};
    std::vector<std::pair<std::size_t, ContentChangeListener>> listeners_;
    std::size_t next_listener_id_{0};

    void fire_content_change();
};

/// Service that manages all output channels (equivalent to VS Code's Output Panel backend).
class OutputChannelService
{
public:
    OutputChannelService() = default;

    /// Create a new output channel with the given name. Returns a non-owning pointer.
    auto create_channel(const std::string& channel_name) -> OutputChannel*;

    /// Get an existing channel by name. Returns nullptr if not found.
    [[nodiscard]] auto get_channel(const std::string& channel_name) -> OutputChannel*;

    /// Get all channel names.
    [[nodiscard]] auto channel_names() const -> std::vector<std::string>;

    /// Remove a channel.
    void remove_channel(const std::string& channel_name);

    /// Get the currently active (shown) channel name.
    [[nodiscard]] auto active_channel() const -> std::string;

    /// Set the active channel.
    void set_active_channel(const std::string& channel_name);

private:
    std::unordered_map<std::string, std::unique_ptr<OutputChannel>> channels_;
    std::string active_channel_;
};

// ── V9 Phase 04 Task 11: Per-plugin output channel routing ──

/// Routes plugin output to dedicated, auto-created channels.
/// Each plugin gets its own output channel named "Plugin: <plugin_id>".
class PluginOutputRouter
{
public:
    explicit PluginOutputRouter(OutputChannelService& channel_service)
        : channel_service_(channel_service)
    {
    }

    /// Append text to a plugin's dedicated output channel.
    void append(const std::string& plugin_id, const std::string& text)
    {
        auto* channel = ensure_channel(plugin_id);
        if (channel != nullptr)
        {
            channel->append(text);
        }
    }

    /// Append a line to a plugin's dedicated output channel.
    void append_line(const std::string& plugin_id, const std::string& text)
    {
        auto* channel = ensure_channel(plugin_id);
        if (channel != nullptr)
        {
            channel->append_line(text);
        }
    }

    /// Clear a plugin's output channel.
    void clear(const std::string& plugin_id)
    {
        auto* channel = channel_service_.get_channel(channel_name(plugin_id));
        if (channel != nullptr)
        {
            channel->clear();
        }
    }

    /// Show a plugin's output channel.
    void show(const std::string& plugin_id)
    {
        auto* channel = ensure_channel(plugin_id);
        if (channel != nullptr)
        {
            channel->show();
        }
    }

    /// Get the channel name for a plugin.
    [[nodiscard]] static auto channel_name(const std::string& plugin_id) -> std::string
    {
        return "Plugin: " + plugin_id;
    }

private:
    OutputChannelService& channel_service_;

    auto ensure_channel(const std::string& plugin_id) -> OutputChannel*
    {
        const std::string name = channel_name(plugin_id);
        auto* existing = channel_service_.get_channel(name);
        if (existing != nullptr)
        {
            return existing;
        }
        return channel_service_.create_channel(name);
    }
};

} // namespace markamp::core
