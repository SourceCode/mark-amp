# Phase 01: Canvas Input Workbench And Interaction Foundation

## Objective
Establish the board shell, input pipeline, command model, and workspace scaffolding required for Miro-grade editing and creation flows.

## AI Agent Execution Rules
- Execute workstreams in order unless a later task is explicitly unblocked by completed model and command contracts.
- Prefer extending existing canvas, UI, and core services before introducing new parallel abstractions.
- Every implementation task should finish with tests or a documented testing gap.
- Preserve theme, accessibility, undo/redo, and serialization behavior as default quality bars rather than follow-up work.
- When adding references to new code during implementation, keep using full file paths in planning notes and PR write-ups.

## Workstreams
- W01 Workspace Shell: Make canvas a first-class workspace surface with predictable chrome, entry points, and mode transitions.
- W02 Board Lifecycle: Tighten new/open/rename/duplicate/close flows so board management feels native and low-friction.
- W03 Viewport Camera: Deliver responsive pan, zoom, fit, and focal-point management across small and large boards.
- W04 Tool Rail: Rationalize tool discovery, activation, grouping, and customization for frequent creators.
- W05 Pointer Routing: Standardize mouse, trackpad, stylus, and touch routing so tools receive coherent pointer state.
- W06 Input State Machine: Harden tool transitions and transient states so creation, editing, and cancellation never conflict.
- W07 Keyboard Commanding: Expand keyboard-first execution so canvas power users can stay off the mouse when precision matters.
- W08 Cursor And Mode Feedback: Make the active tool, pending action, and allowed drop target legible through cursor and status feedback.
- W09 Grid And Rulers: Add the spatial scaffolding creators expect when placing or measuring objects.
- W10 Snap Preferences: Make snapping predictable, configurable, and transparent across all transform tools.
- W11 Context Menus: Turn right-click and long-press into mode-aware quick action hubs instead of generic command dumps.
- W12 Undo And Redo Plumbing: Make all canvas operations produce crisp history entries with reliable inversion.
- W13 Autosave And Recovery: Ensure users can trust boards to survive crashes, force-quits, and sync interruptions.
- W14 Inspector Shell: Define a scalable inspector frame that supports board, single-object, and multi-object editing states.
- W15 Minimap Shell: Expose orientation and rapid relocation without overloading the main board surface.
- W16 Onboarding And Empty States: Give first-run canvas users enough guidance to succeed without burying experts in chrome.
- W17 Board Settings: Centralize board-level controls for size, background, permissions, and default behaviors.
- W18 Theme Tokens: Align the canvas shell and interaction layers with the broader theme system.
- W19 Command Palette Integration: Make canvas commands discoverable, searchable, and context-ranked.
- W20 Event And Analytics Contracts: Define the event vocabulary and instrumentation needed to evolve the canvas safely.

## Workstream 01: Workspace Shell
Make canvas a first-class workspace surface with predictable chrome, entry points, and mode transitions.

### P01-T001: Workspace Shell UX Parity Audit
**Task Title:** Workspace Shell UX Parity Audit
**Definition:** Define the implementation contract for workspace shell by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around make canvas a first-class workspace surface with predictable chrome, entry points, and mode transitions.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** persistent tool rail; board title and status strip; inspector and minimap docking.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.h
- /Users/ryanrentfro/code/markamp/src/ui/ActivityBar.cpp
- /Users/ryanrentfro/code/markamp/src/core/ActivityCommandProvider.cpp

### P01-T002: Workspace Shell Domain Model And State Contract
**Task Title:** Workspace Shell Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support workspace shell.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around make canvas a first-class workspace surface with predictable chrome, entry points, and mode transitions.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** persistent tool rail; board title and status strip; inspector and minimap docking.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.h
- /Users/ryanrentfro/code/markamp/src/ui/ActivityBar.cpp
- /Users/ryanrentfro/code/markamp/src/core/ActivityCommandProvider.cpp

### P01-T003: Workspace Shell Commands Events And Context Keys
**Task Title:** Workspace Shell Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for workspace shell.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make make canvas a first-class workspace surface with predictable chrome, entry points, and mode transitions. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** persistent tool rail; board title and status strip; inspector and minimap docking.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.h
- /Users/ryanrentfro/code/markamp/src/ui/ActivityBar.cpp
- /Users/ryanrentfro/code/markamp/src/core/ActivityCommandProvider.cpp

### P01-T004: Workspace Shell Workspace Surface And Controls
**Task Title:** Workspace Shell Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose workspace shell in the main canvas workspace.
**Description:** Build the primary UI surfaces for make canvas a first-class workspace surface with predictable chrome, entry points, and mode transitions., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** persistent tool rail; board title and status strip; inspector and minimap docking.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.h
- /Users/ryanrentfro/code/markamp/src/ui/ActivityBar.cpp
- /Users/ryanrentfro/code/markamp/src/core/ActivityCommandProvider.cpp

### P01-T005: Workspace Shell Pointer Interaction Flow
**Task Title:** Workspace Shell Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for workspace shell.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for make canvas a first-class workspace surface with predictable chrome, entry points, and mode transitions.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** persistent tool rail; board title and status strip; inspector and minimap docking.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.h
- /Users/ryanrentfro/code/markamp/src/ui/ActivityBar.cpp
- /Users/ryanrentfro/code/markamp/src/core/ActivityCommandProvider.cpp

### P01-T006: Workspace Shell Keyboard Gesture And Shortcut Flow
**Task Title:** Workspace Shell Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for workspace shell.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for make canvas a first-class workspace surface with predictable chrome, entry points, and mode transitions.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** persistent tool rail; board title and status strip; inspector and minimap docking.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.h
- /Users/ryanrentfro/code/markamp/src/ui/ActivityBar.cpp
- /Users/ryanrentfro/code/markamp/src/core/ActivityCommandProvider.cpp

### P01-T007: Workspace Shell Visual Feedback And Rendering Polish
**Task Title:** Workspace Shell Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make workspace shell legible in use.
**Description:** Implement the visible feedback for make canvas a first-class workspace surface with predictable chrome, entry points, and mode transitions.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** persistent tool rail; board title and status strip; inspector and minimap docking.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.h
- /Users/ryanrentfro/code/markamp/src/ui/ActivityBar.cpp
- /Users/ryanrentfro/code/markamp/src/core/ActivityCommandProvider.cpp

### P01-T008: Workspace Shell Persistence Preferences And Serialization
**Task Title:** Workspace Shell Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by workspace shell.
**Description:** Identify what parts of make canvas a first-class workspace surface with predictable chrome, entry points, and mode transitions. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** persistent tool rail; board title and status strip; inspector and minimap docking.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.h
- /Users/ryanrentfro/code/markamp/src/ui/ActivityBar.cpp
- /Users/ryanrentfro/code/markamp/src/core/ActivityCommandProvider.cpp

### P01-T009: Workspace Shell Safeguards Telemetry And Recovery
**Task Title:** Workspace Shell Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for workspace shell.
**Description:** Instrument make canvas a first-class workspace surface with predictable chrome, entry points, and mode transitions. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** persistent tool rail; board title and status strip; inspector and minimap docking.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.h
- /Users/ryanrentfro/code/markamp/src/ui/ActivityBar.cpp
- /Users/ryanrentfro/code/markamp/src/core/ActivityCommandProvider.cpp

### P01-T010: Workspace Shell Tests Documentation And Rollout Gate
**Task Title:** Workspace Shell Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship workspace shell.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for make canvas a first-class workspace surface with predictable chrome, entry points, and mode transitions.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** persistent tool rail; board title and status strip; inspector and minimap docking.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.h
- /Users/ryanrentfro/code/markamp/src/ui/ActivityBar.cpp
- /Users/ryanrentfro/code/markamp/src/core/ActivityCommandProvider.cpp

## Workstream 02: Board Lifecycle
Tighten new/open/rename/duplicate/close flows so board management feels native and low-friction.

### P01-T011: Board Lifecycle UX Parity Audit
**Task Title:** Board Lifecycle UX Parity Audit
**Definition:** Define the implementation contract for board lifecycle by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around tighten new/open/rename/duplicate/close flows so board management feels native and low-friction.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** new board from command palette; inline board rename; duplicate board with preserved metadata.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/Board.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardSerializer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp

### P01-T012: Board Lifecycle Domain Model And State Contract
**Task Title:** Board Lifecycle Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support board lifecycle.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around tighten new/open/rename/duplicate/close flows so board management feels native and low-friction.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** new board from command palette; inline board rename; duplicate board with preserved metadata.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/Board.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardSerializer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp

### P01-T013: Board Lifecycle Commands Events And Context Keys
**Task Title:** Board Lifecycle Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for board lifecycle.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make tighten new/open/rename/duplicate/close flows so board management feels native and low-friction. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** new board from command palette; inline board rename; duplicate board with preserved metadata.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/Board.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardSerializer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp

### P01-T014: Board Lifecycle Workspace Surface And Controls
**Task Title:** Board Lifecycle Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose board lifecycle in the main canvas workspace.
**Description:** Build the primary UI surfaces for tighten new/open/rename/duplicate/close flows so board management feels native and low-friction., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** new board from command palette; inline board rename; duplicate board with preserved metadata.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/Board.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardSerializer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp

### P01-T015: Board Lifecycle Pointer Interaction Flow
**Task Title:** Board Lifecycle Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for board lifecycle.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for tighten new/open/rename/duplicate/close flows so board management feels native and low-friction.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** new board from command palette; inline board rename; duplicate board with preserved metadata.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/Board.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardSerializer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp

### P01-T016: Board Lifecycle Keyboard Gesture And Shortcut Flow
**Task Title:** Board Lifecycle Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for board lifecycle.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for tighten new/open/rename/duplicate/close flows so board management feels native and low-friction.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** new board from command palette; inline board rename; duplicate board with preserved metadata.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/Board.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardSerializer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp

