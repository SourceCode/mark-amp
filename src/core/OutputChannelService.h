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

} // namespace markamp::core
