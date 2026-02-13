#pragma once

#include <chrono>
#include <cstdint>
#include <functional>
#include <queue>
#include <vector>

namespace markamp::core
{

/// Task priority levels for the frame scheduler.
/// Lower numeric value = higher priority = processed first.
///
/// Pattern implemented: #20 Deterministic scheduling
enum class TaskPriority : uint8_t
{
    Input = 0,     // Highest — process immediately (keystroke, mouse)
    Paint = 1,     // Repaint dirty regions
    Layout = 2,    // Re-measure / re-layout changed lines
    Highlight = 3, // Apply syntax highlighting results
    Search = 4,    // Process incremental search results
    FileIO = 5,    // File load/save completion handling
    Background = 6 // Lowest — housekeeping, only when idle
};

/// A single schedulable unit of work.
struct ScheduledTask
{
    TaskPriority priority{TaskPriority::Background};
    std::chrono::microseconds budget{0};

    /// Execute one chunk of work. Return true if more work remains
    /// (the task will be re-enqueued for the next frame at the same priority).
    std::function<bool()> execute;

    /// Comparison for priority_queue (min-heap: lowest priority value = highest urgency).
    auto operator>(const ScheduledTask& other) const noexcept -> bool
    {
        return static_cast<uint8_t>(priority) > static_cast<uint8_t>(other.priority);
    }
};

/// Frame-budget-aware cooperative scheduler.
///
/// Enqueue tasks with priorities. Call run_frame() once per frame with
/// the available time budget. The scheduler executes tasks in priority
/// order until the budget is exhausted, then defers remaining work
/// to the next frame.
///
/// This ensures input and paint always run first, while background
/// work (highlighting, search, indexing) gracefully yields when the
/// frame is running long.
///
/// Pattern implemented: #20 Deterministic scheduling
class FrameScheduler
{
public:
    FrameScheduler() = default;

    /// Enqueue a task for execution.
    void enqueue(ScheduledTask task)
    {
        queue_.push(std::move(task));
    }

    /// Run tasks until the frame budget is exhausted.
    /// Tasks that return true from execute() are re-enqueued.
    void run_frame(std::chrono::microseconds frame_budget)
    {
        auto frame_start = std::chrono::high_resolution_clock::now();
        std::vector<ScheduledTask> deferred;

        while (!queue_.empty())
        {
            auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(
                std::chrono::high_resolution_clock::now() - frame_start);
            if (elapsed >= frame_budget)
            {
                break; // budget exhausted — remaining tasks deferred to next frame
            }

            // R20 Fix 15: Avoid const_cast — extract top by copy, then pop.
            // Priority queues expose top() as const&, so const_cast was fragile.
            auto task = queue_.top();
            queue_.pop();

            bool has_more = task.execute();
            if (has_more)
            {
                deferred.push_back(std::move(task));
            }
        }

        // Re-enqueue incomplete tasks
        for (auto& task : deferred)
        {
            queue_.push(std::move(task));
        }
    }

    /// Check if there are pending tasks.
    [[nodiscard]] auto has_pending() const noexcept -> bool
    {
        return !queue_.empty();
    }

    /// Number of pending tasks.
    [[nodiscard]] auto pending_count() const noexcept -> std::size_t
    {
        return queue_.size();
    }

    /// Clear all pending tasks.
    void clear()
    {
        queue_ = {};
    }

private:
    std::priority_queue<ScheduledTask, std::vector<ScheduledTask>, std::greater<>> queue_;
};

} // namespace markamp::core
