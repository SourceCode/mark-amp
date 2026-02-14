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

} // namespace markamp::core
