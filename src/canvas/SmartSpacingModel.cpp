// ============================================================================
// File: src/canvas/SmartSpacingModel.cpp
// Phase 02 W09: Smart Spacing model implementation
// ============================================================================

#include "canvas/SmartSpacingModel.h"

#include <cmath>

namespace markamp::canvas
{

void SmartSpacingModel::set_guides(std::vector<SpacingGuide> guides)
{
    guides_ = std::move(guides);
}

auto SmartSpacingModel::guides() const -> const std::vector<SpacingGuide>&
{
    return guides_;
}

void SmartSpacingModel::set_target_spacing(double spacing_px)
{
    target_spacing_ = spacing_px;
}

auto SmartSpacingModel::target_spacing() const -> double
{
    return target_spacing_;
}

void SmartSpacingModel::set_tolerance(double tolerance_px)
{
    tolerance_ = tolerance_px;
}

auto SmartSpacingModel::tolerance() const -> double
{
    return tolerance_;
}

auto SmartSpacingModel::is_within_tolerance(double spacing_px) const -> bool
{
    return std::abs(spacing_px - target_spacing_) <= tolerance_;
}

auto SmartSpacingModel::state() const -> SpacingState
{
    return state_;
}

void SmartSpacingModel::activate()
{
    state_ = SpacingState::kPreviewing;
}

void SmartSpacingModel::deactivate()
{
    state_ = SpacingState::kInactive;
    guides_.clear();
}

void SmartSpacingModel::apply()
{
    state_ = SpacingState::kApplied;
}

} // namespace markamp::canvas
