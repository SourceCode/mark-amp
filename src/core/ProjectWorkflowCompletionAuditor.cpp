/// @file ProjectWorkflowCompletionAuditor.cpp
/// @brief V23 Phase 09 — ProjectWorkflowCompletionAuditor implementation.

#include "ProjectWorkflowCompletionAuditor.h"
#include <sstream>

namespace markamp::core
{

void ProjectWorkflowCompletionAuditor::add_item(ProjectCompletionItem item) { items_.push_back(std::move(item)); }
void ProjectWorkflowCompletionAuditor::add_items(std::vector<ProjectCompletionItem> items)
{ for (auto& i : items) items_.push_back(std::move(i)); }

auto ProjectWorkflowCompletionAuditor::item_count() const noexcept -> std::size_t { return items_.size(); }

auto ProjectWorkflowCompletionAuditor::items_by_area(ProjectCapabilityArea area) const
    -> std::vector<const ProjectCompletionItem*>
{
    std::vector<const ProjectCompletionItem*> r;
    for (const auto& i : items_) if (i.area == area) r.push_back(&i);
    return r;
}

auto ProjectWorkflowCompletionAuditor::complete_items() const -> std::vector<const ProjectCompletionItem*>
{
    std::vector<const ProjectCompletionItem*> r;
    for (const auto& i : items_) if (i.is_complete()) r.push_back(&i);
    return r;
}

auto ProjectWorkflowCompletionAuditor::incomplete_items() const -> std::vector<const ProjectCompletionItem*>
{
    std::vector<const ProjectCompletionItem*> r;
    for (const auto& i : items_) if (!i.is_complete()) r.push_back(&i);
    return r;
}

auto ProjectWorkflowCompletionAuditor::report() const -> ProjectCompletionReport
{
    ProjectCompletionReport rpt;
    rpt.total = items_.size();
    for (const auto& i : items_)
    {
        if (i.is_complete()) ++rpt.complete;
        else
        {
            ++rpt.incomplete;
            switch (i.area)
            {
            case ProjectCapabilityArea::kGitOperations:
            case ProjectCapabilityArea::kRepositoryDetection:
            case ProjectCapabilityArea::kSCMPanel:
            case ProjectCapabilityArea::kGitLog: ++rpt.git_gaps; break;
            case ProjectCapabilityArea::kBuildSystem:
            case ProjectCapabilityArea::kTaskRunner:
            case ProjectCapabilityArea::kProjectTemplates: ++rpt.build_gaps; break;
            case ProjectCapabilityArea::kDebugSession: ++rpt.debug_gaps; break;
            }
        }
    }
    return rpt;
}

void ProjectWorkflowCompletionAuditor::clear() { items_.clear(); }

auto ProjectWorkflowCompletionAuditor::export_json() const -> std::string
{
    auto r = report();
    std::ostringstream ss;
    ss << "{\n  \"total\": " << r.total << ",\n  \"complete\": " << r.complete
       << ",\n  \"incomplete\": " << r.incomplete << ",\n  \"coverage_pct\": " << r.coverage_pct()
       << ",\n  \"git_gaps\": " << r.git_gaps << ",\n  \"build_gaps\": " << r.build_gaps
       << ",\n  \"debug_gaps\": " << r.debug_gaps << "\n}\n";
    return ss.str();
}

auto ProjectWorkflowCompletionAuditor::export_markdown() const -> std::string
{
    auto r = report();
    std::ostringstream ss;
    ss << "# V23 Project Workflow Completion\n\n"
       << "**Coverage:** " << r.coverage_pct() << "% (" << r.complete << "/" << r.total << ")\n\n"
       << "| Area | Gaps |\n|---|---|\n"
       << "| Git | " << r.git_gaps << " |\n"
       << "| Build | " << r.build_gaps << " |\n"
       << "| Debug | " << r.debug_gaps << " |\n";
    return ss.str();
}

} // namespace markamp::core
