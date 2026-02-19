#pragma once

#include <cstdint>
#include <string>

namespace markamp::canvas
{

/// Zoom preset.
enum class ZoomPreset : uint8_t
{
    kFiftyPercent,
    kHundredPercent,
    kTwoHundredPercent,
    kFitBoard,
    kFitSelection,
};

/// Testable model for Pan/Zoom Camera Ergonomics (Phase 65).
///
/// Encapsulates:
/// - Zoom level with cursor-focused anchor
/// - Pan offset
/// - Zoom presets and fit modes
/// - Camera boundary enforcement and recovery
/// - Smooth pan velocity settings
class CameraModel
{
public:
    // ── Zoom ────────────────────────────────────────────────────────

    void set_zoom(double zoom);
    [[nodiscard]] auto zoom() const -> double;

    void zoom_at(double delta, double cursor_x, double cursor_y);
    [[nodiscard]] auto anchor_x() const -> double;
    [[nodiscard]] auto anchor_y() const -> double;

    void apply_preset(ZoomPreset preset);

    // ── Pan ─────────────────────────────────────────────────────────

    void set_pan(double pan_x, double pan_y);
    [[nodiscard]] auto pan_x() const -> double;
    [[nodiscard]] auto pan_y() const -> double;

    // ── Velocity ────────────────────────────────────────────────────

    void set_pan_velocity(double velocity);
    [[nodiscard]] auto pan_velocity() const -> double;

    // ── Boundaries ──────────────────────────────────────────────────

    void set_bounds(double min_x, double min_y, double max_x, double max_y);
    void enforce_bounds();
    void recenter();

    [[nodiscard]] auto is_out_of_bounds() const -> bool;

private:
    double zoom_{1.0};
    double anchor_x_{0.0};
    double anchor_y_{0.0};
    double pan_x_{0.0};
    double pan_y_{0.0};
    double pan_velocity_{1.0};

    static constexpr double kMinZoom = 0.1;
    static constexpr double kMaxZoom = 10.0;

    double bound_min_x_{-10000.0};
    double bound_min_y_{-10000.0};
    double bound_max_x_{10000.0};
    double bound_max_y_{10000.0};
};

} // namespace markamp::canvas
