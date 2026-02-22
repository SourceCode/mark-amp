# Phase 04 -- Motion And Animation Framework

## Objective

Build a centralized motion and animation framework that provides consistent, performant, accessibility-aware animations across the entire MarkAmp UI. Replace the current ad-hoc timer-based animations scattered across individual controls (sidebar collapse, tab fade-in, notification slide-in, splitter hover, save pulse) with a unified system that manages easing curves, transition lifecycles, spring physics, and a global reduced-motion setting.

## Prerequisites

- Phase 01 (Design System Foundation) -- for `DesignTokenRegistry` (density-aware durations), `MotionModel` (existing easing enum and motion tokens).
- Phase 03 (Theme Engine V2) -- for reduced-motion theme flag propagation.

## Deliverables

- `EasingLibrary` -- comprehensive easing function library (cubic-bezier, spring, bounce).
- `AnimationTimeline` -- frame-driven animation runner with interpolation.
- `TransitionManager` -- named transitions with lifecycle hooks.
- `SpringPhysics` -- spring dynamics for natural-feeling interactions.
- `SkeletonLoader` -- loading placeholder rendering.
- Refactored animations in all existing controls.
- Catch2 test target: `test_animation_framework`.

## Estimated Complexity

High -- requires frame-accurate timing, multiple interpolation strategies, and integration across all animated controls.

---

## Tasks

### Task 1: Implement the Easing Function Library

**Description:** Create a comprehensive library of easing functions that map a normalized time input (0.0-1.0) to an output value (0.0-1.0), following standard CSS/Material Design easing specifications.

**Key Implementation Details:**
- Namespace: `markamp::ui::easing`
- Function signature: `using EasingFn = float(*)(float t)`.
- Built-in functions:
  - `linear(t)` -- identity
  - `ease_in_quad(t)` -- t^2
  - `ease_out_quad(t)` -- 1-(1-t)^2
  - `ease_in_out_quad(t)` -- piecewise
  - `ease_in_cubic(t)` -- t^3
  - `ease_out_cubic(t)` -- 1-(1-t)^3
  - `ease_in_out_cubic(t)`
  - `ease_in_expo(t)` -- 2^(10(t-1))
  - `ease_out_expo(t)` -- 1-2^(-10t)
  - `ease_in_out_expo(t)`
  - `ease_out_back(t)` -- overshoot (for bouncy reveals)
  - `ease_out_elastic(t)` -- oscillating overshoot
  - `ease_out_bounce(t)` -- bouncing ball
- Method: `auto cubic_bezier(float x1, float y1, float x2, float y2) -> std::function<float(float)>` -- arbitrary cubic-bezier curves.
- Predefined named curves (matching CSS):
  - `kEase` = cubic_bezier(0.25, 0.1, 0.25, 1.0)
  - `kEaseIn` = cubic_bezier(0.42, 0, 1.0, 1.0)
  - `kEaseOut` = cubic_bezier(0, 0, 0.58, 1.0)
  - `kEaseInOut` = cubic_bezier(0.42, 0, 0.58, 1.0)
  - `kMaterialStandard` = cubic_bezier(0.4, 0.0, 0.2, 1.0)
  - `kMaterialDecelerate` = cubic_bezier(0.0, 0.0, 0.2, 1.0)
  - `kMaterialAccelerate` = cubic_bezier(0.4, 0.0, 1.0, 1.0)

**Files Affected:**
- `src/ui/animation/EasingLibrary.h` (new)
- `src/ui/animation/EasingLibrary.cpp` (new)

**Acceptance Criteria:**
- `linear(0.5)` returns 0.5.
- `ease_out_cubic(0.0)` returns 0.0, `ease_out_cubic(1.0)` returns 1.0.
- `cubic_bezier` approximation is accurate within 0.001 of the true curve.
- All functions are monotonically increasing for standard curves.

**Dependencies:** None.

---

### Task 2: Implement the Animation Value Interpolator

**Description:** Create an interpolation system that can tween between values of different types: float, int, wxColour, wxPoint, wxRect. This decouples "what changes" from "how it changes over time."

**Key Implementation Details:**
- Template function: `template<typename T> auto interpolate(const T& from, const T& to, float t) -> T`.
- Specializations:
  - `float`: linear interpolation `from + (to - from) * t`.
  - `int`: same, rounded to nearest.
  - `wxColour`: per-channel interpolation (R, G, B, A separately).
  - `wxPoint`: interpolate x and y.
  - `wxSize`: interpolate width and height.
  - `wxRect`: interpolate all four components.
