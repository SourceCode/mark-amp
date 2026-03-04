// ============================================================================
// File: src/testing/ThemeCoverageChecker.h
// Phase 50: UI Integration Testing Harness — Theme coverage checker
// ============================================================================
#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::testing
{

/// A hardcoded color finding.
struct HardcodedColorFinding
{
    std::string file_path;
    int line_number{0};
    std::string color_value; ///< e.g., "#FF0000" or "rgb(255,0,0)"
    std::string context;     ///< Surrounding code
};

/// Theme coverage report.
struct ThemeCoverageReport
{
    int total_color_usages{0};
    int themed_usages{0};
    int hardcoded_usages{0};
    std::vector<HardcodedColorFinding> findings;

    [[nodiscard]] auto coverage_percentage() const -> double
    {
        return total_color_usages > 0 ? (static_cast<double>(themed_usages) /
                                         static_cast<double>(total_color_usages)) *
                                            100.0
                                      : 100.0;
    }
};

/// Checker for theme token usage coverage.
class ThemeCoverageChecker
{
public:
    ThemeCoverageChecker() = default;

    /// Scan a code string for hardcoded colors.
    [[nodiscard]] auto scan_for_hardcoded_colors(const std::string& content,
                                                 const std::string& file_path) const
        -> std::vector<HardcodedColorFinding>;

    /// Record themed usage count.
    void record_themed_usage()
    {
        ++themed_usages_;
    }

    /// Record hardcoded usage.
    void record_hardcoded_finding(HardcodedColorFinding finding);

    /// Generate report.
    [[nodiscard]] auto generate_report() const -> ThemeCoverageReport;

private:
    int themed_usages_{0};
    std::vector<HardcodedColorFinding> findings_;
};

} // namespace markamp::testing
