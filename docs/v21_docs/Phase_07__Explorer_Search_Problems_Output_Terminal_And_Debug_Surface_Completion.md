# Phase 07: Explorer Search Problems Output Terminal And Debug Surface Completion

## Outcome

Finish the highest-traffic workbench surfaces so their local controls, context menus, empty states, and panel behaviors are fully implemented and trustworthy.

## Improvement Count

132 atomic improvements across 6 execution tasks.

### P07-T01

- Phase ID: P07
- Task ID: P07-T01
- Task Title: Complete Explorer surface controls and subordinate workflows
- Priority: P0
- Category: Panel Implementation
- Atomic Improvements Covered: 22
- Objective: Make Explorer header actions, open-editors behavior, tree actions, and subordinate sections function as one coherent project-navigation surface.
- Why This Matters Now: Explorer is the most frequently used control surface in the IDE and still mixes real behavior with empty or local-only sections.
- Problem Statement: `/Users/ryanrentfro/code/markamp/src/ui/ExplorerPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/ExplorerToolbar.cpp`, and `/Users/ryanrentfro/code/markamp/src/ui/FileTreeCtrl.cpp` contain duplicated action paths, empty section shells, and direct widget logic.
- User Impact: Users cannot trust project navigation, inline creation affordances, or section-level quick actions.
- Scope: Explorer header and toolbar actions, Open Editors, file tree context actions, section collapse state, empty-state CTAs, selection/focus propagation.
- Out of Scope: Advanced repository-aware explorer decorations beyond basic correctness.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/ExplorerPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/ExplorerToolbar.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/FileTreeCtrl.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/OpenEditorsSection.cpp`
- Related Systems / Components: Explorer, file tree, open editors, sidebar sections, commands.
- Current Behavior: Explorer mixes implemented tree behavior with empty subordinate sections and local action callbacks.
- Intended Behavior: Every visible Explorer action resolves through canonical commands, every visible section has real content or is intentionally hidden, and selection state stays coherent across the workbench.
- Technical Approach: Refactor Explorer to consume canonical action metadata, attach real subordinate hosts, and unify tree/action state with the shell context model.
- Implementation Steps: Audit all Explorer buttons and menus; remove local-only callbacks; wire section content to real hosts; normalize empty-state and selection behavior; verify create/reveal/refresh actions use canonical flows.
- Validation Steps: Open and close folders, switch active tabs, invoke Explorer toolbar and context actions, and verify action traces plus visible state updates.
- Acceptance Criteria: Explorer contains no dead section shell, no orphaned action, and no action path that bypasses canonical command routing without an explicit exception.
- Dependencies: Phase 01, Phase 04, Phase 05.
- Risks / Failure Modes: Explorer tree refresh or selection restoration may regress if old local assumptions are removed too aggressively.
- UX Notes: Explorer must feel dense, fast, and reliable; hidden sections are preferable to empty ones.
- Settings / Panel / Control Notes where relevant: Explorer section expansion and tree presentation settings must stay synchronized with the actual Explorer composition.
- Observability / Diagnostics Notes: Emit Explorer action traces including target node type, workspace root, and selection state.
- Rollback / Safety Notes: Migrate Explorer subsections independently so tree correctness is not coupled to section-host replacement.
- References / Context: `/Users/ryanrentfro/code/markamp/src/ui/ExplorerPanel.cpp` builds empty outline and timeline section shells while `/Users/ryanrentfro/code/markamp/src/ui/FileTreeCtrl.cpp` still owns native context-menu logic.
- Example scenarios where useful: Clicking `New File` from the Explorer toolbar and from a folder context menu produces the same canonical file-creation flow and visible tree update.

### P07-T02

- Phase ID: P07
- Task ID: P07-T02
- Task Title: Replace placeholder search-sidebar execution seams with a real workspace-aware search host
- Priority: P0
- Category: Panel Implementation
- Atomic Improvements Covered: 22
- Objective: Make the Search sidebar a real search workbench surface instead of a UI shell with placeholder execution seams.
- Why This Matters Now: Search is a foundational IDE panel, and it still carries placeholder root and async-management behavior.
- Problem Statement: `/Users/ryanrentfro/code/markamp/src/ui/SearchSidebarPanel.cpp` uses `"."` as a placeholder workspace root and comments that async task management is temporary.
- User Impact: Users get inconsistent search scope, unstable result updates, and low confidence in search actions.
- Scope: Search input, scope selection, async execution, result grouping, reveal/open actions, replace hooks, empty/loading/error states.
- Out of Scope: New query-language features beyond making existing search reliable.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/SearchSidebarPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/core/SearchService.cpp`; `/Users/ryanrentfro/code/markamp/src/core/SearchEngine.cpp`; `/Users/ryanrentfro/code/markamp/src/core/SearchResultNavigator.cpp`
- Related Systems / Components: Search panel, search service, workspace context, async task management, commands.
- Current Behavior: Search UI is present, but execution scope and task lifecycle remain partially placeholder-backed.
- Intended Behavior: Search uses the active workspace/project context, exposes trustworthy progress and result state, and routes local actions through canonical commands.
- Technical Approach: Replace placeholder workspace and task assumptions with explicit search-session ownership and shell-context integration.
- Implementation Steps: Define active search scope ownership; replace placeholder root lookup; introduce durable async-task/session management; normalize result actions and empty/error states; verify panel persistence and restore.
- Validation Steps: Run searches across an open workspace, cancel and restart searches, open results, and verify correct scope, loading state, and command traces.
- Acceptance Criteria: Search sidebar results always map to the active workspace context, search state is cancel-safe, and no placeholder execution path remains.
- Dependencies: Phase 01, Phase 05, Phase 06.
- Risks / Failure Modes: Search cancellation and rapid re-query flows may surface latent threading assumptions.
- UX Notes: Search must communicate scope and progress clearly; loading and empty states must not look identical.
- Settings / Panel / Control Notes where relevant: Search-related settings and toggles should apply immediately and reflect actual search-host capabilities.
- Observability / Diagnostics Notes: Add structured search-session diagnostics with scope, query, cancellation reason, and result counts.
- Rollback / Safety Notes: Preserve old search-path behavior behind a compatibility shim until new session management is stable.
- References / Context: `/Users/ryanrentfro/code/markamp/src/ui/SearchSidebarPanel.cpp` explicitly documents placeholder workspace-root and async-management behavior.
- Example scenarios where useful: Running a search from the sidebar in a multi-folder workspace searches the active workspace root rather than the process working directory.

