// ============================================================================
// File: src/core/MergeEngine.cpp
// Phase 46: Diff Editor and Merge Tool — 3-way merge engine
// ============================================================================
#include "MergeEngine.h"

#include <algorithm>
#include <sstream>

namespace markamp::core
{

auto MergeResult::merged_text() const -> std::string
{
    std::string output;
    for (size_t i = 0; i < lines.size(); ++i)
    {
        output += lines[i].content;
        if (i + 1 < lines.size())
        {
            output += '\n';
        }
    }
    return output;
}

auto MergeEngine::split_lines(std::string_view text) -> std::vector<std::string>
{
    std::vector<std::string> result;
    std::string line;
    for (char ch : text)
    {
        if (ch == '\n')
        {
            result.push_back(std::move(line));
            line.clear();
        }
        else
        {
            line += ch;
        }
    }
    if (!line.empty())
    {
        result.push_back(std::move(line));
    }
    return result;
}

auto MergeEngine::lcs_lines(const std::vector<std::string>& a, const std::vector<std::string>& b)
    -> std::vector<std::pair<int, int>>
{
    const auto m = static_cast<int>(a.size());
    const auto n = static_cast<int>(b.size());

    std::vector<std::vector<int>> dp(static_cast<size_t>(m + 1),
                                     std::vector<int>(static_cast<size_t>(n + 1), 0));

    for (int i = 1; i <= m; ++i)
    {
        for (int j = 1; j <= n; ++j)
        {
            if (a[static_cast<size_t>(i - 1)] == b[static_cast<size_t>(j - 1)])
            {
                dp[static_cast<size_t>(i)][static_cast<size_t>(j)] =
                    dp[static_cast<size_t>(i - 1)][static_cast<size_t>(j - 1)] + 1;
            }
            else
            {
                dp[static_cast<size_t>(i)][static_cast<size_t>(j)] =
                    std::max(dp[static_cast<size_t>(i - 1)][static_cast<size_t>(j)],
                             dp[static_cast<size_t>(i)][static_cast<size_t>(j - 1)]);
            }
        }
    }

    std::vector<std::pair<int, int>> matches;
    int i = m;
    int j = n;
    while (i > 0 && j > 0)
    {
        if (a[static_cast<size_t>(i - 1)] == b[static_cast<size_t>(j - 1)])
        {
            matches.emplace_back(i - 1, j - 1);
            --i;
            --j;
        }
        else if (dp[static_cast<size_t>(i - 1)][static_cast<size_t>(j)] >=
                 dp[static_cast<size_t>(i)][static_cast<size_t>(j - 1)])
        {
            --i;
        }
        else
        {
            --j;
        }
    }

    std::ranges::reverse(matches);
    return matches;
}

auto MergeEngine::extract_changes(const std::vector<std::string>& base,
                                  const std::vector<std::string>& other,
                                  const std::vector<std::pair<int, int>>& matches,
                                  ChangeRegion::Source source) -> std::vector<ChangeRegion>
{
    std::vector<ChangeRegion> changes;
    int prev_base = 0;
    int prev_other = 0;

    for (const auto& [bi, oi] : matches)
    {
        if (bi > prev_base || oi > prev_other)
        {
            changes.push_back({.base_start = prev_base,
                               .base_end = bi,
                               .other_start = prev_other,
                               .other_end = oi,
                               .source = source});
        }
        prev_base = bi + 1;
        prev_other = oi + 1;
    }

    // Trailing change after last match.
    if (prev_base < static_cast<int>(base.size()) || prev_other < static_cast<int>(other.size()))
    {
        changes.push_back({.base_start = prev_base,
                           .base_end = static_cast<int>(base.size()),
                           .other_start = prev_other,
                           .other_end = static_cast<int>(other.size()),
                           .source = source});
    }

    return changes;
}

auto MergeEngine::merge(std::string_view base_text,
                        std::string_view theirs_text,
                        std::string_view yours_text) const -> MergeResult
{
    MergeResult result;

    auto base = split_lines(base_text);
    auto theirs = split_lines(theirs_text);
    auto yours = split_lines(yours_text);

    auto theirs_matches = lcs_lines(base, theirs);
    auto yours_matches = lcs_lines(base, yours);

    auto theirs_changes =
        extract_changes(base, theirs, theirs_matches, ChangeRegion::Source::Theirs);
    auto yours_changes = extract_changes(base, yours, yours_matches, ChangeRegion::Source::Yours);

    // Walk through base, applying changes.
    int base_pos = 0;
    size_t ti = 0; // theirs change index
    size_t yi = 0; // yours change index

    while (base_pos < static_cast<int>(base.size()) || ti < theirs_changes.size() ||
           yi < yours_changes.size())
    {
        // Find the next change region (earliest base_start).
        bool have_theirs = ti < theirs_changes.size();
        bool have_yours = yi < yours_changes.size();

        if (!have_theirs && !have_yours)
        {
            // No more changes, copy remaining base.
            while (base_pos < static_cast<int>(base.size()))
            {
                result.lines.push_back({.content = base[static_cast<size_t>(base_pos)],
                                        .origin = MergeOrigin::Base,
                                        .base_line = base_pos});
                ++base_pos;
            }
            break;
        }

        int next_theirs_start =
            have_theirs ? theirs_changes[ti].base_start : static_cast<int>(base.size());
        int next_yours_start =
            have_yours ? yours_changes[yi].base_start : static_cast<int>(base.size());

        // Copy unchanged base lines up to the next change.
        int copy_to = std::min(next_theirs_start, next_yours_start);
        while (base_pos < copy_to)
        {
            result.lines.push_back({.content = base[static_cast<size_t>(base_pos)],
                                    .origin = MergeOrigin::Base,
                                    .base_line = base_pos});
            ++base_pos;
        }

        // Check for overlapping changes (conflict).
        if (have_theirs && have_yours)
        {
            const auto& tc = theirs_changes[ti];
            const auto& yc = yours_changes[yi];

            bool overlaps = (tc.base_start < yc.base_end && yc.base_start < tc.base_end);

            if (overlaps)
            {
                // Conflict: both sides changed overlapping base region.
                MergeConflict conflict;
                conflict.start_line = static_cast<int>(result.lines.size());

                int conflict_base_start = std::min(tc.base_start, yc.base_start);
                int conflict_base_end = std::max(tc.base_end, yc.base_end);

                for (int b = conflict_base_start; b < conflict_base_end; ++b)
                {
                    conflict.base_lines.push_back(base[static_cast<size_t>(b)]);
                }
                for (int t = tc.other_start; t < tc.other_end; ++t)
                {
                    conflict.theirs_lines.push_back(theirs[static_cast<size_t>(t)]);
                }
                for (int y = yc.other_start; y < yc.other_end; ++y)
                {
                    conflict.yours_lines.push_back(yours[static_cast<size_t>(y)]);
                }

                // Insert conflict markers as placeholder lines.
                result.lines.push_back(
                    {.content = "<<<<<<< YOURS", .origin = MergeOrigin::Conflict});
                for (const auto& line : conflict.yours_lines)
                {
                    result.lines.push_back({.content = line, .origin = MergeOrigin::Yours});
                }
                result.lines.push_back({.content = "=======", .origin = MergeOrigin::Conflict});
                for (const auto& line : conflict.theirs_lines)
                {
                    result.lines.push_back({.content = line, .origin = MergeOrigin::Theirs});
                }
                result.lines.push_back(
                    {.content = ">>>>>>> THEIRS", .origin = MergeOrigin::Conflict});

                conflict.end_line = static_cast<int>(result.lines.size());
                result.conflicts.push_back(std::move(conflict));
                result.has_conflicts = true;

                base_pos = conflict_base_end;
                ++ti;
                ++yi;
                continue;
            }
        }

        // Non-overlapping: apply the earlier change.
        if (next_theirs_start <= next_yours_start && have_theirs)
        {
            const auto& tc = theirs_changes[ti];
            // Skip base lines covered by theirs change.
            base_pos = tc.base_end;
            // Insert theirs lines.
            for (int t = tc.other_start; t < tc.other_end; ++t)
            {
                result.lines.push_back({.content = theirs[static_cast<size_t>(t)],
                                        .origin = MergeOrigin::Theirs,
                                        .theirs_line = t});
            }
            ++result.auto_merged_regions;
            ++ti;
        }
        else if (have_yours)
        {
            const auto& yc = yours_changes[yi];
            base_pos = yc.base_end;
            for (int y = yc.other_start; y < yc.other_end; ++y)
            {
                result.lines.push_back({.content = yours[static_cast<size_t>(y)],
                                        .origin = MergeOrigin::Yours,
                                        .yours_line = y});
            }
            ++result.auto_merged_regions;
            ++yi;
        }
    }

    return result;
}

void MergeEngine::resolve_conflict(MergeResult& result,
                                   size_t conflict_index,
                                   ConflictResolution resolution)
{
    if (conflict_index >= result.conflicts.size())
    {
        return;
    }
    result.conflicts[conflict_index].resolution = resolution;
}

void MergeEngine::resolve_conflict_manual(MergeResult& result,
                                          size_t conflict_index,
                                          std::vector<std::string> resolved_lines)
{
    if (conflict_index >= result.conflicts.size())
    {
        return;
    }
    result.conflicts[conflict_index].resolution = ConflictResolution::Manual;
    result.conflicts[conflict_index].resolved_lines = std::move(resolved_lines);
}

} // namespace markamp::core
