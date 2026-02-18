/// @file RegressionTestEngine.cpp
/// @brief V9 Phase 49 — RegressionTestEngine implementation.

#include "RegressionTestEngine.h"

#include <algorithm>

namespace markamp::core
{

void RegressionTestEngine::add_baseline(RegressionBaseline baseline)
{
    baselines_.push_back(std::move(baseline));
}

auto RegressionTestEngine::has_baseline(const std::string& test_name) const -> bool
{
    return std::any_of(baselines_.begin(),
                       baselines_.end(),
                       [&](const RegressionBaseline& baseline)
                       { return baseline.test_name == test_name; });
}

auto RegressionTestEngine::run_regression(const std::string& test_name, double actual_value)
    -> RegressionResult
{
    RegressionResult result;
    result.test_name = test_name;
    result.actual_value = actual_value;

    for (const auto& baseline : baselines_)
    {
        if (baseline.test_name == test_name)
        {
            result.expected_value = baseline.expected_value;
            result.delta = actual_value - baseline.expected_value;
            result.passed =
                within_tolerance(actual_value, baseline.expected_value, baseline.tolerance);
            results_.push_back(result);
            return result;
        }
    }

    // No baseline found — mark as failed
    result.passed = false;
    result.delta = actual_value;
    results_.push_back(result);
    return result;
}

auto RegressionTestEngine::get_results() const -> const std::vector<RegressionResult>&
{
    return results_;
}

auto RegressionTestEngine::passed_count() const -> int
{
    return static_cast<int>(std::count_if(
        results_.begin(), results_.end(), [](const RegressionResult& res) { return res.passed; }));
}

auto RegressionTestEngine::failed_count() const -> int
{
    return static_cast<int>(std::count_if(
        results_.begin(), results_.end(), [](const RegressionResult& res) { return !res.passed; }));
}

auto RegressionTestEngine::within_tolerance(double actual, double expected, double tolerance)
    -> bool
{
    if (expected == 0.0)
    {
        return std::abs(actual) <= tolerance;
    }
    double relative_delta = std::abs((actual - expected) / expected);
    return relative_delta <= tolerance;
}

void RegressionTestEngine::clear()
{
    baselines_.clear();
    results_.clear();
}

} // namespace markamp::core
