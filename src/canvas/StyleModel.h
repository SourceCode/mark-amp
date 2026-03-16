#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::canvas
{

/// Stroke dash pattern.
enum class DashPattern : uint8_t
{
    kSolid,
    kDashed,
    kDotted,
    kDashDot,
};

/// Line cap style.
enum class LineCap : uint8_t
{
    kButt,
    kRound,
    kSquare,
};

/// Line join style.
enum class LineJoin : uint8_t
{
    kMiter,
    kRound,
    kBevel,
};

/// Fill type.
enum class FillType : uint8_t
{
    kNone,
    kSolid,
    kLinearGradient,
    kRadialGradient,
};

/// Shadow properties.
struct ShadowStyle
{
    double offset_x{2.0};
    double offset_y{2.0};
    double blur{4.0};
    std::string color{"#00000040"};
    bool enabled{false};

    // ── Round 5 Batch 6 (#51-53) ────────────────────────────────

    /// (#51) Whether shadow is enabled.
    [[nodiscard]] auto is_enabled() const noexcept -> bool
    {
        return enabled;
    }

    /// (#52) Whether blur is applied.
    [[nodiscard]] auto has_blur() const noexcept -> bool
    {
        return blur > 0.0;
    }

    /// (#53) Whether an offset is set.
    [[nodiscard]] auto has_offset() const noexcept -> bool
    {
        return offset_x != 0.0 || offset_y != 0.0;
    }
};

/// Testable model for Stroke/Fill/Border & Effects (Phase 47).
///
/// Encapsulates:
/// - Stroke with dash, cap, join options
/// - Fill type (none/solid/gradient)
/// - Object opacity
/// - Shadow/elevation controls
class StyleModel
{
public:
    // ── Stroke ──────────────────────────────────────────────────────

    void set_stroke_width(double width);
    [[nodiscard]] auto stroke_width() const -> double;

    void set_dash(DashPattern pattern);
    [[nodiscard]] auto dash() const -> DashPattern;

    void set_line_cap(LineCap cap);
    [[nodiscard]] auto line_cap() const -> LineCap;

    void set_line_join(LineJoin join);
    [[nodiscard]] auto line_join() const -> LineJoin;

    // ── Fill ────────────────────────────────────────────────────────

    void set_fill_type(FillType type);
    [[nodiscard]] auto fill_type() const -> FillType;

    void set_gradient_stops(std::vector<std::string> stops);
    [[nodiscard]] auto gradient_stops() const -> const std::vector<std::string>&;

    // ── Opacity ─────────────────────────────────────────────────────

    void set_opacity(double opacity);
    [[nodiscard]] auto opacity() const -> double;

    // ── Shadow ──────────────────────────────────────────────────────

    void set_shadow(ShadowStyle shadow);
    [[nodiscard]] auto shadow() const -> const ShadowStyle&;

private:
    double stroke_width_{2.0};
    DashPattern dash_{DashPattern::kSolid};
    LineCap line_cap_{LineCap::kRound};
    LineJoin line_join_{LineJoin::kRound};
    FillType fill_type_{FillType::kSolid};
    std::vector<std::string> gradient_stops_;
    double opacity_{1.0};
    ShadowStyle shadow_;

    // ── Round 5 Batch 6 (#54-60) ────────────────────────────────

    /// (#54) Whether dash pattern is dashed.
    [[nodiscard]] auto is_dashed() const noexcept -> bool
    {
        return dash_ == DashPattern::kDashed;
    }

    /// (#55) Whether dash pattern is solid.
    [[nodiscard]] auto is_solid_dash() const noexcept -> bool
    {
        return dash_ == DashPattern::kSolid;
    }

    /// (#56) Whether dash pattern is dotted.
    [[nodiscard]] auto is_dotted() const noexcept -> bool
    {
        return dash_ == DashPattern::kDotted;
    }

    /// (#57) Whether fill is present (not none).
    [[nodiscard]] auto has_fill() const noexcept -> bool
    {
        return fill_type_ != FillType::kNone;
    }

    /// (#58) Whether fill is a gradient.
    [[nodiscard]] auto is_gradient() const noexcept -> bool
    {
        return fill_type_ == FillType::kLinearGradient || fill_type_ == FillType::kRadialGradient;
    }

    /// (#59) Whether object is fully opaque.
    [[nodiscard]] auto is_fully_opaque() const noexcept -> bool
    {
        return opacity_ == 1.0;
    }

    /// (#60) Whether shadow is enabled.
    [[nodiscard]] auto has_shadow() const noexcept -> bool
    {
        return shadow_.enabled;
    }
};

} // namespace markamp::canvas
