#pragma once

#include "../FlashcardTypes.h"

#include <cstdint>
#include <expected>
#include <string>
#include <vector>

namespace markamp::core::fsrs
{

// ============================================================================
// Export Format
// ============================================================================

enum class FlashcardExportFormat
{
    CSV,
    JSON
};

// ============================================================================
// Import Result
// ============================================================================

struct FlashcardImportResult
{
    int32_t cards_imported{0};
    int32_t cards_failed{0};
    std::vector<std::string> errors;
};

// ============================================================================
// FlashcardImportExport: CSV and JSON import/export for flashcard data
// ============================================================================

class FlashcardImportExport
{
public:
    FlashcardImportExport() = default;

    // ── Export ──

    /// Export a deck's cards to CSV format.
    /// Columns: front, back, tags, state, due, stability, difficulty, reps, lapses
    [[nodiscard]] auto export_to_csv(const FlashcardDeck& deck) const -> std::string;

    /// Export a deck's cards to JSON format.
    /// Produces a JSON array of card objects with all scheduling fields.
    [[nodiscard]] auto export_to_json(const FlashcardDeck& deck) const -> std::string;

    // ── Import ──

    /// Import cards from CSV data into a deck.
    /// Expects header row: front,back[,tags][,state]
    [[nodiscard]] auto import_from_csv(const std::string& csv_data,
                                       const std::string& deck_id) const
        -> std::expected<FlashcardImportResult, std::string>;

    /// Import cards from JSON data into a deck.
    /// Expects a JSON array of card objects.
    [[nodiscard]] auto import_from_json(const std::string& json_data,
                                        const std::string& deck_id) const
        -> std::expected<FlashcardImportResult, std::string>;

    // ── Utilities ──

    /// Auto-detect import format from data content.
    [[nodiscard]] static auto detect_format(const std::string& data) -> FlashcardExportFormat;

private:
    /// Parse a single CSV line respecting quoted fields.
    [[nodiscard]] static auto parse_csv_line(const std::string& line) -> std::vector<std::string>;

    /// Escape a string for CSV output (quote if needed).
    [[nodiscard]] static auto escape_csv_field(const std::string& field) -> std::string;

    /// Escape a string for JSON output.
    [[nodiscard]] static auto escape_json_string(const std::string& str) -> std::string;

    /// Convert a time_point to an ISO 8601 string.
    [[nodiscard]] static auto time_to_iso(std::chrono::system_clock::time_point tp) -> std::string;

    /// Parse an ISO 8601 string to a time_point.
    [[nodiscard]] static auto iso_to_time(const std::string& iso)
        -> std::chrono::system_clock::time_point;

    /// Convert CardState to a string label.
    [[nodiscard]] static auto state_to_string(CardState state) -> std::string;

    /// Parse a string label to CardState.
    [[nodiscard]] static auto string_to_state(const std::string& str) -> CardState;
};

} // namespace markamp::core::fsrs
