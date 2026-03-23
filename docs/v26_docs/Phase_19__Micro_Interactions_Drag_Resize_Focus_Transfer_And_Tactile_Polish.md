# Phase 19 - Micro Interactions Drag Resize Focus Transfer And Tactile Polish

## Goal

Finish the small interactive details that make MarkAmp feel precise, tactile, and unmistakably high-end.

## Definition Of Done

- drag, resize, selection transfer, and focus-transfer states feel deliberate and coherent
- panel activation and surface switching feel polished
- high-frequency micro-interactions reinforce, rather than fight, the visual system

## Tasks

### Phase ID: V26-P19
### Task ID: V26-P19-T01
### Task Title: Polish Drag, Resize, And Selection-Transfer Visual States Across Shell, Notebook, And Canvas
- Priority: P0
- Category: Motion / Animation
- Objective: Give movement-heavy interactions a polished visual language that communicates control and precision.
- Why This Matters Now: Drag and resize quality strongly influence whether custom productivity UIs feel premium or rough.
- UI Gap Statement: Shell splitters, notebook cell movement, and canvas object manipulation likely use different state language and fidelity.
- User Experience Impact: Better drag and resize visuals increase confidence and perceived responsiveness.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`, `/Users/ryanrentfro/code/markamp/src/core/NotebookChromeCoordinator.h`, `/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp`
- Prior Plan References: `v22` micro-interaction and canvas work, `v25` release-path workflow hardening
- Scope: splitter drag states, cell drag states, selection-transfer visuals, resize feedback, drop target emphasis
- Out of Scope: interaction mechanics themselves
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`, `/Users/ryanrentfro/code/markamp/src/core/NotebookChromeCoordinator.h`, `/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp`
- Related Screens / Components / Surfaces: shell splitters, notebook cells, canvas object manipulation
- Current UI Behavior / Appearance: movement-heavy states are likely functional but not yet unified or premium in feel.
- Intended UI Behavior / Appearance: drag and resize states feel precise, calm, and visually intentional across all major surfaces.
- Visual / Interaction Design Direction: professional tool-grade movement feedback with subtle emphasis and strong clarity.
- Technical Styling Approach: define micro-interaction state roles shared across drag/resize-capable surfaces.
- Implementation Steps:
  1. Audit drag and resize states across shell, notebook, and canvas.
  2. Define shared visual cues for active movement and drop targets.
  3. Apply refined visuals to the major surfaces.
  4. Validate pointer and keyboard-triggered movement states where applicable.
- Validation Steps:
  1. Drag shell splitters, notebook cells, and canvas objects.
  2. Compare drop-target and active-move feedback.
  3. Verify feedback remains clear in both themes.
- Acceptance Criteria: drag and resize interactions feel consistently premium and controlled across core surfaces.
- Dependencies: V26-P02-T01, V26-P08-T01, V26-P09-T02, V26-P17-T01
- Parallelization Notes: can run late once major surfaces are visually stable.
- Risks / Failure Modes: overly strong drag cues can clutter the screen during movement.
- Accessibility / Readability Notes: active movement states should remain obvious without relying only on motion.
- Theme / Styling Notes: drag indicators need contrast against varied backgrounds.
- Motion / Interaction Notes where relevant: use small, crisp transitions only where they improve control perception.
- Cleanup / Consolidation Notes where relevant: remove per-surface movement-state hacks.
- Rollback / Safety Notes: keep interaction hit zones and drop behavior unchanged.
- References / Context: this is a key premium feel task for creative and productivity workflows.
- Example scenarios where useful: dragging a notebook cell and dragging a canvas object should both feel deliberate and controlled.

### Phase ID: V26-P19
### Task ID: V26-P19-T02
### Task Title: Refine Focus Transfer, Surface Activation, And Context Switching Polish
- Priority: P1
- Category: Motion / Animation
- Objective: Improve how the UI visually communicates movement between active surfaces, tabs, panels, and transient overlays.
- Why This Matters Now: Activation quality is one of the least obvious but most important premium feel signals.
- UI Gap Statement: Focus transfer between shell zones and surfaces can feel abrupt or under-signaled when custom components evolve separately.
- User Experience Impact: Clear activation and context switching makes the app feel responsive and cognitively lighter.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/TabBar.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/ActivityBar.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/ThemedDialog.cpp`
- Prior Plan References: `v22` navigation and micro-interaction work
- Scope: active panel emphasis, surface activation states, overlay focus transfer, panel-to-editor context switching
- Out of Scope: focus-routing logic
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/TabBar.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/ActivityBar.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/ThemedDialog.cpp`
- Related Screens / Components / Surfaces: tabs, panels, overlays, shell activation states
- Current UI Behavior / Appearance: active context cues exist but likely remain uneven across shell zones.
- Intended UI Behavior / Appearance: switching focus feels smooth, obvious, and visually coherent without noisy highlighting.
- Visual / Interaction Design Direction: subtle context-shift cues with strong orientation value.
- Technical Styling Approach: combine shared state tokens with restrained motion and activation emphasis rules.
- Implementation Steps:
  1. Identify high-frequency context switches.
  2. Tune activation visuals for tabs, panels, and overlays.
  3. Align focus-transfer signals with the shared state and motion systems.
  4. Validate rapid context switching across work areas.