### P07-T03

- Phase ID: P07
- Task ID: P07-T03
- Task Title: Finish Problems panel navigation and quick-fix integrity
- Priority: P0
- Category: Panel Implementation
- Atomic Improvements Covered: 22
- Objective: Turn Problems from a mostly visual list into a real diagnostics workflow surface.
- Why This Matters Now: Problems appears implemented, but key actions still terminate in placeholder comments and empty fix providers.
- Problem Statement: `/Users/ryanrentfro/code/markamp/src/ui/ProblemsPanel.cpp` contains placeholder navigation and empty quick-fix methods.
- User Impact: Users see diagnostics and action affordances that do not actually help them move to or resolve issues.
- Scope: Problem navigation, reveal/open behavior, filter actions, collapse/expand, quick fixes, autofix, context menus, double-click handling.
- Out of Scope: Full LSP quick-fix ecosystem beyond making the panel honest and functionally complete for supported diagnostics.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/ProblemsPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/ProblemsPanel.h`; `/Users/ryanrentfro/code/markamp/src/core/DiagnosticsService.cpp`; `/Users/ryanrentfro/code/markamp/src/core/ProblemsCommands.cpp`
- Related Systems / Components: Problems panel, diagnostics service, editors, commands, context menus.
- Current Behavior: Problems renders diagnostics, but navigation and fix affordances are partially stubbed.
- Intended Behavior: Selecting or activating a diagnostic always routes to the correct document/location, and only real quick-fix affordances are shown.
- Technical Approach: Replace placeholder methods with canonical diagnostic navigation and fix-provider contracts, and hide unsupported fix actions until providers exist.
- Implementation Steps: Implement real navigation target resolution; add diagnostic target payloads; wire supported quick-fix execution; gate unsupported actions; normalize context menu and toolbar state.
- Validation Steps: Open diagnostics, navigate by keyboard and pointer, invoke context actions, and verify editor focus plus diagnostic selection sync.
- Acceptance Criteria: No visible Problems action is a no-op, and unsupported fix actions are absent or clearly unavailable with rationale.
- Dependencies: Phase 01, Phase 04, Phase 06.
- Risks / Failure Modes: Diagnostics providers may not yet expose enough metadata for safe autofix routing.
- UX Notes: Problems should never invite a fix action that cannot execute; navigation must feel instant and precise.
- Settings / Panel / Control Notes where relevant: Diagnostic filter and severity controls must persist and reflect the actual panel model.
- Observability / Diagnostics Notes: Record diagnostic-action traces with source diagnostic ID, target file, and execution result.
- Rollback / Safety Notes: Hide quick-fix surfaces behind capability checks while providers mature.
- References / Context: `/Users/ryanrentfro/code/markamp/src/ui/ProblemsPanel.cpp` contains placeholder `NavigateToSelectedProblem`, `available_quick_fixes`, `apply_quick_fix`, and `autofix_all`.
- Example scenarios where useful: Double-clicking a warning in Problems opens the file at the exact range and highlights the same diagnostic in the editor gutter.

### P07-T04

- Phase ID: P07
- Task ID: P07-T04
- Task Title: Audit Output, Terminal, Source Control, and Git log controls for full local action integrity
- Priority: P1
- Category: Panel Implementation
- Atomic Improvements Covered: 22
- Objective: Ensure major bottom/workbench utility panels expose only real actions with correct enablement, menus, and state feedback.
- Why This Matters Now: These surfaces contain many daily controls and local context menus that still rely on ad hoc native menu paths.
- Problem Statement: `/Users/ryanrentfro/code/markamp/src/ui/OutputPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/TerminalPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/SourceControlPanel.cpp`, and `/Users/ryanrentfro/code/markamp/src/ui/GitLogPanel.cpp` still hand-build large portions of their interaction models.
- User Impact: Local commands like copy, clear, search, stage, reveal, or branch actions can drift from the rest of the IDE and become unreliable.
- Scope: Panel toolbar actions, context menus, selection-aware buttons, empty/loading states, local navigation and reveal actions.
- Out of Scope: New Git or terminal feature expansion not required for control integrity.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/OutputPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/TerminalPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/SourceControlPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/GitLogPanel.cpp`
- Related Systems / Components: Output, terminal, source control, git log, commands, context menus.
- Current Behavior: These panels appear feature-rich but still own bespoke action wiring and target-state rules.
- Intended Behavior: Every local action routes through canonical commands or explicit panel capability handlers with correct enablement and feedback.
- Technical Approach: Map panel-local controls to canonical action IDs, normalize capability checks, and migrate native context menus into the shared target-aware system.
- Implementation Steps: Inventory local controls; bind to command IDs; replace local-only enablement; migrate context menus; verify panel-local feedback and focus return.
- Validation Steps: Use keyboard and pointer to execute panel-local actions under empty, selected, and error states.
- Acceptance Criteria: No visible utility-panel action remains bespoke, misleading, or untestable.
- Dependencies: Phase 01, Phase 04, Phase 06.
- Risks / Failure Modes: Some terminal and source-control flows may need specialized payload objects that are not yet part of the command contract.
- UX Notes: These panels must feel as polished as primary editor surfaces, not like auxiliary tools.
- Settings / Panel / Control Notes where relevant: Panel-local settings toggles such as wrap, timestamps, auto-scroll, and filters must stay synchronized with real persisted state.
- Observability / Diagnostics Notes: Add panel-local action traces with panel ID, selection state, and target payload.
- Rollback / Safety Notes: Preserve specialized panel handlers behind canonical command adapters when deep refactors would otherwise stall the migration.
- References / Context: The repo contains many `PopupMenu(&menu)` code paths across these surfaces despite the presence of a shared context-menu stack.
- Example scenarios where useful: Right-clicking terminal text and pressing `Copy` uses the same action ID and success/error feedback path as the Edit menu copy route when a terminal selection is active.

