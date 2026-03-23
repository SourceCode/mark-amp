# Phase 20 - Visual Cleanup Consolidation Legacy Style Removal And UI QA Gates

## Goal

Retire residual visual debt, remove duplicate style fragments, and define the final acceptance gate for an elite and visually complete MarkAmp UI.

## Definition Of Done

- visible legacy style fragments and duplicate style systems are removed or retired
- the remaining UI is validated against explicit visual-completeness gates
- `v26` ends with a durable UI baseline rather than a pile of isolated patches

## Tasks

### Phase ID: V26-P20
### Task ID: V26-P20-T01
### Task Title: Retire Duplicate Local Style Systems And Legacy Visual Fragments
- Priority: P0
- Category: Cleanup / Consolidation
- Objective: Remove obsolete local styling branches and duplicate component-level visual systems that remain after convergence.
- Why This Matters Now: Premium finish will not stay durable if old style paths remain in place.
- UI Gap Statement: The repository contains many locally evolved styling patterns that can continue to drift even after visible polish lands.
- User Experience Impact: Consolidation reduces future regressions and keeps UI quality stable after `v26`.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/ThemeGallery.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/SearchSidebarPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`
- Prior Plan References: `v22` cleanup work, `v25` release-readiness de-duplication, `v26` all prior phases
- Scope: duplicated style helpers, obsolete local constants, fallback visual branches, no-longer-used placeholder styling
- Out of Scope: functional backend cleanup unrelated to visible UI
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/ThemeGallery.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/SearchSidebarPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`
- Related Screens / Components / Surfaces: shell, settings, theme gallery, panels, command surfaces
- Current UI Behavior / Appearance: local style duplication still exists even where newer shared systems are available.
- Intended UI Behavior / Appearance: major surfaces depend on shared visual systems, not retained historical branches.
- Visual / Interaction Design Direction: durable coherence through consolidation.
- Technical Styling Approach: remove or collapse obsolete style code only after replacement paths are verified.
- Implementation Steps:
  1. Identify no-longer-needed local visual helpers and constants.
  2. Remove or merge them into shared systems.
  3. Re-test affected surfaces.
  4. Document any intentionally retained exceptions.
- Validation Steps:
  1. Run screenshot review across touched surfaces.
  2. Confirm no dead or fallback visual branches remain on the visible path.
  3. Verify build and visual behavior remain stable.
- Acceptance Criteria: duplicate or obsolete visible-surface styling paths are materially reduced and the shared system is the clear source of truth.
- Dependencies: completion of the major convergence and surface polish phases
- Parallelization Notes: can run component by component late in the cycle.
- Risks / Failure Modes: removing old paths too early can expose gaps on less-tested surfaces.
- Accessibility / Readability Notes: ensure cleanup does not remove focus or contrast safeguards.
- Theme / Styling Notes: keep semantic tokens, not ad hoc replacements, as the remaining abstraction layer.
- Motion / Interaction Notes where relevant: preserve reduced-motion and shared motion hooks while cleaning up old branches.
- Cleanup / Consolidation Notes where relevant: this is the primary consolidation task for `v26`.
- Rollback / Safety Notes: remove old styling incrementally with screenshot-based review after each cleanup set.
- References / Context: this task turns `v26` from a polish wave into a durable UI baseline.
- Example scenarios where useful: settings and theme gallery should stop carrying custom styling logic that the rest of the app no longer needs.

### Phase ID: V26-P20
### Task ID: V26-P20-T02
### Task Title: Build A Visual QA Matrix For Core Journeys And Surface Completeness
- Priority: P0
- Category: Cleanup / Consolidation
- Objective: Define and execute a structured visual review matrix covering all release-critical surfaces from a UI perspective.
- Why This Matters Now: Elite UI quality requires explicit validation, not only incremental refinement.
- UI Gap Statement: Without a formal UI QA gate, residual roughness and parity gaps can survive in low-frequency but high-visibility surfaces.
- User Experience Impact: The final product feels uniformly premium rather than polished only on its main path.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/StartupPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp`
- Prior Plan References: `v25` release gating mindset adapted here for UI-only validation
- Scope: screenshot matrix, journey checklist, theme parity checklist, edge-state checklist, visual-completeness signoff
- Out of Scope: backend test automation beyond what supports screenshot capture or review
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/StartupPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp`
- Related Screens / Components / Surfaces: startup, shell, editor, notebook, canvas, settings, panels, dialogs, feedback, empty states
- Current UI Behavior / Appearance: visual review is possible, but a unified signoff framework is not yet defined in this package.
- Intended UI Behavior / Appearance: every major surface is reviewed against explicit premium-quality criteria before `v26` is considered complete.
- Visual / Interaction Design Direction: comprehensive visual signoff focused on cohesion, precision, and premium finish.
- Technical Styling Approach: create a review matrix tied to the `v26` definitions of visually complete.
- Implementation Steps:
  1. Enumerate core UI journeys and surfaces.
  2. Define screenshot states for normal, empty, and error conditions.
  3. Review both themes and accessibility-visual states.
  4. Track and close any residual gaps before signoff.
