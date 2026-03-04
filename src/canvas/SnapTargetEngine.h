#pragma once

/**
 * @file SnapTargetEngine.h
 * @brief Phase 49 Task 1,3: Snap targets, priority, and smart spacing.
 */

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::canvas
{

/// Snap target type.
enum class SnapTarget : uint8_t
{
    kGrid,
    kObjectEdge,
    kObjectCenter,
    kKeyPoint,
};

/// Priority of a snap candidate.
struct SnapCandidate
{
    SnapTarget type{SnapTarget::kGrid};
    double snap_x{0.0};
    double snap_y{0.0};
    double distance{0.0};
    int priority{0}; ///< Lower = higher priority

    /// Get target type as string.
    [[nodiscard]] auto type_name() const -> std::string;
};

/// Smart spacing hint.
struct SpacingHint
{
    double spacing{0.0};
    int match_count{0};   ///< How many pairs share this spacing
    bool is_equal{false}; ///< True if objects are equally spaced
};

/**
 * @brief Snap target resolution with priority and spacing hints.
 */
class SnapTargetEngine
{
public:
    SnapTargetEngine() = default;

    // ── Configuration ──────────────────────────────────────────────

    /// Set snap tolerance (in pixels).
    void set_tolerance(double tolerance);

    /// Get snap tolerance.
    [[nodiscard]] auto tolerance() const -> double;

    /// Enable/disable a snap target type.
    void set_target_enabled(SnapTarget target, bool enabled);

    /// Check if a snap target type is enabled.
    [[nodiscard]] auto is_target_enabled(SnapTarget target) const -> bool;

    // ── Snap resolution ────────────────────────────────────────────

    /// Add a snap candidate.
    void add_candidate(const SnapCandidate& candidate);

    /// Clear all candidates.
    void clear_candidates();

    /// Get candidate count.
    [[nodiscard]] auto candidate_count() const -> int;

    /// Resolve the best snap candidate within tolerance.
    [[nodiscard]] auto resolve(double pos_x, double pos_y) const -> SnapCandidate;

    /// Check if position is within snap range of any candidate.
    [[nodiscard]] auto has_snap(double pos_x, double pos_y) const -> bool;

    // ── Smart spacing ──────────────────────────────────────────────

    /// Detect equal spacing among a set of positions.
    [[nodiscard]] static auto detect_equal_spacing(const std::vector<double>& positions,
                                                   double spacing_tolerance) -> SpacingHint;

private:
    double tolerance_{8.0};
    bool grid_enabled_{true};
    bool edge_enabled_{true};
    bool center_enabled_{true};
    bool keypoint_enabled_{true};
    std::vector<SnapCandidate> candidates_;
};

} // namespace markamp::canvas
