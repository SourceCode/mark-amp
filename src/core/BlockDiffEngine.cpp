// ============================================================================
// File: src/core/BlockDiffEngine.cpp
// Phase 33: Version Diff & Comparison — BlockDiffEngine implementation
// ============================================================================
#include "BlockDiffEngine.h"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <set>
#include <sstream>
#include <unordered_set>

namespace markamp::core
{

auto BlockDiffEngine::diff_documents(
    const std::vector<std::pair<std::string, std::string>>& old_blocks,
    const std::vector<std::pair<std::string, std::string>>& new_blocks,
    const DiffOptions& options) const -> BlockDiffResult
{
    const auto start = std::chrono::steady_clock::now();

    BlockDiffResult result;

    // Match blocks by similarity
    auto matches = match_blocks(old_blocks, new_blocks);

    // Build sets of matched indices
    std::set<int> matched_old;
    std::set<int> matched_new;
    for (const auto& [old_idx, new_idx] : matches)
    {
        matched_old.insert(old_idx);
        matched_new.insert(new_idx);
    }

    // Process matched blocks (unchanged or modified)
    for (const auto& [old_idx, new_idx] : matches)
    {
        BlockDiff diff;
        diff.block_id = "block-" + std::to_string(new_idx);
        diff.old_content = old_blocks[old_idx].second;
        diff.new_content = new_blocks[new_idx].second;
        diff.old_block_type = old_blocks[old_idx].first;
        diff.new_block_type = new_blocks[new_idx].first;
        diff.old_position = old_idx;
        diff.new_position = new_idx;
        diff.similarity = compute_similarity(diff.old_content, diff.new_content);

        if (options.ignore_whitespace)
        {
            // Simplified: compare without whitespace
            auto strip_ws = [](const std::string& str)
            {
                std::string result;
                for (char chr : str)
                {
                    if (!std::isspace(static_cast<unsigned char>(chr)))
                    {
                        result += chr;
                    }
                }
                return result;
            };
            diff.similarity =
                compute_similarity(strip_ws(diff.old_content), strip_ws(diff.new_content));
        }

        if (diff.similarity >= 0.999)
        {
            diff.diff_type = (old_idx == new_idx) ? DiffType::Unchanged : DiffType::Moved;
        }
        else
        {
            diff.diff_type = DiffType::Modified;
        }

        result.diffs.push_back(std::move(diff));
    }

    // Process deleted blocks (in old but not matched)
    for (int idx = 0; idx < static_cast<int>(old_blocks.size()); ++idx)
    {
        if (!matched_old.contains(idx))
        {
            BlockDiff diff;
            diff.block_id = "block-old-" + std::to_string(idx);
            diff.diff_type = DiffType::Deleted;
            diff.old_content = old_blocks[idx].second;
            diff.old_block_type = old_blocks[idx].first;
            diff.old_position = idx;
            diff.similarity = 0.0;
            result.diffs.push_back(std::move(diff));
        }
    }

    // Process added blocks (in new but not matched)
    for (int idx = 0; idx < static_cast<int>(new_blocks.size()); ++idx)
    {
        if (!matched_new.contains(idx))
        {
            BlockDiff diff;
            diff.block_id = "block-new-" + std::to_string(idx);
            diff.diff_type = DiffType::Added;
            diff.new_content = new_blocks[idx].second;
            diff.new_block_type = new_blocks[idx].first;
            diff.new_position = idx;
            diff.similarity = 0.0;
            result.diffs.push_back(std::move(diff));
        }
    }

    // Sort by position
    std::sort(result.diffs.begin(),
              result.diffs.end(),
              [](const BlockDiff& left, const BlockDiff& right)
              {
                  int left_pos = std::max(left.old_position, left.new_position);
                  int right_pos = std::max(right.old_position, right.new_position);
                  return left_pos < right_pos;
              });

    // Compute stats
    result.stats.total_blocks = static_cast<int>(result.diffs.size());
    for (const auto& diff : result.diffs)
    {
        switch (diff.diff_type)
        {
            case DiffType::Unchanged:
                ++result.stats.unchanged_blocks;
                break;
            case DiffType::Added:
                ++result.stats.added_blocks;
                break;
            case DiffType::Deleted:
                ++result.stats.deleted_blocks;
                break;
            case DiffType::Modified:
                ++result.stats.modified_blocks;
                break;
            case DiffType::Moved:
                ++result.stats.moved_blocks;
                break;
        }
    }

    const auto end = std::chrono::steady_clock::now();
    result.compute_time_ms =
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    return result;
}

auto BlockDiffEngine::diff_text_as_blocks(const std::string& old_text,
                                          const std::string& new_text,
                                          const DiffOptions& options) const -> BlockDiffResult
{
    auto old_blocks = split_into_blocks(old_text);
    auto new_blocks = split_into_blocks(new_text);
    return diff_documents(old_blocks, new_blocks, options);
}

auto BlockDiffEngine::compute_similarity(const std::string& left, const std::string& right)
    -> double
{
    if (left == right)
    {
        return 1.0;
    }
    if (left.empty() || right.empty())
    {
        return 0.0;
    }

    // Bigram similarity (Dice coefficient)
    auto make_bigrams = [](const std::string& str)
    {
        std::unordered_set<std::string> bigrams;
        for (std::size_t idx = 0; idx + 1 < str.size(); ++idx)
        {
            bigrams.insert(str.substr(idx, 2));
        }
        return bigrams;
    };

    auto bigrams_left = make_bigrams(left);
    auto bigrams_right = make_bigrams(right);

    int intersection_size = 0;
    for (const auto& bigram : bigrams_left)
    {
        if (bigrams_right.contains(bigram))
        {
            ++intersection_size;
        }
    }

    return (2.0 * static_cast<double>(intersection_size)) /
           static_cast<double>(bigrams_left.size() + bigrams_right.size());
}

auto BlockDiffEngine::split_into_blocks(const std::string& text)
    -> std::vector<std::pair<std::string, std::string>>
{
    std::vector<std::pair<std::string, std::string>> blocks;
    std::istringstream stream(text);
    std::string line;
    std::ostringstream current_block;
    std::string current_type = "paragraph";

    while (std::getline(stream, line))
    {
        // Heading breaks into new block
        if (!line.empty() && line[0] == '#')
        {
            // Save current block
            auto content = current_block.str();
            if (!content.empty())
            {
                blocks.emplace_back(current_type, content);
                current_block.str("");
                current_block.clear();
            }
            current_type = "heading";
        }
        else if (line.empty())
        {
            // Blank line: save current block
            auto content = current_block.str();
            if (!content.empty())
            {
                blocks.emplace_back(current_type, content);
                current_block.str("");
                current_block.clear();
                current_type = "paragraph";
            }
            continue;
        }
        else if (line.starts_with("```"))
        {
            current_type = "code";
        }
        else if (line.starts_with("> "))
        {
            current_type = "blockquote";
        }
        else if (line.starts_with("- ") || line.starts_with("* "))
        {
            current_type = "list";
        }

        current_block << line << "\n";
    }

    // Final block
    auto content = current_block.str();
    if (!content.empty())
    {
        blocks.emplace_back(current_type, content);
    }

    return blocks;
}

auto BlockDiffEngine::match_blocks(
    const std::vector<std::pair<std::string, std::string>>& old_blocks,
    const std::vector<std::pair<std::string, std::string>>& new_blocks,
    double threshold) const -> std::vector<std::pair<int, int>>
{
    std::vector<std::pair<int, int>> matches;
    std::set<int> used_new;

    for (int old_idx = 0; old_idx < static_cast<int>(old_blocks.size()); ++old_idx)
    {
        double best_sim = 0.0;
        int best_new = -1;

        for (int new_idx = 0; new_idx < static_cast<int>(new_blocks.size()); ++new_idx)
        {
            if (used_new.contains(new_idx))
            {
                continue;
            }

            double sim = compute_similarity(old_blocks[old_idx].second, new_blocks[new_idx].second);
            if (sim > best_sim)
            {
                best_sim = sim;
                best_new = new_idx;
            }
        }

        if (best_new >= 0 && best_sim >= threshold)
        {
            matches.emplace_back(old_idx, best_new);
            used_new.insert(best_new);
        }
    }

    return matches;
}

} // namespace markamp::core
