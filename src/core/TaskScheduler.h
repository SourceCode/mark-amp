/// @file TaskScheduler.h
/// @brief V9 Phase 40 — Cron-like task scheduling for workspace automation.
#pragma once

#include <chrono>
#include <cstdint>
#include <functional>
#include <string>
#include <vector>

namespace markamp::core
{

/// State of a scheduled task.
enum class ScheduledTaskState : uint8_t
{
    kPending = 0,   ///< Waiting to run
    kRunning = 1,   ///< Currently executing
    kCompleted = 2, ///< Finished successfully
    kFailed = 3,    ///< Finished with error
    kCancelled = 4, ///< Cancelled by user
    kPaused = 5,    ///< Temporarily paused
};

/// A scheduled task entry.
struct ScheduledTask
{
    std::string task_id;              ///< Unique identifier
    std::string name;                 ///< Human-readable name
    std::string description;          ///< What this task does
    std::function<bool()> execute_fn; ///< Task function (returns true on success)
    ScheduledTaskState state{ScheduledTaskState::kPending};

    int interval_seconds{0}; ///< 0 = one-shot, >0 = repeating
    bool repeat{false};      ///< Whether to repeat after completion
    int execution_count{0};  ///< How many times executed
    int max_executions{0};   ///< Max runs (0 = unlimited)

    std::chrono::system_clock::time_point created_at;
    std::chrono::system_clock::time_point last_run;
    std::chrono::system_clock::time_point next_run;
};

/// Manages scheduled tasks for workspace automation.
///
/// Provides a simple scheduler that tracks named tasks with intervals,
/// one-shot or repeating execution, and execution limits. Tasks are
/// checked via `tick()` which should be called periodically.
class TaskScheduler
{
public:
    TaskScheduler() = default;

    // ── Task management ───────────────────────────────────────────────
    void schedule(ScheduledTask task);
    auto cancel(const std::string& task_id) -> bool;
    auto pause(const std::string& task_id) -> bool;
    auto resume(const std::string& task_id) -> bool;
    [[nodiscard]] auto find_task(const std::string& task_id) const -> const ScheduledTask*;
    [[nodiscard]] auto all_tasks() const -> const std::vector<ScheduledTask>&;
    [[nodiscard]] auto task_count() const -> int;

    // ── Execution ─────────────────────────────────────────────────────
    /// Check all tasks and execute those that are due.
    /// Returns the number of tasks executed.
    auto tick() -> int;

    /// Execute a specific task immediately.
    auto execute_now(const std::string& task_id) -> bool;

    // ── Queries ───────────────────────────────────────────────────────
    [[nodiscard]] auto pending_tasks() const -> std::vector<const ScheduledTask*>;
    [[nodiscard]] auto running_tasks() const -> std::vector<const ScheduledTask*>;
    [[nodiscard]] auto completed_tasks() const -> std::vector<const ScheduledTask*>;

    void clear_completed();
    void clear_all();

private:
    std::vector<ScheduledTask> tasks_;

    auto find_mut(const std::string& task_id) -> ScheduledTask*;
    void execute_task(ScheduledTask& task);
};

} // namespace markamp::core
