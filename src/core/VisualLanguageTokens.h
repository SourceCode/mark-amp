#pragma once

#include <cstdint>

namespace markamp::core
{

// ── V22 Phase 01: Corner Radius Tokens ──────────────────────────────────────
/// Standard corner radius values consumed by all container and control surfaces.
enum class CornerRadiusToken : uint8_t
{
    kNone = 0,   /// Sharp corners (e.g., editor area edges)
    kSm   = 1,   /// 2px — subtle rounding (badges, chips, inline controls)
    kMd   = 2,   /// 4px — standard control rounding (buttons, inputs, cards)
    kLg   = 3,   /// 8px — panel rounding (dialogs, popovers, floating surfaces)
    kXl   = 4,   /// 12px — prominent rounding (modals, large cards)
    kPill = 5,   /// 9999px — fully rounded (tags, pills, toggle tracks)
};

/// Resolve a corner radius token to logical pixels (DPI-independent).
[[nodiscard]] constexpr auto resolve_corner_radius(CornerRadiusToken token) -> int
{
    switch (token)
    {
        case CornerRadiusToken::kNone:
            return 0;
        case CornerRadiusToken::kSm:
            return 2;
        case CornerRadiusToken::kMd:
            return 4;
        case CornerRadiusToken::kLg:
            return 8;
        case CornerRadiusToken::kXl:
            return 12;
        case CornerRadiusToken::kPill:
            return 9999;
    }
    return 0;
}

// ── V22 Phase 01: Border Weight Tokens ──────────────────────────────────────
/// Canonical border widths used by all container and control borders.
enum class BorderWeightToken : uint8_t
{
    kNone   = 0, /// No border
    kThin   = 1, /// 1px — standard borders (control outlines, separators)
    kMedium = 2, /// 2px — emphasized borders (focus rings, active indicators)
    kThick  = 3, /// 3px — heavy borders (dragged elements, selection regions)
};

/// Resolve a border weight token to logical pixels.
[[nodiscard]] constexpr auto resolve_border_weight(BorderWeightToken token) -> int
{
    switch (token)
    {
        case BorderWeightToken::kNone:
            return 0;
        case BorderWeightToken::kThin:
            return 1;
        case BorderWeightToken::kMedium:
            return 2;
        case BorderWeightToken::kThick:
            return 3;
    }
    return 0;
}

// ── V22 Phase 01: Elevation Tokens ──────────────────────────────────────────
/// Elevation levels controlling z-order visual treatment (shadow depth, border prominence).
enum class ElevationToken : uint8_t
{
    kNone    = 0, /// Flat, no shadow (inline content)
    kLow     = 1, /// Subtle shadow (cards, panels)
    kMedium  = 2, /// Standard shadow (popovers, dropdowns)
    kHigh    = 3, /// Prominent shadow (dialogs, modals)
    kOverlay = 4, /// Topmost shadow (drag ghosts, notifications)
};

/// Shadow parameters for a given elevation level.
struct ElevationParams
{
    int offset_x;
    int offset_y;
    int blur_radius;
    int spread_radius;
    uint8_t opacity; /// Alpha value 0–255 for shadow color
};

/// Resolve an elevation token to shadow parameters.
[[nodiscard]] constexpr auto resolve_elevation(ElevationToken token) -> ElevationParams
{
    switch (token)
    {
        case ElevationToken::kNone:
            return {0, 0, 0, 0, 0};
        case ElevationToken::kLow:
            return {0, 1, 3, 0, 20};
        case ElevationToken::kMedium:
            return {0, 2, 8, 0, 30};
        case ElevationToken::kHigh:
            return {0, 4, 16, 0, 40};
        case ElevationToken::kOverlay:
            return {0, 8, 24, 0, 50};
    }
    return {0, 0, 0, 0, 0};
}

// ── V22 Phase 01: Typography Scale Tokens ───────────────────────────────────
/// Canonical type scale slots. Each slot has a fixed point size, weight, and line-height.
enum class TypeScaleToken : uint8_t
{
    kCaption   = 0, /// 10px — captions, timestamps, tertiary metadata
    kBody      = 1, /// 12px — standard UI body text, list items, settings descriptions
    kBodyLarge = 2, /// 13px — primary editor text, code, prominent body
    kSubtitle  = 3, /// 14px — section headers, panel titles, emphasized labels
    kTitle     = 4, /// 16px — dialog titles, major section headers
    kDisplay   = 5, /// 20px — hero text, welcome headings, splash labels
};

/// Resolved typography metrics for a scale token.
struct TypeScaleMetrics
{
    int point_size;
    int line_height_px; /// Canonical line height (px, not CSS multiplier)
    int letter_spacing; /// Additional letter spacing in 1/10 px (0 = default)
    bool is_bold;
};

/// Resolve a type scale token to its metrics.
[[nodiscard]] constexpr auto resolve_type_scale(TypeScaleToken token) -> TypeScaleMetrics
{
    switch (token)
    {
        case TypeScaleToken::kCaption:
            return {10, 14, 2, false};
        case TypeScaleToken::kBody:
            return {12, 18, 0, false};
        case TypeScaleToken::kBodyLarge:
            return {13, 20, 0, false};
        case TypeScaleToken::kSubtitle:
            return {14, 20, 0, true};
        case TypeScaleToken::kTitle:
            return {16, 22, 0, true};
        case TypeScaleToken::kDisplay:
            return {20, 28, -1, true};
    }
    return {12, 18, 0, false};
}

// ── V22 Phase 01: Container Style Composition ───────────────────────────────
/// A container style composite describing the full visual treatment of a panel, card, or surface.
struct ContainerStyle
{
    CornerRadiusToken corner{CornerRadiusToken::kNone};
    BorderWeightToken border{BorderWeightToken::kNone};
    ElevationToken elevation{ElevationToken::kNone};
    TypeScaleToken title_font{TypeScaleToken::kSubtitle};
    TypeScaleToken body_font{TypeScaleToken::kBody};
    int padding_h{0}; /// Horizontal content padding (logical px)
    int padding_v{0}; /// Vertical content padding (logical px)

