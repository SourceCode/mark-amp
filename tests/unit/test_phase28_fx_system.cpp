/// test_phase28_fx_system.cpp — Phase 28: FX Visual Effects System Tests

#include "core/EventBus.h"
#include "core/Events.h"
#include "core/Theme.h"
#include "rendering/FxAccessibility.h"
#include "rendering/FxCommandProvider.h"
#include "rendering/FxCssGenerator.h"
#include "rendering/FxDiagnostics.h"
#include "rendering/FxEngine.h"
#include "rendering/FxProfilePersistence.h"
#include "rendering/FxTransitionEngine.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;
using namespace markamp::core::events;
using namespace markamp::rendering;

// ============================================================================
// FxTransitionEngine Tests
// ============================================================================

TEST_CASE("FxTransitionEngine: start and complete transition", "[fx][transition]")
{
    EventBus bus;
    FxTransitionEngine engine(bus);

    REQUIRE_FALSE(engine.is_transitioning());
    REQUIRE(engine.transitions_completed() == 0);

    TransitionConfig config;
    config.duration_ms = 100.0F;
    engine.start_transition("from", "to", config);

    REQUIRE(engine.is_transitioning());
    auto state = engine.state();
    REQUIRE(state.has_value());
    REQUIRE(state->from_preset == "from");
    REQUIRE(state->to_preset == "to");
    REQUIRE(state->progress() == 0.0F);

    // Advance 50% through.
    REQUIRE(engine.update(50.0F));
    state = engine.state();
    REQUIRE(state.has_value());
    REQUIRE(state->progress() == Catch::Approx(0.5F));

    // Complete.
    REQUIRE_FALSE(engine.update(60.0F));
    REQUIRE_FALSE(engine.is_transitioning());
    REQUIRE(engine.transitions_completed() == 1);
}

TEST_CASE("FxTransitionEngine: cancel transition", "[fx][transition]")
{
    EventBus bus;
    FxTransitionEngine engine(bus);

    engine.start_transition("a", "b");
    REQUIRE(engine.is_transitioning());

    engine.cancel();
    REQUIRE_FALSE(engine.is_transitioning());
    REQUIRE(engine.transitions_completed() == 0);
}

TEST_CASE("FxTransitionEngine: easing functions", "[fx][transition]")
{
    // Linear: identity.
    REQUIRE(FxTransitionEngine::apply_easing(0.0F, EasingFunction::kLinear) == 0.0F);
    REQUIRE(FxTransitionEngine::apply_easing(0.5F, EasingFunction::kLinear) == 0.5F);
    REQUIRE(FxTransitionEngine::apply_easing(1.0F, EasingFunction::kLinear) == 1.0F);

    // Ease-in: starts slow.
    REQUIRE(FxTransitionEngine::apply_easing(0.5F, EasingFunction::kEaseIn) < 0.5F);

    // Ease-out: starts fast.
    REQUIRE(FxTransitionEngine::apply_easing(0.5F, EasingFunction::kEaseOut) > 0.5F);

    // Ease-in-out: endpoints.
    REQUIRE(FxTransitionEngine::apply_easing(0.0F, EasingFunction::kEaseInOut) == 0.0F);
    REQUIRE(FxTransitionEngine::apply_easing(1.0F, EasingFunction::kEaseInOut) == 1.0F);

    // Spring: can overshoot.
    float spring_end = FxTransitionEngine::apply_easing(1.0F, EasingFunction::kSpring);
    REQUIRE(spring_end == Catch::Approx(1.0F).margin(0.01F));
}

TEST_CASE("FxTransitionEngine: easing names", "[fx][transition]")
{
    REQUIRE(FxTransitionEngine::easing_name(EasingFunction::kLinear) == "Linear");
    REQUIRE(FxTransitionEngine::easing_name(EasingFunction::kEaseIn) == "Ease In");
    REQUIRE(FxTransitionEngine::easing_name(EasingFunction::kEaseOut) == "Ease Out");
    REQUIRE(FxTransitionEngine::easing_name(EasingFunction::kEaseInOut) == "Ease In-Out");
    REQUIRE(FxTransitionEngine::easing_name(EasingFunction::kSpring) == "Spring");
}

