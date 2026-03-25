/// @file test_v26_p01_visual_system.cpp
/// @brief V26 Phase 01: Visual system convergence and premium standards tests.
#include <catch2/catch_test_macros.hpp>
#include "core/V26PremiumTokens.h"
#include "core/ThemeTokens.h"
#include "core/VisualLanguageTokens.h"
#include "ui/UILayoutTokens.h"
#include "ui/TypographyScale.h"
#include "ui/SpacingGrid.h"
#include "ui/LayoutMetrics.h"

using namespace markamp::core;
using namespace markamp::ui;

// ── V26-P01-T01: Layout, Spacing, Radius Token Tests ─────────────────────

TEST_CASE("V26 P01: Phase and task counts are correct", "[v26][p01]")
{
    REQUIRE(v26_phase_count() == 20);
    REQUIRE(v26_task_count() == 60);
}

TEST_CASE("V26 P01: Surface tier count", "[v26][p01]")
{
    REQUIRE(surface_tier_count() == 6);
}

TEST_CASE("V26 P01: Divider grammar resolve returns valid thickness", "[v26][p01]")
{
    REQUIRE(resolve_divider_thickness(DividerRole::kSectionSeparator) == 1);
    REQUIRE(resolve_divider_thickness(DividerRole::kSurfaceBorder) == 1);
    REQUIRE(resolve_divider_thickness(DividerRole::kSubtleRule) == 1);
}

TEST_CASE("V26 P01: Divider grammar padding follows section hierarchy", "[v26][p01]")
{
    REQUIRE(resolve_divider_padding(DividerRole::kSectionSeparator) > resolve_divider_padding(DividerRole::kGroupDivider));
    REQUIRE(resolve_divider_padding(DividerRole::kGroupDivider) >= resolve_divider_padding(DividerRole::kSubtleRule));
    REQUIRE(resolve_divider_padding(DividerRole::kSurfaceBorder) == 0);
}

TEST_CASE("V26 P01: Divider role count", "[v26][p01]")
{
    REQUIRE(divider_role_count() == 5);
}

TEST_CASE("V26 P01: Radius roles resolve to expected values", "[v26][p01]")
{
    REQUIRE(resolve_radius_role(RadiusRole::kControl) == 4);
    REQUIRE(resolve_radius_role(RadiusRole::kCard) == 6);
    REQUIRE(resolve_radius_role(RadiusRole::kPanel) == 8);
    REQUIRE(resolve_radius_role(RadiusRole::kDialog) == 8);
    REQUIRE(resolve_radius_role(RadiusRole::kBadge) == 9999);
    REQUIRE(resolve_radius_role(RadiusRole::kInlineControl) == 2);
    REQUIRE(radius_role_count() == 7);
}

TEST_CASE("V26 P01: SpacingScale 4px base unit progression", "[v26][p01]")
{
    REQUIRE(SpacingScale::kUnit == 4);
    REQUIRE(SpacingScale::kXs == 4);
    REQUIRE(SpacingScale::kSm == 8);
    REQUIRE(SpacingScale::kMd == 12);
    REQUIRE(SpacingScale::kLg == 16);
    REQUIRE(SpacingScale::kXl == 24);
    REQUIRE(SpacingScale::kXxl == 32);
    REQUIRE(SpacingScale::kXxxl == 48);
}

TEST_CASE("V26 P01: SpacingGrid resolve matches SpacingScale", "[v26][p01]")
{
    REQUIRE(SpacingGrid::resolve(SpacingToken::kXs) == SpacingScale::kXs);
    REQUIRE(SpacingGrid::resolve(SpacingToken::kSm) == SpacingScale::kSm);
    REQUIRE(SpacingGrid::resolve(SpacingToken::kLg) == SpacingScale::kLg);
    REQUIRE(SpacingGrid::resolve(SpacingToken::kXxxl) == SpacingScale::kXxxl);
}

TEST_CASE("V26 P01: Panel spacing tokens are consistent", "[v26][p01]")
{
    REQUIRE(PanelSpacingTokens::kHeaderPaddingH == 12);
    REQUIRE(PanelSpacingTokens::kContentPaddingH == 12);
    REQUIRE(PanelSpacingTokens::kSectionGap >= PanelSpacingTokens::kGroupGap);
    REQUIRE(PanelSpacingTokens::kCardGap > 0);
}

