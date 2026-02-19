#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::canvas
{

/// Stroke style preset type.
enum class StrokePreset : uint8_t
{
    kPen,
    kMarker,
    kHighlighter,
    kCalligraphy,
};

/// Eraser mode.
enum class EraserMode : uint8_t
{
    kObject,  ///< Erase entire object
    kSegment, ///< Erase stroke segment
};

/// A point in a freehand stroke.
struct StrokePoint
{
    double x{0.0};
    double y{0.0};
    double pressure{1.0};  ///< 0.0–1.0, default full pressure
    double timestamp{0.0}; ///< ms since stroke start
};

/// Testable model for Freehand Drawing Engine (Phase 42).
///
/// Encapsulates:
/// - Stroke point collection with pressure channel
/// - Speed-adaptive smoothing
/// - Eraser modes (object/segment)
/// - Stroke style presets (pen/marker/highlighter)
/// - Stroke width from pressure
class FreehandModel
{
public:
    // ── Stroke recording ────────────────────────────────────────────

    void begin_stroke();
    void add_point(StrokePoint point);
    void end_stroke();
    [[nodiscard]] auto points() const -> const std::vector<StrokePoint>&;
    [[nodiscard]] auto point_count() const -> int;

    // ── Smoothing ───────────────────────────────────────────────────

    void set_smoothing(double factor); ///< 0.0 = none, 1.0 = max
    [[nodiscard]] auto smoothing() const -> double;

    /// Smoothed points (applies moving average based on smoothing factor).
    [[nodiscard]] auto smoothed_points() const -> std::vector<StrokePoint>;

    // ── Pressure ────────────────────────────────────────────────────

    [[nodiscard]] auto supports_pressure() const -> bool;
    void set_pressure_support(bool supported);
    [[nodiscard]] auto width_at(int index, double base_width) const -> double;

    // ── Preset ──────────────────────────────────────────────────────

    void set_preset(StrokePreset preset);
    [[nodiscard]] auto preset() const -> StrokePreset;

    // ── Eraser ──────────────────────────────────────────────────────

    void set_eraser_mode(EraserMode mode);
    [[nodiscard]] auto eraser_mode() const -> EraserMode;

private:
    std::vector<StrokePoint> points_;
    double smoothing_{0.5};
    bool pressure_support_{false};
    StrokePreset preset_{StrokePreset::kPen};
    EraserMode eraser_mode_{EraserMode::kObject};
    bool recording_{false};
};

} // namespace markamp::canvas