TEST_CASE("FxTransitionEngine: interpolate floats", "[fx][transition]")
{
    EventBus bus;
    FxTransitionEngine engine(bus);

    TransitionConfig config;
    config.duration_ms = 100.0F;
    config.easing = EasingFunction::kLinear;
    engine.start_transition("from", "to", config);

    engine.update(50.0F); // 50% progress.
    float val = engine.interpolate(0.0F, 1.0F);
    REQUIRE(val == Catch::Approx(0.5F).margin(0.01F));
}

TEST_CASE("FxTransitionEngine: interpolate motion preset", "[fx][transition]")
{
    EventBus bus;
    FxTransitionEngine engine(bus);

    TransitionConfig config;
    config.duration_ms = 100.0F;
    config.easing = EasingFunction::kLinear;
    engine.start_transition("a", "b", config);
    engine.update(50.0F);

    MotionPreset from_preset;
    from_preset.motion_intensity = 0.0F;
    from_preset.glow_intensity = 0.0F;

    MotionPreset to_preset;
    to_preset.motion_intensity = 1.0F;
    to_preset.glow_intensity = 1.0F;

    auto result = engine.interpolate_motion(from_preset, to_preset);
    REQUIRE(result.motion_intensity == Catch::Approx(0.5F).margin(0.05F));
    REQUIRE(result.glow_intensity == Catch::Approx(0.5F).margin(0.05F));
    REQUIRE(result.preset_id == MotionPresetId::kCustom);
}

TEST_CASE("FxTransitionEngine: default config", "[fx][transition]")
{
    EventBus bus;
    FxTransitionEngine engine(bus);

    auto& def = engine.default_config();
    REQUIRE(def.duration_ms == 500.0F);
    REQUIRE(def.easing == EasingFunction::kEaseInOut);
    REQUIRE(def.crossfade);

    TransitionConfig custom;
    custom.duration_ms = 200.0F;
    custom.easing = EasingFunction::kSpring;
    engine.set_default_config(custom);
    REQUIRE(engine.default_config().duration_ms == 200.0F);
}

TEST_CASE("FxTransitionEngine: replacing in-progress transition", "[fx][transition]")
{
    EventBus bus;
    FxTransitionEngine engine(bus);

    engine.start_transition("a", "b");
    REQUIRE(engine.is_transitioning());

    engine.start_transition("b", "c");
    REQUIRE(engine.is_transitioning());
    auto state = engine.state();
    REQUIRE(state->to_preset == "c");
}

// ============================================================================
// FxCssGenerator Tests
// ============================================================================

TEST_CASE("FxCssGenerator: text effects CSS", "[fx][css]")
{
    FxCssGenerator gen;
    TextEffects effects;
    effects.shadow_enabled = true;
    effects.outer_glow_enabled = true;

    auto css = gen.generate_text_effects_css(effects);
    REQUIRE(css.find("text-shadow") != std::string::npos);
    REQUIRE(css.find("fx-text-shadow") != std::string::npos);
    REQUIRE(css.find("fx-text-glow") != std::string::npos);
}

TEST_CASE("FxCssGenerator: gradient fill CSS", "[fx][css]")
{
    FxCssGenerator gen;
    TextEffects effects;
    effects.gradient_fill_enabled = true;

    auto css = gen.generate_text_effects_css(effects);
    REQUIRE(css.find("linear-gradient") != std::string::npos);
    REQUIRE(css.find("-webkit-background-clip") != std::string::npos);
}