- Validation Steps:
  1. Switch repeatedly between editor, panel, notebook, and dialog contexts.
  2. Verify active area is always visually obvious.
  3. Confirm cues remain subtle and theme-correct.
- Acceptance Criteria: users can feel context change clearly without visual clutter.
- Dependencies: V26-P04-T01, V26-P05-T01, V26-P11-T01, V26-P18-T01
- Parallelization Notes: can run alongside tactile control polish.
- Risks / Failure Modes: excessive activation emphasis can make the shell feel busy.
- Accessibility / Readability Notes: active context must remain discernible for keyboard and mouse users alike.
- Theme / Styling Notes: activation cues must be theme-safe and not rely on aggressive saturation.
- Motion / Interaction Notes where relevant: keep focus-transfer motion subtle and short.
- Cleanup / Consolidation Notes where relevant: consolidate activation-state logic across shell surfaces.
- Rollback / Safety Notes: retain current focus semantics while improving visuals.
- References / Context: this is a final-sheen task that substantially affects perceived desktop quality.
- Example scenarios where useful: moving from a search panel back to the editor should clearly shift active context without a harsh flash.

### Phase ID: V26-P19
### Task ID: V26-P19-T03
### Task Title: Tune Tactile Response For Buttons, Rows, Cards, And Small Affordances
- Priority: P1
- Category: Motion / Animation
- Objective: Finish the small hover, press, and release interactions that make controls feel physical and refined.
- Why This Matters Now: Once the main surfaces are polished, the remaining perception gap is often in these small tactile responses.
- UI Gap Statement: Control families now share structure, but their fine-grained press/release feel may still differ.
- User Experience Impact: Better tactile response improves confidence and makes the app feel premium in everyday use.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/ThemeGallery.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/StartupPanel.cpp`
- Prior Plan References: `v22` tactile polish work
- Scope: buttons, list rows, gallery cards, empty-state actions, lightweight action affordances
- Out of Scope: major surface entry/exit transitions
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/ThemeGallery.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/StartupPanel.cpp`
- Related Screens / Components / Surfaces: toolbar buttons, settings rows, theme cards, startup actions
- Current UI Behavior / Appearance: controls are visually stronger than earlier passes but may still lack a fully unified tactile feel.
- Intended UI Behavior / Appearance: small interactions feel crisp, immediate, and quietly luxurious.
- Visual / Interaction Design Direction: subtle tactile feedback that emphasizes responsiveness over spectacle.
- Technical Styling Approach: apply shared state timing and micro-elevation or opacity rules selectively to high-value controls.
- Implementation Steps:
  1. Identify the most frequently used control families.
  2. Tune press/release and hover behavior for consistency.
  3. Validate interaction feel across shell, settings, and showcase surfaces.
  4. Ensure reduced-motion compatibility.
- Validation Steps:
  1. Interact repeatedly with representative buttons, rows, and cards.
  2. Compare tactile feel across surface types.
  3. Verify controls remain immediate and not laggy.
- Acceptance Criteria: small interactions feel consistently refined across the product.
- Dependencies: V26-P17-T03, V26-P10-T03, V26-P16-T02
- Parallelization Notes: can run after shared motion and icon/control metrics are stable.
- Risks / Failure Modes: too much micro-response can become distracting or slow.
- Accessibility / Readability Notes: interaction cues must still be understandable with reduced motion.
- Theme / Styling Notes: tactile cues should remain consistent in both themes.
- Motion / Interaction Notes where relevant: keep durations extremely short and responses subtle.
- Cleanup / Consolidation Notes where relevant: remove local one-off press/hover tweaks.
- Rollback / Safety Notes: revert any tactile effect that reduces perceived responsiveness.
- References / Context: this is the final feel-and-finish pass before cleanup.
- Example scenarios where useful: clicking a theme gallery card should feel slightly richer than clicking a plain list row, but still clearly part of one system.
