/// @file SpecializedSurfaceCompletionAuditor.cpp
/// @brief V23 Phase 08 — SpecializedSurfaceCompletionAuditor implementation.

#include "SpecializedSurfaceCompletionAuditor.h"
#include <sstream>

namespace markamp::core
{

void SpecializedSurfaceCompletionAuditor::add_item(SurfaceCompletionItem item) { items_.push_back(std::move(item)); }
void SpecializedSurfaceCompletionAuditor::add_items(std::vector<SurfaceCompletionItem> items)
{ for (auto& i : items) items_.push_back(std::move(i)); }

auto SpecializedSurfaceCompletionAuditor::item_count() const noexcept -> std::size_t { return items_.size(); }

auto SpecializedSurfaceCompletionAuditor::items_by_area(SurfaceCapabilityArea area) const
    -> std::vector<const SurfaceCompletionItem*>
{
    std::vector<const SurfaceCompletionItem*> r;
    for (const auto& i : items_) if (i.area == area) r.push_back(&i);
    return r;
}

auto SpecializedSurfaceCompletionAuditor::complete_items() const -> std::vector<const SurfaceCompletionItem*>
{
    std::vector<const SurfaceCompletionItem*> r;
    for (const auto& i : items_) if (i.is_complete()) r.push_back(&i);
    return r;
}

auto SpecializedSurfaceCompletionAuditor::incomplete_items() const -> std::vector<const SurfaceCompletionItem*>
{
    std::vector<const SurfaceCompletionItem*> r;
    for (const auto& i : items_) if (!i.is_complete()) r.push_back(&i);
    return r;
}

auto SpecializedSurfaceCompletionAuditor::report() const -> SurfaceCompletionReport
{
    SurfaceCompletionReport rpt;
    rpt.total = items_.size();
    for (const auto& i : items_)
    { if (i.is_complete()) ++rpt.complete; else ++rpt.incomplete; }
    return rpt;
}

void SpecializedSurfaceCompletionAuditor::clear() { items_.clear(); }

auto SpecializedSurfaceCompletionAuditor::export_json() const -> std::string
{
    auto r = report();
    std::ostringstream ss;
    ss << "{\n  \"total\": " << r.total << ",\n  \"complete\": " << r.complete
       << ",\n  \"incomplete\": " << r.incomplete << ",\n  \"coverage_pct\": " << r.coverage_pct() << "\n}\n";
    return ss.str();
}

auto SpecializedSurfaceCompletionAuditor::export_markdown() const -> std::string
{
    auto r = report();
    std::ostringstream ss;
    ss << "# V23 Specialized Surface Completion\n\n"
       << "**Coverage:** " << r.coverage_pct() << "% (" << r.complete << "/" << r.total << ")\n";
    return ss.str();
}

} // namespace markamp::core
