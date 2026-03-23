# Phase 20 - Full System Cleanup Legacy Asset Removal And Visual QA Gates

## Goal

Remove the old visual world, retire legacy icon and style residue, and define the final signoff gate for the transformed product.

## Definition Of Done

- legacy icon and visual systems are retired or clearly isolated outside the visible product path
- the redesigned product is validated through explicit visual-completeness criteria
- `v27` ends with a durable redesigned baseline rather than another layer of partially overlapping systems

## Tasks

### Phase ID: V27-P20
### Task ID: V27-P20-T01
### Task Title: Remove Legacy Icon Assets Icon APIs And Obsolete Visual Fragments From The Visible Product Path
- Priority: P0
- Category: Cleanup / Consolidation
- Objective: retire the icon and style systems that the redesign has superseded so the product no longer ships a mixed visual stack.
- Why This Matters Now: a redesign is not complete while legacy icon and style paths remain active on the visible path.
- Visual Gap Statement: the repository still contains old icon chars, glyph fallbacks, local style fragments, and parallel icon systems that can survive the redesign if not removed.
- User Experience Impact: removing legacy residue makes the redesign feel durable and complete rather than provisional.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/IconProvider.h`, `/Users/ryanrentfro/code/markamp/src/ui/FileIconResolver.h`, `/Users/ryanrentfro/code/markamp/src/ui/SidebarPanelRegistry.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/IconLibrary.cpp`
- Prior Plan References: all icon cleanup phases from earlier generations, `v26` Phase 20
- Scope: legacy icon chars, emoji/unicode surface prefixes, obsolete style fragments, duplicate icon systems, outdated visual branches on visible surfaces
- Out of Scope: content-side emoji features not used as UI chrome
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/IconProvider.h`, `/Users/ryanrentfro/code/markamp/src/ui/FileIconResolver.h`, `/Users/ryanrentfro/code/markamp/src/ui/SidebarPanelRegistry.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/IconLibrary.cpp`
- Related Screens / Components / Surfaces: every surface that formerly used legacy icon or style paths
- Current Visual Behavior / Appearance: the redesign may coexist with older systems unless cleanup is explicit and aggressive.
- Intended Visual Behavior / Appearance: the visible product runs on the redesigned systems only, without old visual residue peeking through.
- Icon / Visual Design Direction: one visual and icon world, not several historical layers.
- Technical Styling Approach: remove or isolate legacy assets and APIs only after redesigned replacements are fully validated on the visible path.
- Implementation Steps:
  1. Enumerate legacy icon and visual paths still reachable from visible surfaces.
  2. Remove, isolate, or permanently gate them.
  3. Verify the redesigned systems cover all former usage.
  4. Document any intentionally retained non-visible compatibility seams.
- Validation Steps:
  1. Search the visible UI code for banned icon chars and legacy style markers.
  2. Review representative screenshots for any remaining old-world artifacts.
  3. Confirm all major surfaces still render correctly after cleanup.
- Acceptance Criteria: no visible surface still depends on a legacy icon or visual system that `v27` was meant to replace.
- Dependencies: completion of the redesign phases and icon migration phases
- Parallelization Notes: can run late surface by surface, but final completion requires a full pass.
- Risks / Failure Modes: deleting old paths too early can expose hidden dependencies; deleting them too late leaves the redesign non-durable.
- Accessibility / Readability Notes: ensure cleanup does not remove accessibility-visible cues while retiring old systems.
- Theme / Styling Notes: cleanup should preserve only the redesigned semantic theme system.
- Motion / Interaction Notes where relevant: keep redesigned motion hooks while removing obsolete transition paths.
- Cleanup / Consolidation Notes where relevant: this is the primary consolidation task for `v27`.
- Rollback / Safety Notes: remove old systems incrementally and validate each family before proceeding.
- References / Context: this task determines whether the redesign is truly complete or only layered on top of legacy code.
- Example scenarios where useful: searching the visible UI code should no longer find emoji-based panel and status prefixes.

### Phase ID: V27-P20
### Task ID: V27-P20-T02
### Task Title: Build The Full Visual QA Matrix For Every Redesigned Surface And Journey
- Priority: P0
- Category: Cleanup / Consolidation
- Objective: create and run a visual QA matrix that validates the transformed product across all major user-visible journeys and states.
- Why This Matters Now: a full redesign requires explicit signoff against completeness criteria, not only “looks better” impressions.
- Visual Gap Statement: without a visual QA gate, residual weak surfaces, parity gaps, or icon inconsistencies can survive on low-frequency paths.
- User Experience Impact: users receive a uniformly transformed application rather than a selectively redesigned one.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/StartupPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp`
- Prior Plan References: `v25` release gate mindset adapted to UI, `v26` Phase 20
- Scope: screenshot matrix, interaction review matrix, icon completeness review, theme parity review, accessibility-visible review, edge-state review
- Out of Scope: backend E2E automation not needed for visual review
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/StartupPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/ThemedDialog.cpp`
- Related Screens / Components / Surfaces: startup, shell, editor, notebook, canvas, settings, panels, dialogs, feedback, empty states, menus, toolbars
- Current Visual Behavior / Appearance: the redesign phases define targets, but explicit cross-surface signoff still needs to be operationalized.
- Intended Visual Behavior / Appearance: every major surface and journey is verified against the `v27` definitions of visually transformed.
- Icon / Visual Design Direction: comprehensive signoff for icon and visual consistency, not only isolated surface review.
- Technical Styling Approach: create a QA matrix tied directly to the `v27` phase goals and acceptance criteria.
- Implementation Steps:
  1. Enumerate core user journeys and visual states.
  2. Define screenshot and interaction-review checkpoints for each.
  3. Include icon consistency, theme parity, accessibility visuals, and edge states in the matrix.
  4. Run the matrix and close any remaining redesign gaps.
