/// @file ExecutionLedger.h
/// @brief V24 P01-T01: Single, de-duplicated execution ledger for the v24 program.
///
/// Provides a unified task registry where each entry tracks: task ID, phase,
/// status (pending/active/complete/gated), subsystem owner, dependencies,
/// and evidence links.  Eliminates duplicate task entries from prior planning
/// generations (v18–v23) and serves as the single source of truth for
/// program-execution status.
#pragma once

#include <algorithm>
#include <chrono>
#include <cstddef>
#include <functional>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

/// Status of a single ledger task.
enum class LedgerTaskStatus
{
    kPending,     ///< Not yet started
    kActive,      ///< Work in progress
    kComplete,    ///< Finished and validated
    kGated,       ///< Intentionally hidden from release scope
    kDeferred     ///< Deferred to a future cycle
};

/// Returns a human-readable label for a LedgerTaskStatus.
[[nodiscard]] constexpr auto ledger_status_label(LedgerTaskStatus s) noexcept -> const char*
{
    switch (s) {
        case LedgerTaskStatus::kPending:  return "Pending";
        case LedgerTaskStatus::kActive:   return "Active";
        case LedgerTaskStatus::kComplete: return "Complete";
        case LedgerTaskStatus::kGated:    return "Gated";
        case LedgerTaskStatus::kDeferred: return "Deferred";
    }
    return "Unknown";
}

/// Subsystem that owns a task.
enum class Subsystem
{
    kArtifactLifecycle,
    kPersistence,
    kShellCommands,
    kPanels,
    kEditor,
    kSearch,
    kSourceControl,
    kSettings,
    kCloudSync,
    kRendering,
    kPlugins,
    kVisualSystem,
    kWorkspace,
    kBuildTerminal,
    kAdvancedDomains,
    kGovernance,
    kValidation,
    kRelease
};

/// Returns a human-readable label for a Subsystem.
[[nodiscard]] constexpr auto subsystem_label(Subsystem s) noexcept -> const char*
{
    switch (s) {
        case Subsystem::kArtifactLifecycle: return "ArtifactLifecycle";
        case Subsystem::kPersistence:       return "Persistence";
        case Subsystem::kShellCommands:     return "ShellCommands";
        case Subsystem::kPanels:            return "Panels";
        case Subsystem::kEditor:            return "Editor";
        case Subsystem::kSearch:            return "Search";
        case Subsystem::kSourceControl:     return "SourceControl";
        case Subsystem::kSettings:          return "Settings";
        case Subsystem::kCloudSync:         return "CloudSync";
        case Subsystem::kRendering:         return "Rendering";
        case Subsystem::kPlugins:           return "Plugins";
        case Subsystem::kVisualSystem:      return "VisualSystem";
        case Subsystem::kWorkspace:         return "Workspace";
        case Subsystem::kBuildTerminal:     return "BuildTerminal";
        case Subsystem::kAdvancedDomains:   return "AdvancedDomains";
        case Subsystem::kGovernance:        return "Governance";
        case Subsystem::kValidation:        return "Validation";
        case Subsystem::kRelease:           return "Release";
    }
    return "Unknown";
}

/// A single task entry in the execution ledger.
struct LedgerTask
{
    std::string task_id;                        ///< e.g. "P01-T01"
    std::string phase_id;                       ///< e.g. "P01"
    std::string title;                          ///< Human-readable title
    LedgerTaskStatus status{LedgerTaskStatus::kPending};
    Subsystem subsystem{Subsystem::kGovernance};
    std::vector<std::string> dependencies;      ///< Task IDs this depends on
    std::vector<std::string> evidence_links;    ///< Paths or URLs proving completion
    int priority{2};                            ///< 0 = P0, 1 = P1, 2 = P2

    [[nodiscard]] auto is_complete() const noexcept -> bool
    {
        return status == LedgerTaskStatus::kComplete;
    }

    [[nodiscard]] auto is_actionable() const noexcept -> bool
    {
        return status == LedgerTaskStatus::kPending || status == LedgerTaskStatus::kActive;
    }

