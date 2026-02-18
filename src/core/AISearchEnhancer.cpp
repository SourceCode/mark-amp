// ============================================================================
// File: src/core/AISearchEnhancer.cpp
// Phase 26: AI Integration — semantic search implementation
// ============================================================================

#include "AISearchEnhancer.h"

#include "AIService.h"

#include <algorithm>
#include <fmt/format.h>
#include <ranges>
#include <sstream>

namespace markamp::core
{

AISearchEnhancer::AISearchEnhancer(AIService& ai_service)
    : ai_service_(ai_service)
{
}

auto AISearchEnhancer::semantic_search(
    const std::string& query, const std::vector<std::pair<std::string, std::string>>& documents)
    -> std::vector<SemanticSearchResult>
{
    if (documents.empty())
    {
        return {};
    }

    // Build context with document summaries.
    std::string context = "Documents to search:\n";
    for (const auto& [doc_id, doc_content] : documents)
    {
        // Use first 200 chars as summary.
        const std::string snippet =
            doc_content.substr(0, std::min(static_cast<size_t>(200), doc_content.size()));
        context += fmt::format("- Document '{}': {}\n", doc_id, snippet);
    }
    context += "\nFor each relevant document, provide:\n"
               "ID: [document id]\n"
               "Relevance: [0.0-1.0]\n"
               "Reason: [why it's relevant]\n"
               "Only include documents relevant to the query.";

    const auto response = ai_service_.execute_action(AIAction::Custom, query, context);

    if (!response.success || response.content.empty())
    {
        return {};
    }

    // Parse results from response.
    std::vector<SemanticSearchResult> results;
    std::istringstream stream(response.content);
    std::string line;

    SemanticSearchResult current;
    bool has_current = false;

    while (std::getline(stream, line))
    {
        while (!line.empty() && std::isspace(line.front()) != 0)
        {
            line.erase(line.begin());
        }
        while (!line.empty() && std::isspace(line.back()) != 0)
        {
            line.pop_back();
        }

        if (line.size() > 4 && line.substr(0, 3) == "ID:")
        {
            if (has_current && !current.document_id.empty())
            {
                results.push_back(current);
            }
            current = SemanticSearchResult{};
            current.document_id = line.substr(3);
            while (!current.document_id.empty() && current.document_id.front() == ' ')
            {
                current.document_id.erase(current.document_id.begin());
            }
            has_current = true;
        }
        else if (has_current && line.size() > 11 && line.substr(0, 10) == "Relevance:")
        {
            std::string relevance_str = line.substr(10);
            while (!relevance_str.empty() && relevance_str.front() == ' ')
            {
                relevance_str.erase(relevance_str.begin());
            }
            try
            {
                current.relevance = std::stod(relevance_str);
            }
            catch (...)
            {
                current.relevance = 0.5;
            }
        }
        else if (has_current && line.size() > 8 && line.substr(0, 7) == "Reason:")
        {
            current.explanation = line.substr(7);
            while (!current.explanation.empty() && current.explanation.front() == ' ')
            {
                current.explanation.erase(current.explanation.begin());
            }
        }
    }
    if (has_current && !current.document_id.empty())
    {
        results.push_back(current);
    }

    // Add snippets from original documents.
    for (auto& result : results)
    {
        for (const auto& [doc_id, doc_content] : documents)
        {
            if (doc_id == result.document_id)
            {
                result.snippet =
                    doc_content.substr(0, std::min(static_cast<size_t>(150), doc_content.size()));
                break;
            }
        }
    }

    // Sort by relevance descending.
    std::ranges::sort(results,
                      [](const SemanticSearchResult& lhs, const SemanticSearchResult& rhs)
                      { return lhs.relevance > rhs.relevance; });

    return results;
}

auto AISearchEnhancer::ask_question(
    const std::string& question, const std::vector<std::pair<std::string, std::string>>& documents)
    -> AIAnswerResult
{
    if (documents.empty())
    {
        AIAnswerResult empty_result;
        empty_result.answer = "No documents available to answer from.";
        empty_result.confidence = 0.0;
        return empty_result;
    }

    // Build context with full document content (truncated).
    std::string context = "Answer the question using only the following documents. "
                          "Cite source documents in your answer.\n\n";
    for (const auto& [doc_id, doc_content] : documents)
    {
        const std::string truncated =
            doc_content.substr(0, std::min(static_cast<size_t>(500), doc_content.size()));
        context += fmt::format("--- Document: {} ---\n{}\n\n", doc_id, truncated);
    }

    const auto response = ai_service_.execute_action(AIAction::Custom, question, context);

    AIAnswerResult result;
    if (response.success)
    {
        result.answer = response.content;
        result.confidence = 0.7;

        // Extract cited document IDs from the response.
        for (const auto& [doc_id, doc_content] : documents)
        {
            if (response.content.find(doc_id) != std::string::npos)
            {
                result.source_documents.push_back(doc_id);
            }
        }
        if (!result.source_documents.empty())
        {
            result.confidence = 0.8;
        }
    }
    else
    {
        result.answer =
            response.error_message.empty() ? "Failed to generate answer" : response.error_message;
        result.confidence = 0.0;
    }

    return result;
}

auto AISearchEnhancer::is_available() const -> bool
{
    return available_;
}

} // namespace markamp::core