- Validation Steps:
  1. Capture or manually review all matrix entries.
  2. Compare each against the `v27` definitions of transformed quality.
  3. Confirm all major gaps are closed before declaring completion.
- Acceptance Criteria: a full visual QA matrix exists and all major surfaces pass it without unresolved redesign-blocking issues.
- Dependencies: all prior redesign phases
- Parallelization Notes: matrix construction can start earlier, but final signoff is an end-stage task.
- Risks / Failure Modes: incomplete review coverage can leave visually old surfaces hidden in the product.
- Accessibility / Readability Notes: QA must include focus, contrast, icon clarity, and reduced-motion review.
- Theme / Styling Notes: theme parity review is mandatory in the matrix.
- Motion / Interaction Notes where relevant: include transitions and micro-interactions for high-frequency surfaces.
- Cleanup / Consolidation Notes where relevant: use the matrix to catch remaining legacy icon or visual residue.
- Rollback / Safety Notes: do not claim redesign completion before the matrix is executed.
- References / Context: this is the UI equivalent of a release gate, but for the full visual overhaul.
- Example scenarios where useful: startup, shell, editor, notebook, canvas, and settings should each be reviewed in normal, empty, and error-like states where applicable.

### Phase ID: V27-P20
### Task ID: V27-P20-T03
### Task Title: Define Surface Specific Visual Transformation Criteria And Close The Last Residual Gaps
- Priority: P1
- Category: Cleanup / Consolidation
- Objective: turn the `v27` redesign vision into explicit per-surface signoff criteria and use those criteria to close the last visible rough edges.
- Why This Matters Now: “fully transformed” must be measurable or the redesign will remain subjective and incomplete.
- Visual Gap Statement: even after large-scale redesign work, some surfaces can remain only partially transformed unless explicit signoff criteria are applied.
- User Experience Impact: users get a product whose major surfaces all feel equally redesigned rather than a mix of hero and lagging screens.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/TabBar.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/ExplorerPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/ThemedDialog.cpp`
- Prior Plan References: all `v27` phases and the earlier UI refinement packages they supersede
- Scope: per-surface transformed criteria for shell, menus, toolbars, tabs, editor, syntax, notebook, canvas, panels, settings, dialogs, feedback, onboarding, structured surfaces
- Out of Scope: non-visual functionality signoff
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/TabBar.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/ExplorerPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/ThemedDialog.cpp`
- Related Screens / Components / Surfaces: all major visible product surfaces
- Current Visual Behavior / Appearance: the redesign package defines high-level goals, but final transformed criteria still need to be made explicit and enforceable.
- Intended Visual Behavior / Appearance: each major surface has a clear “transformed” definition and no unresolved redesign-grade blocker remains.
- Icon / Visual Design Direction: explicit completion with no ambiguity about what a transformed MarkAmp surface looks like.
- Technical Styling Approach: derive surface criteria directly from the redesigned icon system, visual language, and surface-family work completed in earlier phases.
- Implementation Steps:
  1. Define transformed criteria for each major surface family.
  2. Review every major surface against those criteria.
  3. Close residual gaps or formally defer only genuinely non-critical follow-on polish.
  4. Record the final transformed baseline.
- Validation Steps:
  1. Perform a final cross-surface review using the QA matrix.
  2. Check for residual icon inconsistency, old-world styling, or parity holes.
  3. Confirm every major surface meets its transformed criteria.
- Acceptance Criteria: all major surface families have explicit transformation criteria and no unresolved redesign-blocking issue remains hidden.
- Dependencies: V27-P20-T02
- Parallelization Notes: final evaluation can run surface by surface, but final signoff is sequential.
- Risks / Failure Modes: vague signoff language will turn this into another open backlog rather than a completion gate.
- Accessibility / Readability Notes: transformed criteria must include focus, contrast, readability, and reduced-motion behavior where relevant.
- Theme / Styling Notes: every surface criterion must include theme correctness.
- Motion / Interaction Notes where relevant: high-frequency surfaces must include tactile and motion signoff.
- Cleanup / Consolidation Notes where relevant: transformed criteria should require removal of old icon and style residue.
- Rollback / Safety Notes: preserve a short and explicit deferred list rather than hiding unresolved gaps.
- References / Context: this is the final closure task for the `v27` planning package.
- Example scenarios where useful: a transformed canvas must require premium shell, tool, and overlay design, not just icon replacement and color cleanup.
