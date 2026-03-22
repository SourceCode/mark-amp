# Phase 02: Action Control Menu And Panel Readiness Closure

## Outcome

Remove dead, stubbed, unbound, or misleading action and panel states from production by turning the existing audit models into enforced completion work.

## Completion Count

300 atomic completion tasks across 5 execution tasks.

### P02-T01

- Phase ID: P02
- Task ID: P02-T01
- Task Title: Reconcile control manifests with all visible action surfaces
- Priority: P0
- Category: Command Chain Completion
- Atomic Completion Tasks Covered: 60
- Objective: Ensure every visible action maps to a real manifest action with a real handler.
- Why This Matters Now: The code already detects missing handlers, but still allows them to persist.
- Completion Gap Statement: Visible controls can still exist without manifest entries or executable handlers.
- User / Product Impact: Menus, buttons, palette items, and context actions can look complete while doing nothing.
- Repository Evidence: [SurfaceActionAuditor.cpp](/Users/ryanrentfro/code/markamp/src/core/SurfaceActionAuditor.cpp), [ControlActionManifest.cpp](/Users/ryanrentfro/code/markamp/src/core/ControlActionManifest.cpp), [MainFrame.cpp](/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp), [Toolbar.cpp](/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp)
- Scope: Main menus, toolbar, command palette, context menus, panel actions, status actions, notebook and canvas actions.
- Out of Scope: New feature design beyond already visible action surfaces.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/SurfaceActionAuditor.cpp`; `/Users/ryanrentfro/code/markamp/src/core/ControlActionManifest.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`
- Related Features / Systems / Components: Controls, commands, menus, shell actions.
- Current Behavior: Audit helpers can report dead or placeholder actions after the fact.
- Intended Completed Behavior: No visible action surface remains unmanifested, unbound, or handlerless.
- Missing Pieces: Canonical registration sweep, handler parity checks, and blocker gating.
- Technical Approach: Use the action manifest as the single executable contract and reconcile every visible surface against it.
- Implementation Steps: Inventory visible actions; sync manifests; remove direct orphan callbacks; bind missing handlers; gate or hide unresolved actions.
- Validation Steps: Run control audits and verify zero production-surface dead or placeholder actions remain.
- Acceptance Criteria: Control audits pass for all visible surfaces.
- Dependencies: Phase 01.
- Risks / Failure Modes: Hidden duplicate handlers can mask unresolved action ownership.
- Cleanup / Migration Notes where relevant: Remove legacy direct-bind code once the manifest path is authoritative.
- Observability / Diagnostics Notes where relevant: Export per-surface dead-action reports in CI.
- Rollback / Safety Notes: Prefer gating unresolved actions over leaving misleading live controls.
- References / Context: The repo already contains the audit machinery needed to finish this.
- Example scenarios where useful: A toolbar button and its menu twin both route through the same command contract.

### P02-T02

- Phase ID: P02
- Task ID: P02-T02
- Task Title: Eliminate menu binding stubs and category-path drift
- Priority: P0
- Category: Command Chain Completion
- Atomic Completion Tasks Covered: 60
- Objective: Make menu models trustworthy by removing stub, orphaned, or misleading bindings.
- Why This Matters Now: Menu breadth is already large enough that unimplemented items actively harm trust.
- Completion Gap Statement: Menu binding diagnostics already know about missing and stub handlers, but those states still exist.
- User / Product Impact: Menus imply feature completeness that is not always real.
- Repository Evidence: [MenuCommandBinder.cpp](/Users/ryanrentfro/code/markamp/src/core/MenuCommandBinder.cpp), [ContextMenuActionBinder.cpp](/Users/ryanrentfro/code/markamp/src/core/ContextMenuActionBinder.cpp), [test_v21_menu_palette.cpp](/Users/ryanrentfro/code/markamp/tests/unit/test_v21_menu_palette.cpp)
- Scope: Main menu, submenus, overflow menus, and context-menu bindings.
- Out of Scope: New menu IA unrelated to existing commands.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/MenuCommandBinder.cpp`; `/Users/ryanrentfro/code/markamp/src/core/ContextMenuActionBinder.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`; `/Users/ryanrentfro/code/markamp/tests/unit/test_v21_menu_palette.cpp`
- Related Features / Systems / Components: Menus, palette, context menus.
- Current Behavior: Stub handlers and synthetic menu paths can still coexist with production menus.
- Intended Completed Behavior: Menu trees expose only finished actions with correct labels, categories, and enablement.
- Missing Pieces: Binding cleanup, path normalization, and removal of manifest/menu divergence.
- Technical Approach: Drive menu bindings from canonical action metadata and reject unresolved stubs in production menus.
- Implementation Steps: Diff menu trees against manifest; normalize paths; remove orphaned IDs; complete missing handlers; gate unfinished menu items.
- Validation Steps: Run menu diagnostics and verify no stub or orphan entries remain in production menus.
- Acceptance Criteria: Menu binding reports no production stub actions.
- Dependencies: P02-T01.
- Risks / Failure Modes: Category normalization can accidentally collapse intentional submenu structure.
- Cleanup / Migration Notes where relevant: Delete legacy menu IDs once migration is complete.
- Observability / Diagnostics Notes where relevant: Add menu diff reports between manifest and rendered menu models.
- Rollback / Safety Notes: Hide unresolved menu items if completion is not yet feasible.
- References / Context: This closes the specific menu-trust gap called out in `v21`.
- Example scenarios where useful: A Git menu no longer contains a visible action backed only by `kStub`.