TEST_CASE("V26 P01: Control metric tokens are consistent", "[v26][p01]")
{
    REQUIRE(ControlMetricTokens::kInputHeight >= ControlMetricTokens::kInputHeightSmall);
    REQUIRE(ControlMetricTokens::kButtonHeight >= ControlMetricTokens::kButtonHeightSm);
    REQUIRE(ControlMetricTokens::kToggleWidth > ControlMetricTokens::kToggleHeight);
}

TEST_CASE("V26 P01: Dialog tokens define valid constraints", "[v26][p01]")
{
    REQUIRE(DialogTokens::kMinWidth < DialogTokens::kMaxWidth);
    REQUIRE(DialogTokens::kBorderRadius > 0);
    REQUIRE(DialogTokens::kBackdropOpacity <= 255);
}

TEST_CASE("V26 P01: Panel header tokens are valid", "[v26][p01]")
{
    REQUIRE(PanelHeaderTokens::kHeight > 0);
    REQUIRE(PanelHeaderTokens::kActionSize < PanelHeaderTokens::kHeight);
    REQUIRE(PanelHeaderTokens::kDividerHeight == 1);
}

TEST_CASE("V26 P01: Settings row tokens define valid layout", "[v26][p01]")
{
    REQUIRE(SettingsRowTokens::kRowMinHeight > 0);
    REQUIRE(SettingsRowTokens::kCategoryGap > SettingsRowTokens::kGroupGap);
    REQUIRE(SettingsRowTokens::kLabelWidth > 0);
}

TEST_CASE("V26 P01: UI radius tokens alias correct values", "[v26][p01]")
{
    REQUIRE(RadiusTokens::kNone == 0);
    REQUIRE(RadiusTokens::kMd == 4);
    REQUIRE(RadiusTokens::kXl == 8);
    REQUIRE(RadiusTokens::kPill == 9999);
}

// ── V26-P01-T02: Typography Hierarchy Tests ───────────────────────────────

TEST_CASE("V26 P01: Typography role count", "[v26][p01]")
{
    REQUIRE(typography_role_count() == 12);
}

TEST_CASE("V26 P01: Typography roles resolve to decreasing font sizes", "[v26][p01]")
{
    REQUIRE(resolve_typography_role_pt(TypographyRole::kPageTitle) > resolve_typography_role_pt(TypographyRole::kSectionTitle));
    REQUIRE(resolve_typography_role_pt(TypographyRole::kSectionTitle) > resolve_typography_role_pt(TypographyRole::kSubsectionTitle));
    REQUIRE(resolve_typography_role_pt(TypographyRole::kSubsectionTitle) >= resolve_typography_role_pt(TypographyRole::kRowLabel));
    REQUIRE(resolve_typography_role_pt(TypographyRole::kRowLabel) > resolve_typography_role_pt(TypographyRole::kCaption));
}

TEST_CASE("V26 P01: Typography role line heights are proportional to point sizes", "[v26][p01]")
{
    REQUIRE(resolve_typography_role_lh(TypographyRole::kPageTitle) >= resolve_typography_role_pt(TypographyRole::kPageTitle));
    REQUIRE(resolve_typography_role_lh(TypographyRole::kCaption) >= resolve_typography_role_pt(TypographyRole::kCaption));
}

TEST_CASE("V26 P01: Typography heading roles are bold", "[v26][p01]")
{
    REQUIRE(typography_role_is_bold(TypographyRole::kPageTitle));
    REQUIRE(typography_role_is_bold(TypographyRole::kSectionTitle));
    REQUIRE(typography_role_is_bold(TypographyRole::kSubsectionTitle));
    REQUIRE_FALSE(typography_role_is_bold(TypographyRole::kRowLabel));
    REQUIRE_FALSE(typography_role_is_bold(TypographyRole::kRowDescription));
    REQUIRE_FALSE(typography_role_is_bold(TypographyRole::kCaption));
}

TEST_CASE("V26 P01: Typography mono roles are correctly identified", "[v26][p01]")
{
    REQUIRE(typography_role_is_mono(TypographyRole::kMonoCode));
    REQUIRE(typography_role_is_mono(TypographyRole::kMonoCodeSmall));
    REQUIRE_FALSE(typography_role_is_mono(TypographyRole::kRowLabel));
    REQUIRE_FALSE(typography_role_is_mono(TypographyRole::kPageTitle));
}

