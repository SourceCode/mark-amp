# Phase 17 - Motion Transitions And Responsiveness Feel

## Goal

Add restrained, premium motion so the interface feels responsive and cohesive without becoming flashy or heavy.

## Definition Of Done

- major shell and transient surfaces use a consistent motion language
- motion improves clarity and tactile feel rather than acting as decoration
- reduced-motion support is respected and predictable

## Tasks

### Phase ID: V26-P17
### Task ID: V26-P17-T01
### Task Title: Define A Restrained Shared Motion Language For Core UI States
- Priority: P0
- Category: Motion / Animation
- Objective: Establish timing, easing, and motion-intent rules for hover, focus, selection, and surface entry/exit transitions.
- Why This Matters Now: Motion added without a shared language quickly becomes inconsistent or gimmicky.
- UI Gap Statement: Several surfaces already have local interactive styling, but motion appears uneven or absent.
- User Experience Impact: Subtle consistent motion makes the app feel more responsive and premium.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/TabBar.cpp`
- Prior Plan References: `v22` motion polish
- Scope: motion principles, timing tiers, easing choices, reduced-motion fallback rules
- Out of Scope: dramatic animation sequences
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/TabBar.cpp`
- Related Screens / Components / Surfaces: command palette, toolbar, tabs, overlays, focus transitions
- Current UI Behavior / Appearance: some surfaces hint at motion or interactive scaling, but there is no clear shared motion system.
- Intended UI Behavior / Appearance: motion is subtle, fast, and consistent across shell and transient interactions.
- Visual / Interaction Design Direction: professional, restrained desktop motion focused on continuity and confidence.
- Technical Styling Approach: define reusable timing and easing constants and apply them selectively to high-value transitions.
- Implementation Steps:
  1. Audit existing animations and interaction transitions.
  2. Define motion tiers and reduced-motion behavior.
  3. Update shared helpers or relevant components to use those rules.
  4. Validate responsiveness and visual restraint.
- Validation Steps:
  1. Compare hover, focus, and entry transitions across core surfaces.
  2. Test with reduced-motion preferences enabled if supported.
  3. Confirm no transition feels slow or ornamental.
- Acceptance Criteria: core motion behaviors feel clearly related and improve perceived responsiveness.
- Dependencies: most core surface styling phases materially complete
- Parallelization Notes: can begin after shell, editor, and overlay visuals stabilize.
- Risks / Failure Modes: too much motion can cheapen the UI or hurt responsiveness.
- Accessibility / Readability Notes: every motion pattern needs a reduced-motion-compatible fallback.
- Theme / Styling Notes: motion should reinforce state clarity, not depend on theme-specific tricks.
- Motion / Interaction Notes where relevant: this task defines those rules.
- Cleanup / Consolidation Notes where relevant: remove local animation constants once shared motion roles exist.
- Rollback / Safety Notes: make motion easy to disable and easy to tune centrally.
- References / Context: motion is a finishing layer, not a substitute for static clarity.
- Example scenarios where useful: tabs, menus, and overlays should all settle with similar pacing.

### Phase ID: V26-P17
### Task ID: V26-P17-T02
### Task Title: Add Premium Transition Continuity To Menus, Dialogs, Palette, And Panels
- Priority: P1
- Category: Motion / Animation
- Objective: Give major transient and shell surfaces subtle entry/exit continuity so the interface feels responsive and composed.
- Why This Matters Now: Static polish is in place by this phase; targeted transitions can now reinforce it.
- UI Gap Statement: Menus, command palette, dialogs, and panel changes likely still appear instantly or inconsistently.
- User Experience Impact: Better transitions improve perceived smoothness and reduce abruptness.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/ThemedDialog.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`
- Prior Plan References: `v22` motion and overlay work
- Scope: panel reveal/hide, palette open/close, dialog appearance, lightweight menu/popover transitions
- Out of Scope: complex physics or long animations
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/ThemedDialog.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`
- Related Screens / Components / Surfaces: command palette, dialogs, panel toggles, menus, popovers
- Current UI Behavior / Appearance: transitions are likely absent, inconsistent, or locally implemented.
- Intended UI Behavior / Appearance: major transient surfaces appear and disappear with subtle, confident continuity.
- Visual / Interaction Design Direction: fast desktop transitions with low-travel motion and soft opacity support.
- Technical Styling Approach: apply the shared motion tiers only to high-value surface changes, with short durations and minimal movement.
- Implementation Steps:
  1. Identify high-value transitions where motion improves clarity.
  2. Apply shared motion tiers to those transitions.
  3. Tune motion to remain fast and subtle.
  4. Validate reduced-motion fallback behavior.
