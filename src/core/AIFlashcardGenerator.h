// ============================================================================
// File: src/core/AIFlashcardGenerator.h
// Phase 26: AI Integration — AI flashcard generation from documents
// ============================================================================
#pragma once

#include "AITypes.h"

#include <string>
#include <vector>

namespace markamp::core
{

class AIService;

/// Generates flashcards (Q&A and cloze) from document content using AI.
class AIFlashcardGenerator
{
public:
    explicit AIFlashcardGenerator(AIService& ai_service);

    /// Generate basic Q&A flashcards from document content.
    /// @param document_content The document text to extract knowledge from.
    /// @param count Desired number of cards (AI may generate fewer).
    /// @param difficulty Target difficulty level (0.0 = easy, 1.0 = hard).
    [[nodiscard]] auto generate_cards(const std::string& document_content,
                                      int count = 10,
                                      double difficulty = 0.5) -> std::vector<GeneratedCard>;

    /// Generate cloze deletion flashcards from document content.
    /// @param document_content The document text.
    /// @param count Desired number of cloze cards.
    [[nodiscard]] auto generate_cloze(const std::string& document_content, int count = 5)
        -> std::vector<GeneratedCard>;

private:
    AIService& ai_service_;

    /// Parse card pairs from AI response text.
    [[nodiscard]] static auto parse_cards(const std::string& response,
                                          CardType card_type,
                                          double difficulty) -> std::vector<GeneratedCard>;
};

} // namespace markamp::core