### P01-T017: Board Lifecycle Visual Feedback And Rendering Polish
**Task Title:** Board Lifecycle Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make board lifecycle legible in use.
**Description:** Implement the visible feedback for tighten new/open/rename/duplicate/close flows so board management feels native and low-friction.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** new board from command palette; inline board rename; duplicate board with preserved metadata.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/Board.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardSerializer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp

### P01-T018: Board Lifecycle Persistence Preferences And Serialization
**Task Title:** Board Lifecycle Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by board lifecycle.
**Description:** Identify what parts of tighten new/open/rename/duplicate/close flows so board management feels native and low-friction. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** new board from command palette; inline board rename; duplicate board with preserved metadata.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/Board.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardSerializer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp

### P01-T019: Board Lifecycle Safeguards Telemetry And Recovery
**Task Title:** Board Lifecycle Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for board lifecycle.
**Description:** Instrument tighten new/open/rename/duplicate/close flows so board management feels native and low-friction. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** new board from command palette; inline board rename; duplicate board with preserved metadata.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/Board.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardSerializer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp

### P01-T020: Board Lifecycle Tests Documentation And Rollout Gate
**Task Title:** Board Lifecycle Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship board lifecycle.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for tighten new/open/rename/duplicate/close flows so board management feels native and low-friction.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** new board from command palette; inline board rename; duplicate board with preserved metadata.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/Board.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardSerializer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp

## Workstream 03: Viewport Camera
Deliver responsive pan, zoom, fit, and focal-point management across small and large boards.

### P01-T021: Viewport Camera UX Parity Audit
**Task Title:** Viewport Camera UX Parity Audit
**Definition:** Define the implementation contract for viewport camera by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around deliver responsive pan, zoom, fit, and focal-point management across small and large boards.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** zoom to fit; zoom around cursor; smooth camera recenter.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CameraModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ViewportTransform.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp

### P01-T022: Viewport Camera Domain Model And State Contract
**Task Title:** Viewport Camera Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support viewport camera.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around deliver responsive pan, zoom, fit, and focal-point management across small and large boards.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** zoom to fit; zoom around cursor; smooth camera recenter.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CameraModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ViewportTransform.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp

### P01-T023: Viewport Camera Commands Events And Context Keys
**Task Title:** Viewport Camera Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for viewport camera.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make deliver responsive pan, zoom, fit, and focal-point management across small and large boards. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** zoom to fit; zoom around cursor; smooth camera recenter.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CameraModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ViewportTransform.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp

### P01-T024: Viewport Camera Workspace Surface And Controls
**Task Title:** Viewport Camera Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose viewport camera in the main canvas workspace.
**Description:** Build the primary UI surfaces for deliver responsive pan, zoom, fit, and focal-point management across small and large boards., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** zoom to fit; zoom around cursor; smooth camera recenter.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CameraModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ViewportTransform.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp

### P01-T025: Viewport Camera Pointer Interaction Flow
**Task Title:** Viewport Camera Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for viewport camera.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for deliver responsive pan, zoom, fit, and focal-point management across small and large boards.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** zoom to fit; zoom around cursor; smooth camera recenter.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CameraModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ViewportTransform.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp

### P01-T026: Viewport Camera Keyboard Gesture And Shortcut Flow
**Task Title:** Viewport Camera Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for viewport camera.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for deliver responsive pan, zoom, fit, and focal-point management across small and large boards.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** zoom to fit; zoom around cursor; smooth camera recenter.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CameraModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ViewportTransform.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp

### P01-T027: Viewport Camera Visual Feedback And Rendering Polish
**Task Title:** Viewport Camera Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make viewport camera legible in use.
**Description:** Implement the visible feedback for deliver responsive pan, zoom, fit, and focal-point management across small and large boards.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** zoom to fit; zoom around cursor; smooth camera recenter.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CameraModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ViewportTransform.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp

### P01-T028: Viewport Camera Persistence Preferences And Serialization
**Task Title:** Viewport Camera Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by viewport camera.
**Description:** Identify what parts of deliver responsive pan, zoom, fit, and focal-point management across small and large boards. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** zoom to fit; zoom around cursor; smooth camera recenter.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CameraModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ViewportTransform.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp

### P01-T029: Viewport Camera Safeguards Telemetry And Recovery
**Task Title:** Viewport Camera Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for viewport camera.
**Description:** Instrument deliver responsive pan, zoom, fit, and focal-point management across small and large boards. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** zoom to fit; zoom around cursor; smooth camera recenter.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CameraModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ViewportTransform.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp

### P01-T030: Viewport Camera Tests Documentation And Rollout Gate
**Task Title:** Viewport Camera Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship viewport camera.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for deliver responsive pan, zoom, fit, and focal-point management across small and large boards.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** zoom to fit; zoom around cursor; smooth camera recenter.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CameraModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ViewportTransform.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp

## Workstream 04: Tool Rail
Rationalize tool discovery, activation, grouping, and customization for frequent creators.

### P01-T031: Tool Rail UX Parity Audit
**Task Title:** Tool Rail UX Parity Audit
**Definition:** Define the implementation contract for tool rail by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around rationalize tool discovery, activation, grouping, and customization for frequent creators.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** select and pan tools; drawing tool groups; recent tool memory.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ToolRailModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/FloatingToolbar.cpp
- /Users/ryanrentfro/code/markamp/src/ui/FloatingToolbarManager.cpp

### P01-T032: Tool Rail Domain Model And State Contract
**Task Title:** Tool Rail Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support tool rail.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around rationalize tool discovery, activation, grouping, and customization for frequent creators.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** select and pan tools; drawing tool groups; recent tool memory.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ToolRailModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/FloatingToolbar.cpp
- /Users/ryanrentfro/code/markamp/src/ui/FloatingToolbarManager.cpp

### P01-T033: Tool Rail Commands Events And Context Keys
**Task Title:** Tool Rail Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for tool rail.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make rationalize tool discovery, activation, grouping, and customization for frequent creators. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** select and pan tools; drawing tool groups; recent tool memory.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ToolRailModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/FloatingToolbar.cpp
- /Users/ryanrentfro/code/markamp/src/ui/FloatingToolbarManager.cpp

### P01-T034: Tool Rail Workspace Surface And Controls
**Task Title:** Tool Rail Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose tool rail in the main canvas workspace.
**Description:** Build the primary UI surfaces for rationalize tool discovery, activation, grouping, and customization for frequent creators., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** select and pan tools; drawing tool groups; recent tool memory.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ToolRailModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/FloatingToolbar.cpp
- /Users/ryanrentfro/code/markamp/src/ui/FloatingToolbarManager.cpp

### P01-T035: Tool Rail Pointer Interaction Flow
**Task Title:** Tool Rail Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for tool rail.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for rationalize tool discovery, activation, grouping, and customization for frequent creators.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** select and pan tools; drawing tool groups; recent tool memory.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ToolRailModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/FloatingToolbar.cpp
- /Users/ryanrentfro/code/markamp/src/ui/FloatingToolbarManager.cpp

### P01-T036: Tool Rail Keyboard Gesture And Shortcut Flow
**Task Title:** Tool Rail Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for tool rail.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for rationalize tool discovery, activation, grouping, and customization for frequent creators.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** select and pan tools; drawing tool groups; recent tool memory.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ToolRailModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/FloatingToolbar.cpp
- /Users/ryanrentfro/code/markamp/src/ui/FloatingToolbarManager.cpp

### P01-T037: Tool Rail Visual Feedback And Rendering Polish
**Task Title:** Tool Rail Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make tool rail legible in use.
**Description:** Implement the visible feedback for rationalize tool discovery, activation, grouping, and customization for frequent creators.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** select and pan tools; drawing tool groups; recent tool memory.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ToolRailModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/FloatingToolbar.cpp
- /Users/ryanrentfro/code/markamp/src/ui/FloatingToolbarManager.cpp

### P01-T038: Tool Rail Persistence Preferences And Serialization
**Task Title:** Tool Rail Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by tool rail.
**Description:** Identify what parts of rationalize tool discovery, activation, grouping, and customization for frequent creators. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** select and pan tools; drawing tool groups; recent tool memory.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ToolRailModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/FloatingToolbar.cpp
- /Users/ryanrentfro/code/markamp/src/ui/FloatingToolbarManager.cpp

### P01-T039: Tool Rail Safeguards Telemetry And Recovery
**Task Title:** Tool Rail Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for tool rail.
**Description:** Instrument rationalize tool discovery, activation, grouping, and customization for frequent creators. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** select and pan tools; drawing tool groups; recent tool memory.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ToolRailModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/FloatingToolbar.cpp
- /Users/ryanrentfro/code/markamp/src/ui/FloatingToolbarManager.cpp

### P01-T040: Tool Rail Tests Documentation And Rollout Gate
**Task Title:** Tool Rail Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship tool rail.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for rationalize tool discovery, activation, grouping, and customization for frequent creators.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** select and pan tools; drawing tool groups; recent tool memory.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ToolRailModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/FloatingToolbar.cpp
- /Users/ryanrentfro/code/markamp/src/ui/FloatingToolbarManager.cpp

## Workstream 05: Pointer Routing
Standardize mouse, trackpad, stylus, and touch routing so tools receive coherent pointer state.

### P01-T041: Pointer Routing UX Parity Audit
**Task Title:** Pointer Routing UX Parity Audit
**Definition:** Define the implementation contract for pointer routing by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around standardize mouse, trackpad, stylus, and touch routing so tools receive coherent pointer state.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** pointer capture; hover target updates; gesture-safe drag start.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PointerEventRouter.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/HitTestModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputModel.cpp

### P01-T042: Pointer Routing Domain Model And State Contract
**Task Title:** Pointer Routing Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support pointer routing.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around standardize mouse, trackpad, stylus, and touch routing so tools receive coherent pointer state.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** pointer capture; hover target updates; gesture-safe drag start.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PointerEventRouter.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/HitTestModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputModel.cpp

### P01-T043: Pointer Routing Commands Events And Context Keys
**Task Title:** Pointer Routing Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for pointer routing.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make standardize mouse, trackpad, stylus, and touch routing so tools receive coherent pointer state. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** pointer capture; hover target updates; gesture-safe drag start.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PointerEventRouter.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/HitTestModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputModel.cpp