- Validation Steps:
  1. Open and close dialogs, palette, and panels repeatedly.
  2. Compare motion consistency between surface types.
  3. Verify no transition blocks fast workflows.
- Acceptance Criteria: major transient and shell transitions feel smoother and more premium without feeling animated for animation’s sake.
- Dependencies: V26-P17-T01, V26-P11-T01, V26-P03-T02
- Parallelization Notes: can run once overlay and panel shells are visually stable.
- Risks / Failure Modes: too much opacity or travel distance can feel sluggish.
- Accessibility / Readability Notes: reduced-motion mode must remain fully supported.
- Theme / Styling Notes: motion should work identically across themes.
- Motion / Interaction Notes where relevant: keep durations short and use one or two easing curves only.
- Cleanup / Consolidation Notes where relevant: remove bespoke per-surface animation code.
- Rollback / Safety Notes: maintain a central motion toggle or clear reduced-motion path.
- References / Context: this task should improve perceived quality without altering information architecture.
- Example scenarios where useful: opening the command palette should feel smooth and immediate, not abrupt or flashy.

### Phase ID: V26-P17
### Task ID: V26-P17-T03
### Task Title: Polish Hover, Press, And Selection Response For High-Frequency Controls
- Priority: P1
- Category: Motion / Animation
- Objective: Add tactile responsiveness to frequently used controls through subtle response timing and state transitions.
- Why This Matters Now: Small interactions define the “luxury” feel of a desktop app once static visuals are solid.
- UI Gap Statement: Hover and press states may still feel instantaneous in some places and animated in others, creating inconsistent tactile feedback.
- User Experience Impact: Users feel more direct control and higher quality when controls respond consistently.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/TabBar.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp`
- Prior Plan References: `v22` micro-interaction and motion work
- Scope: toolbar buttons, tabs, settings controls, canvas tool palette, list rows
- Out of Scope: content editing animations inside the editor
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/TabBar.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp`
- Related Screens / Components / Surfaces: toolbar, tabs, settings controls, canvas tools, dense rows
- Current UI Behavior / Appearance: state changes are visible but not yet likely tuned as one tactile system.
- Intended UI Behavior / Appearance: common controls respond with quiet polish and consistent timing.
- Visual / Interaction Design Direction: tactile but restrained desktop responsiveness.
- Technical Styling Approach: tie control-state transitions to the shared motion and state systems instead of per-surface experiments.
- Implementation Steps:
  1. Identify the highest-frequency controls and rows.
  2. Apply subtle state-transition timing to hover, press, and selection changes.
  3. Verify consistency across control families.
  4. Confirm reduced-motion compatibility.
- Validation Steps:
  1. Interact repeatedly with toolbar buttons, tabs, and settings controls.
  2. Compare hover and press feel across surfaces.
  3. Ensure zero noticeable latency is introduced.
- Acceptance Criteria: high-frequency controls feel tactile and coherent across the app.
- Dependencies: V26-P17-T01, V26-P03-T01, V26-P04-T01, V26-P10-T03
- Parallelization Notes: can run after major control families are visually stabilized.
- Risks / Failure Modes: even subtle motion can feel laggy if tied to slow state updates.
- Accessibility / Readability Notes: static state clarity must remain sufficient without animation.
- Theme / Styling Notes: motion should complement, not replace, visual state differences.
- Motion / Interaction Notes where relevant: use motion only to smooth transitions between already clear states.
- Cleanup / Consolidation Notes where relevant: consolidate control-state timing values.
- Rollback / Safety Notes: keep a fast path with zero animation if responsiveness suffers.
- References / Context: this is a premium tactile polish task, not a major behavioral change.
- Example scenarios where useful: switching tabs rapidly should still feel immediate while preserving subtle state continuity.
