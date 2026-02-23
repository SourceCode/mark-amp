#pragma once

#include "core/Theme.h"

#include <string>
#include <vector>

namespace markamp::ui::accessibility
{

/// Tally of accessibility compliance ratings based on color contrast.
struct ContrastAuditReport
{
    int total_pairs{0};
    int compliant_aa{0};
    int compliant_aaa{0};
    int failures{0};

    struct FailureDetail
    {
        std::string foreground_token;
        std::string background_token;
        double ratio;
    };
    std::vector<FailureDetail> failure_details;

    [[nodiscard]] auto is_fully_aa_compliant() const -> bool
    {
        return failures == 0;
    }
};

/// Service that performs a runtime accessibility audit sweep across visual parameters.
class AccessibilityAuditor
{
public:
    /// Runs a contrast ratio audit on the currently active theme tokens.
    /// Returns a report detailing compliance levels across all registered token pairs.
    [[nodiscard]] static auto run_contrast_audit(const core::Theme& current_theme)
        -> ContrastAuditReport;

private:
    [[nodiscard]] static auto resolve_token_to_color(const core::Theme& theme,
                                                     core::ThemeColorToken token) -> wxColour;
    [[nodiscard]] static auto token_to_string(core::ThemeColorToken token) -> std::string;
};

} // namespace markamp::ui::accessibility
