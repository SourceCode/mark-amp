/// @file CloudSecurityCompletionAuditor.cpp
#include "CloudSecurityCompletionAuditor.h"
#include <sstream>
namespace markamp::core {
void CloudSecurityCompletionAuditor::add_item(CloudSecurityItem item) { items_.push_back(std::move(item)); }
auto CloudSecurityCompletionAuditor::item_count() const noexcept -> std::size_t { return items_.size(); }
auto CloudSecurityCompletionAuditor::items_by_area(CloudSecurityArea a) const -> std::vector<const CloudSecurityItem*> {
    std::vector<const CloudSecurityItem*> r; for (const auto& i : items_) if (i.area == a) r.push_back(&i); return r; }
auto CloudSecurityCompletionAuditor::complete_items() const -> std::vector<const CloudSecurityItem*> {
    std::vector<const CloudSecurityItem*> r; for (const auto& i : items_) if (i.is_complete()) r.push_back(&i); return r; }
auto CloudSecurityCompletionAuditor::incomplete_items() const -> std::vector<const CloudSecurityItem*> {
    std::vector<const CloudSecurityItem*> r; for (const auto& i : items_) if (!i.is_complete()) r.push_back(&i); return r; }
auto CloudSecurityCompletionAuditor::report() const -> CloudSecurityReport {
    CloudSecurityReport rpt; rpt.total = items_.size();
    for (const auto& i : items_) { if (i.is_complete()) ++rpt.complete; else ++rpt.incomplete; } return rpt; }
void CloudSecurityCompletionAuditor::clear() { items_.clear(); }
auto CloudSecurityCompletionAuditor::export_json() const -> std::string {
    auto r = report(); std::ostringstream ss;
    ss << "{\n  \"total\": " << r.total << ",\n  \"complete\": " << r.complete
       << ",\n  \"incomplete\": " << r.incomplete << ",\n  \"coverage_pct\": " << r.coverage_pct() << "\n}\n"; return ss.str(); }
auto CloudSecurityCompletionAuditor::export_markdown() const -> std::string {
    auto r = report(); std::ostringstream ss;
    ss << "# V23 Cloud Security Completion\n\n**Coverage:** " << r.coverage_pct() << "% (" << r.complete << "/" << r.total << ")\n"; return ss.str(); }
} // namespace markamp::core
