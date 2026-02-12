#pragma once

#include <array>
#include <atomic>
#include <cstddef>
#include <type_traits>

namespace markamp::core
{

/// Lock-free single-producer single-consumer ring buffer.
/// Used for fast worker→UI thread communication without mutexes.
///
/// Capacity must be a power of 2. Both try_push() and try_pop() are
/// O(1) and noexcept — safe for use on the UI thread's hot path.
///
/// Patterns implemented:
///   #1  Single-purpose latency-first UI thread
///   #7  Minimal locking via message passing
template <typename T, std::size_t Capacity>
class SPSCQueue
{
    static_assert((Capacity & (Capacity - 1)) == 0, "Capacity must be a power of 2");
    static_assert(Capacity > 0, "Capacity must be positive");

public:
    SPSCQueue() = default;

    /// Producer: enqueue an item. Returns false if the queue is full.
    [[nodiscard]] auto try_push(const T& item) noexcept -> bool
    {
        auto tail = tail_.load(std::memory_order_relaxed);
        auto next = (tail + 1) & kMask;
        if (next == head_.load(std::memory_order_acquire))
        {
            return false; // queue is full
        }
        buffer_[tail] = item;
        tail_.store(next, std::memory_order_release);
        return true;
    }

    /// Producer: enqueue a moved item. Returns false if the queue is full.
    [[nodiscard]] auto try_push(T&& item) noexcept -> bool
    {
        auto tail = tail_.load(std::memory_order_relaxed);
        auto next = (tail + 1) & kMask;
        if (next == head_.load(std::memory_order_acquire))
        {
            return false;
        }
        buffer_[tail] = std::move(item);
        tail_.store(next, std::memory_order_release);
        return true;
    }

    /// Consumer: dequeue an item. Returns false if the queue is empty.
    [[nodiscard]] auto try_pop(T& item) noexcept -> bool
    {
        auto head = head_.load(std::memory_order_relaxed);
        if (head == tail_.load(std::memory_order_acquire))
        {
            return false; // queue is empty
        }
        item = std::move(buffer_[head]);
        head_.store((head + 1) & kMask, std::memory_order_release);
        return true;
    }

    /// Check if the queue is empty (approximate — for diagnostics only).
    [[nodiscard]] auto empty() const noexcept -> bool
    {
        return head_.load(std::memory_order_acquire) == tail_.load(std::memory_order_acquire);
    }

    /// Approximate number of items in the queue (snapshot — for diagnostics only).
    [[nodiscard]] auto size_approx() const noexcept -> std::size_t
    {
        auto tail = tail_.load(std::memory_order_acquire);
        auto head = head_.load(std::memory_order_acquire);
        return (tail - head) & kMask;
    }

    /// Maximum number of items the queue can hold.
    static constexpr auto capacity() noexcept -> std::size_t
    {
        return Capacity - 1;
    }

private:
    static constexpr std::size_t kMask = Capacity - 1;

    // Cache-line-aligned head and tail to prevent false sharing.
    alignas(64) std::atomic<std::size_t> head_{0};
    alignas(64) std::atomic<std::size_t> tail_{0};
    std::array<T, Capacity> buffer_{};
};

} // namespace markamp::core
