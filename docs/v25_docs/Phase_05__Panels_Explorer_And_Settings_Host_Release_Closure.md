# Phase 05: Panels, Explorer, And Settings Host Release Closure

## Phase Intent

Remove panel and settings-host ambiguity from the release path.

## Release-Ready Exit Criteria

- every visible release-path panel has a real factory, restore lifecycle, and non-placeholder content,
- explorer and search sidebars no longer lean on temporary assumptions,
- settings host behavior is authoritative and visible entry points no longer over-promise.

## Task Count

3

## Task P05-T01

- Phase ID: P05
- Task ID: P05-T01
- Task Title: Replace placeholder or stubbed release-path panel registrations with real implementations or explicit gating
- Priority: P0
- Category: Panel Hardening
- Objective: ensure no release-path navigation entry opens a placeholder panel or a stub shell.
- Why This Matters Now: visible placeholder panels are one of the clearest release blockers in the workbench.
- Release Gap Statement: panel readiness infrastructure exists, but placeholder factories and stubbed panels still remain visible.
- User / Product Impact: users lose trust when navigation opens thin or unfinished content.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/HistoryPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/PDFViewerPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/core/PanelLifecycleAuditor.cpp`
- Prior Plan References: `v21 Phase 05`; `v21 Phase 06`; `v23 Phase 08`; `v24 Phase 05`
- Scope: explorer-adjacent panels, search sidebar, problems, output, terminal, source control, history, PDF, settings host.
- Out of Scope: explicitly gated future-only specialty panels.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/SearchSidebarPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/ProblemsPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/ExplorerPanel.cpp`
- Related Features / Systems / Components: panel registry; docking; restore; readiness gate
- Current Behavior: panel registration breadth exceeds real panel depth.
- Intended Release-Ready Behavior: each visible panel is real, restorable, and appropriately scoped for release.
- Missing Pieces: panel inventory classification; factory replacement or gating; readiness enforcement.
- Technical Approach: use the panel readiness gate to force either completion or explicit removal from the release path.
- Implementation Steps:
1. Enumerate release-path panels and classify them as real, must-finish, or gated.
2. Replace placeholder factories where required.
3. Remove navigation affordances for gated-out panels.
- Validation Steps:
1. Open every visible release-path panel from its shell entry point.
2. Verify content, restore, and toggle behavior.
- Acceptance Criteria: no release-path panel is placeholder-backed or stubbed.
- Dependencies: P01-T02; P04-T01
- Parallelization Notes: can be split by panel family.
- Risks / Failure Modes: hidden placeholder registrations; panel toggles without factories.
- Release Notes / Cleanup Notes: document any gated panels removed from release scope.
- Observability / Diagnostics Notes: panel audit report must list visible placeholder panels as blockers.
- Rollback / Safety Notes: prefer gating/removal over shipping misleading shells.
- References / Context: `/Users/ryanrentfro/code/markamp/src/core/ActionReadinessGate.h`

## Task P05-T02