TEST_CASE("V26 P01: TypeSlot V26 additions exist", "[v26][p01]")
{
    // Verify new V26 TypeSlot values are valid enum members
    [[maybe_unused]] auto rd = TypeSlot::kRowDescription;
    [[maybe_unused]] auto bl = TypeSlot::kBadgeLabel;
    [[maybe_unused]] auto bt = TypeSlot::kButtonLabel;
    [[maybe_unused]] auto et = TypeSlot::kEmptyStateTitle;
    [[maybe_unused]] auto eb = TypeSlot::kEmptyStateBody;
    REQUIRE(true); // compilation success
}

TEST_CASE("V26 P01: TypeSlot mono check helper", "[v26][p01]")
{
    REQUIRE(type_slot_is_mono(TypeSlot::kMono));
    REQUIRE(type_slot_is_mono(TypeSlot::kMonoSmall));
    REQUIRE_FALSE(type_slot_is_mono(TypeSlot::kBody));
    REQUIRE_FALSE(type_slot_is_mono(TypeSlot::kButtonLabel));
}

// ── V26-P01-T03: Interaction State & Theme Token Tests ────────────────────

TEST_CASE("V26 P01: Interaction state count", "[v26][p01]")
{
    REQUIRE(interaction_state_count() == 8);
}

TEST_CASE("V26 P01: Feedback severity count", "[v26][p01]")
{
    REQUIRE(feedback_severity_count() == 4);
}

TEST_CASE("V26 P01: Motion tier count and progression", "[v26][p01]")
{
    REQUIRE(motion_tier_count() == 5);
    REQUIRE(resolve_motion_tier_ms(MotionTier::kInstant) == 0);
    REQUIRE(resolve_motion_tier_ms(MotionTier::kMicro) < resolve_motion_tier_ms(MotionTier::kFast));
    REQUIRE(resolve_motion_tier_ms(MotionTier::kFast) < resolve_motion_tier_ms(MotionTier::kStandard));
    REQUIRE(resolve_motion_tier_ms(MotionTier::kStandard) < resolve_motion_tier_ms(MotionTier::kGentle));
}

TEST_CASE("V26 P01: ThemeColorToken V26 additions compile", "[v26][p01]")
{
    // Verify V26 surface tier tokens exist
    [[maybe_unused]] auto st = ThemeColorToken::SurfaceShellBg;
    [[maybe_unused]] auto sw = ThemeColorToken::SurfaceWorkBg;
    [[maybe_unused]] auto se = ThemeColorToken::SurfaceElevatedBg;
    [[maybe_unused]] auto so = ThemeColorToken::SurfaceOverlayBg;
    [[maybe_unused]] auto ss = ThemeColorToken::SurfaceSunkenBg;
    [[maybe_unused]] auto sc = ThemeColorToken::SurfaceCanvasBg;
    REQUIRE(true);
}

TEST_CASE("V26 P01: ThemeColorToken divider grammar tokens compile", "[v26][p01]")
{
    [[maybe_unused]] auto ds = ThemeColorToken::DividerSectionColor;
    [[maybe_unused]] auto dg = ThemeColorToken::DividerGroupColor;
    [[maybe_unused]] auto di = ThemeColorToken::DividerInlineColor;
    [[maybe_unused]] auto db = ThemeColorToken::DividerSurfaceBorderColor;
    [[maybe_unused]] auto dr = ThemeColorToken::DividerSubtleRuleColor;
    REQUIRE(true);
}

TEST_CASE("V26 P01: ThemeColorToken interaction state tokens compile", "[v26][p01]")
{
    [[maybe_unused]] auto ho = ThemeColorToken::StateHoverOverlay;
    [[maybe_unused]] auto pr = ThemeColorToken::StatePressedOverlay;
    [[maybe_unused]] auto fr = ThemeColorToken::StateFocusRing;
    [[maybe_unused]] auto sb = ThemeColorToken::StateSelectedBg;
    [[maybe_unused]] auto sf = ThemeColorToken::StateSelectedFg;
    [[maybe_unused]] auto df = ThemeColorToken::StateDisabledFg;
    [[maybe_unused]] auto db = ThemeColorToken::StateDisabledBg;
    [[maybe_unused]] auto dg = ThemeColorToken::StateDragGhost;
    [[maybe_unused]] auto dt = ThemeColorToken::StateDropTargetBorder;
    REQUIRE(true);
}

