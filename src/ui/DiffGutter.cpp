// ============================================================================
// File: src/ui/DiffGutter.cpp
// Phase 46: Diff Editor and Merge Tool — Diff gutter indicator model
// ============================================================================
#include "DiffGutter.h"

namespace markamp::ui
{

void DiffGutterModel::build_from_diff(const core::DiffResult& diff,
                                      int total_old_lines,
                                      int total_new_lines)
{
    left_decorations_.clear();
    right_decorations_.clear();
    change_start_lines_.clear();
    change_count_ = 0;

    // Initialize all lines as unchanged.
    left_decorations_.resize(static_cast<size_t>(total_old_lines));
    right_decorations_.resize(static_cast<size_t>(total_new_lines));

    for (int i = 0; i < total_old_lines; ++i)
    {
        left_decorations_[static_cast<size_t>(i)] = {.line_number = i,
                                                     .indicator = GutterIndicator::Unchanged};
    }
    for (int i = 0; i < total_new_lines; ++i)
    {
        right_decorations_[static_cast<size_t>(i)] = {.line_number = i,
                                                      .indicator = GutterIndicator::Unchanged};
    }

    // Apply diff hunks to mark changed lines.
    for (const auto& hunk : diff.hunks)
    {
        bool change_recorded = false;
        for (const auto& line : hunk.lines)
        {
            switch (line.type)
            {
                case core::DiffLineType::Deletion:
                    if (line.old_line_number >= 0 && line.old_line_number < total_old_lines)
                    {
                        left_decorations_[static_cast<size_t>(line.old_line_number)].indicator =
                            GutterIndicator::Deleted;
                        if (!change_recorded)
                        {
                            change_start_lines_.push_back(line.old_line_number);
                            change_recorded = true;
                        }
                    }
                    break;
                case core::DiffLineType::Addition:
                    if (line.new_line_number >= 0 && line.new_line_number < total_new_lines)
                    {
                        right_decorations_[static_cast<size_t>(line.new_line_number)].indicator =
                            GutterIndicator::Added;
                        if (!change_recorded)
                        {
                            change_start_lines_.push_back(line.new_line_number);
                            change_recorded = true;
                        }
                    }
                    break;
                case core::DiffLineType::Context:
                    break;
            }
        }
    }

    change_count_ = static_cast<int>(change_start_lines_.size());
}

void DiffGutterModel::highlight_change(int change_index)
{
    clear_highlights();
    if (change_index < 0 || change_index >= change_count_)
    {
        return;
    }

    // Find all lines in the change region and highlight them.
    int target_line = change_start_lines_[static_cast<size_t>(change_index)];
    for (auto& dec : left_decorations_)
    {
        if (dec.line_number == target_line && dec.indicator != GutterIndicator::Unchanged)
        {
            dec.is_highlighted = true;
        }
    }
    for (auto& dec : right_decorations_)
    {
        if (dec.line_number == target_line && dec.indicator != GutterIndicator::Unchanged)
        {
            dec.is_highlighted = true;
        }
    }
}

void DiffGutterModel::clear_highlights()
{
    for (auto& dec : left_decorations_)
    {
        dec.is_highlighted = false;
    }
    for (auto& dec : right_decorations_)
    {
        dec.is_highlighted = false;
    }
}

auto DiffGutterModel::change_line(int change_index) const -> int
{
    if (change_index < 0 || change_index >= change_count_)
    {
        return -1;
    }
    return change_start_lines_[static_cast<size_t>(change_index)];
}

} // namespace markamp::ui
