/// test_phase13_fx_engine.cpp — Phase 56: FX Engine Quality Bar Tests

#include "core/Config.h"
#include "core/EventBus.h"
#include "core/Events.h"
#include "core/Theme.h"
#include "rendering/FxEngine.h"
#include "rendering/FxMotionPreset.h"
#include "rendering/FxPass.h"
#include "rendering/FxPresetRegistry.h"
#include "rendering/FxSafetyController.h"
#include "rendering/TextFxRenderer.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

using namespace markamp::rendering;
using namespace markamp::core;

// ============================================================================
// FxPass Tests
// ============================================================================

TEST_CASE("FxPass: creation and defaults", "[fx][pass]")
{
    FxPassConfig config;
    FxPass pass("test_blur", FxPassType::kBlur, config);

    REQUIRE(pass.name() == "test_blur");
    REQUIRE(pass.type() == FxPassType::kBlur);
    REQUIRE(pass.is_enabled());
    REQUIRE(pass.intensity() == 1.0F);
    REQUIRE(pass.min_tier() == QualityTier::kCinematic);
    REQUIRE(pass.target() == FxSurfaceTarget::kAll);
}

TEST_CASE("FxPass: enable/disable", "[fx][pass]")
{
    FxPassConfig config;
    FxPass pass("glow", FxPassType::kGlow, config);

    REQUIRE(pass.is_enabled());
    pass.set_enabled(false);
    REQUIRE_FALSE(pass.is_enabled());
    REQUIRE_FALSE(pass.execute());
}

TEST_CASE("FxPass: quality tier gating", "[fx][pass]")
{
    FxPassConfig config;
    config.min_tier = QualityTier::kBalanced;
    FxPass pass("shadow", FxPassType::kShadow, config);

    REQUIRE(pass.is_active_at(QualityTier::kCinematic));
    REQUIRE(pass.is_active_at(QualityTier::kBalanced));
    REQUIRE_FALSE(pass.is_active_at(QualityTier::kEfficient));
    REQUIRE_FALSE(pass.is_active_at(QualityTier::kMinimal));
}

TEST_CASE("FxPass: named parameters", "[fx][pass]")
{
    FxPassConfig config;
    FxPass pass("blur", FxPassType::kBlur, config);

    pass.set_param("radius", 5.0F);
    pass.set_param("sigma", 1.5F);

    REQUIRE(pass.get_param("radius") == 5.0F);
    REQUIRE(pass.get_param("sigma") == 1.5F);
    REQUIRE(pass.get_param("missing") == 0.0F);
}

TEST_CASE("FxPass: type_name returns display string", "[fx][pass]")
{
    REQUIRE(FxPass::type_name(FxPassType::kBlur) == "Blur");
    REQUIRE(FxPass::type_name(FxPassType::kGlow) == "Glow");
    REQUIRE(FxPass::type_name(FxPassType::kShadow) == "Shadow");
    REQUIRE(FxPass::type_name(FxPassType::kBloom) == "Bloom");
    REQUIRE(FxPass::type_name(FxPassType::kChromaShift) == "Chroma Shift");
    REQUIRE(FxPass::type_name(FxPassType::kFrostedGlass) == "Frosted Glass");
}

// ============================================================================
// FxEngine Tests
// ============================================================================

TEST_CASE("FxEngine: pipeline execution", "[fx][engine]")
{
    EventBus bus;
    FxEngine engine(bus);

    FxPassConfig blur_config;
    blur_config.min_tier = QualityTier::kCinematic;
    engine.add_pass("blur", FxPassType::kBlur, blur_config);

    FxPassConfig glow_config;
    glow_config.min_tier = QualityTier::kCinematic;
    engine.add_pass("glow", FxPassType::kGlow, glow_config);

    engine.set_quality_tier(QualityTier::kCinematic);
    auto result = engine.execute_pipeline();

    REQUIRE(result.passes_executed == 2);
    REQUIRE(result.passes_skipped == 0);
    REQUIRE(result.master_enabled);
}

