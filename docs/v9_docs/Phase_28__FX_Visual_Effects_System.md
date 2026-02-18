# Phase 28: FX Visual Effects System

## Overview
FxEngine (109 lines), FxPass, FxMotionPreset, FxPresetRegistry, FxSafetyController, and TextFxRenderer exist in the rendering layer. The FX system defines quality tiers, master toggle, and per-surface targeting but effects are not wired to user-visible interactions. This phase completes the visual effects system for polished UI interactions.

## Prerequisites
- Phase 03 (Theme engine for effect color integration)
- Phase 07 (Editor for text effects)
- Phase 11 (Canvas for canvas effects)

## Tasks

### Task 1: Wire FxEngine Compositor Pipeline
**Files:** `src/rendering/FxEngine.cpp`, `src/rendering/FxEngine.h`
**Description:** FxEngine has quality tiers and master toggle. Wire the compositor pipeline: collect passes, sort by order, composite results, output final frame.
**Acceptance Criteria:**
- `compose()` method processes all active passes
- Passes sorted by order (lower runs first)
- Each pass receives previous output as input
- Master toggle enables/disables all effects
- Quality tier: Off, Low, Medium, High
- `FxFrameResult` returned with timing data

### Task 2: Wire FxMotionPreset Registry
**Files:** `src/rendering/FxMotionPreset.cpp`, `src/rendering/FxPresetRegistry.cpp`
**Description:** FxMotionPreset and FxPresetRegistry exist. Wire preset registration and lookup: built-in presets for common transitions (fade, slide, scale, blur).
**Acceptance Criteria:**
- Built-in presets: fade-in, fade-out, slide-left, slide-right, scale-up, scale-down, blur-in, blur-out
- Presets registered in FxPresetRegistry on init
- Preset lookup by name
- Custom presets configurable
- Preset parameters: duration, easing, delay
- `FxPresetLoadedEvent` emitted on registration

### Task 3: Wire TextFxRenderer for Editor Effects
**Files:** `src/rendering/TextFxRenderer.cpp`, `src/ui/EditorPanel.cpp`
**Description:** TextFxRenderer exists. Wire it for editor text effects: cursor glow, selection highlight animation, focus line highlight, typing ripple.
**Acceptance Criteria:**
- Cursor glow: subtle glow around cursor (configurable color)
- Selection highlight: animated selection boundary
- Focus line: current line subtly highlighted with fade
- Typing ripple: subtle pulse on keystroke (optional)
- All effects respect quality tier
- Effects disabled at "Off" quality

### Task 4: Wire Panel Transition Effects
**Files:** `src/rendering/FxEngine.cpp`, `src/ui/LayoutManager.cpp`
**Description:** Panel open/close/resize transitions: panels slide in/out with configurable duration. Tab switch fades content.
**Acceptance Criteria:**
- Panel open: slide from edge (200ms default)
- Panel close: slide to edge (150ms default)
- Panel resize: smooth interpolation
- Tab switch: cross-fade (100ms)
- Duration configurable in settings
- Transitions disabled at "Off" quality

### Task 5: Wire FxSafetyController for Performance Protection
**Files:** `src/rendering/FxSafetyController.cpp`
**Description:** FxSafetyController exists. Wire it to monitor frame times and automatically reduce quality tier when frame budget is exceeded.
**Acceptance Criteria:**
- Frame time monitored per frame
- If frame time > 16ms for 5 consecutive frames: reduce quality
- If frame time > 32ms: disable effects temporarily
- Restore quality when performance recovers
- Quality reduction events logged
- Manual override: force quality tier

### Task 6: Wire Surface Transition Effects
**Files:** `src/ui/SurfaceTransitionCoordinator.cpp`, `src/rendering/FxEngine.cpp`
**Description:** Surface transitions (editor to canvas, canvas to graph) use FX: cross-fade or slide transition between surfaces.
**Acceptance Criteria:**
- Surface switch: cross-fade (200ms)
- Slide transition option for directional navigation
- Transition captures snapshot of outgoing surface
- Incoming surface fades in over snapshot
- Transition cancelable (instant switch)
- Per-surface transition style configurable