    [[nodiscard]] auto is_terminal() const noexcept -> bool
    {
        return status == LedgerTaskStatus::kComplete ||
               status == LedgerTaskStatus::kGated ||
               status == LedgerTaskStatus::kDeferred;
    }

    [[nodiscard]] auto has_dependencies() const noexcept -> bool
    {
        return !dependencies.empty();
    }

    [[nodiscard]] auto dependency_count() const noexcept -> int
    {
        return static_cast<int>(dependencies.size());
    }
};

/// Aggregated report from the execution ledger.
struct LedgerReport
{
    int total{0};
    int pending{0};
    int active{0};
    int complete{0};
    int gated{0};
    int deferred{0};

    [[nodiscard]] auto has_gaps() const noexcept -> bool
    {
        return pending > 0 || active > 0;
    }

    [[nodiscard]] auto completion_ratio() const noexcept -> double
    {
        return total > 0 ? static_cast<double>(complete) / static_cast<double>(total) : 0.0;
    }

    [[nodiscard]] auto terminal_count() const noexcept -> int
    {
        return complete + gated + deferred;
    }
};

/// Single, de-duplicated execution ledger for the v24 program.
///
/// Tracks every task across all 20 phases, supports dependency queries,
/// subsystem filtering, and report generation.
class ExecutionLedger
{
public:
    ExecutionLedger() = default;

    /// Register a task. Returns false if task_id already exists.
    [[nodiscard]] auto register_task(const LedgerTask& task) -> bool;

    /// Mark a task complete. Returns false if task_id not found.
    [[nodiscard]] auto complete_task(const std::string& task_id) -> bool;

    /// Mark a task as active. Returns false if task_id not found.
    [[nodiscard]] auto activate_task(const std::string& task_id) -> bool;

    /// Mark a task as gated. Returns false if task_id not found.
    [[nodiscard]] auto gate_task(const std::string& task_id) -> bool;

    /// Mark a task as deferred. Returns false if task_id not found.
    [[nodiscard]] auto defer_task(const std::string& task_id) -> bool;

    /// Add evidence link to a task. Returns false if task_id not found.
    [[nodiscard]] auto add_evidence(const std::string& task_id, const std::string& link) -> bool;

    /// Get a task by ID. Returns nullptr if not found.
    [[nodiscard]] auto get_task(const std::string& task_id) const -> const LedgerTask*;

    /// Total number of tasks.
    [[nodiscard]] auto task_count() const noexcept -> int
    {
        return static_cast<int>(tasks_.size());
    }

    /// Query all tasks in a specific phase.
    [[nodiscard]] auto query_by_phase(const std::string& phase_id) const -> std::vector<const LedgerTask*>;

    /// Query all tasks for a specific subsystem.
    [[nodiscard]] auto query_by_subsystem(Subsystem subsystem) const -> std::vector<const LedgerTask*>;

    /// Query all tasks with a specific status.
    [[nodiscard]] auto query_by_status(LedgerTaskStatus status) const -> std::vector<const LedgerTask*>;

    /// Get the dependency chain for a task (recursive transitive closure).
    [[nodiscard]] auto dependency_chain(const std::string& task_id) const -> std::vector<std::string>;

    /// Check if all dependencies of a task are in terminal state.
    [[nodiscard]] auto dependencies_met(const std::string& task_id) const -> bool;

    /// Generate an aggregated report.
    [[nodiscard]] auto report() const -> LedgerReport;

    /// Export as JSON string.
    [[nodiscard]] auto export_json() const -> std::string;

    /// Export as Markdown string.
    [[nodiscard]] auto export_markdown() const -> std::string;

    /// Clear all tasks.
    void clear();

private:
    std::unordered_map<std::string, LedgerTask> tasks_;

    void collect_dependencies(const std::string& task_id, std::vector<std::string>& out,
                              std::vector<std::string>& visited) const;
};

} // namespace markamp::core
