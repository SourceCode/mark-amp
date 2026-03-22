/// @file SettingsCompletionAuditor.cpp
/// @brief V23 Phase 10 — SettingsCompletionAuditor implementation.

#include "SettingsCompletionAuditor.h"
#include <sstream>

namespace markamp::core
{

void SettingsCompletionAuditor::add_item(SettingsCompletionItem item) { items_.push_back(std::move(item)); }
void SettingsCompletionAuditor::add_items(std::vector<SettingsCompletionItem> items)
{ for (auto& i : items) items_.push_back(std::move(i)); }

auto SettingsCompletionAuditor::item_count() const noexcept -> std::size_t { return items_.size(); }

auto SettingsCompletionAuditor::items_by_area(SettingsCapabilityArea area) const
    -> std::vector<const SettingsCompletionItem*>
{
    std::vector<const SettingsCompletionItem*> r;
    for (const auto& i : items_) if (i.area == area) r.push_back(&i);
    return r;
}

auto SettingsCompletionAuditor::complete_items() const -> std::vector<const SettingsCompletionItem*>
{
    std::vector<const SettingsCompletionItem*> r;
    for (const auto& i : items_) if (i.is_complete()) r.push_back(&i);
    return r;
}

auto SettingsCompletionAuditor::incomplete_items() const -> std::vector<const SettingsCompletionItem*>
{
    std::vector<const SettingsCompletionItem*> r;
    for (const auto& i : items_) if (!i.is_complete()) r.push_back(&i);
    return r;
}

auto SettingsCompletionAuditor::report() const -> SettingsCompletionReport
{
    SettingsCompletionReport rpt;
    rpt.total = items_.size();
    for (const auto& i : items_)
    { if (i.is_complete()) ++rpt.complete; else ++rpt.incomplete; }
    return rpt;
}

void SettingsCompletionAuditor::clear() { items_.clear(); }

auto SettingsCompletionAuditor::export_json() const -> std::string
{
    auto r = report();
    std::ostringstream ss;
    ss << "{\n  \"total\": " << r.total << ",\n  \"complete\": " << r.complete
       << ",\n  \"incomplete\": " << r.incomplete << ",\n  \"coverage_pct\": " << r.coverage_pct() << "\n}\n";
    return ss.str();
}

auto SettingsCompletionAuditor::export_markdown() const -> std::string
{
    auto r = report();
    std::ostringstream ss;
    ss << "# V23 Settings Completion\n\n"
       << "**Coverage:** " << r.coverage_pct() << "% (" << r.complete << "/" << r.total << ")\n";
    return ss.str();
}

} // namespace markamp::core