### Task 7: Wire Canvas Object Effects
**Files:** `src/canvas/CanvasRenderer.cpp`, `src/rendering/FxEngine.cpp`
**Description:** Canvas objects have subtle effects: hover highlight, selection glow, creation animation, deletion fade.
**Acceptance Criteria:**
- Object hover: subtle brightness increase
- Object select: glow effect with accent color
- Object create: scale-up animation (150ms)
- Object delete: fade-out animation (100ms)
- Object drag: slight shadow/lift effect
- Effects scale with zoom level

### Task 8: Wire Scroll Effects
**Files:** `src/ui/EditorPanel.cpp`, `src/ui/PreviewPanel.cpp`
**Description:** Smooth scrolling with momentum. Scroll-to-target animation when navigating to specific positions (e.g., search result, heading).
**Acceptance Criteria:**
- Smooth scroll: momentum-based scroll with deceleration
- Scroll-to-target: animated scroll to position (300ms)
- Overscroll bounce: subtle bounce at scroll boundaries
- Scroll indicator fade: scrollbar fades when idle
- Performance: scroll effects maintain 60fps
- Disable smooth scroll option in settings

### Task 9: Wire Notification Effects
**Files:** `src/ui/NotificationManager.cpp`, `src/rendering/FxEngine.cpp`
**Description:** Notifications have entrance/exit effects: slide in from right, auto-dismiss with fade out.
**Acceptance Criteria:**
- Notification entrance: slide from right edge (200ms)
- Auto-dismiss: fade out after timeout
- Stack: notifications stack vertically with slide
- Dismiss: swipe or slide out
- Priority: error notifications shake effect
- Effect timing: entrance 200ms, exit 150ms

### Task 10: Wire Theme Transition Effects
**Files:** `src/core/ThemeEngine.cpp`, `src/rendering/FxEngine.cpp`
**Description:** Theme changes animate: background color cross-fades, text color transitions, accent color morphs.
**Acceptance Criteria:**
- Theme switch: all colors cross-fade (300ms)
- No flash of unstyled content
- Individual elements transition at same rate
- Dark-to-light and light-to-dark smooth
- Disabled at "Off" quality
- Instant switch option in settings

### Task 11: Wire Loading and Progress Effects
**Files:** `src/rendering/FxEngine.cpp`, `src/ui/StatusBarPanel.cpp`
**Description:** Loading indicators with animated effects: progress bar with shimmer, indeterminate loading with pulse, skeleton screens for loading content.
**Acceptance Criteria:**
- Progress bar: animated fill with shimmer highlight
- Indeterminate: pulsing bar animation
- Skeleton screen: placeholder blocks with shimmer
- Loading spinner: rotating indicator
- All loading effects respect quality tier
- Immediate fallback when effects disabled

### Task 12: Wire Focus and Blur Effects
**Files:** `src/rendering/FxEngine.cpp`, `src/ui/LayoutManager.cpp`
**Description:** Window focus/blur effects: inactive window slightly dims. Panel focus: active panel has full brightness, inactive panels slightly reduced.
**Acceptance Criteria:**
- Window blur: slight dim (95% brightness)
- Panel focus: active panel full brightness
- Inactive panels: slight dim (97% brightness)
- Focus transition: smooth (100ms)
- Modal overlay: background dims (80% brightness)
- Disabled at "Off" quality

### Task 13: Wire Micro-Interaction Effects
**Files:** `src/rendering/FxEngine.cpp`, `src/ui/Toolbar.cpp`
**Description:** Button and control micro-interactions: button press scale, toggle switch slide, checkbox check animation, dropdown open animation.
**Acceptance Criteria:**
- Button press: slight scale-down (98%, 50ms)
- Toggle switch: slide animation (100ms)
- Checkbox: check mark draw animation (80ms)
- Dropdown: expand animation (100ms)
- Hover: subtle brightness change
- All interactions respect quality tier

