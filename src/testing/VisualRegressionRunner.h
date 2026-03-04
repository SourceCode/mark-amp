// ============================================================================
// File: src/testing/VisualRegressionRunner.h
// Phase 50: UI Integration Testing Harness — Visual regression testing
// ============================================================================
#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::testing
{

/// Result of a visual comparison.
enum class ComparisonResult : uint8_t
{
    Match,       ///< Images match within tolerance
    Mismatch,    ///< Images differ beyond tolerance
    NewBaseline, ///< No baseline exists (first run)
    Error        ///< Could not perform comparison
};

/// A single visual regression test result.
struct VisualTestResult
{
    std::string test_name;
    std::string theme_name;
    ComparisonResult result{ComparisonResult::Error};
    double diff_percentage{0.0}; ///< Pixel difference percentage
    std::string baseline_path;
    std::string actual_path;
    std::string diff_path;
};

/// Runner for visual regression tests.
/// Captures screenshots, compares against baselines, generates diff images.
class VisualRegressionRunner
{
public:
    static constexpr double kDefaultTolerance = 0.5; // 0.5% pixel difference

    VisualRegressionRunner() = default;

    /// Set the baseline directory.
    void set_baseline_dir(const std::string& dir)
    {
        baseline_dir_ = dir;
    }

    /// Set the output directory for actual/diff images.
    void set_output_dir(const std::string& dir)
    {
        output_dir_ = dir;
    }

    /// Set tolerance (percentage of pixels that can differ).
    void set_tolerance(double percent)
    {
        tolerance_ = percent;
    }
    [[nodiscard]] auto tolerance() const -> double
    {
        return tolerance_;
    }

    /// Compare two pixel buffers (RGBA, width x height).
    [[nodiscard]] auto compare_buffers(const std::vector<uint8_t>& baseline,
                                       const std::vector<uint8_t>& actual,
                                       int width,
                                       int height) const -> double;

    /// Record a test result.
    void record_result(VisualTestResult result);

    /// Get all results.
    [[nodiscard]] auto results() const -> const std::vector<VisualTestResult>&
    {
        return results_;
    }

    /// Get pass count.
    [[nodiscard]] auto pass_count() const -> int;

    /// Get fail count.
    [[nodiscard]] auto fail_count() const -> int;

    /// Get pass rate (0.0 - 100.0).
    [[nodiscard]] auto pass_rate() const -> double;

    /// Update baseline with actual image.
    void update_baseline(const std::string& test_name);

private:
    std::string baseline_dir_;
    std::string output_dir_;
    double tolerance_{kDefaultTolerance};
    std::vector<VisualTestResult> results_;
};

} // namespace markamp::testing
