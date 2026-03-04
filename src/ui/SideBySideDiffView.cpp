// ============================================================================
// File: src/ui/SideBySideDiffView.cpp
// Phase 46: Diff Editor and Merge Tool — Side-by-side diff view model
// ============================================================================
#include "SideBySideDiffView.h"

namespace markamp::ui
{

void SideBySideDiffViewModel::build(const core::DiffResult& diff,
                                    const std::vector<std::string>& old_lines,
                                    const std::vector<std::string>& new_lines,
                                    bool enable_word_diff)
{
    left_lines_.clear();
    right_lines_.clear();
    change_line_indices_.clear();
    current_change_ = -1;

    core::WordDiffEngine word_engine;

    // Process hunks to build aligned lines.
    int old_pos = 0;
    int new_pos = 0;

    for (const auto& hunk : diff.hunks)
    {
        // Add unchanged lines before this hunk.
        int hunk_old_start = hunk.old_start;
        int hunk_new_start = hunk.new_start;

        while (old_pos < hunk_old_start && new_pos < hunk_new_start)
        {
            left_lines_.push_back({.content = old_lines[static_cast<size_t>(old_pos)],
                                   .original_line_number = old_pos});
            right_lines_.push_back({.content = new_lines[static_cast<size_t>(new_pos)],
                                    .original_line_number = new_pos});
            ++old_pos;
            ++new_pos;
        }

        // Record change start.
        change_line_indices_.push_back(static_cast<int>(left_lines_.size()));

        // Process hunk lines.
        std::vector<const core::DiffLine*> deletions;
        std::vector<const core::DiffLine*> additions;

        for (const auto& line : hunk.lines)
        {
            if (line.type == core::DiffLineType::Context)
            {
                // Flush pending deletions/additions.
                auto max_count = std::max(deletions.size(), additions.size());
                for (size_t i = 0; i < max_count; ++i)
                {
                    SideBySideLine left_entry;
                    SideBySideLine right_entry;

                    if (i < deletions.size())
                    {
                        left_entry.content = deletions[i]->content;
                        left_entry.original_line_number = deletions[i]->old_line_number;
                        left_entry.is_deleted = true;
                        old_pos = deletions[i]->old_line_number + 1;
                    }
                    else
                    {
                        left_entry.is_filler = true;
                    }

                    if (i < additions.size())
                    {
                        right_entry.content = additions[i]->content;
                        right_entry.original_line_number = additions[i]->new_line_number;
                        right_entry.is_added = true;
                        new_pos = additions[i]->new_line_number + 1;
                    }
                    else
                    {
                        right_entry.is_filler = true;
                    }

                    // Word-level diff for modified lines.
                    if (enable_word_diff && i < deletions.size() && i < additions.size())
                    {
                        auto word_diff =
                            word_engine.diff_lines(deletions[i]->content, additions[i]->content);
                        left_entry.is_modified = true;
                        left_entry.is_deleted = false;
                        left_entry.word_spans = std::move(word_diff.old_spans);
                        right_entry.is_modified = true;
                        right_entry.is_added = false;
                        right_entry.word_spans = std::move(word_diff.new_spans);
                    }

                    left_lines_.push_back(std::move(left_entry));
                    right_lines_.push_back(std::move(right_entry));
                }
                deletions.clear();
                additions.clear();

                // Add context line.
                left_lines_.push_back(
                    {.content = line.content, .original_line_number = line.old_line_number});
                right_lines_.push_back(
                    {.content = line.content, .original_line_number = line.new_line_number});
                if (line.old_line_number >= 0)
                {
                    old_pos = line.old_line_number + 1;
                }
                if (line.new_line_number >= 0)
                {
                    new_pos = line.new_line_number + 1;
                }
            }
            else if (line.type == core::DiffLineType::Deletion)
            {
                deletions.push_back(&line);
            }
            else if (line.type == core::DiffLineType::Addition)
            {
                additions.push_back(&line);
            }
        }

        // Flush remaining deletions/additions.
        auto max_count = std::max(deletions.size(), additions.size());
        for (size_t i = 0; i < max_count; ++i)
        {
            SideBySideLine left_entry;
            SideBySideLine right_entry;

            if (i < deletions.size())
            {
                left_entry.content = deletions[i]->content;
                left_entry.original_line_number = deletions[i]->old_line_number;
                left_entry.is_deleted = true;
                old_pos = deletions[i]->old_line_number + 1;
            }
            else
            {
                left_entry.is_filler = true;
            }

            if (i < additions.size())
            {
                right_entry.content = additions[i]->content;
                right_entry.original_line_number = additions[i]->new_line_number;
                right_entry.is_added = true;
                new_pos = additions[i]->new_line_number + 1;
            }
            else
            {
                right_entry.is_filler = true;
            }

            if (enable_word_diff && i < deletions.size() && i < additions.size())
            {
                auto word_diff =
                    word_engine.diff_lines(deletions[i]->content, additions[i]->content);
                left_entry.is_modified = true;
                left_entry.is_deleted = false;
                left_entry.word_spans = std::move(word_diff.old_spans);
                right_entry.is_modified = true;
                right_entry.is_added = false;
                right_entry.word_spans = std::move(word_diff.new_spans);
            }

            left_lines_.push_back(std::move(left_entry));
            right_lines_.push_back(std::move(right_entry));
        }
    }

    // Add remaining unchanged lines.
    while (old_pos < static_cast<int>(old_lines.size()) &&
           new_pos < static_cast<int>(new_lines.size()))
    {
        left_lines_.push_back(
            {.content = old_lines[static_cast<size_t>(old_pos)], .original_line_number = old_pos});
        right_lines_.push_back(
            {.content = new_lines[static_cast<size_t>(new_pos)], .original_line_number = new_pos});
        ++old_pos;
        ++new_pos;
    }
}

auto SideBySideDiffViewModel::navigate_next() -> int
{
    if (change_line_indices_.empty())
    {
        return -1;
    }
    current_change_ = (current_change_ + 1) % static_cast<int>(change_line_indices_.size());
    return change_line_indices_[static_cast<size_t>(current_change_)];
}

auto SideBySideDiffViewModel::navigate_prev() -> int
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
