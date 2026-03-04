#include "OutputChannelService.h"

#include <algorithm>
#include <array>
#include <utility>

namespace markamp::core
{

// ── OutputChannel ──

OutputChannel::OutputChannel(std::string name)
    : name_(std::move(name))
{
}

auto OutputChannel::name() const -> const std::string&
{
    return name_;
}

auto OutputChannel::content() const -> std::string
{
    return content_;
}

void OutputChannel::append(const std::string& text)
{
    content_ += text;
    fire_content_change();
}

void OutputChannel::append_line(const std::string& text)
{
    content_ += text + "\n";
    lines_.push_back(OutputLine{
        .text = text,
        .ansi_text = text,
        .level = LogLevel::kInfo,
        .timestamp = std::chrono::system_clock::now(),
    });
    fire_content_change();
}

void OutputChannel::append_line(const std::string& text, LogLevel level)
{
    content_ += text + "\n";
    lines_.push_back(OutputLine{
        .text = text,
        .ansi_text = text,
        .level = level,
        .timestamp = std::chrono::system_clock::now(),
    });
    fire_content_change();
}

void OutputChannel::clear()
{
    content_.clear();
    lines_.clear();
    read_index_ = 0;
    fire_content_change();
}

void OutputChannel::show()
{
    visible_ = true;
}

void OutputChannel::hide()
{
    visible_ = false;
}

auto OutputChannel::is_visible() const -> bool
{
    return visible_;
}

auto OutputChannel::lines() const -> const std::vector<OutputLine>&
{
    return lines_;
}

auto OutputChannel::lines_filtered(LogLevel min_level) const -> std::vector<OutputLine>
{
    std::vector<OutputLine> result;
    result.reserve(lines_.size());
    for (const auto& line : lines_)
    {
        if (line.level >= min_level)
        {
            result.push_back(line);
        }
    }
    return result;
}

auto OutputChannel::line_count() const -> std::size_t
{
    return lines_.size();
}

auto OutputChannel::auto_reveal() const -> bool
{
    return auto_reveal_;
}

void OutputChannel::set_auto_reveal(bool enabled)
{
    auto_reveal_ = enabled;
}

auto OutputChannel::unread_count() const -> std::size_t
{
    if (lines_.size() > read_index_)
    {
        return lines_.size() - read_index_;
    }
    return 0;
}

void OutputChannel::mark_read()
{
    read_index_ = lines_.size();
}

auto OutputChannel::on_content_change(ContentChangeListener listener) -> std::size_t
{
    auto listener_id = next_listener_id_++;
    listeners_.emplace_back(listener_id, std::move(listener));
    return listener_id;
}

void OutputChannel::remove_content_listener(std::size_t listener_id)
{
    listeners_.erase(std::remove_if(listeners_.begin(),
                                    listeners_.end(),
                                    [listener_id](const auto& pair)
                                    { return pair.first == listener_id; }),
                     listeners_.end());
}

void OutputChannel::fire_content_change()
{
    for (const auto& [id, listener] : listeners_)
    {
        listener(*this);
    }
}

// ── OutputChannelService ──

auto OutputChannelService::create_channel(const std::string& channel_name) -> OutputChannel*
{
    auto [inserted_it, inserted] =
        channels_.emplace(channel_name, std::make_unique<OutputChannel>(channel_name));
    return inserted_it->second.get();
}

auto OutputChannelService::get_channel(const std::string& channel_name) -> OutputChannel*
{
    auto found = channels_.find(channel_name);
    return found != channels_.end() ? found->second.get() : nullptr;
}

auto OutputChannelService::channel_names() const -> std::vector<std::string>
{
    std::vector<std::string> names;
    names.reserve(channels_.size());
    for (const auto& [channel_name, channel] : channels_)
    {
        names.push_back(channel_name);
    }
    return names;
}

void OutputChannelService::remove_channel(const std::string& channel_name)
{
    channels_.erase(channel_name);
}

auto OutputChannelService::active_channel() const -> std::string
{
    return active_channel_;
}

void OutputChannelService::set_active_channel(const std::string& channel_name)
{
    active_channel_ = channel_name;
}

void OutputChannelService::create_default_channels()
{
    static constexpr std::array<const char*, 5> kDefaultChannels = {
        "Build",
        "Git",
        "Tasks",
        "Extension Host",
        "Log",
    };
    for (const auto* channel_name : kDefaultChannels)
    {
        if (get_channel(channel_name) == nullptr)
        {
            create_channel(channel_name);
        }
    }
    if (active_channel_.empty())
    {
        active_channel_ = "Log";
    }
}

auto OutputChannelService::ensure_channel(const std::string& channel_name) -> OutputChannel*
{
    auto* existing = get_channel(channel_name);
    if (existing != nullptr)
    {
        return existing;
    }
    return create_channel(channel_name);
}

} // namespace markamp::core