- File: `src/ui/animation/Interpolation.h`

**Files Affected:**
- `src/ui/animation/Interpolation.h` (new)

**Acceptance Criteria:**
- `interpolate(0.0f, 10.0f, 0.5f)` returns 5.0f.
- `interpolate(wxColour(0,0,0), wxColour(255,255,255), 0.5f)` returns grey.
- `interpolate(wxRect(0,0,100,100), wxRect(50,50,200,200), 1.0f)` returns the target rect.

**Dependencies:** None.

---

### Task 3: Implement the AnimationTimeline

**Description:** Build the core animation runner that drives animations frame-by-frame using wxTimer. Supports multiple concurrent animations, each with its own duration, easing, and value interpolation.

**Key Implementation Details:**
- Class: `AnimationTimeline` in `src/ui/animation/AnimationTimeline.h`
- Singleton: `static auto get() -> AnimationTimeline&`.
- Struct `AnimationHandle` -- opaque identifier for a running animation.
- Method: `auto animate(float from, float to, int duration_ms, EasingFn easing, std::function<void(float)> on_update, std::function<void()> on_complete = nullptr) -> AnimationHandle`.
- Method: `void cancel(AnimationHandle handle)`.
- Method: `void cancel_all()`.
- Method: `auto is_running(AnimationHandle handle) const -> bool`.
- Method: `auto running_count() const -> int`.
- Internal:
  - Single wxTimer running at 60fps (16ms interval) when any animation is active.
  - Timer auto-starts when first animation is added, auto-stops when all complete.
  - Each animation stores: start_time, duration, easing function, current value, update callback, complete callback.
- Reduced motion: if `ThemeEngine::is_reduced_motion()`, all durations are set to 0 (instant snap).

**Files Affected:**
- `src/ui/animation/AnimationTimeline.h` (new)
- `src/ui/animation/AnimationTimeline.cpp` (new)

**Acceptance Criteria:**
- Starting an animation and waiting its duration calls on_complete.
- Canceling an animation stops update callbacks.
- With reduced motion on, on_update is called once with the final value, then on_complete.
- Timer stops when no animations are running.

**Dependencies:** Tasks 1, 2.

---

### Task 4: Implement Spring Physics Engine

**Description:** Create a spring dynamics simulator for natural-feeling animations that overshoot, oscillate, and settle. Used for panel reveals, drag release, and scroll physics.

**Key Implementation Details:**
- Class: `SpringPhysics` in `src/ui/animation/SpringPhysics.h`
- Constructor: `SpringPhysics(float stiffness, float damping, float mass)`.
- Method: `void set_target(float target)` -- set the target position.
- Method: `auto step(float dt_seconds) -> float` -- advance simulation, return current position.
- Method: `auto velocity() const -> float`.
- Method: `auto at_rest() const -> bool` -- true when velocity < threshold and position is within epsilon of target.
- Method: `void snap_to(float value)` -- instantly set position without animation.
- Presets:
  - `SpringPhysics::gentle()` -- stiffness=170, damping=26, mass=1 (slow, no overshoot)
  - `SpringPhysics::responsive()` -- stiffness=300, damping=20, mass=1 (fast with slight overshoot)
  - `SpringPhysics::bouncy()` -- stiffness=200, damping=10, mass=1 (visible oscillation)
  - `SpringPhysics::stiff()` -- stiffness=400, damping=30, mass=1 (very fast, minimal overshoot)

**Files Affected:**
- `src/ui/animation/SpringPhysics.h` (new)
- `src/ui/animation/SpringPhysics.cpp` (new)

**Acceptance Criteria:**
- `SpringPhysics::responsive()` with target=100.0 reaches within 1.0 of target in under 60 steps at 60fps.
- `at_rest()` returns true when settled.
- `snap_to()` immediately sets position and velocity to zero.
- `bouncy()` preset visibly overshoots (position exceeds target during simulation).

**Dependencies:** None.

---

### Task 5: Implement the TransitionManager

**Description:** Create a named transition system for complex multi-property animations. A "transition" is a named, reusable animation configuration that can be applied to any control, managing show/hide, expand/collapse, and state change animations.

