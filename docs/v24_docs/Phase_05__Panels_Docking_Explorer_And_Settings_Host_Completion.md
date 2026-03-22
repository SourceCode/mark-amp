# Phase 05 - Panels, Docking, Explorer, And Settings Host Completion

## Phase Goal

Finish panel and settings host authority so visible shells stop acting like placeholders, deferred experiments, or split ownership zones.

## Measurable Outcome

- Registered panels reflect real lifecycle status.
- Explorer and settings hosts are authoritative, not thin shells over duplicate logic.
- Docking and visibility behavior aligns with panel readiness and restore rules.

### Task P05-T01

- Phase ID: `P05`
- Task ID: `P05-T01`
- Task Title: Align panel registry, capability model, and actual panel factories
- Priority: `P0`
- Category: `Panel Completion`
- Objective: Ensure registered panels and visible panels represent the same truth.
- Why This Matters Now: Panel lifecycle diagnostics already exist but do not yet match real factory behavior consistently.
- Execution Gap Statement: Deferred and placeholder panel factories in the shell can drift from capability and readiness models.
- User / Product Impact: Users can open panels that are visually present but not truly implemented.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`, `/Users/ryanrentfro/code/markamp/src/core/PanelCapabilityModel.cpp`
- Prior Plan References: `v21` Phase 05-07, `v23` Phase 08
- Scope: Panel registration, placeholder detection, factory ownership, host-area mapping
- Out of Scope: Surface-specific feature completion inside each panel
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/SidebarPanelRegistry.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/PanelLifecycleManager.cpp`
- Related Features / Systems / Components: Panels, docking, registry, lifecycle audits
- Current Behavior: Registry and visible shell can disagree about which panels are real.
- Intended Completed Behavior: A panel is either fully registered and live, explicitly gated, or absent from production UI.
- Missing Pieces: Unified readiness metadata and host integration
- Technical Approach: Make panel factories declare readiness and capability state into one registry consumed by shell hosts.
- Implementation Steps: Normalize panel metadata; bind registry to capability model; gate placeholder panels out of production navigation.
- Validation Steps: Compare shell panel lists to capability diagnostics automatically.
- Acceptance Criteria: No production-visible panel lacks a live readiness state and factory contract.
- Dependencies: `P01-T04`, `P04-T03`
- Parallelization Notes: Enables later specialized panel completion.
- Risks / Failure Modes: Placeholder panels can still leak into edge navigation paths.
- Cleanup / Migration Notes: Remove ad hoc placeholder factory helpers once unified registration exists.
- Observability / Diagnostics Notes: Emit panel registry snapshots and readiness diffs.
- Rollback / Safety Notes: Allow temporary hidden registration while migrating old factories.
- References / Context: Panel capability and lifecycle systems
- Example scenarios where useful: Graph mini-map remains hidden until its factory is live, not shown as a blank secondary-sidebar panel.

### Task P05-T02

