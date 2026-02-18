#include "EventBus.h"

#include <chrono>

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

// --- EventBus destructor: subscription leak detection (Phase 01 Task 5) ---

EventBus::~EventBus()
{
    const std::lock_guard lock(mutex_);
    for (const auto& [type, handler_list_ptr] : handlers_)
    {
        auto list = std::atomic_load(&handler_list_ptr);
        if (list && !list->empty())
        {
            MARKAMP_LOG_WARN("EventBus leak: {} active subscription(s) for type '{}'",
                             list->size(),
                             type.name());
        }
    }
}

// --- process_queued: main-thread-only, bounded ring buffer drain ---

void EventBus::process_queued()
{
    MARKAMP_ASSERT_MAIN_THREAD();

    std::vector<std::function<void()>> events_to_process;
    {
        const std::lock_guard lock(mutex_);
        if (queue_size_ == 0)
        {
            return;
        }

        events_to_process.reserve(queue_size_);

        // Determine read position: if the buffer hasn't wrapped, start from 0
        // Otherwise, read from (write_pos) which is the oldest entry
        if (queued_events_.size() < kMaxQueuedEvents || queue_size_ < kMaxQueuedEvents)
        {
            // Buffer hasn't filled or has space — entries are [0, queue_size_)
            // But we need to handle the ring correctly.
            // Simplest: if size < capacity, all entries are valid from start
            std::size_t read_pos = 0;
            if (queued_events_.size() >= kMaxQueuedEvents)
            {
                // Ring is at capacity, oldest is at write_pos
                read_pos = queue_write_pos_;
            }
            for (std::size_t i = 0; i < queue_size_; ++i)
            {
                auto idx = (read_pos + i) % queued_events_.size();
                events_to_process.push_back(std::move(queued_events_[idx]));
            }
        }
        else
        {
            // Shouldn't reach here, but safety
            for (std::size_t i = 0; i < queue_size_; ++i)
            {
                auto idx = (queue_write_pos_ + i) % kMaxQueuedEvents;
                events_to_process.push_back(std::move(queued_events_[idx]));
            }
        }

        queue_size_ = 0;
        queue_write_pos_ = 0;
        queued_events_.clear();
    }

    auto start = std::chrono::steady_clock::now();
    for (const auto& func : events_to_process)
    {
        if (func)
        {
            func();
        }
    }
    auto end = std::chrono::steady_clock::now();
    auto elapsed_us = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

    stat_total_drain_time_us_.fetch_add(elapsed_us, std::memory_order_relaxed);
    stat_drain_count_.fetch_add(1, std::memory_order_relaxed);
}

// --- drain_fast_queue: main-thread-only ---

void EventBus::drain_fast_queue()
{
    MARKAMP_ASSERT_MAIN_THREAD();

    // Track max fast queue depth
    auto approx_size = fast_queue_.size_approx();
    auto prev_max = stat_max_fast_queue_depth_.load(std::memory_order_relaxed);
    while (approx_size > prev_max && !stat_max_fast_queue_depth_.compare_exchange_weak(
                                         prev_max, approx_size, std::memory_order_relaxed))
    {
    }

    std::function<void()> func;
    while (fast_queue_.try_pop(func))
    {
        if (func)
        {
            func();
        }
    }
}

// --- has_pending: lock-free check for idle loop optimization ---

auto EventBus::has_pending() const noexcept -> bool
{
    // Fast check: if fast_queue_ is non-empty, we have work
    if (!fast_queue_.empty())
    {
        return true;
    }
    // Check queued_events_ — read queue_size_ atomically under no lock
    // This is a best-effort check; the mutex protects writes but
    // reading a size_t is atomic on all modern platforms.
    return queue_size_ > 0;
}

// --- stats: snapshot of performance counters ---

auto EventBus::stats() const noexcept -> EventBusStats
{
    EventBusStats result;
    result.publish_count = stat_publish_count_.load(std::memory_order_relaxed);
    result.publish_fast_count = stat_publish_fast_count_.load(std::memory_order_relaxed);
    result.queue_count = stat_queue_count_.load(std::memory_order_relaxed);
    result.drop_count = stat_drop_count_.load(std::memory_order_relaxed);
    result.max_queue_depth = stat_max_queue_depth_.load(std::memory_order_relaxed);
    result.max_fast_queue_depth = stat_max_fast_queue_depth_.load(std::memory_order_relaxed);
    result.total_drain_time_us = stat_total_drain_time_us_.load(std::memory_order_relaxed);
    result.drain_count = stat_drain_count_.load(std::memory_order_relaxed);
    result.active_subscriptions = stat_active_subscriptions_.load(std::memory_order_relaxed);
    return result;
}

} // namespace markamp::core
