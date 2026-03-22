# Phase 04: Shell Action, Menu, Toolbar, And Context Authority

## Phase Intent

Make one action model authoritative across all visible shell entry points.

## Release-Ready Exit Criteria

- menus, toolbars, palette commands, and context menus route through one manifest-backed action model,
- enablement and visibility are context-correct,
- direct widget-only release-path handlers are retired or reduced to thin delegates.

## Task Count

3

## Task P04-T01

- Phase ID: P04
- Task ID: P04-T01
- Task Title: Adopt `ActionManifest` as the canonical action registry for release-path commands
- Priority: P0
- Category: Menu / Command Hardening
- Objective: unify visible shell actions under one registry with shared metadata, handler ownership, and enablement rules.
- Why This Matters Now: action duplication remains one of the biggest coherence risks in the workbench.
- Release Gap Statement: command registration and execution still diverge between menu bindings, palette registrations, toolbar callbacks, and surface-local code.
- User / Product Impact: inconsistent behavior by entry point makes the IDE feel unreliable.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/core/ActionManifest.cpp`; `/Users/ryanrentfro/code/markamp/src/core/MenuCommandBinder.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`; `/Users/ryanrentfro/code/markamp/tests/unit/test_v24_p04_action_manifest.cpp`
- Prior Plan References: `v19 Phase 02`; `v21 Phase 01`; `v24 Phase 04`
- Scope: release-path file/edit/view/notebook/canvas/workspace actions.
- Out of Scope: hidden debug and experimental commands.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/WorkbenchCommands.cpp`; `/Users/ryanrentfro/code/markamp/src/core/ActionManifest.h`; `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`
- Related Features / Systems / Components: action registry; menu binding; command palette; toolbar
- Current Behavior: a new action layer exists, but legacy bindings remain widely active.
- Intended Release-Ready Behavior: each visible action has one manifest entry and all surfaces delegate to it.
- Missing Pieces: action inventory migration; handler centralization; context enablement.
- Technical Approach: create a release-path action manifest bootstrap and convert legacy bindings into manifest delegates.
- Implementation Steps:
1. Enumerate release-path actions and map them into the manifest.
2. Replace direct binding sites with manifest execution.
3. Remove duplicate command definitions where possible.
- Validation Steps:
1. Trigger the same action from menu, toolbar, palette, and context menu.
2. Verify identical behavior and state updates.
- Acceptance Criteria: visible release-path actions resolve through one registry and do not have duplicate behavior by surface.
- Dependencies: P01-T02
- Parallelization Notes: can proceed alongside panel host work.
- Risks / Failure Modes: action ID drift; missing enablement wiring; orphaned legacy handlers.
- Observability / Diagnostics Notes: log action source surface and manifest ID.
- Rollback / Safety Notes: preserve compatibility wrappers while migrating surface-by-surface.
- References / Context: `docs/v21_docs/Phase_01__Control_Audit_Spine_And_Canonical_Action_Model.md`

## Task P04-T02

- Phase ID: P04
- Task ID: P04-T02
- Task Title: Normalize menu, toolbar, and context-menu enablement against live shell context
- Priority: P0
- Category: Core Workflow Hardening
- Objective: ensure actions are only available when their target artifact, panel, or selection context supports them.
- Why This Matters Now: release-path dead or misleading affordances remain a major trust problem.
- Release Gap Statement: visible actions can still imply capabilities that are unavailable or routed incorrectly.
- User / Product Impact: wrong enablement creates accidental errors, false confidence, and no-op UI.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/core/ContextKeyService.h`; `/Users/ryanrentfro/code/markamp/src/core/SurfaceActionAuditor.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/ContextMenuRegistry.cpp`
- Prior Plan References: `v21 Phase 02`; `v21 Phase 04`; `v24 Phase 04`
- Scope: artifact-scoped actions, notebook actions, canvas actions, panel toggles, selection-sensitive actions.
- Out of Scope: future extension-contributed conditions.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/ContextKeyService.cpp`; `/Users/ryanrentfro/code/markamp/src/core/WhenClause.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/TabBar.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/FileTreeCtrl.cpp`
- Related Features / Systems / Components: context keys; action enablement; shell focus; selection context
- Current Behavior: surface-local enablement and direct callbacks still compete with central readiness checks.
- Intended Release-Ready Behavior: enablement is derived from shared shell context and matches the actual capability state.
- Missing Pieces: complete context key updates; per-surface integration; audit coverage.
- Technical Approach: define canonical context keys for active artifact kind, focus, selection state, and panel availability, then bind action enablement to them.
- Implementation Steps:
1. Define the release-path context key set.
2. Update shell surfaces to publish context changes.
3. Rebind enablement logic to manifest predicates.
- Validation Steps:
1. Verify enablement while switching between editor, notebook, canvas, explorer, and empty shell states.
2. Audit visible actions for wrong enabled state.
- Acceptance Criteria: release-path actions are neither dead-when-enabled nor hidden-when-valid.
- Dependencies: P04-T01
- Parallelization Notes: pairs well with panel host cleanup.
- Risks / Failure Modes: stale context; focus drift; mismatched selection state.
- Observability / Diagnostics Notes: expose current context keys in debug diagnostics.
- Rollback / Safety Notes: keep fallback disablement for ambiguous contexts until coverage is complete.
- References / Context: `/Users/ryanrentfro/code/markamp/src/core/ContextKeyService.h`

## Task P04-T03

- Phase ID: P04
- Task ID: P04-T03
- Task Title: Retire release-path direct widget bindings that bypass canonical commands
- Priority: P1
- Category: Cleanup / De-duplication
- Objective: remove or isolate direct shell bindings that still bypass action manifest routing.
- Why This Matters Now: without retiring these paths, command drift will keep reappearing.
- Release Gap Statement: direct widget wiring still allows old and new control models to diverge.
- User / Product Impact: the same button or menu item can behave differently from another entry point.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/TabBar.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/FileTreeCtrl.cpp`
- Prior Plan References: `v21 Phase 03`; `v21 Phase 04`; `v24 Phase 04`
- Scope: only release-path commands and shell-visible controls.
- Out of Scope: control internals that do not trigger product actions.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/FileTreeCtrl.cpp`
- Related Features / Systems / Components: command routing; widget bindings; event bus usage
- Current Behavior: direct bindings remain common in high-visibility surfaces.
- Intended Release-Ready Behavior: UI controls delegate to canonical actions, not their own business logic.
- Missing Pieces: binding inventory; migration wrappers; regression tests.
- Technical Approach: replace direct business logic with thin manifest or command-dispatch adapters, then remove dead fallback code.
- Implementation Steps:
1. Search and classify release-path direct bindings.
2. Convert them to canonical action dispatch.
3. Delete duplicate logic after validation.
- Validation Steps:
1. Compare behavior across entry points before and after migration.
2. Run action audit to ensure no orphaned UI remains.
- Acceptance Criteria: release-path shell controls no longer own separate business logic.
- Dependencies: P04-T01; P04-T02
- Parallelization Notes: can be split by surface ownership.
- Risks / Failure Modes: hidden regressions in old callbacks; duplicate event publication.
- Release Notes / Cleanup Notes: document any intentionally retained direct bindings and why.
- Observability / Diagnostics Notes: add audit output listing remaining bypass paths.
- Rollback / Safety Notes: migrate incrementally by surface.
- References / Context: `docs/v21_docs/ASSESSMENT__MarkAmp_V21_Control_Panels_And_Settings_Assessment.md`
