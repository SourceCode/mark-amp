# Phase 20: Iconography Visual Cleanup Legacy Style Removal And UI QA Gates

## Outcome

Finish the UI wave by completing iconography normalization, removing visual legacy residue, consolidating duplicated styling pathways, and enforcing final product-wide UI quality gates.

## Improvement Count

360 atomic improvements across 6 execution tasks.

### P20-T01

- Phase ID: P20
- Task ID: P20-T01
- Task Title: Complete visual MUI icon migration and retire legacy or placeholder iconography
- Priority: P0
- Category: Iconography
- Atomic Improvements Covered: 60
- Objective: Ensure every major visible icon family uses one polished visual system with correct semantics and alignment.
- Why This Matters Now: Icon inconsistency remains one of the easiest ways to expose unfinished UI work.
- UI Problem Statement: MUI icon infrastructure exists, but legacy icons, Unicode placeholders, and local semantic drift remain in several surfaces, especially [CanvasWorkspacePanel.cpp](/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp).
- User Experience Impact: The interface can still feel visually mixed and less premium.
- Scope: MUI icon completion, placeholder icon removal, semantic icon mapping, icon-only button clarity, panel and shell icon parity, icon contrast states.
- Out of Scope: Illustration systems beyond product iconography.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/MuiIconPipeline.cpp`; `/Users/ryanrentfro/code/markamp/src/core/IconSemanticMapper.cpp`; `/Users/ryanrentfro/code/markamp/src/core/IconValidationGate.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/IconManager.cpp`
- Related Screens / Components / Surfaces: Entire app, especially canvas, shell chrome, settings, panels, context menus.
- Current UI Behavior / Appearance: Icon infrastructure is strong, but product-wide visual completion is not finished.
- Intended UI Behavior / Appearance: Icons feel fully normalized, optically aligned, and semantically consistent across the entire product.
- Visual / Interaction Design Direction: One precise icon language with zero placeholder residue.
- Technical Styling Approach: Use icon audits and semantic mapping to replace legacy residue and align all surfaces to the MUI-based system.
- Implementation Steps: Audit icon usage; replace Unicode and legacy icons; align semantic mappings; retune icon-only controls; verify contrast and active-state behavior; remove fallback placeholder paths from production.
- Validation Steps: Review all high-frequency icon surfaces in dark, light, and high-contrast themes.
- Acceptance Criteria: No prominent production surface still uses placeholder or visually inconsistent iconography.
- Dependencies: Phase 01, Phase 18.
- Risks / Failure Modes: Some existing symbols may need careful semantic substitution to preserve user recognition.
- Accessibility / Readability Notes: Icon-only actions need excellent labeling and state visibility after migration.
- Theme / Styling Notes: Icon weight and contrast need per-theme review, not blind reuse.
- Motion / Interaction Notes where relevant: Icon-state transitions should remain crisp and understated.
- Observability / Diagnostics Notes where relevant: Add icon-residue reports and per-surface icon-family checks.
- Rollback / Safety Notes: Keep semantic ID aliases while visually retiring old icon assets.
- References / Context: Icon completion is a stated requirement for this UI wave.
- Example scenarios where useful: Canvas tools, panel headers, and menus all use the same polished icon system instead of mixed glyph sources.

### P20-T02

- Phase ID: P20
- Task ID: P20-T02
- Task Title: Remove hardcoded visual literals and subsystem-local style fragments from visible surfaces
- Priority: P0
- Category: Cleanup / Consolidation
- Atomic Improvements Covered: 60
- Objective: Retire local styling hacks so the UI system remains coherent after `v22`.
- Why This Matters Now: Premium polish will not stick if local hardcoded styling remains the easiest path.
- UI Problem Statement: Local visual literals still appear in surfaces such as [StartupPanel.cpp](/Users/ryanrentfro/code/markamp/src/ui/StartupPanel.cpp), [ThemeGallery.cpp](/Users/ryanrentfro/code/markamp/src/ui/ThemeGallery.cpp), [CanvasWorkspacePanel.cpp](/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp), and elsewhere.
- User Experience Impact: Surface quality remains uneven and future drift stays easy.
- Scope: Hardcoded colors, point sizes, paddings, radii, shell dimensions, icon offsets, placeholder strings that imply temporary UI treatment.
- Out of Scope: Necessary backend literals unrelated to visible UI.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/StartupPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/ThemeGallery.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`
- Related Screens / Components / Surfaces: Entire app, especially locally styled legacy surfaces.
- Current UI Behavior / Appearance: Multiple visible surfaces still bypass design-system tokens and component variants.
- Intended UI Behavior / Appearance: Styling comes primarily from tokens and shared surface systems rather than local literals.
- Visual / Interaction Design Direction: Durable visual-system discipline.
- Technical Styling Approach: Combine static audits with targeted refactors to eliminate visible-local style drift.
- Implementation Steps: Inventory local UI literals; map them to system tokens or variants; remove obsolete styling files and comments; add checks for new local visual literals.
- Validation Steps: Re-run UI audits and compare surfaces before and after local-style removal.
- Acceptance Criteria: Visible local style hacks are materially reduced and no longer define major surfaces.
- Dependencies: Phase 01, Phase 18.
- Risks / Failure Modes: Some highly custom surfaces may temporarily need bridge adapters before full tokenization is possible.
- Accessibility / Readability Notes: Literal removal should preserve or improve contrast and focus quality, not flatten it.
- Theme / Styling Notes: Replacing literals with tokens must improve theme parity, not merely shift where the same color comes from.
- Motion / Interaction Notes where relevant: Include local animation literals and timings where they affect visible UI.
- Observability / Diagnostics Notes where relevant: Add static reports for local visible-style literals in key UI directories.
- Rollback / Safety Notes: Preserve compatibility token aliases while migrating heavily customized surfaces.
- References / Context: `v22` should end with the UI system being easier to maintain, not just prettier in the moment.
- Example scenarios where useful: A hardcoded purple startup button and a local theme-gallery font override are both eliminated in favor of shared tokens.

