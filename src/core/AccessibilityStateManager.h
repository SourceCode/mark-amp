/// @file AccessibilityStateManager.h
/// @brief V24 P07-T03: Standardize accessibility visual states.
///
/// Centralized accessibility state management: focus indicators,
/// contrast validation, reduced-motion detection, and screen reader hints.
#pragma once

#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

/// Accessibility feature flags.
enum class AccessibilityFeature
{
    kFocusIndicators,
    kHighContrast,
    kReducedMotion,
    kScreenReaderHints,
    kLargeText,
    kKeyboardNavigation
};

/// Returns a label for an AccessibilityFeature.
[[nodiscard]] constexpr auto accessibility_feature_label(AccessibilityFeature f) noexcept
    -> const char*
{
    switch (f) {
        case AccessibilityFeature::kFocusIndicators:    return "FocusIndicators";
        case AccessibilityFeature::kHighContrast:       return "HighContrast";
        case AccessibilityFeature::kReducedMotion:      return "ReducedMotion";
        case AccessibilityFeature::kScreenReaderHints:  return "ScreenReaderHints";
        case AccessibilityFeature::kLargeText:          return "LargeText";
        case AccessibilityFeature::kKeyboardNavigation: return "KeyboardNavigation";
    }
    return "Unknown";
}

/// Contrast validation result.
struct ContrastResult
{
    std::string element_id;
    double ratio{0.0};
    bool passes_aa{false};
    bool passes_aaa{false};
};

/// Accessibility audit report.
struct AccessibilityReport
{
    int elements_checked{0};
    int contrast_passes{0};
    int contrast_failures{0};
    int focus_issues{0};
    std::vector<ContrastResult> contrast_results;

    [[nodiscard]] auto is_compliant() const noexcept -> bool
    {
        return contrast_failures == 0 && focus_issues == 0;
    }
};

/// Manages accessibility states across all UI surfaces.
class AccessibilityStateManager
{
public:
    AccessibilityStateManager() = default;

    /// Enable/disable a feature.
    void set_feature(AccessibilityFeature feature, bool enabled);

    /// Check if a feature is enabled.
    [[nodiscard]] auto is_enabled(AccessibilityFeature feature) const -> bool;

    /// Get all enabled features.
    [[nodiscard]] auto enabled_features() const -> std::vector<AccessibilityFeature>;

    /// Record a contrast check.
    void record_contrast(const std::string& element_id, double ratio);

    /// Get contrast results.
    [[nodiscard]] auto contrast_results() const -> const std::vector<ContrastResult>&
    {
        return contrast_results_;
    }

    /// Record a focus indicator issue.
    void record_focus_issue(const std::string& element_id);

    /// Generate accessibility audit report.
    [[nodiscard]] auto audit() const -> AccessibilityReport;

    /// Total elements checked.
    [[nodiscard]] auto checked_count() const noexcept -> int
    {
        return static_cast<int>(contrast_results_.size());
    }

    /// Clear all state.
    void clear();

private:
    std::unordered_map<int, bool> features_;    // AccessibilityFeature -> bool
    std::vector<ContrastResult> contrast_results_;
    std::vector<std::string> focus_issues_;
};

} // namespace markamp::core
