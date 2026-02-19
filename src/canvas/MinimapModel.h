#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::canvas
{

/// Minimap render mode.
enum class MinimapMode : uint8_t
{
    kSimplified,
    kDetailed,
};

/// Frame marker for minimap quick-jump.
struct FrameMarker
{
    std::string frame_id;
    std::string label;
    double center_x{0.0};
    double center_y{0.0};
};

/// Testable model for Minimap & Board Navigator (Phase 64).
///
/// Encapsulates:
/// - Viewport rectangle (position + size in board coords)
/// - Minimap render mode (simplified/detailed)
/// - Frame markers for quick jump
/// - Navigation hotkey commands (fit, center, back)
/// - Navigation history stack
class MinimapModel
{
public:
    // ── Viewport ────────────────────────────────────────────────────

    void set_viewport(double vp_x, double vp_y, double vp_w, double vp_h);
    [[nodiscard]] auto viewport_x() const -> double;
    [[nodiscard]] auto viewport_y() const -> double;
    [[nodiscard]] auto viewport_w() const -> double;
    [[nodiscard]] auto viewport_h() const -> double;

    // ── Mode ────────────────────────────────────────────────────────

    void set_mode(MinimapMode mode);
    [[nodiscard]] auto mode() const -> MinimapMode;

    // ── Frame markers ───────────────────────────────────────────────

    void set_markers(std::vector<FrameMarker> markers);
    [[nodiscard]] auto markers() const -> const std::vector<FrameMarker>&;

    // ── Navigation ──────────────────────────────────────────────────

    void push_location(); ///< Save current viewport to history
    void navigate_back(); ///< Restore previous location
    [[nodiscard]] auto history_depth() const -> int;

private:
    double vp_x_{0.0};
    double vp_y_{0.0};
    double vp_w_{800.0};
    double vp_h_{600.0};
    MinimapMode mode_{MinimapMode::kSimplified};
    std::vector<FrameMarker> markers_;

    struct ViewportSnapshot
    {
        double snap_x;
        double snap_y;
        double snap_w;
        double snap_h;
    };
    std::vector<ViewportSnapshot> history_;
};

} // namespace markamp::canvas