- Phase ID: `P05`
- Task ID: `P05-T02`
- Task Title: Finish explorer host ownership and remove file-tree bypass flows
- Priority: `P0`
- Category: `Panel Completion`
- Objective: Make explorer actions and state flow through canonical artifact and command systems.
- Why This Matters Now: Explorer remains one of the most-used surfaces and still performs direct file operations.
- Execution Gap Statement: `/Users/ryanrentfro/code/markamp/src/ui/FileTreeCtrl.cpp` still performs direct create/rename/delete work and local dummy-node logic.
- User / Product Impact: Explorer behavior can diverge from shell and artifact lifecycle rules.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/FileTreeCtrl.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/ExplorerPanel.cpp`
- Prior Plan References: `v20` Phase 02 and 06, `v21` Phase 07
- Scope: Explorer create/open/rename/delete/move commands, selection, refresh, empty-state actions
- Out of Scope: Tree row visual polish beyond functionally necessary state
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/FileOperationService.h`, `/Users/ryanrentfro/code/markamp/src/core/WorkspaceOpenOrchestrator.cpp`
- Related Features / Systems / Components: Explorer, file operations, artifact lifecycle
- Current Behavior: File tree still performs direct file system operations and UI-local temporary state management.
- Intended Completed Behavior: Explorer becomes a shell view over canonical file and artifact operations.
- Missing Pieces: Operation delegation, artifact sync, selection propagation
- Technical Approach: Delegate explorer operations to file and artifact services; keep the tree as projection and interaction shell only.
- Implementation Steps: Migrate create/rename/delete/move; unify open/select behavior; wire refresh and error states from service results.
- Validation Steps: Exercise all explorer operations and compare results to command-palette and menu-driven flows.
- Acceptance Criteria: Explorer no longer owns direct workflow logic that bypasses artifact or command authority.
- Dependencies: `P02-T02`, `P04-T02`
- Parallelization Notes: Can proceed with project-navigation work in Phase 10.
- Risks / Failure Modes: Tree responsiveness can regress if async/service callbacks are not integrated cleanly.
- Cleanup / Migration Notes: Remove inline dummy-file creation branches after service delegation lands.
- Observability / Diagnostics Notes: Log explorer-originated operation requests and outcomes.
- Rollback / Safety Notes: Preserve undoable rename/move/delete safety checks.
- References / Context: File operation and workspace orchestration services
- Example scenarios where useful: Creating a file from the explorer produces the same artifact registration and tab behavior as creating it from `File > New File`.

### Task P05-T03

- Phase ID: `P05`
- Task ID: `P05-T03`
- Task Title: Make settings host ownership authoritative across dialog, panel, and deep links
- Priority: `P0`
- Category: `Settings Completion`
- Objective: Remove the split between staged settings UI promises and immediate config mutation behavior.
- Why This Matters Now: Settings are still one of the clearest examples of visible UX promising more than the implementation delivers.
- Execution Gap Statement: `SettingsDialog` presents staged lifecycle semantics while `SettingsPanel` and related editors still mutate config directly in places.
- User / Product Impact: Users cannot trust apply/cancel/reset/export behavior.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/SettingsDialog.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/core/SettingsStateOwner.cpp`
- Prior Plan References: `v21` Phase 08-09, `v23` Phase 10
- Scope: Settings host lifecycle, staged changes, apply/cancel/reset, deep-link entry
- Out of Scope: Theme/gallery visual polish
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/SettingsDeepLinkRouter.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/SettingsJsonEditor.cpp`
- Related Features / Systems / Components: Settings UI, config ownership, deep links
- Current Behavior: Settings ownership is split across dialog, panel, JSON editor, and state owner.
- Intended Completed Behavior: One settings host owns staged changes, runtime application, persistence, and deep-link resolution.
- Missing Pieces: Host contract adoption and legacy direct-write removal
- Technical Approach: Make `SettingsStateOwner` the true source of staged state and use the host to mediate all apply/revert flows.
- Implementation Steps: Route all settings edits through state owner; update dialog/panel/json editor; fix deep-link host resolution.
- Validation Steps: Change settings, cancel, apply, undo, redo, import, and export through all entry paths.
- Acceptance Criteria: No visible settings surface bypasses the canonical settings host and staged-state contract.
- Dependencies: `P04-T01`
- Parallelization Notes: Can proceed with deeper config/schema work in Phase 13.
- Risks / Failure Modes: Mixed staged and direct-write logic can create phantom unsaved indicators or incorrect revert behavior.
- Cleanup / Migration Notes: Remove direct config mutation from settings widgets once host routing is stable.
- Observability / Diagnostics Notes: Track staged-change count, apply outcomes, revert actions, and deep-link routes.
- Rollback / Safety Notes: Keep a compatibility import/export path during migration.
- References / Context: V21 settings lifecycle contradictions
- Example scenarios where useful: Opening settings from a command deep link and from the toolbar lands in the same host with the same staged-state behavior.

### Task P05-T04