### P01-T044: Pointer Routing Workspace Surface And Controls
**Task Title:** Pointer Routing Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose pointer routing in the main canvas workspace.
**Description:** Build the primary UI surfaces for standardize mouse, trackpad, stylus, and touch routing so tools receive coherent pointer state., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** pointer capture; hover target updates; gesture-safe drag start.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PointerEventRouter.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/HitTestModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputModel.cpp

### P01-T045: Pointer Routing Pointer Interaction Flow
**Task Title:** Pointer Routing Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for pointer routing.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for standardize mouse, trackpad, stylus, and touch routing so tools receive coherent pointer state.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** pointer capture; hover target updates; gesture-safe drag start.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PointerEventRouter.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/HitTestModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputModel.cpp

### P01-T046: Pointer Routing Keyboard Gesture And Shortcut Flow
**Task Title:** Pointer Routing Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for pointer routing.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for standardize mouse, trackpad, stylus, and touch routing so tools receive coherent pointer state.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** pointer capture; hover target updates; gesture-safe drag start.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PointerEventRouter.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/HitTestModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputModel.cpp

### P01-T047: Pointer Routing Visual Feedback And Rendering Polish
**Task Title:** Pointer Routing Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make pointer routing legible in use.
**Description:** Implement the visible feedback for standardize mouse, trackpad, stylus, and touch routing so tools receive coherent pointer state.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** pointer capture; hover target updates; gesture-safe drag start.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PointerEventRouter.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/HitTestModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputModel.cpp

### P01-T048: Pointer Routing Persistence Preferences And Serialization
**Task Title:** Pointer Routing Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by pointer routing.
**Description:** Identify what parts of standardize mouse, trackpad, stylus, and touch routing so tools receive coherent pointer state. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** pointer capture; hover target updates; gesture-safe drag start.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PointerEventRouter.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/HitTestModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputModel.cpp

### P01-T049: Pointer Routing Safeguards Telemetry And Recovery
**Task Title:** Pointer Routing Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for pointer routing.
**Description:** Instrument standardize mouse, trackpad, stylus, and touch routing so tools receive coherent pointer state. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** pointer capture; hover target updates; gesture-safe drag start.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PointerEventRouter.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/HitTestModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputModel.cpp

### P01-T050: Pointer Routing Tests Documentation And Rollout Gate
**Task Title:** Pointer Routing Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship pointer routing.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for standardize mouse, trackpad, stylus, and touch routing so tools receive coherent pointer state.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** pointer capture; hover target updates; gesture-safe drag start.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PointerEventRouter.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/HitTestModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputModel.cpp

## Workstream 06: Input State Machine
Harden tool transitions and transient states so creation, editing, and cancellation never conflict.

### P01-T051: Input State Machine UX Parity Audit
**Task Title:** Input State Machine UX Parity Audit
**Definition:** Define the implementation contract for input state machine by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around harden tool transitions and transient states so creation, editing, and cancellation never conflict.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** escape to cancel; tool-local preview state; resume previous tool after modal action.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolHost.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTool.h
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp

### P01-T052: Input State Machine Domain Model And State Contract
**Task Title:** Input State Machine Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support input state machine.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around harden tool transitions and transient states so creation, editing, and cancellation never conflict.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** escape to cancel; tool-local preview state; resume previous tool after modal action.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolHost.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTool.h
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp

### P01-T053: Input State Machine Commands Events And Context Keys
**Task Title:** Input State Machine Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for input state machine.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make harden tool transitions and transient states so creation, editing, and cancellation never conflict. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** escape to cancel; tool-local preview state; resume previous tool after modal action.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolHost.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTool.h
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp

### P01-T054: Input State Machine Workspace Surface And Controls
**Task Title:** Input State Machine Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose input state machine in the main canvas workspace.
**Description:** Build the primary UI surfaces for harden tool transitions and transient states so creation, editing, and cancellation never conflict., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** escape to cancel; tool-local preview state; resume previous tool after modal action.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolHost.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTool.h
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp

### P01-T055: Input State Machine Pointer Interaction Flow
**Task Title:** Input State Machine Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for input state machine.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for harden tool transitions and transient states so creation, editing, and cancellation never conflict.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** escape to cancel; tool-local preview state; resume previous tool after modal action.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolHost.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTool.h
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp

### P01-T056: Input State Machine Keyboard Gesture And Shortcut Flow
**Task Title:** Input State Machine Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for input state machine.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for harden tool transitions and transient states so creation, editing, and cancellation never conflict.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** escape to cancel; tool-local preview state; resume previous tool after modal action.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolHost.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTool.h
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp

### P01-T057: Input State Machine Visual Feedback And Rendering Polish
**Task Title:** Input State Machine Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make input state machine legible in use.
**Description:** Implement the visible feedback for harden tool transitions and transient states so creation, editing, and cancellation never conflict.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** escape to cancel; tool-local preview state; resume previous tool after modal action.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolHost.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTool.h
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp

### P01-T058: Input State Machine Persistence Preferences And Serialization
**Task Title:** Input State Machine Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by input state machine.
**Description:** Identify what parts of harden tool transitions and transient states so creation, editing, and cancellation never conflict. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** escape to cancel; tool-local preview state; resume previous tool after modal action.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolHost.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTool.h
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp

### P01-T059: Input State Machine Safeguards Telemetry And Recovery
**Task Title:** Input State Machine Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for input state machine.
**Description:** Instrument harden tool transitions and transient states so creation, editing, and cancellation never conflict. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** escape to cancel; tool-local preview state; resume previous tool after modal action.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolHost.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTool.h
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp

### P01-T060: Input State Machine Tests Documentation And Rollout Gate
**Task Title:** Input State Machine Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship input state machine.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for harden tool transitions and transient states so creation, editing, and cancellation never conflict.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** escape to cancel; tool-local preview state; resume previous tool after modal action.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolHost.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTool.h
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp

## Workstream 07: Keyboard Commanding
Expand keyboard-first execution so canvas power users can stay off the mouse when precision matters.

### P01-T061: Keyboard Commanding UX Parity Audit
**Task Title:** Keyboard Commanding UX Parity Audit
**Definition:** Define the implementation contract for keyboard commanding by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around expand keyboard-first execution so canvas power users can stay off the mouse when precision matters.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** tool shortcuts; command chords; mode-aware key handling.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KeyboardCommandModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandRegistry.cpp
- /Users/ryanrentfro/code/markamp/src/core/ChordShortcutManager.cpp

### P01-T062: Keyboard Commanding Domain Model And State Contract
**Task Title:** Keyboard Commanding Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support keyboard commanding.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around expand keyboard-first execution so canvas power users can stay off the mouse when precision matters.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** tool shortcuts; command chords; mode-aware key handling.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KeyboardCommandModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandRegistry.cpp
- /Users/ryanrentfro/code/markamp/src/core/ChordShortcutManager.cpp

### P01-T063: Keyboard Commanding Commands Events And Context Keys
**Task Title:** Keyboard Commanding Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for keyboard commanding.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make expand keyboard-first execution so canvas power users can stay off the mouse when precision matters. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** tool shortcuts; command chords; mode-aware key handling.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KeyboardCommandModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandRegistry.cpp
- /Users/ryanrentfro/code/markamp/src/core/ChordShortcutManager.cpp

### P01-T064: Keyboard Commanding Workspace Surface And Controls
**Task Title:** Keyboard Commanding Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose keyboard commanding in the main canvas workspace.
**Description:** Build the primary UI surfaces for expand keyboard-first execution so canvas power users can stay off the mouse when precision matters., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** tool shortcuts; command chords; mode-aware key handling.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KeyboardCommandModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandRegistry.cpp
- /Users/ryanrentfro/code/markamp/src/core/ChordShortcutManager.cpp

### P01-T065: Keyboard Commanding Pointer Interaction Flow
**Task Title:** Keyboard Commanding Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for keyboard commanding.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for expand keyboard-first execution so canvas power users can stay off the mouse when precision matters.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** tool shortcuts; command chords; mode-aware key handling.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KeyboardCommandModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandRegistry.cpp
- /Users/ryanrentfro/code/markamp/src/core/ChordShortcutManager.cpp

### P01-T066: Keyboard Commanding Keyboard Gesture And Shortcut Flow
**Task Title:** Keyboard Commanding Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for keyboard commanding.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for expand keyboard-first execution so canvas power users can stay off the mouse when precision matters.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** tool shortcuts; command chords; mode-aware key handling.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KeyboardCommandModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandRegistry.cpp
- /Users/ryanrentfro/code/markamp/src/core/ChordShortcutManager.cpp

### P01-T067: Keyboard Commanding Visual Feedback And Rendering Polish
**Task Title:** Keyboard Commanding Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make keyboard commanding legible in use.
**Description:** Implement the visible feedback for expand keyboard-first execution so canvas power users can stay off the mouse when precision matters.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** tool shortcuts; command chords; mode-aware key handling.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KeyboardCommandModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandRegistry.cpp
- /Users/ryanrentfro/code/markamp/src/core/ChordShortcutManager.cpp

### P01-T068: Keyboard Commanding Persistence Preferences And Serialization
**Task Title:** Keyboard Commanding Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by keyboard commanding.
**Description:** Identify what parts of expand keyboard-first execution so canvas power users can stay off the mouse when precision matters. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** tool shortcuts; command chords; mode-aware key handling.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KeyboardCommandModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandRegistry.cpp
- /Users/ryanrentfro/code/markamp/src/core/ChordShortcutManager.cpp

### P01-T069: Keyboard Commanding Safeguards Telemetry And Recovery
**Task Title:** Keyboard Commanding Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for keyboard commanding.
**Description:** Instrument expand keyboard-first execution so canvas power users can stay off the mouse when precision matters. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** tool shortcuts; command chords; mode-aware key handling.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KeyboardCommandModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandRegistry.cpp
- /Users/ryanrentfro/code/markamp/src/core/ChordShortcutManager.cpp

