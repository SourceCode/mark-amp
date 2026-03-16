#include "InputBoxService.h"

#include "EventBus.h"
#include "Events.h"

namespace markamp::core
{

void InputBoxService::show(const InputBoxOptions& options, ResultCallback on_result)
{
    current_options_ = options;
    current_callback_ = std::move(on_result);
    visible_ = true;

    // Publish UI request event so LayoutManager/MainFrame can show a dialog
    if (event_bus_ != nullptr)
    {
        events::ShowInputBoxRequestEvent evt;
        evt.title = options.title;
        evt.prompt = options.prompt;
        evt.value = options.value;
        evt.placeholder = options.placeholder;
        evt.password = options.password;
        event_bus_->publish(evt);
    }
}

void InputBoxService::test_accept(const std::string& value)
{
    if (visible_ && current_callback_)
    {
        visible_ = false;
        auto callback = std::move(current_callback_);
        current_callback_ = nullptr;
        callback(value);
    }
}

void InputBoxService::test_cancel()
{
    if (visible_ && current_callback_)
    {
        visible_ = false;
        auto callback = std::move(current_callback_);
        current_callback_ = nullptr;
        callback(std::nullopt);
    }
}

void InputBoxService::set_event_bus(EventBus* bus)
{
    event_bus_ = bus;
}

// ── Phase 36: InputHistory ──

void InputHistory::add(const std::string& prompt_type, const std::string& value)
{
    auto& hist = history_[prompt_type];
    hist.push_back(value);
    while (static_cast<int>(hist.size()) > kMaxPerType)
    {
        hist.erase(hist.begin());
    }
}

auto InputHistory::get_previous(const std::string& prompt_type, int offset) const -> std::string
{
    const auto iter = history_.find(prompt_type);
    if (iter == history_.end() || iter->second.empty())
    {
        return "";
    }
    const auto& hist = iter->second;
    const int idx = static_cast<int>(hist.size()) - 1 - offset;
    if (idx < 0 || idx >= static_cast<int>(hist.size()))
    {
        return "";
    }
    return hist[static_cast<std::size_t>(idx)];
}

auto InputHistory::get_all(const std::string& prompt_type) const -> std::vector<std::string>
{
    const auto iter = history_.find(prompt_type);
    if (iter == history_.end())
    {
        return {};
    }
    return iter->second;
}

void InputHistory::clear(const std::string& prompt_type)
{
    history_.erase(prompt_type);
}

void InputHistory::clear_all()
{
    history_.clear();
}

// (#86) Return the number of prompt types with stored history.
auto InputHistory::history_count() const -> std::size_t
{
    return history_.size();
}

} // namespace markamp::core
