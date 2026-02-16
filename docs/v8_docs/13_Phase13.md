Phase 46 Prompt: FX Core Engine (Compiz/Beryl Foundation)

Goal: build a real-time compositor/effects pipeline that can drive window, panel, and text effects consistently.
Files: FxEngine.h, FxEngine.cpp, FxPass.h, FxPass.cpp, CustomChrome.cpp, Theme.h, ThemeEngine.cpp.
Implement: add effect graph with ordered passes (blur, glow, shadow, distortion, color-grade, composite), per-surface render targets, and quality tiers.
Implement: extend WindowEffects tokens in Theme.h for animation curves, blur radius, wobble intensity, bloom, chroma, scanline, and reflection.
Acceptance: app can enable/disable effect groups at runtime with no restart and no visual tearing.
Phase 47 Prompt: Compiz/Beryl Motion Pack

Goal: replicate signature Compiz/Beryl interaction feel across the app shell.
Files: LayoutManager.cpp, TabBar.cpp, SplitterBar.cpp, ActivityBar.cpp, SurfaceTransitionCoordinator.cpp (or create it), Events.h.
Implement effects: wobbly panel drag, elastic snapping, expo-style tab/workspace overview, glide/scale transitions, magic-lamp minimize variant, burn/fade close transition option.
Implement: motion presets Compiz Classic, Beryl Neon, Minimal Smooth with user-selectable intensity.
Acceptance: transitions between editor, preview, canvas, graph feel continuous and physically coherent, with consistent easing.
Phase 48 Prompt: Full Text FX Renderer Stack

Goal: add full visual FX rendering for text in editor, preview, status, and canvas labels.
Files: TextFxRenderer.h, TextFxRenderer.cpp, GlyphAdvanceCache.h, EditorPanel.cpp, PreviewPanel.cpp, StatusBarPanel.cpp, TextBoxRenderer.cpp.
Implement text passes: subpixel outline, soft shadow, neon glow, gradient ink, bloom, subtle distortion, optional CRT/scanline filter mode.
Implement: per-token/per-context style channels so code tokens, headings, diagnostics, and UI labels can have different FX recipes.
Implement: fallback raster path for low-end hardware and “clean mode” for no effects.
Acceptance: text remains sharp and readable at all zoom levels while FX can be layered on top without aliasing artifacts.
Phase 49 Prompt: FX UX Controls and Theming

Goal: make effects configurable, discoverable, and safe for productivity use.
Files: SettingsPanel.cpp, ThemeTokenEditor.cpp, MainFrame.cpp, CommandPalette.cpp.
Implement settings sections: Visual Effects, Text Effects, Motion, Performance.
Implement controls: global FX master toggle, preset selector, per-surface toggles (Editor, Preview, Canvas, Chrome), intensity sliders, blur/glow levels, text FX profile selector.
Implement menu and palette entries: Toggle Effects, Switch FX Preset, Text FX On/Off, Reduce Motion.
Acceptance: users can switch from cinematic to minimal instantly without disrupting workflow state.
Phase 50 Prompt: Performance, Stability, and Accessibility Gates

Goal: ensure cinematic effects never degrade core editing usability.
Files: test_visual_fx.cpp, test_phase20_perf.cpp, test_accessibility.cpp, test_editor_qol.cpp.
Implement perf budgets: frame time, input latency, scroll latency, transition duration, text render cost per 1k glyphs.
Implement behavior gates: auto-degrade effects under load, disable heavy passes on battery saver/low-power profile, preserve caret and selection responsiveness first.
Implement accessibility: reduced-motion mode, contrast-safe text FX constraints, no mandatory blur for critical text.
Acceptance: with effects enabled, editor/preview remain responsive and pass performance thresholds; reduced-motion mode fully bypasses Compiz/Beryl motion effects.

**Next Set Of Updates (to fully support Compiz/Beryl-style UX + configurable FX)**

1. **Phase 51: FX Capability Matrix + Runtime Negotiation**