TEST_CASE("FxCssGenerator: UI effects CSS", "[fx][css]")
{
    FxCssGenerator gen;
    UiElementEffects effects;

    auto css = gen.generate_ui_effects_css(effects);
    REQUIRE(css.find("fx-hover-glow") != std::string::npos);
    REQUIRE(css.find("fx-button") != std::string::npos);
    REQUIRE(css.find("fx-focus-ring") != std::string::npos);
    REQUIRE(css.find("fx-panel-shadow") != std::string::npos);
}

TEST_CASE("FxCssGenerator: editor effects CSS", "[fx][css]")
{
    FxCssGenerator gen;
    EditorEffects effects;
    effects.caret_glow_enabled = true;
    effects.active_line_glow = true;
    effects.selection_shimmer = true;

    auto css = gen.generate_editor_effects_css(effects);
    REQUIRE(css.find("fx-caret") != std::string::npos);
    REQUIRE(css.find("fx-active-line") != std::string::npos);
    REQUIRE(css.find("fx-shimmer") != std::string::npos);
}

TEST_CASE("FxCssGenerator: canvas effects CSS", "[fx][css]")
{
    FxCssGenerator gen;
    CanvasEffects effects;

    auto css = gen.generate_canvas_effects_css(effects);
    REQUIRE(css.find("fx-canvas-object") != std::string::npos);
    REQUIRE(css.find("fx-canvas-selected") != std::string::npos);
}

TEST_CASE("FxCssGenerator: CSS variables", "[fx][css]")
{
    FxCssGenerator gen;
    FxPreset preset;
    preset.name = "Test";

    auto css = gen.generate_css_variables(preset);
    REQUIRE(css.find(":root") != std::string::npos);
    REQUIRE(css.find("--fx-motion-intensity") != std::string::npos);
    REQUIRE(css.find("--fx-glow-intensity") != std::string::npos);
    REQUIRE(css.find("--fx-duration-scale") != std::string::npos);
}

TEST_CASE("FxCssGenerator: animation keyframes", "[fx][css]")
{
    FxCssGenerator gen;
    MotionPreset motion;

    auto css = gen.generate_animation_keyframes(motion);
    REQUIRE(css.find("@keyframes fx-glow-pulse") != std::string::npos);
    REQUIRE(css.find("@keyframes fx-bloom-pulse") != std::string::npos);
    REQUIRE(css.find("fx-animated") != std::string::npos);
}

TEST_CASE("FxCssGenerator: full stylesheet", "[fx][css]")
{
    FxCssGenerator gen;
    FxPreset preset;
    preset.name = "Neon";
    preset.text.shadow_enabled = true;
    preset.editor.caret_glow_enabled = true;

    auto css = gen.generate_full_stylesheet(preset);
    REQUIRE(css.find("MarkAmp FX Stylesheet: Neon") != std::string::npos);
    REQUIRE(css.find(":root") != std::string::npos);
    REQUIRE(css.find("@keyframes") != std::string::npos);
}

TEST_CASE("FxCssGenerator: css_color utility", "[fx][css]")
{
    auto opaque = FxCssGenerator::css_color({255, 100, 50});
    REQUIRE(opaque == "rgb(255, 100, 50)");

    auto alpha = FxCssGenerator::css_color({100, 99, 255}, 128);
    REQUIRE(alpha.find("rgba(100, 99, 255") != std::string::npos);
}

TEST_CASE("FxCssGenerator: has_active_effects", "[fx][css]")
{
    FxPreset empty_preset;
    empty_preset.motion.motion_intensity = 0.0F;
    empty_preset.motion.glow_intensity = 0.0F;
    empty_preset.motion.bloom_intensity = 0.0F;
    REQUIRE_FALSE(FxCssGenerator::has_active_effects(empty_preset));

    FxPreset active_preset;
    active_preset.text.bloom_enabled = true;
    REQUIRE(FxCssGenerator::has_active_effects(active_preset));
}

