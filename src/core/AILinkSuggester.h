// ============================================================================
// File: src/core/AILinkSuggester.h
// Phase 26: AI Integration — AI-powered wiki-link suggestions
// ============================================================================
#pragma once

#include "AITypes.h"

#include <string>
#include <vector>

namespace markamp::core
{

class AIService;

/// AI-powered wiki-link suggestions. Analyzes document content and suggests
/// links to related documents in the workspace.
class AILinkSuggester
{
public:
    explicit AILinkSuggester(AIService& ai_service);

    /// Suggest wiki-links for a document.
    /// @param document_content The document text to analyze.
    /// @param document_id The current document's ID (to exclude self-links).
    /// @param existing_links Already-linked document IDs to exclude.
    /// @param available_documents Documents available for linking (id, title pairs).
    [[nodiscard]] auto
    suggest_links(const std::string& document_content,
                  const std::string& document_id,
                  const std::vector<std::string>& existing_links = {},
                  const std::vector<std::pair<std::string, std::string>>& available_documents = {})
        -> std::vector<LinkSuggestion>;

    /// Record that a link suggestion was accepted.
    auto accept_suggestion(const std::string& target_document) -> void;

    /// Get list of accepted document links.
    [[nodiscard]] auto accepted_history() const -> std::vector<std::string>;

private:
    AIService& ai_service_;
    std::vector<std::string> accepted_;

    /// Parse link suggestions from AI response text.
    [[nodiscard]] static auto parse_suggestions(const std::string& response,
                                                const std::string& document_id,
                                                const std::vector<std::string>& existing_links)
        -> std::vector<LinkSuggestion>;
};

} // namespace markamp::core
