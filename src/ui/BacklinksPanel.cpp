/// @file BacklinksPanel.cpp
/// @brief V4 Phase 06 – Backlinks Panel and Local Graph Panel controller implementation.

#include "ui/BacklinksPanel.h"

#include "core/Events.h"
#include "core/VaultService.h"

#include <algorithm>
#include <cmath>
#include <numbers>

namespace markamp::core
{

// ============================================================================
// Helpers
// ============================================================================

namespace
{

/// Look up a VaultIndexEntry by document_id from the vault service.
[[nodiscard]] auto find_entry_by_id(const VaultService& vault_service,
                                    const std::string& document_id)
    -> std::optional<VaultIndexEntry>
{
    auto all_docs = vault_service.list_documents();
    for (auto& entry : all_docs)
    {
        if (entry.document_id == document_id)
        {
            return entry;
        }
    }
    return std::nullopt;
}

} // anonymous namespace

// ============================================================================
// BacklinksPanelController
// ============================================================================

BacklinksPanelController::BacklinksPanelController(EventBus& event_bus,
                                                   VaultService& vault_service,
                                                   BacklinkIndex& backlink_index)
    : event_bus_(event_bus)
    , vault_service_(vault_service)
    , backlink_index_(backlink_index)
{
    doc_switched_sub_ = event_bus_.subscribe<events::FileOpenRequestEvent>(
        [this](const events::FileOpenRequestEvent& evt) { set_active_document(evt.file_path); });
}

auto BacklinksPanelController::compute_backlinks(const std::string& document_id) const
    -> BacklinksPanelData
{
    BacklinksPanelData data;
    data.document_id = document_id;

    // Get document title from vault index
    auto entry = find_entry_by_id(vault_service_, document_id);
    if (entry)
    {
        data.document_title = entry->title;
    }

    // Get full backlink result (linked + unlinked)
    auto result = backlink_index_.get_backlink_result(document_id);

    // Build linked groups
    data.linked_groups = build_mention_items(result.linked_mentions, true);
    data.total_linked = result.linked_count();

    // Build unlinked groups from MentionEntry (convert to BacklinkEntry-like structure)
    std::vector<BacklinkEntry> unlinked_as_entries;
    for (const auto& mention : result.unlinked_mentions)
    {
        BacklinkEntry entry_item;
        entry_item.source_document_id = mention.source_document_id;
        entry_item.source_document_title = mention.source_document_title;
        entry_item.target_document_id = mention.mentioned_document_id;
        entry_item.context = mention.context;
        entry_item.line_number = mention.line_number;
        unlinked_as_entries.push_back(std::move(entry_item));
    }
    data.unlinked_groups = build_mention_items(unlinked_as_entries, false);
    data.total_unlinked = result.unlinked_count();

    return data;
}

auto BacklinksPanelController::apply_filter(const BacklinksPanelData& data,
                                            const BacklinkFilterState& filter) const
    -> BacklinksPanelData
{
    BacklinksPanelData filtered = data;

    // Filter by show_linked / show_unlinked
    if (!filter.show_linked)
    {
        filtered.linked_groups.clear();
        filtered.total_linked = 0;
    }
    if (!filter.show_unlinked)
    {
        filtered.unlinked_groups.clear();
        filtered.total_unlinked = 0;
    }

    // Apply text filter
    if (!filter.filter_text.empty())
    {
        const auto matches_filter = [&](const BacklinkSourceGroup& group) -> bool
        {
            // Check document title
            if (group.document_title.find(filter.filter_text) != std::string::npos)
            {
                return true;
            }
            // Check mention contexts
            return std::ranges::any_of(
                group.mentions,
                [&](const BacklinkMentionItem& mention)
                { return mention.context_snippet.find(filter.filter_text) != std::string::npos; });
        };

        std::erase_if(filtered.linked_groups,
                      [&](const BacklinkSourceGroup& group) { return !matches_filter(group); });
        std::erase_if(filtered.unlinked_groups,
                      [&](const BacklinkSourceGroup& group) { return !matches_filter(group); });

        // Recount
        filtered.total_linked = 0;
        for (const auto& group : filtered.linked_groups)
        {
            filtered.total_linked += static_cast<int>(group.mentions.size());
        }
        filtered.total_unlinked = 0;
        for (const auto& group : filtered.unlinked_groups)
        {
            filtered.total_unlinked += static_cast<int>(group.mentions.size());
        }
    }

    // Sort groups
    auto sort_groups = [&](std::vector<BacklinkSourceGroup>& groups)
    {
        switch (filter.sort_order)
        {
            case BacklinkFilterState::SortOrder::kByDocument:
                std::sort(groups.begin(),
                          groups.end(),
                          [](const BacklinkSourceGroup& lhs, const BacklinkSourceGroup& rhs)
                          { return lhs.document_title < rhs.document_title; });
                break;
            case BacklinkFilterState::SortOrder::kByLineNumber:
                std::sort(groups.begin(),
                          groups.end(),
                          [](const BacklinkSourceGroup& lhs, const BacklinkSourceGroup& rhs)
                          {
                              const int first_lhs =
                                  lhs.mentions.empty() ? 0 : lhs.mentions[0].line_number;
                              const int first_rhs =
                                  rhs.mentions.empty() ? 0 : rhs.mentions[0].line_number;
                              return first_lhs < first_rhs;
                          });
                break;
            case BacklinkFilterState::SortOrder::kByRecent:
                // Recent sort would use file modified time; for now reverse title order
                std::sort(groups.begin(),
                          groups.end(),
                          [](const BacklinkSourceGroup& lhs, const BacklinkSourceGroup& rhs)
                          { return lhs.document_title > rhs.document_title; });
                break;
        }
    };

    sort_groups(filtered.linked_groups);
    sort_groups(filtered.unlinked_groups);

    return filtered;
}

auto BacklinksPanelController::convert_to_link(const std::string& source_document_id,
                                               int line_number,
                                               const std::string& mention_text) -> bool
{
    // Look up the source document to get its file path
    auto source_entry = find_entry_by_id(vault_service_, source_document_id);
    if (!source_entry)
    {
        return false;
    }

    // Open the document to get its content
    auto doc_result = vault_service_.open_document(source_entry->file_path);
    if (!doc_result)
    {
        return false;
    }

    auto doc = doc_result.value();
    auto content = doc->markdown();
    const std::string replacement = "[[" + mention_text + "]]";

    // Find the mention on the specified line
    int current_line = 1;
    size_t line_start = 0;

    for (size_t pos = 0; pos < content.size() && current_line < line_number; ++pos)
    {
        if (content[pos] == '\n')
        {
            ++current_line;
            line_start = pos + 1;
        }
    }

    // Find the end of the target line
    size_t line_end = content.find('\n', line_start);
    if (line_end == std::string::npos)
    {
        line_end = content.size();
    }

    // Search for the mention text within this line
    const size_t mention_pos = content.find(mention_text, line_start);
    if (mention_pos == std::string::npos || mention_pos >= line_end)
    {
        return false;
    }

    // Replace the first occurrence on this line
    content.replace(mention_pos, mention_text.size(), replacement);

    // Save the modified content via DocumentModel
    doc->set_markdown(content);
    auto save_result = vault_service_.save_document(source_document_id);
    return save_result.has_value();
}

auto BacklinksPanelController::compute_local_graph(const std::string& document_id, int depth) const
    -> LocalGraphData
{
    LocalGraphData graph;
    graph.center_document_id = document_id;

    // Get connected documents using BacklinkIndex
    auto connected = backlink_index_.get_connected_documents(document_id);

    // If depth > 1, expand the neighborhood
    if (depth > 1)
    {
        std::set<std::string> all_connected = connected;
        for (const auto& neighbor_id : connected)
        {
            auto second_degree = backlink_index_.get_connected_documents(neighbor_id);
            all_connected.insert(second_degree.begin(), second_degree.end());
        }
        connected = all_connected;
    }

    // Remove the center document from neighbors (it's added separately)
    connected.erase(document_id);

    // Add center node
    {
        LocalGraphNode center;
        center.document_id = document_id;
        auto entry = find_entry_by_id(vault_service_, document_id);
        center.label = entry ? entry->title : document_id;
        center.is_center = true;
        center.radius = 10.0;
        center.link_count = backlink_index_.backlink_count(document_id);
        graph.nodes.push_back(std::move(center));
    }

    // Add neighbor nodes in circular layout
    const int neighbor_count = static_cast<int>(connected.size());
    int idx = 0;
    for (const auto& neighbor_id : connected)
    {
        LocalGraphNode node;
        node.document_id = neighbor_id;
        auto entry = find_entry_by_id(vault_service_, neighbor_id);
        node.label = entry ? entry->title : neighbor_id;
        node.is_center = false;
        node.link_count = backlink_index_.backlink_count(neighbor_id);
        node.radius = 6.0;

        // Circular layout
        if (neighbor_count > 0)
        {
            node.angle = (2.0 * std::numbers::pi * static_cast<double>(idx)) /
                         static_cast<double>(neighbor_count);
        }

        graph.nodes.push_back(std::move(node));
        ++idx;
    }

    // Build edges (center to each neighbor)
    for (int edge_idx = 1; edge_idx < static_cast<int>(graph.nodes.size()); ++edge_idx)
    {
        LocalGraphEdge edge;
        edge.source_index = 0; // Center
        edge.target_index = edge_idx;

        // Check if bidirectional
        const auto& neighbor_id = graph.nodes[static_cast<size_t>(edge_idx)].document_id;
        auto neighbor_connected = backlink_index_.get_connected_documents(neighbor_id);
        edge.is_bidirectional = neighbor_connected.contains(document_id);

        graph.edges.push_back(edge);
    }

    // Also add edges between neighbors if they are connected
    for (int outer = 1; outer < static_cast<int>(graph.nodes.size()); ++outer)
    {
        for (int inner = outer + 1; inner < static_cast<int>(graph.nodes.size()); ++inner)
        {
            const auto& outer_id = graph.nodes[static_cast<size_t>(outer)].document_id;
            const auto& inner_id = graph.nodes[static_cast<size_t>(inner)].document_id;
            auto outer_connected = backlink_index_.get_connected_documents(outer_id);
            if (outer_connected.contains(inner_id))
            {
                LocalGraphEdge edge;
                edge.source_index = outer;
                edge.target_index = inner;
                auto inner_connected = backlink_index_.get_connected_documents(inner_id);
                edge.is_bidirectional = inner_connected.contains(outer_id);
                graph.edges.push_back(edge);
            }
        }
    }

    return graph;
}

auto BacklinksPanelController::active_document_id() const -> const std::string&
{
    return active_document_id_;
}

auto BacklinksPanelController::set_active_document(const std::string& document_id) -> void
{
    active_document_id_ = document_id;

    auto data = compute_backlinks(document_id);
    auto event = events::BacklinksPanelRefreshedEvent{};
    event.document_id = document_id;
    event.linked_count = data.total_linked;
    event.unlinked_count = data.total_unlinked;
    event_bus_.publish(event);
}

auto BacklinksPanelController::build_mention_items(const std::vector<BacklinkEntry>& entries,
                                                   bool is_linked) const
    -> std::vector<BacklinkSourceGroup>
{
    // Group entries by source document
    std::unordered_map<std::string, std::vector<const BacklinkEntry*>> grouped;
    for (const auto& entry : entries)
    {
        grouped[entry.source_document_id].push_back(&entry);
    }

    std::vector<BacklinkSourceGroup> groups;
    for (const auto& [doc_id, doc_entries] : grouped)
    {
        BacklinkSourceGroup group;
        group.document_id = doc_id;
        group.document_title = doc_entries.empty() ? "" : doc_entries[0]->source_document_title;

        for (const auto* entry_ptr : doc_entries)
        {
            BacklinkMentionItem item;
            item.source_document_id = entry_ptr->source_document_id;
            item.source_document_title = entry_ptr->source_document_title;
            item.context_snippet = entry_ptr->context;
            item.line_number = entry_ptr->line_number;
            item.is_linked = is_linked;
            group.mentions.push_back(std::move(item));
        }

        // Sort mentions within group by line number
        std::sort(group.mentions.begin(),
                  group.mentions.end(),
                  [](const BacklinkMentionItem& lhs, const BacklinkMentionItem& rhs)
                  { return lhs.line_number < rhs.line_number; });

        groups.push_back(std::move(group));
    }

    return groups;
}

} // namespace markamp::core
