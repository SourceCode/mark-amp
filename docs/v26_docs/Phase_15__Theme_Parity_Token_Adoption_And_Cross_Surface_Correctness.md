# Phase 15 - Theme Parity Token Adoption And Cross Surface Correctness

## Goal

Validate and finish theme correctness across all major surfaces so premium polish holds in light and dark themes without local exceptions.

## Definition Of Done

- shell, editor, notebook, canvas, settings, panels, and overlays all render correctly in supported themes
- token adoption is consistent and hardcoded color debt on visible surfaces is removed
- light and dark themes feel intentionally equivalent, not merely supported

## Tasks

### Phase ID: V26-P15
### Task ID: V26-P15-T01
### Task Title: Audit Cross-Surface Theme Correctness After UI Convergence
- Priority: P0
- Category: Color / Theme
- Objective: Run a full cross-surface review after convergence work to identify remaining token-adoption gaps and theme mismatches.
- Why This Matters Now: Theme parity cannot be solved reliably before major surfaces stabilize.
- UI Gap Statement: The token system is broad, but adoption remains uneven across settings, shell, command surfaces, editor, notebook, and canvas.
- User Experience Impact: Reliable theme correctness is essential for trust and premium feel, especially in a customizable desktop app.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/core/ThemeTokens.h`, `/Users/ryanrentfro/code/markamp/src/core/ThemeEngine.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/ThemeGallery.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp`
- Prior Plan References: `v22` theme parity work, `v25` syntax/theme/icon closure
- Scope: shell, panels, dialogs, editor, notebook, canvas, settings, gallery, feedback, empty states
- Out of Scope: introduction of new theme families
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/ThemeTokens.h`, `/Users/ryanrentfro/code/markamp/src/core/ThemeEngine.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/ThemeGallery.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp`
- Related Screens / Components / Surfaces: all major visible surfaces
- Current UI Behavior / Appearance: many surfaces are theme-aware, but final parity and correctness are not yet assured.
- Intended UI Behavior / Appearance: both light and dark themes feel fully deliberate across the entire visible product.
- Visual / Interaction Design Direction: consistent premium finish independent of theme choice.
- Technical Styling Approach: use a structured screenshot-and-audit pass driven by token adoption gaps, not random visual tweaks.
- Implementation Steps:
  1. Capture representative surfaces in both themes after earlier phases land.
  2. Identify remaining hardcoded colors and token mismatches.
  3. Patch shared tokens and component usage.
  4. Re-run the audit until no major parity gaps remain.
- Validation Steps:
  1. Review paired screenshots surface by surface.
  2. Compare interaction states in both themes.
  3. Check edge states such as empty, error, and selection overlays.
- Acceptance Criteria: no major visible surface shows clear theme mismatch, broken contrast, or non-tokenized color behavior.
- Dependencies: completion of Phases 01 through 14 core surface work
- Parallelization Notes: should begin late once the main surfaces are stable.
- Risks / Failure Modes: theme fixes applied too early will churn repeatedly.
- Accessibility / Readability Notes: parity review must include contrast and focus-state readability.
- Theme / Styling Notes: prefer shared-token fixes over per-theme special cases.
- Motion / Interaction Notes where relevant: verify motion visibility remains appropriate in both themes.
- Cleanup / Consolidation Notes where relevant: remove now-obsolete color derivation code after adoption is complete.
- Rollback / Safety Notes: keep a clear record of theme-token changes so regressions can be isolated.
- References / Context: this is the major validation gate for all visual-system work.
- Example scenarios where useful: settings, editor, notebook, and canvas should all feel equally premium in dark mode and light mode.

### Phase ID: V26-P15
### Task ID: V26-P15-T02
### Task Title: Eliminate Remaining Hardcoded And Local Theme Derivations On Visible Surfaces
- Priority: P1
- Category: Color / Theme
- Objective: Remove visible-surface color and contrast logic that bypasses the shared theme system.
- Why This Matters Now: Hardcoded paint logic is one of the last major blockers to durable premium consistency.
- UI Gap Statement: Files like `/Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/StartupPanel.cpp`, and `/Users/ryanrentfro/code/markamp/src/ui/ThemeGallery.cpp` still show local visual decisions.
- User Experience Impact: Users get more reliable theme quality and fewer jarring color mismatches.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/StartupPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/ThemeGallery.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp`
- Prior Plan References: `v22` theme consistency, `v25` visible theme closure
- Scope: visible hardcoded colors, fallback accents, local color derivations, direct theme bypasses
- Out of Scope: internal debug-only surfaces
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/StartupPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/ThemeGallery.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp`
- Related Screens / Components / Surfaces: breadcrumbs, startup, settings, theme gallery, shell adjacencies
- Current UI Behavior / Appearance: several visible surfaces still carry local theme logic that undermines parity.
- Intended UI Behavior / Appearance: all major visible surfaces derive appearance through shared theme roles and tokens.
- Visual / Interaction Design Direction: durable consistency through systemic token use rather than hand-tuned exceptions.
- Technical Styling Approach: replace local derivations with named token roles; add new tokens only when a true semantic gap exists.
- Implementation Steps:
  1. Search for remaining hardcoded visible-surface colors and local derivations.
  2. Map them to existing or newly justified semantic tokens.
  3. Refactor affected surfaces.
  4. Re-run theme parity review.