### P01-T070: Keyboard Commanding Tests Documentation And Rollout Gate
**Task Title:** Keyboard Commanding Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship keyboard commanding.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for expand keyboard-first execution so canvas power users can stay off the mouse when precision matters.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** tool shortcuts; command chords; mode-aware key handling.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KeyboardCommandModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandRegistry.cpp
- /Users/ryanrentfro/code/markamp/src/core/ChordShortcutManager.cpp

## Workstream 08: Cursor And Mode Feedback
Make the active tool, pending action, and allowed drop target legible through cursor and status feedback.

### P01-T071: Cursor And Mode Feedback UX Parity Audit
**Task Title:** Cursor And Mode Feedback UX Parity Audit
**Definition:** Define the implementation contract for cursor and mode feedback by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around make the active tool, pending action, and allowed drop target legible through cursor and status feedback.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** crosshair for draw; grab/grabbing for pan; invalid-target cursor.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAffordanceController.cpp
- /Users/ryanrentfro/code/markamp/src/ui/FocusRingRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ContextMenuModel.cpp

### P01-T072: Cursor And Mode Feedback Domain Model And State Contract
**Task Title:** Cursor And Mode Feedback Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support cursor and mode feedback.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around make the active tool, pending action, and allowed drop target legible through cursor and status feedback.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** crosshair for draw; grab/grabbing for pan; invalid-target cursor.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAffordanceController.cpp
- /Users/ryanrentfro/code/markamp/src/ui/FocusRingRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ContextMenuModel.cpp

### P01-T073: Cursor And Mode Feedback Commands Events And Context Keys
**Task Title:** Cursor And Mode Feedback Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for cursor and mode feedback.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make make the active tool, pending action, and allowed drop target legible through cursor and status feedback. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** crosshair for draw; grab/grabbing for pan; invalid-target cursor.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAffordanceController.cpp
- /Users/ryanrentfro/code/markamp/src/ui/FocusRingRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ContextMenuModel.cpp

### P01-T074: Cursor And Mode Feedback Workspace Surface And Controls
**Task Title:** Cursor And Mode Feedback Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose cursor and mode feedback in the main canvas workspace.
**Description:** Build the primary UI surfaces for make the active tool, pending action, and allowed drop target legible through cursor and status feedback., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** crosshair for draw; grab/grabbing for pan; invalid-target cursor.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAffordanceController.cpp
- /Users/ryanrentfro/code/markamp/src/ui/FocusRingRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ContextMenuModel.cpp

### P01-T075: Cursor And Mode Feedback Pointer Interaction Flow
**Task Title:** Cursor And Mode Feedback Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for cursor and mode feedback.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for make the active tool, pending action, and allowed drop target legible through cursor and status feedback.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** crosshair for draw; grab/grabbing for pan; invalid-target cursor.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAffordanceController.cpp
- /Users/ryanrentfro/code/markamp/src/ui/FocusRingRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ContextMenuModel.cpp

### P01-T076: Cursor And Mode Feedback Keyboard Gesture And Shortcut Flow
**Task Title:** Cursor And Mode Feedback Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for cursor and mode feedback.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for make the active tool, pending action, and allowed drop target legible through cursor and status feedback.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** crosshair for draw; grab/grabbing for pan; invalid-target cursor.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAffordanceController.cpp
- /Users/ryanrentfro/code/markamp/src/ui/FocusRingRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ContextMenuModel.cpp

### P01-T077: Cursor And Mode Feedback Visual Feedback And Rendering Polish
**Task Title:** Cursor And Mode Feedback Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make cursor and mode feedback legible in use.
**Description:** Implement the visible feedback for make the active tool, pending action, and allowed drop target legible through cursor and status feedback.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** crosshair for draw; grab/grabbing for pan; invalid-target cursor.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAffordanceController.cpp
- /Users/ryanrentfro/code/markamp/src/ui/FocusRingRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ContextMenuModel.cpp

### P01-T078: Cursor And Mode Feedback Persistence Preferences And Serialization
**Task Title:** Cursor And Mode Feedback Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by cursor and mode feedback.
**Description:** Identify what parts of make the active tool, pending action, and allowed drop target legible through cursor and status feedback. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** crosshair for draw; grab/grabbing for pan; invalid-target cursor.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAffordanceController.cpp
- /Users/ryanrentfro/code/markamp/src/ui/FocusRingRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ContextMenuModel.cpp

### P01-T079: Cursor And Mode Feedback Safeguards Telemetry And Recovery
**Task Title:** Cursor And Mode Feedback Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for cursor and mode feedback.
**Description:** Instrument make the active tool, pending action, and allowed drop target legible through cursor and status feedback. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** crosshair for draw; grab/grabbing for pan; invalid-target cursor.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAffordanceController.cpp
- /Users/ryanrentfro/code/markamp/src/ui/FocusRingRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ContextMenuModel.cpp

### P01-T080: Cursor And Mode Feedback Tests Documentation And Rollout Gate
**Task Title:** Cursor And Mode Feedback Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship cursor and mode feedback.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for make the active tool, pending action, and allowed drop target legible through cursor and status feedback.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** crosshair for draw; grab/grabbing for pan; invalid-target cursor.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAffordanceController.cpp
- /Users/ryanrentfro/code/markamp/src/ui/FocusRingRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ContextMenuModel.cpp

## Workstream 09: Grid And Rulers
Add the spatial scaffolding creators expect when placing or measuring objects.

### P01-T081: Grid And Rulers UX Parity Audit
**Task Title:** Grid And Rulers UX Parity Audit
**Definition:** Define the implementation contract for grid and rulers by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around add the spatial scaffolding creators expect when placing or measuring objects.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** toggle grid; ruler units; origin marker.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GridGuideController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AlignmentGuides.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasRenderer.cpp

### P01-T082: Grid And Rulers Domain Model And State Contract
**Task Title:** Grid And Rulers Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support grid and rulers.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around add the spatial scaffolding creators expect when placing or measuring objects.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** toggle grid; ruler units; origin marker.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GridGuideController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AlignmentGuides.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasRenderer.cpp

### P01-T083: Grid And Rulers Commands Events And Context Keys
**Task Title:** Grid And Rulers Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for grid and rulers.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make add the spatial scaffolding creators expect when placing or measuring objects. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** toggle grid; ruler units; origin marker.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GridGuideController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AlignmentGuides.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasRenderer.cpp

### P01-T084: Grid And Rulers Workspace Surface And Controls
**Task Title:** Grid And Rulers Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose grid and rulers in the main canvas workspace.
**Description:** Build the primary UI surfaces for add the spatial scaffolding creators expect when placing or measuring objects., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** toggle grid; ruler units; origin marker.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GridGuideController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AlignmentGuides.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasRenderer.cpp

### P01-T085: Grid And Rulers Pointer Interaction Flow
**Task Title:** Grid And Rulers Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for grid and rulers.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for add the spatial scaffolding creators expect when placing or measuring objects.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** toggle grid; ruler units; origin marker.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GridGuideController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AlignmentGuides.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasRenderer.cpp

### P01-T086: Grid And Rulers Keyboard Gesture And Shortcut Flow
**Task Title:** Grid And Rulers Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for grid and rulers.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for add the spatial scaffolding creators expect when placing or measuring objects.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** toggle grid; ruler units; origin marker.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GridGuideController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AlignmentGuides.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasRenderer.cpp

### P01-T087: Grid And Rulers Visual Feedback And Rendering Polish
**Task Title:** Grid And Rulers Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make grid and rulers legible in use.
**Description:** Implement the visible feedback for add the spatial scaffolding creators expect when placing or measuring objects.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** toggle grid; ruler units; origin marker.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GridGuideController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AlignmentGuides.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasRenderer.cpp

### P01-T088: Grid And Rulers Persistence Preferences And Serialization
**Task Title:** Grid And Rulers Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by grid and rulers.
**Description:** Identify what parts of add the spatial scaffolding creators expect when placing or measuring objects. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** toggle grid; ruler units; origin marker.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GridGuideController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AlignmentGuides.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasRenderer.cpp

### P01-T089: Grid And Rulers Safeguards Telemetry And Recovery
**Task Title:** Grid And Rulers Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for grid and rulers.
**Description:** Instrument add the spatial scaffolding creators expect when placing or measuring objects. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** toggle grid; ruler units; origin marker.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GridGuideController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AlignmentGuides.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasRenderer.cpp

### P01-T090: Grid And Rulers Tests Documentation And Rollout Gate
**Task Title:** Grid And Rulers Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship grid and rulers.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for add the spatial scaffolding creators expect when placing or measuring objects.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** toggle grid; ruler units; origin marker.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GridGuideController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AlignmentGuides.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasRenderer.cpp

## Workstream 10: Snap Preferences
Make snapping predictable, configurable, and transparent across all transform tools.

### P01-T091: Snap Preferences UX Parity Audit
**Task Title:** Snap Preferences UX Parity Audit
**Definition:** Define the implementation contract for snap preferences by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around make snapping predictable, configurable, and transparent across all transform tools.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** snap to grid; snap to geometry; temporary snap disable modifier.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SnapEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasCommands.cpp
- /Users/ryanrentfro/code/markamp/src/core/Config.cpp

### P01-T092: Snap Preferences Domain Model And State Contract
**Task Title:** Snap Preferences Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support snap preferences.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around make snapping predictable, configurable, and transparent across all transform tools.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** snap to grid; snap to geometry; temporary snap disable modifier.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SnapEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasCommands.cpp
- /Users/ryanrentfro/code/markamp/src/core/Config.cpp

### P01-T093: Snap Preferences Commands Events And Context Keys
**Task Title:** Snap Preferences Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for snap preferences.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make make snapping predictable, configurable, and transparent across all transform tools. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** snap to grid; snap to geometry; temporary snap disable modifier.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SnapEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasCommands.cpp
- /Users/ryanrentfro/code/markamp/src/core/Config.cpp