// ============================================================================
// FxProfilePersistence Tests
// ============================================================================

TEST_CASE("FxProfilePersistence: serialize and deserialize round-trip", "[fx][profile]")
{
    FxProfilePersistence persistence;

    FxPreset preset;
    preset.name = "Neon Dreams";
    preset.description = "Vibrant neon effects";
    preset.version = 2;
    preset.motion.motion_intensity = 0.8F;
    preset.motion.glow_intensity = 0.6F;
    preset.text.bloom_enabled = true;
    preset.text.bloom_intensity = 0.5F;
    preset.editor.caret_glow_enabled = true;
    preset.canvas.selection_glow_pulse = true;

    auto json = persistence.serialize(preset);
    auto deserialized = persistence.deserialize(json);

    REQUIRE(deserialized.has_value());
    REQUIRE(deserialized->name == "Neon Dreams");
    REQUIRE(deserialized->version == 2);
    REQUIRE(deserialized->motion.motion_intensity == Catch::Approx(0.8F));
    REQUIRE(deserialized->motion.glow_intensity == Catch::Approx(0.6F));
    REQUIRE(deserialized->text.bloom_enabled);
    REQUIRE(deserialized->editor.caret_glow_enabled);
    REQUIRE(deserialized->canvas.selection_glow_pulse);
}

TEST_CASE("FxProfilePersistence: deserialize invalid JSON", "[fx][profile]")
{
    FxProfilePersistence persistence;
    auto result = persistence.deserialize("not valid json {{{");
    REQUIRE_FALSE(result.has_value());
}

TEST_CASE("FxProfilePersistence: save and load profile", "[fx][profile]")
{
    FxProfilePersistence persistence;

    FxPreset preset;
    preset.name = "My Profile";

    persistence.save_user_profile("prof1", preset);
    REQUIRE(persistence.has_profile("prof1"));
    REQUIRE(persistence.profile_count() == 1);

    auto loaded = persistence.load_user_profile("prof1");
    REQUIRE(loaded.has_value());
    REQUIRE(loaded->name == "My Profile");
}

TEST_CASE("FxProfilePersistence: overwrite existing profile", "[fx][profile]")
{
    FxProfilePersistence persistence;

    FxPreset preset1;
    preset1.name = "Version 1";
    persistence.save_user_profile("test", preset1);

    FxPreset preset2;
    preset2.name = "Version 2";
    persistence.save_user_profile("test", preset2);

    REQUIRE(persistence.profile_count() == 1);
    auto loaded = persistence.load_user_profile("test");
    REQUIRE(loaded->name == "Version 2");
}

TEST_CASE("FxProfilePersistence: delete profile", "[fx][profile]")
{
    FxProfilePersistence persistence;

    FxPreset preset;
    persistence.save_user_profile("to_delete", preset);
    REQUIRE(persistence.has_profile("to_delete"));

    REQUIRE(persistence.delete_user_profile("to_delete"));
    REQUIRE_FALSE(persistence.has_profile("to_delete"));
    REQUIRE_FALSE(persistence.delete_user_profile("nonexistent"));
}

TEST_CASE("FxProfilePersistence: list profiles", "[fx][profile]")
{
    FxProfilePersistence persistence;

    FxPreset preset;
    persistence.save_user_profile("alpha", preset);
    persistence.save_user_profile("beta", preset);
    persistence.save_user_profile("gamma", preset);

    auto names = persistence.list_user_profiles();
    REQUIRE(names.size() == 3);
}

TEST_CASE("FxProfilePersistence: export and import all", "[fx][profile]")
{
    FxProfilePersistence persistence;

    FxPreset p1;
    p1.name = "Preset A";
    FxPreset p2;
    p2.name = "Preset B";

    persistence.save_user_profile("a", p1);
    persistence.save_user_profile("b", p2);

    auto exported = persistence.export_all();
    REQUIRE_FALSE(exported.empty());

    FxProfilePersistence other;
    auto count = other.import_all(exported);
    REQUIRE(count == 2);
    REQUIRE(other.profile_count() == 2);
}

