/// @file OrphanManager.h
/// @brief V9 Phase 16 – Orphan document detection and link suggestion engine.

#pragma once

#include "GraphTypes.h"

#include <algorithm>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace markamp::core
{

// ============================================================================
// Link Suggestion Types
// ============================================================================

/// Reason a link was suggested between two nodes.
enum class LinkSuggestionReason : uint8_t
{
    kTitleMention,     ///< Target's title appears in source content
    kTagOverlap,       ///< Source and target share tags
    kContentSimilarity ///< Content has significant term overlap
};

/// A suggested link between two nodes.
struct LinkSuggestion
{
    std::string target_id;
    std::string target_label;
    LinkSuggestionReason reason{LinkSuggestionReason::kTitleMention};
    double confidence{0.0}; ///< 0.0–1.0
};

/// An orphan node with metadata and suggested connections.
struct OrphanNode
{
    std::string node_id;
    std::string label;
    int word_count{0};
    std::vector<LinkSuggestion> suggested_links;
};

// ============================================================================
// OrphanManager
// ============================================================================

/// Detects orphan documents and suggests connections using heuristics.
class OrphanManager
{
public:
    // ── Orphan Detection ──

    /// Find all orphan nodes (zero connections) in the graph.
    [[nodiscard]] static auto find_orphans(const GraphData& graph) -> std::vector<OrphanNode>
    {
        // Build degree map
        std::unordered_set<std::string> connected;
        for (const auto& link : graph.links)
        {
            connected.insert(link.source_id);
            connected.insert(link.target_id);
        }

        std::vector<OrphanNode> orphans;
        for (const auto& node : graph.nodes)
        {
            if (!connected.contains(node.id))
            {
                OrphanNode orphan;
                orphan.node_id = node.id;
                orphan.label = node.label;
                orphan.word_count = estimate_word_count(node.label);
                orphans.push_back(std::move(orphan));
            }
        }

        return orphans;
    }

    // ── Link Suggestions ──

    /// Suggest connections for a specific node based on content analysis.
    /// @param node_id The node to find suggestions for.
    /// @param graph The full graph data.
    /// @param content The textual content of the node's document.
    /// @return Sorted vector of suggestions (highest confidence first).
    [[nodiscard]] static auto suggest_connections(const std::string& node_id,
                                                  const GraphData& graph,
                                                  const std::string& content)
        -> std::vector<LinkSuggestion>
    {
        std::vector<LinkSuggestion> suggestions;

        // Already connected nodes
        std::unordered_set<std::string> already_linked;
        for (const auto& link : graph.links)
        {
            if (link.source_id == node_id)
            {
                already_linked.insert(link.target_id);
            }
            if (link.target_id == node_id)
            {
                already_linked.insert(link.source_id);
            }
        }

        const auto content_lower = to_lower(content);

        for (const auto& node : graph.nodes)
        {
            if (node.id == node_id || already_linked.contains(node.id))
            {
                continue;
            }

            // Check title mention in content
            const auto label_lower = to_lower(node.label);
            if (!label_lower.empty() && label_lower.length() >= 3 &&
                content_lower.find(label_lower) != std::string::npos)
            {
                LinkSuggestion suggestion;
                suggestion.target_id = node.id;
                suggestion.target_label = node.label;
                suggestion.reason = LinkSuggestionReason::kTitleMention;
                // Confidence based on how specific the title is (longer = more specific)
                suggestion.confidence =
                    std::min(0.95, 0.5 + static_cast<double>(label_lower.length()) * 0.03);
                suggestions.push_back(std::move(suggestion));
                continue; // One suggestion per target
            }

            // Check tag overlap (using node icon field for tags, or node type)
            if (node.type == GraphNodeType::Tag)
            {
                // Tags are not document candidates for direct linking
                continue;
            }

            // Content similarity via shared significant terms
            if (!content.empty() && !node.label.empty())
            {
                const auto similarity = compute_term_overlap(content_lower, label_lower);
                if (similarity > 0.3)
                {
                    LinkSuggestion suggestion;
                    suggestion.target_id = node.id;
                    suggestion.target_label = node.label;
                    suggestion.reason = LinkSuggestionReason::kContentSimilarity;
                    suggestion.confidence = similarity * 0.6; // Scale down
                    suggestions.push_back(std::move(suggestion));
                }
            }
        }

        // Sort by confidence descending
        std::sort(suggestions.begin(),
                  suggestions.end(),
                  [](const LinkSuggestion& lhs, const LinkSuggestion& rhs)
                  { return lhs.confidence > rhs.confidence; });

        return suggestions;
    }

    /// Suggest connections for all orphan nodes.
    [[nodiscard]] static auto
    suggest_for_orphans(const GraphData& graph,
                        const std::unordered_map<std::string, std::string>& node_contents)
        -> std::vector<OrphanNode>
    {
        auto orphans = find_orphans(graph);

        for (auto& orphan : orphans)
        {
            auto content_it = node_contents.find(orphan.node_id);
            const std::string& content =
                (content_it != node_contents.end()) ? content_it->second : orphan.label;
            orphan.suggested_links = suggest_connections(orphan.node_id, graph, content);
        }

        return orphans;
    }

    /// Get a summary of orphan statistics.
    [[nodiscard]] static auto orphan_ratio(const GraphData& graph) -> double
    {
        if (graph.nodes.empty())
        {
            return 0.0;
        }
        const auto orphans = find_orphans(graph);
        return static_cast<double>(orphans.size()) / static_cast<double>(graph.nodes.size());
    }

private:
    /// Estimate word count from label text.
    [[nodiscard]] static auto estimate_word_count(const std::string& text) -> int
    {
        if (text.empty())
        {
            return 0;
        }
        int count = 1;
        bool in_space = false;
        for (const char ch : text)
        {
            if (ch == ' ' || ch == '\t' || ch == '\n')
            {
                if (!in_space)
                {
                    ++count;
                    in_space = true;
                }
            }
            else
            {
                in_space = false;
            }
        }
        return count;
    }

    /// Convert string to lowercase.
    [[nodiscard]] static auto to_lower(const std::string& str) -> std::string
    {
        std::string result = str;
        std::transform(result.begin(),
                       result.end(),
                       result.begin(),
                       [](unsigned char ch) { return static_cast<char>(std::tolower(ch)); });
        return result;
    }

    /// Compute term overlap between two strings (0.0–1.0).
    [[nodiscard]] static auto compute_term_overlap(const std::string& content,
                                                   const std::string& label) -> double
    {
        // Split label into words and check how many appear in content
        std::vector<std::string> words;
        std::string word;
        for (const char ch : label)
        {
            if (ch == ' ' || ch == '-' || ch == '_')
            {
                if (!word.empty())
                {
                    words.push_back(word);
                    word.clear();
                }
            }
            else
            {
                word += ch;
            }
        }
        if (!word.empty())
        {
            words.push_back(word);
        }

        if (words.empty())
        {
            return 0.0;
        }

        int matches = 0;
        for (const auto& term : words)
        {
            if (term.length() >= 3 && content.find(term) != std::string::npos)
            {
                ++matches;
            }
        }

        return static_cast<double>(matches) / static_cast<double>(words.size());
    }
};

} // namespace markamp::core
