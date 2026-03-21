/// @file AccessibilityAuditGate.cpp
/// @brief V20 P09-T04/T05/T06: Accessibility audit gate implementation.

#include "AccessibilityAuditGate.h"

#include "Events.h"
#include "Logger.h"

namespace markamp::core
{

AccessibilityAuditGate::AccessibilityAuditGate(EventBus& bus)
    : event_bus_(bus)
{
}

void AccessibilityAuditGate::record_finding(const A11yFinding& finding)
{
    findings_.push_back(finding);

    events::A11yFindingRecordedEvent evt;
    evt.finding_id = finding.id;
    evt.surface = finding.surface;
    evt.severity = static_cast<int>(finding.severity);
    evt.category = static_cast<int>(finding.category);
    event_bus_.publish(evt);

    MARKAMP_LOG_DEBUG("A11y finding: {} [{}] in {} - {}", finding.id,
                     static_cast<int>(finding.severity), finding.surface, finding.description);
}

auto AccessibilityAuditGate::findings_by_severity(A11ySeverity severity) const
    -> std::vector<A11yFinding>
{
    std::vector<A11yFinding> result;
    for (const auto& f : findings_)
    {
        if (f.severity == severity)
        {
            result.push_back(f);
        }
    }
    return result;
}

auto AccessibilityAuditGate::blocking_count() const -> int
{
    int count = 0;
    for (const auto& f : findings_)
    {
        if (f.is_blocking())
        {
            ++count;
        }
    }
    return count;
}

void AccessibilityAuditGate::record_visual_debt(const VisualDebtEntry& entry)
{
    visual_debt_.push_back(entry);

    events::VisualDebtRecordedEvent evt;
    evt.surface = entry.surface;
    evt.category = entry.category;
    event_bus_.publish(evt);

    MARKAMP_LOG_DEBUG("Visual debt recorded: {} [{}] - {}", entry.surface, entry.category,
                     entry.description);
}

auto AccessibilityAuditGate::unresolved_debt_count() const -> int
{
    int count = 0;
    for (const auto& d : visual_debt_)
    {
        if (!d.resolved)
        {
            ++count;
        }
    }
    return count;
}

void AccessibilityAuditGate::resolve_visual_debt(const std::string& surface,
                                                   const std::string& category)
{
    for (auto& d : visual_debt_)
    {
        if (d.surface == surface && d.category == category)
        {
            d.resolved = true;
            MARKAMP_LOG_DEBUG("Visual debt resolved: {} [{}]", surface, category);
        }
    }
}

auto AccessibilityAuditGate::gates_pass() const -> bool
{
    return blocking_count() == 0;
}

} // namespace markamp::core