TEST_CASE("FxProfilePersistence: clear all", "[fx][profile]")
{
    FxProfilePersistence persistence;

    FxPreset preset;
    persistence.save_user_profile("a", preset);
    persistence.save_user_profile("b", preset);
    REQUIRE(persistence.profile_count() == 2);

    persistence.clear_all();
    REQUIRE(persistence.profile_count() == 0);
}

// ============================================================================
// FxDiagnostics Tests
// ============================================================================

TEST_CASE("FxDiagnostics: record and query frames", "[fx][diagnostics]")
{
    FxDiagnostics diag;

    REQUIRE(diag.total_frames() == 0);
    REQUIRE(diag.avg_frame_time() == 0.0F);

    FxFrameResult result;
    result.passes_executed = 3;
    result.passes_skipped = 1;

    diag.record_frame(result, 8.0F);
    diag.record_frame(result, 12.0F);
    diag.record_frame(result, 10.0F);

    REQUIRE(diag.total_frames() == 3);
    REQUIRE(diag.avg_frame_time() == Catch::Approx(10.0F));
}

TEST_CASE("FxDiagnostics: snapshot", "[fx][diagnostics]")
{
    FxDiagnostics diag;

    FxFrameResult result;
    result.passes_executed = 2;
    diag.record_frame(result, 5.0F);
    diag.record_frame(result, 15.0F);
    diag.record_frame(result, 10.0F);

    auto snapshot = diag.get_snapshot();
    REQUIRE(snapshot.total_frames == 3);
    REQUIRE(snapshot.avg_frame_time_ms == Catch::Approx(10.0F));
    REQUIRE(snapshot.min_frame_time_ms == 5.0F);
    REQUIRE(snapshot.max_frame_time_ms == 15.0F);
}

TEST_CASE("FxDiagnostics: per-pass timing", "[fx][diagnostics]")
{
    FxDiagnostics diag;

    diag.record_pass_timing("blur", 2.0F, false);
    diag.record_pass_timing("blur", 4.0F, false);
    diag.record_pass_timing("blur", 0.0F, true);

    auto metrics = diag.get_pass_metrics("blur");
    REQUIRE(metrics.has_value());
    REQUIRE(metrics->execution_count == 2);
    REQUIRE(metrics->skip_count == 1);
    REQUIRE(metrics->max_time_ms == 4.0F);
}

TEST_CASE("FxDiagnostics: health report — excellent", "[fx][diagnostics]")
{
    FxDiagnostics diag;

    FxFrameResult result;
    for (int idx = 0; idx < 20; ++idx)
    {
        diag.record_frame(result, 8.0F);
    }

    auto report = diag.get_health_report(16.0F);
    REQUIRE(report.level == FxHealthReport::HealthLevel::kExcellent);
    REQUIRE(report.budget_utilization < 1.0F);
}

TEST_CASE("FxDiagnostics: health report — critical", "[fx][diagnostics]")
{
    FxDiagnostics diag;

    FxFrameResult result;
    for (int idx = 0; idx < 20; ++idx)
    {
        diag.record_frame(result, 25.0F);
    }

    auto report = diag.get_health_report(16.0F);
    REQUIRE(report.level == FxHealthReport::HealthLevel::kCritical);
    REQUIRE(report.budget_utilization > 1.0F);
    REQUIRE_FALSE(report.recommendations.empty());
}

TEST_CASE("FxDiagnostics: window size", "[fx][diagnostics]")
{
    FxDiagnostics diag;
    REQUIRE(diag.window_size() == 120);

    diag.set_window_size(5);
    REQUIRE(diag.window_size() == 5);

    FxFrameResult result;
    for (int idx = 0; idx < 10; ++idx)
    {
        diag.record_frame(result, 10.0F);
    }
    REQUIRE(diag.total_frames() == 5);
}

