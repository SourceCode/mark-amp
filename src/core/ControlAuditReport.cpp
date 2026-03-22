/// @file ControlAuditReport.cpp
/// @brief V21 Phase 01 — ControlAuditReport implementation.

#include "ControlAuditReport.h"

#include <sstream>

namespace markamp::core
{

auto ControlAuditReport::generate(const ControlActionManifest& manifest,
                                   const ControlExecutionTracer* tracer) const
    -> std::vector<ControlAuditEntry>
{
    std::vector<ControlAuditEntry> entries;
    entries.reserve(manifest.action_count());

    for (const auto* action : manifest.all_actions())
    {
        ControlAuditEntry entry;
        entry.action_id = action->action_id;
        entry.label = action->label;
        entry.category = action->category;
        entry.validation_status = action->validation_status;
        entry.has_handler = action->has_handler();
        entry.has_enablement = static_cast<bool>(action->enablement);
        entry.has_visibility = static_cast<bool>(action->visibility);
        entry.surfaces = action->surfaces;

        if (tracer != nullptr)
        {
            auto stats = tracer->stats_for(action->action_id);
            entry.activation_count = stats.total_activations;
            entry.success_count = stats.success_count;
            entry.failure_count = stats.failure_count;
        }

        entries.push_back(std::move(entry));
    }

    return entries;
}

auto ControlAuditReport::summarize(const std::vector<ControlAuditEntry>& entries) const
    -> ControlAuditSummary
{
    ControlAuditSummary summary;
    summary.total_actions = static_cast<int>(entries.size());

    for (const auto& entry : entries)
    {
        switch (entry.validation_status)
        {
        case ActionValidationStatus::kLive: ++summary.live_actions; break;
        case ActionValidationStatus::kPartial: ++summary.partial_actions; break;
        case ActionValidationStatus::kStub: ++summary.stub_actions; break;
        case ActionValidationStatus::kDead: ++summary.dead_actions; break;
        case ActionValidationStatus::kDuplicate: ++summary.duplicate_actions; break;
        case ActionValidationStatus::kDeprecated: ++summary.deprecated_actions; break;
        case ActionValidationStatus::kGated: ++summary.gated_actions; break;
        }

        summary.total_activations += entry.activation_count;
        summary.total_failures += entry.failure_count;

        if (entry.activation_count == 0)
        {
            ++summary.never_activated_count;
        }
    }

    return summary;
}

auto ControlAuditReport::summarize(const ControlActionManifest& manifest,
                                    const ControlExecutionTracer* tracer) const
    -> ControlAuditSummary
{
    auto entries = generate(manifest, tracer);
    return summarize(entries);
}

auto ControlAuditReport::export_json(const std::vector<ControlAuditEntry>& entries) const
    -> std::string
{
    std::ostringstream oss;
    oss << "{\n  \"audit_entries\": [\n";

    bool first = true;
    for (const auto& entry : entries)
    {
        if (!first)
        {
            oss << ",\n";
        }
        first = false;

        oss << "    {\n"
            << R"(      "action_id": ")" << entry.action_id << "\",\n"
            << R"(      "label": ")" << entry.label << "\",\n"
            << R"(      "category": ")" << entry.category << "\",\n"
            << R"(      "status": ")" << validation_status_label(entry.validation_status) << "\",\n"
            << R"(      "has_handler": )" << (entry.has_handler ? "true" : "false") << ",\n"
            << R"(      "has_enablement": )" << (entry.has_enablement ? "true" : "false") << ",\n"
            << R"(      "has_visibility": )" << (entry.has_visibility ? "true" : "false") << ",\n"
            << R"(      "activation_count": )" << entry.activation_count << ",\n"
            << R"(      "success_count": )" << entry.success_count << ",\n"
            << R"(      "failure_count": )" << entry.failure_count << "\n"
            << "    }";
    }

    oss << "\n  ]\n}";
    return oss.str();
}

auto ControlAuditReport::export_markdown(const std::vector<ControlAuditEntry>& entries,
                                          const ControlAuditSummary& summary) const
    -> std::string
{
    std::ostringstream oss;

    // Header
    oss << "# V21 Control Audit Report\n\n";
    oss << "## Summary\n\n";
    oss << "| Metric | Value |\n";
    oss << "|--------|-------|\n";
    oss << "| Total Actions | " << summary.total_actions << " |\n";
    oss << "| Live | " << summary.live_actions << " |\n";
    oss << "| Partial | " << summary.partial_actions << " |\n";
    oss << "| Stub | " << summary.stub_actions << " |\n";
    oss << "| Dead | " << summary.dead_actions << " |\n";
    oss << "| Gated | " << summary.gated_actions << " |\n";
    oss << "| Health | " << summary.health_pct() << "% |\n";
    oss << "| Exit Criteria | " << (summary.passes_exit_criteria() ? "PASS" : "FAIL") << " |\n";
    oss << "\n";

    // Table
    oss << "## Action Matrix\n\n";
    oss << "| Action ID | Label | Category | Status | Handler | Activations |\n";
    oss << "|-----------|-------|----------|--------|---------|-------------|\n";

    for (const auto& entry : entries)
    {
        oss << "| " << entry.action_id
            << " | " << entry.label
            << " | " << entry.category
            << " | " << validation_status_label(entry.validation_status)
            << " | " << (entry.has_handler ? "✓" : "✗")
            << " | " << entry.activation_count
            << " |\n";
    }

    return oss.str();
}

} // namespace markamp::core