- Validation Steps:
  1. Capture or manually review the full matrix.
  2. Check each surface against `v26` acceptance criteria.
  3. Confirm completion of any remaining polish fixes.
- Acceptance Criteria: a documented UI QA matrix exists and all core journeys pass it with no major unresolved premium-quality gaps.
- Dependencies: all prior phases materially complete
- Parallelization Notes: can start as a checklist earlier, but final signoff belongs at the end.
- Risks / Failure Modes: incomplete review coverage can leave visible roughness in low-frequency surfaces.
- Accessibility / Readability Notes: include focus, contrast, and reduced-motion review in the matrix.
- Theme / Styling Notes: parity review is mandatory in the matrix.
- Motion / Interaction Notes where relevant: include interaction quality checks for high-frequency surfaces.
- Cleanup / Consolidation Notes where relevant: use the matrix to catch residual duplication or placeholder visuals.
- Rollback / Safety Notes: do not declare `v26` complete without running the matrix.
- References / Context: this is the UI equivalent of a release gate.
- Example scenarios where useful: startup, editor, notebook, and canvas should each be reviewed in both themes and in at least one edge state.

### Phase ID: V26-P20
### Task ID: V26-P20-T03
### Task Title: Define Final Visual Acceptance Criteria For Each Major Surface And Close Residual Gaps
- Priority: P1
- Category: Cleanup / Consolidation
- Objective: Turn the `v26` vision into explicit visual signoff criteria per major surface and use it to close the last remaining rough edges.
- Why This Matters Now: A premium UI plan must end with explicit completion criteria, not subjective “looks better” judgments.
- UI Gap Statement: Surfaces improve quickly during a polish wave, but without explicit end-state criteria, quality remains uneven and open-ended.
- User Experience Impact: Users get a cohesive product where every major surface feels complete, not selectively polished.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/TabBar.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/ExplorerPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp`
- Prior Plan References: all `v26` phases; `v22` broad UI completion goals narrowed and operationalized here
- Scope: per-surface completion criteria for shell, editor, notebook, canvas, panels, settings, overlays, feedback, and empty states
- Out of Scope: new UI features
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/TabBar.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/ExplorerPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp`
- Related Screens / Components / Surfaces: all core UI surfaces
- Current UI Behavior / Appearance: there are strong improvements and explicit phase goals, but final surface signoff criteria still need to be made operational.
- Intended UI Behavior / Appearance: each major surface has clear visual-completeness criteria and no unresolved premium-quality blockers.
- Visual / Interaction Design Direction: explicit completion with no ambiguity about what “elite” means for each surface.
- Technical Styling Approach: derive signoff criteria directly from the shared visual system and surface-specific `v26` work.
- Implementation Steps:
  1. Define visual-complete criteria for each major surface.
  2. Review each surface against those criteria.
  3. Capture and close residual gaps.
  4. Record any intentional follow-up items that are truly beyond `v26`.
- Validation Steps:
  1. Perform a final cross-surface review.
  2. Confirm no visible placeholder, major inconsistency, or theme-parity blocker remains.
  3. Ensure acceptance criteria are testable through direct observation.
- Acceptance Criteria: every major surface has explicit signoff criteria and any residual gaps are either closed or formally deferred with rationale.
- Dependencies: V26-P20-T02
- Parallelization Notes: final evaluation work can run surface by surface, but final signoff is sequential.
- Risks / Failure Modes: vague signoff language can turn this into another open-ended backlog.
- Accessibility / Readability Notes: include focus, contrast, and edge-state quality in each surface definition.
- Theme / Styling Notes: every surface criterion must include theme parity.
- Motion / Interaction Notes where relevant: include tactile and reduced-motion criteria for high-frequency surfaces.
- Cleanup / Consolidation Notes where relevant: ensure no legacy style fragments remain on signoff surfaces.
- Rollback / Safety Notes: preserve a short list of intentionally deferred UI items rather than hiding them.
- References / Context: this is the final closure task for the `v26` planning package.
- Example scenarios where useful: shell signoff should require premium spacing and panel cohesion, while canvas signoff should require premium tool and selection affordances.
