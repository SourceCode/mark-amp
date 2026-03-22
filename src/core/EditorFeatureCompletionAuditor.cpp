/// @file EditorFeatureCompletionAuditor.cpp
/// @brief V23 Phase 05 — EditorFeatureCompletionAuditor implementation.

#include "EditorFeatureCompletionAuditor.h"

#include <sstream>

namespace markamp::core
{

// ── Registration ──

void EditorFeatureCompletionAuditor::add_item(EditorFeatureCoverageItem item)
{
    items_.push_back(std::move(item));
}

void EditorFeatureCompletionAuditor::add_items(std::vector<EditorFeatureCoverageItem> items)
{
    for (auto& item : items) items_.push_back(std::move(item));
}

// ── Queries ──

auto EditorFeatureCompletionAuditor::item_count() const noexcept -> std::size_t
{
    return items_.size();
}

auto EditorFeatureCompletionAuditor::items_by_area(EditorFeatureArea area) const
    -> std::vector<const EditorFeatureCoverageItem*>
{
    std::vector<const EditorFeatureCoverageItem*> result;
    for (const auto& item : items_)
        if (item.area == area) result.push_back(&item);
    return result;
}

auto EditorFeatureCompletionAuditor::complete_items() const
    -> std::vector<const EditorFeatureCoverageItem*>
{
    std::vector<const EditorFeatureCoverageItem*> result;
    for (const auto& item : items_)
        if (item.is_complete()) result.push_back(&item);
    return result;
}

auto EditorFeatureCompletionAuditor::incomplete_items() const
    -> std::vector<const EditorFeatureCoverageItem*>
{
    std::vector<const EditorFeatureCoverageItem*> result;
    for (const auto& item : items_)
        if (!item.is_complete()) result.push_back(&item);
    return result;
}

auto EditorFeatureCompletionAuditor::placeholder_items() const
    -> std::vector<const EditorFeatureCoverageItem*>
{
    std::vector<const EditorFeatureCoverageItem*> result;
    for (const auto& item : items_)
        if (!item.is_implemented) result.push_back(&item);
    return result;
}

// ── Report ──

auto EditorFeatureCompletionAuditor::gap_report() const -> EditorFeatureGapReport
{
    EditorFeatureGapReport report;
    report.total_features = items_.size();
    for (const auto& item : items_)
    {
        if (item.is_complete())
            ++report.implemented;
        else
        {
            ++report.placeholder;
            switch (item.area)
            {
            case EditorFeatureArea::kSearch:       ++report.search_gaps; break;
            case EditorFeatureArea::kPreview:      ++report.preview_gaps; break;
            case EditorFeatureArea::kNavigation:   ++report.navigation_gaps; break;
            case EditorFeatureArea::kDiagnostics:  ++report.diagnostics_gaps; break;
            case EditorFeatureArea::kCodeLens:     ++report.code_lens_gaps; break;
            case EditorFeatureArea::kPeek:         ++report.peek_gaps; break;
            case EditorFeatureArea::kSymbolLookup: ++report.symbol_lookup_gaps; break;
            case EditorFeatureArea::kQuickFix:     ++report.quick_fix_gaps; break;
            }
        }
    }
    return report;
}

// ── Clear ──

void EditorFeatureCompletionAuditor::clear()
{
    items_.clear();
}

// ── Export ──

auto EditorFeatureCompletionAuditor::export_json() const -> std::string
{
    auto r = gap_report();
    std::ostringstream ss;
    ss << "{\n"
       << "  \"total\": " << r.total_features << ",\n"
       << "  \"implemented\": " << r.implemented << ",\n"
       << "  \"placeholder\": " << r.placeholder << ",\n"
       << "  \"coverage_pct\": " << r.coverage_pct() << ",\n"
       << "  \"gaps_by_area\": {\n"
       << "    \"search\": " << r.search_gaps << ",\n"
       << "    \"preview\": " << r.preview_gaps << ",\n"
       << "    \"navigation\": " << r.navigation_gaps << ",\n"
       << "    \"diagnostics\": " << r.diagnostics_gaps << ",\n"
       << "    \"code_lens\": " << r.code_lens_gaps << ",\n"
       << "    \"peek\": " << r.peek_gaps << ",\n"
       << "    \"symbol_lookup\": " << r.symbol_lookup_gaps << ",\n"
       << "    \"quick_fix\": " << r.quick_fix_gaps << "\n"
       << "  },\n"
       << "  \"items\": [\n";
    for (std::size_t i = 0; i < items_.size(); ++i)
    {
        const auto& item = items_[i];
        ss << "    {"
           << "\"area\": \"" << editor_feature_area_label(item.area) << "\", "
           << "\"feature\": \"" << item.feature_name << "\", "
           << "\"complete\": " << (item.is_complete() ? "true" : "false")
           << "}";
        if (i + 1 < items_.size()) ss << ",";
        ss << "\n";
    }
    ss << "  ]\n}\n";
    return ss.str();
}

auto EditorFeatureCompletionAuditor::export_markdown() const -> std::string
{
    auto r = gap_report();
    std::ostringstream ss;
    ss << "# V23 Editor Feature Completion Report\n\n"
       << "**Coverage:** " << r.coverage_pct() << "% (" << r.implemented << "/" << r.total_features << ")\n\n"
       << "| Area | Gaps |\n|---|---|\n"
       << "| Search | " << r.search_gaps << " |\n"
       << "| Preview | " << r.preview_gaps << " |\n"
       << "| Navigation | " << r.navigation_gaps << " |\n"
       << "| Diagnostics | " << r.diagnostics_gaps << " |\n"
       << "| CodeLens | " << r.code_lens_gaps << " |\n"
       << "| Peek | " << r.peek_gaps << " |\n"
       << "| SymbolLookup | " << r.symbol_lookup_gaps << " |\n"
       << "| QuickFix | " << r.quick_fix_gaps << " |\n";

    auto gaps = incomplete_items();
    if (!gaps.empty())
    {
        ss << "\n## Incomplete Features\n\n"
           << "| Area | Feature | Implemented | Indexed | Errors | Async |\n"
           << "|---|---|---|---|---|---|\n";
        for (const auto* item : gaps)
        {
            ss << "| " << editor_feature_area_label(item->area)
               << " | " << item->feature_name
               << " | " << (item->is_implemented ? "✅" : "❌")
               << " | " << (item->is_indexed ? "✅" : "❌")
               << " | " << (item->has_error_handling ? "✅" : "❌")
               << " | " << (item->has_async_support ? "✅" : "❌")
               << " |\n";
        }
    }

    return ss.str();
}

} // namespace markamp::core
