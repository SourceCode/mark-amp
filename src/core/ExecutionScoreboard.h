/// @file ExecutionScoreboard.h
/// @brief V20 P10-T04/T05/T06: V20 execution scoreboard and exit criteria.
///
/// Tracks phase progress, blocker status, workflow performance budgets,
/// and release-gate outcomes for the v20 wave.
#pragma once

#include "EventBus.h"

#include <chrono>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

/// Phase completion status.
enum class PhaseStatus
{
    kNotStarted,
    kInProgress,
    kComplete,
    kBlocked
};

/// Phase progress entry.
struct PhaseProgress
{
    std::string phase_id;           ///< e.g. "P01"
    std::string phase_name;         ///< Human-readable name
    PhaseStatus status{PhaseStatus::kNotStarted};
    int tasks_total{0};
    int tasks_complete{0};
    int improvements_total{0};

    [[nodiscard]] auto completion_pct() const noexcept -> double
    {
        return tasks_total > 0
                   ? static_cast<double>(tasks_complete) / static_cast<double>(tasks_total) * 100.0
                   : 0.0;
    }

    [[nodiscard]] auto is_complete() const noexcept -> bool
    {
        return status == PhaseStatus::kComplete;
    }
};

/// Performance budget for a workflow.
struct PerformanceBudget
{
    std::string workflow;           ///< e.g. "file.create", "file.save"
    double budget_ms{100.0};        ///< Target latency
    double measured_ms{0.0};        ///< Actual measured latency
    bool within_budget{true};

    [[nodiscard]] auto exceeded() const noexcept -> bool
    {
        return measured_ms > budget_ms;
    }
};

/// Exit criteria check result.
struct ExitCriterion
{
    std::string criterion_id;       ///< e.g. "exit.artifact-create"
    std::string description;        ///< What must be true
    bool met{false};                ///< Whether criterion is satisfied
    std::string evidence;           ///< Reference to evidence (test, smoke, trace)
};

/// V20 execution scoreboard — tracks progress and exit criteria.
class ExecutionScoreboard
{
public:
    explicit ExecutionScoreboard(EventBus& bus);

    /// Set phase progress.
    void set_phase(const PhaseProgress& progress);

    /// Get phase progress.
    [[nodiscard]] auto phase(const std::string& phase_id) const -> const PhaseProgress*;

    /// All phases.
    [[nodiscard]] auto all_phases() const -> std::vector<PhaseProgress>;

    /// Record a performance measurement.
    void record_performance(const std::string& workflow, double measured_ms, double budget_ms);

    /// All performance budgets.
    [[nodiscard]] auto all_performance() const -> std::vector<PerformanceBudget>;

    /// Set an exit criterion.
    void set_exit_criterion(const ExitCriterion& criterion);

    /// All exit criteria.
    [[nodiscard]] auto all_exit_criteria() const -> std::vector<ExitCriterion>;

    /// Unmet exit criteria count.
    [[nodiscard]] auto unmet_criteria_count() const -> int;

    /// Overall V20 completion percentage.
    [[nodiscard]] auto overall_completion() const -> double;

    /// Whether V20 exit criteria are all met.
    [[nodiscard]] auto ready_for_exit() const -> bool;

    /// Total registered phases.
    [[nodiscard]] auto phase_count() const noexcept -> int
    {
        return static_cast<int>(phases_.size());
    }

    /// Populate default V20 phases and exit criteria.
    void register_defaults();

private:
    EventBus& event_bus_;
    std::unordered_map<std::string, PhaseProgress> phases_;
    std::vector<PerformanceBudget> performance_;
    std::vector<ExitCriterion> exit_criteria_;
};

} // namespace markamp::core
