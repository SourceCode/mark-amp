#include "OutlineService.h"

#include "Config.h"
#include "Events.h"
#include "MarkdownParser.h"
#include "Md4cWrapper.h"

#include <algorithm>

namespace markamp::core
{

OutlineService::OutlineService(EventBus& event_bus, Config& config)
    : event_bus_(event_bus)
    , config_(config)
{
}

auto OutlineService::get_outline(const std::string& root_id) -> DocumentOutline
{
    std::lock_guard<std::mutex> lock(cache_mutex_);
    auto iter = outline_cache_.find(root_id);
    if (iter != outline_cache_.end())
    {
        return iter->second;
    }
    // If not cached, return an empty outline. Call on_content_changed() to populate.
    DocumentOutline empty;
    empty.root_id = root_id;
    return empty;
}

auto OutlineService::get_flat_outline(const std::string& root_id) -> std::vector<OutlineEntry>
{
    auto outline = get_outline(root_id);
    std::vector<OutlineEntry> entries;
    int index = 0;
    flatten_nodes(outline.roots, entries, index);
    return entries;
}

void OutlineService::scroll_to_heading(const std::string& block_id)
{
    events::OutlineScrollToEvent evt;
    evt.block_id = block_id;
    event_bus_.publish(evt);
}

auto OutlineService::get_heading_at_line(const std::string& root_id, int line)
    -> std::optional<OutlineEntry>
{
    auto entries = get_flat_outline(root_id);
    std::optional<OutlineEntry> best;
    for (const auto& entry : entries)
    {
        if (entry.source_line <= line)
        {
            best = entry;
        }
        else
        {
            break;
        }
    }
    return best;
}

auto OutlineService::get_breadcrumb_path(const std::string& root_id, int line)
    -> std::vector<OutlineEntry>
{
    auto outline = get_outline(root_id);
    std::vector<OutlineEntry> path;
    find_ancestry(outline.roots, line, path);
    return path;
}

void OutlineService::invalidate(const std::string& root_id)
{
    std::lock_guard<std::mutex> lock(cache_mutex_);
    outline_cache_.erase(root_id);
}

auto OutlineService::search_headings(const std::string& root_id, const std::string& query)
    -> std::vector<OutlineEntry>
{
    auto entries = get_flat_outline(root_id);
    std::vector<OutlineEntry> results;

    // Case-insensitive substring search
    std::string lower_query = query;
    std::transform(lower_query.begin(),
                   lower_query.end(),
                   lower_query.begin(),
                   [](unsigned char ch) { return static_cast<char>(std::tolower(ch)); });

    for (const auto& entry : entries)
    {
        std::string lower_text = entry.text;
        std::transform(lower_text.begin(),
                       lower_text.end(),
                       lower_text.begin(),
                       [](unsigned char ch) { return static_cast<char>(std::tolower(ch)); });

        if (lower_text.find(lower_query) != std::string::npos)
        {
            results.push_back(entry);
        }
    }
    return results;
}

void OutlineService::on_content_changed(const std::string& content, const std::string& root_id)
{
    // Parse the new content into a MarkdownDocument.
    MarkdownParser parser;
    auto parse_result = parser.parse(content);
    if (!parse_result)
    {
        return; // Failed to parse, skip outline update.
    }
    const auto& doc = *parse_result;

    // Build the outline.
    auto outline = builder_.build_from_ast(doc);
    outline.root_id = root_id;

    int previous_count = 0;
    {
        std::lock_guard<std::mutex> lock(cache_mutex_);
        auto iter = outline_cache_.find(root_id);
        if (iter != outline_cache_.end())
        {
            previous_count = iter->second.total_heading_count;
        }
        outline_cache_[root_id] = outline;
    }

    // Only publish if the heading structure changed.
    if (outline.total_heading_count != previous_count)
    {
        events::OutlineChangedEvent evt;
        evt.root_id = root_id;
        evt.heading_count = outline.total_heading_count;
        event_bus_.publish(evt);
    }
}

void OutlineService::on_cursor_moved(int line, const std::string& root_id)
{
    auto heading = get_heading_at_line(root_id, line);
    if (!heading.has_value())
    {
        return;
    }

    int current_line = heading->source_line;
    auto iter = active_heading_line_.find(root_id);
    if (iter != active_heading_line_.end() && iter->second == current_line)
    {
        return; // Same heading, no change.
    }

    active_heading_line_[root_id] = current_line;

    events::OutlineActiveHeadingChangedEvent evt;
    evt.root_id = root_id;
    evt.heading_text = heading->text;
    evt.heading_level = heading->level;
    evt.source_line = heading->source_line;
    event_bus_.publish(evt);
}

auto OutlineService::get_statistics(const std::string& root_id) -> DocumentOutline
{
    return get_outline(root_id);
}

void OutlineService::flatten_nodes(const std::vector<OutlineNode>& nodes,
                                   std::vector<OutlineEntry>& entries,
                                   int& index) const
{
    for (const auto& node : nodes)
    {
        OutlineEntry entry;
        entry.block_id = node.block_id;
        entry.text = node.text;
        entry.level = node.level;
        entry.depth = node.depth;
        entry.source_line = node.source_line;
        entry.source_offset = node.source_offset;
        entry.index = index++;

        // Build indent string: 2 spaces per depth level
        for (int depth_idx = 0; depth_idx < node.depth; ++depth_idx)
        {
            entry.indent += "  ";
        }

        entries.push_back(std::move(entry));

        // Recurse into children
        flatten_nodes(node.children, entries, index);
    }
}

void OutlineService::find_ancestry(const std::vector<OutlineNode>& nodes,
                                   int target_line,
                                   std::vector<OutlineEntry>& path) const
{
    for (auto iter = nodes.rbegin(); iter != nodes.rend(); ++iter)
    {
        if (iter->source_line <= target_line)
        {
            OutlineEntry entry;
            entry.text = iter->text;
            entry.level = iter->level;
            entry.depth = iter->depth;
            entry.source_line = iter->source_line;
            path.insert(path.begin(), entry);

            // Check children for deeper match
            find_ancestry(iter->children, target_line, path);
            return;
        }
    }
}

} // namespace markamp::core
