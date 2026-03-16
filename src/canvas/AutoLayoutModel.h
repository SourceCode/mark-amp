#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::canvas
{

/// Layout preset type.
enum class LayoutPresetType : uint8_t
{
    kRow,
    kColumn,
    kGrid,
};

/// An object position for layout.
struct LayoutObject
{
    std::string object_id;
    double pos_x{0.0};
    double pos_y{0.0};
    double width{100.0};
    double height{100.0};
    std::string container_id; ///< Empty if no container

    // ── Round 4 Batch 8 (#79-80) ────────────────────────────────

    /// (#79) Whether this object belongs to a container.
    [[nodiscard]] auto has_container() const noexcept -> bool
    {
        return !container_id.empty();
    }

    /// (#80) Area of the object.
    [[nodiscard]] auto area() const noexcept -> double
    {
        return width * height;
    }
};

/// Testable model for Smart Layout/Auto-Arrange (Phase 75).
///
/// Encapsulates:
/// - Auto-align and tidy actions
/// - Spacing normalization
/// - Flow layout presets (row, column, grid)
/// - Container-aware layout boundaries
class AutoLayoutModel
{
public:
    // ── Objects ─────────────────────────────────────────────────────

    void set_objects(std::vector<LayoutObject> objects);
    [[nodiscard]] auto objects() const -> const std::vector<LayoutObject>&;

    // ── Spacing ─────────────────────────────────────────────────────

    void set_spacing(double spacing);
    [[nodiscard]] auto spacing() const -> double;

    // ── Layout ──────────────────────────────────────────────────────

    void apply_preset(LayoutPresetType preset, double start_x, double start_y);
    void normalize_spacing();

    // ── Container awareness ─────────────────────────────────────────

    void set_container_bounds(double bound_x, double bound_y, double bound_w, double bound_h);
    [[nodiscard]] auto respects_bounds() const -> bool;

private:
    std::vector<LayoutObject> objects_;
    double spacing_{20.0};
    double bound_x_{0.0};
    double bound_y_{0.0};
    double bound_w_{10000.0};
    double bound_h_{10000.0};
};

} // namespace markamp::canvas
