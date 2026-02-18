/// @file IntegrationTestRunner.h
/// @brief V9 Phase 49 — Integration test orchestration and result aggregation.
#pragma once

#include <chrono>
#include <cstdint>
#include <string>
#include <vector>

namespace markamp::core
{

/// Status of a single test.
enum class TestStatus : uint8_t
{
    kPending = 0,
    kRunning = 1,
    kPassed = 2,
    kFailed = 3,
    kSkipped = 4,
};

/// Result of a single test case.
struct TestResult
{
    std::string test_id;
    std::string name;
    TestStatus status{TestStatus::kPending};
    double duration_ms{0.0};
    std::string error_msg;
};

/// A named suite of test results.
struct TestSuite
{
    std::string suite_id;
    std::string name;
    std::vector<TestResult> results;

    [[nodiscard]] auto passed_count() const -> int;
    [[nodiscard]] auto failed_count() const -> int;
    [[nodiscard]] auto skipped_count() const -> int;
};

/// Summary totals across all suites.
struct TestRunSummary
{
    int total_suites{0};
    int total_tests{0};
    int passed{0};
    int failed{0};
    int skipped{0};
    double total_duration_ms{0.0};
    [[nodiscard]] auto all_passed() const -> bool;
};

/// Manages test suites and aggregates results.
class IntegrationTestRunner
{
public:
    IntegrationTestRunner() = default;

    // ── Suite lifecycle ───────────────────────────────────────────────
    auto create_suite(const std::string& name) -> std::string;
    auto add_result(const std::string& suite_id, TestResult result) -> bool;
    [[nodiscard]] auto get_suite(const std::string& suite_id) const -> const TestSuite*;

    // ── Execution ─────────────────────────────────────────────────────
    void run_all();
    [[nodiscard]] auto summary() const -> TestRunSummary;

    // ── Statistics ────────────────────────────────────────────────────
    [[nodiscard]] auto suite_count() const -> int;
    void clear();

private:
    std::vector<TestSuite> suites_;
    int next_id_{1};
};

} // namespace markamp::core