- Add runtime capability detection and pass gating so effects degrade safely by hardware/profile.
- Implement in `/Volumes/SecondDrive/code2/mark-amp/src/rendering/FxEngine.cpp`, `/Volumes/SecondDrive/code2/mark-amp/src/rendering/FxCapabilities.h`, `/Volumes/SecondDrive/code2/mark-amp/src/rendering/FxCapabilities.cpp`.
- Add quality tiers (`cinematic`, `balanced`, `efficient`) and auto-fallback rules.
- Persist capabilities and chosen tier in `/Volumes/SecondDrive/code2/mark-amp/src/core/Config.cpp`.
- Acceptance: each effect pass can be enabled/disabled by capability flags without breaking layout or text legibility.

2. **Phase 52: Preset System + Theme Binding**

- Add effect preset registry and import/export format.
- Implement in `/Volumes/SecondDrive/code2/mark-amp/src/rendering/FxPresetRegistry.h`, `/Volumes/SecondDrive/code2/mark-amp/src/rendering/FxPresetRegistry.cpp`, `/Volumes/SecondDrive/code2/mark-amp/src/core/Theme.h`, `/Volumes/SecondDrive/code2/mark-amp/src/core/ThemeLoader.cpp`.
- Bind presets to themes so VSCode theme imports can carry optional FX profiles.
- Add preset migration/versioning support.
- Acceptance: switching theme can optionally switch FX profile atomically.

3. **Phase 53: Surface-Scoped FX Routing**

- Add per-surface effect routing so window, editor text, preview text, canvas text, and chrome are independently controlled.
- Implement in `/Volumes/SecondDrive/code2/mark-amp/src/rendering/FxGraph.h`, `/Volumes/SecondDrive/code2/mark-amp/src/rendering/FxGraph.cpp`, `/Volumes/SecondDrive/code2/mark-amp/src/ui/CustomChrome.cpp`, `/Volumes/SecondDrive/code2/mark-amp/src/ui/EditorPanel.cpp`, `/Volumes/SecondDrive/code2/mark-amp/src/ui/PreviewPanel.cpp`, `/Volumes/SecondDrive/code2/mark-amp/src/ui/CanvasWorkspacePanel.cpp`.
- Add explicit routing IDs for `window`, `toolbar`, `tabbar`, `statusbar`, `editor_text`, `preview_text`, `canvas_text`, `overlays`.
- Acceptance: users can tune each surface without side effects on other surfaces.

4. **Phase 54: Advanced Text FX Pipeline**

- Expand text renderer for high-quality multi-pass text effects with clarity preservation.
- Implement in `/Volumes/SecondDrive/code2/mark-amp/src/rendering/TextFxRenderer.cpp`, `/Volumes/SecondDrive/code2/mark-amp/src/rendering/GlyphAdvanceCache.h`, `/Volumes/SecondDrive/code2/mark-amp/src/rendering/GlyphAtlas.h`, `/Volumes/SecondDrive/code2/mark-amp/src/rendering/GlyphAtlas.cpp`.
- Add stroke-first compositing, halo pass, glow pass, and final readability clamp.
- Add per-token FX channels for code syntax categories.
- Acceptance: text FX remain crisp at different zoom levels and do not blur caret/selection fidelity.

5. **Phase 55: UX Controls + Live Preview Of FX**

- Add a dedicated Visual Effects settings page with live preview blocks.
- Implement in `/Volumes/SecondDrive/code2/mark-amp/src/ui/SettingsPanel.cpp`, `/Volumes/SecondDrive/code2/mark-amp/src/ui/ThemeTokenEditor.cpp`, `/Volumes/SecondDrive/code2/mark-amp/src/ui/MainFrame.cpp`.
- Add menu and command palette actions for quick toggles and preset cycling.
- Add explicit “safe mode” toggle that disables heavy passes instantly.
- Acceptance: users can adjust FX interactively and rollback immediately if readability or performance drops.

6. **Phase 56: Validation + Regression Gates**

- Add correctness, perf, and accessibility tests for FX.
- Implement tests in `/Volumes/SecondDrive/code2/mark-amp/tests/unit/test_visual_fx.cpp`, `/Volumes/SecondDrive/code2/mark-amp/tests/unit/test_accessibility.cpp`, `/Volumes/SecondDrive/code2/mark-amp/tests/unit/test_phase20_perf.cpp`, `/Volumes/SecondDrive/code2/mark-amp/tests/unit/test_editor_qol.cpp`.
- Add frame-time and input-latency thresholds under each quality tier.
- Acceptance: FX do not regress editing responsiveness, theme switching, or keyboard navigation.

