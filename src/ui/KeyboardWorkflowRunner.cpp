#include "KeyboardWorkflowRunner.h"

namespace markamp::ui
{

auto WorkflowStep::result_name(StepResult result) -> std::string
{
    switch (result)
    {
        case StepResult::kPass:
            return "pass";
        case StepResult::kFail:
            return "fail";
        case StepResult::kSkipped:
            return "skipped";
    }
    return "unknown";
}

auto WorkflowResult::is_pass() const -> bool
{
    return failed == 0;
}

void KeyboardWorkflowRunner::begin_workflow(const std::string& workflow_id)
{
    current_workflow_id_ = workflow_id;
    Workflow workflow;
    workflow.workflow_id = workflow_id;
    workflows_.push_back(workflow);
}

void KeyboardWorkflowRunner::add_step(const WorkflowStep& step)
{
    for (auto& workflow : workflows_)
    {
        if (workflow.workflow_id == current_workflow_id_)
        {
            workflow.steps.push_back(step);
            return;
        }
    }
}

auto KeyboardWorkflowRunner::step_count(const std::string& workflow_id) const -> int
{
    for (const auto& workflow : workflows_)
    {
        if (workflow.workflow_id == workflow_id)
        {
            return static_cast<int>(workflow.steps.size());
        }
    }
    return 0;
}

auto KeyboardWorkflowRunner::workflow_count() const -> int
{
    return static_cast<int>(workflows_.size());
}

auto KeyboardWorkflowRunner::run(const std::string& workflow_id) -> WorkflowResult
{
    WorkflowResult result;
    result.workflow_id = workflow_id;

    for (const auto& workflow : workflows_)
    {
        if (workflow.workflow_id == workflow_id)
        {
            result.total_steps = static_cast<int>(workflow.steps.size());
            for (const auto& step : workflow.steps)
            {
                if (step.verify && step.verify())
                {
                    ++result.passed;
                }
                else if (!step.verify)
                {
                    ++result.skipped;
                }
                else
                {
                    ++result.failed;
                    result.failure_step_ids.push_back(step.step_id);
                }
            }
            return result;
        }
    }

    return result;
}

auto KeyboardWorkflowRunner::run_all() -> std::vector<WorkflowResult>
{
    std::vector<WorkflowResult> results;
    for (const auto& workflow : workflows_)
    {
        results.push_back(run(workflow.workflow_id));
    }
    return results;
}

void KeyboardWorkflowRunner::add_audit_check(const std::string& check_id,
                                             const std::string& description,
                                             std::function<bool()> check)
{
    AuditCheck audit;
    audit.check_id = check_id;
    audit.description = description;
    audit.check = std::move(check);
    audit_checks_.push_back(std::move(audit));
}

auto KeyboardWorkflowRunner::run_audit() -> int
{
    int pass_count = 0;
    for (const auto& check : audit_checks_)
    {
        if (check.check && check.check())
        {
            ++pass_count;
        }
    }
    return pass_count;
}

auto KeyboardWorkflowRunner::audit_check_count() const -> int
{
    return static_cast<int>(audit_checks_.size());
}

} // namespace markamp::ui