### P01-T094: Snap Preferences Workspace Surface And Controls
**Task Title:** Snap Preferences Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose snap preferences in the main canvas workspace.
**Description:** Build the primary UI surfaces for make snapping predictable, configurable, and transparent across all transform tools., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** snap to grid; snap to geometry; temporary snap disable modifier.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SnapEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasCommands.cpp
- /Users/ryanrentfro/code/markamp/src/core/Config.cpp

### P01-T095: Snap Preferences Pointer Interaction Flow
**Task Title:** Snap Preferences Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for snap preferences.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for make snapping predictable, configurable, and transparent across all transform tools.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** snap to grid; snap to geometry; temporary snap disable modifier.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SnapEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasCommands.cpp
- /Users/ryanrentfro/code/markamp/src/core/Config.cpp

### P01-T096: Snap Preferences Keyboard Gesture And Shortcut Flow
**Task Title:** Snap Preferences Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for snap preferences.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for make snapping predictable, configurable, and transparent across all transform tools.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** snap to grid; snap to geometry; temporary snap disable modifier.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SnapEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasCommands.cpp
- /Users/ryanrentfro/code/markamp/src/core/Config.cpp

### P01-T097: Snap Preferences Visual Feedback And Rendering Polish
**Task Title:** Snap Preferences Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make snap preferences legible in use.
**Description:** Implement the visible feedback for make snapping predictable, configurable, and transparent across all transform tools.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** snap to grid; snap to geometry; temporary snap disable modifier.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SnapEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasCommands.cpp
- /Users/ryanrentfro/code/markamp/src/core/Config.cpp

### P01-T098: Snap Preferences Persistence Preferences And Serialization
**Task Title:** Snap Preferences Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by snap preferences.
**Description:** Identify what parts of make snapping predictable, configurable, and transparent across all transform tools. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** snap to grid; snap to geometry; temporary snap disable modifier.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SnapEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasCommands.cpp
- /Users/ryanrentfro/code/markamp/src/core/Config.cpp

### P01-T099: Snap Preferences Safeguards Telemetry And Recovery
**Task Title:** Snap Preferences Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for snap preferences.
**Description:** Instrument make snapping predictable, configurable, and transparent across all transform tools. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** snap to grid; snap to geometry; temporary snap disable modifier.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SnapEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasCommands.cpp
- /Users/ryanrentfro/code/markamp/src/core/Config.cpp

### P01-T100: Snap Preferences Tests Documentation And Rollout Gate
**Task Title:** Snap Preferences Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship snap preferences.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for make snapping predictable, configurable, and transparent across all transform tools.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** snap to grid; snap to geometry; temporary snap disable modifier.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SnapEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasCommands.cpp
- /Users/ryanrentfro/code/markamp/src/core/Config.cpp

## Workstream 11: Context Menus
Turn right-click and long-press into mode-aware quick action hubs instead of generic command dumps.

### P01-T101: Context Menus UX Parity Audit
**Task Title:** Context Menus UX Parity Audit
**Definition:** Define the implementation contract for context menus by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around turn right-click and long-press into mode-aware quick action hubs instead of generic command dumps.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** object menu; empty-canvas menu; selection-specific bulk actions.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasContextMenu.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ContextMenuRegistry.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ContextMenuBuilder.cpp

### P01-T102: Context Menus Domain Model And State Contract
**Task Title:** Context Menus Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support context menus.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around turn right-click and long-press into mode-aware quick action hubs instead of generic command dumps.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** object menu; empty-canvas menu; selection-specific bulk actions.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasContextMenu.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ContextMenuRegistry.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ContextMenuBuilder.cpp

### P01-T103: Context Menus Commands Events And Context Keys
**Task Title:** Context Menus Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for context menus.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make turn right-click and long-press into mode-aware quick action hubs instead of generic command dumps. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** object menu; empty-canvas menu; selection-specific bulk actions.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasContextMenu.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ContextMenuRegistry.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ContextMenuBuilder.cpp

### P01-T104: Context Menus Workspace Surface And Controls
**Task Title:** Context Menus Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose context menus in the main canvas workspace.
**Description:** Build the primary UI surfaces for turn right-click and long-press into mode-aware quick action hubs instead of generic command dumps., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** object menu; empty-canvas menu; selection-specific bulk actions.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasContextMenu.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ContextMenuRegistry.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ContextMenuBuilder.cpp

### P01-T105: Context Menus Pointer Interaction Flow
**Task Title:** Context Menus Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for context menus.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for turn right-click and long-press into mode-aware quick action hubs instead of generic command dumps.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** object menu; empty-canvas menu; selection-specific bulk actions.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasContextMenu.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ContextMenuRegistry.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ContextMenuBuilder.cpp

### P01-T106: Context Menus Keyboard Gesture And Shortcut Flow
**Task Title:** Context Menus Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for context menus.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for turn right-click and long-press into mode-aware quick action hubs instead of generic command dumps.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** object menu; empty-canvas menu; selection-specific bulk actions.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasContextMenu.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ContextMenuRegistry.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ContextMenuBuilder.cpp

### P01-T107: Context Menus Visual Feedback And Rendering Polish
**Task Title:** Context Menus Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make context menus legible in use.
**Description:** Implement the visible feedback for turn right-click and long-press into mode-aware quick action hubs instead of generic command dumps.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** object menu; empty-canvas menu; selection-specific bulk actions.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasContextMenu.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ContextMenuRegistry.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ContextMenuBuilder.cpp

### P01-T108: Context Menus Persistence Preferences And Serialization
**Task Title:** Context Menus Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by context menus.
**Description:** Identify what parts of turn right-click and long-press into mode-aware quick action hubs instead of generic command dumps. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** object menu; empty-canvas menu; selection-specific bulk actions.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasContextMenu.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ContextMenuRegistry.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ContextMenuBuilder.cpp

### P01-T109: Context Menus Safeguards Telemetry And Recovery
**Task Title:** Context Menus Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for context menus.
**Description:** Instrument turn right-click and long-press into mode-aware quick action hubs instead of generic command dumps. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** object menu; empty-canvas menu; selection-specific bulk actions.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasContextMenu.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ContextMenuRegistry.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ContextMenuBuilder.cpp

### P01-T110: Context Menus Tests Documentation And Rollout Gate
**Task Title:** Context Menus Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship context menus.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for turn right-click and long-press into mode-aware quick action hubs instead of generic command dumps.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** object menu; empty-canvas menu; selection-specific bulk actions.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasContextMenu.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ContextMenuRegistry.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ContextMenuBuilder.cpp

## Workstream 12: Undo And Redo Plumbing
Make all canvas operations produce crisp history entries with reliable inversion.

### P01-T111: Undo And Redo Plumbing UX Parity Audit
**Task Title:** Undo And Redo Plumbing UX Parity Audit
**Definition:** Define the implementation contract for undo and redo plumbing by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around make all canvas operations produce crisp history entries with reliable inversion.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** coalesced move history; redo after style edit; history labels visible in UI.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasCommands.h
- /Users/ryanrentfro/code/markamp/src/canvas/CollabUndoRedoManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragUndoAction.cpp

### P01-T112: Undo And Redo Plumbing Domain Model And State Contract
**Task Title:** Undo And Redo Plumbing Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support undo and redo plumbing.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around make all canvas operations produce crisp history entries with reliable inversion.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** coalesced move history; redo after style edit; history labels visible in UI.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasCommands.h
- /Users/ryanrentfro/code/markamp/src/canvas/CollabUndoRedoManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragUndoAction.cpp

### P01-T113: Undo And Redo Plumbing Commands Events And Context Keys
**Task Title:** Undo And Redo Plumbing Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for undo and redo plumbing.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make make all canvas operations produce crisp history entries with reliable inversion. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** coalesced move history; redo after style edit; history labels visible in UI.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasCommands.h
- /Users/ryanrentfro/code/markamp/src/canvas/CollabUndoRedoManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragUndoAction.cpp

### P01-T114: Undo And Redo Plumbing Workspace Surface And Controls
**Task Title:** Undo And Redo Plumbing Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose undo and redo plumbing in the main canvas workspace.
**Description:** Build the primary UI surfaces for make all canvas operations produce crisp history entries with reliable inversion., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** coalesced move history; redo after style edit; history labels visible in UI.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasCommands.h
- /Users/ryanrentfro/code/markamp/src/canvas/CollabUndoRedoManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragUndoAction.cpp

### P01-T115: Undo And Redo Plumbing Pointer Interaction Flow
**Task Title:** Undo And Redo Plumbing Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for undo and redo plumbing.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for make all canvas operations produce crisp history entries with reliable inversion.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** coalesced move history; redo after style edit; history labels visible in UI.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasCommands.h
- /Users/ryanrentfro/code/markamp/src/canvas/CollabUndoRedoManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragUndoAction.cpp

### P01-T116: Undo And Redo Plumbing Keyboard Gesture And Shortcut Flow
**Task Title:** Undo And Redo Plumbing Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for undo and redo plumbing.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for make all canvas operations produce crisp history entries with reliable inversion.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** coalesced move history; redo after style edit; history labels visible in UI.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasCommands.h
- /Users/ryanrentfro/code/markamp/src/canvas/CollabUndoRedoManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragUndoAction.cpp

### P01-T117: Undo And Redo Plumbing Visual Feedback And Rendering Polish
**Task Title:** Undo And Redo Plumbing Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make undo and redo plumbing legible in use.
**Description:** Implement the visible feedback for make all canvas operations produce crisp history entries with reliable inversion.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** coalesced move history; redo after style edit; history labels visible in UI.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasCommands.h
- /Users/ryanrentfro/code/markamp/src/canvas/CollabUndoRedoManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragUndoAction.cpp

### P01-T118: Undo And Redo Plumbing Persistence Preferences And Serialization
**Task Title:** Undo And Redo Plumbing Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by undo and redo plumbing.
**Description:** Identify what parts of make all canvas operations produce crisp history entries with reliable inversion. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** coalesced move history; redo after style edit; history labels visible in UI.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasCommands.h
- /Users/ryanrentfro/code/markamp/src/canvas/CollabUndoRedoManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragUndoAction.cpp

