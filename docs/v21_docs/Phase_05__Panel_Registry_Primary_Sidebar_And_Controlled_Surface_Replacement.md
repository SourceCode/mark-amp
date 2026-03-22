# Phase 05: Panel Registry Primary Sidebar And Controlled Surface Replacement

## Outcome

Replace primary-sidebar placeholder panels with real hosts or remove them from production, and strengthen the primary panel registry from a lazy widget map into a reliable shell surface system.

## Improvement Count

126 atomic improvements across 6 execution tasks.

### P05-T01

- Phase ID: P05
- Task ID: P05-T01
- Task Title: Eliminate placeholder `make_feature_panel` surfaces from the primary sidebar
- Priority: P0
- Category: Panel Implementation
- Atomic Improvements Covered: 21
- Objective: Remove panels that only simulate implementation through generic placeholder UI.
- Why This Matters Now: Placeholder sidebar panels are dead UI by a different name.
- Problem Statement: `LayoutManager.cpp` registers Settings, Themes, Notebooks, Canvas, Graph, and related panels through placeholder helper factories.
- User Impact: Users navigate to surfaces that imply capability without providing real workflows.
- Scope: Primary sidebar placeholder panels and their activity-bar/menu entry points.
- Out of Scope: Full deep implementation of every downstream subsystem in this phase.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`
- Related Systems / Components: Sidebar registry, layout manager, activity bar, panel factories.
- Current Behavior: Placeholder panels provide labels, toolbars, and empty text instead of real content.
- Intended Behavior: Every registered primary sidebar panel is either real or not user-visible.
- Technical Approach: Replace placeholders with real panel components or gate them from production registration.
- Implementation Steps: Audit registrations; classify replace vs gate; build real host adapters; remove helper-factory registrations.
- Validation Steps: Switch through every primary sidebar mode and verify real content or intentional absence.
- Acceptance Criteria: No primary-sidebar panel remains a generic placeholder shell.
- Dependencies: Phase 01.
- Risks / Failure Modes: Removing placeholders may reveal missing navigation fallbacks.
- UX Notes: Prefer honest absence over faux completeness.
- Settings / Panel / Control Notes where relevant: Settings primary-sidebar placeholder is especially misleading because settings are a core system.
- Observability / Diagnostics Notes: Report placeholder panel registrations in CI.
- Rollback / Safety Notes: Use feature flags to stage removals where necessary.
- References / Context: `LayoutManager.cpp` explicitly registers several placeholder-backed primary panels.
- Example scenarios where useful: Clicking the Notebooks sidebar opens a real notebook host or no production affordance at all.

### P05-T02

- Phase ID: P05
- Task ID: P05-T02
- Task Title: Strengthen `SidebarPanelRegistry` into a lifecycle-aware panel registry
- Priority: P0
- Category: Panel Lifecycle
- Atomic Improvements Covered: 21
- Objective: Move beyond a lazy widget cache toward real lifecycle ownership for primary sidebar panels.
- Why This Matters Now: Panel creation, reuse, and teardown rules are too weak for a large IDE.
- Problem Statement: `SidebarPanelRegistry.cpp` only registers factories and caches a raw panel pointer.
- User Impact: Panel state, reparenting, reuse, and stale widget issues become hard to manage.
- Scope: Registration metadata, lifecycle hooks, destroy/recreate rules, diagnostics, ownership semantics.
- Out of Scope: Full docking rewrite.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/SidebarPanelRegistry.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/SidebarPanelRegistry.h`
- Related Systems / Components: Sidebar registry, layout manager, panel lifecycle.
- Current Behavior: Registry is a simple lazy creation map.
- Intended Behavior: Registry understands panel identity, factory status, lifecycle status, and supported capabilities.
- Technical Approach: Add registry entries with lifecycle state, diagnostics, and explicit host contracts.
- Implementation Steps: Extend entry model; add lifecycle callbacks; update `LayoutManager` usage; add stale-panel detection.
- Validation Steps: Repeatedly switch panels, destroy/recreate hosts, and inspect retained state.
- Acceptance Criteria: Panel registry can explain each panel’s lifecycle status deterministically.
- Dependencies: P05-T01.
- Risks / Failure Modes: Migration may expose ownership bugs in existing panel code.
- UX Notes: Users should not perceive flicker or state loss when switching panels.
- Settings / Panel / Control Notes where relevant: Registry metadata should include whether a panel is placeholder, deferred, or fully implemented.
- Observability / Diagnostics Notes: Add registry dumps and panel lifecycle traces.
- Rollback / Safety Notes: Preserve existing lazy behavior behind adapters while upgrading metadata.
- References / Context: Current registry lacks durable lifecycle semantics.
- Example scenarios where useful: Switching away from Search and back does not leak stale async state or duplicate widgets.

