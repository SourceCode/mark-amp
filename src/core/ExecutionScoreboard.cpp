/// @file ExecutionScoreboard.cpp
/// @brief V20 P10-T04/T05/T06: Execution scoreboard implementation.

#include "ExecutionScoreboard.h"

#include "Events.h"
#include "Logger.h"

namespace markamp::core
{

ExecutionScoreboard::ExecutionScoreboard(EventBus& bus)
    : event_bus_(bus)
{
    register_defaults();
}

void ExecutionScoreboard::set_phase(const PhaseProgress& progress)
{
    phases_[progress.phase_id] = progress;

    events::PhaseProgressUpdatedEvent evt;
    evt.phase_id = progress.phase_id;
    evt.completion_pct = progress.completion_pct();
    evt.is_complete = progress.is_complete();
    event_bus_.publish(evt);

    MARKAMP_LOG_DEBUG("Phase progress: {} {:.0f}% ({}/{})", progress.phase_id,
                     progress.completion_pct(), progress.tasks_complete, progress.tasks_total);
}

auto ExecutionScoreboard::phase(const std::string& phase_id) const -> const PhaseProgress*
{
    auto it = phases_.find(phase_id);
    return it != phases_.end() ? &it->second : nullptr;
}

auto ExecutionScoreboard::all_phases() const -> std::vector<PhaseProgress>
{
    std::vector<PhaseProgress> result;
    result.reserve(phases_.size());
    for (const auto& [id, progress] : phases_)
    {
        result.push_back(progress);
    }
    return result;
}

void ExecutionScoreboard::record_performance(const std::string& workflow, double measured_ms,
                                               double budget_ms)
{
    PerformanceBudget budget;
    budget.workflow = workflow;
    budget.measured_ms = measured_ms;
    budget.budget_ms = budget_ms;
    budget.within_budget = measured_ms <= budget_ms;
    performance_.push_back(budget);

    events::PerformanceMeasuredEvent evt;
    evt.workflow = workflow;
    evt.measured_ms = measured_ms;
    evt.budget_ms = budget_ms;
    evt.within_budget = budget.within_budget;
    event_bus_.publish(evt);

    MARKAMP_LOG_DEBUG("Performance: {} {}ms (budget {}ms) {}", workflow, measured_ms, budget_ms,
                     budget.within_budget ? "OK" : "OVER");
}

auto ExecutionScoreboard::all_performance() const -> std::vector<PerformanceBudget>
{
    return performance_;
}

void ExecutionScoreboard::set_exit_criterion(const ExitCriterion& criterion)
{
    // Update existing or add new
    for (auto& ec : exit_criteria_)
    {
        if (ec.criterion_id == criterion.criterion_id)
        {
            ec = criterion;
            return;
        }
    }
    exit_criteria_.push_back(criterion);
}

auto ExecutionScoreboard::all_exit_criteria() const -> std::vector<ExitCriterion>
{
    return exit_criteria_;
}

auto ExecutionScoreboard::unmet_criteria_count() const -> int
{
    int count = 0;
    for (const auto& ec : exit_criteria_)
    {
        if (!ec.met)
        {
            ++count;
        }
    }
    return count;
}

auto ExecutionScoreboard::overall_completion() const -> double
{
    if (phases_.empty())
    {
        return 0.0;
    }

    double total = 0.0;
    for (const auto& [id, progress] : phases_)
    {
        total += progress.completion_pct();
    }
    return total / static_cast<double>(phases_.size());
}

auto ExecutionScoreboard::ready_for_exit() const -> bool
{
    return unmet_criteria_count() == 0;
}

void ExecutionScoreboard::register_defaults()
{
    // Register V20 phases
    const std::vector<std::pair<std::string, std::string>> phase_info = {
        {"P01", "Artifact Creation Spine & Shell Ownership"},
        {"P02", "File Creation & Text Artifact Lifecycle"},
        {"P03", "Notebook Creation & Computational Document Flows"},
        {"P04", "Canvas Creation & Board Workbench Integration"},
        {"P05", "Persistence, Autosave, Restore & Recovery"},
        {"P06", "Commands, Context, Focus & Surface Entry Points"},
        {"P07", "Syntax Highlighting & Language Presentation"},
        {"P08", "Editor, Notebook, Canvas Rendering & Content Styling"},
        {"P09", "Premium Visual System Polish & MUI Icon Completion"},
        {"P10", "Diagnostics, Validation Harnesses & Consolidation"},
    };

    for (const auto& [id, name] : phase_info)
    {
        PhaseProgress pp;
        pp.phase_id = id;
        pp.phase_name = name;
        pp.tasks_total = 6;
        pp.improvements_total = (id == "P01" || id == "P07" || id == "P08" || id == "P09" || id == "P10") ? 210 : 204;
        phases_[id] = pp;
    }

    // Register exit criteria
    set_exit_criterion({"exit.file-create", "Cmd+N creates a text file successfully", false, ""});
    set_exit_criterion({"exit.notebook-create", "Notebook creation works end-to-end", false, ""});
    set_exit_criterion({"exit.canvas-create", "Canvas creation works end-to-end", false, ""});
    set_exit_criterion({"exit.save-restore", "Save and restore cycle is reliable", false, ""});
    set_exit_criterion({"exit.syntax-quality", "Syntax highlighting is IDE-grade", false, ""});
    set_exit_criterion({"exit.visual-polish", "Shell visual quality is premium", false, ""});
    set_exit_criterion({"exit.smoke-pass", "All smoke tests pass", false, ""});

    MARKAMP_LOG_DEBUG("Execution scoreboard initialized: {} phases, {} exit criteria",
                     phase_count(), static_cast<int>(exit_criteria_.size()));
}

} // namespace markamp::core