TEST_CASE("FxDiagnostics: reset", "[fx][diagnostics]")
{
    FxDiagnostics diag;

    FxFrameResult result;
    diag.record_frame(result, 10.0F);
    diag.record_pass_timing("blur", 2.0F, false);

    diag.reset();
    REQUIRE(diag.total_frames() == 0);
    REQUIRE_FALSE(diag.get_pass_metrics("blur").has_value());
}

// ============================================================================
// FxCommandProvider Tests
// ============================================================================

TEST_CASE("FxCommandProvider: registers 10 commands", "[fx][commands]")
{
    FxCommandProvider provider;
    REQUIRE(provider.command_count() == 10);
}

TEST_CASE("FxCommandProvider: find command by ID", "[fx][commands]")
{
    FxCommandProvider provider;

    auto cmd = provider.find_command("fx.toggle_effects");
    REQUIRE(cmd.has_value());
    REQUIRE(cmd->title == "FX: Toggle Effects");

    auto missing = provider.find_command("nonexistent");
    REQUIRE_FALSE(missing.has_value());
}

TEST_CASE("FxCommandProvider: two categories", "[fx][commands]")
{
    FxCommandProvider provider;
    auto cats = provider.categories();

    REQUIRE(cats.size() == 2);
    // Contains both "Visual Effects" and "FX Accessibility".
    bool has_visual = false;
    bool has_accessibility = false;
    for (const auto& cat : cats)
    {
        if (cat == "Visual Effects")
            has_visual = true;
        if (cat == "FX Accessibility")
            has_accessibility = true;
    }
    REQUIRE(has_visual);
    REQUIRE(has_accessibility);
}

TEST_CASE("FxCommandProvider: commands in category", "[fx][commands]")
{
    FxCommandProvider provider;

    auto visual_cmds = provider.commands_in_category("Visual Effects");
    REQUIRE(visual_cmds.size() == 8);

    auto access_cmds = provider.commands_in_category("FX Accessibility");
    REQUIRE(access_cmds.size() == 2);
}

TEST_CASE("FxCommandProvider: available commands filtering", "[fx][commands]")
{
    FxCommandProvider provider;

    auto all_cmds = provider.available_commands(true);
    REQUIRE(all_cmds.size() == 10);

    auto limited_cmds = provider.available_commands(false);
    // Only commands that don't require FX enabled.
    REQUIRE(limited_cmds.size() < 10);
    for (const auto& cmd : limited_cmds)
    {
        REQUIRE_FALSE(cmd.requires_fx_enabled);
    }
}

// ============================================================================
// FxAccessibility Tests
// ============================================================================

TEST_CASE("FxAccessibility: preset change announcement", "[fx][accessibility]")
{
    FxAccessibility access;
    auto msg = access.announce_preset_change("Compiz", "Beryl");
    REQUIRE(msg.find("Compiz") != std::string::npos);
    REQUIRE(msg.find("Beryl") != std::string::npos);
}

TEST_CASE("FxAccessibility: quality change announcement", "[fx][accessibility]")
{
    FxAccessibility access;
    auto msg = access.announce_quality_change(QualityTier::kCinematic);
    REQUIRE(msg.find("Cinematic") != std::string::npos);

    msg = access.announce_quality_change(QualityTier::kMinimal);
    REQUIRE(msg.find("Minimal") != std::string::npos);
}

TEST_CASE("FxAccessibility: safety mode announcement", "[fx][accessibility]")
{
    FxAccessibility access;
    auto on = access.announce_safety_mode("Reduced Motion", true);
    REQUIRE(on.find("enabled") != std::string::npos);

    auto off = access.announce_safety_mode("Reduced Motion", false);
    REQUIRE(off.find("disabled") != std::string::npos);
}

