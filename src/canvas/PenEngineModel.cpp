// ============================================================================
// File: src/canvas/PenEngineModel.cpp
// Phase 03 W01: Pen Engine model implementation
// ============================================================================

#include "canvas/PenEngineModel.h"

namespace markamp::canvas
{

void PenEngineModel::set_smoothing(SmoothingAlgorithm algo)
{
    smoothing_ = algo;
}

auto PenEngineModel::smoothing() const -> SmoothingAlgorithm
{
    return smoothing_;
}

void PenEngineModel::set_smoothing_factor(double factor)
{
    smoothing_factor_ = factor;
}

auto PenEngineModel::smoothing_factor() const -> double
{
    return smoothing_factor_;
}

void PenEngineModel::set_pressure_sensitivity(double sensitivity)
{
    pressure_sensitivity_ = sensitivity;
}

auto PenEngineModel::pressure_sensitivity() const -> double
{
    return pressure_sensitivity_;
}

void PenEngineModel::set_pressure_enabled(bool enabled)
{
    pressure_enabled_ = enabled;
}

auto PenEngineModel::pressure_enabled() const noexcept -> bool
{
    return pressure_enabled_;
}

void PenEngineModel::set_replay_active(bool active)
{
    replay_active_ = active;
}

auto PenEngineModel::replay_active() const noexcept -> bool
{
    return replay_active_;
}

void PenEngineModel::increment_stroke_count()
{
    ++stroke_count_;
}

auto PenEngineModel::stroke_count() const noexcept -> int
{
    return stroke_count_;
}

auto PenEngineModel::smoothing_label(SmoothingAlgorithm algo) -> std::string
{
    switch (algo)
    {
        case SmoothingAlgorithm::kNone: return "none";
        case SmoothingAlgorithm::kMovingAverage: return "moving_average";
        case SmoothingAlgorithm::kCatmullRom: return "catmull_rom";
        case SmoothingAlgorithm::kBezier: return "bezier";
    }
    return "unknown";
}

} // namespace markamp::canvas
