#include "BookmarkService.h"

#include "Config.h"
#include "Events.h"

#include <algorithm>

namespace markamp::core
{

BookmarkService::BookmarkService(EventBus& event_bus, Config& config)
    : event_bus_(event_bus)
    , config_(config)
{
}

auto BookmarkService::get_bookmarks() -> std::vector<BookmarkGroup>
{
    std::lock_guard<std::mutex> lock(index_mutex_);

    auto labels = index_.get_all_labels();
    std::vector<BookmarkGroup> groups;
    groups.reserve(labels.size());

    for (const auto& label : labels)
    {
        BookmarkGroup group;
        group.label = label;

        auto block_ids = index_.get_blocks_with_label(label);
        for (const auto& block_id : block_ids)
        {
            BookmarkEntry entry;
            entry.block_id = block_id;
            entry.label = label;
            // In a full implementation, doc_title, content_snippet, block_type
            // would be populated from BlockDatabase lookup.
            group.entries.push_back(std::move(entry));
        }

        groups.push_back(std::move(group));
    }

    return groups;
}

auto BookmarkService::get_bookmark_blocks(const std::string& label) -> std::vector<BookmarkEntry>
{
    std::lock_guard<std::mutex> lock(index_mutex_);

    auto block_ids = index_.get_blocks_with_label(label);
    std::vector<BookmarkEntry> entries;
    entries.reserve(block_ids.size());

    for (const auto& block_id : block_ids)
    {
        BookmarkEntry entry;
        entry.block_id = block_id;
        entry.label = label;
        entries.push_back(std::move(entry));
    }

    return entries;
}

void BookmarkService::set_bookmark(const BlockId& block_id, const std::string& label)
{
    std::string effective_label = label;
    if (effective_label.empty())
    {
        effective_label = config_.get_string("knowledgebase.bookmarks.default_label", "Favorites");
    }

    {
        std::lock_guard<std::mutex> lock(index_mutex_);
        index_.add(block_id, effective_label);
    }

    events::BookmarkAddedEvent evt;
    evt.block_id = block_id.value;
    evt.label = effective_label;
    event_bus_.publish(evt);
}

void BookmarkService::remove_bookmark(const BlockId& block_id)
{
    std::string previous_label;
    {
        std::lock_guard<std::mutex> lock(index_mutex_);
        previous_label = index_.get_label(block_id);
        if (previous_label.empty())
        {
            return; // Not bookmarked, no-op.
        }
        index_.remove(block_id);
    }

    events::BookmarkRemovedEvent evt;
    evt.block_id = block_id.value;
    evt.previous_label = previous_label;
    event_bus_.publish(evt);
}

void BookmarkService::rename_bookmark(const std::string& old_label, const std::string& new_label)
{
    if (old_label == new_label)
    {
        return; // No-op.
    }

    int affected = 0;
    {
        std::lock_guard<std::mutex> lock(index_mutex_);
        affected = index_.rename_label(old_label, new_label);
    }

    if (affected > 0)
    {
        events::BookmarkRenamedEvent evt;
        evt.old_label = old_label;
        evt.new_label = new_label;
        evt.affected_count = affected;
        event_bus_.publish(evt);
    }
}

auto BookmarkService::search_bookmarks(const std::string& query) -> std::vector<BookmarkGroup>
{
    auto all_groups = get_bookmarks();
    if (query.empty())
    {
        return all_groups;
    }

    // Case-insensitive substring search
    std::string lower_query = query;
    std::transform(lower_query.begin(),
                   lower_query.end(),
                   lower_query.begin(),
                   [](unsigned char ch) { return static_cast<char>(std::tolower(ch)); });

    std::vector<BookmarkGroup> results;
    for (auto& group : all_groups)
    {
        std::string lower_label = group.label;
        std::transform(lower_label.begin(),
                       lower_label.end(),
                       lower_label.begin(),
                       [](unsigned char ch) { return static_cast<char>(std::tolower(ch)); });

        if (lower_label.find(lower_query) != std::string::npos)
        {
            results.push_back(std::move(group));
            continue;
        }

        // Check individual entries
        BookmarkGroup filtered;
        filtered.label = group.label;
        for (auto& entry : group.entries)
        {
            std::string lower_snippet = entry.content_snippet;
            std::transform(lower_snippet.begin(),
                           lower_snippet.end(),
                           lower_snippet.begin(),
                           [](unsigned char ch) { return static_cast<char>(std::tolower(ch)); });

            if (lower_snippet.find(lower_query) != std::string::npos)
            {
                filtered.entries.push_back(std::move(entry));
            }
        }
        if (!filtered.is_empty())
        {
            results.push_back(std::move(filtered));
        }
    }
    return results;
}

auto BookmarkService::get_bookmark_labels() -> std::vector<std::pair<std::string, int>>
{
    std::lock_guard<std::mutex> lock(index_mutex_);

    auto labels = index_.get_all_labels();
    std::vector<std::pair<std::string, int>> result;
    result.reserve(labels.size());

    for (const auto& label : labels)
    {
        auto blocks = index_.get_blocks_with_label(label);
        result.emplace_back(label, static_cast<int>(blocks.size()));
    }

    return result;
}

void BookmarkService::rebuild_index()
{
    std::lock_guard<std::mutex> lock(index_mutex_);
    index_.clear();
    // In a full implementation, this would scan all blocks from BlockDatabase
    // and extract bookmark IAL attributes. For now, starts empty.
}

} // namespace markamp::core
