#include "NavigationChromeCoordinator.h"

namespace markamp::core
{

auto NavigationChromeCoordinator::tab_metrics(TabState state) const -> TabMetrics
{
    TabMetrics metrics;

    switch (state)
    {
        case TabState::kActive:
            metrics.corner = CornerRadiusToken::kNone;
            metrics.active_indicator = BorderWeightToken::kMedium;
            break;
        case TabState::kInactive:
            metrics.corner = CornerRadiusToken::kNone;
            metrics.active_indicator = BorderWeightToken::kNone;
            break;
        case TabState::kModified:
            metrics.corner = CornerRadiusToken::kNone;
            metrics.active_indicator = BorderWeightToken::kNone;
            // Modified tabs show a dot indicator, geometry is the same as inactive
            break;
        case TabState::kPinned:
            metrics.min_width = 36; // Pinned tabs are narrower (icon-only)
            metrics.max_width = 36;
            metrics.padding_h = 0;
            metrics.corner = CornerRadiusToken::kNone;
            break;
        case TabState::kPreview:
            metrics.label_font = TypeScaleToken::kBody; // Italic via rendering, not here
            metrics.corner = CornerRadiusToken::kNone;
            metrics.active_indicator = BorderWeightToken::kNone;
            break;
        case TabState::kDragging:
            metrics.corner = CornerRadiusToken::kSm;
            metrics.active_indicator = BorderWeightToken::kNone;
            break;
    }

    return metrics;
}

auto NavigationChromeCoordinator::wayfinding_parity_check() const -> std::vector<ParityFinding>
{
    // V22 Phase 05 (P05-T05): Return known parity gaps.
    // In a full implementation, this would inspect live component metrics.
    std::vector<ParityFinding> findings;

    // These are the known parity gaps from the V22 assessment
    findings.push_back(
        {"notebook",
         "Notebook tabs do not share the same height as editor tabs",
         "Use NavigationChromeCoordinator::tab_metrics() for both surfaces"});

    findings.push_back(
        {"canvas",
         "Canvas board tabs lack modified-dot indicator",
         "Wire TabModifiedDot token into canvas board tab rendering"});

    return findings;
}

} // namespace markamp::core
