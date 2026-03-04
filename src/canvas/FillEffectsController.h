#pragma once

/**
 * @file FillEffectsController.h
 * @brief Phase 47 Task 2-3: Fill types, shadow/elevation, and opacity.
 */

#include <cstdint>
#include <string>

namespace markamp::canvas
{

/// Fill type.
enum class FillType : uint8_t
{
    kNone,
    kSolid,
    kLinearGradient,
    kRadialGradient,
};

/// Gradient stop.
struct GradientStop
{
    double position{0.0}; ///< 0.0-1.0
    uint32_t color_rgba{0x000000FF};
};

/// Fill specification.
struct FillSpec
{
    FillType type{FillType::kSolid};
    uint32_t solid_color{0xFFFFFFFF};
    GradientStop gradient_start{0.0, 0xFFFFFFFF};
    GradientStop gradient_end{1.0, 0x000000FF};
    double angle{0.0}; ///< Gradient angle in degrees

    /// Get type as string.
    [[nodiscard]] auto type_name() const -> std::string;
};

/// Shadow/elevation specification.
struct ShadowSpec
{
    bool enabled{false};
    double offset_x{2.0};
    double offset_y{2.0};
    double blur{4.0};
    double spread{0.0};
    uint32_t color_rgba{0x00000040}; ///< Semi-transparent black
};

/**
 * @brief Manages fill, shadow, and opacity for canvas objects.
 */
class FillEffectsController
{
public:
    FillEffectsController() = default;

    // ── Fill ───────────────────────────────────────────────────────

    /// Set fill specification.
    void set_fill(const FillSpec& fill);

    /// Get fill specification.
    [[nodiscard]] auto fill() const -> const FillSpec&;

    // ── Shadow ─────────────────────────────────────────────────────

    /// Set shadow specification.
    void set_shadow(const ShadowSpec& shadow);

    /// Get shadow specification.
    [[nodiscard]] auto shadow() const -> const ShadowSpec&;

    /// Enable/disable shadow.
    void set_shadow_enabled(bool enabled);

    /// Check if shadow is enabled.
    [[nodiscard]] auto shadow_enabled() const -> bool;

    // ── Opacity ────────────────────────────────────────────────────

    /// Set opacity (0.0-1.0).
    void set_opacity(double opacity);

    /// Get opacity.
    [[nodiscard]] auto opacity() const -> double;

    // ── Serialization helpers ──────────────────────────────────────

    /// Check if fill is visible (not none and has opacity).
    [[nodiscard]] auto is_fill_visible() const -> bool;

    /// Check if any effect is active.
    [[nodiscard]] auto has_effects() const -> bool;

private:
    FillSpec fill_;
    ShadowSpec shadow_;
    double opacity_{1.0};
};

} // namespace markamp::canvas