### P20-T03

- Phase ID: P20
- Task ID: P20-T03
- Task Title: Consolidate duplicate UI primitives and retire obsolete or temporary-looking component variants
- Priority: P1
- Category: Cleanup / Consolidation
- Atomic Improvements Covered: 60
- Objective: Reduce the number of overlapping visual primitives so the product stays coherent after the redesign wave.
- Why This Matters Now: Duplicate component variants create drift even when a new design system exists.
- UI Problem Statement: The repo contains many overlapping UI primitives, custom widgets, placeholder helpers, and historical variants that need rationalization after the visual redesign.
- User Experience Impact: Users experience inconsistency and future features reintroduce it more easily.
- Scope: Duplicate button and toolbar variants, overlapping panel shells, redundant placeholder components, obsolete row/cell renderers, outdated helper surfaces.
- Out of Scope: Functional component removal unrelated to visible UI.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/ThemedButton.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/SidebarSkeletonPlaceholder.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/ThemePreviewCard.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/PanelHeader.cpp`
- Related Screens / Components / Surfaces: Reusable controls, placeholders, cards, toolbars, panel furniture.
- Current UI Behavior / Appearance: Similar visual roles are often implemented by multiple component families with slightly different styling assumptions.
- Intended UI Behavior / Appearance: The product relies on a smaller, cleaner set of reusable visual primitives.
- Visual / Interaction Design Direction: Fewer, stronger component families.
- Technical Styling Approach: Build a component inventory, classify duplicates, and retire or merge obsolete variants into the new design system.
- Implementation Steps: Inventory visual primitives; classify canonical versus legacy variants; migrate usage to canonical families; remove obsolete helpers and placeholder-looking variants.
- Validation Steps: Review before/after component maps and confirm surface parity remains visually stronger, not weaker.
- Acceptance Criteria: The number of overlapping visible component variants is materially reduced.
- Dependencies: Phase 01 through Phase 19.
- Risks / Failure Modes: Over-consolidation can erase useful domain-specific nuance if roles are merged too aggressively.
- Accessibility / Readability Notes: Consolidation must preserve or improve accessibility-focused variants.
- Theme / Styling Notes: Canonical primitives must support all theme modes before legacy variants are retired.
- Motion / Interaction Notes where relevant: Canonical primitives need complete motion and reduced-motion behavior before they replace older ones.
- Observability / Diagnostics Notes where relevant: Publish a component inventory and legacy-retirement report.
- Rollback / Safety Notes: Keep temporary adapters while legacy components are phased out gradually.
- References / Context: Visual cleanup is only durable if duplicate primitives are reduced.
- Example scenarios where useful: Multiple button-like controls across startup, dialogs, and panel headers collapse into a smaller set of canonical variants.

### P20-T04

- Phase ID: P20
- Task ID: P20-T04
- Task Title: Remove temporary-looking placeholder visuals and unfinished presentation seams from production surfaces
- Priority: P0
- Category: Cleanup / Consolidation
- Atomic Improvements Covered: 60
- Objective: Eliminate the last visible hints that some parts of the UI are provisional.
- Why This Matters Now: A premium product cannot visibly ship TODO-level styling residue.
- UI Problem Statement: The repository still contains visual placeholders, styling note files, skeleton helpers used as stand-ins, and under-designed surfaces that read as temporary.
- User Experience Impact: Temporary-looking surfaces weaken trust in the whole product.
- Scope: Styling TODO residue, placeholder text blocks, generic properties panels, provisional shell copy, thin AV surfaces, placeholder cards and helpers.
- Out of Scope: Hidden internal-only experimental surfaces not exposed in production.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/StartupPanel_styling.txt`; `/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/SidebarSkeletonPlaceholder.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/av/AVTablePanel.cpp`
- Related Screens / Components / Surfaces: Startup, canvas inspector, placeholder states, thin data views, visible TODO-style surfaces.
- Current UI Behavior / Appearance: Several visible seams still read as not-yet-finished.
- Intended UI Behavior / Appearance: No user-visible UI looks like a placeholder, stub, or styling TODO.
- Visual / Interaction Design Direction: Full removal of temporary visual debt from production-facing surfaces.
- Technical Styling Approach: Use the phase-by-phase audit outputs to identify and retire placeholder-looking presentation artifacts.
- Implementation Steps: Classify placeholder-looking visuals; replace, redesign, or gate them; remove explicit styling-note residue; strengthen thin surfaces that still visually read as stand-ins.
- Validation Steps: Compare the production UI against the placeholder audit and verify no unresolved high-severity visual placeholders remain.
- Acceptance Criteria: Temporary-looking UI is materially eliminated from exposed product surfaces.
- Dependencies: All prior surface phases.
- Risks / Failure Modes: Hiding placeholder visuals without replacing them can create empty gaps if not handled carefully.
- Accessibility / Readability Notes: Replacements must improve clarity, not merely hide unfinished elements.
- Theme / Styling Notes: Placeholder removal must include theme-specific residue, not only default-theme appearances.
- Motion / Interaction Notes where relevant: Placeholder shimmer or scaffolding should not remain as a stand-in for real sparse-state design.
- Observability / Diagnostics Notes where relevant: Add a placeholder-surface report to the final UI QA bundle.
- Rollback / Safety Notes: Allow experimental-only gates for internal surfaces still under active development.
- References / Context: `v22` should end with the visible UI feeling finished, not “mostly finished.”
- Example scenarios where useful: The canvas inspector no longer says “Select an object…” in a bare placeholder block that looks like a stub.