TEST_CASE("FxAccessibility: master toggle announcement", "[fx][accessibility]")
{
    FxAccessibility access;
    REQUIRE(access.announce_master_toggle(true) == "Visual effects enabled");
    REQUIRE(access.announce_master_toggle(false) == "Visual effects disabled");
}

TEST_CASE("FxAccessibility: OS preferences", "[fx][accessibility]")
{
    FxAccessibility access;
    // Default returns false (no OS API calls in test).
    REQUIRE_FALSE(access.prefers_reduced_motion());
    REQUIRE_FALSE(access.prefers_high_contrast());
}

TEST_CASE("FxAccessibility: summary", "[fx][accessibility]")
{
    FxAccessibility access;
    auto summary =
        access.get_accessibility_summary(true, true, false, true, QualityTier::kBalanced);

    REQUIRE(summary.find("Enabled") != std::string::npos);
    REQUIRE(summary.find("Balanced") != std::string::npos);
    REQUIRE(summary.find("Reduced Motion: On") != std::string::npos);
    REQUIRE(summary.find("Text Safety: On") != std::string::npos);
}

TEST_CASE("FxAccessibility: announcement history", "[fx][accessibility]")
{
    FxAccessibility access;
    REQUIRE(access.announcement_count() == 0);

    access.record_announcement("Test 1");
    access.record_announcement("Test 2");
    REQUIRE(access.announcement_count() == 2);

    auto& history = access.announcement_history();
    REQUIRE(history[0] == "Test 1");
    REQUIRE(history[1] == "Test 2");

    access.clear_history();
    REQUIRE(access.announcement_count() == 0);
}

TEST_CASE("FxAccessibility: recommendations", "[fx][accessibility]")
{
    FxAccessibility access;
    auto recs = access.get_recommendations(true, false);
    REQUIRE_FALSE(recs.empty());

    auto recs_off = access.get_recommendations(false, false);
    // Less recommendations when FX is off.
    REQUIRE(recs_off.size() <= recs.size());
}

// ============================================================================
// Phase 28 Events Tests
// ============================================================================

TEST_CASE("Events: FxTransitionStartedEvent", "[fx][events]")
{
    FxTransitionStartedEvent evt;
    evt.preset_name = "Beryl Neon";
    evt.duration_ms = 500;

    REQUIRE(evt.preset_name == "Beryl Neon");
    REQUIRE(evt.duration_ms == 500);
}

TEST_CASE("Events: FxTransitionCompletedEvent", "[fx][events]")
{
    FxTransitionCompletedEvent evt;
    evt.preset_name = "StudioPro";
    evt.was_cancelled = true;

    REQUIRE(evt.preset_name == "StudioPro");
    REQUIRE(evt.was_cancelled);
}

TEST_CASE("Events: FxCssGeneratedEvent", "[fx][events]")
{
    FxCssGeneratedEvent evt;
    evt.css_length = 2048;
    evt.preset_name = "Custom";

    REQUIRE(evt.css_length == 2048);
    REQUIRE(evt.preset_name == "Custom");
}

TEST_CASE("Events: FxProfileSavedEvent", "[fx][events]")
{
    FxProfileSavedEvent evt;
    evt.profile_name = "My Effects";
    REQUIRE(evt.profile_name == "My Effects");
}

TEST_CASE("Events: FxDiagnosticsSnapshotEvent", "[fx][events]")
{
    FxDiagnosticsSnapshotEvent evt;
    evt.avg_frame_ms = 8.5F;
    evt.pass_count = 5;

    REQUIRE(evt.avg_frame_ms == Catch::Approx(8.5F));
    REQUIRE(evt.pass_count == 5);
}

TEST_CASE("Events: FxAccessibilityAnnouncedEvent", "[fx][events]")
{
    FxAccessibilityAnnouncedEvent evt;
    evt.message = "Visual effects disabled";
    REQUIRE(evt.message == "Visual effects disabled");
}
