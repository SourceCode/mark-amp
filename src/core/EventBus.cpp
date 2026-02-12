#include "EventBus.h"

namespace markamp::core
{

Subscription::Subscription(std::function<void()> unsubscribe_fn)
    : unsubscribe_fn_(std::move(unsubscribe_fn))
{
}

Subscription::~Subscription()
{
    cancel();
}

Subscription::Subscription(Subscription&& other) noexcept
    : unsubscribe_fn_(std::move(other.unsubscribe_fn_))
{
    other.unsubscribe_fn_ = nullptr;
}

auto Subscription::operator=(Subscription&& other) noexcept -> Subscription&
{
    if (this != &other)
    {
        cancel();
        unsubscribe_fn_ = std::move(other.unsubscribe_fn_);
        other.unsubscribe_fn_ = nullptr;
    }
    return *this;
}

void Subscription::cancel()
{
    if (unsubscribe_fn_)
    {
        unsubscribe_fn_();
        unsubscribe_fn_ = nullptr;
    }
}

void EventBus::process_queued()
{
    std::vector<std::function<void()>> events_to_process;
    {
        std::lock_guard lock(mutex_);
        events_to_process.swap(queued_events_);
    }
    for (const auto& func : events_to_process)
    {
        func();
    }
}

void EventBus::drain_fast_queue()
{
    std::function<void()> func;
    while (fast_queue_.try_pop(func))
    {
        func();
    }
}

} // namespace markamp::core
