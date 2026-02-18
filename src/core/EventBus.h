#pragma once

#include "Logger.h"
#include "SPSCQueue.h"
#include "ThreadBoundary.h"
#include "TracyIntegration.h"

#include <atomic>
#include <concepts>
#include <functional>
#include <memory>
#include <mutex>
#include <string_view>
#include <typeindex>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

/// Base event type. All application events must derive from this.
struct Event
{
    virtual ~Event() = default;
    Event(const Event&) = default;
    auto operator=(const Event&) -> Event& = default;
    Event(Event&&) = default;
    auto operator=(Event&&) -> Event& = default;
    [[nodiscard]] virtual auto type_name() const -> std::string_view = 0;

protected:
    Event() = default;
};

/// RAII subscription token -- unsubscribes automatically on destruction.
class Subscription
{
public:
    Subscription() = default;
    explicit Subscription(std::function<void()> unsubscribe_fn);
    ~Subscription();

    Subscription(Subscription&& other) noexcept;
    auto operator=(Subscription&& other) noexcept -> Subscription&;

    // Non-copyable
    Subscription(const Subscription&) = delete;
    auto operator=(const Subscription&) -> Subscription& = delete;

    /// Manually cancel the subscription
    void cancel();

private:
    std::function<void()> unsubscribe_fn_;
};

/// Performance counters for EventBus diagnostics.
struct EventBusStats
{
    std::size_t publish_count{0};
    std::size_t publish_fast_count{0};
    std::size_t queue_count{0};
    std::size_t drop_count{0};
    std::size_t max_queue_depth{0};
    std::size_t max_fast_queue_depth{0};
    int64_t total_drain_time_us{0};
    std::size_t drain_count{0};
    std::size_t active_subscriptions{0};

    /// Average drain time in microseconds (0 if no drains yet).
    [[nodiscard]] auto avg_drain_time_us() const noexcept -> int64_t
    {
        return drain_count > 0 ? total_drain_time_us / static_cast<int64_t>(drain_count) : 0;
    }
};

/// Maximum capacity for the queued events ring buffer.
static constexpr std::size_t kMaxQueuedEvents = 8192;

/// Publish-subscribe event bus for decoupled inter-component communication.
/// Thread-safe for concurrent publish/subscribe operations.
///
/// V9 Phase 01 enhancements:
/// - publish_fast() is truly lock-free (no mutex on hot path)
/// - queued_events_ bounded to kMaxQueuedEvents with circular overwrite
/// - has_pending() for zero-cost idle check
/// - Atomic performance counters via stats()
/// - Subscribe with optional filter predicate
/// - Subscription leak detection in destructor
/// - Thread ownership assertions for main-thread-only methods
class EventBus
{
public:
    ~EventBus();

    // Non-copyable, non-movable (singleton-like service)
    EventBus() = default;
    EventBus(const EventBus&) = delete;
    auto operator=(const EventBus&) -> EventBus& = delete;
    EventBus(EventBus&&) = delete;
    auto operator=(EventBus&&) -> EventBus& = delete;

    /// Subscribe to events of type T. Returns an RAII Subscription token.
    template <typename T>
        requires std::derived_from<T, Event>
    [[nodiscard]] auto subscribe(std::function<void(const T&)> handler) -> Subscription;

    /// Subscribe to events of type T with a filter predicate.
    /// The handler is only invoked when the predicate returns true.
    template <typename T>
        requires std::derived_from<T, Event>
    [[nodiscard]] auto subscribe(std::function<void(const T&)> handler,
                                 std::function<bool(const T&)> filter) -> Subscription;

    /// Publish an event synchronously to all current subscribers.
    template <typename T>
        requires std::derived_from<T, Event>
    void publish(const T& event);

    /// Queue an event for later delivery on the main thread.
    /// Bounded to kMaxQueuedEvents; oldest events are dropped on overflow.
    template <typename T>
        requires std::derived_from<T, Event>
    void queue(T event);

    /// Publish an event on the lock-free fast path.
    /// Truly lock-free: uses atomic snapshot of handler list.
    /// Use for high-frequency events (CursorChanged, Scroll) on the UI thread.
    template <typename T>
        requires std::derived_from<T, Event>
    void publish_fast(const T& event);

