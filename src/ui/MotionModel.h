#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::ui
{

/// Easing curve type.
enum class EasingCurve : uint8_t
{
    kLinear,
    kEaseIn,
    kEaseOut,
    kEaseInOut,
    kSpring,
};

/// Motion context (what triggers the animation).
enum class MotionContext : uint8_t
{
    kHover,
    kPress,
    kReveal,
    kDismiss,
    kExpand,
    kCollapse,
};

/// A motion token: standardized duration/easing for a context.
struct MotionToken
{
    std::string token_id;
    MotionContext context{MotionContext::kHover};
    int duration_ms{200};
    EasingCurve easing{EasingCurve::kEaseOut};
};

/// Testable model for Motion & Microinteraction Controls (Phase 31).
///
/// Encapsulates:
/// - Motion token registry with context lookup
/// - Reduced-motion mode (disables non-essential animations)
/// - Latency budget enforcement
/// - Duration override for testing
class MotionModel
{
public:
    void set_tokens(std::vector<MotionToken> tokens);
    [[nodiscard]] auto tokens() const -> const std::vector<MotionToken>&;

    /// Get motion token for a given context. Returns default if not found.
    [[nodiscard]] auto token_for(MotionContext context) const -> MotionToken;

    /// Effective duration (0 when reduced motion is on and context is non-essential).
    [[nodiscard]] auto effective_duration(MotionContext context) const -> int;

    // ── Reduced motion ──────────────────────────────────────────────

    void set_reduced_motion(bool enabled);
    [[nodiscard]] auto reduced_motion() const -> bool;

    /// Returns true if the given context is essential (always animates).
    [[nodiscard]] static auto is_essential(MotionContext context) -> bool;

    // ── Latency budget ──────────────────────────────────────────────

    void set_latency_budget_ms(int budget_ms);
    [[nodiscard]] auto latency_budget_ms() const -> int;

    /// Returns true if the token's duration exceeds the budget.
    [[nodiscard]] auto exceeds_budget(MotionContext context) const -> bool;

private:
    std::vector<MotionToken> tokens_;
    bool reduced_motion_{false};
    int latency_budget_ms_{100};
};

} // namespace markamp::ui
