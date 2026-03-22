/// @file ServiceStubCompletionAuditor.cpp
#include "ServiceStubCompletionAuditor.h"
#include <sstream>
namespace markamp::core {
void ServiceStubCompletionAuditor::add_item(ServiceStubItem item) { items_.push_back(std::move(item)); }
auto ServiceStubCompletionAuditor::item_count() const noexcept -> std::size_t { return items_.size(); }
auto ServiceStubCompletionAuditor::items_by_area(ServiceStubArea a) const -> std::vector<const ServiceStubItem*> {
    std::vector<const ServiceStubItem*> r; for (const auto& i : items_) if (i.area == a) r.push_back(&i); return r; }
auto ServiceStubCompletionAuditor::complete_items() const -> std::vector<const ServiceStubItem*> {
    std::vector<const ServiceStubItem*> r; for (const auto& i : items_) if (i.is_complete()) r.push_back(&i); return r; }
auto ServiceStubCompletionAuditor::incomplete_items() const -> std::vector<const ServiceStubItem*> {
    std::vector<const ServiceStubItem*> r; for (const auto& i : items_) if (!i.is_complete()) r.push_back(&i); return r; }
auto ServiceStubCompletionAuditor::report() const -> ServiceStubReport {
    ServiceStubReport rpt; rpt.total = items_.size();
    for (const auto& i : items_) { if (i.is_complete()) ++rpt.complete; else ++rpt.incomplete; } return rpt; }
void ServiceStubCompletionAuditor::clear() { items_.clear(); }
auto ServiceStubCompletionAuditor::export_json() const -> std::string {
    auto r = report(); std::ostringstream ss;
    ss << "{\n  \"total\": " << r.total << ",\n  \"complete\": " << r.complete
       << ",\n  \"incomplete\": " << r.incomplete << ",\n  \"coverage_pct\": " << r.coverage_pct() << "\n}\n"; return ss.str(); }
auto ServiceStubCompletionAuditor::export_markdown() const -> std::string {
    auto r = report(); std::ostringstream ss;
    ss << "# V23 Service Stub Completion\n\n**Coverage:** " << r.coverage_pct() << "% (" << r.complete << "/" << r.total << ")\n"; return ss.str(); }
} // namespace markamp::core
