/// @file AccessibilityStateManager.cpp
/// @brief V24 P07-T03: AccessibilityStateManager implementation.
#include "AccessibilityStateManager.h"

namespace markamp::core
{

void AccessibilityStateManager::set_feature(AccessibilityFeature feature, bool enabled)
{
    features_[static_cast<int>(feature)] = enabled;
}

auto AccessibilityStateManager::is_enabled(AccessibilityFeature feature) const -> bool
{
    auto it = features_.find(static_cast<int>(feature));
    return it != features_.end() && it->second;
}

auto AccessibilityStateManager::enabled_features() const -> std::vector<AccessibilityFeature>
{
    std::vector<AccessibilityFeature> result;
    for (const auto& [key, val] : features_) {
        if (val) result.push_back(static_cast<AccessibilityFeature>(key));
    }
    return result;
}

void AccessibilityStateManager::record_contrast(const std::string& element_id, double ratio)
{
    ContrastResult r;
    r.element_id = element_id;
    r.ratio = ratio;
    r.passes_aa = ratio >= 4.5;
    r.passes_aaa = ratio >= 7.0;
    contrast_results_.push_back(r);
}

void AccessibilityStateManager::record_focus_issue(const std::string& element_id)
{
    focus_issues_.push_back(element_id);
}

auto AccessibilityStateManager::audit() const -> AccessibilityReport
{
    AccessibilityReport report;
    report.elements_checked = static_cast<int>(contrast_results_.size());
    report.focus_issues = static_cast<int>(focus_issues_.size());
    report.contrast_results = contrast_results_;
    for (const auto& cr : contrast_results_) {
        if (cr.passes_aa) ++report.contrast_passes;
        else ++report.contrast_failures;
    }
    return report;
}

void AccessibilityStateManager::clear()
{
    features_.clear();
    contrast_results_.clear();
    focus_issues_.clear();
}

} // namespace markamp::core
