/// @file ProjectWorkflowCompletionAuditor.h
/// @brief V23 Phase 09 — Git, source control, build, run, debug, and project workflow completion auditor.
///
/// Audits that Git/SCM operations, build/debug surfaces, and project workflows
/// have real repository-backed implementations rather than synthetic state.
#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::core
{

enum class ProjectCapabilityArea : uint8_t
{
    kGitOperations,       ///< Status, diff, stage, commit, branch
    kRepositoryDetection, ///< Repo open, detect, workspace integration
    kBuildSystem,         ///< Build task running, output
    kDebugSession,        ///< Debug launch, breakpoints, stepping
    kTaskRunner,          ///< Task runner integration
    kProjectTemplates,    ///< Project scaffolding
    kSCMPanel,            ///< Source control panel wiring
    kGitLog,              ///< Git log, blame, history
};

[[nodiscard]] constexpr auto project_capability_label(ProjectCapabilityArea area) -> const char*
{
    switch (area)
    {
    case ProjectCapabilityArea::kGitOperations:       return "GitOperations";
    case ProjectCapabilityArea::kRepositoryDetection: return "RepositoryDetection";
    case ProjectCapabilityArea::kBuildSystem:         return "BuildSystem";
    case ProjectCapabilityArea::kDebugSession:        return "DebugSession";
    case ProjectCapabilityArea::kTaskRunner:          return "TaskRunner";
    case ProjectCapabilityArea::kProjectTemplates:    return "ProjectTemplates";
    case ProjectCapabilityArea::kSCMPanel:            return "SCMPanel";
    case ProjectCapabilityArea::kGitLog:              return "GitLog";
    }
    return "Unknown";
}

struct ProjectCompletionItem
{
    ProjectCapabilityArea area{ProjectCapabilityArea::kGitOperations};
    std::string feature_name;
    bool is_real_backend{false};     ///< Not synthetic/simulated
    bool has_error_handling{false};
    bool has_shell_integration{false};
    std::string evidence_file;
    int evidence_line{0};

    [[nodiscard]] auto is_complete() const noexcept -> bool
    { return is_real_backend && has_error_handling; }
};

struct ProjectCompletionReport
{
    std::size_t total{0};
    std::size_t complete{0};
    std::size_t incomplete{0};
    std::size_t git_gaps{0};
    std::size_t build_gaps{0};
    std::size_t debug_gaps{0};

    [[nodiscard]] auto has_gaps() const noexcept -> bool { return incomplete > 0; }
    [[nodiscard]] auto coverage_pct() const noexcept -> double
    { return total > 0 ? (static_cast<double>(complete) / static_cast<double>(total)) * 100.0 : 100.0; }
};

class ProjectWorkflowCompletionAuditor
{
public:
    ProjectWorkflowCompletionAuditor() = default;

    void add_item(ProjectCompletionItem item);
    void add_items(std::vector<ProjectCompletionItem> items);

    [[nodiscard]] auto item_count() const noexcept -> std::size_t;
    [[nodiscard]] auto items_by_area(ProjectCapabilityArea area) const
        -> std::vector<const ProjectCompletionItem*>;
    [[nodiscard]] auto complete_items() const -> std::vector<const ProjectCompletionItem*>;
    [[nodiscard]] auto incomplete_items() const -> std::vector<const ProjectCompletionItem*>;

    [[nodiscard]] auto report() const -> ProjectCompletionReport;
    void clear();

    [[nodiscard]] auto export_json() const -> std::string;
    [[nodiscard]] auto export_markdown() const -> std::string;

private:
    std::vector<ProjectCompletionItem> items_;
};

} // namespace markamp::core
