// ============================================================================
// File: src/rendering/DiffRenderer.cpp
// Phase 33: Version Diff & Comparison — DiffRenderer implementation
// ============================================================================
#include "DiffRenderer.h"

#include <iomanip>
#include <sstream>

namespace markamp::rendering
{

auto DiffRenderer::render_inline(const core::BlockDiffResult& diff_result) const -> std::string
{
    std::ostringstream oss;
    oss << R"(<div class="block-diff-inline">)"
        << "\n";
    oss << render_stats_bar(diff_result.stats);

    for (const auto& block_diff : diff_result.diffs)
    {
        if (block_diff.diff_type == core::DiffType::Unchanged)
        {
            oss << R"(  <div class="diff-block diff-unchanged">)" << block_diff.new_content
                << "</div>\n";
            continue;
        }

        oss << render_block_diff_inline(block_diff);
    }

    oss << "</div>\n";
    return oss.str();
}

auto DiffRenderer::render_side_by_side(const core::BlockDiffResult& diff_result) const
    -> std::string
{
    std::ostringstream oss;
    oss << R"(<div class="block-diff-side-by-side">)"
        << "\n";
    oss << render_stats_bar(diff_result.stats);

    oss << R"(  <div class="diff-columns">)"
        << "\n"
        << R"(    <div class="diff-column diff-old">)"
        << "\n"
        << R"(      <h3>Old Version</h3>)"
        << "\n";

    for (const auto& block_diff : diff_result.diffs)
    {
        const auto css = css_class_for(block_diff.diff_type);

        if (block_diff.diff_type == core::DiffType::Added)
        {
            oss << R"(      <div class="diff-block diff-placeholder"></div>)"
                << "\n";
        }
        else
        {
            oss << R"(      <div class="diff-block )" << css << R"(">)" << block_diff.old_content
                << "</div>\n";
        }
    }

    oss << "    </div>\n"
        << R"(    <div class="diff-column diff-new">)"
        << "\n"
        << R"(      <h3>New Version</h3>)"
        << "\n";

    for (const auto& block_diff : diff_result.diffs)
    {
        const auto css = css_class_for(block_diff.diff_type);

        if (block_diff.diff_type == core::DiffType::Deleted)
        {
            oss << R"(      <div class="diff-block diff-placeholder"></div>)"
                << "\n";
        }
        else
        {
            oss << R"(      <div class="diff-block )" << css << R"(">)" << block_diff.new_content
                << "</div>\n";
        }
    }

    oss << "    </div>\n  </div>\n</div>\n";
    return oss.str();
}

auto DiffRenderer::render_block_diff_inline(const core::BlockDiff& block_diff) const -> std::string
{
    const auto css = css_class_for(block_diff.diff_type);
    const auto label = label_for(block_diff.diff_type);

    std::ostringstream oss;
    oss << R"(  <div class="diff-block )" << css << R"(">)"
        << "\n"
        << R"(    <span class="diff-label">)" << label << "</span>\n";

    switch (block_diff.diff_type)
    {
        case core::DiffType::Added:
            oss << R"(    <div class="diff-content diff-add">)" << block_diff.new_content
                << "</div>\n";
            break;
        case core::DiffType::Deleted:
            oss << R"(    <div class="diff-content diff-delete">)" << block_diff.old_content
                << "</div>\n";
            break;
        case core::DiffType::Modified:
            oss << R"(    <div class="diff-content diff-delete">)" << block_diff.old_content
                << "</div>\n"
                << R"(    <div class="diff-content diff-add">)" << block_diff.new_content
                << "</div>\n";
            break;
        case core::DiffType::Moved:
            oss << R"(    <div class="diff-content diff-moved">)" << block_diff.new_content
                << "</div>\n";
            break;
        case core::DiffType::Unchanged:
            oss << R"(    <div class="diff-content">)" << block_diff.new_content << "</div>\n";
            break;
    }

    oss << "  </div>\n";
    return oss.str();
}

auto DiffRenderer::render_stats_bar(const core::DiffStats& stats) -> std::string
{
    std::ostringstream oss;
    oss << R"(  <div class="diff-stats-bar">)"
        << "\n"
        << R"(    <span class="diff-stat-total">)" << stats.total_blocks << " blocks</span>\n"
        << R"(    <span class="diff-stat-add">+)" << stats.added_blocks << "</span>\n"
        << R"(    <span class="diff-stat-del">-)" << stats.deleted_blocks << "</span>\n"
        << R"(    <span class="diff-stat-mod">~)" << stats.modified_blocks << "</span>\n";

    if (stats.moved_blocks > 0)
    {
        oss << R"(    <span class="diff-stat-mov">→)" << stats.moved_blocks << "</span>\n";
    }

    oss << R"(    <span class="diff-stat-pct">)" << std::fixed << std::setprecision(1)
        << stats.change_percentage() << "% changed</span>\n"
        << "  </div>\n";

    return oss.str();
}

auto DiffRenderer::css_class_for(core::DiffType type) -> std::string
{
    switch (type)
    {
        case core::DiffType::Unchanged:
            return "diff-unchanged";
        case core::DiffType::Added:
            return "diff-added";
        case core::DiffType::Deleted:
            return "diff-deleted";
        case core::DiffType::Modified:
            return "diff-modified";
        case core::DiffType::Moved:
            return "diff-moved";
    }
    return "diff-unknown";
}

auto DiffRenderer::label_for(core::DiffType type) -> std::string_view
{
    switch (type)
    {
        case core::DiffType::Unchanged:
            return "Unchanged";
        case core::DiffType::Added:
            return "Added";
        case core::DiffType::Deleted:
            return "Deleted";
        case core::DiffType::Modified:
            return "Modified";
        case core::DiffType::Moved:
            return "Moved";
    }
    return "Unknown";
}

} // namespace markamp::rendering
