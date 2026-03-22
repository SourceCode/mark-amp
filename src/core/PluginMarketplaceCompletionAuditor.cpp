/// @file PluginMarketplaceCompletionAuditor.cpp
#include "PluginMarketplaceCompletionAuditor.h"
#include <sstream>
namespace markamp::core {
void PluginMarketplaceCompletionAuditor::add_item(PluginCompletionItem item) { items_.push_back(std::move(item)); }
auto PluginMarketplaceCompletionAuditor::item_count() const noexcept -> std::size_t { return items_.size(); }
auto PluginMarketplaceCompletionAuditor::items_by_area(PluginCapabilityArea a) const -> std::vector<const PluginCompletionItem*> {
    std::vector<const PluginCompletionItem*> r; for (const auto& i : items_) if (i.area == a) r.push_back(&i); return r; }
auto PluginMarketplaceCompletionAuditor::complete_items() const -> std::vector<const PluginCompletionItem*> {
    std::vector<const PluginCompletionItem*> r; for (const auto& i : items_) if (i.is_complete()) r.push_back(&i); return r; }
auto PluginMarketplaceCompletionAuditor::incomplete_items() const -> std::vector<const PluginCompletionItem*> {
    std::vector<const PluginCompletionItem*> r; for (const auto& i : items_) if (!i.is_complete()) r.push_back(&i); return r; }
auto PluginMarketplaceCompletionAuditor::report() const -> PluginCompletionReport {
    PluginCompletionReport rpt; rpt.total = items_.size();
    for (const auto& i : items_) { if (i.is_complete()) ++rpt.complete; else ++rpt.incomplete; } return rpt; }
void PluginMarketplaceCompletionAuditor::clear() { items_.clear(); }
auto PluginMarketplaceCompletionAuditor::export_json() const -> std::string {
    auto r = report(); std::ostringstream ss;
    ss << "{\n  \"total\": " << r.total << ",\n  \"complete\": " << r.complete
       << ",\n  \"incomplete\": " << r.incomplete << ",\n  \"coverage_pct\": " << r.coverage_pct() << "\n}\n"; return ss.str(); }
auto PluginMarketplaceCompletionAuditor::export_markdown() const -> std::string {
    auto r = report(); std::ostringstream ss;
    ss << "# V23 Plugin Marketplace Completion\n\n**Coverage:** " << r.coverage_pct() << "% (" << r.complete << "/" << r.total << ")\n"; return ss.str(); }
} // namespace markamp::core
