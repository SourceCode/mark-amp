/// FlatEventBus.h — Phase 20: Flat-array subscriber EventBus
///
/// O(1) event dispatch via EventTypeId-indexed arrays.
/// Eliminates unordered_map lookup. Uses shared_mutex for read-heavy workloads:
/// - Readers take shared_lock (concurrent reads allowed)
/// - Writers (subscribe/unsubscribe) take unique_lock + COW
///
/// Patterns implemented:
///   #5  O(1) subscriber array, skip map lookup
///   #7  Minimal locking via COW handler lists

#pragma once

#include "EventBus.h"
#include "EventTypeId.h"

#include <array>
#include <cstdint>
#include <functional>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <vector>

namespace markamp::core
{

/// Flat-array EventBus using EventTypeId for O(1) subscriber lookup.
///
/// Usage:
///   FlatEventBus bus;
///   auto sub = bus.subscribe(EventTypeId::ThemeChanged, [](const Event& e) { ... });
///   bus.publish(EventTypeId::ThemeChanged, my_event);
class FlatEventBus
{
public:
    /// Subscription handle — removes handler on destruction.
    class Subscription
    {
    public:
        Subscription() = default;
        Subscription(FlatEventBus* bus, EventTypeId event_id, uint64_t handler_id)
            : bus_(bus)
            , id_(event_id)
            , handler_id_(handler_id)
        {
        }
        ~Subscription()
        {
            if (bus_ != nullptr)
            {
                bus_->unsubscribe(id_, handler_id_);
            }
        }

        // Move-only
        Subscription(const Subscription&) = delete;
        auto operator=(const Subscription&) -> Subscription& = delete;
        Subscription(Subscription&& other) noexcept
            : bus_(other.bus_)
            , id_(other.id_)
            , handler_id_(other.handler_id_)
        {
            other.bus_ = nullptr;
        }
        auto operator=(Subscription&& other) noexcept -> Subscription&
        {
            if (this != &other)
            {
                if (bus_ != nullptr)
                {
                    bus_->unsubscribe(id_, handler_id_);
                }
                bus_ = other.bus_;
                id_ = other.id_;
                handler_id_ = other.handler_id_;
                other.bus_ = nullptr;
            }
            return *this;
        }

    private:
        FlatEventBus* bus_{nullptr};
        EventTypeId id_{};
        uint64_t handler_id_{0};
    };

    using Handler = std::function<void(const Event&)>;

    FlatEventBus() = default;

    /// Subscribe a handler for a specific EventTypeId. Returns RAII subscription.
    [[nodiscard]] auto subscribe(EventTypeId id, Handler handler) -> Subscription
    {
        auto index = static_cast<std::size_t>(id);
        if (index >= kMaxEventTypes) [[unlikely]]
        {
            return {};
        }

        auto& slot = slots_[index];
        const std::unique_lock lock(slot.mutex);

        auto handler_id = ++next_handler_id_;

        // COW: copy the handler list, append, then swap
        auto new_list = std::make_shared<HandlerVector>(*slot.handlers);
        new_list->push_back({handler_id, std::move(handler)});
        slot.handlers = std::move(new_list);

        return Subscription(this, id, handler_id);
    }

    /// Publish an event to all subscribers. Shared lock for concurrent reads.
    void publish(EventTypeId id, const Event& event) const
    {
        auto index = static_cast<std::size_t>(id);
        if (index >= kMaxEventTypes) [[unlikely]]
        {
            return;
        }

        // Take shared lock — allows concurrent publishes
        std::shared_ptr<HandlerVector> handlers;
        {
            const std::shared_lock lock(slots_[index].mutex);
            handlers = slots_[index].handlers;
        }

        // Dispatch outside the lock — handlers can subscribe/unsubscribe
        for (const auto& entry : *handlers)
        {
            entry.handler(event);
        }
    }

    /// Number of subscribers for a specific event type.
    [[nodiscard]] auto subscriber_count(EventTypeId id) const -> std::size_t
    {
        auto index = static_cast<std::size_t>(id);
        if (index >= kMaxEventTypes)
        {
            return 0;
        }
        const std::shared_lock lock(slots_[index].mutex);
        return slots_[index].handlers->size();
    }

    /// Total number of subscribers across all event types.
    [[nodiscard]] auto total_subscriber_count() const -> std::size_t
    {
        std::size_t total = 0;
        for (const auto& slot : slots_)
        {
            const std::shared_lock lock(slot.mutex);
            total += slot.handlers->size();
        }
        return total;
    }

private:
    struct HandlerEntry
    {
        uint64_t id{0};
        Handler handler;
    };

    using HandlerVector = std::vector<HandlerEntry>;

    struct Slot
    {
        std::shared_ptr<HandlerVector> handlers{std::make_shared<HandlerVector>()};
        mutable std::shared_mutex mutex;
    };

    void unsubscribe(EventTypeId id, uint64_t handler_id)
    {
        auto index = static_cast<std::size_t>(id);
        if (index >= kMaxEventTypes)
        {
            return;
        }

        auto& slot = slots_[index];
        const std::unique_lock lock(slot.mutex);

        auto new_list = std::make_shared<HandlerVector>();
        new_list->reserve(slot.handlers->size());

        for (const auto& entry : *slot.handlers)
        {
            if (entry.id != handler_id)
            {
                new_list->push_back(entry);
            }
        }

        slot.handlers = std::move(new_list);
    }

    std::array<Slot, kMaxEventTypes> slots_;
    std::atomic<uint64_t> next_handler_id_{0};
};

} // namespace markamp::core
