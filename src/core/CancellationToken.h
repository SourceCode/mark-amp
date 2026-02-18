/// CancellationToken.h — V7 Phase 17: Async task cancellation tokens
///
/// Provides cooperative cancellation for async operations. A
/// CancellationTokenSource creates tokens that can be shared with
/// background tasks. Tasks check for cancellation and exit cleanly.

#pragma once

#include "Result.h"

#include <atomic>
#include <functional>
#include <memory>
#include <mutex>
#include <vector>

namespace markamp::core
{

/// Shared state for cancellation signaling.
struct CancellationState
{
    std::atomic<bool> cancelled{false};
    std::mutex callback_mutex;
    std::vector<std::function<void()>> callbacks;
};

/// Read-only cancellation token. Pass to background tasks.
class CancellationToken
{
public:
    CancellationToken() = default;
    explicit CancellationToken(std::shared_ptr<CancellationState> state)
        : state_(std::move(state))
    {
    }

    /// Check if cancellation has been requested.
    [[nodiscard]] auto is_cancelled() const noexcept -> bool
    {
        return state_ && state_->cancelled.load(std::memory_order_acquire);
    }

    /// Returns an error if cancelled.
    [[nodiscard]] auto throw_if_cancelled() const -> Result<void>
    {
        if (is_cancelled())
        {
            return std::unexpected(
                make_error(ErrorCode::Cancelled, SubsystemId::Core, "Operation was cancelled"));
        }
        return {};
    }

    /// Register a callback invoked when cancellation is requested.
    /// If already cancelled, the callback fires immediately.
    void register_callback(std::function<void()> callback)
    {
        if (!state_)
        {
            return;
        }
        if (state_->cancelled.load(std::memory_order_acquire))
        {
            callback();
            return;
        }
        std::lock_guard lock(state_->callback_mutex);
        if (state_->cancelled.load(std::memory_order_acquire))
        {
            callback();
        }
        else
        {
            state_->callbacks.push_back(std::move(callback));
        }
    }

    /// Returns true if this token has shared state (i.e. is linked to a source).
    [[nodiscard]] auto is_valid() const noexcept -> bool
    {
        return state_ != nullptr;
    }

    /// A token that is never cancelled.
    [[nodiscard]] static auto none() -> CancellationToken
    {
        return CancellationToken{};
    }

private:
    std::shared_ptr<CancellationState> state_;
};

/// Creates and owns cancellation tokens. Call cancel() to signal all tokens.
class CancellationTokenSource
{
public:
    CancellationTokenSource()
        : state_(std::make_shared<CancellationState>())
    {
    }

    /// Get a read-only token to share with workers.
    [[nodiscard]] auto token() const -> CancellationToken
    {
        return CancellationToken(state_);
    }

    /// Signal cancellation. All registered callbacks will fire.
    void cancel()
    {
        if (state_->cancelled.exchange(true, std::memory_order_release))
        {
            return; // Already cancelled
        }
        std::lock_guard lock(state_->callback_mutex);
        for (auto& callback : state_->callbacks)
        {
            callback();
        }
        state_->callbacks.clear();
    }

    /// Check if cancellation has already been requested.
    [[nodiscard]] auto is_cancelled() const noexcept -> bool
    {
        return state_->cancelled.load(std::memory_order_acquire);
    }

private:
    std::shared_ptr<CancellationState> state_;
};

} // namespace markamp::core
