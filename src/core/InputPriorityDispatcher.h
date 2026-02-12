#pragma once

#include "FrameScheduler.h"

#include <functional>
#include <vector>

namespace markamp::core
{

/// Event categories for priority-ordered dispatch.
/// Input events are always processed before render, which is before secondary.
///
/// Pattern implemented: #22 Input-first event handling (priority inversion avoidance)
enum class EventCategory : uint8_t
{
    Input,    // Keystrokes, mouse — highest priority
    Render,   // Repaint/layout — normal priority
    Secondary // Background analysis, prefetch — lowest priority
};

/// Central dispatcher that guarantees input events are processed first,
/// then render updates, then secondary work. Wraps FrameScheduler with
/// explicit category-based enqueue methods.
///
/// Pattern implemented: #22 Input-first event handling (priority inversion avoidance)
class InputPriorityDispatcher
{
public:
    InputPriorityDispatcher() = default;

    /// Dispatch an input callback (keystrokes, mouse).
    /// Guaranteed to execute before any render or secondary work.
    void dispatch_input(std::function<bool()> callback)
    {
        scheduler_.enqueue(
            ScheduledTask{.priority = TaskPriority::Input, .execute = std::move(callback)});
    }

    /// Dispatch a render callback (paint, layout).
    /// Runs after all pending input callbacks.
    void dispatch_render(std::function<bool()> callback)
    {
        scheduler_.enqueue(
            ScheduledTask{.priority = TaskPriority::Paint, .execute = std::move(callback)});
    }

    /// Dispatch secondary/background work (highlighting, search, prefetch).
    /// Runs only after all input and render work is complete within budget.
    void dispatch_secondary(std::function<bool()> callback)
    {
        scheduler_.enqueue(
            ScheduledTask{.priority = TaskPriority::Background, .execute = std::move(callback)});
    }

    /// Dispatch with a specific TaskPriority for fine-grained control.
    void dispatch(TaskPriority priority, std::function<bool()> callback)
    {
        scheduler_.enqueue(ScheduledTask{.priority = priority, .execute = std::move(callback)});
    }

    /// Drain all input callbacks first, then run remaining work within budget.
    /// This is the per-frame entry point.
    void process_frame(std::chrono::microseconds frame_budget)
    {
        scheduler_.run_frame(frame_budget);
    }

    /// Check if any work is pending.
    [[nodiscard]] auto has_pending() const noexcept -> bool
    {
        return scheduler_.has_pending();
    }

    /// Number of pending tasks.
    [[nodiscard]] auto pending_count() const noexcept -> std::size_t
    {
        return scheduler_.pending_count();
    }

    /// Clear all pending tasks.
    void clear()
    {
        scheduler_.clear();
    }

    /// Access the underlying scheduler (for testing or advanced use).
    [[nodiscard]] auto scheduler() noexcept -> FrameScheduler&
    {
        return scheduler_;
    }

    /// Map EventCategory to TaskPriority.
    [[nodiscard]] static constexpr auto category_to_priority(EventCategory category) noexcept
        -> TaskPriority
    {
        switch (category)
        {
            case EventCategory::Input:
                return TaskPriority::Input;
            case EventCategory::Render:
                return TaskPriority::Paint;
            case EventCategory::Secondary:
                return TaskPriority::Background;
        }
        return TaskPriority::Background;
    }

private:
    FrameScheduler scheduler_;
};

} // namespace markamp::core
