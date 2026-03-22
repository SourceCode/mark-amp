# Phase 06: Secondary Bottom And Specialized Panel Lifecycle Completion

## Outcome

Complete the lifecycle, docking, activation, and restore behavior of secondary sidebar panels, bottom panels, and specialized panel containers so they behave like real workbench surfaces instead of loosely attached widgets.

## Improvement Count

126 atomic improvements across 6 execution tasks.

### P06-T01

- Phase ID: P06
- Task ID: P06-T01
- Task Title: Replace secondary-sidebar placeholder panels with real surfaces or remove them
- Priority: P0
- Category: Panel Implementation
- Atomic Improvements Covered: 21
- Objective: Eliminate lightweight placeholder registrations in the secondary sidebar.
- Why This Matters Now: The secondary sidebar currently ships visible but shallow implementations for several panels.
- Problem Statement: `LayoutManager.cpp` registers custom stub-like secondary panels for outline, backlinks, and graph mini-map.
- User Impact: Secondary sidebar looks richer than it is.
- Scope: Secondary outline, backlinks, graph mini-map, and any other stub-like secondary panel registrations.
- Out of Scope: Full graph engine feature expansion.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`
- Related Systems / Components: Secondary sidebar, panel registries, outline, backlinks, graph.
- Current Behavior: Secondary panels are bespoke lightweight widgets instead of real panel hosts.
- Intended Behavior: Secondary panels are either fully integrated surfaces or absent from production navigation.
- Technical Approach: Replace registrations with real panel components or gate them until ready.
- Implementation Steps: Audit secondary registrations; wire real panel classes where they exist; remove inline placeholder builders.
- Validation Steps: Switch through every secondary panel and verify real behavior.
- Acceptance Criteria: No production secondary panel remains a lightweight stub shell.
- Dependencies: Phase 05.
- Risks / Failure Modes: Secondary sidebar may depend on placeholder-specific layout assumptions.
- UX Notes: Secondary panels should feel equal in credibility to primary panels.
- Settings / Panel / Control Notes where relevant: Secondary panel selection controls must not expose gated panels.
- Observability / Diagnostics Notes: Report secondary panel registrations and placeholder status in diagnostics.
- Rollback / Safety Notes: Feature-flag near-ready panels rather than shipping placeholders.
- References / Context: `LayoutManager.cpp` secondary registrations explicitly build small placeholder panels.
- Example scenarios where useful: Secondary Outline shows the same real outline data model as the dedicated outline surface.

### P06-T02

- Phase ID: P06
- Task ID: P06-T02
- Task Title: Strengthen bottom-panel registration, activation, and content-host contracts
- Priority: P0
- Category: Panel Lifecycle
- Atomic Improvements Covered: 21
- Objective: Make bottom-panel hosts behave consistently across Output, Problems, Terminal, Build, and Debug surfaces.
- Why This Matters Now: Bottom panels are a major daily workflow area and a common source of partial implementation.
- Problem Statement: `PanelService.cpp` is thin and `PanelContainer.cpp` only partially handles toolbars and lazy content.
- User Impact: Bottom panels can feel inconsistent in switching, focus, and toolbar behavior.
- Scope: Panel registration metadata, deferred creation, action toolbar ownership, active-panel synchronization.
- Out of Scope: New bottom-panel feature categories.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/PanelService.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/PanelContainer.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/PanelAreaModel.cpp`
- Related Systems / Components: Bottom panels, panel service, panel area model, panel container.
- Current Behavior: Panel hosting is functional but still thin in lifecycle semantics.
- Intended Behavior: Bottom-panel surfaces share one registration, activation, and toolbar-host contract.
- Technical Approach: Extend panel service metadata and make panel container enforce it consistently.
- Implementation Steps: Add panel capability metadata; unify deferred creation; fix toolbar registration and activation ordering; add destroy/recreate rules.
- Validation Steps: Switch rapidly between bottom panels and verify content/toolbars stay correct.
- Acceptance Criteria: Bottom-panel hosts are deterministic and lifecycle-safe.
- Dependencies: Phase 01 and Phase 05.
- Risks / Failure Modes: Existing plugin-panel integration may depend on thin assumptions.
- UX Notes: Bottom-panel switching should feel instant and state-preserving.
- Settings / Panel / Control Notes where relevant: Panel controls for show/hide/toggle should use this same lifecycle contract.
- Observability / Diagnostics Notes: Trace panel host activation and deferred creation results.
- Rollback / Safety Notes: Add adapters so old panel registrations still work during migration.
- References / Context: `PanelService.cpp` and `PanelContainer.cpp` reveal a thin lifecycle stack today.
- Example scenarios where useful: Switching from Terminal to Problems preserves each panel’s internal state and toolbar.

