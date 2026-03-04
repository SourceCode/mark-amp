#pragma once

/**
 * @file KeyboardWorkflowRunner.h
 * @brief Phase 39 Task 3: Keyboard workflow integration tests.
 *
 * Define keyboard workflows as step sequences, run with pass/fail
 * per step, and perform accessibility audit checks.
 */

#include <cstdint>
#include <functional>
#include <string>
#include <vector>

namespace markamp::ui
{

/// Result of a workflow step.
enum class StepResult : uint8_t
{
    kPass,
    kFail,
    kSkipped,
};

/// A single workflow step.
struct WorkflowStep
{
    std::string step_id;
    std::string description;
    std::string key_sequence;     ///< e.g. "Tab", "Enter", "Ctrl+S"
    std::function<bool()> verify; ///< Returns true if step succeeded

    /// Get step result name.
    [[nodiscard]] static auto result_name(StepResult result) -> std::string;
};

/// Workflow run result.
struct WorkflowResult
{
    std::string workflow_id;
    int total_steps{0};
    int passed{0};
    int failed{0};
    int skipped{0};
    std::vector<std::string> failure_step_ids;

    /// Check if workflow passed.
    [[nodiscard]] auto is_pass() const -> bool;
};

/**
 * @brief Runs keyboard workflow sequences and accessibility audits.
 */
class KeyboardWorkflowRunner
{
public:
    KeyboardWorkflowRunner() = default;

    // ── Workflow definition ────────────────────────────────────────

    /// Start defining a new workflow.
    void begin_workflow(const std::string& workflow_id);

    /// Add a step to the current workflow.
    void add_step(const WorkflowStep& step);

    /// Get step count for a workflow.
    [[nodiscard]] auto step_count(const std::string& workflow_id) const -> int;

    /// Get workflow count.
    [[nodiscard]] auto workflow_count() const -> int;

    // ── Execution ──────────────────────────────────────────────────

    /// Run a workflow and return results.
    [[nodiscard]] auto run(const std::string& workflow_id) -> WorkflowResult;

    /// Run all workflows.
    [[nodiscard]] auto run_all() -> std::vector<WorkflowResult>;

    // ── Accessibility audit ────────────────────────────────────────

    /// Register an accessibility check.
    void add_audit_check(const std::string& check_id,
                         const std::string& description,
                         std::function<bool()> check);

    /// Run all audit checks. Returns pass count.
    [[nodiscard]] auto run_audit() -> int;

    /// Get audit check count.
    [[nodiscard]] auto audit_check_count() const -> int;

private:
    struct Workflow
    {
        std::string workflow_id;
        std::vector<WorkflowStep> steps;
    };

    struct AuditCheck
    {
        std::string check_id;
        std::string description;
        std::function<bool()> check;
    };

    std::vector<Workflow> workflows_;
    std::vector<AuditCheck> audit_checks_;
    std::string current_workflow_id_;
};

} // namespace markamp::ui