### P02-T03

- Phase ID: P02
- Task ID: P02-T03
- Task Title: Finish panel toggle ownership, readiness gating, and missing-handler cleanup
- Priority: P0
- Category: Panel Completion
- Atomic Completion Tasks Covered: 60
- Objective: Ensure every visible panel has a real implementation, a real toggle, and a real lifecycle path.
- Why This Matters Now: The code already models stub panels and missing toggle handlers explicitly.
- Completion Gap Statement: Panels can still be registered, navigable, or visible while being stubs, placeholders, or missing command wiring.
- User / Product Impact: The shell can expose incomplete product areas as if they were finished.
- Repository Evidence: [PanelCapabilityModel.cpp](/Users/ryanrentfro/code/markamp/src/core/PanelCapabilityModel.cpp), [PanelLifecycleAuditor.cpp](/Users/ryanrentfro/code/markamp/src/core/PanelLifecycleAuditor.cpp), [LayoutManager.cpp](/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp)
- Scope: Primary, secondary, and bottom panels plus specialized panel toggles.
- Out of Scope: Net-new panel concepts not already exposed by the product.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/PanelCapabilityModel.cpp`; `/Users/ryanrentfro/code/markamp/src/core/PanelLifecycleAuditor.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/PanelContainer.cpp`
- Related Features / Systems / Components: Panel registry, layout, activity bar, panel toggles.
- Current Behavior: Stub and placeholder panels are detectable but not yet fully purged from production pathways.
- Intended Completed Behavior: Production-visible panels are real, toggleable, and lifecycle-complete.
- Missing Pieces: Toggle binding cleanup, factory cleanup, gating rules, and placeholder replacement.
- Technical Approach: Make readiness diagnostics drive panel registration and release gating instead of passive reporting.
- Implementation Steps: Audit visible panels; complete or gate stubs; wire missing toggles; remove placeholder factories; align panel registry with lifecycle status.
- Validation Steps: Run panel capability and lifecycle audits until no production stubs remain.
- Acceptance Criteria: No visible panel fails readiness checks.
- Dependencies: Phase 01.
- Risks / Failure Modes: Hiding unresolved panels without replacement can break discoverability or user workflows.
- Cleanup / Migration Notes where relevant: Retire inline placeholder factories inside `LayoutManager`.
- Observability / Diagnostics Notes where relevant: Publish panel readiness reports with host-area breakdowns.
- Rollback / Safety Notes: Gate unfinished panels rather than leaving them accessible and misleading.
- References / Context: The repo already knows which panels are incomplete; this task removes the excuse not to act on that knowledge.
- Example scenarios where useful: An outline or history panel is either finished or absent, not visible-but-thin.

### P02-T04

- Phase ID: P02
- Task ID: P02-T04
- Task Title: Replace placeholder panel factories and helper shells with canonical surface implementations
- Priority: P1
- Category: Panel Completion
- Atomic Completion Tasks Covered: 60
- Objective: Remove the remaining inline placeholder shells from panel registration paths.
- Why This Matters Now: Placeholder factories make the shell look broader than the product really is.
- Completion Gap Statement: Several panel registrations still build simple fallback shells directly inside the layout layer.
- User / Product Impact: Users can reach panels that are still mostly scaffolding.
- Repository Evidence: [LayoutManager.cpp](/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp), [HistoryPanel.cpp](/Users/ryanrentfro/code/markamp/src/ui/HistoryPanel.cpp), [PDFViewerPanel.cpp](/Users/ryanrentfro/code/markamp/src/ui/PDFViewerPanel.cpp)
- Scope: Outline, backlinks, graph minimap, history, PDF, diff, and similar specialized panels.
- Out of Scope: General panel styling refinements already covered in prior plans.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/HistoryPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/PDFViewerPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/DiffPanel.h`
- Related Features / Systems / Components: Specialized panels, shell integration, panel factories.
- Current Behavior: Inline shell construction stands in for real feature panels.
- Intended Completed Behavior: Registered panels are backed by dedicated, fully wired panel classes or are hidden until they are.
- Missing Pieces: Real factories, real services, and gating logic for unfinished panels.
- Technical Approach: Move placeholder panel creation out of layout registration and replace it with real panel classes or production gating.
- Implementation Steps: Inventory inline panel lambdas; map each to a target panel class; complete missing services; remove placeholder lambdas.
- Validation Steps: Verify registered panel factories instantiate real components with real data paths.
- Acceptance Criteria: No production panel is created through a temporary placeholder lambda.
- Dependencies: P02-T03.
- Risks / Failure Modes: Factory replacement can surface deeper missing-service dependencies.
- Cleanup / Migration Notes where relevant: Remove one-off empty-state shell code duplicated in `LayoutManager`.
- Observability / Diagnostics Notes where relevant: Tag panel registrations with readiness metadata during startup.
- Rollback / Safety Notes: Defer panel exposure if its backing service is still incomplete.
- References / Context: This phase converts shell breadth into honest shell completeness.
- Example scenarios where useful: The graph mini-map is either a real graph navigator or not registered.