TEST_CASE("FxEngine: quality tier gating", "[fx][engine]")
{
    EventBus bus;
    FxEngine engine(bus);

    FxPassConfig cinematic_config;
    cinematic_config.min_tier = QualityTier::kCinematic;
    engine.add_pass("bloom", FxPassType::kBloom, cinematic_config);

    FxPassConfig balanced_config;
    balanced_config.min_tier = QualityTier::kBalanced;
    engine.add_pass("shadow", FxPassType::kShadow, balanced_config);

    FxPassConfig efficient_config;
    efficient_config.min_tier = QualityTier::kEfficient;
    engine.add_pass("glow", FxPassType::kGlow, efficient_config);

    engine.set_quality_tier(QualityTier::kBalanced);
    auto result = engine.execute_pipeline();

    REQUIRE(result.passes_executed == 2); // bloom + shadow
    REQUIRE(result.passes_skipped == 1);  // glow at efficient
}

TEST_CASE("FxEngine: master toggle", "[fx][engine]")
{
    EventBus bus;
    FxEngine engine(bus);

    engine.add_pass("blur", FxPassType::kBlur, FxPassConfig{});
    engine.set_master_enabled(false);

    REQUIRE_FALSE(engine.is_master_enabled());
    REQUIRE(engine.active_pass_count() == 0);

    auto result = engine.execute_pipeline();
    REQUIRE(result.passes_executed == 0);
    REQUIRE(result.passes_skipped == 1);
}

TEST_CASE("FxEngine: pass management", "[fx][engine]")
{
    EventBus bus;
    FxEngine engine(bus);

    engine.add_pass("a", FxPassType::kBlur, FxPassConfig{});
    engine.add_pass("b", FxPassType::kGlow, FxPassConfig{});
    engine.add_pass("c", FxPassType::kShadow, FxPassConfig{});

    REQUIRE(engine.pass_count() == 3);
    REQUIRE(engine.has_pass_type(FxPassType::kBlur));
    REQUIRE_FALSE(engine.has_pass_type(FxPassType::kBloom));

    REQUIRE(engine.remove_pass("b"));
    REQUIRE(engine.pass_count() == 2);
    REQUIRE_FALSE(engine.remove_pass("nonexistent"));

    auto names = engine.pass_names();
    REQUIRE(names.size() == 2);
    REQUIRE(names[0] == "a");
    REQUIRE(names[1] == "c");
}

TEST_CASE("FxEngine: reorder pass", "[fx][engine]")
{
    EventBus bus;
    FxEngine engine(bus);

    engine.add_pass("a", FxPassType::kBlur, FxPassConfig{});
    engine.add_pass("b", FxPassType::kGlow, FxPassConfig{});
    engine.add_pass("c", FxPassType::kShadow, FxPassConfig{});

    REQUIRE(engine.reorder_pass("c", 0));
    auto names = engine.pass_names();
    REQUIRE(names[0] == "c");
    REQUIRE(names[1] == "a");
    REQUIRE(names[2] == "b");
}

TEST_CASE("FxEngine: execute for surface", "[fx][engine]")
{
    EventBus bus;
    FxEngine engine(bus);

    FxPassConfig window_config;
    window_config.target = FxSurfaceTarget::kWindow;
    engine.add_pass("edge_glow", FxPassType::kGlow, window_config);

    FxPassConfig text_config;
    text_config.target = FxSurfaceTarget::kEditorText;
    engine.add_pass("text_shadow", FxPassType::kShadow, text_config);

    engine.set_quality_tier(QualityTier::kCinematic);
    auto result = engine.execute_for_surface(FxSurfaceTarget::kWindow);
    REQUIRE(result.passes_executed == 1);
    REQUIRE(result.passes_skipped == 1);
}

TEST_CASE("FxEngine: tier_name", "[fx][engine]")
{
    REQUIRE(FxEngine::tier_name(QualityTier::kCinematic) == "Cinematic");
    REQUIRE(FxEngine::tier_name(QualityTier::kBalanced) == "Balanced");
    REQUIRE(FxEngine::tier_name(QualityTier::kEfficient) == "Efficient");
    REQUIRE(FxEngine::tier_name(QualityTier::kMinimal) == "Minimal");
}

// ============================================================================
// WindowEffects Token Extension Tests
// ============================================================================