**Key Implementation Details:**
- Class: `TransitionManager` in `src/ui/animation/TransitionManager.h`
- Struct: `TransitionConfig { std::string name; int duration_ms; EasingFn easing; int delay_ms; }`.
- Named presets:
  - `"panel-show"` -- 250ms, ease_out_cubic, 0 delay
  - `"panel-hide"` -- 200ms, ease_in_cubic, 0 delay
  - `"sidebar-expand"` -- 300ms, material_standard, 0 delay
  - `"sidebar-collapse"` -- 200ms, material_accelerate, 0 delay
  - `"tab-close"` -- 150ms, ease_out_quad, 0 delay
  - `"notification-enter"` -- 300ms, ease_out_back, 0 delay
  - `"notification-exit"` -- 200ms, ease_in_quad, 0 delay
  - `"fade-in"` -- 200ms, ease_out_quad
  - `"fade-out"` -- 150ms, ease_in_quad
  - `"scale-up"` -- 200ms, ease_out_back (for button press release)
  - `"skeleton-pulse"` -- 1500ms, ease_in_out_quad, looping
- Method: `auto get(const std::string& name) const -> TransitionConfig`.
- Method: `void register_transition(const TransitionConfig& config)`.
- Method: `auto start(const std::string& name, float from, float to, std::function<void(float)> update, std::function<void()> complete = nullptr) -> AnimationHandle`.
- Delegates to `AnimationTimeline` for execution.

**Files Affected:**
- `src/ui/animation/TransitionManager.h` (new)
- `src/ui/animation/TransitionManager.cpp` (new)

**Acceptance Criteria:**
- `start("panel-show", 0.0, 1.0, cb)` animates over 250ms with ease_out_cubic.
- Registered custom transitions can be started by name.
- Transitions respect reduced-motion setting (instant snap).

**Dependencies:** Tasks 1, 3.

---

### Task 6: Implement Skeleton Loading States

**Description:** Create a skeleton loading component that renders animated placeholder content while real content loads. Used for sidebar panels, extension browser, and any panel with async data loading.

**Key Implementation Details:**
- Class: `SkeletonRenderer` in `src/ui/animation/SkeletonRenderer.h`
- Method: `void draw_line(wxGraphicsContext& gc, const wxRect& bounds, const core::Theme& theme)` -- renders a single skeleton line (rounded rect with shimmer gradient).
- Method: `void draw_card(wxGraphicsContext& gc, const wxRect& bounds, const core::Theme& theme)` -- renders a card placeholder (rectangle with icon placeholder, text lines).
- Method: `void draw_tree(wxGraphicsContext& gc, const wxRect& bounds, int row_count, const core::Theme& theme)` -- renders tree-like skeleton rows.
- Shimmer animation: a horizontal gradient sweep from left to right, looping.
- Shimmer uses `TransitionManager::start("skeleton-pulse", ...)`.
- Colors: base = `ThemeColorToken::BgPanel`, shimmer highlight = `ThemeColorToken::HoverBg`.

**Files Affected:**
- `src/ui/animation/SkeletonRenderer.h` (new)
- `src/ui/animation/SkeletonRenderer.cpp` (new)

**Acceptance Criteria:**
- `draw_line` renders a rounded rectangle with animated shimmer.
- `draw_tree` renders multiple rows with staggered indent.
- Reduced motion: shimmer is static (no animation), just the base color.

**Dependencies:** Tasks 3, 5.

---

### Task 7: Refactor Sidebar Collapse/Expand Animation

**Description:** Replace the current ad-hoc sidebar animation in `LayoutManager` (using `sidebar_anim_timer_`, `sidebar_anim_progress_`, manual easing) with the `TransitionManager`.

**Key Implementation Details:**
- Remove: `sidebar_anim_timer_`, `sidebar_anim_start_width_`, `sidebar_anim_target_width_`, `sidebar_anim_progress_`, `sidebar_anim_showing_`, `OnSidebarAnimTimer`.
- Replace with: `TransitionManager::start("sidebar-expand", ...)` or `start("sidebar-collapse", ...)`.
- The update callback calls `UpdateSidebarSize(interpolated_width)` and `Refresh()`.
- The complete callback finalizes the size and layout.
- This reduces ~40 lines of animation boilerplate to ~5 lines.

**Files Affected:**
- `src/ui/LayoutManager.h` (modify -- remove animation state members)
- `src/ui/LayoutManager.cpp` (modify -- replace animation code)

**Acceptance Criteria:**
- Sidebar expand/collapse looks identical to current behavior.
- No `sidebar_anim_timer_` or related state remains.
- Reduced motion: sidebar snaps instantly.

**Dependencies:** Task 5.

---

### Task 8: Refactor Tab Fade-In Animation

**Description:** Replace the ad-hoc `fade_timer_` in `TabBar` with the animation framework.

