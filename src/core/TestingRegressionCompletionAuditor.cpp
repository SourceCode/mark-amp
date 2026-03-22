/// @file TestingRegressionCompletionAuditor.cpp
#include "TestingRegressionCompletionAuditor.h"
#include <sstream>
namespace markamp::core {
void TestingRegressionCompletionAuditor::add_item(TestingCompletionItem item) { items_.push_back(std::move(item)); }
auto TestingRegressionCompletionAuditor::item_count() const noexcept -> std::size_t { return items_.size(); }
auto TestingRegressionCompletionAuditor::items_by_area(TestingArea a) const -> std::vector<const TestingCompletionItem*> {
    std::vector<const TestingCompletionItem*> r; for (const auto& i : items_) if (i.area == a) r.push_back(&i); return r; }
auto TestingRegressionCompletionAuditor::complete_items() const -> std::vector<const TestingCompletionItem*> {
    std::vector<const TestingCompletionItem*> r; for (const auto& i : items_) if (i.is_complete()) r.push_back(&i); return r; }
auto TestingRegressionCompletionAuditor::incomplete_items() const -> std::vector<const TestingCompletionItem*> {
    std::vector<const TestingCompletionItem*> r; for (const auto& i : items_) if (!i.is_complete()) r.push_back(&i); return r; }
auto TestingRegressionCompletionAuditor::report() const -> TestingCompletionReport {
    TestingCompletionReport rpt; rpt.total = items_.size();
    for (const auto& i : items_) { if (i.is_complete()) ++rpt.complete; else ++rpt.incomplete; } return rpt; }
void TestingRegressionCompletionAuditor::clear() { items_.clear(); }
auto TestingRegressionCompletionAuditor::export_json() const -> std::string {
    auto r = report(); std::ostringstream ss;
    ss << "{\n  \"total\": " << r.total << ",\n  \"complete\": " << r.complete
       << ",\n  \"incomplete\": " << r.incomplete << ",\n  \"coverage_pct\": " << r.coverage_pct() << "\n}\n"; return ss.str(); }
auto TestingRegressionCompletionAuditor::export_markdown() const -> std::string {
    auto r = report(); std::ostringstream ss;
    ss << "# V23 Testing Regression Completion\n\n**Coverage:** " << r.coverage_pct() << "% (" << r.complete << "/" << r.total << ")\n"; return ss.str(); }
} // namespace markamp::core
