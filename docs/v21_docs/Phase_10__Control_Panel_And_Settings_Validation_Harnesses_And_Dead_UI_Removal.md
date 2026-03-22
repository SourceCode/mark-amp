# Phase 10: Control Panel And Settings Validation Harnesses And Dead UI Removal

## Outcome

Make dead UI impossible to ship by adding release-gating validation around controls, panels, and settings, then removing or gating every remaining pseudo-surface that fails those checks.

## Improvement Count

132 atomic improvements across 6 execution tasks.

### P10-T01

- Phase ID: P10
- Task ID: P10-T01
- Task Title: Build a release-gating control and handler completeness matrix
- Priority: P0
- Category: Diagnostics / Regression Protection
- Atomic Improvements Covered: 22
- Objective: Track every visible control from affordance to resolved handler and resulting state change.
- Why This Matters Now: v21 only succeeds if dead UI becomes mechanically visible and unacceptable.
- Problem Statement: The repository still allows visible actions to exist without a verified end-to-end execution path.
- User Impact: Users click controls that look complete but terminate in missing handlers, weak events, or no state changes.
- Scope: Menus, toolbar actions, status actions, inline buttons, panel controls, settings controls, quick actions, context menus.
- Out of Scope: Product analytics unrelated to integrity validation.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/StatusBarPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp`; `/Users/ryanrentfro/code/markamp/tests/unit`
- Related Systems / Components: Control manifest, command routing, panel lifecycle, settings architecture, CI.
- Current Behavior: Control completeness has to be inferred manually from code inspection and scattered runtime behavior.
- Intended Behavior: A generated matrix lists each control, its canonical action ID, its handler, its enablement source, its target context, and its validation status.
- Technical Approach: Generate the matrix from the phase-01 control manifest plus runtime traces and targeted smoke results.
- Implementation Steps: Extend control-manifest output; add handler-resolution checks; merge smoke status; publish CI artifact; fail CI for unresolved P0/P1 controls.
- Validation Steps: Intentionally leave one control unbound and verify the matrix flags it as unresolved with source details.
- Acceptance Criteria: Teams can identify every unresolved or duplicate control path from one generated report.
- Dependencies: Phase 01 through Phase 09.
- Risks / Failure Modes: If action IDs are not canonical enough, the matrix may duplicate or misclassify controls.
- UX Notes: Internal integrity reporting indirectly improves user trust by preventing dead affordances from shipping.
- Settings / Panel / Control Notes where relevant: The matrix must separate control, panel, and settings statuses while preserving cross-links between them.
- Observability / Diagnostics Notes: Emit structured JSON and readable markdown output for the completeness matrix.
- Rollback / Safety Notes: Start as warning-level in local dev but fail CI for release profiles.
- References / Context: v21 requires an authoritative blueprint plus enforcement, not just another audit memo.
- Example scenarios where useful: The matrix shows that a Git submenu item exists in `MainFrame.cpp` but has no bound command handler and no smoke coverage.

### P10-T02

- Phase ID: P10
- Task ID: P10-T02
- Task Title: Add panel completeness and placeholder-surface gates
- Priority: P0
- Category: Diagnostics / Regression Protection
- Atomic Improvements Covered: 22
- Objective: Ensure no registered panel ships with placeholder content, missing lifecycle ownership, or dead header actions.
- Why This Matters Now: Panels are one of the most convincing forms of misleading UI because they consume full workbench real estate.
- Problem Statement: Placeholder primary and secondary panels, as well as explicit stub panels, still exist in production-adjacent code paths.
- User Impact: Users can navigate into large dead-end surfaces and lose trust in the workbench quickly.
- Scope: Primary, secondary, bottom, and specialized panel registrations; panel content readiness; header action readiness; restore eligibility.
- Out of Scope: Deep domain-feature validation inside already-real panels.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/SidebarPanelRegistry.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/PanelContainer.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/HistoryPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/PDFViewerPanel.cpp`
- Related Systems / Components: Panel registry, panel lifecycle, layout manager, diagnostics.
- Current Behavior: Placeholder panel factories and explicit stubs can still appear in user-visible pathways.
- Intended Behavior: CI and startup diagnostics identify any panel exposed without a real implementation and lifecycle contract.
- Technical Approach: Add panel readiness metadata, startup registration assertions, and smoke fixtures that open every exposed panel.
- Implementation Steps: Define panel-readiness schema; mark placeholder/stub panels; add startup and test-time assertions; fail release builds for exposed unresolved panels.
- Validation Steps: Leave a placeholder panel exposed in test mode and verify the readiness gate blocks release.
- Acceptance Criteria: No production-registered panel can be placeholder-backed without an explicit experimental gate.
- Dependencies: Phase 05 through Phase 09.
- Risks / Failure Modes: Near-ready experimental panels may need a clean gating story to avoid blocking internal development.
- UX Notes: Removing misleading panels is better than shipping aspirational chrome.
- Settings / Panel / Control Notes where relevant: Settings-related pseudo-panels must be treated with the same strictness as any other dead panel.
- Observability / Diagnostics Notes: Report panel ID, registration source, readiness state, and blocking reason.
- Rollback / Safety Notes: Support explicit experimental or internal-build flags for incomplete panels that are not meant for production.
- References / Context: `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/HistoryPanel.cpp`, and `/Users/ryanrentfro/code/markamp/src/ui/PDFViewerPanel.cpp` already show the need for hard gating.
- Example scenarios where useful: CI fails if the Themes sidebar is still registered through a generic placeholder helper.

