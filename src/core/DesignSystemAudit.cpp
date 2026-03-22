#include "DesignSystemAudit.h"

#include "VisualLanguageTokens.h"

#include <algorithm>

namespace markamp::core
{

void DesignSystemAudit::register_component(const ComponentEntry& entry)
{
    components_.push_back(entry);
}

auto DesignSystemAudit::audit_spacing_consistency() const -> std::vector<Finding>
{
    std::vector<Finding> findings;

    // V22 Phase 01 (P01-T01): Check that all row heights align to the 4px spacing grid.
    // Valid heights: multiples of 4, or the canonical values 22, 26, 30, 36, 48.
    for (const auto& comp : components_)
    {
        if (comp.row_height > 0 && (comp.row_height % 4 != 0))
        {
            findings.push_back(
                {Severity::kWarning,
                 comp.name,
                 "Row height " + std::to_string(comp.row_height) + "px is not on the 4px grid",
                 "Use a row height that is a multiple of 4 (e.g., "
                     + std::to_string((comp.row_height / 4 + 1) * 4) + "px)"});
        }

        // Check padding consistency with spacing tokens (should be 0, 2, 4, 8, 12, 16, 24, 32, 48)
        auto is_valid_spacing = [](int value) -> bool
        {
            constexpr int kValidSpacings[] = {0, 2, 4, 8, 12, 16, 24, 32, 48};
            return std::any_of(std::begin(kValidSpacings),
                               std::end(kValidSpacings),
                               [value](int v) { return v == value; });
        };

        if (!is_valid_spacing(comp.padding_h))
        {
            findings.push_back(
                {Severity::kWarning,
                 comp.name,
                 "Horizontal padding " + std::to_string(comp.padding_h)
                     + "px is not a SpacingGrid token value",
                 "Use a standard spacing token (0, 2, 4, 8, 12, 16, 24, 32, 48)"});
        }
        if (!is_valid_spacing(comp.padding_v))
        {
            findings.push_back(
                {Severity::kWarning,
                 comp.name,
                 "Vertical padding " + std::to_string(comp.padding_v)
                     + "px is not a SpacingGrid token value",
                 "Use a standard spacing token (0, 2, 4, 8, 12, 16, 24, 32, 48)"});
        }
    }

    return findings;
}

auto DesignSystemAudit::audit_typography_consistency() const -> std::vector<Finding>
{
    std::vector<Finding> findings;

    // V22 Phase 01 (P01-T02): Check that all font sizes match the V22 type scale.
    // Valid sizes from TypeScaleToken: 10, 12, 13, 14, 16, 20.
    for (const auto& comp : components_)
    {
        auto metrics = resolve_type_scale(comp.font);
        // Ensure that the component uses a recognized TypeScaleToken
        // (all registered tokens are valid, this checks for misuse)
        if (metrics.point_size < 10 || metrics.point_size > 20)
        {
            findings.push_back(
                {Severity::kError,
                 comp.name,
                 "Font point size " + std::to_string(metrics.point_size) + " is outside the V22 "
                     "type scale range (10–20)",
                 "Use a TypeScaleToken value (kCaption, kBody, kBodyLarge, kSubtitle, kTitle, "
                 "kDisplay)"});
        }
    }

    return findings;
}

auto DesignSystemAudit::audit_corner_radius_consistency() const -> std::vector<Finding>
{
    std::vector<Finding> findings;

    // V22 Phase 01 (P01-T03): Check that corners use CornerRadiusToken values.
    for (const auto& comp : components_)
    {
        auto radius = resolve_corner_radius(comp.corner);
        // All CornerRadiusToken values are valid by definition in V22.
        // This check validates that containers with elevation also have rounding.
        if (comp.border != BorderWeightToken::kNone && comp.corner == CornerRadiusToken::kNone)
        {
            findings.push_back(
                {Severity::kInfo,
                 comp.name,
                 "Container has a border (weight=" + std::to_string(resolve_border_weight(comp.border))
                     + "px) but no corner rounding (radius=" + std::to_string(radius) + "px)",
                 "Consider adding kSm or kMd corner rounding for visual softness"});
        }
    }

    return findings;
}

auto DesignSystemAudit::audit_icon_metric_consistency() const -> std::vector<Finding>
{
    std::vector<Finding> findings;

    // V22 Phase 01 (P01-T05): Check that icon sizes match IconMetricToken values.
    // Valid sizes: 12, 14, 16, 20, 24, 32.
    auto is_valid_icon_size = [](int size) -> bool
    {
        constexpr int kValidSizes[] = {12, 14, 16, 20, 24, 32};
        return std::any_of(std::begin(kValidSizes),
                           std::end(kValidSizes),
                           [size](int v) { return v == size; });
    };

    for (const auto& comp : components_)
    {
        if (comp.icon_size > 0 && !is_valid_icon_size(comp.icon_size))
        {
            findings.push_back(
                {Severity::kWarning,
                 comp.name,
                 "Icon size " + std::to_string(comp.icon_size) + "px is not a standard "
                     "IconMetricToken value",
                 "Use a standard icon size (12, 14, 16, 20, 24, 32)"});
        }
    }

    return findings;
}

auto DesignSystemAudit::audit_all() const -> std::vector<Finding>
{
    std::vector<Finding> all;

    auto spacing = audit_spacing_consistency();
    auto typo = audit_typography_consistency();
    auto corners = audit_corner_radius_consistency();
    auto icons = audit_icon_metric_consistency();

    all.insert(all.end(), spacing.begin(), spacing.end());
    all.insert(all.end(), typo.begin(), typo.end());
    all.insert(all.end(), corners.begin(), corners.end());
    all.insert(all.end(), icons.begin(), icons.end());

    last_findings_count_ = all.size();
    return all;
}

auto DesignSystemAudit::generate_component_atlas() const -> std::vector<ComponentEntry>
{
    return components_;
}

} // namespace markamp::core
