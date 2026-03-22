/// @file StructuredDataCompletionAuditor.cpp
#include "StructuredDataCompletionAuditor.h"
#include <sstream>
namespace markamp::core {
void StructuredDataCompletionAuditor::add_item(StructuredDataItem item) { items_.push_back(std::move(item)); }
auto StructuredDataCompletionAuditor::item_count() const noexcept -> std::size_t { return items_.size(); }
auto StructuredDataCompletionAuditor::items_by_area(StructuredDataArea a) const -> std::vector<const StructuredDataItem*> {
    std::vector<const StructuredDataItem*> r; for (const auto& i : items_) if (i.area == a) r.push_back(&i); return r; }
auto StructuredDataCompletionAuditor::complete_items() const -> std::vector<const StructuredDataItem*> {
    std::vector<const StructuredDataItem*> r; for (const auto& i : items_) if (i.is_complete()) r.push_back(&i); return r; }
auto StructuredDataCompletionAuditor::incomplete_items() const -> std::vector<const StructuredDataItem*> {
    std::vector<const StructuredDataItem*> r; for (const auto& i : items_) if (!i.is_complete()) r.push_back(&i); return r; }
auto StructuredDataCompletionAuditor::report() const -> StructuredDataReport {
    StructuredDataReport rpt; rpt.total = items_.size();
    for (const auto& i : items_) { if (i.is_complete()) ++rpt.complete; else ++rpt.incomplete; } return rpt; }
void StructuredDataCompletionAuditor::clear() { items_.clear(); }
auto StructuredDataCompletionAuditor::export_json() const -> std::string {
    auto r = report(); std::ostringstream ss;
    ss << "{\n  \"total\": " << r.total << ",\n  \"complete\": " << r.complete
       << ",\n  \"incomplete\": " << r.incomplete << ",\n  \"coverage_pct\": " << r.coverage_pct() << "\n}\n"; return ss.str(); }
auto StructuredDataCompletionAuditor::export_markdown() const -> std::string {
    auto r = report(); std::ostringstream ss;
    ss << "# V23 Structured Data Completion\n\n**Coverage:** " << r.coverage_pct() << "% (" << r.complete << "/" << r.total << ")\n"; return ss.str(); }
} // namespace markamp::core
