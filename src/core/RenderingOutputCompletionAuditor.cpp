/// @file RenderingOutputCompletionAuditor.cpp
#include "RenderingOutputCompletionAuditor.h"
#include <sstream>
namespace markamp::core {
void RenderingOutputCompletionAuditor::add_item(RenderingCompletionItem item) { items_.push_back(std::move(item)); }
auto RenderingOutputCompletionAuditor::item_count() const noexcept -> std::size_t { return items_.size(); }
auto RenderingOutputCompletionAuditor::items_by_area(RenderingArea a) const -> std::vector<const RenderingCompletionItem*> {
    std::vector<const RenderingCompletionItem*> r; for (const auto& i : items_) if (i.area == a) r.push_back(&i); return r; }
auto RenderingOutputCompletionAuditor::complete_items() const -> std::vector<const RenderingCompletionItem*> {
    std::vector<const RenderingCompletionItem*> r; for (const auto& i : items_) if (i.is_complete()) r.push_back(&i); return r; }
auto RenderingOutputCompletionAuditor::incomplete_items() const -> std::vector<const RenderingCompletionItem*> {
    std::vector<const RenderingCompletionItem*> r; for (const auto& i : items_) if (!i.is_complete()) r.push_back(&i); return r; }
auto RenderingOutputCompletionAuditor::report() const -> RenderingCompletionReport {
    RenderingCompletionReport rpt; rpt.total = items_.size();
    for (const auto& i : items_) { if (i.is_complete()) ++rpt.complete; else ++rpt.incomplete; } return rpt; }
void RenderingOutputCompletionAuditor::clear() { items_.clear(); }
auto RenderingOutputCompletionAuditor::export_json() const -> std::string {
    auto r = report(); std::ostringstream ss;
    ss << "{\n  \"total\": " << r.total << ",\n  \"complete\": " << r.complete
       << ",\n  \"incomplete\": " << r.incomplete << ",\n  \"coverage_pct\": " << r.coverage_pct() << "\n}\n"; return ss.str(); }
auto RenderingOutputCompletionAuditor::export_markdown() const -> std::string {
    auto r = report(); std::ostringstream ss;
    ss << "# V23 Rendering Output Completion\n\n**Coverage:** " << r.coverage_pct() << "% (" << r.complete << "/" << r.total << ")\n"; return ss.str(); }
} // namespace markamp::core
