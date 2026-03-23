# Phase 17 - Theme Parity Token Normalization And Cross Surface Correctness

## Goal

Validate and finish the redesigned visual system across theme families so light and dark modes both feel deliberate, premium, and fully correct.

## Definition Of Done

- redesigned icon, surface, and state systems are correct across themes
- remaining hardcoded or local theme derivations on visible surfaces are removed
- theme parity is treated as a redesign signoff criterion, not only a regression check

## Tasks

### Phase ID: V27-P17
### Task ID: V27-P17-T01
### Task Title: Run A Full Cross Surface Theme Audit On The Redesigned UI
- Priority: P0
- Category: Color / Theme
- Objective: perform a comprehensive theme audit after redesign phases land to identify parity and correctness gaps across all major surfaces.
- Why This Matters Now: parity review is only meaningful once the new icon and visual systems are actually present across surfaces.
- Visual Gap Statement: theme support exists, but the redesigned product could still look uneven or reveal local color/state assumptions across surfaces.
- User Experience Impact: strong theme parity increases trust and lets the redesign feel complete for all users, not only one theme preference.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/core/ThemeTokens.h`, `/Users/ryanrentfro/code/markamp/src/core/ThemeEngine.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/ThemeGallery.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp`
- Prior Plan References: `v22` theme parity phase, `v26` Phase 15
- Scope: shell, panels, editor, syntax, notebook, canvas, settings, dialogs, feedback, onboarding, icon-state rendering
- Out of Scope: adding new theme families
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/ThemeTokens.h`, `/Users/ryanrentfro/code/markamp/src/core/ThemeEngine.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/ThemeGallery.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp`
- Related Screens / Components / Surfaces: all redesigned visible surfaces
- Current Visual Behavior / Appearance: individual theme-aware systems exist, but a full redesigned-product audit is still required to ensure cross-surface correctness.
- Intended Visual Behavior / Appearance: both light and dark presentations feel equally premium and fully intentional across the entire app.
- Icon / Visual Design Direction: stable redesigned identity independent of theme choice.
- Technical Styling Approach: use structured screenshot and interaction audits tied to semantic token usage and redesigned state roles.
- Implementation Steps:
  1. Capture representative screens and states for all major surfaces in both themes.
  2. Audit surface correctness, icon-state correctness, and visual balance.
  3. Record and prioritize parity gaps.
  4. Patch tokens and surface usage until the audit is clean.
- Validation Steps:
  1. Review screenshots and live interactions side by side for both themes.
  2. Compare normal, dense, empty, and error states.
  3. Confirm redesigned surfaces keep their intended hierarchy in both themes.
- Acceptance Criteria: no major redesigned surface shows obvious theme mismatch, broken contrast, or legacy local theming behavior.
- Dependencies: completion of the core redesign phases
- Parallelization Notes: can start as earlier surfaces complete, but final signoff is late.
- Risks / Failure Modes: auditing too early will create churn; auditing too late may reveal a large tail of parity work.
- Accessibility / Readability Notes: parity review must include focus and contrast behavior, not only base colors.
- Theme / Styling Notes: prioritize shared semantic fixes over per-surface theme exceptions.
- Motion / Interaction Notes where relevant: confirm state transitions remain visually appropriate in both themes.
- Cleanup / Consolidation Notes where relevant: record any remaining local theme derivations for Phase 20 removal.
- Rollback / Safety Notes: keep theme changes incremental and traceable.
- References / Context: theme parity is one of the final proofs that the redesign is systemically complete.
- Example scenarios where useful: editor, settings, notebook, and canvas should all feel equally deliberate in dark and light themes.

### Phase ID: V27-P17
### Task ID: V27-P17-T02
### Task Title: Remove Remaining Hardcoded Theme And Local Color Derivation Residue On Redesigned Surfaces
- Priority: P1
- Category: Color / Theme
- Objective: eliminate the remaining visible-surface theme and color logic that bypasses the redesigned semantic system.
- Why This Matters Now: local theme residue will quickly undermine parity and make future redesign maintenance brittle.
- Visual Gap Statement: even redesigned surfaces can still be undercut by old hardcoded color decisions or component-local derivations.
- User Experience Impact: removing local theme residue improves consistency and prevents jarring visual mismatches.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/StartupPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/ThemeGallery.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`
- Prior Plan References: `v22` and `v26` theme cleanup phases
- Scope: hardcoded visible-surface colors, fallback accents, local derived color paths, direct theme bypasses in redesigned surfaces
- Out of Scope: invisible debug-only visuals
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/StartupPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/ThemeGallery.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`
- Related Screens / Components / Surfaces: navigation surfaces, startup, settings, shell composition, theme gallery
- Current Visual Behavior / Appearance: local theme derivations remain a likely source of residual inconsistency even after major redesign work.
- Intended Visual Behavior / Appearance: redesigned surfaces depend on the semantic theme system rather than local color improvisation.
- Icon / Visual Design Direction: durable redesign correctness through semantic token discipline.
- Technical Styling Approach: replace surface-local color logic with semantic theme roles and ban newly introduced local derivation on visible surfaces.
- Implementation Steps:
  1. Search redesigned surfaces for hardcoded or locally derived color logic.
  2. Map each to the correct semantic theme role.
  3. Refactor and retest the affected surfaces.
  4. Update validation guidance to block reintroduction.