- Validation Steps:
  1. Compare before/after in both themes.
  2. Verify no visual regressions in nearby surfaces.
  3. Confirm improved consistency for hover, selected, and focused states.
- Acceptance Criteria: visible-surface color decisions route through the shared theme system and parity issues materially decrease.
- Dependencies: V26-P15-T01
- Parallelization Notes: can run component by component once the audit identifies targets.
- Risks / Failure Modes: adding too many niche tokens can recreate design drift.
- Accessibility / Readability Notes: ensure token replacements preserve minimum contrast and state visibility.
- Theme / Styling Notes: semantic token naming must remain stable and reusable.
- Motion / Interaction Notes where relevant: none
- Cleanup / Consolidation Notes where relevant: remove old fallback branches and ad hoc color helpers.
- Rollback / Safety Notes: preserve a narrow patch scope per component to simplify regression review.
- References / Context: this is a convergence cleanup task, not a new theme-creation task.
- Example scenarios where useful: the breadcrumb bar should stop drifting visually from tabs and toolbar under different themes.

### Phase ID: V26-P15
### Task ID: V26-P15-T03
### Task Title: Validate Cross-Surface Selection, Focus, And Disabled State Parity Across Themes
- Priority: P1
- Category: Color / Theme
- Objective: Ensure interaction states remain equally clear and elegant in both light and dark themes across all major surfaces.
- Why This Matters Now: Static theme correctness can still fail if interactive states diverge across themes.
- UI Gap Statement: Selection, focus, hover, and disabled states often reveal parity gaps later than base surface colors do.
- User Experience Impact: Consistent interaction-state clarity improves trust and reduces friction in daily use.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/TabBar.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp`
- Prior Plan References: `v22` theme and interaction work, `v25` visible closure
- Scope: hover, selected, focused, disabled, active, current-row/current-cell states across major surfaces
- Out of Scope: behavioral state transitions
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/TabBar.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp`
- Related Screens / Components / Surfaces: controls, tabs, settings rows, tree/list rows, editor states
- Current UI Behavior / Appearance: interaction-state correctness is likely uneven across themes and surfaces.
- Intended UI Behavior / Appearance: interactive states look equally intentional and readable regardless of theme.
- Visual / Interaction Design Direction: stable, restrained interaction cues with strong cross-theme parity.
- Technical Styling Approach: review state tokens and per-surface state mapping together instead of patching one component at a time.
- Implementation Steps:
  1. Capture interactive states across representative surfaces in both themes.
  2. Identify parity mismatches or weak states.
  3. Tune shared state tokens and component usage.
  4. Re-test with keyboard and mouse interactions.
- Validation Steps:
  1. Hover, focus, select, and disable controls across core surfaces.
  2. Compare screenshots side by side across themes.
  3. Confirm no state disappears or becomes too loud in either theme.
- Acceptance Criteria: cross-theme interaction states remain clear, consistent, and visually refined across major surfaces.
- Dependencies: V26-P01-T03, V26-P15-T01
- Parallelization Notes: can run concurrently with hardcoded-color cleanup after the audit.
- Risks / Failure Modes: state tuning can accidentally reduce discoverability if over-softened.
- Accessibility / Readability Notes: focus and disabled states must remain visible without relying on subtle color alone.
- Theme / Styling Notes: state parity is a first-class theme requirement, not a nice-to-have.
- Motion / Interaction Notes where relevant: later animations must reinforce the same state hierarchy.
- Cleanup / Consolidation Notes where relevant: consolidate state mappings that have drifted per component.
- Rollback / Safety Notes: review interactive regressions immediately after each state-token change.
- References / Context: this is the final interaction-state parity pass before motion and cleanup.
- Example scenarios where useful: a disabled settings control and a disabled toolbar button should feel different in context but consistent in state language.