**Key Implementation Details:**
- Remove: `fade_timer_`, `OnFadeTimer`, per-tab `opacity` field manual management.
- When adding a new tab, start: `TransitionManager::start("fade-in", 0.0, 1.0, [tab_index](float v) { tabs_[index].opacity = v; Refresh(); })`.
- Tab close animation: `TransitionManager::start("tab-close", ...)` animating width to 0 before removing the tab.

**Files Affected:**
- `src/ui/TabBar.h` (modify -- remove fade_timer_)
- `src/ui/TabBar.cpp` (modify -- use TransitionManager)

**Acceptance Criteria:**
- New tabs fade in smoothly.
- Closing tabs shrink before disappearing.
- No manual timer management in TabBar.

**Dependencies:** Task 5.

---

### Task 9: Refactor Notification Slide-In/Fade-Out

**Description:** Replace the ad-hoc `animation_timer_` in `NotificationManager` with the animation framework.

**Key Implementation Details:**
- Remove: `animation_timer_`, `OnAnimationTimer`, manual opacity/elapsed tracking.
- Enter animation: `TransitionManager::start("notification-enter", 0.0, 1.0, ...)` driving opacity.
- Exit animation: `TransitionManager::start("notification-exit", 1.0, 0.0, ...)` driving opacity.
- Slide-in: combine opacity and y-position animation (animate from +20px offset to 0).
- Auto-dismiss after `duration_ms` by scheduling a delayed exit animation.

**Files Affected:**
- `src/ui/NotificationManager.h` (modify -- remove animation_timer_)
- `src/ui/NotificationManager.cpp` (modify)

**Acceptance Criteria:**
- Notifications slide in from the bottom-right with opacity fade.
- Notifications fade out when dismissed.
- Auto-dismiss timing still works correctly.

**Dependencies:** Task 5.

---

### Task 10: Refactor Splitter Bar Hover Animation

**Description:** Replace the ad-hoc `hover_timer_` in `SplitterBar` with the animation framework.

**Key Implementation Details:**
- Remove: `hover_timer_`, `OnHoverTimer`, `hover_alpha_` manual management.
- On mouse enter: `AnimationTimeline::animate(0.0, 1.0, 150, ease_out_quad, [this](float v) { hover_alpha_ = v; Refresh(); })`.
- On mouse leave: `AnimationTimeline::animate(current_alpha, 0.0, 100, ease_in_quad, ...)`.

**Files Affected:**
- `src/ui/SplitterBar.h` (modify -- remove hover_timer_)
- `src/ui/SplitterBar.cpp` (modify)

**Acceptance Criteria:**
- Hover alpha animates smoothly on enter/leave.
- No dedicated timer in SplitterBar.

**Dependencies:** Task 3.

---

### Task 11: Refactor Toolbar Save Button Pulse

**Description:** Replace the ad-hoc `save_pulse_timer_` and `save_pulse_scale_` in `Toolbar` with the animation framework.

**Key Implementation Details:**
- Remove: `save_pulse_timer_`, `save_pulse_scale_`, `save_flash_timer_`.
- On save: `AnimationTimeline::animate(1.0, 1.2, 150, ease_out_back, [this](float v) { save_pulse_scale_ = v; Refresh(); }, [this]() { AnimationTimeline::animate(1.2, 1.0, 150, ease_out_quad, ...); })`.
- Chain: scale up to 1.2x, then back to 1.0x (two-phase animation).

**Files Affected:**
- `src/ui/Toolbar.h` (modify)
- `src/ui/Toolbar.cpp` (modify)

**Acceptance Criteria:**
- Save button pulses on save action.
- No dedicated timers in Toolbar for animation.

**Dependencies:** Task 3.

---

### Task 12: Refactor StatusBar Save Flash

**Description:** Replace the ad-hoc `save_flash_timer_` in `StatusBarPanel` with the animation framework.

**Key Implementation Details:**
- Remove: `save_flash_timer_`, `save_flash_active_` manual management.
- On save event: `AnimationTimeline::animate(1.0, 0.0, 500, ease_out_quad, [this](float v) { flash_opacity_ = v; Refresh(); })`.

**Files Affected:**
- `src/ui/StatusBarPanel.h` (modify)
- `src/ui/StatusBarPanel.cpp` (modify)

**Acceptance Criteria:**
- Save flash fades out smoothly.
- No dedicated timer for save flash.

**Dependencies:** Task 3.

---

### Task 13: Refactor Sidebar Transition Alpha

**Description:** Replace the ad-hoc `sidebar_transition_timer_` and `sidebar_transition_alpha_` in `LayoutManager` with the animation framework.