TEST_CASE("Theme: FxSettings defaults", "[fx][theme]")
{
    Theme theme;

    REQUIRE_FALSE(theme.fx_settings.master_enabled);
    REQUIRE_FALSE(theme.fx_settings.reduced_motion);
    REQUIRE_FALSE(theme.fx_settings.low_power_mode);
    REQUIRE_FALSE(theme.fx_settings.text_safety_mode);
    REQUIRE(theme.fx_settings.high_contrast_guard);
    REQUIRE(theme.fx_settings.max_frame_time_budget_ms == 16.0F);
}

TEST_CASE("Theme: TransitionEffects defaults", "[fx][theme]")
{
    TransitionEffects effects;

    REQUIRE(effects.wobbly_stiffness == 0.5F);
    REQUIRE(effects.wobbly_damping == 0.7F);
    REQUIRE(effects.glide_duration_ms == 250.0F);
    REQUIRE(effects.magic_lamp_bend == 0.4F);
    REQUIRE(effects.cube_perspective == 0.8F);
}

TEST_CASE("Theme: TextEffects defaults", "[fx][theme]")
{
    TextEffects effects;

    REQUIRE_FALSE(effects.stroke_enabled);
    REQUIRE_FALSE(effects.shadow_enabled);
    REQUIRE_FALSE(effects.outer_glow_enabled);
    REQUIRE_FALSE(effects.gradient_fill_enabled);
    REQUIRE_FALSE(effects.bloom_enabled);
    REQUIRE(effects.subpixel_hinting);
    REQUIRE(effects.readability_min_contrast == 4.5F);
    REQUIRE(effects.readability_halo_guard);
}

TEST_CASE("Theme: UiElementEffects defaults", "[fx][theme]")
{
    UiElementEffects effects;

    REQUIRE(effects.hover_glow_radius == 4.0F);
    REQUIRE(effects.active_pill_bloom == 0.3F);
    REQUIRE(effects.button_press_depth == 1.0F);
    REQUIRE(effects.focus_ring_thickness == 2.0F);
    REQUIRE_FALSE(effects.focus_ring_pulse);
    REQUIRE(effects.panel_shadow_blur == 8.0F);
}

TEST_CASE("Theme: EditorEffects defaults", "[fx][theme]")
{
    EditorEffects effects;

    REQUIRE_FALSE(effects.caret_glow_enabled);
    REQUIRE_FALSE(effects.selection_shimmer);
    REQUIRE_FALSE(effects.active_line_glow);
    REQUIRE_FALSE(effects.diagnostic_pulse);
}

TEST_CASE("Theme: CanvasEffects defaults", "[fx][theme]")
{
    CanvasEffects effects;

    REQUIRE(effects.object_shadow_blur == 6.0F);
    REQUIRE(effects.object_shadow_elevation == 2.0F);
    REQUIRE(effects.selection_glow_width == 2.0F);
    REQUIRE_FALSE(effects.selection_glow_pulse);
    REQUIRE_FALSE(effects.sticky_note_lighting);
}

// ============================================================================
// FxMotionPreset Tests
// ============================================================================

TEST_CASE("FxMotionPreset: built-in presets", "[fx][motion]")
{
    auto compiz = FxMotionPreset::builtin(MotionPresetId::kCompizClassic);
    REQUIRE(compiz.name == "Compiz Classic");
    REQUIRE(compiz.motion_intensity == 0.9F);
    REQUIRE_FALSE(compiz.text_distortion_allowed);

    auto beryl = FxMotionPreset::builtin(MotionPresetId::kBerylNeon);
    REQUIRE(beryl.name == "Beryl Neon");
    REQUIRE(beryl.glow_intensity == 0.9F);
    REQUIRE(beryl.chroma_intensity == 0.3F);

    auto studio = FxMotionPreset::builtin(MotionPresetId::kStudioPro);
    REQUIRE(studio.name == "Studio Pro");
    REQUIRE(studio.motion_intensity == 0.3F);

    auto focus = FxMotionPreset::builtin(MotionPresetId::kFocusMinimal);
    REQUIRE(focus.name == "Focus Minimal");
    REQUIRE(focus.motion_intensity == 0.0F);
    REQUIRE(focus.glow_intensity == 0.0F);
}