### P01-T119: Undo And Redo Plumbing Safeguards Telemetry And Recovery
**Task Title:** Undo And Redo Plumbing Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for undo and redo plumbing.
**Description:** Instrument make all canvas operations produce crisp history entries with reliable inversion. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** coalesced move history; redo after style edit; history labels visible in UI.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasCommands.h
- /Users/ryanrentfro/code/markamp/src/canvas/CollabUndoRedoManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragUndoAction.cpp

### P01-T120: Undo And Redo Plumbing Tests Documentation And Rollout Gate
**Task Title:** Undo And Redo Plumbing Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship undo and redo plumbing.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for make all canvas operations produce crisp history entries with reliable inversion.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** coalesced move history; redo after style edit; history labels visible in UI.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasCommands.h
- /Users/ryanrentfro/code/markamp/src/canvas/CollabUndoRedoManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragUndoAction.cpp

## Workstream 13: Autosave And Recovery
Ensure users can trust boards to survive crashes, force-quits, and sync interruptions.

### P01-T121: Autosave And Recovery UX Parity Audit
**Task Title:** Autosave And Recovery UX Parity Audit
**Definition:** Define the implementation contract for autosave and recovery by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around ensure users can trust boards to survive crashes, force-quits, and sync interruptions.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** debounced autosave; restore unsaved board; dirty-state indicator.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardSerializerModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/AtomicWriter.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp

### P01-T122: Autosave And Recovery Domain Model And State Contract
**Task Title:** Autosave And Recovery Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support autosave and recovery.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around ensure users can trust boards to survive crashes, force-quits, and sync interruptions.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** debounced autosave; restore unsaved board; dirty-state indicator.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardSerializerModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/AtomicWriter.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp

### P01-T123: Autosave And Recovery Commands Events And Context Keys
**Task Title:** Autosave And Recovery Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for autosave and recovery.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make ensure users can trust boards to survive crashes, force-quits, and sync interruptions. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** debounced autosave; restore unsaved board; dirty-state indicator.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardSerializerModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/AtomicWriter.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp

### P01-T124: Autosave And Recovery Workspace Surface And Controls
**Task Title:** Autosave And Recovery Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose autosave and recovery in the main canvas workspace.
**Description:** Build the primary UI surfaces for ensure users can trust boards to survive crashes, force-quits, and sync interruptions., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** debounced autosave; restore unsaved board; dirty-state indicator.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardSerializerModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/AtomicWriter.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp

### P01-T125: Autosave And Recovery Pointer Interaction Flow
**Task Title:** Autosave And Recovery Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for autosave and recovery.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for ensure users can trust boards to survive crashes, force-quits, and sync interruptions.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** debounced autosave; restore unsaved board; dirty-state indicator.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardSerializerModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/AtomicWriter.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp

### P01-T126: Autosave And Recovery Keyboard Gesture And Shortcut Flow
**Task Title:** Autosave And Recovery Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for autosave and recovery.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for ensure users can trust boards to survive crashes, force-quits, and sync interruptions.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** debounced autosave; restore unsaved board; dirty-state indicator.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardSerializerModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/AtomicWriter.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp

### P01-T127: Autosave And Recovery Visual Feedback And Rendering Polish
**Task Title:** Autosave And Recovery Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make autosave and recovery legible in use.
**Description:** Implement the visible feedback for ensure users can trust boards to survive crashes, force-quits, and sync interruptions.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** debounced autosave; restore unsaved board; dirty-state indicator.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardSerializerModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/AtomicWriter.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp

### P01-T128: Autosave And Recovery Persistence Preferences And Serialization
**Task Title:** Autosave And Recovery Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by autosave and recovery.
**Description:** Identify what parts of ensure users can trust boards to survive crashes, force-quits, and sync interruptions. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** debounced autosave; restore unsaved board; dirty-state indicator.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardSerializerModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/AtomicWriter.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp

### P01-T129: Autosave And Recovery Safeguards Telemetry And Recovery
**Task Title:** Autosave And Recovery Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for autosave and recovery.
**Description:** Instrument ensure users can trust boards to survive crashes, force-quits, and sync interruptions. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** debounced autosave; restore unsaved board; dirty-state indicator.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardSerializerModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/AtomicWriter.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp

### P01-T130: Autosave And Recovery Tests Documentation And Rollout Gate
**Task Title:** Autosave And Recovery Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship autosave and recovery.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for ensure users can trust boards to survive crashes, force-quits, and sync interruptions.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** debounced autosave; restore unsaved board; dirty-state indicator.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardSerializerModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/AtomicWriter.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp

## Workstream 14: Inspector Shell
Define a scalable inspector frame that supports board, single-object, and multi-object editing states.

### P01-T131: Inspector Shell UX Parity Audit
**Task Title:** Inspector Shell UX Parity Audit
**Definition:** Define the implementation contract for inspector shell by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around define a scalable inspector frame that supports board, single-object, and multi-object editing states.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** board properties view; multi-select mixed values; contextual action slots.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInspector.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/InspectorModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MetadataPanel.cpp

### P01-T132: Inspector Shell Domain Model And State Contract
**Task Title:** Inspector Shell Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support inspector shell.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around define a scalable inspector frame that supports board, single-object, and multi-object editing states.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** board properties view; multi-select mixed values; contextual action slots.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInspector.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/InspectorModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MetadataPanel.cpp

### P01-T133: Inspector Shell Commands Events And Context Keys
**Task Title:** Inspector Shell Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for inspector shell.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make define a scalable inspector frame that supports board, single-object, and multi-object editing states. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** board properties view; multi-select mixed values; contextual action slots.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInspector.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/InspectorModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MetadataPanel.cpp

### P01-T134: Inspector Shell Workspace Surface And Controls
**Task Title:** Inspector Shell Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose inspector shell in the main canvas workspace.
**Description:** Build the primary UI surfaces for define a scalable inspector frame that supports board, single-object, and multi-object editing states., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** board properties view; multi-select mixed values; contextual action slots.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInspector.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/InspectorModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MetadataPanel.cpp

### P01-T135: Inspector Shell Pointer Interaction Flow
**Task Title:** Inspector Shell Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for inspector shell.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for define a scalable inspector frame that supports board, single-object, and multi-object editing states.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** board properties view; multi-select mixed values; contextual action slots.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInspector.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/InspectorModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MetadataPanel.cpp

### P01-T136: Inspector Shell Keyboard Gesture And Shortcut Flow
**Task Title:** Inspector Shell Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for inspector shell.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for define a scalable inspector frame that supports board, single-object, and multi-object editing states.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** board properties view; multi-select mixed values; contextual action slots.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInspector.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/InspectorModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MetadataPanel.cpp

### P01-T137: Inspector Shell Visual Feedback And Rendering Polish
**Task Title:** Inspector Shell Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make inspector shell legible in use.
**Description:** Implement the visible feedback for define a scalable inspector frame that supports board, single-object, and multi-object editing states.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** board properties view; multi-select mixed values; contextual action slots.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInspector.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/InspectorModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MetadataPanel.cpp

### P01-T138: Inspector Shell Persistence Preferences And Serialization
**Task Title:** Inspector Shell Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by inspector shell.
**Description:** Identify what parts of define a scalable inspector frame that supports board, single-object, and multi-object editing states. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** board properties view; multi-select mixed values; contextual action slots.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInspector.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/InspectorModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MetadataPanel.cpp

### P01-T139: Inspector Shell Safeguards Telemetry And Recovery
**Task Title:** Inspector Shell Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for inspector shell.
**Description:** Instrument define a scalable inspector frame that supports board, single-object, and multi-object editing states. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** board properties view; multi-select mixed values; contextual action slots.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInspector.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/InspectorModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MetadataPanel.cpp

### P01-T140: Inspector Shell Tests Documentation And Rollout Gate
**Task Title:** Inspector Shell Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship inspector shell.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for define a scalable inspector frame that supports board, single-object, and multi-object editing states.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** board properties view; multi-select mixed values; contextual action slots.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInspector.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/InspectorModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MetadataPanel.cpp

## Workstream 15: Minimap Shell
Expose orientation and rapid relocation without overloading the main board surface.

### P01-T141: Minimap Shell UX Parity Audit
**Task Title:** Minimap Shell UX Parity Audit
**Definition:** Define the implementation contract for minimap shell by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around expose orientation and rapid relocation without overloading the main board surface.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** viewport rectangle; click to pan; section labels in minimap.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp

### P01-T142: Minimap Shell Domain Model And State Contract
**Task Title:** Minimap Shell Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support minimap shell.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around expose orientation and rapid relocation without overloading the main board surface.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** viewport rectangle; click to pan; section labels in minimap.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp

### P01-T143: Minimap Shell Commands Events And Context Keys
**Task Title:** Minimap Shell Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for minimap shell.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make expose orientation and rapid relocation without overloading the main board surface. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** viewport rectangle; click to pan; section labels in minimap.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp

### P01-T144: Minimap Shell Workspace Surface And Controls
**Task Title:** Minimap Shell Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose minimap shell in the main canvas workspace.
**Description:** Build the primary UI surfaces for expose orientation and rapid relocation without overloading the main board surface., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** viewport rectangle; click to pan; section labels in minimap.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp

### P01-T145: Minimap Shell Pointer Interaction Flow
**Task Title:** Minimap Shell Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for minimap shell.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for expose orientation and rapid relocation without overloading the main board surface.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** viewport rectangle; click to pan; section labels in minimap.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp

### P01-T146: Minimap Shell Keyboard Gesture And Shortcut Flow
**Task Title:** Minimap Shell Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for minimap shell.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for expose orientation and rapid relocation without overloading the main board surface.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** viewport rectangle; click to pan; section labels in minimap.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp

### P01-T147: Minimap Shell Visual Feedback And Rendering Polish
**Task Title:** Minimap Shell Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make minimap shell legible in use.
**Description:** Implement the visible feedback for expose orientation and rapid relocation without overloading the main board surface.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** viewport rectangle; click to pan; section labels in minimap.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp

