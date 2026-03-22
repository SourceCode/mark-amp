/// @file CanvasCompletionAuditor.cpp
/// @brief V23 Phase 07 — CanvasCompletionAuditor implementation.

#include "CanvasCompletionAuditor.h"
#include <sstream>

namespace markamp::core
{

void CanvasCompletionAuditor::add_item(CanvasCompletionItem item) { items_.push_back(std::move(item)); }
void CanvasCompletionAuditor::add_items(std::vector<CanvasCompletionItem> items)
{ for (auto& i : items) items_.push_back(std::move(i)); }

auto CanvasCompletionAuditor::item_count() const noexcept -> std::size_t { return items_.size(); }

auto CanvasCompletionAuditor::items_by_area(CanvasCapabilityArea area) const
    -> std::vector<const CanvasCompletionItem*>
{
    std::vector<const CanvasCompletionItem*> r;
    for (const auto& i : items_) if (i.area == area) r.push_back(&i);
    return r;
}

auto CanvasCompletionAuditor::complete_items() const -> std::vector<const CanvasCompletionItem*>
{
    std::vector<const CanvasCompletionItem*> r;
    for (const auto& i : items_) if (i.is_complete()) r.push_back(&i);
    return r;
}

auto CanvasCompletionAuditor::incomplete_items() const -> std::vector<const CanvasCompletionItem*>
{
    std::vector<const CanvasCompletionItem*> r;
    for (const auto& i : items_) if (!i.is_complete()) r.push_back(&i);
    return r;
}

auto CanvasCompletionAuditor::report() const -> CanvasCompletionReport
{
    CanvasCompletionReport rpt;
    rpt.total = items_.size();
    for (const auto& i : items_)
    {
        if (i.is_complete()) ++rpt.complete;
        else ++rpt.incomplete;
    }
    return rpt;
}

void CanvasCompletionAuditor::clear() { items_.clear(); }

auto CanvasCompletionAuditor::export_json() const -> std::string
{
    auto r = report();
    std::ostringstream ss;
    ss << "{\n  \"total\": " << r.total << ",\n  \"complete\": " << r.complete
       << ",\n  \"incomplete\": " << r.incomplete << ",\n  \"coverage_pct\": " << r.coverage_pct() << "\n}\n";
    return ss.str();
}

auto CanvasCompletionAuditor::export_markdown() const -> std::string
{
    auto r = report();
    std::ostringstream ss;
    ss << "# V23 Canvas Completion\n\n"
       << "**Coverage:** " << r.coverage_pct() << "% (" << r.complete << "/" << r.total << ")\n";
    return ss.str();
}

} // namespace markamp::core
