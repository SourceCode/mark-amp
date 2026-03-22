/// @file SubsystemDoneCriteria.h
/// @brief V24 P01-T02: Per-subsystem completion contracts.
///
/// Defines what "done" means for each MarkAmp subsystem — required tests,
/// smoke scenarios, readiness gates — and evaluates pass/fail with evidence.
#pragma once

#include "ExecutionLedger.h"

#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

/// A single evaluation criterion for a subsystem.
struct DoneCriterion
{
    std::string criterion_id;      ///< e.g. "artifact-lifecycle-smoke"
    std::string description;       ///< Human-readable description
    bool is_required{true};        ///< Must pass for subsystem signoff
    bool is_passed{false};         ///< Current evaluation state
    std::string evidence_link;     ///< Link to passing evidence

    [[nodiscard]] auto is_blocking() const noexcept -> bool
    {
        return is_required && !is_passed;
    }
};

/// Evaluation result for a subsystem.
struct SubsystemEvaluation
{
    Subsystem subsystem{Subsystem::kGovernance};
    int total_criteria{0};
    int passed{0};
    int failed{0};
    int required_passed{0};
    int required_total{0};
    std::vector<std::string> blocking_criteria;

    [[nodiscard]] auto is_signed_off() const noexcept -> bool
    {
        return required_passed == required_total && required_total > 0;
    }

    [[nodiscard]] auto pass_ratio() const noexcept -> double
    {
        return total_criteria > 0
            ? static_cast<double>(passed) / static_cast<double>(total_criteria)
            : 0.0;
    }

    [[nodiscard]] auto has_blockers() const noexcept -> bool
    {
        return !blocking_criteria.empty();
    }
};

/// Aggregated evaluation across all subsystems.
struct DoneCriteriaReport
{
    int total_subsystems{0};
    int signed_off{0};
    int blocked{0};
    std::vector<SubsystemEvaluation> evaluations;

    [[nodiscard]] auto all_signed_off() const noexcept -> bool
    {
        return signed_off == total_subsystems && total_subsystems > 0;
    }

    [[nodiscard]] auto signoff_ratio() const noexcept -> double
    {
        return total_subsystems > 0
            ? static_cast<double>(signed_off) / static_cast<double>(total_subsystems)
            : 0.0;
    }
};

/// Manages per-subsystem "done" criteria and evaluates readiness.
class SubsystemDoneCriteria
{
public:
    SubsystemDoneCriteria() = default;

    /// Add a criterion to a subsystem.
    void add_criterion(Subsystem subsystem, const DoneCriterion& criterion);

    /// Mark a criterion as passed.
    [[nodiscard]] auto mark_passed(Subsystem subsystem, const std::string& criterion_id,
                                    const std::string& evidence) -> bool;

    /// Mark a criterion as failed.
    [[nodiscard]] auto mark_failed(Subsystem subsystem, const std::string& criterion_id) -> bool;

    /// Get all criteria for a subsystem.
    [[nodiscard]] auto get_criteria(Subsystem subsystem) const -> std::vector<DoneCriterion>;

    /// Evaluate a single subsystem.
    [[nodiscard]] auto evaluate(Subsystem subsystem) const -> SubsystemEvaluation;

    /// Evaluate all subsystems.
    [[nodiscard]] auto evaluate_all() const -> DoneCriteriaReport;

    /// Total criteria count across all subsystems.
    [[nodiscard]] auto total_criteria() const noexcept -> int;

    /// Export as JSON.
    [[nodiscard]] auto export_json() const -> std::string;

    /// Export as Markdown.
    [[nodiscard]] auto export_markdown() const -> std::string;

    /// Clear all criteria.
    void clear();

private:
    std::unordered_map<int, std::vector<DoneCriterion>> criteria_;
};

} // namespace markamp::core
