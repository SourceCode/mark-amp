// ============================================================================
// File: src/testing/VisualRegressionRunner.cpp
// Phase 50: UI Integration Testing Harness — Visual regression testing
// ============================================================================
#include "VisualRegressionRunner.h"

#include <algorithm>
#include <cmath>

namespace markamp::testing
{

auto VisualRegressionRunner::compare_buffers(const std::vector<uint8_t>& baseline,
                                             const std::vector<uint8_t>& actual,
                                             int width,
                                             int height) const -> double
{
    auto pixel_count = static_cast<size_t>(width * height);
    if (pixel_count == 0)
    {
        return 0.0;
    }

    size_t expected_size = pixel_count * 4; // RGBA
    if (baseline.size() != expected_size || actual.size() != expected_size)
    {
        return 100.0; // Size mismatch = 100% different
    }

    size_t diff_pixels = 0;
    for (size_t i = 0; i < pixel_count; ++i)
    {
        size_t offset = i * 4;
        bool differs = false;
        for (size_t c = 0; c < 4; ++c)
        {
            int delta = std::abs(static_cast<int>(baseline[offset + c]) -
                                 static_cast<int>(actual[offset + c]));
            if (delta > 2) // Allow tiny color rounding
            {
                differs = true;
                break;
            }
        }
        if (differs)
        {
            ++diff_pixels;
        }
    }

    return (static_cast<double>(diff_pixels) / static_cast<double>(pixel_count)) * 100.0;
}

void VisualRegressionRunner::record_result(VisualTestResult result)
{
    results_.push_back(std::move(result));
}

auto VisualRegressionRunner::pass_count() const -> int
{
    return static_cast<int>(std::ranges::count_if(
        results_, [](const auto& r) { return r.result == ComparisonResult::Match; }));
}

auto VisualRegressionRunner::fail_count() const -> int
{
    return static_cast<int>(std::ranges::count_if(
        results_, [](const auto& r) { return r.result == ComparisonResult::Mismatch; }));
}

auto VisualRegressionRunner::pass_rate() const -> double
{
    if (results_.empty())
    {
        return 100.0;
    }
    return (static_cast<double>(pass_count()) / static_cast<double>(results_.size())) * 100.0;
}

void VisualRegressionRunner::update_baseline(const std::string& /*test_name*/)
{
    // In a real implementation, this copies the actual image to baseline_dir_.
    // Model-layer stub for now.
}

} // namespace markamp::testing
