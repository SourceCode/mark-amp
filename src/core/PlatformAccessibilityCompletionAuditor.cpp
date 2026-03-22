/// @file PlatformAccessibilityCompletionAuditor.cpp
#include "PlatformAccessibilityCompletionAuditor.h"
#include <sstream>
namespace markamp::core {
void PlatformAccessibilityCompletionAuditor::add_item(PlatformAccessibilityItem item) { items_.push_back(std::move(item)); }
auto PlatformAccessibilityCompletionAuditor::item_count() const noexcept -> std::size_t { return items_.size(); }
auto PlatformAccessibilityCompletionAuditor::items_by_area(PlatformAccessibilityArea a) const -> std::vector<const PlatformAccessibilityItem*> {
    std::vector<const PlatformAccessibilityItem*> r; for (const auto& i : items_) if (i.area == a) r.push_back(&i); return r; }
auto PlatformAccessibilityCompletionAuditor::complete_items() const -> std::vector<const PlatformAccessibilityItem*> {
    std::vector<const PlatformAccessibilityItem*> r; for (const auto& i : items_) if (i.is_complete()) r.push_back(&i); return r; }
auto PlatformAccessibilityCompletionAuditor::incomplete_items() const -> std::vector<const PlatformAccessibilityItem*> {
    std::vector<const PlatformAccessibilityItem*> r; for (const auto& i : items_) if (!i.is_complete()) r.push_back(&i); return r; }
auto PlatformAccessibilityCompletionAuditor::report() const -> PlatformAccessibilityReport {
    PlatformAccessibilityReport rpt; rpt.total = items_.size();
    for (const auto& i : items_) { if (i.is_complete()) ++rpt.complete; else ++rpt.incomplete; } return rpt; }
void PlatformAccessibilityCompletionAuditor::clear() { items_.clear(); }
auto PlatformAccessibilityCompletionAuditor::export_json() const -> std::string {
    auto r = report(); std::ostringstream ss;
    ss << "{\n  \"total\": " << r.total << ",\n  \"complete\": " << r.complete
       << ",\n  \"incomplete\": " << r.incomplete << ",\n  \"coverage_pct\": " << r.coverage_pct() << "\n}\n"; return ss.str(); }
auto PlatformAccessibilityCompletionAuditor::export_markdown() const -> std::string {
    auto r = report(); std::ostringstream ss;
    ss << "# V23 Platform Accessibility Completion\n\n**Coverage:** " << r.coverage_pct() << "% (" << r.complete << "/" << r.total << ")\n"; return ss.str(); }
} // namespace markamp::core
