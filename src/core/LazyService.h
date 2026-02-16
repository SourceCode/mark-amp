#pragma once

/// @file LazyService.h
/// @brief Thread-safe lazy service construction wrapper.
///
/// Phase 12: Startup Deferral — converts eagerly-constructed services
/// to demand-constructed instances. Zero overhead after first construction
/// (just a pointer dereference + flag check).
///
/// Usage:
///   LazyService<OutputChannelService> output_channel_{[this]() {
///       return std::make_unique<OutputChannelService>(event_bus_.get());
///   }};
///
///   // First call constructs; subsequent calls return cached pointer
///   auto* svc = output_channel_.get_or_create();

#include <atomic>
#include <functional>
#include <memory>
#include <mutex>

namespace markamp::core
{

/// Thread-safe lazy service wrapper with factory-based construction.
///
/// @tparam T The service type to lazily construct.
template <typename T>
class LazyService
{
public:
    /// Factory function type: produces a unique_ptr<T> on demand.
    using Factory = std::function<std::unique_ptr<T>()>;

    /// Construct with a factory that will be invoked on first access.
    explicit LazyService(Factory factory)
        : factory_(std::move(factory))
    {
    }

    /// Default constructor — must call set_factory() before get_or_create().
    LazyService() = default;

    // Non-copyable, non-movable
    LazyService(const LazyService&) = delete;
    auto operator=(const LazyService&) -> LazyService& = delete;
    LazyService(LazyService&&) = delete;
    auto operator=(LazyService&&) -> LazyService& = delete;

    ~LazyService() = default;

    /// Set or replace the factory. Must be called before first get_or_create().
    void set_factory(Factory factory)
    {
        factory_ = std::move(factory);
    }

    /// Get the service instance, constructing it on first call.
    /// Thread-safe: uses double-checked locking with acquire/release semantics.
    ///
    /// @return Non-owning pointer to the service. Lifetime managed by this wrapper.
    auto get_or_create() -> T*
    {
        // Fast path: already created (acquire for visibility)
        if (created_.load(std::memory_order_acquire))
        {
            return instance_.get();
        }

        // Slow path: first call, take the lock
        std::lock_guard<std::mutex> lock(mutex_);
        if (!created_.load(std::memory_order_relaxed))
        {
            if (factory_)
            {
                instance_ = factory_();
            }
            created_.store(true, std::memory_order_release);
        }
        return instance_.get();
    }

    /// Check whether the service has been constructed without triggering construction.
    [[nodiscard]] auto is_created() const noexcept -> bool
    {
        return created_.load(std::memory_order_acquire);
    }

    /// Get the raw pointer without triggering construction.
    /// Returns nullptr if not yet created.
    [[nodiscard]] auto get() const noexcept -> T*
    {
        return instance_.get();
    }

    /// Reset for test teardown. NOT thread-safe — use only in tests.
    void reset()
    {
        instance_.reset();
        created_.store(false, std::memory_order_release);
    }

private:
    Factory factory_;
    std::unique_ptr<T> instance_;
    std::atomic<bool> created_{false};
    std::mutex mutex_;
};

} // namespace markamp::core