### Task 14: Wire FX Settings UI
**Files:** `src/ui/SettingsPanel.cpp`, `src/core/Config.h`
**Description:** FX settings in preferences: master toggle, quality tier, individual effect toggles, custom durations.
**Acceptance Criteria:**
- Master toggle: enable/disable all effects
- Quality tier: Off, Low, Medium, High
- Individual toggles: text effects, transitions, canvas effects, scroll
- Duration multiplier: 0.5x to 2.0x
- "Reduce Motion" system setting respected
- Preview effects in settings panel

### Task 15: Wire FX Performance Metrics
**Files:** `src/rendering/FxEngine.cpp`, `src/core/OutputChannelService.cpp`
**Description:** Track FX performance: per-pass timing, total compositor time, quality tier changes, frame drops.
**Acceptance Criteria:**
- Per-pass timing logged in debug mode
- Total compositor time per frame
- Quality tier change events logged
- Frame drop count tracked
- "FX Performance" output channel
- Statistics available via command

### Task 16: Wire FX Accessibility Integration
**Files:** `src/rendering/FxSafetyController.cpp`, `src/core/AccessibilityManager.cpp`
**Description:** Respect system accessibility settings: "Reduce Motion" disables animations, "Reduce Transparency" disables blur/glass effects.
**Acceptance Criteria:**
- System "Reduce Motion": all animations disabled
- System "Reduce Transparency": no blur effects
- High contrast mode: no dimming effects
- Accessibility overrides user FX settings
- Settings UI shows when system override is active

### Task 17: Wire Custom Effect Presets
**Files:** `src/rendering/FxPresetRegistry.cpp`, `src/core/Config.h`
**Description:** Users can define custom effect presets in config: name, effect type, parameters (duration, easing, color, intensity).
**Acceptance Criteria:**
- Custom presets in `.markamp/fx-presets.yaml`
- Parameters: duration, easing function, intensity (0-1), color
- Easing functions: linear, ease-in, ease-out, ease-in-out, bounce
- Custom presets selectable in settings
- Export/import presets

### Task 18: Wire FX Debug Overlay
**Files:** `src/rendering/FxEngine.cpp`, `src/ui/StatusBarPanel.cpp`
**Description:** Debug overlay shows real-time FX metrics: compositor time, active passes, quality tier, frame time breakdown.
**Acceptance Criteria:**
- Toggle: "FX: Show Debug Overlay"
- Shows: compositor time (ms), pass count, quality tier
- Frame time graph (last 60 frames)
- Color-coded: green (<8ms), yellow (<16ms), red (>16ms)
- Overlay positioned in corner, semi-transparent

### Task 19: Wire FX Command Palette Commands
**Files:** `src/ui/MainFrame.cpp`
**Description:** Register FX commands: "FX: Toggle Effects", "FX: Set Quality", "FX: Show Debug Overlay", "FX: Reset to Defaults".
**Acceptance Criteria:**
- All commands registered in command palette
- Commands categorized under "FX:" prefix
- "Toggle Effects" toggles master switch
- "Set Quality" shows quality picker
- "Reset to Defaults" restores default settings

### Task 20: Add FX System Tests
**Files:** `tests/unit/test_phase13_fx_engine.cpp`
**Description:** Test FX system: compositor pipeline, safety controller, preset registry, and quality tier transitions.
**Acceptance Criteria:**
- Compositor processes passes in correct order
- Safety controller reduces quality on frame drops
- Preset registry: register, lookup, custom presets
- Quality tier transitions: up and down
- Master toggle disables all effects
- Accessibility override respected

## Testing Requirements
- Compositor pipeline correctness
- Safety controller frame budget enforcement
- Preset registry management
- Quality tier automatic adjustment

## Phase Completion Criteria
- FxEngine compositor pipeline functional
- Built-in motion presets for transitions
- Text effects in editor
- Canvas object effects
- Panel and surface transition effects
- Safety controller auto-adjusts quality
- Accessibility integration
- All tests pass
