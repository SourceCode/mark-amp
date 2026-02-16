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
    [[nodiscard]] static auto extract_text(const CanvasObject& obj) -> std::string;

    /// (#27) Find objects of a specific type.
    [[nodiscard]] static auto search_by_type(const Board& board, CanvasObjectType obj_type)
        -> std::vector<ObjectId>;

    /// (#28) Find objects with a matching tag.
    [[nodiscard]] static auto search_by_tag(const Board& board, const std::string& tag)
        -> std::vector<ObjectId>;

    /// (#29) Search-and-replace across all text-containing objects. Returns count of replacements.
    [[nodiscard]] static auto replace_text(Board& board,
                                           const std::string& find_str,
                                           const std::string& replace_str,
                                           bool case_sensitive = false) -> int;
};

} // namespace markamp::canvas