### P10-T03

- Phase ID: P10
- Task ID: P10-T03
- Task Title: Add settings persistence, deep-link, and runtime-apply release gates
- Priority: P0
- Category: Diagnostics / Regression Protection
- Atomic Improvements Covered: 22
- Objective: Treat settings save/apply/restore correctness as a release-blocking contract.
- Why This Matters Now: Settings bugs destabilize the entire product because they affect every surface.
- Problem Statement: The settings stack still contains duplicate ownership, placeholder export behavior, weak deep-link routing, and incomplete live-apply coverage.
- User Impact: Users cannot stabilize the application if settings do not save, restore, or apply reliably.
- Scope: Persistence round-trips, staged apply/cancel semantics, startup restore, deep-link routing, JSON-editor integrity, live-apply and restart-required signaling.
- Out of Scope: Cloud sync or account-scoped settings distribution.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/SettingsDialog.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/SettingsJsonEditor.cpp`; `/Users/ryanrentfro/code/markamp/src/core/SettingsStateOwner.cpp`; `/Users/ryanrentfro/code/markamp/tests/unit`
- Related Systems / Components: Settings UI, settings persistence, settings deep links, live reactor, diagnostics.
- Current Behavior: Settings correctness is improving through v21, but it still needs hard release gates.
- Intended Behavior: CI and startup diagnostics verify that visible settings controls and entry points are complete, persisted, and applied correctly.
- Technical Approach: Turn key settings flows into smoke suites and startup self-checks linked to canonical setting IDs and scopes.
- Implementation Steps: Define critical settings paths; add persistence and deep-link tests; verify runtime-apply handlers; report missing restart metadata or rogue direct writes.
- Validation Steps: Run the settings gate suite on clean profile, migrated profile, and workspace-scoped fixtures.
- Acceptance Criteria: Release builds fail when critical settings flows save incorrectly, route incorrectly, or apply incorrectly.
- Dependencies: Phase 08 and Phase 09.
- Risks / Failure Modes: Overly broad gating could temporarily expose more legacy debt than the team can clear in one pass, so severity needs triage.
- UX Notes: Strong internal gating is justified because settings failure destroys user trust far beyond the settings surface itself.
- Settings / Panel / Control Notes where relevant: This gate should treat settings controls as first-class product controls, not backend implementation details.
- Observability / Diagnostics Notes: Publish structured settings-integrity reports with failing keys, scopes, and apply outcomes.
- Rollback / Safety Notes: Gate the strictest checks in release CI first while keeping local developer workflows informative rather than blocked.
- References / Context: The current stack already shows why settings need the same rigor as command and panel validation.
- Example scenarios where useful: CI opens a deep-linked settings host, changes a value, applies it, restarts, and confirms the effective value plus runtime side effect.

### P10-T04

- Phase ID: P10
- Task ID: P10-T04
- Task Title: Remove, hide, or explicitly gate every remaining dead or misleading affordance
- Priority: P0
- Category: Cleanup / Consolidation
- Atomic Improvements Covered: 22
- Objective: Convert audit findings into a hard product-cleanup pass that removes deceptive UI.
- Why This Matters Now: Diagnostics are only useful if unresolved dead UI is actually removed from production surfaces.
- Problem Statement: Many controls and panels currently survive because they are visible but not yet release-gated.
- User Impact: Users lose trust fastest when the UI advertises capabilities the product cannot actually deliver.
- Scope: Dead menus, unbound toolbar buttons, misleading status actions, placeholder panels, unsupported quick fixes, pseudo-settings hosts, duplicate entry points to dead behavior.
- Out of Scope: Implementing every deferred feature rather than honestly gating it.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/ProblemsPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp`
- Related Systems / Components: Menus, toolbar, status bar, panels, settings, command routing.
- Current Behavior: Some dead UI is known but still visible because there is no final removal sweep.
- Intended Behavior: Production UI contains only controls and surfaces that meet the v21 integrity bar or are explicitly hidden behind experimental gates.
- Technical Approach: Use the control and panel completeness reports to drive a dead-UI removal backlog and enforce zero unresolved P0 misleading affordances for release.
- Implementation Steps: Classify unresolved controls/panels/settings by implement vs gate vs remove; update menus and navigation; remove duplicate dead entry points; verify discoverability remains coherent.
- Validation Steps: Compare visible UI against the completeness matrix and verify no unresolved P0/P1 affordance remains exposed in release configuration.
- Acceptance Criteria: No visible control or panel in the production build is known-dead, pseudo-backed, or misleading.
- Dependencies: P10-T01 through P10-T03.
- Risks / Failure Modes: Removing affordances without adjusting surrounding information architecture can make navigation feel sparse if not handled carefully.
- UX Notes: Honest omission is better than aspirational clutter.
- Settings / Panel / Control Notes where relevant: If a settings category or panel is gated, related menus, toolbar buttons, and palette commands must also be gated.
- Observability / Diagnostics Notes: Track removed or gated affordances so regressions reintroducing them are obvious.
- Rollback / Safety Notes: Use feature flags for internal-only surfaces rather than deleting code paths needed for active development.
- References / Context: v21 explicitly treats dead UI removal as part of completion, not as optional polish.
- Example scenarios where useful: The product no longer exposes a `Quick Fix` context action when no quick-fix provider exists for the selected diagnostic.

