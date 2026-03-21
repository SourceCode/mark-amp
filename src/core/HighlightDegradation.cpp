/// @file HighlightDegradation.cpp
/// @brief V20 P07-T04/T05: Highlight degradation implementation.

#include "HighlightDegradation.h"

#include "Logger.h"

namespace markamp::core
{

auto HighlightDegradationService::determine_mode(int line_count) const -> DegradationMode
{
    if (line_count <= budget_.total_line_threshold)
    {
        return DegradationMode::kFull;
    }
    if (line_count <= budget_.total_line_threshold * 2)
    {
        return DegradationMode::kViewport;
    }
    if (line_count <= budget_.total_line_threshold * 5)
    {
        return DegradationMode::kBudgeted;
    }
    if (line_count <= budget_.total_line_threshold * 10)
    {
        return DegradationMode::kMinimal;
    }
    return DegradationMode::kNone;
}

void HighlightDegradationService::set_budget(const HighlightBudget& budget)
{
    budget_ = budget;
}

void HighlightDegradationService::record_pass(const std::string& document_id,
                                                int lines_highlighted, double time_ms)
{
    ++pass_count_;

    auto& st = states_[document_id];
    st.highlighted_lines += lines_highlighted;
    st.last_pass_ms = time_ms;

    MARKAMP_LOG_DEBUG("Highlight pass: {} lines={} time={:.1f}ms", document_id,
                      lines_highlighted, time_ms);
}

auto HighlightDegradationService::state(const std::string& document_id) const -> HighlightState
{
    auto it = states_.find(document_id);
    if (it != states_.end())
    {
        return it->second;
    }
    return {};
}

auto HighlightDegradationService::should_share_tokens(HighlightSurface surface) const -> bool
{
    switch (surface)
    {
    case HighlightSurface::kEditor:
    case HighlightSurface::kNotebookCell:
    case HighlightSurface::kMarkdownFence:
    case HighlightSurface::kDiffPanel:
    case HighlightSurface::kCanvasEmbed:
        return true;
    }
    return false;
}

auto HighlightDegradationService::shared_surfaces() const -> std::vector<HighlightSurface>
{
    return {HighlightSurface::kEditor, HighlightSurface::kNotebookCell,
            HighlightSurface::kMarkdownFence, HighlightSurface::kDiffPanel,
            HighlightSurface::kCanvasEmbed};
}

} // namespace markamp::core
