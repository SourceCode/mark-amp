/// Backpressure.h — V7 Phase 19: Queue backpressure & rate limiting
///
/// BackpressureQueue: bounded queue with configurable overflow policies.
/// RateLimiter: token-bucket algorithm for rate limiting.

#pragma once

#include "Result.h"

#include <chrono>
#include <deque>
#include <mutex>
#include <optional>
#include <string>

namespace markamp::core
{

// ══════════════════════════════════════════════════════════════════════════════
// Overflow Policy
// ══════════════════════════════════════════════════════════════════════════════

enum class OverflowPolicy : uint8_t
{
    DropOldest = 0, // Remove oldest item to make room
    Reject = 1,     // Reject the new item
    Block = 2,      // Block until space is available (not yet implemented)
};

// ══════════════════════════════════════════════════════════════════════════════
// BackpressureQueue
// ══════════════════════════════════════════════════════════════════════════════

/// Thread-safe bounded queue with configurable overflow policy.
template <typename T>
class BackpressureQueue
{
public:
    explicit BackpressureQueue(size_t max_size, OverflowPolicy policy = OverflowPolicy::DropOldest)
        : max_size_(max_size)
        , policy_(policy)
    {
    }

    /// Push an item. Returns error if policy is Reject and queue is full.
    [[nodiscard]] auto push(T item) -> Result<void>
    {
        std::lock_guard lock(mutex_);
        if (queue_.size() >= max_size_)
        {
            switch (policy_)
            {
                case OverflowPolicy::DropOldest:
                    queue_.pop_front();
                    ++dropped_count_;
                    break;
                case OverflowPolicy::Reject:
                    ++rejected_count_;
                    return std::unexpected(
                        make_error(ErrorCode::ResourceExhausted,
                                   SubsystemId::Core,
                                   "Queue full (max " + std::to_string(max_size_) + ")"));
                case OverflowPolicy::Block:
                    // Future: condition variable wait
                    return std::unexpected(make_error(ErrorCode::NotSupported,
                                                      SubsystemId::Core,
                                                      "Block policy not yet implemented"));
            }
        }
        queue_.push_back(std::move(item));
        return {};
    }

    /// Try to pop an item. Returns std::nullopt if empty.
    [[nodiscard]] auto try_pop() -> std::optional<T>
    {
        std::lock_guard lock(mutex_);
        if (queue_.empty())
        {
            return std::nullopt;
        }
        T item = std::move(queue_.front());
        queue_.pop_front();
        return item;
    }

    /// Current queue size.
    [[nodiscard]] auto size() const -> size_t
    {
        std::lock_guard lock(mutex_);
        return queue_.size();
    }

    /// Check if queue is empty.
    [[nodiscard]] auto empty() const -> bool
    {
        std::lock_guard lock(mutex_);
        return queue_.empty();
    }

    /// Maximum queue capacity.
    [[nodiscard]] auto capacity() const noexcept -> size_t
    {
        return max_size_;
    }

    /// Number of items dropped due to overflow.
    [[nodiscard]] auto dropped_count() const noexcept -> size_t
    {
        return dropped_count_;
    }

    /// Number of items rejected due to overflow.
    [[nodiscard]] auto rejected_count() const noexcept -> size_t
    {
        return rejected_count_;
    }

    /// Clear all items.
    void clear()
    {
        std::lock_guard lock(mutex_);
        queue_.clear();
    }

private:
    size_t max_size_;
    OverflowPolicy policy_;
    mutable std::mutex mutex_;
    std::deque<T> queue_;
    size_t dropped_count_{0};
    size_t rejected_count_{0};
};

// ══════════════════════════════════════════════════════════════════════════════
// RateLimiter
// ══════════════════════════════════════════════════════════════════════════════

/// Token-bucket rate limiter.
class RateLimiter
{
public:
    /// Construct with tokens_per_second and max burst size.
    RateLimiter(double tokens_per_second, double max_burst)
        : rate_(tokens_per_second)
        , max_tokens_(max_burst)
        , tokens_(max_burst)
        , last_refill_(std::chrono::steady_clock::now())
    {
    }

    /// Try to consume one token. Returns true if allowed.
    [[nodiscard]] auto try_acquire() -> bool
    {
        return try_acquire(1.0);
    }

    /// Try to consume N tokens. Returns true if allowed.
    [[nodiscard]] auto try_acquire(double count) -> bool
    {
        std::lock_guard lock(mutex_);
        refill();
        if (tokens_ >= count)
        {
            tokens_ -= count;
            return true;
        }
        return false;
    }

    /// Current available tokens.
    [[nodiscard]] auto available_tokens() -> double
    {
        std::lock_guard lock(mutex_);
        refill();
        return tokens_;
    }

    /// Reset the limiter to full capacity.
    void reset()
    {
        std::lock_guard lock(mutex_);
        tokens_ = max_tokens_;
        last_refill_ = std::chrono::steady_clock::now();
    }

private:
    void refill()
    {
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration<double>(now - last_refill_).count();
        tokens_ = std::min(max_tokens_, tokens_ + elapsed * rate_);
        last_refill_ = now;
    }

    double rate_;
    double max_tokens_;
    double tokens_;
    std::chrono::steady_clock::time_point last_refill_;
    std::mutex mutex_;
};

} // namespace markamp::core
