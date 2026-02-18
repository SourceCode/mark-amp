// ============================================================================
// File: src/canvas/CanvasObjectAligner.h
// Phase 12: Canvas Advanced Objects — alignment and distribution
// ============================================================================
#pragma once

#include "canvas/Board.h"
#include "canvas/CanvasTypes.h"

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::canvas
{

/// Alignment direction for align operations.
enum class AlignDirection : uint8_t
{
    kLeft,
    kRight,
    kTop,
    kBottom,
    kCenterHorizontal,
    kCenterVertical
};

/// Distribution mode for distribute operations.
enum class DistributeMode : uint8_t
{
    kHorizontal,
    kVertical,
    kEqualSpacingH,
    kEqualSpacingV
};

/// An alignment suggestion from auto-detect.
struct AlignmentSuggestion
{
    AlignDirection direction;
    double reference_value{0.0}; ///< The coordinate to align to
    size_t matching_objects{0};  ///< How many objects are near this alignment
    double max_deviation{0.0};   ///< Maximum deviation from perfect alignment
};

/// Result of an alignment or distribution operation.
struct AlignResult
{
    bool success{false};
    size_t objects_moved{0};
};

/// Canvas object alignment and distribution service.
///
/// Provides:
///   - Align left/right/top/bottom/center horizontally/vertically
///   - Distribute horizontally/vertically with equal spacing
///   - Smart alignment suggestions (detect near-aligned objects)
///   - Reference object selection (align to first selected)
class CanvasObjectAligner
{
public:
    explicit CanvasObjectAligner(Board& board);

    // ── Alignment ─────────────────────────────────────────────────

    /// Align objects in a given direction.
    /// By default, aligns to the first object in the list (reference object).
    auto align(const std::vector<ObjectId>& ids, AlignDirection direction) -> AlignResult;

    /// Align objects to a specific reference object.
    auto align_to_reference(const std::vector<ObjectId>& ids,
                            ObjectId reference_id,
                            AlignDirection direction) -> AlignResult;

    // ── Distribution ──────────────────────────────────────────────

    /// Distribute objects evenly along an axis.
    auto distribute(const std::vector<ObjectId>& ids, DistributeMode mode) -> AlignResult;

    /// Distribute with a specific spacing value.
    auto distribute_with_spacing(const std::vector<ObjectId>& ids,
                                 DistributeMode mode,
                                 double spacing) -> AlignResult;

    // ── Smart Suggestions ─────────────────────────────────────────

    /// Detect alignment opportunities among the given objects.
    [[nodiscard]] auto suggest_alignments(const std::vector<ObjectId>& ids,
                                          double tolerance = 5.0) const
        -> std::vector<AlignmentSuggestion>;

    // ── Utility ───────────────────────────────────────────────────

    /// Get a human-readable name for an alignment direction.
    [[nodiscard]] static auto direction_name(AlignDirection direction) -> std::string;

    /// Get a human-readable name for a distribution mode.
    [[nodiscard]] static auto distribute_mode_name(DistributeMode mode) -> std::string;

private:
    Board& board_;

    /// Get position data for alignment computation.
    struct ObjectAlignData
    {
        ObjectId obj_id;
        double left{0.0};
        double right{0.0};
        double top{0.0};
        double bottom{0.0};
        double center_x{0.0};
        double center_y{0.0};
        double width{0.0};
        double height{0.0};
    };

    /// Collect alignment data for a set of objects.
    [[nodiscard]] auto collect_align_data(const std::vector<ObjectId>& ids) const
        -> std::vector<ObjectAlignData>;

    /// Move an object to a new position.
    auto move_object_to(ObjectId obj_id, double new_x, double new_y) -> void;
};

} // namespace markamp::canvas
