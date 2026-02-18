// ============================================================================
// File: src/core/AccessibilityAudit.cpp
// Phase 31: Accessibility System — Built-in accessibility scanner
// ============================================================================

#include "AccessibilityAudit.h"

#include <algorithm>
#include <sstream>

namespace markamp::core
{

AccessibilityAudit::AccessibilityAudit() = default;

// ── Individual checks ──────────────────────────────────────────────────────

void AccessibilityAudit::check_contrast(const std::string& element_id,
                                        const std::string& element_label,
                                        double actual_ratio,
                                        double min_ratio)
{
    if (actual_ratio < min_ratio)
    {
        std::ostringstream desc;
        desc << "Contrast ratio " << actual_ratio << ":1 is below the minimum " << min_ratio
             << ":1";

        add_issue(element_id,
                  element_label,
                  AuditCategory::kContrast,
                  AuditSeverity::kCritical,
                  desc.str(),
                  "Increase foreground/background contrast to at least " +
                      std::to_string(static_cast<int>(min_ratio)) + ":1");
    }
}

void AccessibilityAudit::check_missing_label(const std::string& element_id,
                                             const std::string& element_label,
                                             bool has_label)
{
    if (!has_label)
    {
        add_issue(element_id,
                  element_label,
                  AuditCategory::kLabel,
                  AuditSeverity::kCritical,
                  "Element is missing an accessible label",
                  "Add an aria-label or aria-labelledby attribute");
    }
}

void AccessibilityAudit::check_missing_alt_text(const std::string& element_id,
                                                const std::string& element_label,
                                                bool has_alt_text)
{
    if (!has_alt_text)
    {
        add_issue(element_id,
                  element_label,
                  AuditCategory::kAltText,
                  AuditSeverity::kWarning,
                  "Image is missing alternative text",
                  "Add descriptive alt text to the image");
    }
}

void AccessibilityAudit::check_keyboard_access(const std::string& element_id,
                                               const std::string& element_label,
                                               bool is_keyboard_reachable)
{
    if (!is_keyboard_reachable)
    {
        add_issue(element_id,
                  element_label,
                  AuditCategory::kKeyboard,
                  AuditSeverity::kCritical,
                  "Element is not reachable via keyboard navigation",
                  "Add tabindex or ensure element is in the tab order");
    }
}

void AccessibilityAudit::check_focus_ring(const std::string& element_id,
                                          const std::string& element_label,
                                          bool has_focus_ring)
{
    if (!has_focus_ring)
    {
        add_issue(element_id,
                  element_label,
                  AuditCategory::kFocusRing,
                  AuditSeverity::kWarning,
                  "Element has no visible focus indicator",
                  "Add a visible focus ring using FocusRingStyler");
    }
}

void AccessibilityAudit::check_role(const std::string& element_id,
                                    const std::string& element_label,
                                    bool has_role)
{
    if (!has_role)
    {
        add_issue(element_id,
                  element_label,
                  AuditCategory::kRole,
                  AuditSeverity::kInfo,
                  "Custom control is missing an accessible role",
                  "Add an appropriate ARIA role (e.g., button, tab, tree)");
    }
}

// ── Audit result ───────────────────────────────────────────────────────────

auto AccessibilityAudit::result() const -> AuditResult
{
    AuditResult res;
    res.issues = issues_;

    for (const auto& issue : issues_)
    {
        switch (issue.severity)
        {
            case AuditSeverity::kCritical:
                ++res.summary.critical_count;
                break;
            case AuditSeverity::kWarning:
                ++res.summary.warning_count;
                break;
            case AuditSeverity::kInfo:
                ++res.summary.info_count;
                break;
        }
    }

    res.passed = (res.summary.critical_count == 0);
    return res;
}

auto AccessibilityAudit::issue_count() const -> int
{
    return static_cast<int>(issues_.size());
}

auto AccessibilityAudit::issues_by_severity(AuditSeverity severity) const -> std::vector<AuditIssue>
{
    std::vector<AuditIssue> filtered;
    for (const auto& issue : issues_)
    {
        if (issue.severity == severity)
        {
            filtered.push_back(issue);
        }
    }
    return filtered;
}

auto AccessibilityAudit::issues_by_category(AuditCategory category) const -> std::vector<AuditIssue>
{
    std::vector<AuditIssue> filtered;
    for (const auto& issue : issues_)
    {
        if (issue.category == category)
        {
            filtered.push_back(issue);
        }
    }
    return filtered;
}

auto AccessibilityAudit::passed() const -> bool
{
    return std::none_of(issues_.begin(),
                        issues_.end(),
                        [](const AuditIssue& issue)
                        { return issue.severity == AuditSeverity::kCritical; });
}

void AccessibilityAudit::reset()
{
    issues_.clear();
}

// ── Export ──────────────────────────────────────────────────────────────────

auto AccessibilityAudit::to_json() const -> std::string
{
    const auto res = result();
    std::ostringstream oss;
    oss << "{\n";
    oss << "  \"passed\": " << (res.passed ? "true" : "false") << ",\n";
    oss << "  \"summary\": {\n";
    oss << "    \"critical\": " << res.summary.critical_count << ",\n";
    oss << "    \"warning\": " << res.summary.warning_count << ",\n";
    oss << "    \"info\": " << res.summary.info_count << "\n";
    oss << "  },\n";
    oss << "  \"issues\": [\n";

    for (std::size_t idx = 0; idx < res.issues.size(); ++idx)
    {
        const auto& issue = res.issues.at(idx);
        oss << "    {\n";
        oss << "      \"element_id\": \"" << issue.element_id << "\",\n";
        oss << "      \"element_label\": \"" << issue.element_label << "\",\n";
        oss << "      \"category\": \"" << category_name(issue.category) << "\",\n";
        oss << "      \"severity\": \"" << severity_name(issue.severity) << "\",\n";
        oss << "      \"description\": \"" << issue.description << "\",\n";
        oss << "      \"fix_suggestion\": \"" << issue.fix_suggestion << "\"\n";
        oss << "    }";
        if (idx + 1 < res.issues.size())
        {
            oss << ",";
        }
        oss << "\n";
    }

    oss << "  ]\n";
    oss << "}";
    return oss.str();
}

// ── Helpers ────────────────────────────────────────────────────────────────

auto AccessibilityAudit::severity_name(AuditSeverity severity) -> std::string
{
    switch (severity)
    {
        case AuditSeverity::kCritical:
            return "critical";
        case AuditSeverity::kWarning:
            return "warning";
        case AuditSeverity::kInfo:
            return "info";
    }
    return "unknown";
}

auto AccessibilityAudit::category_name(AuditCategory category) -> std::string
{
    switch (category)
    {
        case AuditCategory::kContrast:
            return "contrast";
        case AuditCategory::kLabel:
            return "label";
        case AuditCategory::kKeyboard:
            return "keyboard";
        case AuditCategory::kAltText:
            return "alt_text";
        case AuditCategory::kFocusRing:
            return "focus_ring";
        case AuditCategory::kRole:
            return "role";
    }
    return "unknown";
}

// ── Private ────────────────────────────────────────────────────────────────

void AccessibilityAudit::add_issue(const std::string& element_id,
                                   const std::string& element_label,
                                   AuditCategory category,
                                   AuditSeverity severity,
                                   const std::string& description,
                                   const std::string& fix_suggestion)
{
    AuditIssue issue;
    issue.element_id = element_id;
    issue.element_label = element_label;
    issue.category = category;
    issue.severity = severity;
    issue.description = description;
    issue.fix_suggestion = fix_suggestion;
    issues_.push_back(issue);
}

} // namespace markamp::core
