#include "OutputPanel.h"

namespace markamp::ui
{

void OutputPanel::set_service(core::OutputChannelService* service)
{
    service_ = service;
    if (service_ != nullptr)
    {
        auto names = service_->channel_names();
        if (!names.empty() && active_channel_.empty())
        {
            active_channel_ = names.front();
        }
    }
}

auto OutputPanel::active_channel() const -> const std::string&
{
    return active_channel_;
}

void OutputPanel::set_active_channel(const std::string& channel_name)
{
    active_channel_ = channel_name;
}

auto OutputPanel::channel_names() const -> std::vector<std::string>
{
    return service_ != nullptr ? service_->channel_names() : std::vector<std::string>{};
}

auto OutputPanel::active_content() const -> std::string
{
    if (service_ == nullptr || active_channel_.empty())
    {
        return {};
    }
    auto* channel = service_->get_channel(active_channel_);
    return channel != nullptr ? channel->content() : std::string{};
}

auto OutputPanel::auto_scroll() const -> bool
{
    return auto_scroll_;
}

void OutputPanel::set_auto_scroll(bool enabled)
{
    auto_scroll_ = enabled;
}

void OutputPanel::clear_active_channel()
{
    if (service_ == nullptr || active_channel_.empty())
    {
        return;
    }
    auto* channel = service_->get_channel(active_channel_);
    if (channel != nullptr)
    {
        channel->clear();
    }
}

void OutputPanel::lock_scroll()
{
    auto_scroll_ = false;
}

void OutputPanel::unlock_scroll()
{
    auto_scroll_ = true;
}

} // namespace markamp::ui