### P06-T03

- Phase ID: P06
- Task ID: P06-T03
- Task Title: Persist and restore bottom and secondary panel visibility, size, and active tab correctly
- Priority: P1
- Category: Docking / Layout
- Atomic Improvements Covered: 21
- Objective: Make non-primary panels reappear in the exact state users expect after restart.
- Why This Matters Now: Panel completeness includes docking and restore integrity.
- Problem Statement: Persistence is split across layout config keys, shell layout state, and panel-local assumptions.
- User Impact: Users lose work context when panel layout does not restore.
- Scope: Bottom panel visibility and height, secondary sidebar width and mode, active tab, registered panel order, action toolbar state.
- Out of Scope: Multi-window layout sync.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`; `/Users/ryanrentfro/code/markamp/src/core/ShellLayoutState.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/PanelLifecycleManager.cpp`
- Related Systems / Components: Layout manager, shell layout state, panel lifecycle manager, config.
- Current Behavior: State persistence exists but is spread across multiple local systems.
- Intended Behavior: Panel restore uses one durable contract and yields predictable layout.
- Technical Approach: Define a panel layout snapshot format owned by one layer and applied through panel services.
- Implementation Steps: Inventory persisted keys; unify snapshot schema; save active/visible/sized state; restore after panel registration completes.
- Validation Steps: Change bottom/secondary panel state, restart, and compare restored layout.
- Acceptance Criteria: Non-primary panels restore exactly and repeatably.
- Dependencies: P06-T02.
- Risks / Failure Modes: Restore order may race with lazy panel creation.
- UX Notes: Restore should not flash through incorrect intermediate states.
- Settings / Panel / Control Notes where relevant: Settings panels should not unexpectedly hijack restored panel state.
- Observability / Diagnostics Notes: Add panel restore traces with timing and missing-panel reasons.
- Rollback / Safety Notes: Support legacy restore keys for one transition cycle.
- References / Context: `PanelLifecycleManager.cpp` is currently memory-only and not the full restore owner.
- Example scenarios where useful: User leaves Terminal active in the bottom area and Search active in the secondary sidebar and gets both back after restart.

### P06-T04

- Phase ID: P06
- Task ID: P06-T04
- Task Title: Complete panel show/hide/toggle commands and chrome controls across non-primary panels
- Priority: P1
- Category: Docking / Layout
- Atomic Improvements Covered: 21
- Objective: Ensure every panel toggle, collapse, close, pin, and show command works consistently.
- Why This Matters Now: Panel-level affordances are high-risk for misleading UI.
- Problem Statement: Panel lifecycle state is not yet canonical enough to guarantee correct toggle behavior.
- User Impact: Panels may appear to toggle while leaving incorrect visibility or focus state behind.
- Scope: Show/hide/toggle commands, close controls, panel header buttons, pin/unpin if supported, keyboard toggles.
- Out of Scope: New docking paradigms.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/SidebarCommandProvider.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/PanelHeader.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`
- Related Systems / Components: Panel commands, headers, shell layout state, activity bar.
- Current Behavior: Toggling and activation behavior remains partly distributed.
- Intended Behavior: Panel controls change panel state through one shared panel lifecycle path.
- Technical Approach: Route all panel toggles through panel services and shell layout state.
- Implementation Steps: Audit panel commands; replace local toggles; unify panel header controls; add panel-state feedback.
- Validation Steps: Toggle primary, secondary, and bottom panels from multiple surfaces and compare results.
- Acceptance Criteria: No panel toggle control leaves visibility or focus state ambiguous.
- Dependencies: P06-T02 and P06-T03.
- Risks / Failure Modes: Hidden assumptions in `LayoutManager` could create oscillating state.
- UX Notes: Panel show/hide should be reversible and predictable.
- Settings / Panel / Control Notes where relevant: Settings-related panels must obey the same lifecycle if kept as panels.
- Observability / Diagnostics Notes: Trace panel-state transitions and control sources.
- Rollback / Safety Notes: Keep explicit panel-state guards to prevent recursive toggles.
- References / Context: Sidebar and panel commands already exist but are not fully lifecycle-backed.
- Example scenarios where useful: A shortcut, activity bar click, and panel header close button all update the same panel visibility state.