TEST_CASE("FxMotionPreset: apply and reset", "[fx][motion]")
{
    EventBus bus;
    FxMotionPreset presets(bus);

    // Default is Studio Pro
    REQUIRE(presets.current().name == "Studio Pro");

    auto applied = presets.apply(MotionPresetId::kBerylNeon);
    REQUIRE(applied.name == "Beryl Neon");
    REQUIRE(presets.current().name == "Beryl Neon");

    presets.reset();
    REQUIRE(presets.current().name == "Studio Pro");
}

TEST_CASE("FxMotionPreset: custom preset", "[fx][motion]")
{
    EventBus bus;
    FxMotionPreset presets(bus);

    MotionPreset custom;
    custom.name = "My Custom";
    custom.motion_intensity = 0.42F;

    presets.apply_custom(custom);
    REQUIRE(presets.current().name == "My Custom");
    REQUIRE(presets.current().preset_id == MotionPresetId::kCustom);
    REQUIRE(presets.current().motion_intensity == 0.42F);
}

TEST_CASE("FxMotionPreset: builtin_names", "[fx][motion]")
{
    auto names = FxMotionPreset::builtin_names();
    REQUIRE(names.size() == 4);
    REQUIRE(names[0] == "Compiz Classic");
    REQUIRE(names[1] == "Beryl Neon");
    REQUIRE(names[2] == "Studio Pro");
    REQUIRE(names[3] == "Focus Minimal");
}

// ============================================================================
// TextFxRenderer Tests
// ============================================================================

TEST_CASE("TextFxRenderer: channel profiles", "[fx][text]")
{
    TextFxRenderer renderer;

    REQUIRE(renderer.channel_count() == 0);
    REQUIRE_FALSE(renderer.has_active_profiles());

    TextFxProfile keyword_profile;
    keyword_profile.enabled = true;
    keyword_profile.glow_radius = 3.0F;
    keyword_profile.glow_alpha = 40;

    renderer.set_profile(TextFxChannel::kKeyword, keyword_profile);

    REQUIRE(renderer.channel_count() == 1);
    REQUIRE(renderer.has_active_profiles());

    auto retrieved = renderer.get_profile(TextFxChannel::kKeyword);
    REQUIRE(retrieved.enabled);
    REQUIRE(retrieved.glow_radius == 3.0F);
}

TEST_CASE("TextFxRenderer: clean mode", "[fx][text]")
{
    TextFxRenderer renderer;

    TextFxProfile profile;
    profile.enabled = true;
    renderer.set_profile(TextFxChannel::kDefault, profile);

    REQUIRE(renderer.render_text_fx(TextFxChannel::kDefault));

    renderer.set_clean_mode(true);
    REQUIRE(renderer.is_clean_mode());
    REQUIRE_FALSE(renderer.render_text_fx(TextFxChannel::kDefault));
    REQUIRE_FALSE(renderer.has_active_profiles());
}

TEST_CASE("TextFxRenderer: channel_name", "[fx][text]")
{
    REQUIRE(TextFxRenderer::channel_name(TextFxChannel::kDefault) == "Default");
    REQUIRE(TextFxRenderer::channel_name(TextFxChannel::kKeyword) == "Keyword");
    REQUIRE(TextFxRenderer::channel_name(TextFxChannel::kString) == "String");
    REQUIRE(TextFxRenderer::channel_name(TextFxChannel::kHeading) == "Heading");
    REQUIRE(TextFxRenderer::channel_name(TextFxChannel::kDiagnostic) == "Diagnostic");
}

TEST_CASE("TextFxRenderer: apply theme text effects", "[fx][text]")
{
    TextFxRenderer renderer;

    TextEffects theme_fx;
    theme_fx.stroke_enabled = true;
    theme_fx.stroke_width = 2.0F;
    theme_fx.outer_glow_enabled = true;
    theme_fx.outer_glow_radius = 5.0F;

    renderer.apply_theme_text_effects(theme_fx);

    auto profile = renderer.get_profile(TextFxChannel::kDefault);
    REQUIRE(profile.enabled);
    REQUIRE(profile.stroke_width == 2.0F);
    REQUIRE(profile.glow_radius == 5.0F);
}