**Key Implementation Details:**
- Remove: `sidebar_transition_timer_`, `sidebar_transition_alpha_`, `sidebar_transition_active_`.
- When switching sidebar panels: `TransitionManager::start("fade-out", 1.0, 0.0, ..., [this]() { swap_panel(); TransitionManager::start("fade-in", 0.0, 1.0, ...); })`.
- Cross-fade between old and new sidebar panel content.

**Files Affected:**
- `src/ui/LayoutManager.h` (modify)
- `src/ui/LayoutManager.cpp` (modify)

**Acceptance Criteria:**
- Switching sidebar modes shows a smooth cross-fade transition.
- No dedicated timer for sidebar panel transitions.

**Dependencies:** Task 5.

---

### Task 14: Refactor StatusBar Progress Spinner

**Description:** Replace the ad-hoc `progress_spinner_timer_` in `StatusBarPanel` with the animation framework.

**Key Implementation Details:**
- Remove: `progress_spinner_timer_`, `spinner_frame_` manual management.
- Use a looping animation: `AnimationTimeline::animate(0.0, 360.0, 1000, linear, [this](float v) { spinner_angle_ = v; Refresh(); })` with repeat.
- Add repeat support to `AnimationTimeline`: `auto animate_loop(...)` -- automatically restarts when complete.

**Files Affected:**
- `src/ui/animation/AnimationTimeline.h` (modify -- add animate_loop)
- `src/ui/animation/AnimationTimeline.cpp` (modify)
- `src/ui/StatusBarPanel.h` (modify)
- `src/ui/StatusBarPanel.cpp` (modify)

**Acceptance Criteria:**
- Spinner rotates smoothly at 1 revolution per second.
- Stopping progress stops the animation.
- animate_loop API works for any looping animation.

**Dependencies:** Task 3.

---

### Task 15: Implement Panel Show/Hide Animations

**Description:** Add slide-in/slide-out animations for bottom panel show/hide and sidebar show/hide, going beyond the current width animation to include opacity and position offset.

**Key Implementation Details:**
- Bottom panel show: slide up from below (y-offset animation) + fade in.
- Bottom panel hide: slide down + fade out.
- Duration: 250ms show, 200ms hide, using material_standard easing.
- Combine two AnimationTimeline animations (opacity + position) running concurrently.
- The `LayoutManager::ShowBottomPanel(bool)` method triggers the transition.

**Files Affected:**
- `src/ui/LayoutManager.cpp` (modify -- add animation to ShowBottomPanel)

**Acceptance Criteria:**
- Bottom panel slides up when shown.
- Bottom panel slides down when hidden.
- Reduced motion: instant show/hide.

**Dependencies:** Tasks 3, 5.

---

### Task 16: Implement CommandPalette Open/Close Animation

**Description:** Add a reveal animation for the Command Palette: scale-up from 95% + fade in when opening, scale-down + fade out when closing.

**Key Implementation Details:**
- In `CommandPalette` show: animate opacity 0->1 and scale 0.95->1.0 over 150ms with ease_out_cubic.
- In `CommandPalette` hide: animate opacity 1->0 and scale 1.0->0.95 over 100ms with ease_in_quad.
- Scale is applied via `wxWindow::SetSize()` with centered positioning adjustment.

**Files Affected:**
- `src/ui/CommandPalette.h` (modify -- add animation state)
- `src/ui/CommandPalette.cpp` (modify -- add show/hide animations)

**Acceptance Criteria:**
- Command palette smoothly appears and disappears.
- Focus management still works correctly with animated show.
- Reduced motion: instant show/hide.

**Dependencies:** Task 3.

---

### Task 17: Implement Tooltip Fade Animations

**Description:** Add consistent fade-in/fade-out animations for all tooltips across the application (activity bar, toolbar, status bar).

**Key Implementation Details:**
- Remove per-control tooltip timers (`tooltip_timer_` in ActivityBar, `tooltip_delay_timer_` in Toolbar).
- Create `TooltipAnimator` utility in `src/ui/animation/TooltipAnimator.h`.
- Method: `void show_after_delay(int delay_ms, std::function<void()> show_fn)`.
- Method: `void fade_in(wxWindow* tooltip, int duration_ms)`.
- Method: `void fade_out(wxWindow* tooltip, int duration_ms, std::function<void()> on_complete)`.
- Standard delay: 500ms before showing, 150ms fade-in, 100ms fade-out.