### P20-T05

- Phase ID: P20
- Task ID: P20-T05
- Task Title: Build whole-product UI scoreboards, family boards, and blocker reports for final finish management
- Priority: P1
- Category: Cleanup / Consolidation
- Atomic Improvements Covered: 60
- Objective: Give the implementation team one operational view of UI completion, remaining blockers, and visual debt.
- Why This Matters Now: A 20-phase UI wave needs a concise execution dashboard, not just individual documents.
- UI Problem Statement: Without a comparative scoreboard, it becomes difficult to see which surface families still block the premium UI bar.
- User Experience Impact: Indirect but important; weak internal visibility lets visual debt persist into release.
- Scope: Phase completion summaries, family blockers, unresolved placeholder counts, theme parity status, accessibility visual status, motion/reduced-motion readiness, icon migration residue counts.
- Out of Scope: End-user telemetry dashboards.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/docs/v22_docs`; `/Users/ryanrentfro/code/markamp/tests/unit`; `/Users/ryanrentfro/code/markamp/src/ui`; `/Users/ryanrentfro/code/markamp/src/core`
- Related Screens / Components / Surfaces: Entire UI program.
- Current UI Behavior / Appearance: Planning artifacts exist, but not one live UI-finish scoreboard tied to execution outputs.
- Intended UI Behavior / Appearance: Teams can identify the highest-risk unresolved visual areas quickly and accurately.
- Visual / Interaction Design Direction: Operational clarity for final UI finish work.
- Technical Styling Approach: Aggregate family-gallery status, audits, and blocker counts into one machine-readable and human-readable scoreboard.
- Implementation Steps: Define scoreboard schema; summarize phase/family status; attach unresolved blocker counts and report links; publish with CI artifacts and local review bundles.
- Validation Steps: Mark known gaps unresolved and verify the scoreboard reflects the right highest-priority blockers.
- Acceptance Criteria: The team can tell at a glance what still prevents the UI from feeling world-class.
- Dependencies: All prior `v22` family boards and audits.
- Risks / Failure Modes: Scoreboards can become vanity metrics if they do not stay tied to real blockers and visible evidence.
- Accessibility / Readability Notes: Scoreboard reports should explicitly track accessibility visual blockers, not only styling blockers.
- Theme / Styling Notes: Include theme parity blockers and unresolved token issues in the scoreboard.
- Motion / Interaction Notes where relevant: Include motion/reduced-motion blocker status.
- Observability / Diagnostics Notes where relevant: The scoreboard should aggregate reports produced by the UI QA stack.
- Rollback / Safety Notes: Keep scoreboard generation read-only and deterministic.
- References / Context: The user asked for an authoritative blueprint; an execution scoreboard makes that blueprint operable.
- Example scenarios where useful: The scoreboard shows notebook output styling, light-theme panel parity, and remaining Unicode canvas icons as active blockers.

### P20-T06

- Phase ID: P20
- Task ID: P20-T06
- Task Title: Define and enforce the v22 UI-only exit criteria and release gates
- Priority: P0
- Category: Cleanup / Consolidation
- Atomic Improvements Covered: 60
- Objective: Turn `v22` from a planning package into a concrete release bar for UI quality.
- Why This Matters Now: Without explicit exit criteria, the wave can end with broad polish work still feeling unfinished or subjective.
- UI Problem Statement: The app needs a hard definition of done for visual cohesion, premium finish, theme parity, accessibility visuals, and cleanup.
- User Experience Impact: Users continue to experience an uneven UI if the finish bar is interpreted loosely.
- Scope: Exit criteria, blocker definitions, visual QA gates, unresolved placeholder policy, icon parity gates, theme parity gates, accessibility visual gates, motion parity gates.
- Out of Scope: Non-UI functionality acceptance.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/docs/v22_docs/README__MarkAmp_V22_UI_Execution_Master_Index.md`; `/Users/ryanrentfro/code/markamp/docs/v22_docs/ASSESSMENT__MarkAmp_V22_UI_Quality_And_Visual_Cohesion_Assessment.md`; `/Users/ryanrentfro/code/markamp/tests/unit`
- Related Screens / Components / Surfaces: Entire visible product.
- Current UI Behavior / Appearance: UI quality goals exist in prose, but the product lacks one final operational release bar for this UI wave.
- Intended UI Behavior / Appearance: `v22` exits only when the visible product meets a clearly defined premium UI threshold across all major surface families.
- Visual / Interaction Design Direction: Finalized, enforced UI quality standard for MarkAmp.
- Technical Styling Approach: Encode UI exit gates using the family boards, audits, placeholder reports, and parity checks defined throughout `v22`.
- Implementation Steps: Define required family boards and audits; set blocker thresholds; require sign-off on unresolved placeholder, contrast, theme, icon, and motion issues; wire the gates into release checklists.
- Validation Steps: Simulate unresolved high-severity UI blockers and verify that the release gate would stop the wave from closing.
- Acceptance Criteria: The team has one enforceable definition of done for the entire UI-only execution pass.
- Dependencies: P20-T01 through P20-T05 and the earlier `v22` phases that produce galleries, audits, and scoreboards.
- Risks / Failure Modes: If the exit criteria are too vague or too forgiving, the UI wave will not materially finish the product interface.
- Accessibility / Readability Notes: Exit criteria must require strong focus visibility, contrast, and reduced-motion parity, not treat them as optional polish.
- Theme / Styling Notes: Exit criteria must require actual light/dark/high-contrast parity, not only token presence.
- Motion / Interaction Notes where relevant: Exit criteria must include restrained motion quality and reduced-motion parity.
- Observability / Diagnostics Notes where relevant: Exit status should be generated from reports and evidence, not only manual opinion.
- Rollback / Safety Notes: Allow only tightly scoped, time-bounded waivers for experimental surfaces not exposed in production.
- References / Context: The user asked for a serious operating plan for finishing the interface; this task turns that plan into a product bar.
- Example scenarios where useful: Release is blocked until canvas tool iconography, notebook output styling, and light-theme settings contrast all clear their UI gates.

