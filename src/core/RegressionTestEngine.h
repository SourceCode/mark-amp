/// @file RegressionTestEngine.h
/// @brief V9 Phase 49 — Regression baseline comparison and delta detection.
#pragma once

#include <cmath>
#include <string>
#include <vector>

namespace markamp::core
{

/// A regression baseline to compare against.
struct RegressionBaseline
{
    std::string test_name;
    double expected_value{0.0};
    double tolerance{0.01}; ///< Allowed percentage deviation (0.01 = 1%)
};

/// Result of a regression comparison.
struct RegressionResult
{
    std::string test_name;
    bool passed{false};
    double actual_value{0.0};
    double expected_value{0.0};
    double delta{0.0};
};

/// Manages regression baselines and detects regressions.
class RegressionTestEngine
{
public:
    RegressionTestEngine() = default;

    // ── Baselines ─────────────────────────────────────────────────────
    void add_baseline(RegressionBaseline baseline);
    [[nodiscard]] auto has_baseline(const std::string& test_name) const -> bool;

    // ── Execution ─────────────────────────────────────────────────────
    auto run_regression(const std::string& test_name, double actual_value) -> RegressionResult;

    // ── Results ───────────────────────────────────────────────────────
    [[nodiscard]] auto get_results() const -> const std::vector<RegressionResult>&;
    [[nodiscard]] auto passed_count() const -> int;
    [[nodiscard]] auto failed_count() const -> int;

    // ── Utility ───────────────────────────────────────────────────────
    [[nodiscard]] static auto within_tolerance(double actual, double expected, double tolerance)
        -> bool;

    void clear();

private:
    std::vector<RegressionBaseline> baselines_;
    std::vector<RegressionResult> results_;
};

} // namespace markamp::core
