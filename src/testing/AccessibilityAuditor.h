// ============================================================================
// File: src/testing/AccessibilityAuditor.h
// Phase 50: UI Integration Testing Harness — Accessibility auditor
// ============================================================================
#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::testing
{

/// Severity of an accessibility finding.
enum class A11ySeverity : uint8_t
{
    Error,   ///< WCAG violation
    Warning, ///< Best practice issue
    Info     ///< Informational
};

/// A single accessibility finding.
struct A11yFinding
{
    std::string element_id;
    std::string rule_id; ///< e.g., "axe-name-role-value"
    std::string description;
    A11ySeverity severity{A11ySeverity::Error};
};

/// Result of an accessibility audit.
struct A11yAuditResult
{
    std::vector<A11yFinding> findings;
    int elements_scanned{0};
    int accessible_names_found{0};
    int keyboard_focusable{0};

    [[nodiscard]] auto error_count() const -> int
    {
        int count = 0;
        for (const auto& f : findings)
        {
            if (f.severity == A11ySeverity::Error)
            {
                ++count;
            }
        }
        return count;
    }

    [[nodiscard]] auto compliance_rate() const -> double
    {
        if (elements_scanned == 0)
        {
            return 100.0;
        }
        return ((static_cast<double>(elements_scanned) - static_cast<double>(error_count())) /
                static_cast<double>(elements_scanned)) *
               100.0;
    }
};

/// Accessibility auditor that scans element metadata.
class AccessibilityAuditor
{
public:
    AccessibilityAuditor() = default;

    /// Add an element for auditing.
    void add_element(const std::string& id,
                     const std::string& accessible_name,
                     const std::string& role,
                     bool focusable);

    /// Run the audit.
    [[nodiscard]] auto audit() const -> A11yAuditResult;

private:
    struct ElementInfo
    {
        std::string id;
        std::string accessible_name;
        std::string role;
        bool focusable{false};
    };
    std::vector<ElementInfo> elements_;
};

} // namespace markamp::testing
