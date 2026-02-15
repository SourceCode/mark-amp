#pragma once

#include "canvas/CanvasTypes.h"

#include <string>
#include <utility>
#include <vector>

namespace markamp::canvas
{

class StickyNote;

/// Describes a suggested merge between two similar sticky notes.
struct AggregationSuggestion
{
    ObjectId note_a;
    ObjectId note_b;
    double similarity{0.0}; // 0.0–1.0
    std::string reason;     // e.g. "Identical text", "Similar content"
};

/// Detects duplicate or near-duplicate sticky notes using word-set
/// Jaccard similarity.
class StickyAggregator
{
public:
    /// Find duplicate or near-duplicate sticky notes.
    [[nodiscard]] auto
    find_duplicates(const std::vector<std::pair<ObjectId, const StickyNote*>>& notes,
                    double threshold = 0.8) const -> std::vector<AggregationSuggestion>;

    /// Compute text similarity (Jaccard index on word sets).
    [[nodiscard]] auto text_similarity(const std::string& text_a, const std::string& text_b) const
        -> double;
};

} // namespace markamp::canvas
