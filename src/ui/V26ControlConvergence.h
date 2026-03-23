/// @file V26ControlConvergence.h
/// @brief V26 Phase 10: Settings, preferences, theme gallery & control styling convergence.
///
/// Defines premium layout rules for preference rows, control styling, theme gallery,
/// and shared form patterns across settings, dialogs, and embedded forms.
#pragma once

#include <cstdint>

namespace markamp::ui
{

// ═══════════════════════════════════════════════════════════════════════════════
// Phase 10: Settings, Preferences, Theme Gallery & Control Styling
// ═══════════════════════════════════════════════════════════════════════════════

/// Premium preference row layout metrics.
struct PreferenceRowMetrics
{
    static constexpr int kRowMinHeight       = 44;
    static constexpr int kRowPaddingH        = 16;
    static constexpr int kRowPaddingV        = 8;
    static constexpr int kLabelFontPt        = 13;
    static constexpr int kHelpFontPt         = 11;
    static constexpr int kLabelHelpGap       = 4;     ///< Gap between label and help text

    // ── Control alignment ────────────────────────────────────────────
    static constexpr int kControlMinW        = 180;
    static constexpr int kControlMaxW        = 300;
    static constexpr int kControlHeight      = 28;
    static constexpr int kToggleHeight       = 20;
    static constexpr int kToggleWidth        = 36;

    // ── Category header ──────────────────────────────────────────────
    static constexpr int kCategoryHeaderH    = 36;
    static constexpr int kCategoryFontPt     = 14;
    static constexpr int kCategoryGap        = 16;    ///< Gap between categories
    static constexpr int kCategoryPaddingL   = 16;    ///< Left indent

    // ── Reset button ─────────────────────────────────────────────────
    static constexpr int kResetButtonSize    = 20;

    // ── Badges ───────────────────────────────────────────────────────
    static constexpr int kBadgeHeight        = 16;
    static constexpr int kBadgePaddingH      = 6;
    static constexpr int kBadgeRadius        = 8;
};

/// Theme gallery card layout metrics.
struct ThemeGalleryCardMetrics
{
    // ── Card sizing ──────────────────────────────────────────────────
    static constexpr int kCardWidth          = 220;
    static constexpr int kCardHeight         = 160;
    static constexpr int kCardRadius         = 8;
    static constexpr int kCardGap            = 12;

    // ── Preview area ─────────────────────────────────────────────────
    static constexpr int kPreviewHeight      = 100;
    static constexpr int kPreviewPadding     = 8;
    static constexpr int kPreviewLineH       = 3;     ///< Simulated code line height
    static constexpr int kPreviewLineGap     = 2;
    static constexpr int kPreviewLineCount   = 8;

    // ── Card footer ──────────────────────────────────────────────────
    static constexpr int kFooterHeight       = 44;
    static constexpr int kFooterPaddingH     = 12;
    static constexpr int kTitleFontPt        = 12;
    static constexpr int kAuthorFontPt       = 10;

    // ── Active state ─────────────────────────────────────────────────
    static constexpr int kActiveBorderW      = 2;
    static constexpr float kHoverScale       = 1.02F;
    static constexpr float kInactiveOpacity  = 0.8F;
};

/// Shared control styling metrics (text inputs, dropdowns, sliders, etc.).
struct ControlStyleMetrics
{
    // ── Text input ───────────────────────────────────────────────────
    static constexpr int kInputHeight        = 28;
    static constexpr int kInputPaddingH      = 8;
    static constexpr int kInputRadius        = 4;
    static constexpr int kInputBorderW       = 1;
    static constexpr int kInputFocusBorderW  = 2;

    // ── Dropdown / Choice ────────────────────────────────────────────
    static constexpr int kDropdownHeight     = 28;
    static constexpr int kDropdownArrowW     = 20;
    static constexpr int kDropdownMaxH       = 200;

    // ── Checkbox ─────────────────────────────────────────────────────
    static constexpr int kCheckboxSize       = 16;
    static constexpr int kCheckboxRadius     = 3;
    static constexpr int kCheckboxLabelGap   = 8;

    // ── Slider ───────────────────────────────────────────────────────
    static constexpr int kSliderTrackH       = 4;
    static constexpr int kSliderThumbSize    = 14;
    static constexpr int kSliderTrackRadius  = 2;

    // ── Button ───────────────────────────────────────────────────────
    static constexpr int kButtonHeight       = 28;
    static constexpr int kButtonPaddingH     = 12;
    static constexpr int kButtonRadius       = 4;
    static constexpr int kButtonMinW         = 60;

    // ── Focus ring ───────────────────────────────────────────────────
    static constexpr int kFocusRingOffset    = 2;
    static constexpr int kFocusRingWidth     = 2;
    static constexpr int kFocusRingRadius    = 6;
};

/// Search/filter bar metrics (used in settings panel, command palette, etc.).
struct SearchBarMetrics
{
    static constexpr int kHeight             = 36;
    static constexpr int kPaddingH           = 12;
    static constexpr int kIconSize           = 16;
    static constexpr int kClearButtonSize    = 16;
    static constexpr int kRadius             = 6;
    static constexpr int kBorderW            = 1;
    static constexpr int kFocusBorderW       = 2;
};

} // namespace markamp::ui
