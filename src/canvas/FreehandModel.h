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

    // ── Round 5 Batch 10 (#91) ─────────────────────────────────

    /// (#91) Whether pressure varies from default (non-full).
    [[nodiscard]] auto has_pressure() const noexcept -> bool
    {
        return pressure < 1.0;
    }
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

    // ── Round 5 Batch 10 (#92-96) ───────────────────────────────

    /// (#92) Whether a stroke is being recorded.
    [[nodiscard]] auto is_recording() const noexcept -> bool
    {
        return recording_;
    }

    /// (#93) Whether preset is pen.
    [[nodiscard]] auto is_pen() const noexcept -> bool
    {
        return preset_ == StrokePreset::kPen;
    }

    /// (#94) Whether preset is highlighter.
    [[nodiscard]] auto is_highlighter() const noexcept -> bool
    {
        return preset_ == StrokePreset::kHighlighter;
    }

    /// (#95) Whether eraser mode is object.
    [[nodiscard]] auto is_object_eraser() const noexcept -> bool
    {
        return eraser_mode_ == EraserMode::kObject;
    }

    /// (#96) Whether points have been recorded.
    [[nodiscard]] auto has_points() const noexcept -> bool
    {
        return !points_.empty();
    }
};

} // namespace markamp::canvas
