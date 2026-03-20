// ============================================================================
// File: src/canvas/SmartSpacingModel.h
// Phase 02 W09: Smart Spacing — spacing visualization and even-spacing calculation
// ============================================================================
#pragma once

#include <string>
#include <vector>

namespace markamp::canvas
{

/// Spacing guide descriptor for visualization.
struct SpacingGuide
{
    std::string from_id;
    std::string to_id;
    double spacing_px{0.0};
    bool is_even{false};

    /// Whether this guide has valid endpoints.
    [[nodiscard]] auto is_valid() const noexcept -> bool
    {
        return !from_id.empty() && !to_id.empty();
    }
};

/// State of smart spacing visualization.
enum class SpacingState : uint8_t
{
    kInactive,   ///< No spacing visualization
    kPreviewing, ///< Showing spacing guides during drag
    kApplied,    ///< Even spacing has been applied
};

/// Testable model for Smart Spacing (Phase 02 W09).
///
/// Encapsulates:
/// - Spacing guide collection and visualization
/// - Even-spacing calculation and target
/// - Drag preview state
/// - Tolerance for "close enough" matching
class SmartSpacingModel
{
public:
    // ── Spacing Guides ──────────────────────────────────────────────

    void set_guides(std::vector<SpacingGuide> guides);
    [[nodiscard]] auto guides() const -> const std::vector<SpacingGuide>&;

    // ── Target Spacing ──────────────────────────────────────────────

    void set_target_spacing(double spacing_px);
    [[nodiscard]] auto target_spacing() const -> double;

    // ── Tolerance ───────────────────────────────────────────────────

    void set_tolerance(double tolerance_px);
    [[nodiscard]] auto tolerance() const -> double;

    /// Whether a value is within tolerance of target spacing.
    [[nodiscard]] auto is_within_tolerance(double spacing_px) const -> bool;

    // ── State ───────────────────────────────────────────────────────

    [[nodiscard]] auto state() const -> SpacingState;
    void activate();
    void deactivate();
    void apply();

    /// Convenience queries.
    [[nodiscard]] auto is_active() const noexcept -> bool
    {
        return state_ != SpacingState::kInactive;
    }

    [[nodiscard]] auto is_previewing() const noexcept -> bool
    {
        return state_ == SpacingState::kPreviewing;
    }

    [[nodiscard]] auto guide_count() const noexcept -> int
    {
        return static_cast<int>(guides_.size());
    }

private:
    std::vector<SpacingGuide> guides_;
    double target_spacing_{20.0};
    double tolerance_{2.0};
    SpacingState state_{SpacingState::kInactive};
};

} // namespace markamp::canvas