### P01-T148: Minimap Shell Persistence Preferences And Serialization
**Task Title:** Minimap Shell Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by minimap shell.
**Description:** Identify what parts of expose orientation and rapid relocation without overloading the main board surface. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** viewport rectangle; click to pan; section labels in minimap.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp

### P01-T149: Minimap Shell Safeguards Telemetry And Recovery
**Task Title:** Minimap Shell Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for minimap shell.
**Description:** Instrument expose orientation and rapid relocation without overloading the main board surface. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** viewport rectangle; click to pan; section labels in minimap.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp

### P01-T150: Minimap Shell Tests Documentation And Rollout Gate
**Task Title:** Minimap Shell Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship minimap shell.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for expose orientation and rapid relocation without overloading the main board surface.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** viewport rectangle; click to pan; section labels in minimap.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp

## Workstream 16: Onboarding And Empty States
Give first-run canvas users enough guidance to succeed without burying experts in chrome.

### P01-T151: Onboarding And Empty States UX Parity Audit
**Task Title:** Onboarding And Empty States UX Parity Audit
**Definition:** Define the implementation contract for onboarding and empty states by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around give first-run canvas users enough guidance to succeed without burying experts in chrome.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** blank board hints; tool coachmarks; template-first creation.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OnboardingModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/FirstRunWizard.cpp
- /Users/ryanrentfro/code/markamp/src/ui/EmptyPanelState.cpp

### P01-T152: Onboarding And Empty States Domain Model And State Contract
**Task Title:** Onboarding And Empty States Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support onboarding and empty states.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around give first-run canvas users enough guidance to succeed without burying experts in chrome.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** blank board hints; tool coachmarks; template-first creation.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OnboardingModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/FirstRunWizard.cpp
- /Users/ryanrentfro/code/markamp/src/ui/EmptyPanelState.cpp

### P01-T153: Onboarding And Empty States Commands Events And Context Keys
**Task Title:** Onboarding And Empty States Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for onboarding and empty states.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make give first-run canvas users enough guidance to succeed without burying experts in chrome. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** blank board hints; tool coachmarks; template-first creation.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OnboardingModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/FirstRunWizard.cpp
- /Users/ryanrentfro/code/markamp/src/ui/EmptyPanelState.cpp

### P01-T154: Onboarding And Empty States Workspace Surface And Controls
**Task Title:** Onboarding And Empty States Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose onboarding and empty states in the main canvas workspace.
**Description:** Build the primary UI surfaces for give first-run canvas users enough guidance to succeed without burying experts in chrome., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** blank board hints; tool coachmarks; template-first creation.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OnboardingModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/FirstRunWizard.cpp
- /Users/ryanrentfro/code/markamp/src/ui/EmptyPanelState.cpp

### P01-T155: Onboarding And Empty States Pointer Interaction Flow
**Task Title:** Onboarding And Empty States Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for onboarding and empty states.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for give first-run canvas users enough guidance to succeed without burying experts in chrome.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** blank board hints; tool coachmarks; template-first creation.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OnboardingModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/FirstRunWizard.cpp
- /Users/ryanrentfro/code/markamp/src/ui/EmptyPanelState.cpp

### P01-T156: Onboarding And Empty States Keyboard Gesture And Shortcut Flow
**Task Title:** Onboarding And Empty States Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for onboarding and empty states.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for give first-run canvas users enough guidance to succeed without burying experts in chrome.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** blank board hints; tool coachmarks; template-first creation.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OnboardingModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/FirstRunWizard.cpp
- /Users/ryanrentfro/code/markamp/src/ui/EmptyPanelState.cpp

### P01-T157: Onboarding And Empty States Visual Feedback And Rendering Polish
**Task Title:** Onboarding And Empty States Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make onboarding and empty states legible in use.
**Description:** Implement the visible feedback for give first-run canvas users enough guidance to succeed without burying experts in chrome.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** blank board hints; tool coachmarks; template-first creation.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OnboardingModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/FirstRunWizard.cpp
- /Users/ryanrentfro/code/markamp/src/ui/EmptyPanelState.cpp

### P01-T158: Onboarding And Empty States Persistence Preferences And Serialization
**Task Title:** Onboarding And Empty States Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by onboarding and empty states.
**Description:** Identify what parts of give first-run canvas users enough guidance to succeed without burying experts in chrome. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** blank board hints; tool coachmarks; template-first creation.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OnboardingModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/FirstRunWizard.cpp
- /Users/ryanrentfro/code/markamp/src/ui/EmptyPanelState.cpp

### P01-T159: Onboarding And Empty States Safeguards Telemetry And Recovery
**Task Title:** Onboarding And Empty States Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for onboarding and empty states.
**Description:** Instrument give first-run canvas users enough guidance to succeed without burying experts in chrome. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** blank board hints; tool coachmarks; template-first creation.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OnboardingModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/FirstRunWizard.cpp
- /Users/ryanrentfro/code/markamp/src/ui/EmptyPanelState.cpp

### P01-T160: Onboarding And Empty States Tests Documentation And Rollout Gate
**Task Title:** Onboarding And Empty States Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship onboarding and empty states.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for give first-run canvas users enough guidance to succeed without burying experts in chrome.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** blank board hints; tool coachmarks; template-first creation.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OnboardingModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/FirstRunWizard.cpp
- /Users/ryanrentfro/code/markamp/src/ui/EmptyPanelState.cpp

## Workstream 17: Board Settings
Centralize board-level controls for size, background, permissions, and default behaviors.

### P01-T161: Board Settings UX Parity Audit
**Task Title:** Board Settings UX Parity Audit
**Definition:** Define the implementation contract for board settings by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around centralize board-level controls for size, background, permissions, and default behaviors.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** board background picker; grid defaults; authoring defaults per board.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigProfile.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasThemeModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardLockController.cpp

### P01-T162: Board Settings Domain Model And State Contract
**Task Title:** Board Settings Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support board settings.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around centralize board-level controls for size, background, permissions, and default behaviors.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** board background picker; grid defaults; authoring defaults per board.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigProfile.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasThemeModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardLockController.cpp

### P01-T163: Board Settings Commands Events And Context Keys
**Task Title:** Board Settings Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for board settings.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make centralize board-level controls for size, background, permissions, and default behaviors. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** board background picker; grid defaults; authoring defaults per board.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigProfile.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasThemeModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardLockController.cpp

### P01-T164: Board Settings Workspace Surface And Controls
**Task Title:** Board Settings Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose board settings in the main canvas workspace.
**Description:** Build the primary UI surfaces for centralize board-level controls for size, background, permissions, and default behaviors., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** board background picker; grid defaults; authoring defaults per board.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigProfile.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasThemeModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardLockController.cpp

### P01-T165: Board Settings Pointer Interaction Flow
**Task Title:** Board Settings Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for board settings.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for centralize board-level controls for size, background, permissions, and default behaviors.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** board background picker; grid defaults; authoring defaults per board.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigProfile.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasThemeModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardLockController.cpp

### P01-T166: Board Settings Keyboard Gesture And Shortcut Flow
**Task Title:** Board Settings Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for board settings.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for centralize board-level controls for size, background, permissions, and default behaviors.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** board background picker; grid defaults; authoring defaults per board.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigProfile.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasThemeModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardLockController.cpp

### P01-T167: Board Settings Visual Feedback And Rendering Polish
**Task Title:** Board Settings Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make board settings legible in use.
**Description:** Implement the visible feedback for centralize board-level controls for size, background, permissions, and default behaviors.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** board background picker; grid defaults; authoring defaults per board.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigProfile.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasThemeModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardLockController.cpp

### P01-T168: Board Settings Persistence Preferences And Serialization
**Task Title:** Board Settings Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by board settings.
**Description:** Identify what parts of centralize board-level controls for size, background, permissions, and default behaviors. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** board background picker; grid defaults; authoring defaults per board.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigProfile.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasThemeModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardLockController.cpp

### P01-T169: Board Settings Safeguards Telemetry And Recovery
**Task Title:** Board Settings Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for board settings.
**Description:** Instrument centralize board-level controls for size, background, permissions, and default behaviors. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** board background picker; grid defaults; authoring defaults per board.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigProfile.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasThemeModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardLockController.cpp

### P01-T170: Board Settings Tests Documentation And Rollout Gate
**Task Title:** Board Settings Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship board settings.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for centralize board-level controls for size, background, permissions, and default behaviors.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** board background picker; grid defaults; authoring defaults per board.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigProfile.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasThemeModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardLockController.cpp

## Workstream 18: Theme Tokens
Align the canvas shell and interaction layers with the broader theme system.

### P01-T171: Theme Tokens UX Parity Audit
**Task Title:** Theme Tokens UX Parity Audit
**Definition:** Define the implementation contract for theme tokens by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around align the canvas shell and interaction layers with the broader theme system.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** theme-aware selection color; board chrome tokens; high-contrast token fallbacks.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasColorToken.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasColorModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/BuiltInThemes.cpp

### P01-T172: Theme Tokens Domain Model And State Contract
**Task Title:** Theme Tokens Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support theme tokens.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around align the canvas shell and interaction layers with the broader theme system.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** theme-aware selection color; board chrome tokens; high-contrast token fallbacks.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasColorToken.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasColorModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/BuiltInThemes.cpp

### P01-T173: Theme Tokens Commands Events And Context Keys
**Task Title:** Theme Tokens Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for theme tokens.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make align the canvas shell and interaction layers with the broader theme system. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** theme-aware selection color; board chrome tokens; high-contrast token fallbacks.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasColorToken.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasColorModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/BuiltInThemes.cpp

### P01-T174: Theme Tokens Workspace Surface And Controls
**Task Title:** Theme Tokens Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose theme tokens in the main canvas workspace.
**Description:** Build the primary UI surfaces for align the canvas shell and interaction layers with the broader theme system., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** theme-aware selection color; board chrome tokens; high-contrast token fallbacks.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasColorToken.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasColorModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/BuiltInThemes.cpp

