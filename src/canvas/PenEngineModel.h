// ============================================================================
// File: src/canvas/PenEngineModel.h
// Phase 03 W01: Freehand Pen Engine — smoothing, pressure, replay state
// ============================================================================
#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::canvas
{

/// Stroke smoothing algorithm.
enum class SmoothingAlgorithm : uint8_t
{
    kNone,
    kMovingAverage,
    kCatmullRom,
    kBezier,
};

/// Testable model for the Freehand Pen Engine (Phase 03 W01).
///
/// Encapsulates:
/// - Stroke smoothing settings
/// - Pressure sensitivity
/// - Replay state
class PenEngineModel
{
public:
    // ── Smoothing ───────────────────────────────────────────────────

    void set_smoothing(SmoothingAlgorithm algo);
    [[nodiscard]] auto smoothing() const -> SmoothingAlgorithm;

    void set_smoothing_factor(double factor);
    [[nodiscard]] auto smoothing_factor() const -> double;

    // ── Pressure ────────────────────────────────────────────────────

    void set_pressure_sensitivity(double sensitivity);
    [[nodiscard]] auto pressure_sensitivity() const -> double;

    void set_pressure_enabled(bool enabled);
    [[nodiscard]] auto pressure_enabled() const noexcept -> bool;

    // ── Replay ──────────────────────────────────────────────────────

    void set_replay_active(bool active);
    [[nodiscard]] auto replay_active() const noexcept -> bool;

    // ── Stroke Tracking ─────────────────────────────────────────────

    void increment_stroke_count();
    [[nodiscard]] auto stroke_count() const noexcept -> int;

    /// Human-readable label for smoothing algorithm.
    [[nodiscard]] static auto smoothing_label(SmoothingAlgorithm algo) -> std::string;

private:
    SmoothingAlgorithm smoothing_{SmoothingAlgorithm::kCatmullRom};
    double smoothing_factor_{0.5};
    double pressure_sensitivity_{1.0};
    bool pressure_enabled_{true};
    bool replay_active_{false};
    int stroke_count_{0};
};

} // namespace markamp::canvas