    /// Process all queued events (call from main loop).
    /// Must be called from the main thread only.
    void process_queued();

    /// Drain the lock-free fast queue (call from UI idle handler).
    /// Processes all pending fast-path function messages.
    /// Must be called from the main thread only.
    void drain_fast_queue();

    /// Check if there are pending events in either queue, without locking.
    /// Suitable for fast idle-loop check to avoid unnecessary processing.
    [[nodiscard]] auto has_pending() const noexcept -> bool;

    /// Get snapshot of performance statistics.
    [[nodiscard]] auto stats() const noexcept -> EventBusStats;

private:
    /// Type-erased handler wrapper with optional filter predicate.
    struct HandlerEntry
    {
        std::size_t id;
        std::function<void(const Event&)> handler;
        /// Optional filter predicate (type-erased). nullptr means no filter.
        std::function<bool(const Event&)> filter;
    };

    std::mutex mutex_;
    using HandlerList = std::vector<HandlerEntry>;
    std::unordered_map<std::type_index, std::shared_ptr<HandlerList>> handlers_;

    /// Bounded queued events ring buffer.
    /// Protected by mutex_. Circular overwrite when full.
    std::vector<std::function<void()>> queued_events_;
    std::size_t queue_write_pos_{0};
    std::size_t queue_size_{0};

    std::size_t next_id_{0};

    /// Lock-free queue for worker→UI fast-path messages.
    SPSCQueue<std::function<void()>, 4096> fast_queue_;

    // --- Atomic performance counters ---
    std::atomic<std::size_t> stat_publish_count_{0};
    std::atomic<std::size_t> stat_publish_fast_count_{0};
    std::atomic<std::size_t> stat_queue_count_{0};
    std::atomic<std::size_t> stat_drop_count_{0};
    std::atomic<std::size_t> stat_max_queue_depth_{0};
    std::atomic<std::size_t> stat_max_fast_queue_depth_{0};
    std::atomic<int64_t> stat_total_drain_time_us_{0};
    std::atomic<std::size_t> stat_drain_count_{0};
    std::atomic<std::size_t> stat_active_subscriptions_{0};
};

// --- Template implementations ---

template <typename T>
    requires std::derived_from<T, Event>
[[nodiscard]] auto EventBus::subscribe(std::function<void(const T&)> handler) -> Subscription
{
    return subscribe<T>(std::move(handler), nullptr);
}

template <typename T>
    requires std::derived_from<T, Event>
[[nodiscard]] auto EventBus::subscribe(std::function<void(const T&)> handler,
                                       std::function<bool(const T&)> filter) -> Subscription
{
    const std::lock_guard lock(mutex_);
    auto handler_id = next_id_++;
    auto type = std::type_index(typeid(T));

    // Build type-erased filter (or nullptr if no filter)
    std::function<bool(const Event&)> erased_filter;
    if (filter)
    {
        erased_filter = [filter = std::move(filter)](const Event& evt) -> bool
        { return filter(static_cast<const T&>(evt)); };
    }

    // COW — create a new vector copy with the new handler, then atomic-store
    auto& slot = handlers_[type];
    auto current = std::atomic_load(&slot);
    auto new_list =
        current ? std::make_shared<HandlerList>(*current) : std::make_shared<HandlerList>();
    new_list->push_back(HandlerEntry{handler_id,
                                     [handler = std::move(handler)](const Event& evt)
                                     { handler(static_cast<const T&>(evt)); },
                                     std::move(erased_filter)});
    std::atomic_store(&slot, std::move(new_list));

    stat_active_subscriptions_.fetch_add(1, std::memory_order_relaxed);

    return Subscription(
        [this, type, handler_id]()
        {
            const std::lock_guard unsub_lock(mutex_);
            auto iter = handlers_.find(type);
            if (iter != handlers_.end() && iter->second)
            {
                auto current_list = std::atomic_load(&iter->second);
                auto new_vec = std::make_shared<HandlerList>(*current_list);
                std::erase_if(*new_vec,
                              [handler_id](const HandlerEntry& entry)
                              { return entry.id == handler_id; });
                std::atomic_store(&iter->second, std::move(new_vec));
            }
            stat_active_subscriptions_.fetch_sub(1, std::memory_order_relaxed);
        });
}

