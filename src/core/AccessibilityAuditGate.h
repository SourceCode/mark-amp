/// @file AccessibilityAuditGate.h
/// @brief V20 P09-T04/T05/T06: Accessibility audit and visual debt tracking.
///
/// Ensures premium polish remains keyboard-first, labeled, and legible.
/// Tracks visual debt, missing labels, contrast violations, and
/// provides review gates for premium polish work.
#pragma once

#include "EventBus.h"

#include <string>
#include <vector>

namespace markamp::core
{

/// Severity of an accessibility finding.
enum class A11ySeverity
{
    kInfo,       ///< Informational
    kWarning,    ///< Should fix, not blocking
    kError,      ///< Must fix before release
    kCritical    ///< Blocks core workflow
};

/// Category of accessibility finding.
enum class A11yCategory
{
    kLabel,           ///< Missing or inadequate label
    kFocusOrder,      ///< Focus ordering issue
    kContrast,        ///< Contrast ratio violation
    kKeyboard,        ///< Keyboard-only unreachable
    kTooltip,         ///< Missing or unhelpful tooltip
    kVisualDebt       ///< Visual inconsistency or debt
};

/// An accessibility or visual-debt finding.
struct A11yFinding
{
    std::string id;                    ///< Unique finding ID
    std::string surface;               ///< Surface/component where found
    std::string description;           ///< What's wrong
    A11ySeverity severity{A11ySeverity::kWarning};
    A11yCategory category{A11yCategory::kLabel};
    std::string recommendation;        ///< How to fix

    [[nodiscard]] auto is_blocking() const noexcept -> bool
    {
        return severity == A11ySeverity::kError || severity == A11ySeverity::kCritical;
    }
};

/// Visual debt inventory entry.
struct VisualDebtEntry
{
    std::string surface;               ///< Affected surface
    std::string description;           ///< Nature of debt
    std::string category;              ///< e.g. "spacing", "icon", "state"
    bool resolved{false};
};

/// Manages accessibility auditing and visual debt tracking.
class AccessibilityAuditGate
{
public:
    explicit AccessibilityAuditGate(EventBus& bus);

    /// Record an accessibility finding.
    void record_finding(const A11yFinding& finding);

    /// All findings.
    [[nodiscard]] auto all_findings() const -> const std::vector<A11yFinding>& { return findings_; }

    /// Findings by severity.
    [[nodiscard]] auto findings_by_severity(A11ySeverity severity) const -> std::vector<A11yFinding>;

    /// Blocking findings count.
    [[nodiscard]] auto blocking_count() const -> int;

    /// Record a visual debt entry.
    void record_visual_debt(const VisualDebtEntry& entry);

    /// All visual debt entries.
    [[nodiscard]] auto all_visual_debt() const -> const std::vector<VisualDebtEntry>&
    {
        return visual_debt_;
    }

    /// Unresolved visual debt count.
    [[nodiscard]] auto unresolved_debt_count() const -> int;

    /// Mark visual debt as resolved.
    void resolve_visual_debt(const std::string& surface, const std::string& category);

    /// Check if release gates pass (no blocking findings).
    [[nodiscard]] auto gates_pass() const -> bool;

    /// Total findings.
    [[nodiscard]] auto finding_count() const noexcept -> int
    {
        return static_cast<int>(findings_.size());
    }

private:
    EventBus& event_bus_;
    std::vector<A11yFinding> findings_;
    std::vector<VisualDebtEntry> visual_debt_;
};

} // namespace markamp::core
