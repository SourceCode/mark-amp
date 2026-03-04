// ============================================================================
// File: src/ui/InlineDiffView.cpp
// Phase 46: Diff Editor and Merge Tool — Inline diff view model
// ============================================================================
#include "InlineDiffView.h"

namespace markamp::ui
{

void InlineDiffViewModel::build(const core::DiffResult& diff,
                                const std::vector<std::string>& old_lines,
                                const std::vector<std::string>& new_lines)
{
    lines_.clear();
    change_line_indices_.clear();
    current_change_ = -1;

    int old_pos = 0;
    int new_pos = 0;

    for (const auto& hunk : diff.hunks)
    {
        // Add unchanged lines before this hunk.
        while (old_pos < hunk.old_start && new_pos < hunk.new_start)
        {
            lines_.push_back({.content = old_lines[static_cast<size_t>(old_pos)],
                              .type = core::DiffLineType::Context,
                              .old_line_number = old_pos,
                              .new_line_number = new_pos});
            ++old_pos;
            ++new_pos;
        }

        // Record change start.
        change_line_indices_.push_back(static_cast<int>(lines_.size()));

        // Add hunk lines.
        for (const auto& line : hunk.lines)
        {
            lines_.push_back({.content = line.content,
                              .type = line.type,
                              .old_line_number = line.old_line_number,
                              .new_line_number = line.new_line_number});

            if (line.type == core::DiffLineType::Deletion && line.old_line_number >= 0)
            {
                old_pos = std::max(old_pos, line.old_line_number + 1);
            }
            if (line.type == core::DiffLineType::Addition && line.new_line_number >= 0)
            {
                new_pos = std::max(new_pos, line.new_line_number + 1);
            }
            if (line.type == core::DiffLineType::Context)
            {
                if (line.old_line_number >= 0)
                {
                    old_pos = line.old_line_number + 1;
                }
                if (line.new_line_number >= 0)
                {
                    new_pos = line.new_line_number + 1;
                }
            }
        }
    }

    // Add remaining unchanged lines.
    while (old_pos < static_cast<int>(old_lines.size()) &&
           new_pos < static_cast<int>(new_lines.size()))
    {
        lines_.push_back({.content = old_lines[static_cast<size_t>(old_pos)],
                          .type = core::DiffLineType::Context,
                          .old_line_number = old_pos,
                          .new_line_number = new_pos});
        ++old_pos;
        ++new_pos;
    }
}

auto InlineDiffViewModel::navigate_next() -> int
{
    if (change_line_indices_.empty())
    {
        return -1;
    }
    current_change_ = (current_change_ + 1) % static_cast<int>(change_line_indices_.size());
    return change_line_indices_[static_cast<size_t>(current_change_)];
}

auto InlineDiffViewModel::navigate_prev() -> int
{
    if (change_line_indices_.empty())
    {
        return -1;
    }
    current_change_ = current_change_ <= 0 ? static_cast<int>(change_line_indices_.size()) - 1
                                           : current_change_ - 1;
    return change_line_indices_[static_cast<size_t>(current_change_)];
}

} // namespace markamp::ui
