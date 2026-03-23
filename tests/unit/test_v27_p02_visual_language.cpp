/// @file test_v27_p02_visual_language.cpp
/// @brief V27 Phase 02: Visual language, density, depth, typography, interaction states.
#include <catch2/catch_test_macros.hpp>
#include "core/V27VisualLanguage.h"
#include "core/V27TypographySystem.h"
#include "core/V27InteractionStates.h"

using namespace markamp::core;

TEST_CASE("V27 P02: Phase and task counts", "[v27][p02]") {
    REQUIRE(v27_phase_count() == 20);
    REQUIRE(v27_task_count() == 60);
}
TEST_CASE("V27 P02: Depth tier count", "[v27][p02]") {
    REQUIRE(v27_depth_tier_count() == 6);
}
TEST_CASE("V27 P02: Depth blur increases with tier", "[v27][p02]") {
    REQUIRE(v27_depth_blur_px(V27DepthTier::kSunken) == 0);
    REQUIRE(v27_depth_blur_px(V27DepthTier::kBase) == 0);
    REQUIRE(v27_depth_blur_px(V27DepthTier::kRaised) < v27_depth_blur_px(V27DepthTier::kElevated));
    REQUIRE(v27_depth_blur_px(V27DepthTier::kElevated) < v27_depth_blur_px(V27DepthTier::kOverlay));
    REQUIRE(v27_depth_blur_px(V27DepthTier::kOverlay) < v27_depth_blur_px(V27DepthTier::kPopover));
}
TEST_CASE("V27 P02: Density profile count", "[v27][p02]") {
    REQUIRE(v27_density_profile_count() == 3);
}
TEST_CASE("V27 P02: Density tokens increase with comfort", "[v27][p02]") {
    auto compact = V27DensityTokens::for_profile(V27DensityProfile::kCompact);
    auto def = V27DensityTokens::for_profile(V27DensityProfile::kDefault);
    auto comfort = V27DensityTokens::for_profile(V27DensityProfile::kComfortable);
    REQUIRE(compact.row_height < def.row_height);
    REQUIRE(def.row_height < comfort.row_height);
}
TEST_CASE("V27 P02: Spacing tokens are valid", "[v27][p02]") {
    REQUIRE(V27SpacingTokens::kPanelPadding > 0);
    REQUIRE(V27SpacingTokens::kDialogPadding > V27SpacingTokens::kPanelPadding);
}
TEST_CASE("V27 P02: Radius tokens progression", "[v27][p02]") {
    REQUIRE(V27RadiusTokens::kNone < V27RadiusTokens::kSubtle);
    REQUIRE(V27RadiusTokens::kSubtle < V27RadiusTokens::kControl);
    REQUIRE(V27RadiusTokens::kControl < V27RadiusTokens::kCard);
    REQUIRE(V27RadiusTokens::kDialog < V27RadiusTokens::kLarge);
    REQUIRE(V27RadiusTokens::kPill == 9999);
}
TEST_CASE("V27 P02: Interaction state count", "[v27][p02]") {
    REQUIRE(v27_interaction_state_count() == 8);
}
TEST_CASE("V27 P02: Interaction visuals for states", "[v27][p02]") {
    auto idle = V27InteractionVisuals::for_state(V27InteractionState::kIdle);
    REQUIRE(idle.opacity == 1.0);
    REQUIRE(idle.timing_ms == 0);
    auto disabled = V27InteractionVisuals::for_state(V27InteractionState::kDisabled);
    REQUIRE(disabled.opacity < 1.0);
}
TEST_CASE("V27 P02: Typography role count", "[v27][p02]") {
    REQUIRE(v27_typography_role_count() == 16);
}
TEST_CASE("V27 P02: Typography mono detection", "[v27][p02]") {
    REQUIRE(v27_type_is_mono(V27TypographyRole::kCodeInline));
    REQUIRE(v27_type_is_mono(V27TypographyRole::kCodeSmall));
    REQUIRE(v27_type_is_mono(V27TypographyRole::kActionShortcut));
    REQUIRE_FALSE(v27_type_is_mono(V27TypographyRole::kShellTitle));
}
TEST_CASE("V27 P02: Typography bold detection", "[v27][p02]") {
    REQUIRE(v27_type_is_bold(V27TypographyRole::kPanelSectionLabel));
    REQUIRE(v27_type_is_bold(V27TypographyRole::kMetadataStrong));
    REQUIRE(v27_type_is_bold(V27TypographyRole::kBadgeCount));
    REQUIRE_FALSE(v27_type_is_bold(V27TypographyRole::kMetadataCaption));
}
TEST_CASE("V27 P02: Hover/pressed state properties", "[v27][p02]") {
    auto hover = v27_state_hover();
    REQUIRE(hover.cursor_pointer);
    REQUIRE(hover.transition_ms > 0);
    auto pressed = v27_state_pressed();
    REQUIRE(pressed.transition_ms < hover.transition_ms);
}
TEST_CASE("V27 P02: Hit target minimums", "[v27][p02]") {
    REQUIRE(V27HitTargets::kMinimum >= 24);
    REQUIRE(V27HitTargets::kStandard >= V27HitTargets::kMinimum);
    REQUIRE(V27HitTargets::kComfortable >= V27HitTargets::kStandard);
}
TEST_CASE("V27 P02: Focus ring config", "[v27][p02]") {
    REQUIRE(V27FocusRingConfig::kThickness > 0);
    REQUIRE(V27FocusRingConfig::kOffset >= 0);
}
