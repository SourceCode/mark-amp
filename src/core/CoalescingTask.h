#pragma once

#include <atomic>
#include <cstdint>
#include <memory>

namespace markamp::core
{

/// Lightweight cancellation token — portable replacement for std::stop_token.
/// Workers check `stop_requested()` periodically and abandon stale work.
class CancelToken
{
public:
    CancelToken()
        : cancelled_(std::make_shared<std::atomic<bool>>(false))
    {
    }

    /// Request cancellation.
    void request_stop() noexcept
    {
        if (cancelled_)
        {
            cancelled_->store(true, std::memory_order_release);
        }
    }

    /// Check if cancellation has been requested.
    [[nodiscard]] auto stop_requested() const noexcept -> bool
    {
        return cancelled_ && cancelled_->load(std::memory_order_acquire);
    }

private:
    std::shared_ptr<std::atomic<bool>> cancelled_;
};

/// Manages a cancelable, coalescable background task (latest-wins).
///
/// When the user types quickly, many re-highlight / re-search / re-index
/// jobs are created. Each call to submit() cancels the previous job's
/// token and issues a fresh CancelToken. Workers periodically
/// check the token and abandon stale work.
///
/// Pattern implemented: #8 Work coalescing and cancellation
class CoalescingTask
{
public:
    CoalescingTask() = default;

    /// Submit new work. Cancels any in-flight work for a prior version.
    /// Returns the CancelToken the worker should check periodically.
    [[nodiscard]] auto submit(uint64_t version) -> CancelToken
    {
        // Cancel the previous task
        current_token_.request_stop();

        // Create a new token for the new task
        current_token_ = CancelToken{};
        latest_version_.store(version, std::memory_order_release);

        return current_token_;
    }

    /// Check if a result for the given version is still wanted.
    /// Workers call this before publishing their results.
    [[nodiscard]] auto is_current(uint64_t version) const noexcept -> bool
    {
        return latest_version_.load(std::memory_order_acquire) == version;
    }

    /// Get the current version number.
    [[nodiscard]] auto current_version() const noexcept -> uint64_t
    {
        return latest_version_.load(std::memory_order_acquire);
    }

    /// Request cancellation of the current task.
    void cancel() noexcept
    {
        current_token_.request_stop();
    }

    /// Check if cancellation was requested for the current task.
    [[nodiscard]] auto stop_requested() const noexcept -> bool
    {
        return current_token_.stop_requested();
    }

private:
    CancelToken current_token_;
    std::atomic<uint64_t> latest_version_{0};
};

} // namespace markamp::core