### P05-T03

- Phase ID: P05
- Task ID: P05-T03
- Task Title: Complete the Explorer panel surface and remove empty section shells
- Priority: P0
- Category: Panel Implementation
- Atomic Improvements Covered: 21
- Objective: Make the Explorer panel fully real, including its subordinate sections.
- Why This Matters Now: Explorer is a primary IDE surface and currently contains empty outline/timeline shells.
- Problem Statement: `ExplorerPanel.cpp` builds outline and timeline sections as empty placeholder panels.
- User Impact: The most important project-navigation panel still contains visibly incomplete areas.
- Scope: Open Editors, Workspace, Outline, Timeline, footer actions, header actions, empty-state correctness.
- Out of Scope: Advanced source-control integration inside Explorer.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/ExplorerPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/OpenEditorsSection.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/OutlinePanel.cpp`
- Related Systems / Components: Explorer, outline, timeline, sidebar sections, file tree.
- Current Behavior: Explorer mixes real file-tree behavior with empty subordinate sections.
- Intended Behavior: Explorer sections are fully implemented or intentionally hidden by configuration until real.
- Technical Approach: Replace empty sections with real panel components or remove them from the Explorer composition.
- Implementation Steps: Integrate real outline/timeline components; connect explorer header actions; fix empty-state switching.
- Validation Steps: Open workspace, switch files, and verify all visible Explorer sections behave meaningfully.
- Acceptance Criteria: Explorer no longer contains dead section shells.
- Dependencies: P05-T01, Phase 07.
- Risks / Failure Modes: Section state persistence may conflict with new content hosts.
- UX Notes: Explorer should feel dense and useful, not padded with dead zones.
- Settings / Panel / Control Notes where relevant: Section expansion state should remain persistent and correct.
- Observability / Diagnostics Notes: Add section-content readiness checks.
- Rollback / Safety Notes: Hide incomplete sections rather than shipping empty containers.
- References / Context: `ExplorerPanel.cpp` explicitly constructs empty outline and timeline section content.
- Example scenarios where useful: Opening a markdown file shows a real outline in Explorer instead of an empty panel shell.

### P05-T04

- Phase ID: P05
- Task ID: P05-T04
- Task Title: Make primary-sidebar panel actions and headers canonical and state-aware
- Priority: P1
- Category: Panel Lifecycle
- Atomic Improvements Covered: 21
- Objective: Ensure panel headers, section toggles, and local toolbar actions reflect real capabilities.
- Why This Matters Now: Panel chrome often survives after content behavior has drifted.
- Problem Statement: Explorer refresh/collapse and similar actions are panel-local and inconsistent across surfaces.
- User Impact: Panel actions feel arbitrary and can desync from actual panel state.
- Scope: Panel headers, explorer toolbar, sidebar footer actions, section controls, panel title semantics.
- Out of Scope: Full panel visual redesign.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/ExplorerPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/ExplorerToolbar.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/PanelHeader.cpp`
- Related Systems / Components: Panel headers, inline actions, commands, panel state.
- Current Behavior: Panel actions are mostly local callbacks and not uniformly state-derived.
- Intended Behavior: Panel chrome actions are canonical controls with correct enablement and feedback.
- Technical Approach: Attach panel action IDs, target panel IDs, and enablement predicates to panel header actions.
- Implementation Steps: Normalize panel action metadata; update header widgets; route actions through canonical commands; remove local one-offs.
- Validation Steps: Activate panel actions across implemented primary panels.
- Acceptance Criteria: Panel headers no longer contain ambiguous or dead actions.
- Dependencies: Phase 01 and P05-T02.
- Risks / Failure Modes: Some panel-local actions may still need specialized payloads.
- UX Notes: Panel chrome should stay lightweight but trustworthy.
- Settings / Panel / Control Notes where relevant: Settings and theme panel headers must not expose actions to placeholder destinations.
- Observability / Diagnostics Notes: Trace panel action source and resolved panel target.
- Rollback / Safety Notes: Hide unsupported actions until corresponding panel capability exists.
- References / Context: Explorer panel currently comments that refresh would be handled elsewhere.
- Example scenarios where useful: Explorer refresh button always triggers the same workspace refresh path as other refresh surfaces.

