/// @file NodeEditorCompletionAuditor.cpp
#include "NodeEditorCompletionAuditor.h"
#include <sstream>
namespace markamp::core {
void NodeEditorCompletionAuditor::add_item(NodeEditorCompletionItem item) { items_.push_back(std::move(item)); }
auto NodeEditorCompletionAuditor::item_count() const noexcept -> std::size_t { return items_.size(); }
auto NodeEditorCompletionAuditor::items_by_area(NodeEditorArea a) const -> std::vector<const NodeEditorCompletionItem*> {
    std::vector<const NodeEditorCompletionItem*> r; for (const auto& i : items_) if (i.area == a) r.push_back(&i); return r; }
auto NodeEditorCompletionAuditor::complete_items() const -> std::vector<const NodeEditorCompletionItem*> {
    std::vector<const NodeEditorCompletionItem*> r; for (const auto& i : items_) if (i.is_complete()) r.push_back(&i); return r; }
auto NodeEditorCompletionAuditor::incomplete_items() const -> std::vector<const NodeEditorCompletionItem*> {
    std::vector<const NodeEditorCompletionItem*> r; for (const auto& i : items_) if (!i.is_complete()) r.push_back(&i); return r; }
auto NodeEditorCompletionAuditor::report() const -> NodeEditorCompletionReport {
    NodeEditorCompletionReport rpt; rpt.total = items_.size();
    for (const auto& i : items_) { if (i.is_complete()) ++rpt.complete; else ++rpt.incomplete; } return rpt; }
void NodeEditorCompletionAuditor::clear() { items_.clear(); }
auto NodeEditorCompletionAuditor::export_json() const -> std::string {
    auto r = report(); std::ostringstream ss;
    ss << "{\n  \"total\": " << r.total << ",\n  \"complete\": " << r.complete
       << ",\n  \"incomplete\": " << r.incomplete << ",\n  \"coverage_pct\": " << r.coverage_pct() << "\n}\n"; return ss.str(); }
auto NodeEditorCompletionAuditor::export_markdown() const -> std::string {
    auto r = report(); std::ostringstream ss;
    ss << "# V23 Node Editor Completion\n\n**Coverage:** " << r.coverage_pct() << "% (" << r.complete << "/" << r.total << ")\n"; return ss.str(); }
} // namespace markamp::core
