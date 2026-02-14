#include "OutputChannelService.h"

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
    fire_content_change();
}

void OutputChannel::clear()
{
    content_.clear();
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
    for (const auto& [name, channel] : channels_)
    {
        names.push_back(name);
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

} // namespace markamp::core
