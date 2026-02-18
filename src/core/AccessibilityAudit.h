// ============================================================================
// File: src/core/AccessibilityAudit.h
// Phase 31: Accessibility System — Built-in accessibility scanner
// ============================================================================
#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::core
{

/// Severity of an accessibility audit issue.
enum class AuditSeverity : uint8_t
{
    kCritical, ///< Must fix — blocks accessibility
    kWarning,  ///< Should fix — degrades experience
    kInfo,     ///< Nice to fix — improves experience
};

/// Category of an accessibility audit issue.
enum class AuditCategory : uint8_t
{
    kContrast,  ///< Color contrast below WCAG threshold
    kLabel,     ///< Missing accessible label
    kKeyboard,  ///< Not keyboard-reachable
    kAltText,   ///< Missing alt text for images
    kFocusRing, ///< Missing visible focus indicator
    kRole,      ///< Missing ARIA-like role
};

/// A single accessibility audit issue.
struct AuditIssue
{
    std::string element_id;     ///< Identifier of the element
    std::string element_label;  ///< Human-readable label of the element
    AuditCategory category;     ///< Issue category
    AuditSeverity severity;     ///< Issue severity
    std::string description;    ///< Description of the issue
    std::string fix_suggestion; ///< Suggested fix
};

/// Summary counts for an audit run.
struct AuditSummary
{
    int critical_count{0};
    int warning_count{0};
    int info_count{0};
    int total_elements_checked{0};
};

/// Complete result of an accessibility audit.
struct AuditResult
{
    std::vector<AuditIssue> issues;
    AuditSummary summary;
    bool passed{true}; ///< True if no critical issues found
};

/// AccessibilityAudit — built-in scanner for accessibility issues.
///
/// Phase 31 Task 18: Scans the current view for missing labels, low contrast,
/// no keyboard access, missing alt text, missing focus rings, and missing roles.
/// Results categorized by severity with fix suggestions.
class AccessibilityAudit
{
public:
    AccessibilityAudit();

    // ── Individual checks ──────────────────────────────────────────

    /// Check a contrast ratio against a minimum threshold.
    /// Issues a Critical finding if ratio < min_ratio.
    void check_contrast(const std::string& element_id,
                        const std::string& element_label,
                        double actual_ratio,
                        double min_ratio);

    /// Check whether an element has an accessible label.
    void check_missing_label(const std::string& element_id,
                             const std::string& element_label,
                             bool has_label);

    /// Check whether an image element has alt text.
    void check_missing_alt_text(const std::string& element_id,
                                const std::string& element_label,
                                bool has_alt_text);

    /// Check whether an element is keyboard-reachable.
    void check_keyboard_access(const std::string& element_id,
                               const std::string& element_label,
                               bool is_keyboard_reachable);

    /// Check whether a focusable element has a visible focus ring.
    void check_focus_ring(const std::string& element_id,
                          const std::string& element_label,
                          bool has_focus_ring);

    /// Check whether a custom control has an accessible role.
    void check_role(const std::string& element_id, const std::string& element_label, bool has_role);

    // ── Audit result ───────────────────────────────────────────────

    /// Get the current audit result.
    [[nodiscard]] auto result() const -> AuditResult;

    /// Get the number of issues found.
    [[nodiscard]] auto issue_count() const -> int;

    /// Get issues filtered by severity.
    [[nodiscard]] auto issues_by_severity(AuditSeverity severity) const -> std::vector<AuditIssue>;

    /// Get issues filtered by category.
    [[nodiscard]] auto issues_by_category(AuditCategory category) const -> std::vector<AuditIssue>;

    /// Whether the audit passed (no critical issues).
    [[nodiscard]] auto passed() const -> bool;

    /// Reset / clear all issues.
    void reset();

    // ── Export ──────────────────────────────────────────────────────

    /// Export audit result as JSON string.
    [[nodiscard]] auto to_json() const -> std::string;

    // ── Helpers ────────────────────────────────────────────────────

    /// Get a human-readable name for a severity level.
    [[nodiscard]] static auto severity_name(AuditSeverity severity) -> std::string;

    /// Get a human-readable name for an audit category.
    [[nodiscard]] static auto category_name(AuditCategory category) -> std::string;

private:
    std::vector<AuditIssue> issues_;

    /// Add an issue to the list.
    void add_issue(const std::string& element_id,
                   const std::string& element_label,
                   AuditCategory category,
                   AuditSeverity severity,
                   const std::string& description,
                   const std::string& fix_suggestion);
};

} // namespace markamp::core
