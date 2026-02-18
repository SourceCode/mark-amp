// ============================================================================
// File: src/core/AITagSuggester.h
// Phase 26: AI Integration — AI-powered tag suggestions
// ============================================================================
#pragma once

#include "AITypes.h"

#include <string>
#include <vector>

namespace markamp::core
{

class AIService;

/// AI-powered tag suggestions for documents. Analyzes content and suggests
/// both existing workspace tags and new tags with confidence scores.
class AITagSuggester
{
public:
    explicit AITagSuggester(AIService& ai_service);

    /// Suggest tags for a document based on its content.
    /// @param document_content The full document text.
    /// @param existing_tags Tags already in the workspace for context.
    /// @return Sorted list of tag suggestions (highest confidence first).
    [[nodiscard]] auto suggest_tags(const std::string& document_content,
                                    const std::vector<std::string>& existing_tags = {})
        -> std::vector<TagSuggestion>;

    /// Record that a tag suggestion was accepted (for learning).
    auto accept_suggestion(const std::string& tag) -> void;

    /// Record that a tag suggestion was rejected (for learning).
    auto reject_suggestion(const std::string& tag) -> void;

    /// Get list of previously accepted tags.
    [[nodiscard]] auto accepted_history() const -> std::vector<std::string>;

    /// Get list of previously rejected tags.
    [[nodiscard]] auto rejected_history() const -> std::vector<std::string>;

private:
    AIService& ai_service_;
    std::vector<std::string> accepted_;
    std::vector<std::string> rejected_;

    /// Parse structured tag suggestions from AI response text.
    [[nodiscard]] static auto parse_suggestions(const std::string& response,
                                                const std::vector<std::string>& existing_tags)
        -> std::vector<TagSuggestion>;
};

} // namespace markamp::core
