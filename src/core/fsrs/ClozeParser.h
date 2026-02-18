#pragma once

#include <cstdint>
#include <string>
#include <utility>
#include <vector>

namespace markamp::core::fsrs
{

// ============================================================================
// Cloze Field: a single cloze deletion within a card
// ============================================================================

struct ClozeField
{
    int32_t index{0}; // Cloze index (1 for c1, 2 for c2, etc.)
    std::string text; // The hidden text
    std::string hint; // Optional hint shown in place of the blank
};

// ============================================================================
// Cloze Card: a card containing one or more cloze deletions
// ============================================================================

struct ClozeCard
{
    std::string original_text;      // The raw text with cloze markup
    std::vector<ClozeField> fields; // All cloze deletions found
    int32_t card_count{0};          // Number of unique cloze indices (= review cards)
};

// ============================================================================
// ClozeParser: parses and renders cloze deletion syntax
// Syntax:
//   {{c1::word}}            — basic cloze
//   {{c1::word::hint}}      — cloze with hint
//   {{text}}                — shorthand: auto-assigned c1
// ============================================================================

class ClozeParser
{
public:
    ClozeParser() = default;

    /// Parse cloze deletions from text.
    /// Extracts all {{cN::text}} and {{cN::text::hint}} patterns.
    [[nodiscard]] auto parse(const std::string& text) const -> ClozeCard;

    /// Render the front of a cloze card for a specific cloze index.
    /// The active cloze is replaced with [...] or [...hint...].
    /// Other cloze fields are shown as plain text.
    [[nodiscard]] auto render_front(const ClozeCard& card, int32_t active_index) const
        -> std::string;

    /// Render the back of a cloze card for a specific cloze index.
    /// The active cloze is shown highlighted; others shown as plain text.
    [[nodiscard]] auto render_back(const ClozeCard& card, int32_t active_index) const
        -> std::string;

    /// Generate front/back pairs for each unique cloze index.
    /// Each pair becomes a separate review card.
    [[nodiscard]] auto generate_cards(const ClozeCard& card) const
        -> std::vector<std::pair<std::string, std::string>>;

    /// Quick check whether text contains cloze syntax.
    [[nodiscard]] static auto is_cloze(const std::string& text) -> bool;

    /// Get a sorted list of unique cloze indices from a parsed cloze card.
    [[nodiscard]] static auto unique_indices(const ClozeCard& card) -> std::vector<int32_t>;
};

} // namespace markamp::core::fsrs
