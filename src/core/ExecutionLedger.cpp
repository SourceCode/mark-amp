/// @file ExecutionLedger.cpp
/// @brief V24 P01-T01: Implementation of the unified execution ledger.
#include "ExecutionLedger.h"

#include <sstream>

namespace markamp::core
{

auto ExecutionLedger::register_task(const LedgerTask& task) -> bool
{
    if (task.task_id.empty()) return false;
    auto [it, inserted] = tasks_.emplace(task.task_id, task);
    return inserted;
}

auto ExecutionLedger::complete_task(const std::string& task_id) -> bool
{
    auto it = tasks_.find(task_id);
    if (it == tasks_.end()) return false;
    it->second.status = LedgerTaskStatus::kComplete;
    return true;
}

auto ExecutionLedger::activate_task(const std::string& task_id) -> bool
{
    auto it = tasks_.find(task_id);
    if (it == tasks_.end()) return false;
    it->second.status = LedgerTaskStatus::kActive;
    return true;
}

auto ExecutionLedger::gate_task(const std::string& task_id) -> bool
{
    auto it = tasks_.find(task_id);
    if (it == tasks_.end()) return false;
    it->second.status = LedgerTaskStatus::kGated;
    return true;
}

auto ExecutionLedger::defer_task(const std::string& task_id) -> bool
{
    auto it = tasks_.find(task_id);
    if (it == tasks_.end()) return false;
    it->second.status = LedgerTaskStatus::kDeferred;
    return true;
}

auto ExecutionLedger::add_evidence(const std::string& task_id, const std::string& link) -> bool
{
    auto it = tasks_.find(task_id);
    if (it == tasks_.end()) return false;
    it->second.evidence_links.push_back(link);
    return true;
}

auto ExecutionLedger::get_task(const std::string& task_id) const -> const LedgerTask*
{
    auto it = tasks_.find(task_id);
    return it != tasks_.end() ? &it->second : nullptr;
}

auto ExecutionLedger::query_by_phase(const std::string& phase_id) const
    -> std::vector<const LedgerTask*>
{
    std::vector<const LedgerTask*> result;
    for (const auto& [id, task] : tasks_) {
        if (task.phase_id == phase_id) {
            result.push_back(&task);
        }
    }
    return result;
}

auto ExecutionLedger::query_by_subsystem(Subsystem subsystem) const
    -> std::vector<const LedgerTask*>
{
    std::vector<const LedgerTask*> result;
    for (const auto& [id, task] : tasks_) {
        if (task.subsystem == subsystem) {
            result.push_back(&task);
        }
    }
    return result;
}

auto ExecutionLedger::query_by_status(LedgerTaskStatus status) const
    -> std::vector<const LedgerTask*>
{
    std::vector<const LedgerTask*> result;
    for (const auto& [id, task] : tasks_) {
        if (task.status == status) {
            result.push_back(&task);
        }
    }
    return result;
}

void ExecutionLedger::collect_dependencies(const std::string& task_id,
                                           std::vector<std::string>& out,
                                           std::vector<std::string>& visited) const
{
    if (std::find(visited.begin(), visited.end(), task_id) != visited.end()) return;
    visited.push_back(task_id);

    auto it = tasks_.find(task_id);
    if (it == tasks_.end()) return;

    for (const auto& dep : it->second.dependencies) {
        out.push_back(dep);
        collect_dependencies(dep, out, visited);
    }
}

auto ExecutionLedger::dependency_chain(const std::string& task_id) const
    -> std::vector<std::string>
{
    std::vector<std::string> result;
    std::vector<std::string> visited;
    collect_dependencies(task_id, result, visited);
    return result;
}

auto ExecutionLedger::dependencies_met(const std::string& task_id) const -> bool
{
    auto it = tasks_.find(task_id);
    if (it == tasks_.end()) return false;

    for (const auto& dep_id : it->second.dependencies) {
        auto dep_it = tasks_.find(dep_id);
        if (dep_it == tasks_.end()) return false;
        if (!dep_it->second.is_terminal()) return false;
    }
    return true;
}

auto ExecutionLedger::report() const -> LedgerReport
{
    LedgerReport r;
    r.total = static_cast<int>(tasks_.size());
    for (const auto& [id, task] : tasks_) {
        switch (task.status) {
            case LedgerTaskStatus::kPending:  ++r.pending;  break;
            case LedgerTaskStatus::kActive:   ++r.active;   break;
            case LedgerTaskStatus::kComplete: ++r.complete; break;
            case LedgerTaskStatus::kGated:    ++r.gated;    break;
            case LedgerTaskStatus::kDeferred: ++r.deferred; break;
        }
    }
    return r;
}

auto ExecutionLedger::export_json() const -> std::string
{
    std::ostringstream ss;
    ss << "{\n  \"total\": " << tasks_.size() << ",\n  \"tasks\": [\n";
    bool first = true;
    for (const auto& [id, task] : tasks_) {
        if (!first) ss << ",\n";
        first = false;
        ss << "    {\"id\": \"" << task.task_id
           << "\", \"phase\": \"" << task.phase_id
           << "\", \"status\": \"" << ledger_status_label(task.status)
           << "\", \"subsystem\": \"" << subsystem_label(task.subsystem)
           << "\"}";
    }
    ss << "\n  ]\n}";
    return ss.str();
}

auto ExecutionLedger::export_markdown() const -> std::string
{
    std::ostringstream ss;
    ss << "# Execution Ledger\n\n";
    ss << "| Task | Phase | Status | Subsystem |\n";
    ss << "|------|-------|--------|----------|\n";
    for (const auto& [id, task] : tasks_) {
        ss << "| " << task.task_id
           << " | " << task.phase_id
           << " | " << ledger_status_label(task.status)
           << " | " << subsystem_label(task.subsystem)
           << " |\n";
    }
    return ss.str();
}

void ExecutionLedger::clear()
{
    tasks_.clear();
}

} // namespace markamp::core
