/// @file TestingRegressionCompletionAuditor.h
/// @brief V23 Phase 19 — Testing, fuzz, E2E, smoke, and regression completion auditor.
#pragma once
#include <cstdint>
#include <string>
#include <vector>
namespace markamp::core {
enum class TestingArea : uint8_t {
    kPlaceholderTests, kFuzzHarness, kSmokeE2E,
    kFailureModeTests, kScoreboard, kCorpusCoverage,
    kRestartTests, kUnsupportedStateTests,
};
[[nodiscard]] constexpr auto testing_area_label(TestingArea a) -> const char* {
    switch (a) {
    case TestingArea::kPlaceholderTests:     return "PlaceholderTests";
    case TestingArea::kFuzzHarness:          return "FuzzHarness";
    case TestingArea::kSmokeE2E:             return "SmokeE2E";
    case TestingArea::kFailureModeTests:     return "FailureModeTests";
    case TestingArea::kScoreboard:           return "Scoreboard";
    case TestingArea::kCorpusCoverage:       return "CorpusCoverage";
    case TestingArea::kRestartTests:         return "RestartTests";
    case TestingArea::kUnsupportedStateTests:return "UnsupportedStateTests";
    } return "Unknown";
}
struct TestingCompletionItem {
    TestingArea area{TestingArea::kPlaceholderTests};
    std::string feature_name;
    bool is_real_test{false}; bool has_error_handling{false};
    std::string evidence_file; int evidence_line{0};
    [[nodiscard]] auto is_complete() const noexcept -> bool { return is_real_test && has_error_handling; }
};
struct TestingCompletionReport {
    std::size_t total{0}; std::size_t complete{0}; std::size_t incomplete{0};
    [[nodiscard]] auto has_gaps() const noexcept -> bool { return incomplete > 0; }
    [[nodiscard]] auto coverage_pct() const noexcept -> double
    { return total > 0 ? (static_cast<double>(complete)/static_cast<double>(total))*100.0 : 100.0; }
};
class TestingRegressionCompletionAuditor {
public:
    void add_item(TestingCompletionItem item);
    [[nodiscard]] auto item_count() const noexcept -> std::size_t;
    [[nodiscard]] auto items_by_area(TestingArea a) const -> std::vector<const TestingCompletionItem*>;
    [[nodiscard]] auto complete_items() const -> std::vector<const TestingCompletionItem*>;
    [[nodiscard]] auto incomplete_items() const -> std::vector<const TestingCompletionItem*>;
    [[nodiscard]] auto report() const -> TestingCompletionReport;
    void clear();
    [[nodiscard]] auto export_json() const -> std::string;
    [[nodiscard]] auto export_markdown() const -> std::string;
private:
    std::vector<TestingCompletionItem> items_;
};
} // namespace markamp::core
