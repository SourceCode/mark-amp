#pragma once

#include "VisualLanguageTokens.h"

#include <string>
#include <vector>

namespace markamp::core
{

/// V22 Phase 01 (P01-T06): Design system audit service for validating consistency
/// of spacing, typography, corners, and icon metrics across the UI surface.
///
/// Usage:
///   DesignSystemAudit audit;
///   auto issues = audit.audit_spacing_consistency();
///   auto atlas = audit.generate_component_atlas();
class DesignSystemAudit
{
public:
    /// Severity levels for audit findings.
    enum class Severity : uint8_t
    {
        kInfo,    /// Informational — no action required
        kWarning, /// Should be fixed — deviation from standard
        kError,   /// Must be fixed — breaks visual language
    };

    /// A single audit finding.
    struct Finding
    {
        Severity severity{Severity::kInfo};
        std::string component;   /// Component name or file
        std::string description; /// Human-readable finding description
        std::string suggestion;  /// Suggested fix

        [[nodiscard]] auto is_error() const -> bool
        {
            return severity == Severity::kError;
        }
        [[nodiscard]] auto is_warning() const -> bool
        {
            return severity == Severity::kWarning;
        }
    };

    /// Component atlas entry — describes measured metrics for one UI component.
    struct ComponentEntry
    {
        std::string name;
        int row_height{0};
        int icon_size{0};
        int padding_h{0};
        int padding_v{0};
        CornerRadiusToken corner{CornerRadiusToken::kNone};
        BorderWeightToken border{BorderWeightToken::kNone};
        TypeScaleToken font{TypeScaleToken::kBody};
    };

    DesignSystemAudit() = default;

    /// Audit spacing consistency across registered components.
    /// Returns findings for surfaces not using SpacingGrid tokens.
    [[nodiscard]] auto audit_spacing_consistency() const -> std::vector<Finding>;

    /// Audit typography consistency across registered components.
    /// Returns findings for non-scalable font declarations.
    [[nodiscard]] auto audit_typography_consistency() const -> std::vector<Finding>;

    /// Audit corner radius consistency across registered components.
    /// Returns findings for hardcoded radius values.
    [[nodiscard]] auto audit_corner_radius_consistency() const -> std::vector<Finding>;

    /// Audit icon metric consistency across registered components.
    /// Returns findings for non-standard icon sizes.
    [[nodiscard]] auto audit_icon_metric_consistency() const -> std::vector<Finding>;

    /// Run all audit checks and return combined findings.
    [[nodiscard]] auto audit_all() const -> std::vector<Finding>;

    /// Generate a component atlas (JSON-serializable list of component metrics).
    [[nodiscard]] auto generate_component_atlas() const -> std::vector<ComponentEntry>;

    /// Register a component for auditing.
    void register_component(const ComponentEntry& entry);

    /// Number of registered components.
    [[nodiscard]] auto component_count() const -> std::size_t
    {
        return components_.size();
    }

    /// Total number of findings from the last full audit.
    [[nodiscard]] auto last_finding_count() const -> std::size_t
    {
        return last_findings_count_;
    }

private:
    std::vector<ComponentEntry> components_;
    mutable std::size_t last_findings_count_{0};
};

} // namespace markamp::core