### P06-T05

- Phase ID: P06
- Task ID: P06-T05
- Task Title: Finish stub and partial specialized panels in the secondary and bottom workbench
- Priority: P1
- Category: Panel Implementation
- Atomic Improvements Covered: 21
- Objective: Identify and either complete or remove specialized panels that still expose partial workflows.
- Why This Matters Now: The specialized panel layer is where unfinished UI often hides.
- Problem Statement: History, PDF viewer, and similar specialized panels are explicit stubs or partials.
- User Impact: Users hit dead ends in advanced but visible workbench surfaces.
- Scope: History, PDF viewer, graph mini surfaces, backlinks mini surfaces, any specialized registered panel that is still placeholder-backed.
- Out of Scope: Deep domain-feature completion beyond making the panel honest and functional.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/HistoryPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/PDFViewerPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`
- Related Systems / Components: Specialized panels, sidebar registry, bottom panels.
- Current Behavior: Some specialized panels are explicit stubs.
- Intended Behavior: Specialized panels are either real surfaces or removed from exposure.
- Technical Approach: Classify each specialized panel as finish, gate, or remove from production entry points.
- Implementation Steps: Audit specialized panels; wire real content where available; gate stubs; remove dead commands and menu items that expose them.
- Validation Steps: Attempt to open every specialized panel from all visible entry points.
- Acceptance Criteria: No user-visible specialized panel remains stub-backed.
- Dependencies: Phase 05 and panel/menu audit phases.
- Risks / Failure Modes: Removing panel entry points may affect existing docs or habits.
- UX Notes: Honesty beats breadth for these surfaces.
- Settings / Panel / Control Notes where relevant: Settings should not advertise features whose panels remain gated.
- Observability / Diagnostics Notes: Flag any registered specialized panel lacking content readiness.
- Rollback / Safety Notes: Gate behind experimental flags if partial but still valuable for internal builds.
- References / Context: `HistoryPanel.cpp` and `PDFViewerPanel.cpp` are explicitly marked stub.
- Example scenarios where useful: Opening Document History yields a real workflow or no production affordance.

### P06-T06

- Phase ID: P06
- Task ID: P06-T06
- Task Title: Add panel-lifecycle smoke coverage for non-primary workbench surfaces
- Priority: P0
- Category: Diagnostics / Regression Protection
- Atomic Improvements Covered: 21
- Objective: Make panel lifecycle and restore reliability testable for secondary and bottom hosts.
- Why This Matters Now: Lifecycle regressions in complex panel areas are easy to miss manually.
- Problem Statement: Panel hosting and restore depend on many moving parts and local assumptions.
- User Impact: Panels disappear, restore incorrectly, or show wrong toolbars.
- Scope: Register, create, activate, toggle, resize, restore, destroy, re-open flows for non-primary panels.
- Out of Scope: Deep content correctness inside every panel.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/tests/unit`; `/Users/ryanrentfro/code/markamp/src/ui/PanelContainer.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`
- Related Systems / Components: Panel container, panel service, layout manager, shell state.
- Current Behavior: Non-primary panel lifecycle is not clearly release-gated.
- Intended Behavior: CI catches lifecycle and restore regressions before release.
- Technical Approach: Add host-level tests and restore snapshot verification.
- Implementation Steps: Define panel-host fixtures; test toggles and restore; test deferred creation; verify toolbar host switching.
- Validation Steps: Run panel lifecycle suite on clean and restored sessions.
- Acceptance Criteria: Non-primary panel lifecycle regressions become release-blocking.
- Dependencies: P06-T01 through P06-T05.
- Risks / Failure Modes: Tests may be brittle if panel activation is still too implicit.
- UX Notes: Include visible-state and focus assertions, not just internal state.
- Settings / Panel / Control Notes where relevant: Include settings-related panel hosts if they remain in scope.
- Observability / Diagnostics Notes: Capture panel traces and registry snapshots on failure.
- Rollback / Safety Notes: Keep tests centered on public panel contracts.
- References / Context: Secondary and bottom panel complexity now warrants direct harness coverage.
- Example scenarios where useful: CI toggles the bottom panel, switches active tabs, restarts, and verifies the same panel returns.
