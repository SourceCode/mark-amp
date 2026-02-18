/// @file IntegrationTestRunner.cpp
/// @brief V9 Phase 49 — IntegrationTestRunner implementation.

#include "IntegrationTestRunner.h"

#include <algorithm>

namespace markamp::core
{

auto TestSuite::passed_count() const -> int
{
    return static_cast<int>(std::count_if(results.begin(),
                                          results.end(),
                                          [](const TestResult& res)
                                          { return res.status == TestStatus::kPassed; }));
}

auto TestSuite::failed_count() const -> int
{
    return static_cast<int>(std::count_if(results.begin(),
                                          results.end(),
                                          [](const TestResult& res)
                                          { return res.status == TestStatus::kFailed; }));
}

auto TestSuite::skipped_count() const -> int
{
    return static_cast<int>(std::count_if(results.begin(),
                                          results.end(),
                                          [](const TestResult& res)
                                          { return res.status == TestStatus::kSkipped; }));
}

auto TestRunSummary::all_passed() const -> bool
{
    return failed == 0 && total_tests > 0;
}

auto IntegrationTestRunner::create_suite(const std::string& name) -> std::string
{
    TestSuite suite;
    suite.suite_id = "suite_" + std::to_string(next_id_++);
    suite.name = name;
    suites_.push_back(std::move(suite));
    return suites_.back().suite_id;
}

auto IntegrationTestRunner::add_result(const std::string& suite_id, TestResult result) -> bool
{
    for (auto& suite : suites_)
    {
        if (suite.suite_id == suite_id)
        {
            suite.results.push_back(std::move(result));
            return true;
        }
    }
    return false;
}

auto IntegrationTestRunner::get_suite(const std::string& suite_id) const -> const TestSuite*
{
    for (const auto& suite : suites_)
    {
        if (suite.suite_id == suite_id)
        {
            return &suite;
        }
    }
    return nullptr;
}

void IntegrationTestRunner::run_all()
{
    // Mark all pending tests as passed (simulation for testing infrastructure)
    for (auto& suite : suites_)
    {
        for (auto& result : suite.results)
        {
            if (result.status == TestStatus::kPending)
            {
                result.status = TestStatus::kPassed;
                result.duration_ms = 1.0; // Simulated duration
            }
        }
    }
}

auto IntegrationTestRunner::summary() const -> TestRunSummary
{
    TestRunSummary result;
    result.total_suites = static_cast<int>(suites_.size());
    for (const auto& suite : suites_)
    {
        result.total_tests += static_cast<int>(suite.results.size());
        result.passed += suite.passed_count();
        result.failed += suite.failed_count();
        result.skipped += suite.skipped_count();
        for (const auto& test_result : suite.results)
        {
            result.total_duration_ms += test_result.duration_ms;
        }
    }
    return result;
}

auto IntegrationTestRunner::suite_count() const -> int
{
    return static_cast<int>(suites_.size());
}

void IntegrationTestRunner::clear()
{
    suites_.clear();
}

} // namespace markamp::core