### P10-T05

- Phase ID: P10
- Task ID: P10-T05
- Task Title: Publish a control, panel, and settings quality scoreboard for implementation teams
- Priority: P1
- Category: Diagnostics / Regression Protection
- Atomic Improvements Covered: 22
- Objective: Give implementation teams a live operational view of completeness, risk, and remaining blockers.
- Why This Matters Now: The v21 wave spans many surfaces and needs a concise operating view beyond raw task documents.
- Problem Statement: Without a scoreboard, progress and blockers across controls, panels, and settings remain too diffuse to manage efficiently.
- User Impact: Indirect but important; weak internal visibility lets quality debt linger into user-facing builds.
- Scope: Completion percentages, blocker counts, unresolved dead UI counts, by-surface status, by-phase status, ownership/status metadata.
- Out of Scope: End-user analytics or telemetry dashboards.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/docs/v21_docs`; `/Users/ryanrentfro/code/markamp/tests/unit`; `/Users/ryanrentfro/code/markamp/scripts/smoke_test.sh`
- Related Systems / Components: Documentation, diagnostics, CI artifacts, planning package.
- Current Behavior: Planning documents exist, but there is no consolidated quality scoreboard driven by execution status.
- Intended Behavior: Teams can see at a glance which control surfaces, panels, or settings areas still block release.
- Technical Approach: Generate a scoreboard from the completeness matrix, smoke suites, and panel/settings readiness reports.
- Implementation Steps: Define scoreboard schema; aggregate outputs; publish markdown and machine-readable versions; highlight P0/P1 blockers and trends.
- Validation Steps: Mark sample items complete and incomplete and verify the scoreboard updates accordingly.
- Acceptance Criteria: Implementation teams can identify highest-risk unresolved areas from one generated artifact.
- Dependencies: P10-T01 through P10-T04.
- Risks / Failure Modes: Scoreboards can become vanity metrics if they do not tie directly to release gates and unresolved blockers.
- UX Notes: Internal clarity here improves user-facing quality by keeping execution disciplined.
- Settings / Panel / Control Notes where relevant: Scoreboard segments should match the v21 surface taxonomy so ownership is obvious.
- Observability / Diagnostics Notes: Include links from scoreboard items back to source files, failing tests, and canonical action IDs.
- Rollback / Safety Notes: Keep scoreboard generation read-only and deterministic.
- References / Context: The user requested an execution blueprint that clearly tells the team what to do next and how to validate it.
- Example scenarios where useful: The scoreboard shows `Settings Deep Links` and `Problems Quick Fixes` as remaining P0 blockers while most toolbar actions have cleared.

### P10-T06

- Phase ID: P10
- Task ID: P10-T06
- Task Title: Define and enforce the v21 exit criteria for production control integrity
- Priority: P0
- Category: Cleanup / Consolidation
- Atomic Improvements Covered: 22
- Objective: Turn the v21 planning package into a concrete release bar for controls, panels, and settings.
- Why This Matters Now: Without explicit exit criteria, the wave can drift into endless partial cleanup without a real definition of done.
- Problem Statement: The app currently lacks one enforceable product bar for menu trustworthiness, panel completeness, and settings correctness.
- User Impact: Users continue to encounter inconsistent or misleading surfaces if completion is judged loosely.
- Scope: Release criteria, blocker definitions, acceptable experimental gating rules, validation requirements, sign-off artifacts.
- Out of Scope: General product strategy beyond the v21 control/panel/settings mission.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/docs/v21_docs/README__MarkAmp_V21_Execution_Master_Index.md`; `/Users/ryanrentfro/code/markamp/docs/v21_docs/ASSESSMENT__MarkAmp_V21_Control_Panels_And_Settings_Assessment.md`; `/Users/ryanrentfro/code/markamp/tests/unit`
- Related Systems / Components: Planning docs, CI, diagnostics, control manifest, panel readiness, settings integrity.
- Current Behavior: Quality goals exist in prose, but they need a hard operational sign-off contract.
- Intended Behavior: v21 exits only when every exposed control and panel is real or gated, and settings are canonical, persistent, and validated.
- Technical Approach: Encode explicit exit gates tied to the completeness matrix, panel readiness checks, settings smoke suites, and dead-UI removal results.
- Implementation Steps: Define required reports and test suites; set release-blocking thresholds; document waiver rules for experimental-only surfaces; enforce sign-off in CI and release checklists.
- Validation Steps: Simulate unresolved blocker states and verify the exit criteria correctly block release.
- Acceptance Criteria: The team has one clear, enforceable definition of done for the v21 wave.
- Dependencies: P10-T01 through P10-T05.
- Risks / Failure Modes: If the exit criteria are vague or too permissive, the wave will not materially improve product trustworthiness.
- UX Notes: Strong exit criteria are justified because these surfaces are foundational to how users judge the entire IDE.
- Settings / Panel / Control Notes where relevant: Exit criteria must require parity across menus, buttons, panels, and settings rather than allowing one area to lag silently.
- Observability / Diagnostics Notes: Exit status should be generated from artifacts, not manual interpretation alone.
- Rollback / Safety Notes: Keep waiver mechanisms explicit, time-bounded, and limited to non-production experimental surfaces.
- References / Context: The v21 mission is completion of controls, panels, and settings, not another advisory iteration.
- Example scenarios where useful: Release is blocked until the settings dialog cancel flow, the Search sidebar async flow, and the remaining unbound Git menu items all clear their respective gates.