**Files Affected:**
- `src/ui/animation/TooltipAnimator.h` (new)
- `src/ui/animation/TooltipAnimator.cpp` (new)
- `src/ui/ActivityBar.h` / `.cpp` (modify -- use TooltipAnimator)
- `src/ui/Toolbar.h` / `.cpp` (modify -- use TooltipAnimator)

**Acceptance Criteria:**
- Tooltips appear with a delay and fade in smoothly.
- Moving the mouse away before the delay cancels the tooltip.
- All controls use the same tooltip timing.

**Dependencies:** Task 3.

---

### Task 18: Implement Scroll Animation Utility

**Description:** Create a reusable scroll animation utility for smooth scrolling in panels, trees, and the editor. Supports both programmatic scroll-to and momentum-based scroll physics.

**Key Implementation Details:**
- Class: `ScrollAnimator` in `src/ui/animation/ScrollAnimator.h`
- Method: `void scroll_to(int target_position, int duration_ms = 300)` -- smooth scroll to position.
- Method: `void scroll_by(int delta, int duration_ms = 200)` -- smooth scroll by offset.
- Method: `void apply_momentum(float velocity)` -- physics-based deceleration.
- Uses `SpringPhysics::gentle()` for momentum deceleration.
- Integrates with `wxScrolledWindow::Scroll()` or raw position management.

**Files Affected:**
- `src/ui/animation/ScrollAnimator.h` (new)
- `src/ui/animation/ScrollAnimator.cpp` (new)

**Acceptance Criteria:**
- `scroll_to` smoothly animates to the target position.
- Momentum scrolling decelerates naturally and stops.
- Reduced motion: instant jump to target.

**Dependencies:** Task 4.

---

### Task 19: Implement Focus Ring Animation

**Description:** Create an animated focus ring that smoothly transitions between focused controls. Instead of a static focus ring that jumps, the ring morphs its shape and position to the newly focused element.

**Key Implementation Details:**
- Class: `FocusRingAnimator` in `src/ui/animation/FocusRingAnimator.h`
- Method: `void move_to(const wxRect& new_bounds)` -- animates focus ring from current position to new position.
- Method: `void show(const wxRect& bounds)` -- fade in focus ring at position.
- Method: `void hide()` -- fade out focus ring.
- Focus ring is drawn as a rounded rectangle with theme-colored border (`ThemeColorToken::FocusRingColor`).
- Animation: 150ms, ease_out_quad for position + size interpolation.
- Renders as an overlay (does not affect child layout).
- Width: 2px, corner radius: 4px.

**Files Affected:**
- `src/ui/animation/FocusRingAnimator.h` (new)
- `src/ui/animation/FocusRingAnimator.cpp` (new)

**Acceptance Criteria:**
- Focus ring smoothly transitions between focused elements.
- Focus ring appears and disappears with fade animation.
- Ring color follows the theme token.

**Dependencies:** Tasks 2, 3.

---

### Task 20: Implement Progress Bar Animation

**Description:** Create a reusable animated progress bar component for indeterminate and determinate progress states.

**Key Implementation Details:**
- Class: `AnimatedProgressBar` in `src/ui/animation/AnimatedProgressBar.h`
- Determinate mode: bar fills from left to right with smooth transitions between values.
- Indeterminate mode: a pulsing gradient segment sweeps left to right continuously.
- Method: `void set_progress(float value)` -- 0.0 to 1.0, animates to new position.
- Method: `void set_indeterminate(bool indeterminate)`.
- Height: 4px (compact), configurable.
- Uses theme accent color for the filled portion.

**Files Affected:**
- `src/ui/animation/AnimatedProgressBar.h` (new)
- `src/ui/animation/AnimatedProgressBar.cpp` (new)

**Acceptance Criteria:**
- Setting progress from 0.3 to 0.7 smoothly fills the bar.
- Indeterminate mode shows a sweeping pulse animation.
- Reduced motion: instant value changes, static indeterminate bar.

**Dependencies:** Tasks 1, 3.

---

### Task 21: Global Reduced-Motion Setting

**Description:** Wire the reduced-motion accessibility setting throughout the entire animation framework. When enabled, all animations snap to their final state instantly.

**Key Implementation Details:**
- `AnimationTimeline`: check `ThemeEngine::is_reduced_motion()` before starting; if true, call on_update(final_value) + on_complete immediately.
- `SpringPhysics`: `snap_to(target)` immediately.
- `TransitionManager`: all transitions resolve instantly.
- `SkeletonRenderer`: show static placeholders (no shimmer).
- Expose via Command Palette: "View: Toggle Reduced Motion".
- Persist in Config under `ui.reduced_motion`.
- Respect OS-level reduced-motion setting (macOS: `NSWorkspace.shared.accessibilityDisplayShouldReduceMotion`).

