#pragma once

/**
 * @file GridGuideController.h
 * @brief Phase 49 Task 2,4: Grid settings, guide lines, and board persistence.
 */

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::canvas
{

/// Grid style.
enum class GridStyle : uint8_t
{
    kDots,
    kLines,
    kCrosshairs,
};

/// Grid configuration.
struct GridConfig
{
    double spacing{20.0};
    int major_interval{5}; ///< Every N lines is a major line
    GridStyle style{GridStyle::kDots};
    bool visible{true};
    uint32_t color_rgba{0xCCCCCC40}; ///< Very light grey

    /// Get style as string.
    [[nodiscard]] auto style_name() const -> std::string;
};

/// A user-defined guide line.
struct GuideLine
{
    std::string guide_id;
    bool is_horizontal{true};
    double position{0.0}; ///< X for vertical, Y for horizontal
    bool locked{false};
};

/**
 * @brief Manages grid and guide configuration per board.
 */
class GridGuideController
{
public:
    GridGuideController() = default;

    // ── Grid ───────────────────────────────────────────────────────

    /// Set grid configuration.
    void set_grid(const GridConfig& config);

    /// Get grid configuration.
    [[nodiscard]] auto grid() const -> const GridConfig&;

    /// Toggle grid visibility.
    void set_grid_visible(bool visible);

    /// Check if grid is visible.
    [[nodiscard]] auto grid_visible() const -> bool;

    /// Snap a value to the nearest grid line.
    [[nodiscard]] auto snap_to_grid(double value) const -> double;

    // ── Guides ─────────────────────────────────────────────────────

    /// Add a guide line.
    void add_guide(const GuideLine& guide);

    /// Remove a guide by ID.
    void remove_guide(const std::string& guide_id);

    /// Get guide count.
    [[nodiscard]] auto guide_count() const -> int;

    /// Get all guides.
    [[nodiscard]] auto guides() const -> const std::vector<GuideLine>&;

    /// Lock/unlock a guide.
    void set_guide_locked(const std::string& guide_id, bool locked);

    /// Clear all guides.
    void clear_guides();

private:
    GridConfig grid_;
    std::vector<GuideLine> guides_;
};

} // namespace markamp::canvas