### P07-T05

- Phase ID: P07
- Task ID: P07-T05
- Task Title: Complete Debug Console, run/build controls, and panel-local status integrity
- Priority: P1
- Category: Panel Implementation
- Atomic Improvements Covered: 22
- Objective: Make debug and run/build panel controls fully wired, correctly enabled, and integrated into shell focus and panel activation behavior.
- Why This Matters Now: Debug and build surfaces are central IDE credibility signals, and partially wired controls here make the product feel unfinished immediately.
- Problem Statement: Debug, run, and build-related surfaces expose local controls and menus but still rely on thin lifecycle and action plumbing.
- User Impact: Users cannot trust when build/debug controls are available, where output will appear, or how panel activation follows execution state.
- Scope: Debug console controls, run-config dropdown triggers, build panel actions, show-on-build/show-on-error behavior, panel activation on run/debug events.
- Out of Scope: Full debugger backend expansion.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/DebugConsolePanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/BuildPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/RunConfigDropdown.cpp`; `/Users/ryanrentfro/code/markamp/src/core/BuildLogManager.cpp`
- Related Systems / Components: Debug console, build panel, run configuration, panel activation, commands.
- Current Behavior: These panels are more substantial than pure stubs, but control routing and activation behavior are still not uniformly canonical.
- Intended Behavior: Run/build/debug controls always advertise only valid actions, activate the correct panel targets, and preserve focus expectations.
- Technical Approach: Normalize run/build/debug actions through canonical commands and explicit panel-activation contracts.
- Implementation Steps: Audit all debug/build controls; replace local triggers with canonical action dispatch; unify show-panel-on-event behavior; verify enablement, labels, and status feedback.
- Validation Steps: Start builds and debug-console actions from menu, toolbar, and local panel controls; verify active panel transitions and control state.
- Acceptance Criteria: Build and debug surfaces no longer expose inconsistent enablement or local-only action behavior.
- Dependencies: Phase 01, Phase 03, Phase 06.
- Risks / Failure Modes: Build/debug event timing may create panel activation races if the lifecycle contract is still too loose.
- UX Notes: Run/build/debug panels should surface feedback with confidence and minimal surprise.
- Settings / Panel / Control Notes where relevant: Settings that govern panel auto-reveal or run/build behavior must apply predictably and be visible in the affected surfaces.
- Observability / Diagnostics Notes: Trace action source, panel activation target, and resulting execution status for run/build/debug flows.
- Rollback / Safety Notes: Keep previous auto-reveal behavior behind a setting or compatibility branch until the unified path is stable.
- References / Context: `/Users/ryanrentfro/code/markamp/src/ui/RunConfigDropdown.cpp` and panel-local menus represent high-risk specialized control paths.
- Example scenarios where useful: Starting a build from a toolbar or dropdown consistently reveals the Build panel when configured to do so and leaves focus in the correct place when configured not to.

### P07-T06

- Phase ID: P07
- Task ID: P07-T06
- Task Title: Add focused regression coverage for daily workbench surfaces
- Priority: P0
- Category: Diagnostics / Regression Protection
- Atomic Improvements Covered: 22
- Objective: Make Explorer, Search, Problems, Output, Terminal, and Debug surface integrity release-gated.
- Why This Matters Now: These panels are touched constantly, and their control regressions are expensive and visible.
- Problem Statement: Current coverage does not guarantee that panel-local controls, context actions, and activation flows remain trustworthy.
- User Impact: Routine workflows regress silently between builds.
- Scope: Surface initialization, local actions, context menus, empty/loading/error states, selection/focus changes, panel activation and restore.
- Out of Scope: Full domain-behavior testing beyond surface integrity.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/tests/unit`; `/Users/ryanrentfro/code/markamp/scripts/smoke_test.sh`; `/Users/ryanrentfro/code/markamp/src/ui/ExplorerPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/SearchSidebarPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/ProblemsPanel.cpp`
- Related Systems / Components: Workbench panels, command routing, panel lifecycle, diagnostics.
- Current Behavior: These surfaces are only partially protected despite containing many manual action paths.
- Intended Behavior: CI catches dead buttons, wrong-target actions, placeholder sections, and broken activation/restore behavior in daily-use panels.
- Technical Approach: Build surface-level smoke suites around canonical action IDs, target fixtures, and panel-state assertions.
- Implementation Steps: Define per-surface smoke matrices; add control invocation tests; add focus/selection assertions; require no-placeholder and no-no-op results in CI.
- Validation Steps: Run the surface suite against clean-profile and restored-session fixtures.
- Acceptance Criteria: Daily-use workbench surfaces are regression-gated with actionable failures that identify the broken control or panel state.
- Dependencies: P07-T01 through P07-T05.
- Risks / Failure Modes: Tests may become overly UI-structure-specific if they target widget layout rather than action contracts.
- UX Notes: Include keyboard-only flows for search, problems, and terminal where they are core to user productivity.
- Settings / Panel / Control Notes where relevant: Include settings-controlled variants such as auto-reveal, wrapping, and filter persistence.
- Observability / Diagnostics Notes: Attach panel registry, action trace, and focus-state snapshots to failing smoke runs.
- Rollback / Safety Notes: Keep tests aligned to canonical control contracts so UI refactors do not invalidate them unnecessarily.
- References / Context: These surfaces currently combine substantial functionality with enough local wiring to remain a high regression risk.
- Example scenarios where useful: CI opens Search, runs a query, jumps to a result, opens Problems, navigates a diagnostic, then uses Terminal copy and verifies every action path resolves correctly.
