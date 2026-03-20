# Phase 04: Canvas Creation And Board Workbench Integration

## Outcome

Make canvas creation, save, reopen, and interaction flows real product workflows by merging placeholder UI behavior with the actual board workbench and fixing the event topology.

## Improvement Count

204 atomic improvements across 6 execution tasks.

### P04-T01

- Phase ID: P04
- Task ID: P04-T01
- Task Title: Route new-canvas creation through CanvasWorkbench instead of CanvasWorkspacePanel placeholders
- Priority: P0
- Category: Canvas Workflow
- Atomic Improvements Covered: 34
- Objective: Ensure new board creation uses the real board model and artifact pipeline.
- Why This Matters Now: Canvas creation currently stops at label changes and counters.
- Problem Statement: `CanvasWorkspacePanel::NewBoard()` does not create a durable board artifact.
- User Impact: New canvases are not trustworthy or restorable.
- Scope: Board creation service routing, board artifact registration, panel/workbench handshake.
- Out of Scope: Deep canvas feature expansion.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp`; `/Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp`
- Related Systems / Components: Canvas workspace, workbench shell, tabs, persistence.
- Current Behavior: Empty `BoardOpenRequestEvent` leads to `CanvasWorkspacePanel::NewBoard()`.
- Intended Behavior: New canvas creates a real board via `CanvasWorkbench::create_board()` and mounts it.
- Technical Approach: Replace panel-side `NewBoard()` creation with artifact-creation service plus workbench adapter.
- Implementation Steps: Add canvas creation request; wire to workbench; return board artifact record; update panel title from board metadata.
- Validation Steps: Create board, add content, save, close, and reopen workspace.
- Acceptance Criteria: A new canvas exists as a real board model immediately after creation.
- Dependencies: Phase 01.
- Risks / Failure Modes: Dual creation paths survive.
- UX Notes: Board creation should feel instant and land the user inside a ready canvas.
- Styling / Highlighting Notes where relevant: New board empty state should visually match notebook and editor quality.
- Observability / Diagnostics Notes: Log board create source, board ID, workbench mount, and save target.
- Rollback / Safety Notes: Keep panel shim calling workbench during migration.
- References / Context: `CanvasWorkspacePanel.cpp` currently increments a counter and updates a label.
- Example scenarios where useful: User creates a new board from the command palette and sees a real saveable canvas, not a temporary shell label.

### P04-T02

- Phase ID: P04
- Task ID: P04-T02
- Task Title: Remove the private canvas event bus and reconnect canvas panels to the app event graph
- Priority: P0
- Category: Command / Event Wiring
- Atomic Improvements Covered: 34
- Objective: Fix the event topology so canvas tools, shell state, commands, and diagnostics operate on the same bus.
- Why This Matters Now: The local event bus isolates canvas interactions from the rest of the workbench.
- Problem Statement: `CanvasWorkspacePanel` constructs `std::make_shared<core::EventBus>()` for `CanvasPanel`.
- User Impact: Canvas behavior can look alive while remaining disconnected from shell context and diagnostics.
- Scope: Canvas panel bus ownership, event subscriptions, workbench interactions, selection/focus propagation.
- Out of Scope: Network collaboration event routing.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp`
- Related Systems / Components: Event bus, shell controller, commands, status bar, diagnostics.
- Current Behavior: Canvas UI and application shell can be event-isolated.
- Intended Behavior: Canvas uses shared app events and contributes authoritative context.
- Technical Approach: Thread the shared bus through canvas panel/workbench construction and remove local bus creation.
- Implementation Steps: Replace local bus; audit subscriptions; reconnect tool, selection, viewport, save, and context events; add assertions against private bus creation.
- Validation Steps: Use canvas commands and verify status bar, panels, and logs react on the shared bus.
- Acceptance Criteria: Canvas events are visible to shell diagnostics and other workbench surfaces.
- Dependencies: P04-T01.
- Risks / Failure Modes: Subscription lifetimes may expose hidden reentrancy issues.
- UX Notes: Selection and tool changes should reflect immediately across canvas chrome.
- Styling / Highlighting Notes where relevant: Shared event state should drive active/inactive tool visuals consistently.
- Observability / Diagnostics Notes: Add event-bus identity checks in debug builds.
- Rollback / Safety Notes: Keep explicit bus ownership docs to avoid future regressions.
- References / Context: `CanvasWorkspacePanel.cpp` line creating local shared `EventBus` is a direct integration defect.
- Example scenarios where useful: Selecting the draw tool updates canvas, toolbar state, and diagnostics on the same event chain.