- Phase ID: `P05`
- Task ID: `P05-T04`
- Task Title: Finish docking, visibility, and restore rules for primary and secondary panels
- Priority: `P1`
- Category: `Panel Lifecycle`
- Objective: Make panel activation, hide/show, size restore, and side swapping predictable.
- Why This Matters Now: Panels cannot be considered complete if their lifecycle still drifts by host area or mode.
- Execution Gap Statement: Shell docking and restore behavior remains partially implemented and partly local to the shell.
- User / Product Impact: Panels open in surprising places or lose state across sessions.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/WorkspaceLayout.cpp`
- Prior Plan References: `v19` Phase 04, `v21` Phase 05-06
- Scope: Primary sidebar, secondary sidebar, bottom panel, split sizing, restore rules
- Out of Scope: New docking paradigms beyond current shell model
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/SecondarySidebarTabStrip.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/PanelContainer.cpp`
- Related Features / Systems / Components: Docking, layout persistence, panel activation
- Current Behavior: Some shell state is saved, but lifecycle authority and restore semantics are still uneven.
- Intended Completed Behavior: Panel visibility, host area, active tab, and sizing restore consistently.
- Missing Pieces: Shared panel layout contract and restore ordering
- Technical Approach: Couple panel readiness and host metadata to layout persistence and restore logic.
- Implementation Steps: Define host-state schema; persist active/visible/sizing state; restore in panel-ready order; add failure fallbacks.
- Validation Steps: Reconfigure panels, restart app, and verify layout restoration across multiple workbench modes.
- Acceptance Criteria: Panel docking and restore behavior is deterministic and consistent across host areas.
- Dependencies: `P05-T01`, `P03-T03`
- Parallelization Notes: Can progress with panel host and explorer work.
- Risks / Failure Modes: Restoring hidden or not-yet-ready panels can corrupt shell state.
- Cleanup / Migration Notes: Remove ad hoc host-specific restore logic after unified schema lands.
- Observability / Diagnostics Notes: Emit panel layout snapshots and restore failures.
- Rollback / Safety Notes: Fallback to safe default layout when restore is invalid.
- References / Context: Workspace layout manager and shell hosts
- Example scenarios where useful: Moving Search to the secondary sidebar remains stable across restart and workbench-mode switches.

### Task P05-T05

- Phase ID: `P05`
- Task ID: `P05-T05`
- Task Title: Gate placeholder and thin panels out of production navigation until complete
- Priority: `P1`
- Category: `Placeholder / Stub Removal`
- Objective: Stop incomplete panels from shipping as if they were finished.
- Why This Matters Now: Panels such as history and PDF remain visible risk surfaces.
- Execution Gap Statement: Stub and placeholder panels still exist in visible panel ecosystems.
- User / Product Impact: Users lose trust when panel affordances open unfinished shells.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/HistoryPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/PDFViewerPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/core/PanelCapabilityModel.cpp`
- Prior Plan References: `v21` assessment, `v23` Phase 08 and 20
- Scope: Gating, hiding, warning, or explicit experimental labeling of incomplete panels
- Out of Scope: Completing panel internals themselves
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/SpecializedSurfaceCompletionAuditor.h`
- Related Features / Systems / Components: Panel visibility, specialized surfaces, release gating
- Current Behavior: Some incomplete panels are still reachable or too close to production state.
- Intended Completed Behavior: Only live panels appear as standard product surfaces.
- Missing Pieces: Gating policy, shell visibility integration, audit-to-UI enforcement
- Technical Approach: Drive panel visibility from readiness metadata and explicit release policy.
- Implementation Steps: Inventory incomplete panels; define policy; hide or relabel them; update tests and menus.
- Validation Steps: Attempt to open gated panels from menus, commands, and restored layouts.
- Acceptance Criteria: Stub or placeholder panels are either completed, clearly gated, or absent from production navigation.
- Dependencies: `P05-T01`, `P01-T04`
- Parallelization Notes: Can land in parallel with specialized panel completion later.
- Risks / Failure Modes: Hidden panels can still leak through deep links or layout restore.
- Cleanup / Migration Notes: Remove temporary shells once full replacements land.
- Observability / Diagnostics Notes: Log attempted access to gated panels.
- Rollback / Safety Notes: Support explicit developer-mode overrides only outside release paths.
- References / Context: Specialized surface auditor and panel readiness model
- Example scenarios where useful: `PDF Viewer` is not offered in normal panel toggles until page render, thumbnails, and annotations are real.