### P02-T05

- Phase ID: P02
- Task ID: P02-T05
- Task Title: Turn control and panel audits into hard completion gates
- Priority: P1
- Category: Diagnostics / Recovery Completion
- Atomic Completion Tasks Covered: 60
- Objective: Stop treating action and panel audits as passive reports.
- Why This Matters Now: Audit models already exist; the missing piece is operational enforcement.
- Completion Gap Statement: The product can still ship with known dead controls and stub panels because audit outputs are not hard blockers.
- User / Product Impact: Trust suffers when the repo knows about incomplete surfaces but still exposes them.
- Repository Evidence: [ControlCompletenessMatrix.cpp](/Users/ryanrentfro/code/markamp/src/core/ControlCompletenessMatrix.cpp), [ReleaseGateChecker.cpp](/Users/ryanrentfro/code/markamp/src/ui/ReleaseGateChecker.cpp), [VisualCleanupCoordinator.cpp](/Users/ryanrentfro/code/markamp/src/core/VisualCleanupCoordinator.cpp)
- Scope: Action audits, menu audits, panel audits, release checks, CI summaries.
- Out of Scope: Non-control subsystem gates handled in later phases.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/ControlCompletenessMatrix.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/ReleaseGateChecker.cpp`; `/Users/ryanrentfro/code/markamp/src/core/VisualCleanupCoordinator.cpp`; `/Users/ryanrentfro/code/markamp/tests/unit/test_v21_validation_harness.cpp`
- Related Features / Systems / Components: Validation, CI, release readiness.
- Current Behavior: Audits produce diagnostics, but not hard completion pressure.
- Intended Completed Behavior: Production-visible dead controls and stub panels fail completion gates.
- Missing Pieces: Thresholds, CI hooks, waiver policy, and surfacing in release reports.
- Technical Approach: Promote audit summaries into phase-aware completion gates.
- Implementation Steps: Define blocker thresholds; wire audits into CI; publish failures; require explicit waivers for gated exceptions.
- Validation Steps: Seed a known stub control and verify the gate fails.
- Acceptance Criteria: Control and panel completion debt can no longer regress silently.
- Dependencies: P02-T01 through P02-T04.
- Risks / Failure Modes: Overly broad gating can block work-in-progress branches without useful signal.
- Cleanup / Migration Notes where relevant: Replace manual audit spot-checks with automated gate output.
- Observability / Diagnostics Notes where relevant: Export blocker counts by surface family.
- Rollback / Safety Notes: Start with warning mode, then ratchet to blocking mode once the baseline is understood.
- References / Context: This is how `v23` converts audit knowledge into product completion.
- Example scenarios where useful: A new menu item without a handler fails CI before it reaches users.