### P01-T175: Theme Tokens Pointer Interaction Flow
**Task Title:** Theme Tokens Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for theme tokens.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for align the canvas shell and interaction layers with the broader theme system.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** theme-aware selection color; board chrome tokens; high-contrast token fallbacks.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasColorToken.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasColorModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/BuiltInThemes.cpp

### P01-T176: Theme Tokens Keyboard Gesture And Shortcut Flow
**Task Title:** Theme Tokens Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for theme tokens.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for align the canvas shell and interaction layers with the broader theme system.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** theme-aware selection color; board chrome tokens; high-contrast token fallbacks.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasColorToken.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasColorModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/BuiltInThemes.cpp

### P01-T177: Theme Tokens Visual Feedback And Rendering Polish
**Task Title:** Theme Tokens Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make theme tokens legible in use.
**Description:** Implement the visible feedback for align the canvas shell and interaction layers with the broader theme system.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** theme-aware selection color; board chrome tokens; high-contrast token fallbacks.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasColorToken.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasColorModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/BuiltInThemes.cpp

### P01-T178: Theme Tokens Persistence Preferences And Serialization
**Task Title:** Theme Tokens Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by theme tokens.
**Description:** Identify what parts of align the canvas shell and interaction layers with the broader theme system. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** theme-aware selection color; board chrome tokens; high-contrast token fallbacks.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasColorToken.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasColorModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/BuiltInThemes.cpp

### P01-T179: Theme Tokens Safeguards Telemetry And Recovery
**Task Title:** Theme Tokens Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for theme tokens.
**Description:** Instrument align the canvas shell and interaction layers with the broader theme system. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** theme-aware selection color; board chrome tokens; high-contrast token fallbacks.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasColorToken.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasColorModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/BuiltInThemes.cpp

### P01-T180: Theme Tokens Tests Documentation And Rollout Gate
**Task Title:** Theme Tokens Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship theme tokens.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for align the canvas shell and interaction layers with the broader theme system.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** theme-aware selection color; board chrome tokens; high-contrast token fallbacks.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasColorToken.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasColorModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/BuiltInThemes.cpp

## Workstream 19: Command Palette Integration
Make canvas commands discoverable, searchable, and context-ranked.

### P01-T181: Command Palette Integration UX Parity Audit
**Task Title:** Command Palette Integration UX Parity Audit
**Definition:** Define the implementation contract for command palette integration by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around make canvas commands discoverable, searchable, and context-ranked.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** Canvas: New Board; Canvas: Toggle Grid; Canvas: Zoom To Selection.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CommandPaletteModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/Command.cpp

### P01-T182: Command Palette Integration Domain Model And State Contract
**Task Title:** Command Palette Integration Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support command palette integration.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around make canvas commands discoverable, searchable, and context-ranked.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** Canvas: New Board; Canvas: Toggle Grid; Canvas: Zoom To Selection.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CommandPaletteModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/Command.cpp

### P01-T183: Command Palette Integration Commands Events And Context Keys
**Task Title:** Command Palette Integration Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for command palette integration.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make make canvas commands discoverable, searchable, and context-ranked. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** Canvas: New Board; Canvas: Toggle Grid; Canvas: Zoom To Selection.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CommandPaletteModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/Command.cpp

### P01-T184: Command Palette Integration Workspace Surface And Controls
**Task Title:** Command Palette Integration Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose command palette integration in the main canvas workspace.
**Description:** Build the primary UI surfaces for make canvas commands discoverable, searchable, and context-ranked., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** Canvas: New Board; Canvas: Toggle Grid; Canvas: Zoom To Selection.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CommandPaletteModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/Command.cpp

### P01-T185: Command Palette Integration Pointer Interaction Flow
**Task Title:** Command Palette Integration Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for command palette integration.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for make canvas commands discoverable, searchable, and context-ranked.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** Canvas: New Board; Canvas: Toggle Grid; Canvas: Zoom To Selection.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CommandPaletteModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/Command.cpp

### P01-T186: Command Palette Integration Keyboard Gesture And Shortcut Flow
**Task Title:** Command Palette Integration Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for command palette integration.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for make canvas commands discoverable, searchable, and context-ranked.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** Canvas: New Board; Canvas: Toggle Grid; Canvas: Zoom To Selection.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CommandPaletteModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/Command.cpp

### P01-T187: Command Palette Integration Visual Feedback And Rendering Polish
**Task Title:** Command Palette Integration Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make command palette integration legible in use.
**Description:** Implement the visible feedback for make canvas commands discoverable, searchable, and context-ranked.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** Canvas: New Board; Canvas: Toggle Grid; Canvas: Zoom To Selection.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CommandPaletteModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/Command.cpp

### P01-T188: Command Palette Integration Persistence Preferences And Serialization
**Task Title:** Command Palette Integration Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by command palette integration.
**Description:** Identify what parts of make canvas commands discoverable, searchable, and context-ranked. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** Canvas: New Board; Canvas: Toggle Grid; Canvas: Zoom To Selection.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CommandPaletteModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/Command.cpp

### P01-T189: Command Palette Integration Safeguards Telemetry And Recovery
**Task Title:** Command Palette Integration Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for command palette integration.
**Description:** Instrument make canvas commands discoverable, searchable, and context-ranked. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** Canvas: New Board; Canvas: Toggle Grid; Canvas: Zoom To Selection.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CommandPaletteModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/Command.cpp

### P01-T190: Command Palette Integration Tests Documentation And Rollout Gate
**Task Title:** Command Palette Integration Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship command palette integration.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for make canvas commands discoverable, searchable, and context-ranked.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** Canvas: New Board; Canvas: Toggle Grid; Canvas: Zoom To Selection.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CommandPaletteModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/Command.cpp

## Workstream 20: Event And Analytics Contracts
Define the event vocabulary and instrumentation needed to evolve the canvas safely.

### P01-T191: Event And Analytics Contracts UX Parity Audit
**Task Title:** Event And Analytics Contracts UX Parity Audit
**Definition:** Define the implementation contract for event and analytics contracts by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around define the event vocabulary and instrumentation needed to evolve the canvas safely.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** tool-switch event; board-open event; interaction latency metric.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectLifecycleEvents.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/src/core/ActivityFeed.cpp

### P01-T192: Event And Analytics Contracts Domain Model And State Contract
**Task Title:** Event And Analytics Contracts Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support event and analytics contracts.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around define the event vocabulary and instrumentation needed to evolve the canvas safely.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** tool-switch event; board-open event; interaction latency metric.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectLifecycleEvents.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/src/core/ActivityFeed.cpp

### P01-T193: Event And Analytics Contracts Commands Events And Context Keys
**Task Title:** Event And Analytics Contracts Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for event and analytics contracts.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make define the event vocabulary and instrumentation needed to evolve the canvas safely. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** tool-switch event; board-open event; interaction latency metric.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectLifecycleEvents.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/src/core/ActivityFeed.cpp

### P01-T194: Event And Analytics Contracts Workspace Surface And Controls
**Task Title:** Event And Analytics Contracts Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose event and analytics contracts in the main canvas workspace.
**Description:** Build the primary UI surfaces for define the event vocabulary and instrumentation needed to evolve the canvas safely., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** tool-switch event; board-open event; interaction latency metric.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectLifecycleEvents.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/src/core/ActivityFeed.cpp

### P01-T195: Event And Analytics Contracts Pointer Interaction Flow
**Task Title:** Event And Analytics Contracts Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for event and analytics contracts.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for define the event vocabulary and instrumentation needed to evolve the canvas safely.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** tool-switch event; board-open event; interaction latency metric.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectLifecycleEvents.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/src/core/ActivityFeed.cpp

### P01-T196: Event And Analytics Contracts Keyboard Gesture And Shortcut Flow
**Task Title:** Event And Analytics Contracts Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for event and analytics contracts.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for define the event vocabulary and instrumentation needed to evolve the canvas safely.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** tool-switch event; board-open event; interaction latency metric.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectLifecycleEvents.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/src/core/ActivityFeed.cpp

### P01-T197: Event And Analytics Contracts Visual Feedback And Rendering Polish
**Task Title:** Event And Analytics Contracts Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make event and analytics contracts legible in use.
**Description:** Implement the visible feedback for define the event vocabulary and instrumentation needed to evolve the canvas safely.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** tool-switch event; board-open event; interaction latency metric.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectLifecycleEvents.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/src/core/ActivityFeed.cpp

### P01-T198: Event And Analytics Contracts Persistence Preferences And Serialization
**Task Title:** Event And Analytics Contracts Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by event and analytics contracts.
**Description:** Identify what parts of define the event vocabulary and instrumentation needed to evolve the canvas safely. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** tool-switch event; board-open event; interaction latency metric.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectLifecycleEvents.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/src/core/ActivityFeed.cpp

### P01-T199: Event And Analytics Contracts Safeguards Telemetry And Recovery
**Task Title:** Event And Analytics Contracts Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for event and analytics contracts.
**Description:** Instrument define the event vocabulary and instrumentation needed to evolve the canvas safely. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** tool-switch event; board-open event; interaction latency metric.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectLifecycleEvents.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/src/core/ActivityFeed.cpp

### P01-T200: Event And Analytics Contracts Tests Documentation And Rollout Gate
**Task Title:** Event And Analytics Contracts Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship event and analytics contracts.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for define the event vocabulary and instrumentation needed to evolve the canvas safely.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** tool-switch event; board-open event; interaction latency metric.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectLifecycleEvents.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/src/core/ActivityFeed.cpp

## Phase Exit Criteria
- All 200 tasks have either shipped, been explicitly deferred with rationale, or been converted into implementation issues with owners.
- Critical workflows in this phase have unit, integration, and at least one end-to-end validation path or a documented gap.
- The shipped work is theme-aware, accessibility-aware, serialization-safe, and undo/redo-safe by default.
- The phase produces measurable progress toward Miro-level editing, drawing, moving, and content authoring quality.