TEST_CASE("V26 P01: ThemeColorToken feedback tokens compile", "[v26][p01]")
{
    [[maybe_unused]] auto t1 = ThemeColorToken::FeedbackInfoFg;
    [[maybe_unused]] auto t2 = ThemeColorToken::FeedbackInfoBorder;
    [[maybe_unused]] auto t3 = ThemeColorToken::FeedbackSuccessFg;
    [[maybe_unused]] auto t4 = ThemeColorToken::FeedbackSuccessBorder;
    [[maybe_unused]] auto t5 = ThemeColorToken::FeedbackWarningFg;
    [[maybe_unused]] auto t6 = ThemeColorToken::FeedbackWarningBorder;
    [[maybe_unused]] auto t7 = ThemeColorToken::FeedbackErrorFg;
    [[maybe_unused]] auto t8 = ThemeColorToken::FeedbackErrorBorder;
    REQUIRE(true);
}

TEST_CASE("V26 P01: ThemeColorToken canvas surface tokens compile", "[v26][p01]")
{
    [[maybe_unused]] auto c1 = ThemeColorToken::CanvasGridColor;
    [[maybe_unused]] auto c2 = ThemeColorToken::CanvasSelectionHandleBg;
    [[maybe_unused]] auto c3 = ThemeColorToken::CanvasSelectionHandleBorder;
    [[maybe_unused]] auto c4 = ThemeColorToken::CanvasGuideColor;
    [[maybe_unused]] auto c5 = ThemeColorToken::CanvasInlineEditBg;
    [[maybe_unused]] auto c6 = ThemeColorToken::CanvasInlineEditBorder;
    REQUIRE(true);
}

TEST_CASE("V26 P01: ThemeFontToken V26 additions compile", "[v26][p01]")
{
    [[maybe_unused]] auto f1 = ThemeFontToken::UIRowDescription;
    [[maybe_unused]] auto f2 = ThemeFontToken::UIBadgeLabel;
    [[maybe_unused]] auto f3 = ThemeFontToken::UIButtonLabel;
    [[maybe_unused]] auto f4 = ThemeFontToken::UIEmptyStateTitle;
    REQUIRE(true);
}

TEST_CASE("V26 P01: kColorTokenCount includes V26 additions", "[v26][p01]")
{
    REQUIRE(kColorTokenCount > static_cast<std::size_t>(ThemeColorToken::TooltipBorder));
    REQUIRE(kColorTokenCount == static_cast<std::size_t>(ThemeColorToken::CanvasInlineEditBorder) + 1);
}

TEST_CASE("V26 P01: LayoutMetrics singleton is accessible", "[v26][p01]")
{
    auto& m = LayoutMetrics::get();
    REQUIRE(m.row_height() > 0);
    REQUIRE(m.tab_height() > 0);
    REQUIRE(m.toolbar_height() > 0);
    REQUIRE(m.status_bar_height() > 0);
}

TEST_CASE("V26 P01: LayoutMetrics V26 extended accessors", "[v26][p01]")
{
    auto& m = LayoutMetrics::get();
    REQUIRE(m.dialog_padding_h() > 0);
    REQUIRE(m.dialog_padding_v() > 0);
    REQUIRE(m.panel_header_height_v26() > 0);
    REQUIRE(m.settings_category_gap() > 0);
    REQUIRE(m.settings_group_gap() > 0);
    REQUIRE(m.settings_row_min_height() > 0);
}

TEST_CASE("V26 P01: V22 VisualLanguage tokens still resolve correctly", "[v26][p01]")
{
    REQUIRE(resolve_corner_radius(CornerRadiusToken::kMd) == 4);
    REQUIRE(resolve_border_weight(BorderWeightToken::kThin) == 1);
    REQUIRE(resolve_elevation(ElevationToken::kHigh).blur_radius > 0);
    REQUIRE(resolve_type_scale(TypeScaleToken::kBody).point_size == 12);
    REQUIRE(resolve_icon_metric(IconMetricToken::kMedium) == 16);
}