- Phase ID: P05
- Task ID: P05-T02
- Task Title: Harden explorer, search sidebar, and panel restore lifecycle
- Priority: P0
- Category: Panel Hardening
- Objective: make explorer-adjacent surfaces dependable in creation, selection, search, restore, and panel-state continuity.
- Why This Matters Now: these are daily-use shell workflows and must be trustworthy before release.
- Release Gap Statement: panel lifecycle and content ownership are still inconsistent, especially around placeholder search-root and restore assumptions.
- User / Product Impact: broken explorer and sidebar behavior destabilizes the whole IDE shell.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/ExplorerPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/SearchSidebarPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`
- Prior Plan References: `v19 Phase 05`; `v21 Phase 07`; `v24 Phase 05`; `v24 Phase 10`
- Scope: explorer tree, search sidebar roots, panel restore positions, panel reopen behavior.
- Out of Scope: feature-expansion work unrelated to release-path trust.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/ExplorerPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/SearchSidebarPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/SidebarPanelRegistry.cpp`
- Related Features / Systems / Components: explorer; search sidebar; panel restore; docking lifecycle
- Current Behavior: search sidebar still carries placeholder async and root assumptions; panel restore authority is not fully dependable.
- Intended Release-Ready Behavior: panel content and restore state are deterministic and workspace-aware.
- Missing Pieces: workspace-aware root ownership; restore-state validation; panel smoke tests.
- Technical Approach: bind these panels to workspace/session state rather than local defaults and enforce restorable panel contracts.
- Implementation Steps:
1. Replace placeholder search-root assumptions with workspace context.
2. Validate panel snapshots before restore.
3. Add reopen and restore smoke tests.
- Validation Steps:
1. Restart with various panel arrangements and search contexts.
2. Verify explorer and search panels reopen correctly and target the active workspace.
- Acceptance Criteria: explorer and search sidebars restore correctly and no longer rely on placeholder defaults.
- Dependencies: P03-T03; P05-T01
- Parallelization Notes: can proceed while settings host work starts.
- Risks / Failure Modes: stale restore snapshots; bad workspace root assumptions.
- Observability / Diagnostics Notes: include restored panel IDs and skipped invalid snapshots in logs.
- Rollback / Safety Notes: fall back to safe default layout when validation fails.
- References / Context: `/Users/ryanrentfro/code/markamp/src/ui/SearchSidebarPanel.cpp`

## Task P05-T03

- Phase ID: P05
- Task ID: P05-T03
- Task Title: Establish one real settings host path and remove mixed staged-versus-immediate behavior
- Priority: P0
- Category: Settings Hardening
- Objective: make the visible settings experience flow through one host and one staged-change ownership model.
- Why This Matters Now: settings remain a direct contradiction between UX promise and implementation reality.
- Release Gap Statement: settings surfaces still split between direct config mutation, pending-change staging, and weak deep-link routing.
- User / Product Impact: settings trust affects restart behavior, theme changes, editor preferences, and workspace-scoped correctness.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/SettingsDialog.cpp`; `/Users/ryanrentfro/code/markamp/src/core/SettingsStateOwner.cpp`; `/Users/ryanrentfro/code/markamp/src/core/SettingsDeepLinkRouter.cpp`
- Prior Plan References: `v21 Phase 08`; `v21 Phase 09`; `v24 Phase 05`; `v24 Phase 13`
- Scope: settings host opening, apply, cancel, dirty indicators, deep links, active category selection.
- Out of Scope: advanced settings marketplace or sync scenarios.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/SettingsCatalog.cpp`; `/Users/ryanrentfro/code/markamp/src/core/SettingsCatalogBootstrap.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/SettingsJsonEditor.cpp`
- Related Features / Systems / Components: settings state owner; dialog host; deep-link routing; config mutation
- Current Behavior: settings UI exposes staged semantics while core and panel code still allow immediate mutation patterns.
- Intended Release-Ready Behavior: all visible settings edits are staged, validated, applied, persisted, or cancelled through one owner.
- Missing Pieces: host unification; panel adoption of `SettingsStateOwner`; removal of direct config mutation paths.
- Technical Approach: route visible settings edits through `SettingsStateOwner` and make the dialog/panel host the only release-path entry.
- Implementation Steps:
1. Audit and replace direct config mutation in settings UI.
2. Bind dialog and panel actions to the state owner.
3. Verify deep-link routing opens the canonical host and selects the right section.
- Validation Steps:
1. Edit settings, cancel, apply, restart, and confirm persistence.
2. Deep-link into settings from multiple entry points.
- Acceptance Criteria: visible settings behavior is staged, coherent, and host-authoritative.
- Dependencies: P04-T01
- Parallelization Notes: deeper settings export/scope work continues in Phase 13.
- Risks / Failure Modes: mixed ownership persists; deep links target stale surfaces.
- Observability / Diagnostics Notes: log staged, applied, cancelled, and invalid settings mutations.
- Rollback / Safety Notes: preserve read-only fallback for invalid settings sections.
- References / Context: `docs/v21_docs/ASSESSMENT__MarkAmp_V21_Control_Panels_And_Settings_Assessment.md`
