/// @file FeatureFlagMigrationCompletionAuditor.cpp
#include "FeatureFlagMigrationCompletionAuditor.h"
#include <sstream>
namespace markamp::core {
void FeatureFlagMigrationCompletionAuditor::add_item(FeatureFlagItem item) { items_.push_back(std::move(item)); }
auto FeatureFlagMigrationCompletionAuditor::item_count() const noexcept -> std::size_t { return items_.size(); }
auto FeatureFlagMigrationCompletionAuditor::items_by_area(FeatureFlagArea a) const -> std::vector<const FeatureFlagItem*> {
    std::vector<const FeatureFlagItem*> r; for (const auto& i : items_) if (i.area == a) r.push_back(&i); return r; }
auto FeatureFlagMigrationCompletionAuditor::complete_items() const -> std::vector<const FeatureFlagItem*> {
    std::vector<const FeatureFlagItem*> r; for (const auto& i : items_) if (i.is_complete()) r.push_back(&i); return r; }
auto FeatureFlagMigrationCompletionAuditor::incomplete_items() const -> std::vector<const FeatureFlagItem*> {
    std::vector<const FeatureFlagItem*> r; for (const auto& i : items_) if (!i.is_complete()) r.push_back(&i); return r; }
auto FeatureFlagMigrationCompletionAuditor::report() const -> FeatureFlagReport {
    FeatureFlagReport rpt; rpt.total = items_.size();
    for (const auto& i : items_) { if (i.is_complete()) ++rpt.complete; else ++rpt.incomplete; } return rpt; }
void FeatureFlagMigrationCompletionAuditor::clear() { items_.clear(); }
auto FeatureFlagMigrationCompletionAuditor::export_json() const -> std::string {
    auto r = report(); std::ostringstream ss;
    ss << "{\n  \"total\": " << r.total << ",\n  \"complete\": " << r.complete
       << ",\n  \"incomplete\": " << r.incomplete << ",\n  \"coverage_pct\": " << r.coverage_pct() << "\n}\n"; return ss.str(); }
auto FeatureFlagMigrationCompletionAuditor::export_markdown() const -> std::string {
    auto r = report(); std::ostringstream ss;
    ss << "# V23 Feature Flag Migration Completion\n\n**Coverage:** " << r.coverage_pct() << "% (" << r.complete << "/" << r.total << ")\n"; return ss.str(); }
} // namespace markamp::core
