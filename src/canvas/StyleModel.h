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
};

} // namespace markamp::canvas
