#pragma once

#include "../FlashcardTypes.h"

#include <cstdint>
#include <expected>
#include <optional>
#include <string>
#include <vector>

namespace markamp::core::fsrs
{

// ============================================================================
// Extracted Card: raw card data parsed from Markdown content
// ============================================================================

struct ExtractedCard
{
    std::string front;       // Front side content
    std::string back;        // Back side content
    std::string hint;        // Optional hint (empty if none)
    std::string source_file; // Path to the originating Markdown file
    int32_t source_line{0};  // Line number in the source file
    std::string block_id;    // Unique identifier derived from file + line
    bool is_cloze{false};    // True if this card uses cloze deletion syntax
};

// ============================================================================
// Sync Result: outcome of syncing extracted cards with the store
// ============================================================================

struct SyncResult
{
    int32_t cards_added{0};
    int32_t cards_removed{0};
    int32_t cards_unchanged{0};
    std::vector<std::string> added_card_ids;
    std::vector<std::string> removed_card_ids;
};

// ============================================================================
// FlashcardExtractor: parses Markdown documents for flashcard syntax
// Syntax:
//   ?? front / back ??             — basic card
//   ?? front / back / hint ??      — card with hint
//   ??c1 {{word}} in sentence ??   — cloze deletion card
// ============================================================================

class FlashcardExtractor
{
public:
    FlashcardExtractor() = default;

    /// Extract all flashcards from a Markdown document.
    /// Scans every line for `?? ... ??` patterns and returns extracted cards.
    [[nodiscard]] auto extract_from_document(const std::string& content,
                                             const std::string& file_path) const
        -> std::vector<ExtractedCard>;

    /// Extract a flashcard from a single line, if it contains flashcard syntax.
    [[nodiscard]] auto extract_from_line(const std::string& line,
                                         int32_t line_number,
                                         const std::string& file_path) const
        -> std::optional<ExtractedCard>;

    /// Sync extracted cards against existing cards in a deck.
    /// Adds new cards, identifies removed cards (cards whose block_id no longer appears
    /// in the extraction results). Does not delete — returns IDs for caller to handle.
    [[nodiscard]] auto sync_cards(const std::vector<ExtractedCard>& extracted,
                                  const FlashcardDeck& deck) const -> SyncResult;

    /// Generate a deterministic block ID from a file path and line number.
    [[nodiscard]] static auto generate_block_id(const std::string& file_path, int32_t line_number)
        -> std::string;

private:
    /// Trim leading and trailing whitespace from a string.
    [[nodiscard]] static auto trim(const std::string& str) -> std::string;
};

} // namespace markamp::core::fsrs
