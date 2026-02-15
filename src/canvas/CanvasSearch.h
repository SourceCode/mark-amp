#pragma once

#include "canvas/CanvasTypes.h"

#include <string>
#include <vector>

namespace markamp::canvas
{

class Board;
class CanvasObject;

/// Represents a single search match within a canvas object.
struct SearchResult
{
    ObjectId object_id{kInvalidObjectId};
    std::string matched_text; ///< The exact matched substring.
    std::string context;      ///< Surrounding text for preview.
    int match_offset{0};      ///< Character offset within the object's full text.
    int match_length{0};      ///< Length of the match.
};

/// Provides text search across all canvas objects.
/// Extracts text from every object type and performs substring matching.
class CanvasSearch
{
public:
    /// Search all objects on the board for the query string.
    [[nodiscard]] static auto search(const Board& board,
                                     const std::string& query,
                                     bool case_sensitive = false) -> std::vector<SearchResult>;

    /// Extract all searchable text from a canvas object.
    /// Switches on object type: StickyNote→text(), TextBox→text(),
    /// Shape→text(), Connector→label(), Frame/Section→title(),
    /// Table→concatenated cell text, default→name().
    [[nodiscard]] static auto extract_text(const CanvasObject& obj) -> std::string;
};

} // namespace markamp::canvas
