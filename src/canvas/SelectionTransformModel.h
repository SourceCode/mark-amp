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
};

} // namespace markamp::canvas
