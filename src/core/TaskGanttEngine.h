/// @file TaskGanttEngine.h
/// @brief V9 Phase 23 – Gantt timeline data engine (pure logic, no UI).
/// Generates timeline bars from tasks, supports milestones, time scales,
/// and dependency-based critical path computation.

#pragma once

#include "core/TaskService.h"

#include <chrono>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace markamp::core
{

// ============================================================================
// Gantt types
// ============================================================================

/// A single bar on the Gantt timeline.
struct GanttBar
{
    std::string task_id;
    std::string label;
    std::optional<std::chrono::year_month_day> start_date;
    std::optional<std::chrono::year_month_day> end_date;
    double progress{0.0}; ///< 0.0 to 1.0
    TaskPriority priority{TaskPriority::kNone};
    std::string project;
    std::vector<std::string> dependencies; ///< Task IDs this depends on
    bool is_milestone{false};              ///< Point-in-time vs range
};

/// Gantt timeline zoom level.
enum class GanttTimeScale : uint8_t
{
    kDay,
    kWeek,
    kMonth
};

// ============================================================================
// TaskGanttEngine
// ============================================================================

class TaskGanttEngine
{
public:
    TaskGanttEngine() = default;

    /// Load tasks into the Gantt engine.
    auto set_tasks(const std::vector<Task>& tasks) -> void;

    /// Set the visible date range.
    auto set_time_range(std::chrono::year_month_day start, std::chrono::year_month_day end) -> void;

    /// Get all task bars within the time range.
    [[nodiscard]] auto get_bars() const -> std::vector<GanttBar>;

    /// Get milestones (tasks with only a due date, no scheduled start).
    [[nodiscard]] auto get_milestones() const -> std::vector<GanttBar>;

    /// Set the time scale (zoom level).
    auto set_time_scale(GanttTimeScale scale) -> void;

    /// Get the current time scale.
    [[nodiscard]] auto time_scale() const -> GanttTimeScale;

    /// Get task IDs on the longest dependency chain.
    [[nodiscard]] auto get_critical_path() const -> std::vector<std::string>;

    /// Get the total number of bars.
    [[nodiscard]] auto bar_count() const -> int;

    /// Get the visible time range.
    [[nodiscard]] auto start_date() const -> std::chrono::year_month_day;
    [[nodiscard]] auto end_date() const -> std::chrono::year_month_day;

private:
    std::vector<GanttBar> bars_;
    std::chrono::year_month_day range_start_{std::chrono::year{2024} / std::chrono::January /
                                             std::chrono::day{1}};
    std::chrono::year_month_day range_end_{std::chrono::year{2099} / std::chrono::December /
                                           std::chrono::day{31}};
    GanttTimeScale time_scale_{GanttTimeScale::kWeek};

    /// Convert a Task to a GanttBar.
    [[nodiscard]] static auto task_to_bar(const Task& task) -> GanttBar;

    /// Compute progress based on task status.
    [[nodiscard]] static auto status_to_progress(TaskStatus status) -> double;

    /// Check if a bar falls within the visible time range.
    [[nodiscard]] auto is_in_range(const GanttBar& bar) const -> bool;
};

} // namespace markamp::core