### P04-T03

- Phase ID: P04
- Task ID: P04-T03
- Task Title: Finish board lifecycle for save, save as, rename, duplicate, delete, and restore
- Priority: P0
- Category: Artifact Lifecycle
- Atomic Improvements Covered: 34
- Objective: Give canvases the same end-to-end lifecycle integrity as files and notebooks.
- Why This Matters Now: Canvas boards are not yet first-class project artifacts.
- Problem Statement: `CanvasWorkbench` has real operations, but shell and persistence integration are incomplete.
- User Impact: Users cannot trust boards to survive normal project workflows.
- Scope: Board save path resolution, duplicate, rename, delete, dirty prompts, reopen, recents, restore.
- Out of Scope: Real-time collaboration merge strategies.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`; `/Users/ryanrentfro/code/markamp/src/core/WorkspaceSessionRestore.cpp`
- Related Systems / Components: Canvas workbench, shell, persistence, explorer, recents.
- Current Behavior: Board operations exist but are not fully exposed or shell-consistent.
- Intended Behavior: Boards save and restore like primary IDE artifacts.
- Technical Approach: Promote board operations through artifact lifecycle service and unify path ownership.
- Implementation Steps: Add save/save-as and rename hooks to shell commands; persist board file path in registry; update explorer and recents; add restore support.
- Validation Steps: Save a board, rename it, duplicate it, close workspace, and restore it.
- Acceptance Criteria: Board lifecycle operations update shell state, title chrome, and storage reliably.
- Dependencies: P04-T01, P04-T02, Phase 05.
- Risks / Failure Modes: Board IDs and filenames may diverge if not normalized.
- UX Notes: Dirty prompts should describe board content loss clearly.
- Styling / Highlighting Notes where relevant: Canvas tabs and explorer rows should display board status consistently.
- Observability / Diagnostics Notes: Emit board lifecycle events with file path and artifact ID.
- Rollback / Safety Notes: Create automatic backup snapshots before destructive board operations.
- References / Context: `CanvasWorkbench.cpp` has `rename_board`, `duplicate_board`, and `save_board` but not full shell plumbing.
- Example scenarios where useful: User duplicates a board, edits the copy, saves both, and later restores both from session state.

### P04-T04

- Phase ID: P04
- Task ID: P04-T04
- Task Title: Complete canvas shell integration for tabs, workspace tree, focus, and commands
- Priority: P1
- Category: Canvas Workflow
- Atomic Improvements Covered: 34
- Objective: Make canvases visible and controllable from the same shell structures as files and notebooks.
- Why This Matters Now: Creation and save alone are not enough if the shell still treats canvas as exceptional.
- Problem Statement: Canvas mode is still partly a separate workspace rather than a first-class artifact host.
- User Impact: Commands, focus, and navigation may feel inconsistent or missing.
- Scope: Active board context, tab descriptors, workspace tree nodes, palette actions, toolbar actions, status updates.
- Out of Scope: Multi-window canvas mode.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/WorkbenchShellController.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`
- Related Systems / Components: Shell controller, canvas workspace, explorer, command palette, status bar.
- Current Behavior: Canvas entry points route through mode switches and board open events with limited shell cohesion.
- Intended Behavior: Canvases behave like active artifacts with proper command targeting and visibility.
- Technical Approach: Bind canvas artifact activation into shell active-context and mount adapters.
- Implementation Steps: Add canvas tab model; register canvas actions; unify explorer open behavior; update status and panel activation rules.
- Validation Steps: Open multiple boards and switch between them using tabs, explorer, and palette.
- Acceptance Criteria: Canvas artifacts can be activated, navigated, and commanded consistently.
- Dependencies: P04-T01 through P04-T03, Phase 06.
- Risks / Failure Modes: Canvas mode assumptions may conflict with tabbed artifact hosting.
- UX Notes: Switching into canvas should not feel like leaving the IDE.
- Styling / Highlighting Notes where relevant: Canvas-active tabs and panel chrome should feel native to the same workbench.
- Observability / Diagnostics Notes: Track active-board changes and command-target resolution.
- Rollback / Safety Notes: Keep mode-switch fallback if artifact-tab integration needs staged rollout.
- References / Context: `LayoutManager.cpp` currently special-cases `BoardOpenRequestEvent`.
- Example scenarios where useful: User opens a file and a board side by side and can switch focus without losing command correctness.

