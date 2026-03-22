/// @file PersistenceCompletionAuditor.cpp
/// @brief V23 Phase 04 — PersistenceCompletionAuditor implementation.

#include "PersistenceCompletionAuditor.h"

#include <sstream>

namespace markamp::core
{

// ── Registration ──

void PersistenceCompletionAuditor::add_item(PersistenceCoverageItem item)
{
    items_.push_back(std::move(item));
}

void PersistenceCompletionAuditor::add_items(std::vector<PersistenceCoverageItem> items)
{
    for (auto& item : items) items_.push_back(std::move(item));
}

// ── Queries ──

auto PersistenceCompletionAuditor::item_count() const noexcept -> std::size_t
{
    return items_.size();
}

auto PersistenceCompletionAuditor::items_by_capability(PersistenceCapability cap) const
    -> std::vector<const PersistenceCoverageItem*>
{
    std::vector<const PersistenceCoverageItem*> result;
    for (const auto& item : items_)
        if (item.capability == cap) result.push_back(&item);
    return result;
}

auto PersistenceCompletionAuditor::items_by_artifact(PersistenceArtifactKind kind) const
    -> std::vector<const PersistenceCoverageItem*>
{
    std::vector<const PersistenceCoverageItem*> result;
    for (const auto& item : items_)
        if (item.artifact_kind == kind) result.push_back(&item);
    return result;
}

auto PersistenceCompletionAuditor::complete_items() const
    -> std::vector<const PersistenceCoverageItem*>
{
    std::vector<const PersistenceCoverageItem*> result;
    for (const auto& item : items_)
        if (item.is_complete()) result.push_back(&item);
    return result;
}

auto PersistenceCompletionAuditor::incomplete_items() const
    -> std::vector<const PersistenceCoverageItem*>
{
    std::vector<const PersistenceCoverageItem*> result;
    for (const auto& item : items_)
        if (!item.is_complete()) result.push_back(&item);
    return result;
}

// ── Report ──

auto PersistenceCompletionAuditor::gap_report() const -> PersistenceGapReport
{
    PersistenceGapReport report;
    report.total_items = items_.size();
    for (const auto& item : items_)
    {
        if (item.is_complete())
            ++report.complete;
        else
            ++report.incomplete;

        if (item.is_authoritative)  ++report.authoritative;
        if (item.is_transactional)  ++report.transactional;
        if (item.has_journal_support) ++report.with_journal;
    }
    return report;
}

// ── Clear ──

void PersistenceCompletionAuditor::clear()
{
    items_.clear();
}

// ── Export ──

auto PersistenceCompletionAuditor::export_json() const -> std::string
{
    auto r = gap_report();
    std::ostringstream ss;
    ss << "{\n"
       << "  \"total\": " << r.total_items << ",\n"
       << "  \"complete\": " << r.complete << ",\n"
       << "  \"incomplete\": " << r.incomplete << ",\n"
       << "  \"coverage_pct\": " << r.coverage_pct() << ",\n"
       << "  \"authoritative\": " << r.authoritative << ",\n"
       << "  \"transactional\": " << r.transactional << ",\n"
       << "  \"with_journal\": " << r.with_journal << ",\n"
       << "  \"items\": [\n";
    for (std::size_t i = 0; i < items_.size(); ++i)
    {
        const auto& item = items_[i];
        ss << "    {"
           << "\"capability\": \"" << persistence_capability_label(item.capability) << "\", "
           << "\"artifact\": \"" << persistence_artifact_label(item.artifact_kind) << "\", "
           << "\"complete\": " << (item.is_complete() ? "true" : "false")
           << "}";
        if (i + 1 < items_.size()) ss << ",";
        ss << "\n";
    }
    ss << "  ]\n}\n";
    return ss.str();
}

auto PersistenceCompletionAuditor::export_markdown() const -> std::string
{
    auto r = gap_report();
    std::ostringstream ss;
    ss << "# V23 Persistence Completion Report\n\n"
       << "**Coverage:** " << r.coverage_pct() << "% (" << r.complete << "/" << r.total_items << ")\n\n"
       << "| Metric | Count |\n|---|---|\n"
       << "| Authoritative | " << r.authoritative << " |\n"
       << "| Transactional | " << r.transactional << " |\n"
       << "| With Journal | " << r.with_journal << " |\n\n";

    auto gaps = incomplete_items();
    if (!gaps.empty())
    {
        ss << "## Incomplete Capabilities\n\n"
           << "| Capability | Artifact | Auth | Txn | Failure | Journal |\n"
           << "|---|---|---|---|---|---|\n";
        for (const auto* item : gaps)
        {
            ss << "| " << persistence_capability_label(item->capability)
               << " | " << persistence_artifact_label(item->artifact_kind)
               << " | " << (item->is_authoritative ? "✅" : "❌")
               << " | " << (item->is_transactional ? "✅" : "❌")
               << " | " << (item->has_failure_handling ? "✅" : "❌")
               << " | " << (item->has_journal_support ? "✅" : "❌")
               << " |\n";
        }
    }

    return ss.str();
}

} // namespace markamp::core