**Files Affected:**
- `src/ui/animation/AnimationTimeline.cpp` (modify)
- `src/ui/animation/TransitionManager.cpp` (modify)
- `src/platform/MacPlatform.h` / `.cpp` (modify -- query OS reduced-motion)
- `src/ui/MainFrame.cpp` (modify -- register command)

**Acceptance Criteria:**
- With reduced motion on, no visual animation occurs anywhere in the UI.
- OS-level reduced-motion preference is respected on first launch.
- Toggle command updates the setting immediately.

**Dependencies:** Tasks 3, 4, 5, 6.

---

### Task 22: Animation Performance Budget

**Description:** Implement a frame budget monitor that ensures animations do not drop below 60fps. If frame rendering exceeds the budget, animations are automatically simplified (fewer concurrent animations, lower frame rate fallback).

**Key Implementation Details:**
- In `AnimationTimeline`, measure the time taken for each frame's callbacks.
- If frame time exceeds 16ms for 3 consecutive frames:
  1. Reduce animation frame rate to 30fps (32ms interval).
  2. Log a `MARKAMP_LOG_WARN`.
  3. If still exceeding at 30fps, skip to final values for lowest-priority animations.
- Priority system: `AnimationPriority` enum: `kCritical` (focus ring, user interaction), `kNormal` (panel transitions), `kCosmetic` (shimmer, glow effects).
- Cosmetic animations are dropped first under budget pressure.

**Files Affected:**
- `src/ui/animation/AnimationTimeline.h` (modify -- add priority and budget)
- `src/ui/animation/AnimationTimeline.cpp` (modify)

**Acceptance Criteria:**
- Under normal load, animations run at 60fps.
- Under heavy load, cosmetic animations are dropped before critical ones.
- Budget warnings are logged.

**Dependencies:** Task 3.

---

### Task 23: Motion Token Registry Integration

**Description:** Connect the existing `MotionModel` (from the current codebase) to the new animation framework, ensuring motion tokens defined in `MotionModel` drive the `TransitionManager` configurations.

**Key Implementation Details:**
- `TransitionManager` reads default durations and easing curves from `MotionModel::token_for(context)`.
- `MotionModel` tokens override the hard-coded presets in `TransitionManager`.
- Theme files can define motion tokens in YAML frontmatter under `motion:` section.
- This allows themes to customize animation timing (e.g., a "fast" theme with shorter durations).

**Files Affected:**
- `src/ui/animation/TransitionManager.cpp` (modify -- read from MotionModel)
- `src/ui/MotionModel.h` (modify -- add theme loading)

**Acceptance Criteria:**
- Changing `MotionModel` token for `kReveal` changes the panel show duration.
- A theme with `motion: { reveal_duration_ms: 100 }` produces faster panel shows.

**Dependencies:** Tasks 3, 5.

---

### Task 24: Staggered Animation Utility

**Description:** Create a utility for staggered animations where multiple items animate sequentially with a delay between each (e.g., file tree items appearing one by one, extension cards revealing in sequence).

**Key Implementation Details:**
- Free function: `void stagger(int item_count, int stagger_delay_ms, int item_duration_ms, EasingFn easing, std::function<void(int index, float progress)> on_update, std::function<void()> on_all_complete)`.
- Each item starts `stagger_delay_ms` after the previous item.
- Example: 10 items, 50ms stagger, 200ms duration = first item finishes at 200ms, last item finishes at 650ms.
- Used for: file tree reveal, extension browser grid, search results appearing.

**Files Affected:**
- `src/ui/animation/StaggeredAnimation.h` (new)
- `src/ui/animation/StaggeredAnimation.cpp` (new)

**Acceptance Criteria:**
- 5 items with 50ms stagger start at t=0, t=50, t=100, t=150, t=200.
- on_all_complete fires after the last item finishes.
- Reduced motion: all items snap to final state simultaneously.

**Dependencies:** Task 3.

---

### Task 25: Catch2 Test Suite for Animation Framework

**Description:** Write comprehensive unit tests for all animation framework components.

**Key Implementation Details:**
- Test file: `tests/unit/test_animation_framework.cpp`
- Test target: `test_animation_framework` in CMakeLists.txt
- Test sections:
  - `EasingLibrary`: boundary values (0.0 and 1.0), monotonicity, cubic_bezier accuracy.
  - `Interpolation`: float, int, wxColour, wxRect interpolation.
  - `SpringPhysics`: convergence to target, at_rest detection, snap_to.
  - `TransitionManager`: named preset lookup, custom registration.
  - `AnimationTimeline`: start/cancel/is_running (mock time advancement).
  - `SkeletonRenderer`: draw calls produce no crash.
  - `StaggeredAnimation`: correct stagger timing.
  - Reduced motion: all animations resolve instantly.