### P04-T05

- Phase ID: P04
- Task ID: P04-T05
- Task Title: Upgrade canvas empty-state, tool-rail, inspector, and board chrome quality
- Priority: P1
- Category: Styling / Visual Design
- Atomic Improvements Covered: 34
- Objective: Make canvas creation and first interaction feel premium and intentional.
- Why This Matters Now: The current canvas shell uses placeholder icons and sparse chrome.
- Problem Statement: Tool descriptors use Unicode placeholders and inspector/minimap are visibly provisional.
- User Impact: The canvas surface looks unfinished even before deeper interaction bugs appear.
- Scope: Empty-state design, tool-rail visual system, inspector styling, board header, minimap strip, hover/focus/selected states.
- Out of Scope: Deep custom object inspector features.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/core/IconInventory.cpp`
- Related Systems / Components: Canvas workspace, icon system, theme engine, status surfaces.
- Current Behavior: Canvas chrome is functional-looking but placeholder-heavy.
- Intended Behavior: New and active boards present a polished workbench-grade canvas shell.
- Technical Approach: Apply shared tokens, MUI icons, premium spacing, and clearer state treatment.
- Implementation Steps: Replace placeholder glyphs; normalize toolbar sizing; refine inspector/minimap chrome; add rich empty state and save status cues.
- Validation Steps: Compare canvas shell across themes and window sizes.
- Acceptance Criteria: Canvas creation screen looks intentional and cohesive with the rest of the IDE.
- Dependencies: P04-T01 through P04-T04, Phase 09.
- Risks / Failure Modes: Pure polish before workflow stability may mask deeper defects.
- UX Notes: Tool states need stronger discoverability without clutter.
- Styling / Highlighting Notes where relevant: Embedded code or text objects need styling hooks aligned with editor tokens.
- Observability / Diagnostics Notes: Capture visual-state regressions in screenshot smoke tests.
- Rollback / Safety Notes: Keep token-based styling so future themes do not break layout.
- References / Context: `CanvasWorkspacePanel.cpp` tool rail currently uses Unicode icon placeholders.
- Example scenarios where useful: A first-time user creates a board and immediately understands select, pan, text, and draw actions.

### P04-T06

- Phase ID: P04
- Task ID: P04-T06
- Task Title: Add canvas creation and lifecycle regression coverage
- Priority: P0
- Category: Diagnostics / Regression Protection
- Atomic Improvements Covered: 34
- Objective: Guard the corrected board workflow with direct tests and flow traces.
- Why This Matters Now: Canvas currently mixes real and placeholder logic, which is high-risk during refactor.
- Problem Statement: Regressions could leave the UI appearing functional while the model path is bypassed.
- User Impact: Boards may appear to save or restore but actually lose work.
- Scope: Create/save/reopen tests, shared-bus checks, explorer integration, duplicate/rename tests, visual smoke paths.
- Out of Scope: Full object-editing correctness matrix.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/tests/unit`; `/Users/ryanrentfro/code/markamp/scripts/smoke_test.sh`; `/Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp`
- Related Systems / Components: Canvas workbench, shell, persistence, CI.
- Current Behavior: Canvas lifecycle correctness is underprotected.
- Intended Behavior: CI catches board-flow regressions before release.
- Technical Approach: Add artifact fixtures, event-bus assertions, and end-to-end smoke scripts.
- Implementation Steps: Add board create/save/restore tests; add shared-bus invariant checks; add screenshot checks for empty board and dirty board states.
- Validation Steps: Run canvas smoke suite locally and in CI.
- Acceptance Criteria: Board creation and restore are release-gated with repeatable coverage.
- Dependencies: P04-T01 through P04-T05.
- Risks / Failure Modes: Tests may not simulate actual shell focus or tab interactions.
- UX Notes: Include keyboard-first and command-palette-first canvas flows.
- Styling / Highlighting Notes where relevant: Track canvas chrome regressions visually.
- Observability / Diagnostics Notes: Persist board flow traces on failure.
- Rollback / Safety Notes: Keep temp board storage isolated for tests.
- References / Context: Board creation currently relies on a UI stub path.
- Example scenarios where useful: CI verifies that a board created from the palette survives restart with objects and correct title.