### P05-T05

- Phase ID: P05
- Task ID: P05-T05
- Task Title: Persist and restore primary-sidebar panel state through one durable path
- Priority: P1
- Category: Docking / Layout
- Atomic Improvements Covered: 21
- Objective: Make primary-sidebar selection, visibility, width, and section state survive restart correctly.
- Why This Matters Now: Panel completeness includes persistence, not just rendering.
- Problem Statement: State is split across config keys, section-local persistence, and layout assumptions.
- User Impact: Panels reopen unpredictably or lose user arrangement.
- Scope: Active sidebar mode, sidebar width, section expansion, per-panel local state, empty-state switching.
- Out of Scope: Full workspace layout preset redesign.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/SidebarSection.cpp`; `/Users/ryanrentfro/code/markamp/src/core/ShellLayoutState.cpp`
- Related Systems / Components: Layout manager, sidebar sections, shell layout state, config.
- Current Behavior: Persistence exists in multiple local places without one explicit contract.
- Intended Behavior: Primary-sidebar state restores predictably and consistently.
- Technical Approach: Define a sidebar-state contract and move all relevant persistence through it.
- Implementation Steps: Inventory existing keys; centralize state ownership; restore active panel and width; coordinate local section state.
- Validation Steps: Restart after changing sidebar mode, width, and section expansion.
- Acceptance Criteria: Primary sidebar reopens exactly as the user left it.
- Dependencies: P05-T02.
- Risks / Failure Modes: Migrating config keys may break old layouts if not handled carefully.
- UX Notes: Restore should feel invisible and reliable.
- Settings / Panel / Control Notes where relevant: Settings panel host state should not override the user’s last real sidebar state unexpectedly.
- Observability / Diagnostics Notes: Add sidebar-state save/restore traces.
- Rollback / Safety Notes: Support one release of legacy key migration.
- References / Context: `LayoutManager.cpp`, `SidebarSection.cpp`, and `ShellLayoutState.cpp` all persist overlapping layout state.
- Example scenarios where useful: User leaves Explorer open with Outline expanded and sees the same state after restart.

### P05-T06

- Phase ID: P05
- Task ID: P05-T06
- Task Title: Add primary-sidebar panel completeness tests
- Priority: P0
- Category: Diagnostics / Regression Protection
- Atomic Improvements Covered: 21
- Objective: Turn primary-sidebar panel completeness into a release criterion.
- Why This Matters Now: Placeholder panels and state loss are otherwise easy to ship.
- Problem Statement: Panel registration and switching currently allow incomplete panels to appear valid.
- User Impact: Users encounter dead navigation destinations.
- Scope: Sidebar panel switching, panel state persistence, header actions, empty-state correctness, no-placeholder assertions.
- Out of Scope: Deep feature correctness inside every panel.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/tests/unit`; `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/SidebarPanelRegistry.cpp`
- Related Systems / Components: Sidebar panels, layout manager, diagnostics.
- Current Behavior: Primary-sidebar completeness is not clearly release-gated.
- Intended Behavior: CI fails if a placeholder or dead primary panel is exposed.
- Technical Approach: Build panel-state fixtures and registry assertions.
- Implementation Steps: Add panel registration tests; add switch-and-restore tests; assert no placeholder factories remain in production config.
- Validation Steps: Run panel suite against clean and restored workspace state.
- Acceptance Criteria: Exposed primary sidebar panels are validated automatically.
- Dependencies: P05-T01 through P05-T05.
- Risks / Failure Modes: Tests may overfit to specific panel ordering instead of capability.
- UX Notes: Include user-visible assertions, not just internal state checks.
- Settings / Panel / Control Notes where relevant: Settings sidebar host state should be included if it remains part of the product.
- Observability / Diagnostics Notes: Dump panel registry state on failure.
- Rollback / Safety Notes: Keep registry tests resilient to intentional panel additions.
- References / Context: Primary sidebar is currently one of the biggest dead-UI risk areas.
- Example scenarios where useful: CI verifies that selecting Themes no longer opens a placeholder helper panel.

