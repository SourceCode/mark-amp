#include "JumpToController.h"

#include <algorithm>
#include <cctype>

namespace markamp::ui
{

auto JumpTarget::kind_label() const -> std::string
{
    switch (kind)
    {
        case JumpTargetKind::kFile:
            return "File";
        case JumpTargetKind::kHeading:
            return "Heading";
        case JumpTargetKind::kSymbol:
            return "Symbol";
        case JumpTargetKind::kCanvasNode:
            return "Canvas Node";
        case JumpTargetKind::kBreadcrumb:
            return "Breadcrumb";
        case JumpTargetKind::kRecentLocation:
            return "Recent";
    }
    return "Unknown";
}

auto JumpFilter::includes(JumpTargetKind target_kind) const -> bool
{
    switch (target_kind)
    {
        case JumpTargetKind::kFile:
            return include_files;
        case JumpTargetKind::kHeading:
            return include_headings;
        case JumpTargetKind::kSymbol:
            return include_symbols;
        case JumpTargetKind::kCanvasNode:
            return include_canvas_nodes;
        case JumpTargetKind::kRecentLocation:
            return include_recent;
        case JumpTargetKind::kBreadcrumb:
            return include_files; // Breadcrumbs follow file filter
    }
    return false;
}

auto JumpFilter::active_count() const -> int
{
    int count = 0;
    if (include_files)
    {
        ++count;
    }
    if (include_headings)
    {
        ++count;
    }
    if (include_symbols)
    {
        ++count;
    }
    if (include_canvas_nodes)
    {
        ++count;
    }
    if (include_recent)
    {
        ++count;
    }
    return count;
}

void JumpToController::add_target(const JumpTarget& target)
{
    targets_.push_back(target);
}

void JumpToController::clear_targets()
{
    targets_.clear();
}

auto JumpToController::search(const std::string& query, const JumpFilter& filter) const
    -> std::vector<JumpTarget>
{
    std::vector<JumpTarget> results;

    for (const auto& target : targets_)
    {
        if (!filter.includes(target.kind))
        {
            continue;
        }

        int match_score = score_match(target, query);
        if (match_score > 0 || query.empty())
        {
            JumpTarget result = target;
            result.score = match_score + target.score; // Combine relevance + MRU
            results.push_back(result);
        }
    }

    // Sort by score descending
    std::sort(results.begin(),
              results.end(),
              [](const JumpTarget& left, const JumpTarget& right)
              { return left.score > right.score; });

    return results;
}

auto JumpToController::recent_targets(int max_count) const -> std::vector<JumpTarget>
{
    std::vector<JumpTarget> recent;
    for (const auto& target : targets_)
    {
        if (target.is_recent)
        {
            recent.push_back(target);
        }
    }

    std::sort(recent.begin(),
              recent.end(),
              [](const JumpTarget& left, const JumpTarget& right)
              { return left.score > right.score; });

    if (static_cast<int>(recent.size()) > max_count)
    {
        recent.resize(static_cast<size_t>(max_count));
    }
    return recent;
}

auto JumpToController::target_count() const -> int
{
    return static_cast<int>(targets_.size());
}

void JumpToController::record_access(const std::string& target_id)
{
    for (auto& target : targets_)
    {
        if (target.target_id == target_id)
        {
            target.score += 10;
            target.is_recent = true;
        }
    }
}

auto JumpToController::available_kinds() -> std::vector<JumpTargetKind>
{
    return {JumpTargetKind::kFile,
            JumpTargetKind::kHeading,
            JumpTargetKind::kSymbol,
            JumpTargetKind::kCanvasNode,
            JumpTargetKind::kBreadcrumb,
            JumpTargetKind::kRecentLocation};
}

auto JumpToController::score_match(const JumpTarget& target, const std::string& query) -> int
{
    if (query.empty())
    {
        return 1;
    }

    // Case-insensitive substring match
    std::string lower_label = target.label;
    std::string lower_query = query;
    std::transform(lower_label.begin(),
                   lower_label.end(),
                   lower_label.begin(),
                   [](unsigned char character)
                   { return static_cast<char>(std::tolower(character)); });
    std::transform(lower_query.begin(),
                   lower_query.end(),
                   lower_query.begin(),
                   [](unsigned char character)
                   { return static_cast<char>(std::tolower(character)); });

    auto label_pos = lower_label.find(lower_query);
    if (label_pos == std::string::npos)
    {
        // Try description
        std::string lower_desc = target.description;
        std::transform(lower_desc.begin(),
                       lower_desc.end(),
                       lower_desc.begin(),
                       [](unsigned char character)
                       { return static_cast<char>(std::tolower(character)); });
        auto desc_pos = lower_desc.find(lower_query);
        if (desc_pos == std::string::npos)
        {
            return 0; // No match
        }
        return 5; // Description match
    }

    // Prefix matches score higher
    if (label_pos == 0)
    {
        return 20; // Prefix match
    }
    return 10; // Substring match
}

} // namespace markamp::ui
