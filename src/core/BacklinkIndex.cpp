#include "core/BacklinkIndex.h"

#include "core/DocumentModel.h"
#include "core/Events.h"
#include "core/VaultService.h"

#include <algorithm>
#include <chrono>
#include <ranges>
#include <sstream>

namespace markamp::core
{

// ============================================================================
// BacklinkEntry
// ============================================================================

auto BacklinkEntry::display_text() const -> std::string
{
    if (!link.display_text.empty())
    {
        return link.display_text;
    }
    return link.target;
}

// ============================================================================
// BacklinkResult
// ============================================================================

auto BacklinkResult::total_count() const -> int
{
    return linked_count() + unlinked_count();
}

auto BacklinkResult::linked_count() const -> int
{
    return static_cast<int>(linked_mentions.size());
}

auto BacklinkResult::unlinked_count() const -> int
{
    return static_cast<int>(unlinked_mentions.size());
}

// ============================================================================
// BacklinkIndex – construction
// ============================================================================

BacklinkIndex::BacklinkIndex(EventBus& event_bus, VaultService& vault_service)
    : event_bus_(event_bus)
    , vault_service_(vault_service)
{
    // Subscribe to vault opened event -> full rebuild
    vault_opened_sub_ = event_bus_.subscribe<events::VaultOpenedEvent>(
        std::function<void(const events::VaultOpenedEvent&)>(
            [this](const events::VaultOpenedEvent& /*opened_evt*/) { rebuild(); }));

    // Subscribe to document created -> incremental update
    doc_created_sub_ = event_bus_.subscribe<events::VaultDocumentCreatedEvent>(
        std::function<void(const events::VaultDocumentCreatedEvent&)>(
            [this](const events::VaultDocumentCreatedEvent& created_evt)
            { update_document(created_evt.document_id); }));

    // Subscribe to document deleted -> remove
    doc_deleted_sub_ = event_bus_.subscribe<events::VaultDocumentDeletedEvent>(
        std::function<void(const events::VaultDocumentDeletedEvent&)>(
            [this](const events::VaultDocumentDeletedEvent& deleted_evt)
            { remove_document(deleted_evt.document_id); }));
}

// ============================================================================
// BacklinkIndex – full rebuild
// ============================================================================

auto BacklinkIndex::rebuild() -> void
{
    auto start = std::chrono::steady_clock::now();

    const std::lock_guard guard(mutex_);

    forward_index_.clear();
    reverse_index_.clear();

    auto documents = vault_service_.list_documents();
    int total_links = 0;

    for (const auto& doc_entry : documents)
    {
        // Open the document to get parsed outgoing links
        auto relative_path =
            std::filesystem::relative(doc_entry.file_path, vault_service_.vault_path());
        auto doc_result = vault_service_.open_document(relative_path);
        if (!doc_result.has_value())
        {
            continue;
        }

        const auto& doc_model = doc_result.value();

        for (const auto& link : doc_model->outgoing_links())
        {
            if (link.is_embed)
            {
                continue; // Skip embedded content for backlink indexing
            }

            // Resolve the wikilink to a target document ID
            auto resolved_id = vault_service_.resolve_wikilink(link.target);
            if (!resolved_id.has_value())
            {
                continue; // Unresolved link — not in vault
            }

            BacklinkEntry entry;
            entry.source_document_id = doc_entry.document_id;
            entry.source_document_title = doc_entry.title;
            entry.target_document_id = resolved_id.value();
            entry.link = link;
            entry.line_number = find_link_line(doc_model->markdown(), link);
            entry.context = extract_context(doc_model->markdown(), entry.line_number);

            add_link(entry);
            ++total_links;
        }
    }

    auto end = std::chrono::steady_clock::now();
    const double kElapsed = std::chrono::duration<double, std::milli>(end - start).count();

    // Publish rebuild event (release lock first to avoid deadlock in handlers)
    events::BacklinkIndexRebuiltEvent rebuilt_evt;
    rebuilt_evt.total_links = total_links;
    rebuilt_evt.document_count = static_cast<int>(documents.size());
    rebuilt_evt.elapsed_ms = kElapsed;
    event_bus_.publish(rebuilt_evt);
}

// ============================================================================
// BacklinkIndex – incremental update
// ============================================================================

auto BacklinkIndex::update_document(const std::string& document_id) -> void
{
    std::lock_guard guard(mutex_);

    // Remove old outgoing links from this document
    remove_links_from(document_id);

    // Iterate documents to find by ID
    auto all_docs = vault_service_.list_documents();
    const VaultIndexEntry* matching_entry = nullptr;
    for (const auto& doc : all_docs)
    {
        if (doc.document_id == document_id)
        {
            matching_entry = &doc;
            break;
        }
    }

    if (matching_entry == nullptr)
    {
        return;
    }

    // Open the document
    auto relative_path =
        std::filesystem::relative(matching_entry->file_path, vault_service_.vault_path());
    auto doc_result = vault_service_.open_document(relative_path);
    if (!doc_result.has_value())
    {
        return;
    }

    const auto& doc_model = doc_result.value();
    int link_count = 0;

    for (const auto& link : doc_model->outgoing_links())
    {
        if (link.is_embed)
        {
            continue;
        }

        auto resolved_id = vault_service_.resolve_wikilink(link.target);
        if (!resolved_id.has_value())
        {
            continue;
        }

        BacklinkEntry entry;
        entry.source_document_id = document_id;
        entry.source_document_title = matching_entry->title;
        entry.target_document_id = resolved_id.value();
        entry.link = link;
        entry.line_number = find_link_line(doc_model->markdown(), link);
        entry.context = extract_context(doc_model->markdown(), entry.line_number);

        add_link(entry);
        ++link_count;
    }

    // Publish update event
    events::BacklinkIndexUpdatedEvent updated_evt;
    updated_evt.document_id = document_id;
    updated_evt.backlink_count = link_count;
    event_bus_.publish(updated_evt);
}

// ============================================================================
// BacklinkIndex – remove document
// ============================================================================

auto BacklinkIndex::remove_document(const std::string& document_id) -> void
{
    std::lock_guard guard(mutex_);

    // Remove all outgoing links from this document
    remove_links_from(document_id);

    // Remove all incoming links to this document
    reverse_index_.erase(document_id);

    // Also clean forward_index_ entries that reference this document as target
    for (auto& [source_id, entries] : forward_index_)
    {
        std::erase_if(entries,
                      [&](const BacklinkEntry& backlink_entry)
                      { return backlink_entry.target_document_id == document_id; });
    }
}

// ============================================================================
// Queries
// ============================================================================

auto BacklinkIndex::get_backlinks(const std::string& document_id) const
    -> std::vector<BacklinkEntry>
{
    std::lock_guard guard(mutex_);

    auto reverse_it = reverse_index_.find(document_id);
    if (reverse_it != reverse_index_.end())
    {
        return reverse_it->second;
    }
    return {};
}

auto BacklinkIndex::get_backlink_result(const std::string& document_id) const -> BacklinkResult
{
    BacklinkResult result;
    result.target_document_id = document_id;
    result.linked_mentions = get_backlinks(document_id);

    // Find the document entry to get title and aliases
    auto all_docs = vault_service_.list_documents();
    for (const auto& doc : all_docs)
    {
        if (doc.document_id == document_id)
        {
            result.unlinked_mentions = scan_unlinked_mentions(document_id, doc.title, doc.aliases);
            break;
        }
    }

    return result;
}

auto BacklinkIndex::get_outgoing_links(const std::string& document_id) const
    -> std::vector<BacklinkEntry>
{
    std::lock_guard guard(mutex_);

    auto forward_it = forward_index_.find(document_id);
    if (forward_it != forward_index_.end())
    {
        return forward_it->second;
    }
    return {};
}

auto BacklinkIndex::get_connected_documents(const std::string& document_id) const
    -> std::set<std::string>
{
    std::lock_guard guard(mutex_);
    std::set<std::string> connected;

    // Outgoing: documents this doc links to
    auto forward_it = forward_index_.find(document_id);
    if (forward_it != forward_index_.end())
    {
        for (const auto& entry : forward_it->second)
        {
            connected.insert(entry.target_document_id);
        }
    }

    // Incoming: documents that link to this doc
    auto reverse_it = reverse_index_.find(document_id);
    if (reverse_it != reverse_index_.end())
    {
        for (const auto& entry : reverse_it->second)
        {
            connected.insert(entry.source_document_id);
        }
    }

    return connected;
}

auto BacklinkIndex::backlink_count(const std::string& document_id) const -> int
{
    std::lock_guard guard(mutex_);

    auto reverse_it = reverse_index_.find(document_id);
    if (reverse_it != reverse_index_.end())
    {
        return static_cast<int>(reverse_it->second.size());
    }
    return 0;
}

auto BacklinkIndex::get_unlinked_mentions(const std::string& document_id) const
    -> std::vector<MentionEntry>
{
    auto all_docs = vault_service_.list_documents();
    for (const auto& doc : all_docs)
    {
        if (doc.document_id == document_id)
        {
            return scan_unlinked_mentions(document_id, doc.title, doc.aliases);
        }
    }
    return {};
}

auto BacklinkIndex::get_orphan_documents() const -> std::vector<std::string>
{
    const std::lock_guard guard(mutex_);
    std::vector<std::string> orphans;

    auto all_docs = vault_service_.list_documents();
    for (const auto& doc : all_docs)
    {
        const bool has_outgoing =
            forward_index_.contains(doc.document_id) && !forward_index_.at(doc.document_id).empty();
        const bool has_incoming =
            reverse_index_.contains(doc.document_id) && !reverse_index_.at(doc.document_id).empty();

        if (!has_outgoing && !has_incoming)
        {
            orphans.push_back(doc.document_id);
        }
    }

    return orphans;
}

auto BacklinkIndex::get_most_linked(int limit) const -> std::vector<std::pair<std::string, int>>
{
    std::lock_guard guard(mutex_);

    std::vector<std::pair<std::string, int>> results;
    results.reserve(reverse_index_.size());

    for (const auto& [doc_id, entries] : reverse_index_)
    {
        if (!entries.empty())
        {
            results.emplace_back(doc_id, static_cast<int>(entries.size()));
        }
    }

    // Sort by count descending
    std::sort(results.begin(),
              results.end(),
              [](const auto& lhs, const auto& rhs) { return lhs.second > rhs.second; });

    if (static_cast<int>(results.size()) > limit)
    {
        results.resize(static_cast<size_t>(limit));
    }

    return results;
}

// ============================================================================
// Private helpers
// ============================================================================

auto BacklinkIndex::add_link(const BacklinkEntry& entry) -> void
{
    forward_index_[entry.source_document_id].push_back(entry);
    reverse_index_[entry.target_document_id].push_back(entry);
}

auto BacklinkIndex::remove_links_from(const std::string& source_id) -> void
{
    // Get outgoing entries to find which targets to clean
    auto forward_it = forward_index_.find(source_id);
    if (forward_it != forward_index_.end())
    {
        for (const auto& entry : forward_it->second)
        {
            // Remove from reverse index
            auto reverse_it = reverse_index_.find(entry.target_document_id);
            if (reverse_it != reverse_index_.end())
            {
                std::erase_if(reverse_it->second,
                              [&](const BacklinkEntry& rev_entry)
                              { return rev_entry.source_document_id == source_id; });

                if (reverse_it->second.empty())
                {
                    reverse_index_.erase(reverse_it);
                }
            }
        }

        forward_index_.erase(forward_it);
    }
}

auto BacklinkIndex::extract_context(const std::string& content, int line_number, int context_lines)
    -> std::string
{
    if (content.empty() || line_number <= 0)
    {
        return {};
    }

    // Split into lines
    std::vector<std::string> lines;
    std::istringstream stream(content);
    std::string line;
    while (std::getline(stream, line))
    {
        lines.push_back(line);
    }

    if (lines.empty())
    {
        return {};
    }

    int target_idx = line_number - 1; // Convert to 0-indexed
    if (target_idx < 0)
    {
        target_idx = 0;
    }
    if (target_idx >= static_cast<int>(lines.size()))
    {
        target_idx = static_cast<int>(lines.size()) - 1;
    }

    const int start_idx = std::max(0, target_idx - context_lines);
    const int end_idx = std::min(static_cast<int>(lines.size()) - 1, target_idx + context_lines);

    std::string result;
    for (int idx = start_idx; idx <= end_idx; ++idx)
    {
        if (!result.empty())
        {
            result += ' ';
        }
        result += lines[static_cast<size_t>(idx)];
    }

    // Trim to reasonable length
    constexpr int kMaxContextLen = 200;
    if (static_cast<int>(result.size()) > kMaxContextLen)
    {
        result = result.substr(0, kMaxContextLen) + "...";
    }

    return result;
}

auto BacklinkIndex::find_link_line(const std::string& content, const WikiLink& link) -> int
{
    // Build the wikilink text form to search for
    std::string link_text = "[[" + link.target;
    if (!link.display_text.empty())
    {
        link_text += "|" + link.display_text;
    }
    link_text += "]]";

    auto pos = content.find(link_text);
    if (pos == std::string::npos)
    {
        // Try just the target
        pos = content.find("[[" + link.target + "]]");
    }
    if (pos == std::string::npos)
    {
        return 1;
    }

    // Count newlines before pos
    int line = 1;
    for (std::size_t idx = 0; idx < pos; ++idx)
    {
        if (content[idx] == '\n')
        {
            ++line;
        }
    }
    return line;
}

auto BacklinkIndex::scan_unlinked_mentions(const std::string& target_id,
                                           const std::string& target_title,
                                           const std::vector<std::string>& aliases) const
    -> std::vector<MentionEntry>
{
    std::vector<MentionEntry> mentions;

    if (target_title.empty())
    {
        return mentions;
    }

    // Build list of names to search for
    std::vector<std::string> search_terms;
    search_terms.push_back(target_title);
    for (const auto& alias : aliases)
    {
        search_terms.push_back(alias);
    }

    // Iterate all documents in the vault
    auto all_docs = vault_service_.list_documents();
    for (const auto& doc_entry : all_docs)
    {
        if (doc_entry.document_id == target_id)
        {
            continue; // Skip the target document itself
        }

        // Open the document to read its content
        auto relative_path =
            std::filesystem::relative(doc_entry.file_path, vault_service_.vault_path());
        auto doc_result = vault_service_.open_document(relative_path);
        if (!doc_result.has_value())
        {
            continue;
        }

        const auto& doc_model = doc_result.value();
        const auto& content = doc_model->markdown();

        for (const auto& term : search_terms)
        {
            // Search for the term in content
            const std::string term_lower = to_lower(term);
            const std::string content_lower = to_lower(content);
            std::size_t search_pos = 0;

            while ((search_pos = content_lower.find(term_lower, search_pos)) != std::string::npos)
            {
                // Check this occurrence is NOT inside [[...]] syntax
                bool inside_link = false;

                // Look backwards for [[ without ]]
                if (search_pos >= 2)
                {
                    auto bracket_start = content.rfind("[[", search_pos);
                    if (bracket_start != std::string::npos)
                    {
                        auto bracket_end = content.find("]]", bracket_start);
                        if (bracket_end != std::string::npos &&
                            bracket_end >= search_pos + term.size())
                        {
                            inside_link = true;
                        }
                    }
                }

                if (!inside_link)
                {
                    // Count line number
                    int line = 1;
                    for (std::size_t idx = 0; idx < search_pos; ++idx)
                    {
                        if (content[idx] == '\n')
                        {
                            ++line;
                        }
                    }

                    MentionEntry mention;
                    mention.source_document_id = doc_entry.document_id;
                    mention.source_document_title = doc_entry.title;
                    mention.mentioned_document_id = target_id;
                    mention.matched_text = content.substr(search_pos, term.size());
                    mention.line_number = line;
                    mention.context = extract_context(content, line);

                    mentions.push_back(std::move(mention));
                }

                search_pos += term.size();
            }
        }
    }

    return mentions;
}

auto BacklinkIndex::to_lower(const std::string& str) -> std::string
{
    std::string result = str;
    std::ranges::transform(
        result, result.begin(), [](unsigned char chr) { return std::tolower(chr); });
    return result;
}

} // namespace markamp::core