// ============================================================================
// FxPresetRegistry Tests
// ============================================================================

TEST_CASE("FxPresetRegistry: built-in presets", "[fx][registry]")
{
    EventBus bus;
    FxPresetRegistry registry(bus);

    // Should have 4 built-in presets
    REQUIRE(registry.preset_count() == 4);
    REQUIRE(registry.has_preset("Compiz Classic"));
    REQUIRE(registry.has_preset("Beryl Neon"));
    REQUIRE(registry.has_preset("Studio Pro"));
    REQUIRE(registry.has_preset("Focus Minimal"));
}

TEST_CASE("FxPresetRegistry: get preset", "[fx][registry]")
{
    EventBus bus;
    FxPresetRegistry registry(bus);

    auto preset = registry.get_preset("Studio Pro");
    REQUIRE(preset.has_value());
    REQUIRE(preset->name == "Studio Pro");
    REQUIRE(preset->is_builtin);

    auto missing = registry.get_preset("NonExistent");
    REQUIRE_FALSE(missing.has_value());
}

TEST_CASE("FxPresetRegistry: register and remove custom preset", "[fx][registry]")
{
    EventBus bus;
    FxPresetRegistry registry(bus);

    FxPreset custom;
    custom.name = "My Custom FX";
    custom.description = "A test preset";
    custom.is_builtin = false;

    registry.register_preset(custom);
    REQUIRE(registry.preset_count() == 5);
    REQUIRE(registry.has_preset("My Custom FX"));

    REQUIRE(registry.remove_preset("My Custom FX"));
    REQUIRE(registry.preset_count() == 4);

    // Built-in presets cannot be removed
    REQUIRE_FALSE(registry.remove_preset("Compiz Classic"));
}

TEST_CASE("FxPresetRegistry: list presets", "[fx][registry]")
{
    EventBus bus;
    FxPresetRegistry registry(bus);

    auto names = registry.list_presets();
    REQUIRE(names.size() == 4);
}

TEST_CASE("FxPresetRegistry: theme binding", "[fx][registry]")
{
    EventBus bus;
    FxPresetRegistry registry(bus);

    registry.bind_to_theme("Beryl Neon", "dark-theme-1");
    auto bound = registry.preset_for_theme("dark-theme-1");
    REQUIRE(bound.has_value());
    REQUIRE(bound.value() == "Beryl Neon");

    auto unbound = registry.preset_for_theme("other-theme");
    REQUIRE_FALSE(unbound.has_value());
}

TEST_CASE("FxPresetRegistry: export preset", "[fx][registry]")
{
    EventBus bus;
    FxPresetRegistry registry(bus);

    auto exported = registry.export_preset("Studio Pro");
    REQUIRE(exported.has_value());
    REQUIRE(exported->find("Studio Pro") != std::string::npos);

    auto missing_export = registry.export_preset("NonExistent");
    REQUIRE_FALSE(missing_export.has_value());
}

// ============================================================================
// FxSafetyController Tests
// ============================================================================

TEST_CASE("FxSafetyController: defaults", "[fx][safety]")
{
    EventBus bus;
    FxSafetyController safety(bus);

    REQUIRE(safety.is_master_enabled());
    REQUIRE_FALSE(safety.is_reduced_motion());
    REQUIRE_FALSE(safety.is_low_power_mode());
    REQUIRE_FALSE(safety.is_text_safety_mode());
    REQUIRE(safety.is_high_contrast_guard());
    REQUIRE(safety.max_frame_time_budget() == 16.0F);
}

TEST_CASE("FxSafetyController: reduced motion blocks distortion", "[fx][safety]")
{
    EventBus bus;
    FxSafetyController safety(bus);

    REQUIRE(safety.is_pass_safe(FxPassType::kDistortion));

    safety.set_reduced_motion(true);
    REQUIRE_FALSE(safety.is_pass_safe(FxPassType::kDistortion));
    REQUIRE(safety.is_pass_safe(FxPassType::kGlow)); // Glow OK
}

