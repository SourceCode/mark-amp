// ============================================================================
// File: src/testing/AccessibilityAuditor.cpp
// Phase 50: UI Integration Testing Harness — Accessibility auditor
// ============================================================================
#include "AccessibilityAuditor.h"

namespace markamp::testing
{

void AccessibilityAuditor::add_element(const std::string& id,
                                       const std::string& accessible_name,
                                       const std::string& role,
                                       bool focusable)
{
    elements_.push_back(
        {.id = id, .accessible_name = accessible_name, .role = role, .focusable = focusable});
}

auto AccessibilityAuditor::audit() const -> A11yAuditResult
{
    A11yAuditResult result;
    result.elements_scanned = static_cast<int>(elements_.size());

    for (const auto& elem : elements_)
    {
        // Check for accessible name.
        if (!elem.accessible_name.empty())
        {
            ++result.accessible_names_found;
        }
        else
        {
            result.findings.push_back({.element_id = elem.id,
                                       .rule_id = "name-required",
                                       .description = "Element missing accessible name",
                                       .severity = A11ySeverity::Error});
        }

        // Check for role.
        if (elem.role.empty())
        {
            result.findings.push_back({.element_id = elem.id,
                                       .rule_id = "role-required",
                                       .description = "Element missing ARIA role",
                                       .severity = A11ySeverity::Warning});
        }

        // Track focusable elements.
        if (elem.focusable)
        {
            ++result.keyboard_focusable;
        }
    }

    return result;
}

} // namespace markamp::testing
