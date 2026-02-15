/// @file LinkSuggestionService.cpp
/// @brief V4 Phase 11 – Smart Linking Suggestions implementation.

#include "core/LinkSuggestionService.h"

#include "core/BacklinkIndex.h"
#include "core/Events.h"
#include "core/SearchEngine.h"
#include "core/VaultService.h"

#include <algorithm>
#include <cctype>
#include <set>
#include <unordered_set>

namespace markamp::core
{

// ============================================================================
// Constructor
// ============================================================================

LinkSuggestionService::LinkSuggestionService(EventBus& event_bus,
                                             VaultService& vault_service,
                                             BacklinkIndex& backlink_index,
                                             SearchEngine& search_engine)
    : event_bus_(event_bus)
    , vault_service_(vault_service)
    , backlink_index_(backlink_index)
    , search_engine_(search_engine)
{
}

// ============================================================================
// Autocomplete
// ============================================================================

auto LinkSuggestionService::autocomplete(const std::string& prefix, int limit) const
    -> std::vector<AutocompleteResult>
{
    std::vector<AutocompleteResult> results;
    if (prefix.empty())
    {
        return results;
    }

    // Lowercase prefix for case-insensitive matching
    std::string lower_prefix = prefix;
    std::transform(lower_prefix.begin(),
                   lower_prefix.end(),
                   lower_prefix.begin(),
                   [](unsigned char chr) { return static_cast<char>(std::tolower(chr)); });

    const auto docs = vault_service_.list_documents();

    for (const auto& doc : docs)
    {
        // Check title
        std::string lower_title = doc.title;
        std::transform(lower_title.begin(),
                       lower_title.end(),
                       lower_title.begin(),
                       [](unsigned char chr) { return static_cast<char>(std::tolower(chr)); });

        double score = 0.0;
        bool matched = false;

        // Exact prefix match on title
        if (lower_title.starts_with(lower_prefix))
        {
            score = 100.0;
            matched = true;
        }
        // Contains match
        else if (lower_title.find(lower_prefix) != std::string::npos)
        {
            score = 50.0;
            matched = true;
        }

        // Check aliases
        if (!matched)
        {
            for (const auto& alias : doc.aliases)
            {
                std::string lower_alias = alias;
                std::transform(lower_alias.begin(),
                               lower_alias.end(),
                               lower_alias.begin(),
                               [](unsigned char chr)
                               { return static_cast<char>(std::tolower(chr)); });

                if (lower_alias.starts_with(lower_prefix))
                {
                    score = 90.0;
                    matched = true;
                    break;
                }
                if (lower_alias.find(lower_prefix) != std::string::npos)
                {
                    score = 40.0;
                    matched = true;
                    break;
                }
            }
        }

        // Fuzzy match fallback
        if (!matched && prefix.size() >= 3)
        {
            int dist =
                levenshtein_distance(lower_prefix, lower_title.substr(0, lower_prefix.size()));
            if (dist <= static_cast<int>(prefix.size()) / 3)
            {
                score = 20.0 - static_cast<double>(dist);
                matched = true;
            }
        }

        if (matched)
        {
            AutocompleteResult result;
            result.display_text = doc.title;
            result.insert_text = doc.title;
            result.subtitle = doc.file_path.string();
            result.score = score;
            results.push_back(std::move(result));
        }
    }

    // Sort by score descending
    std::sort(results.begin(),
              results.end(),
              [](const AutocompleteResult& lhs, const AutocompleteResult& rhs)
              { return lhs.score > rhs.score; });

    // Apply limit
    if (static_cast<int>(results.size()) > limit)
    {
        results.resize(static_cast<size_t>(limit));
    }

    return results;
}

// ============================================================================
// Suggest Links
// ============================================================================

auto LinkSuggestionService::suggest_links(const std::string& document_id, int limit) const
    -> std::vector<LinkSuggestion>
{
    std::vector<LinkSuggestion> suggestions;
    std::unordered_set<std::string> seen_ids;

    const auto docs = vault_service_.list_documents();

    // Find the source document's tags
    std::vector<std::string> source_tags;
    for (const auto& doc : docs)
    {
        if (doc.document_id == document_id)
        {
            source_tags = doc.tags;
            break;
        }
    }

    // Get existing outgoing links to exclude already-linked docs
    const auto outgoing = backlink_index_.get_outgoing_links(document_id);
    std::unordered_set<std::string> already_linked;
    for (const auto& link : outgoing)
    {
        already_linked.insert(link.target_document_id);
    }

    for (const auto& doc : docs)
    {
        if (doc.document_id == document_id)
        {
            continue; // Skip self
        }

        // Skip already-linked documents
        if (already_linked.contains(doc.document_id) || already_linked.contains(doc.title))
        {
            continue;
        }

        double best_score = 0.0;
        LinkSuggestion::Source best_source = LinkSuggestion::Source::kTitleMatch;
        std::vector<std::string> shared_tags;

        // Tag overlap scoring
        if (!source_tags.empty() && !doc.tags.empty())
        {
            double tag_score = score_by_tag_overlap(document_id, doc.document_id);
            if (tag_score > 0.0)
            {
                best_score = std::max(best_score, tag_score * 60.0);
                best_source = LinkSuggestion::Source::kTagOverlap;

                // Compute shared tags
                for (const auto& tag : source_tags)
                {
                    for (const auto& other_tag : doc.tags)
                    {
                        if (tag == other_tag)
                        {
                            shared_tags.push_back(tag);
                        }
                    }
                }
            }
        }

        // Graph proximity
        double proximity = score_by_graph_proximity(document_id, doc.document_id);
        if (proximity > 0.0)
        {
            double prox_score = proximity * 50.0;
            if (prox_score > best_score)
            {
                best_score = prox_score;
                best_source = LinkSuggestion::Source::kGraphProximity;
            }
        }

        // Co-link scoring
        double co_link = score_by_co_links(document_id, doc.document_id);
        if (co_link > 0.0)
        {
            double co_score = co_link * 40.0;
            if (co_score > best_score)
            {
                best_score = co_score;
                best_source = LinkSuggestion::Source::kFrequentCoLink;
            }
        }

        if (best_score > 0.0 && seen_ids.insert(doc.document_id).second)
        {
            LinkSuggestion suggestion;
            suggestion.document_id = doc.document_id;
            suggestion.document_title = doc.title;
            suggestion.file_path = doc.file_path.string();
            suggestion.relevance_score = best_score;
            suggestion.source = best_source;
            suggestion.shared_tags = std::move(shared_tags);
            suggestions.push_back(std::move(suggestion));
        }
    }

    // Sort by relevance descending
    std::sort(suggestions.begin(),
              suggestions.end(),
              [](const LinkSuggestion& lhs, const LinkSuggestion& rhs)
              { return lhs.relevance_score > rhs.relevance_score; });

    if (static_cast<int>(suggestions.size()) > limit)
    {
        suggestions.resize(static_cast<size_t>(limit));
    }

    return suggestions;
}

// ============================================================================
// Detect Link Targets
// ============================================================================

auto LinkSuggestionService::detect_link_targets(const std::string& text) const
    -> std::vector<std::pair<std::string, std::string>>
{
    std::vector<std::pair<std::string, std::string>> targets;
    if (text.empty())
    {
        return targets;
    }

    const auto docs = vault_service_.list_documents();

    // Lowercase text for matching
    std::string lower_text = text;
    std::transform(lower_text.begin(),
                   lower_text.end(),
                   lower_text.begin(),
                   [](unsigned char chr) { return static_cast<char>(std::tolower(chr)); });

    for (const auto& doc : docs)
    {
        // Check title match
        std::string lower_title = doc.title;
        std::transform(lower_title.begin(),
                       lower_title.end(),
                       lower_title.begin(),
                       [](unsigned char chr) { return static_cast<char>(std::tolower(chr)); });

        if (lower_title.size() >= 2 && lower_text.find(lower_title) != std::string::npos)
        {
            targets.emplace_back(doc.title, doc.document_id);
        }

        // Check alias match
        for (const auto& alias : doc.aliases)
        {
            std::string lower_alias = alias;
            std::transform(lower_alias.begin(),
                           lower_alias.end(),
                           lower_alias.begin(),
                           [](unsigned char chr) { return static_cast<char>(std::tolower(chr)); });

            if (lower_alias.size() >= 2 && lower_text.find(lower_alias) != std::string::npos)
            {
                targets.emplace_back(alias, doc.document_id);
            }
        }
    }

    return targets;
}

// ============================================================================
// Scoring Helpers
// ============================================================================

auto LinkSuggestionService::score_by_tag_overlap(const std::string& doc_a,
                                                 const std::string& doc_b) const -> double
{
    const auto docs = vault_service_.list_documents();

    std::vector<std::string> tags_a;
    std::vector<std::string> tags_b;

    for (const auto& doc : docs)
    {
        if (doc.document_id == doc_a)
        {
            tags_a = doc.tags;
        }
        if (doc.document_id == doc_b)
        {
            tags_b = doc.tags;
        }
    }

    if (tags_a.empty() || tags_b.empty())
    {
        return 0.0;
    }

    // Jaccard-like: shared / max(a, b)
    int shared = 0;
    for (const auto& tag : tags_a)
    {
        for (const auto& other : tags_b)
        {
            if (tag == other)
            {
                ++shared;
                break;
            }
        }
    }

    return static_cast<double>(shared) /
           static_cast<double>(std::max(tags_a.size(), tags_b.size()));
}

auto LinkSuggestionService::score_by_graph_proximity(const std::string& doc_a,
                                                     const std::string& doc_b) const -> double
{
    // Check if A and B share a common neighbor via the backlink index
    const auto outgoing_a = backlink_index_.get_outgoing_links(doc_a);
    const auto outgoing_b = backlink_index_.get_outgoing_links(doc_b);

    std::unordered_set<std::string> neighbors_a;
    for (const auto& link : outgoing_a)
    {
        neighbors_a.insert(link.target_document_id);
    }

    // Also include links pointing to A
    const auto backlinks_a = backlink_index_.get_backlinks(doc_a);
    for (const auto& backlink : backlinks_a)
    {
        neighbors_a.insert(backlink.source_document_id);
    }

    int shared_neighbors = 0;
    for (const auto& link : outgoing_b)
    {
        if (neighbors_a.contains(link.target_document_id))
        {
            ++shared_neighbors;
        }
    }

    const auto backlinks_b = backlink_index_.get_backlinks(doc_b);
    for (const auto& backlink : backlinks_b)
    {
        if (neighbors_a.contains(backlink.source_document_id))
        {
            ++shared_neighbors;
        }
    }

    if (shared_neighbors == 0)
    {
        return 0.0;
    }

    return std::min(1.0, static_cast<double>(shared_neighbors) / 3.0);
}

auto LinkSuggestionService::score_by_co_links(const std::string& doc_a,
                                              const std::string& doc_b) const -> double
{
    // Count docs that link to both A and B
    const auto backlinks_a = backlink_index_.get_backlinks(doc_a);
    const auto backlinks_b = backlink_index_.get_backlinks(doc_b);

    std::unordered_set<std::string> linking_to_a;
    for (const auto& backlink : backlinks_a)
    {
        linking_to_a.insert(backlink.source_document_id);
    }

    int co_links = 0;
    for (const auto& backlink : backlinks_b)
    {
        if (linking_to_a.contains(backlink.source_document_id))
        {
            ++co_links;
        }
    }

    if (co_links == 0)
    {
        return 0.0;
    }

    return std::min(1.0, static_cast<double>(co_links) / 5.0);
}

auto LinkSuggestionService::levenshtein_distance(const std::string& str_a,
                                                 const std::string& str_b) const -> int
{
    const size_t len_a = str_a.size();
    const size_t len_b = str_b.size();

    std::vector<std::vector<int>> dist(len_a + 1, std::vector<int>(len_b + 1, 0));

    for (size_t idx = 0; idx <= len_a; ++idx)
    {
        dist[idx][0] = static_cast<int>(idx);
    }
    for (size_t idx = 0; idx <= len_b; ++idx)
    {
        dist[0][idx] = static_cast<int>(idx);
    }

    for (size_t row = 1; row <= len_a; ++row)
    {
        for (size_t col = 1; col <= len_b; ++col)
        {
            int cost = (str_a[row - 1] == str_b[col - 1]) ? 0 : 1;
            dist[row][col] = std::min(
                {dist[row - 1][col] + 1, dist[row][col - 1] + 1, dist[row - 1][col - 1] + cost});
        }
    }

    return dist[len_a][len_b];
}

} // namespace markamp::core
