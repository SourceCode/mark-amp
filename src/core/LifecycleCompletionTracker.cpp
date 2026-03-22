/// @file LifecycleCompletionTracker.cpp
/// @brief V23 Phase 03 — LifecycleCompletionTracker implementation.

#include "LifecycleCompletionTracker.h"

#include <sstream>

namespace markamp::core
{

// ── Registration ──

void LifecycleCompletionTracker::add_item(LifecycleCoverageItem item)
{
    items_.push_back(std::move(item));
}

void LifecycleCompletionTracker::add_items(std::vector<LifecycleCoverageItem> items)
{
    for (auto& item : items) items_.push_back(std::move(item));
}

// ── Queries ──

auto LifecycleCompletionTracker::item_count() const noexcept -> std::size_t
{
    return items_.size();
}

auto LifecycleCompletionTracker::items_by_kind(ArtifactKind kind) const
    -> std::vector<const LifecycleCoverageItem*>
{
    std::vector<const LifecycleCoverageItem*> result;
    for (const auto& item : items_)
        if (item.artifact_kind == kind) result.push_back(&item);
    return result;
}

auto LifecycleCompletionTracker::items_by_stage(LifecycleStage stage) const
    -> std::vector<const LifecycleCoverageItem*>
{
    std::vector<const LifecycleCoverageItem*> result;
    for (const auto& item : items_)
        if (item.stage == stage) result.push_back(&item);
    return result;
}

auto LifecycleCompletionTracker::complete_items() const
    -> std::vector<const LifecycleCoverageItem*>
{
    std::vector<const LifecycleCoverageItem*> result;
    for (const auto& item : items_)
        if (item.is_complete()) result.push_back(&item);
    return result;
}

auto LifecycleCompletionTracker::incomplete_items() const
    -> std::vector<const LifecycleCoverageItem*>
{
    std::vector<const LifecycleCoverageItem*> result;
    for (const auto& item : items_)
        if (!item.is_complete()) result.push_back(&item);
    return result;
}

auto LifecycleCompletionTracker::gaps_for_kind(ArtifactKind kind) const
    -> std::vector<const LifecycleCoverageItem*>
{
    std::vector<const LifecycleCoverageItem*> result;
    for (const auto& item : items_)
        if (item.artifact_kind == kind && !item.is_complete())
            result.push_back(&item);
    return result;
}

// ── Report ──

auto LifecycleCompletionTracker::gap_report() const -> LifecycleGapReport
{
    LifecycleGapReport report;
    report.total_items = items_.size();
    for (const auto& item : items_)
    {
        if (item.is_complete())
            ++report.complete;
        else
        {
            ++report.incomplete;
            switch (item.artifact_kind)
            {
            case ArtifactKind::kTextFile:  ++report.text_file_gaps; break;
            case ArtifactKind::kNotebook:  ++report.notebook_gaps; break;
            case ArtifactKind::kCanvas:    ++report.canvas_gaps; break;
            case ArtifactKind::kWorkspace: ++report.workspace_gaps; break;
            case ArtifactKind::kSession:   ++report.session_gaps; break;
            }
        }
    }
    return report;
}

// ── Clear ──

void LifecycleCompletionTracker::clear()
{
    items_.clear();
}

// ── Export ──

auto LifecycleCompletionTracker::export_json() const -> std::string
{
    auto r = gap_report();
    std::ostringstream ss;
    ss << "{\n"
       << "  \"total\": " << r.total_items << ",\n"
       << "  \"complete\": " << r.complete << ",\n"
       << "  \"incomplete\": " << r.incomplete << ",\n"
       << "  \"coverage_pct\": " << r.coverage_pct() << ",\n"
       << "  \"gaps_by_kind\": {\n"
       << "    \"text_file\": " << r.text_file_gaps << ",\n"
       << "    \"notebook\": " << r.notebook_gaps << ",\n"
       << "    \"canvas\": " << r.canvas_gaps << ",\n"
       << "    \"workspace\": " << r.workspace_gaps << ",\n"
       << "    \"session\": " << r.session_gaps << "\n"
       << "  },\n"
       << "  \"items\": [\n";
    for (std::size_t i = 0; i < items_.size(); ++i)
    {
        const auto& item = items_[i];
        ss << "    {"
           << "\"kind\": \"" << artifact_kind_label(item.artifact_kind) << "\", "
           << "\"stage\": \"" << lifecycle_stage_label(item.stage) << "\", "
           << "\"complete\": " << (item.is_complete() ? "true" : "false")
           << "}";
        if (i + 1 < items_.size()) ss << ",";
        ss << "\n";
    }
    ss << "  ]\n}\n";
    return ss.str();
}

auto LifecycleCompletionTracker::export_markdown() const -> std::string
{
    auto r = gap_report();
    std::ostringstream ss;
    ss << "# V23 Lifecycle Completion Report\n\n"
       << "**Coverage:** " << r.coverage_pct() << "% (" << r.complete << "/" << r.total_items << ")\n\n"
       << "| Artifact Kind | Gaps |\n|---|---|\n"
       << "| TextFile | " << r.text_file_gaps << " |\n"
       << "| Notebook | " << r.notebook_gaps << " |\n"
       << "| Canvas | " << r.canvas_gaps << " |\n"
       << "| Workspace | " << r.workspace_gaps << " |\n"
       << "| Session | " << r.session_gaps << " |\n\n";

    auto gaps = incomplete_items();
    if (!gaps.empty())
    {
        ss << "## Incomplete Items\n\n"
           << "| Kind | Stage | Authoritative | Shell Owned | Stable ID |\n"
           << "|---|---|---|---|---|\n";
        for (const auto* item : gaps)
        {
            ss << "| " << artifact_kind_label(item->artifact_kind)
               << " | " << lifecycle_stage_label(item->stage)
               << " | " << (item->is_authoritative ? "✅" : "❌")
               << " | " << (item->is_shell_owned ? "✅" : "❌")
               << " | " << (item->has_stable_id ? "✅" : "❌")
               << " |\n";
        }
    }

    return ss.str();
}

} // namespace markamp::core