    /// True if this container has any visible border or shadow.
    [[nodiscard]] constexpr auto has_decoration() const -> bool
    {
        return border != BorderWeightToken::kNone || elevation != ElevationToken::kNone;
    }
};

// ── V22 Phase 01: Icon Metric Tokens ────────────────────────────────────────
/// Canonical icon sizes for optical consistency across all surfaces.
enum class IconMetricToken : uint8_t
{
    kMicro  = 0, /// 12px — inline metadata, status bar micro-icons
    kSmall  = 1, /// 14px — breadcrumb, tree indent, status bar
    kMedium = 2, /// 16px — standard tree/list icon, toolbar action
    kLarge  = 3, /// 20px — activity bar, panel header
    kXLarge = 4, /// 24px — large toolbar, canvas tool palette
    kHero   = 5, /// 32px — empty-state hero, onboarding graphics
};

/// Resolve an icon metric token to logical pixel size.
[[nodiscard]] constexpr auto resolve_icon_metric(IconMetricToken token) -> int
{
    switch (token)
    {
        case IconMetricToken::kMicro:
            return 12;
        case IconMetricToken::kSmall:
            return 14;
        case IconMetricToken::kMedium:
            return 16;
        case IconMetricToken::kLarge:
            return 20;
        case IconMetricToken::kXLarge:
            return 24;
        case IconMetricToken::kHero:
            return 32;
    }
    return 16;
}

/// Icon stroke weight standards for optical alignment.
enum class IconStrokeWeight : uint8_t
{
    kLight   = 0, /// 1.0px stroke — decorative / large icons
    kRegular = 1, /// 1.5px stroke — standard UI icons
    kBold    = 2, /// 2.0px stroke — emphasized / action icons
};

/// Resolve an icon stroke weight to a stroke width value.
[[nodiscard]] constexpr auto resolve_icon_stroke(IconStrokeWeight weight) -> float
{
    switch (weight)
    {
        case IconStrokeWeight::kLight:
            return 1.0F;
        case IconStrokeWeight::kRegular:
            return 1.5F;
        case IconStrokeWeight::kBold:
            return 2.0F;
    }
    return 1.5F;
}

} // namespace markamp::core
