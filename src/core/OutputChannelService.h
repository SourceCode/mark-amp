#pragma once

#include "LogLevel.h"

#include <chrono>
#include <functional>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

// ── Phase 22: Per-line output storage ──

/// A single line of output with log level, ANSI text, and timestamp.
struct OutputLine
{
    std::string text;      ///< Plain text (ANSI stripped)
    std::string ansi_text; ///< Original text with ANSI sequences preserved
    LogLevel level{LogLevel::kInfo};
    std::chrono::system_clock::time_point timestamp{std::chrono::system_clock::now()};
};

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

    // ── Phase 22: Per-line log level support ──

    /// Append a line with an explicit log level.
    void append_line(const std::string& text, LogLevel level);

    /// Get all stored lines.
    [[nodiscard]] auto lines() const -> const std::vector<OutputLine>&;

    /// Get lines filtered by minimum severity.
    [[nodiscard]] auto lines_filtered(LogLevel min_level) const -> std::vector<OutputLine>;

    /// Total number of stored lines.
    [[nodiscard]] auto line_count() const -> std::size_t;

    /// Auto-reveal: when true, panel should reveal when new content arrives.
    [[nodiscard]] auto auto_reveal() const -> bool;
    void set_auto_reveal(bool enabled);

    /// Unread line count (lines added since last read).
    [[nodiscard]] auto unread_count() const -> std::size_t;
    void mark_read();

    /// Listener for content changes.
    using ContentChangeListener = std::function<void(const OutputChannel& channel)>;
    auto on_content_change(ContentChangeListener listener) -> std::size_t;
    void remove_content_listener(std::size_t listener_id);

private:
    std::string name_;
    std::string content_;
    bool visible_{false};
    bool auto_reveal_{true};
    std::vector<OutputLine> lines_;
    std::size_t read_index_{0};
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

    // ── Phase 22: Default system channels ──

    /// Create default channels: Build, Git, Tasks, Extension Host, Log.
    void create_default_channels();

    /// Get or create a channel (convenience).
    auto ensure_channel(const std::string& channel_name) -> OutputChannel*;

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
