#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::canvas
{

/// Hit-test target type priority.
enum class HitTarget : uint8_t
{
    kHandle,    ///< Highest priority (resize/rotate handles)
    kObject,    ///< Normal object
    kContainer, ///< Frame/container background
    kCanvas,    ///< Empty canvas (lowest priority)
};

/// A hit-test candidate.
struct HitCandidate
{
    std::string object_id;
    HitTarget target_type{HitTarget::kCanvas};
    double distance{0.0}; ///< Distance from cursor in pixels

    // ── Round 5 Batch 5 (#41-44) ────────────────────────────────

    /// (#41) Whether the target is a handle.
    [[nodiscard]] auto is_handle() const noexcept -> bool
    {
        return target_type == HitTarget::kHandle;
    }

    /// (#42) Whether the target is an object.
    [[nodiscard]] auto is_object() const noexcept -> bool
    {
        return target_type == HitTarget::kObject;
    }

    /// (#43) Whether the target is empty canvas.
    [[nodiscard]] auto is_canvas() const noexcept -> bool
    {
        return target_type == HitTarget::kCanvas;
    }

    /// (#44) Whether an object ID is set.
    [[nodiscard]] auto has_id() const noexcept -> bool
    {
        return !object_id.empty();
    }
};

/// Testable model for Hit Testing Precision (Phase 70).
///
/// Encapsulates:
/// - Hit-test priority resolution (handle > object > container > canvas)
/// - Pixel tolerance scaling by zoom
/// - Hover prefetch candidates
/// - Latency budget tracking (pointer-down to feedback)
class HitTestModel
{
public:
    // ── Candidates ──────────────────────────────────────────────────

    void set_candidates(std::vector<HitCandidate> candidates);
    [[nodiscard]] auto candidates() const -> const std::vector<HitCandidate>&;

    /// Resolve the best hit target based on priority then distance.
    [[nodiscard]] auto resolve() const -> HitCandidate;

    // ── Tolerance ───────────────────────────────────────────────────

    void set_base_tolerance(double pixels);
    [[nodiscard]] auto tolerance_at_zoom(double zoom) const -> double;

    // ── Prefetch ────────────────────────────────────────────────────

    void set_prefetch_candidates(std::vector<std::string> ids);
    [[nodiscard]] auto prefetch_candidates() const -> const std::vector<std::string>&;

    // ── Latency ─────────────────────────────────────────────────────

    void set_latency_ms(double latency_ms);
    [[nodiscard]] auto latency_ms() const -> double;
    [[nodiscard]] auto is_within_budget(double budget_ms) const -> bool;

private:
    std::vector<HitCandidate> candidates_;
    double base_tolerance_{4.0};
    std::vector<std::string> prefetch_;
    double latency_ms_{0.0};

    // ── Round 5 Batch 5 (#45-46) ────────────────────────────────

    /// (#45) Whether hit candidates exist.
    [[nodiscard]] auto has_candidates() const noexcept -> bool
    {
        return !candidates_.empty();
    }

    /// (#46) Number of hit candidates.
    [[nodiscard]] auto candidate_count() const noexcept -> size_t
    {
        return candidates_.size();
    }
};

} // namespace markamp::canvas