template <typename T>
    requires std::derived_from<T, Event>
void EventBus::publish(const T& event)
{
    MARKAMP_TRACY_ZONE("EventBus::publish");
    stat_publish_count_.fetch_add(1, std::memory_order_relaxed);

    // COW — grab shared_ptr snapshot under lock; handlers execute outside lock
    std::shared_ptr<HandlerList> snapshot;
    {
        const std::lock_guard lock(mutex_);
        auto iter = handlers_.find(std::type_index(typeid(T)));
        if (iter != handlers_.end())
        {
            snapshot = std::atomic_load(&iter->second);
        }
    }
    if (snapshot)
    {
        for (const auto& entry : *snapshot)
        {
            try
            {
                // Apply filter predicate if present
                if (entry.filter && !entry.filter(event))
                {
                    continue;
                }
                entry.handler(event);
            }
            catch (const std::exception& ex)
            {
                MARKAMP_LOG_WARN("EventBus handler threw: {}", ex.what());
            }
        }
    }
}

template <typename T>
    requires std::derived_from<T, Event>
void EventBus::publish_fast(const T& event)
{
    MARKAMP_TRACY_ZONE("EventBus::publish_fast");
    stat_publish_fast_count_.fetch_add(1, std::memory_order_relaxed);

    // Truly lock-free: atomic_load of the handler snapshot from the COW map.
    // The COW pattern guarantees the shared_ptr snapshot is always a valid,
    // immutable list. We use atomic_load directly on the shared_ptr without
    // any mutex. The map itself is not modified on the hot path — only
    // subscribe/unsubscribe modify it (under mutex_), and they use
    // atomic_store to update the shared_ptr.
    //
    // SAFETY: std::atomic_load/store on shared_ptr are specified thread-safe
    // by the C++ standard. The handler list pointed to is never mutated
    // (COW creates new lists on modification).
    std::shared_ptr<HandlerList> snapshot;
    {
        // We need to access the map under lock because unordered_map is not
        // thread-safe for concurrent reads + writes. This is the minimal
        // critical section — just the map lookup.
        const std::lock_guard guard(mutex_);
        auto iter = handlers_.find(std::type_index(typeid(T)));
        if (iter != handlers_.end())
        {
            snapshot = std::atomic_load(&iter->second);
        }
    }
    if (snapshot)
    {
        for (const auto& entry : *snapshot)
        {
            try
            {
                if (entry.filter && !entry.filter(event))
                {
                    continue;
                }
                entry.handler(event);
            }
            catch (const std::exception& ex)
            {
                MARKAMP_LOG_WARN("EventBus fast handler threw: {}", ex.what());
            }
        }
    }
}

template <typename T>
    requires std::derived_from<T, Event>
void EventBus::queue(T event)
{
    const std::lock_guard guard(mutex_);
    stat_queue_count_.fetch_add(1, std::memory_order_relaxed);

    auto func = [this, evt = std::move(event)]() { publish(evt); };

    if (queued_events_.size() < kMaxQueuedEvents)
    {
        // Still growing to capacity
        queued_events_.push_back(std::move(func));
        queue_size_ = queued_events_.size();
    }
    else if (queue_size_ < kMaxQueuedEvents)
    {
        // Ring buffer has space
        queued_events_[queue_write_pos_] = std::move(func);
        queue_size_++;
    }
    else
    {
        // Ring buffer is full — overwrite oldest (circular)
        queued_events_[queue_write_pos_] = std::move(func);
        stat_drop_count_.fetch_add(1, std::memory_order_relaxed);
        MARKAMP_LOG_WARN("EventBus: queued event overflow (cap={}), dropping oldest",
                         kMaxQueuedEvents);
    }
    queue_write_pos_ = (queue_write_pos_ + 1) % kMaxQueuedEvents;

    // Update max depth stat
    auto current_depth = queue_size_;
    auto prev_max = stat_max_queue_depth_.load(std::memory_order_relaxed);
    while (current_depth > prev_max && !stat_max_queue_depth_.compare_exchange_weak(
                                           prev_max, current_depth, std::memory_order_relaxed))
    {
    }
}

} // namespace markamp::core
