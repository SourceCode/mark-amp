// ============================================================================
// File: src/rendering/DiffRenderer.h
// Phase 33: Version Diff & Comparison — Diff HTML rendering
// ============================================================================
#pragma once

#include "../core/DiffTypes.h"

#include <string>

namespace markamp::rendering
{

/// Renders block-level diffs as HTML for the preview panel.
class DiffRenderer
{
public:
    DiffRenderer() = default;

    /// Render the full diff result as inline HTML.
    [[nodiscard]] auto render_inline(const core::BlockDiffResult& diff_result) const -> std::string;

    /// Render the full diff result as side-by-side HTML.
    [[nodiscard]] auto render_side_by_side(const core::BlockDiffResult& diff_result) const
        -> std::string;

    /// Render a single block diff as inline HTML.
    [[nodiscard]] auto render_block_diff_inline(const core::BlockDiff& block_diff) const
        -> std::string;

    /// Render a stats summary bar.
    [[nodiscard]] static auto render_stats_bar(const core::DiffStats& stats) -> std::string;

    /// Number of render modes (inline and side-by-side).
    [[nodiscard]] static constexpr auto total_render_modes() noexcept -> int
    {
        return 2;
    }

private:
    /// CSS class for a diff type.
    [[nodiscard]] static auto css_class_for(core::DiffType type) -> std::string;

    /// Label for a diff type.
    [[nodiscard]] static auto label_for(core::DiffType type) -> std::string_view;
};

} // namespace markamp::rendering