---

**Visual FX You Should Support And Configure**

**Window/Compositor FX**

- `window.blur_background`: radius, sample count, blend strength.
- `window.frosted_glass`: tint color, opacity, saturation boost.
- `window.inner_shadow`: radius, alpha, spread.
- `window.edge_glow`: color, width, alpha, pulse speed.
- `window.vignette`: strength, falloff.
- `window.reflection`: intensity, gradient size.
- `window.chroma_shift`: red/blue offset, intensity.
- `window.scanline_overlay`: line density, alpha.
- `window.noise_grain`: scale, strength, animation speed.

**Transition/Motion FX (Compiz/Beryl style)**

- `transition.wobbly`: stiffness, damping, max displacement.
- `transition.glide`: duration, easing curve, overshoot.
- `transition.zoom_fade`: scale range, opacity curve.
- `transition.magic_lamp`: bend amount, duration.
- `transition.burn_close`: particle density, fade speed.
- `transition.expo_overview`: spacing, blur during overview, animation speed.
- `transition.cube_workspace`: perspective, rotation speed, edge shading.

**Text FX (full renderer support)**

- `text.stroke`: width, color, opacity.
- `text.shadow_soft`: offset x/y, blur, alpha.
- `text.outer_glow`: radius, color, alpha.
- `text.inner_glow`: radius, color, alpha.
- `text.gradient_fill`: start/end colors, angle.
- `text.bloom`: threshold, intensity.
- `text.crt_mode`: scanline alpha, curvature, color bleed.
- `text.subpixel_hinting`: on/off, strength.
- `text.readability_clamp`: min contrast target, halo guard.
- `text.syntax_fx_channel`: separate profiles for keyword/string/comment/type/function.

**UI Element FX**

- `ui.hover_glow`: radius, color, alpha.
- `ui.active_pill_bloom`: strength, corner radius.
- `ui.button_press_depth`: translate amount, shadow reduction.
- `ui.focus_ring_fx`: color, thickness, pulse.
- `ui.panel_drop_shadow`: blur, spread, alpha.
- `ui.tooltip_frost`: blur, tint, border glow.
- `ui.badge_glow`: hue, alpha, pulse.

**Editor/Preview Specific FX**

- `editor.caret_glow`: radius, alpha, color binding.
- `editor.selection_shimmer`: speed, alpha.
- `editor.active_line_glow`: strength, spread.
- `editor.diagnostic_pulse`: per severity color + pulse.
- `preview.heading_glow`: level-based intensity.
- `preview.code_block_aura`: border glow, background tint.
- `preview.mermaid_frame_fx`: border glow, drop shadow.

**Canvas/Object FX**

- `canvas.object_shadow`: blur, alpha, elevation scale.
- `canvas.selection_outline_glow`: color, width, pulse.
- `canvas.connector_neon`: stroke glow intensity.
- `canvas.sticky_note_lighting`: edge highlight, drop shadow.
- `canvas.minimap_heat`: activity tint strength.

**Safety/Accessibility Controls**

- `fx.master_enabled`: global on/off.
- `fx.reduced_motion_override`: force static transitions.
- `fx.low_power_mode`: disables expensive passes.
- `fx.text_safety_mode`: disables distortion/chroma on text.
- `fx.high_contrast_guard`: auto-adjusts colors/effects to target contrast.
- `fx.max_frame_time_budget_ms`: hard cap for adaptive downgrade.

---

**Recommended Default Presets**

- `Compiz Classic`: strong motion, moderate glow, minimal text distortion.
- `Beryl Neon`: strong glow/bloom/chroma, medium motion, safe text clamp enabled.
- `Studio Pro`: subtle motion, subtle glass, high text clarity.
- `Focus Minimal`: almost no motion, no bloom, crisp text only.

If you want, I can provide the exact settings schema (`config` keys + value ranges + defaults) as the next deliverable so AI agents can implement it directly without design ambiguity.