TEST_CASE("FxSafetyController: text safety blocks chroma and distortion", "[fx][safety]")
{
    EventBus bus;
    FxSafetyController safety(bus);

    safety.set_text_safety_mode(true);
    REQUIRE_FALSE(safety.is_pass_safe(FxPassType::kDistortion));
    REQUIRE_FALSE(safety.is_pass_safe(FxPassType::kChromaShift));
    REQUIRE(safety.is_pass_safe(FxPassType::kGlow));
}

TEST_CASE("FxSafetyController: low power blocks expensive passes", "[fx][safety]")
{
    EventBus bus;
    FxSafetyController safety(bus);

    safety.set_low_power_mode(true);
    REQUIRE_FALSE(safety.is_pass_safe(FxPassType::kBlur));
    REQUIRE_FALSE(safety.is_pass_safe(FxPassType::kBloom));
    REQUIRE_FALSE(safety.is_pass_safe(FxPassType::kReflection));
    REQUIRE_FALSE(safety.is_pass_safe(FxPassType::kNoiseGrain));
    REQUIRE(safety.is_pass_safe(FxPassType::kShadow)); // Still OK
    REQUIRE(safety.is_pass_safe(FxPassType::kStroke)); // Still OK
}

TEST_CASE("FxSafetyController: auto-degrade on frame violations", "[fx][safety]")
{
    EventBus bus;
    FxSafetyController safety(bus);

    REQUIRE_FALSE(safety.should_degrade());
    REQUIRE(safety.recommended_tier() == QualityTier::kCinematic);

    // Simulate frame budget violations
    FrameTimingSnapshot slow_frame;
    slow_frame.frame_time_ms = 20.0F; // over 16ms budget

    safety.report_frame_timing(slow_frame);
    safety.report_frame_timing(slow_frame);
    safety.report_frame_timing(slow_frame);

    REQUIRE(safety.should_degrade());
    REQUIRE(safety.violation_count() >= 3);
    REQUIRE(safety.recommended_tier() == QualityTier::kEfficient);
}

TEST_CASE("FxSafetyController: master disabled blocks all", "[fx][safety]")
{
    EventBus bus;
    FxSafetyController safety(bus);

    safety.set_master_enabled(false);
    REQUIRE_FALSE(safety.is_pass_safe(FxPassType::kBlur));
    REQUIRE_FALSE(safety.is_pass_safe(FxPassType::kGlow));
    REQUIRE(safety.recommended_tier() == QualityTier::kMinimal);
}

TEST_CASE("FxSafetyController: reset to defaults", "[fx][safety]")
{
    EventBus bus;
    FxSafetyController safety(bus);

    safety.set_reduced_motion(true);
    safety.set_low_power_mode(true);
    safety.set_text_safety_mode(true);
    safety.set_max_frame_time_budget(8.0F);

    safety.reset_to_defaults();

    REQUIRE(safety.is_master_enabled());
    REQUIRE_FALSE(safety.is_reduced_motion());
    REQUIRE_FALSE(safety.is_low_power_mode());
    REQUIRE_FALSE(safety.is_text_safety_mode());
    REQUIRE(safety.max_frame_time_budget() == 16.0F);
    REQUIRE(safety.violation_count() == 0);
}

// ============================================================================
// Event Type Tests
// ============================================================================

TEST_CASE("FX Events: construction", "[fx][events]")
{
    events::FxPresetChangedEvent preset_evt;
    preset_evt.preset_name = "Beryl Neon";
    preset_evt.previous_preset = "Studio Pro";
    REQUIRE(preset_evt.preset_name == "Beryl Neon");

    events::FxMasterToggleEvent toggle_evt;
    toggle_evt.enabled = true;
    REQUIRE(toggle_evt.enabled);

    events::FxQualityTierChangedEvent tier_evt;
    tier_evt.tier_name = "Cinematic";
    tier_evt.previous_tier = "Balanced";
    REQUIRE(tier_evt.tier_name == "Cinematic");

    events::FxSafetyModeChangedEvent safety_evt;
    safety_evt.mode_name = "reduced_motion";
    safety_evt.enabled = true;
    REQUIRE(safety_evt.enabled);

    events::FxAutoDegrade degrade_evt;
    degrade_evt.from_tier = "Cinematic";
    degrade_evt.to_tier = "Efficient";
    degrade_evt.violation_count = 5;
    REQUIRE(degrade_evt.violation_count == 5);
}
