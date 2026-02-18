// ============================================================================
// File: src/core/AILinkSuggester.cpp
// Phase 26: AI Integration — wiki-link suggestion implementation
// ============================================================================

#include "AILinkSuggester.h"

#include "AIService.h"

#include <algorithm>
#include <ranges>
#include <sstream>

namespace markamp::core
{

AILinkSuggester::AILinkSuggester(AIService& ai_service)
    : ai_service_(ai_service)
{
}

auto AILinkSuggester::suggest_links(
    const std::string& document_content,
    const std::string& document_id,
    const std::vector<std::string>& existing_links,
    const std::vector<std::pair<std::string, std::string>>& available_documents)
    -> std::vector<LinkSuggestion>
{
    // Build context with available documents.
    std::string context = "Available documents for linking:\n";
    for (const auto& [doc_id, doc_title] : available_documents)
    {
        if (doc_id != document_id &&
            std::ranges::find(existing_links, doc_id) == existing_links.end())
        {
            context += "- [[" + doc_title + "]] (id: " + doc_id + ")\n";
        }
    }
    context += "\nSuggest relevant wiki-links from the available documents above. "
               "For each suggestion, provide the document title and a brief reason "
               "for the relevance.";

    const auto response = ai_service_.execute_action(AIAction::Custom, document_content, context);

    if (!response.success || response.content.empty())
    {
        return {};
    }

    auto suggestions = parse_suggestions(response.content, document_id, existing_links);

    // Sort by relevance descending.
    std::ranges::sort(suggestions,
                      [](const LinkSuggestion& lhs, const LinkSuggestion& rhs)
                      { return lhs.relevance > rhs.relevance; });

    return suggestions;
}

auto AILinkSuggester::accept_suggestion(const std::string& target_document) -> void
{
    if (std::ranges::find(accepted_, target_document) == accepted_.end())
    {
        accepted_.push_back(target_document);
    }
}

auto AILinkSuggester::accepted_history() const -> std::vector<std::string>
{
    return accepted_;
}

auto AILinkSuggester::parse_suggestions(const std::string& response,
                                        const std::string& document_id,
                                        const std::vector<std::string>& existing_links)
    -> std::vector<LinkSuggestion>
{
    std::vector<LinkSuggestion> suggestions;
    std::istringstream stream(response);
    std::string line;

    while (std::getline(stream, line))
    {
        // Trim whitespace.
        while (!line.empty() && std::isspace(line.front()) != 0)
        {
            line.erase(line.begin());
        }
        while (!line.empty() && std::isspace(line.back()) != 0)
        {
            line.pop_back();
        }
        if (line.empty())
        {
            continue;
        }

        // Strip bullet markers.
        if (line.front() == '-' || line.front() == '*')
        {
            line = line.substr(1);
            while (!line.empty() && line.front() == ' ')
            {
                line.erase(line.begin());
            }
        }
        if (line.empty())
        {
            continue;
        }

        LinkSuggestion suggestion;

        // Check for [[wiki-link]] format.
        const auto open_bracket = line.find("[[");
        const auto close_bracket = line.find("]]");
        if (open_bracket != std::string::npos && close_bracket != std::string::npos &&
            close_bracket > open_bracket)
        {
            suggestion.target_document =
                line.substr(open_bracket + 2, close_bracket - open_bracket - 2);
            suggestion.anchor_text = suggestion.target_document;

            // Reason is everything after the link.
            if (close_bracket + 2 < line.size())
            {
                std::string reason = line.substr(close_bracket + 2);
                while (!reason.empty() &&
                       (reason.front() == ':' || reason.front() == '-' || reason.front() == ' '))
                {
                    reason.erase(reason.begin());
                }
                suggestion.reason = reason;
            }
        }
        else
        {
            suggestion.target_document = line;
            suggestion.anchor_text = line;
        }

        // Skip self-links and already-linked documents.
        if (suggestion.target_document == document_id ||
            std::ranges::find(existing_links, suggestion.target_document) != existing_links.end())
        {
            continue;
        }

        suggestion.relevance = 0.7;
        if (!suggestion.reason.empty())
        {
            suggestion.relevance = 0.8;
        }

        suggestions.push_back(std::move(suggestion));
    }

    return suggestions;
}

} // namespace markamp::core
