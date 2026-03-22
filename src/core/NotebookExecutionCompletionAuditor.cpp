/// @file NotebookExecutionCompletionAuditor.cpp
/// @brief V23 Phase 06 — NotebookExecutionCompletionAuditor implementation.

#include "NotebookExecutionCompletionAuditor.h"
#include <sstream>

namespace markamp::core
{

void NotebookExecutionCompletionAuditor::add_item(NotebookCompletionItem item) { items_.push_back(std::move(item)); }
void NotebookExecutionCompletionAuditor::add_items(std::vector<NotebookCompletionItem> items)
{ for (auto& i : items) items_.push_back(std::move(i)); }

auto NotebookExecutionCompletionAuditor::item_count() const noexcept -> std::size_t { return items_.size(); }

auto NotebookExecutionCompletionAuditor::items_by_area(NotebookCapabilityArea area) const
    -> std::vector<const NotebookCompletionItem*>
{
    std::vector<const NotebookCompletionItem*> r;
    for (const auto& i : items_) if (i.area == area) r.push_back(&i);
    return r;
}

auto NotebookExecutionCompletionAuditor::complete_items() const -> std::vector<const NotebookCompletionItem*>
{
    std::vector<const NotebookCompletionItem*> r;
    for (const auto& i : items_) if (i.is_complete()) r.push_back(&i);
    return r;
}

auto NotebookExecutionCompletionAuditor::incomplete_items() const -> std::vector<const NotebookCompletionItem*>
{
    std::vector<const NotebookCompletionItem*> r;
    for (const auto& i : items_) if (!i.is_complete()) r.push_back(&i);
    return r;
}

auto NotebookExecutionCompletionAuditor::report() const -> NotebookCompletionReport
{
    NotebookCompletionReport rpt;
    rpt.total = items_.size();
    for (const auto& i : items_)
    {
        if (i.is_complete()) ++rpt.complete;
        else
        {
            ++rpt.incomplete;
            switch (i.area)
            {
            case NotebookCapabilityArea::kKernelLifecycle:
            case NotebookCapabilityArea::kKernelTransport: ++rpt.kernel_gaps; break;
            case NotebookCapabilityArea::kCellExecution:
            case NotebookCapabilityArea::kOutputRendering:
            case NotebookCapabilityArea::kVariableInspector:
            case NotebookCapabilityArea::kExecutionHistory: ++rpt.execution_gaps; break;
            case NotebookCapabilityArea::kAICompletion:
            case NotebookCapabilityArea::kAIDocumentGen: ++rpt.ai_gaps; break;
            }
        }
    }
    return rpt;
}

void NotebookExecutionCompletionAuditor::clear() { items_.clear(); }

auto NotebookExecutionCompletionAuditor::export_json() const -> std::string
{
    auto r = report();
    std::ostringstream ss;
    ss << "{\n  \"total\": " << r.total << ",\n  \"complete\": " << r.complete
       << ",\n  \"incomplete\": " << r.incomplete << ",\n  \"coverage_pct\": " << r.coverage_pct()
       << ",\n  \"kernel_gaps\": " << r.kernel_gaps << ",\n  \"execution_gaps\": " << r.execution_gaps
       << ",\n  \"ai_gaps\": " << r.ai_gaps << "\n}\n";
    return ss.str();
}

auto NotebookExecutionCompletionAuditor::export_markdown() const -> std::string
{
    auto r = report();
    std::ostringstream ss;
    ss << "# V23 Notebook Execution Completion\n\n"
       << "**Coverage:** " << r.coverage_pct() << "% (" << r.complete << "/" << r.total << ")\n\n"
       << "| Area | Gaps |\n|---|---|\n"
       << "| Kernel | " << r.kernel_gaps << " |\n"
       << "| Execution | " << r.execution_gaps << " |\n"
       << "| AI | " << r.ai_gaps << " |\n";
    return ss.str();
}

} // namespace markamp::core
