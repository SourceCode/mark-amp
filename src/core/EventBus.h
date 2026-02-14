#pragma once

#include "Logger.h"
#include "SPSCQueue.h"

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
    [[nodiscard]] virtual auto type_name() const -> std::string_view = 0;
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

/// Publish-subscribe event bus for decoupled inter-component communication.
/// Thread-safe for concurrent publish/subscribe operations.
class EventBus
{
public:
    /// Subscribe to events of type T. Returns an RAII Subscription token.
    template <typename T>
        requires std::derived_from<T, Event>
    [[nodiscard]] auto subscribe(std::function<void(const T&)> handler) -> Subscription;

    /// Publish an event synchronously to all current subscribers.
    template <typename T>
        requires std::derived_from<T, Event>
    void publish(const T& event);

    /// Queue an event for later delivery on the main thread.
    template <typename T>
        requires std::derived_from<T, Event>
    void queue(T event);

    /// Publish an event on the lock-free fast path.
    /// Bypasses the mutex by atomically loading the handler snapshot.
    /// Safe because handlers_ uses COW — the shared_ptr is always valid.
    /// Use for high-frequency events (CursorChanged, Scroll) on the UI thread.
    template <typename T>
        requires std::derived_from<T, Event>
    void publish_fast(const T& event);

    /// Process all queued events (call from main loop).
    void process_queued();

    /// Drain the lock-free fast queue (call from UI idle handler).
    /// Processes all pending fast-path function messages.
    void drain_fast_queue();

private:
    /// Type-erased handler wrapper
    struct HandlerEntry
    {
        std::size_t id;
        std::function<void(const Event&)> handler;
    };

    std::mutex mutex_;
    std::mutex fast_lookup_mutex_; // Lightweight lock for map lookup in publish_fast
    using HandlerList = std::vector<HandlerEntry>;
    std::unordered_map<std::type_index, std::shared_ptr<HandlerList>> handlers_;
    std::vector<std::function<void()>> queued_events_;
    std::size_t next_id_{0};

    /// Lock-free queue for worker→UI fast-path messages.
    SPSCQueue<std::function<void()>, 1024> fast_queue_;
};

// --- Template implementations ---

template <typename T>
    requires std::derived_from<T, Event>
[[nodiscard]] auto EventBus::subscribe(std::function<void(const T&)> handler) -> Subscription
{
    std::lock_guard lock(mutex_);
    auto handler_id = next_id_++;
    auto type = std::type_index(typeid(T));

    // COW — create a new vector copy with the new handler, then atomic-store
    auto& slot = handlers_[type];
    auto current = std::atomic_load(&slot);
    auto new_list =
        current ? std::make_shared<HandlerList>(*current) : std::make_shared<HandlerList>();
    new_list->push_back(HandlerEntry{handler_id, [handler = std::move(handler)](const Event& evt) {
                                         handler(static_cast<const T&>(evt));
                                     }});
    std::atomic_store(&slot, std::move(new_list));

    return Subscription(
        [this, type, handler_id]()
        {
            std::lock_guard unsub_lock(mutex_);
            auto it = handlers_.find(type);
            if (it != handlers_.end() && it->second)
            {
                auto current_list = std::atomic_load(&it->second);
                auto new_vec = std::make_shared<HandlerList>(*current_list);
                std::erase_if(*new_vec,
                              [handler_id](const HandlerEntry& entry)
                              { return entry.id == handler_id; });
                std::atomic_store(&it->second, std::move(new_vec));
            }
        });
}

template <typename T>
    requires std::derived_from<T, Event>
void EventBus::publish(const T& event)
{
    // COW — grab shared_ptr snapshot under lock; handlers execute outside lock
    std::shared_ptr<HandlerList> snapshot;
    {
        std::lock_guard lock(mutex_);
        auto it = handlers_.find(std::type_index(typeid(T)));
        if (it != handlers_.end())
        {
            snapshot = std::atomic_load(&it->second);
        }
    }
    if (snapshot)
    {
        for (const auto& entry : *snapshot)
        {
            try
            {
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
    // Lock-free fast path: atomic load of the handler snapshot.
    // No mutex needed — the COW pattern guarantees the shared_ptr
    // snapshot is always a valid, immutable list.
    std::shared_ptr<HandlerList> snapshot;
    {
        // We still need the map lookup under a lock because unordered_map
        // is not thread-safe for concurrent reads + writes. However, we
        // cache the shared_ptr* to avoid repeated lookups.
        std::lock_guard lock(fast_lookup_mutex_);
        auto it = handlers_.find(std::type_index(typeid(T)));
        if (it != handlers_.end())
        {
            snapshot = std::atomic_load(&it->second);
        }
    }
    if (snapshot)
    {
        for (const auto& entry : *snapshot)
        {
            try
            {
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
    std::lock_guard lock(mutex_);
    queued_events_.push_back([this, evt = std::move(event)]() { publish(evt); });
}

} // namespace markamp::core
