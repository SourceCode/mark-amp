// ============================================================================
// File: src/core/AISearchEnhancer.h
// Phase 26: AI Integration — AI-enhanced semantic search
// ============================================================================
#pragma once

#include "AITypes.h"

#include <string>
#include <vector>

namespace markamp::core
{

class AIService;

/// AI-enhanced search over workspace content: semantic search using
/// LLM analysis and question-answering with source citations.
class AISearchEnhancer
{
public:
    explicit AISearchEnhancer(AIService& ai_service);

    /// Semantic search: find conceptually similar documents to the query.
    /// @param query Natural language search query.
    /// @param documents Document (id, content) pairs to search over.
    [[nodiscard]] auto
    semantic_search(const std::string& query,
                    const std::vector<std::pair<std::string, std::string>>& documents)
        -> std::vector<SemanticSearchResult>;

    /// Ask a question and get an answer synthesized from workspace documents.
    /// @param question Natural language question.
    /// @param documents Document (id, content) pairs for context.
    [[nodiscard]] auto
    ask_question(const std::string& question,
                 const std::vector<std::pair<std::string, std::string>>& documents)
        -> AIAnswerResult;

    /// Check if AI search is available (provider configured).
    [[nodiscard]] auto is_available() const -> bool;

private:
    AIService& ai_service_;
    bool available_{true};
};

} // namespace markamp::core
