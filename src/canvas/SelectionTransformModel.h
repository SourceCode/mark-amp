#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::canvas
{

/// Alignment direction.
enum class AlignDirection : uint8_t
{
    kLeft,
    kCenterH,
    kRight,
    kTop,
    kCenterV,
    kBottom,
};

/// Distribution direction.
enum class DistributeDirection : uint8_t
{
    kHorizontal,
    kVertical,
};

/// Transform handle type.
enum class HandleType : uint8_t
{
    kMove,
    kScale,
    kRotate,
};

/// Object rect for selection.
struct SelectionRect
{
    std::string object_id;
    double x{0.0};
    double y{0.0};
    double width{100.0};
    double height{100.0};

    // ── Round 5 Batch 4 (#34-35) ────────────────────────────────

    /// (#34) Area of the selection rect.
    [[nodiscard]] auto area() const noexcept -> double
    {
        return width * height;
    }

    /// (#35) Whether an object ID is set.
    [[nodiscard]] auto has_id() const noexcept -> bool
    {
        return !object_id.empty();
    }
};

/// Testable model for Selection, Transform & Alignment (Phase 48).
///
/// Encapsulates:
/// - Multi-object selection management
/// - Alignment commands (edges/centers)
/// - Distribution (equal spacing)
/// - Transform pivot point
class SelectionTransformModel
{
public:
    // ── Selection ───────────────────────────────────────────────────

    void set_selection(std::vector<SelectionRect> objects);
    [[nodiscard]] auto selection() const -> const std::vector<SelectionRect>&;
    [[nodiscard]] auto selection_count() const -> int;

    // ── Alignment ───────────────────────────────────────────────────

    /// Align all selected objects along specified edge/center.
    [[nodiscard]] auto aligned(AlignDirection direction) const -> std::vector<SelectionRect>;

    // ── Distribution ────────────────────────────────────────────────

    /// Distribute objects with equal spacing.
    [[nodiscard]] auto distributed(DistributeDirection direction) const
        -> std::vector<SelectionRect>;

    // ── Transform ───────────────────────────────────────────────────

    void set_pivot(double x_ratio, double y_ratio);
    [[nodiscard]] auto pivot_x() const -> double;
    [[nodiscard]] auto pivot_y() const -> double;

    void set_active_handle(HandleType handle);
    [[nodiscard]] auto active_handle() const -> HandleType;

private:
    std::vector<SelectionRect> selection_;
    double pivot_x_{0.5};
    double pivot_y_{0.5};
    HandleType active_handle_{HandleType::kMove};

    // ── Round 5 Batch 4 (#36-40) ────────────────────────────────

    /// (#36) Whether objects are selected.
    [[nodiscard]] auto has_selection() const noexcept -> bool
    {
        return !selection_.empty();
    }

    /// (#37) Whether the active handle is move.
    [[nodiscard]] auto is_move() const noexcept -> bool
    {
        return active_handle_ == HandleType::kMove;
    }

    /// (#38) Whether the active handle is scale.
    [[nodiscard]] auto is_scale() const noexcept -> bool
    {
        return active_handle_ == HandleType::kScale;
    }

    /// (#39) Whether the active handle is rotate.
    [[nodiscard]] auto is_rotate() const noexcept -> bool
    {
        return active_handle_ == HandleType::kRotate;
    }

    /// (#40) Whether pivot is at center.
    [[nodiscard]] auto is_center_pivot() const noexcept -> bool
    {
        return pivot_x_ == 0.5 && pivot_y_ == 0.5;
    }
};

} // namespace markamp::canvas