- Note: Testing real-time animations requires mock timers; advance time manually.

**Files Affected:**
- `tests/unit/test_animation_framework.cpp` (new)
- `CMakeLists.txt` (modify)

**Acceptance Criteria:**
- All tests pass.
- At least 40 test assertions.
- No dependency on wxWidgets event loop for timing tests.

**Dependencies:** All previous tasks.

---

## Dependency Graph

```
Task 1 (EasingLibrary) ───────────────┐
Task 2 (Interpolation) ───────────────┤
Task 3 (AnimationTimeline) ── 1, 2 ───┤
Task 4 (SpringPhysics) ───────────────┤
Task 5 (TransitionManager) ── 1, 3 ───┤
Task 6 (SkeletonRenderer) ── 3, 5 ────┤
Task 7 (Sidebar collapse) ── 5 ───────┤
Task 8 (Tab fade-in) ── 5 ────────────┤
Task 9 (Notification) ── 5 ───────────┤
Task 10 (Splitter hover) ── 3 ────────┤
Task 11 (Save pulse) ── 3 ────────────┤
Task 12 (Save flash) ── 3 ────────────┤
Task 13 (Sidebar transition) ── 5 ────┤
Task 14 (Progress spinner) ── 3 ──────┤
Task 15 (Panel show/hide) ── 3, 5 ────┤
Task 16 (CommandPalette) ── 3 ────────┤
Task 17 (Tooltips) ── 3 ──────────────┤
Task 18 (ScrollAnimator) ── 4 ────────┤
Task 19 (FocusRing) ── 2, 3 ──────────┤
Task 20 (ProgressBar) ── 1, 3 ────────┤
Task 21 (Reduced motion) ── 3,4,5,6 ──┤
Task 22 (Performance budget) ── 3 ────┤
Task 23 (MotionToken) ── 3, 5 ────────┤
Task 24 (Stagger) ── 3 ───────────────┤
Task 25 (Tests) ── all ───────────────┘
```

## Files Created

| File | Type |
|------|------|
| `src/ui/animation/EasingLibrary.h` / `.cpp` | Easing functions |
| `src/ui/animation/Interpolation.h` | Value interpolation |
| `src/ui/animation/AnimationTimeline.h` / `.cpp` | Animation runner |
| `src/ui/animation/SpringPhysics.h` / `.cpp` | Spring dynamics |
| `src/ui/animation/TransitionManager.h` / `.cpp` | Named transitions |
| `src/ui/animation/SkeletonRenderer.h` / `.cpp` | Loading placeholders |
| `src/ui/animation/TooltipAnimator.h` / `.cpp` | Tooltip animations |
| `src/ui/animation/ScrollAnimator.h` / `.cpp` | Smooth scrolling |
| `src/ui/animation/FocusRingAnimator.h` / `.cpp` | Focus ring animation |
| `src/ui/animation/AnimatedProgressBar.h` / `.cpp` | Progress bar |
| `src/ui/animation/StaggeredAnimation.h` / `.cpp` | Stagger utility |
| `tests/unit/test_animation_framework.cpp` | Test file |

## Files Modified

| File | Change |
|------|--------|
| `src/ui/LayoutManager.h` / `.cpp` | Remove 3 ad-hoc timers, use TransitionManager |
| `src/ui/TabBar.h` / `.cpp` | Remove fade_timer_, use TransitionManager |
| `src/ui/NotificationManager.h` / `.cpp` | Remove animation_timer_ |
| `src/ui/SplitterBar.h` / `.cpp` | Remove hover_timer_ |
| `src/ui/Toolbar.h` / `.cpp` | Remove save_pulse/flash timers |
| `src/ui/StatusBarPanel.h` / `.cpp` | Remove save_flash_timer_, spinner_timer_ |
| `src/ui/CommandPalette.h` / `.cpp` | Add show/hide animations |
| `src/ui/ActivityBar.h` / `.cpp` | Remove tooltip_timer_ |
| `src/ui/MotionModel.h` | Add theme loading |
| `src/platform/MacPlatform.h` / `.cpp` | Query OS reduced-motion |
| `src/ui/MainFrame.cpp` | Register reduced-motion command |
| `CMakeLists.txt` | New sources and test target |
