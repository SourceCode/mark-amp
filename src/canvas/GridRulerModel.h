#pragma once

/**
 * @file GridRulerModel.h
 * @brief V17 Phase 01 W09: Grid & rulers configuration model.
 *
 * Testable model for grid overlay and ruler-bar settings:
 * visibility, spacing, units, and origin offset.
 */

#include "canvas/CanvasTypes.h"

#include <cstdint>
#include <string>

namespace markamp::canvas
{

/// Ruler measurement units.
enum class RulerUnits : uint8_t
{
    kPixels,
    kMillimeters,
    kInches,
    kPoints,
};

/// Testable model for grid and ruler settings.
class GridRulerModel
{
public:
    // ── Grid ────────────────────────────────────────────────────────

    /// Toggle grid overlay.
    auto set_grid_visible(bool visible) -> void { grid_visible_ = visible; }

    /// Whether grid is visible.
    [[nodiscard]] auto is_grid_visible() const noexcept -> bool { return grid_visible_; }

    /// Set grid spacing (in world units).
    auto set_grid_spacing(double spacing) -> void { grid_spacing_ = spacing; }

    /// Get grid spacing.
    [[nodiscard]] auto grid_spacing() const noexcept -> double { return grid_spacing_; }

    // ── Rulers ──────────────────────────────────────────────────────

    /// Toggle ruler bars.
    auto set_ruler_visible(bool visible) -> void { ruler_visible_ = visible; }

    /// Whether rulers are visible.
    [[nodiscard]] auto is_ruler_visible() const noexcept -> bool { return ruler_visible_; }

    /// Set ruler units.
    auto set_ruler_units(RulerUnits units) -> void { units_ = units; }

    /// Get ruler units.
    [[nodiscard]] auto ruler_units() const noexcept -> RulerUnits { return units_; }

    // ── Origin ──────────────────────────────────────────────────────

    /// Set ruler origin offset.
    auto set_origin(const Point2D& origin) -> void { origin_ = origin; }

    /// Get ruler origin offset.
    [[nodiscard]] auto origin() const noexcept -> const Point2D& { return origin_; }

    // ── Helpers ─────────────────────────────────────────────────────

    /// Human-readable units string.
    [[nodiscard]] static auto units_string(RulerUnits units) -> std::string
    {
        switch (units)
        {
        case RulerUnits::kPixels: return "px";
        case RulerUnits::kMillimeters: return "mm";
        case RulerUnits::kInches: return "in";
        case RulerUnits::kPoints: return "pt";
        }
        return "px";
    }

private:
    bool grid_visible_{false};
    double grid_spacing_{20.0};
    bool ruler_visible_{false};
    RulerUnits units_{RulerUnits::kPixels};
    Point2D origin_{0.0, 0.0};
};

} // namespace markamp::canvas
