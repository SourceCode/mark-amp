#include "QuickPickService.h"

#include "EventBus.h"
#include "Events.h"

namespace markamp::core
{

void QuickPickService::show(const std::vector<QuickPickItem>& items,
                            const QuickPickOptions& options,
                            SingleResultCallback on_result)
{
    current_items_ = items;
    current_options_ = options;
    single_callback_ = std::move(on_result);
    multi_callback_ = nullptr;
    visible_ = true;

    // Publish UI request event so LayoutManager can show a dialog
    if (event_bus_ != nullptr)
    {
        events::ShowQuickPickRequestEvent evt;
        evt.title = options.title;
        evt.placeholder = options.placeholder;
        evt.can_pick_many = false;
        event_bus_->publish(evt);
    }
}

void QuickPickService::show_many(const std::vector<QuickPickItem>& items,
                                 const QuickPickOptions& options,
                                 MultiResultCallback on_result)
{
    current_items_ = items;
    current_options_ = options;
    multi_callback_ = std::move(on_result);
    single_callback_ = nullptr;
    visible_ = true;

    if (event_bus_ != nullptr)
    {
        events::ShowQuickPickRequestEvent evt;
        evt.title = options.title;
        evt.placeholder = options.placeholder;
        evt.can_pick_many = true;
        event_bus_->publish(evt);
    }
}

void QuickPickService::test_select(std::size_t index)
{
    if (!visible_ || index >= current_items_.size())
    {
        return;
    }
    visible_ = false;
    if (single_callback_)
    {
        auto callback = std::move(single_callback_);
        single_callback_ = nullptr;
        callback(current_items_[index]);
    }
}

void QuickPickService::test_select_many(const std::vector<std::size_t>& indices)
{
    if (!visible_)
    {
        return;
    }
    visible_ = false;
    if (multi_callback_)
    {
        std::vector<QuickPickItem> selected;
        for (auto idx : indices)
        {
            if (idx < current_items_.size())
            {
                selected.push_back(current_items_[idx]);
            }
        }
        auto callback = std::move(multi_callback_);
        multi_callback_ = nullptr;
        callback(selected);
    }
}

void QuickPickService::test_cancel()
{
    if (!visible_)
    {
        return;
    }
    visible_ = false;
    if (single_callback_)
    {
        auto callback = std::move(single_callback_);
        single_callback_ = nullptr;
        callback(std::nullopt);
    }
    if (multi_callback_)
    {
        auto callback = std::move(multi_callback_);
        multi_callback_ = nullptr;
        callback({});
    }
}

void QuickPickService::set_event_bus(EventBus* bus)
{
    event_bus_ = bus;
}

} // namespace markamp::core