- Validation Steps:
  1. Compare before/after surfaces in both themes.
  2. Verify nearby states and overlays do not regress.
  3. Confirm removal of any lingering special-case color behavior.
- Acceptance Criteria: redesigned visible surfaces no longer depend on obvious local theme derivations where the semantic system should own appearance.
- Dependencies: V27-P17-T01
- Parallelization Notes: can run component by component as the audit reveals targets.
- Risks / Failure Modes: heavy-handed cleanup without semantic replacements can create new parity regressions.
- Accessibility / Readability Notes: replacement tokens must preserve focus and contrast clarity.
- Theme / Styling Notes: the goal is semantic normalization, not per-theme patchwork.
- Motion / Interaction Notes where relevant: none
- Cleanup / Consolidation Notes where relevant: record removed local derivations as part of final consolidation.
- Rollback / Safety Notes: keep changes scoped to one surface family at a time.
- References / Context: redesigned surfaces should not be supported by old local color behavior.
- Example scenarios where useful: startup and theme gallery should stop carrying local color behavior that diverges from the redesigned shell.

### Phase ID: V27-P17
### Task ID: V27-P17-T03
### Task Title: Validate Icon State And Interaction Parity Across Themes On The Redesigned Product
- Priority: P1
- Category: Color / Theme
- Objective: ensure canonical icons and redesigned interaction states remain equally clear across theme families.
- Why This Matters Now: `v27` elevates icon replacement to foundation status, so icon-state parity now becomes a first-class theme requirement.
- Visual Gap Statement: hover, selected, disabled, warning, and error icon states can still drift across themes even if base colors are correct.
- User Experience Impact: better icon-state parity improves trust, discoverability, and visual coherence.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/core/IconMetricsPolicy.h`, `/Users/ryanrentfro/code/markamp/src/core/MuiIconPipeline.h`, `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/TabBar.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp`
- Prior Plan References: `v26` icon and theme phases
- Scope: icon state contrast, disabled behavior, selected/active behavior, severity icon parity, icon-only control parity across themes
- Out of Scope: icon asset creation itself
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/IconMetricsPolicy.h`, `/Users/ryanrentfro/code/markamp/src/core/MuiIconPipeline.h`, `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/TabBar.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp`
- Related Screens / Components / Surfaces: toolbar, tabs, settings, notifications, feedback surfaces, icon-only controls
- Current Visual Behavior / Appearance: icon states are likely improved but not yet guaranteed to be equally strong and elegant in both themes.
- Intended Visual Behavior / Appearance: canonical icons behave predictably and clearly across all relevant states in every theme.
- Icon / Visual Design Direction: one icon-state language with equal clarity in light and dark variants.
- Technical Styling Approach: audit icon-state tokens and per-surface mappings together rather than treating icons as a passive afterthought of theme work.
- Implementation Steps:
  1. Review icon states across representative surfaces in both themes.
  2. Tune icon-state color, opacity, and emphasis rules.
  3. Patch any component-specific state mismatches.
  4. Validate on icon-only and icon-plus-text controls.
- Validation Steps:
  1. Hover, disable, select, and trigger warning/error icon states across the app.
  2. Compare parity between light and dark themes.
  3. Confirm icon states stay visible without becoming too loud.
- Acceptance Criteria: icon states remain coherent, readable, and premium in both themes across redesigned surfaces.
- Dependencies: Phase 03, V27-P17-T01
- Parallelization Notes: can run alongside other late parity work after enough surfaces are redesigned.
- Risks / Failure Modes: over-correcting icon emphasis can create visual noise or break hierarchy.
- Accessibility / Readability Notes: icon states must not rely on minor color shifts alone.
- Theme / Styling Notes: this is the icon-specific parity complement to general theme review.
- Motion / Interaction Notes where relevant: later state transitions must preserve the same parity.
- Cleanup / Consolidation Notes where relevant: consolidate icon-state mappings that drifted per component.
- Rollback / Safety Notes: keep per-surface parity fixes minimal and measurable.
- References / Context: icon-state parity is a uniquely important `v27` requirement because the pass centers icon replacement.
- Example scenarios where useful: a disabled toolbar icon and a selected sidebar icon should both remain equally legible in dark and light modes.
