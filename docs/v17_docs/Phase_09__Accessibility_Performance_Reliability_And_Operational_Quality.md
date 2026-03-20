# Phase 09: Accessibility Performance Reliability And Operational Quality

## Objective
Make the canvas durable, inclusive, observable, and performant enough to support large boards and diverse input methods without regressions.

## AI Agent Execution Rules
- Execute workstreams in order unless a later task is explicitly unblocked by completed model and command contracts.
- Prefer extending existing canvas, UI, and core services before introducing new parallel abstractions.
- Every implementation task should finish with tests or a documented testing gap.
- Preserve theme, accessibility, undo/redo, and serialization behavior as default quality bars rather than follow-up work.
- When adding references to new code during implementation, keep using full file paths in planning notes and PR write-ups.

## Workstreams
- W01 Keyboard Only Canvas: Ensure the core board can be navigated and edited without pointer input.
- W02 Screen Reader Semantics: Expose objects, state, and actions with useful spoken semantics.
- W03 Contrast And Color Blindness: Preserve legibility and meaning across a wider range of visual abilities.
- W04 Focus And Announcements: Make focus transitions and live region events understandable during active editing.
- W05 Touch And Pen Parity: Treat touch and stylus as first-class creation inputs, not degraded mouse emulation.
- W06 Performance Budgets: Set and enforce objective performance targets for common board operations.
- W07 Virtualization: Render only what matters on very large boards without breaking interaction fidelity.
- W08 Crash Recovery: Detect bad states early and help users recover without data loss.
- W09 Data Integrity: Continuously validate board state so corruption is caught before export or sync.
- W10 Accessibility Tooling: Provide tooling that makes regressions visible during development.
- W11 Safe Degradation: Design graceful fallbacks for slow machines, failed embeds, and broken plugins.
- W12 Observability: Instrument the canvas well enough to explain regressions and field issues.
- W13 Feature Flags: Roll out high-risk canvas upgrades safely across teams and builds.
- W14 Snapshot And Harnesses: Expand deterministic tooling for visual and behavioral regressions.
- W15 Unit And Integration Gaps: Close the most expensive correctness gaps before broad UX expansion.
- W16 End To End Determinism: Build stable top-level workflows that tolerate timing variance and platform differences.
- W17 Security And Privacy: Audit the canvas surface for unsafe ingest, embeds, and collaboration leaks.
- W18 Localization And IME: Support multilingual authoring, bidirectional text, and input method editors.
- W19 Compliance Checklists: Turn accessibility and reliability standards into repeatable release checks.
- W20 Operational Dashboards: Expose board health signals to engineering and support teams.

## Workstream 01: Keyboard Only Canvas
Ensure the core board can be navigated and edited without pointer input.

### P09-T001: Keyboard Only Canvas UX Parity Audit
**Task Title:** Keyboard Only Canvas UX Parity Audit
**Definition:** Define the implementation contract for keyboard only canvas by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around ensure the core board can be navigated and edited without pointer input.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** tab through objects; keyboard move and resize; open context actions from keyboard.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KeyboardCommandModel.h
- /Users/ryanrentfro/code/markamp/src/ui/FocusManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp

### P09-T002: Keyboard Only Canvas Domain Model And State Contract
**Task Title:** Keyboard Only Canvas Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support keyboard only canvas.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around ensure the core board can be navigated and edited without pointer input.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** tab through objects; keyboard move and resize; open context actions from keyboard.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KeyboardCommandModel.h
- /Users/ryanrentfro/code/markamp/src/ui/FocusManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp

### P09-T003: Keyboard Only Canvas Commands Events And Context Keys
**Task Title:** Keyboard Only Canvas Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for keyboard only canvas.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make ensure the core board can be navigated and edited without pointer input. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** tab through objects; keyboard move and resize; open context actions from keyboard.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KeyboardCommandModel.h
- /Users/ryanrentfro/code/markamp/src/ui/FocusManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp

### P09-T004: Keyboard Only Canvas Workspace Surface And Controls
**Task Title:** Keyboard Only Canvas Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose keyboard only canvas in the main canvas workspace.
**Description:** Build the primary UI surfaces for ensure the core board can be navigated and edited without pointer input., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** tab through objects; keyboard move and resize; open context actions from keyboard.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KeyboardCommandModel.h
- /Users/ryanrentfro/code/markamp/src/ui/FocusManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp

### P09-T005: Keyboard Only Canvas Pointer Interaction Flow
**Task Title:** Keyboard Only Canvas Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for keyboard only canvas.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for ensure the core board can be navigated and edited without pointer input.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** tab through objects; keyboard move and resize; open context actions from keyboard.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KeyboardCommandModel.h
- /Users/ryanrentfro/code/markamp/src/ui/FocusManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp

### P09-T006: Keyboard Only Canvas Keyboard Gesture And Shortcut Flow
**Task Title:** Keyboard Only Canvas Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for keyboard only canvas.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for ensure the core board can be navigated and edited without pointer input.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** tab through objects; keyboard move and resize; open context actions from keyboard.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KeyboardCommandModel.h
- /Users/ryanrentfro/code/markamp/src/ui/FocusManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp

### P09-T007: Keyboard Only Canvas Visual Feedback And Rendering Polish
**Task Title:** Keyboard Only Canvas Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make keyboard only canvas legible in use.
**Description:** Implement the visible feedback for ensure the core board can be navigated and edited without pointer input.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** tab through objects; keyboard move and resize; open context actions from keyboard.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KeyboardCommandModel.h
- /Users/ryanrentfro/code/markamp/src/ui/FocusManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp

### P09-T008: Keyboard Only Canvas Persistence Preferences And Serialization
**Task Title:** Keyboard Only Canvas Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by keyboard only canvas.
**Description:** Identify what parts of ensure the core board can be navigated and edited without pointer input. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** tab through objects; keyboard move and resize; open context actions from keyboard.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KeyboardCommandModel.h
- /Users/ryanrentfro/code/markamp/src/ui/FocusManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp

### P09-T009: Keyboard Only Canvas Safeguards Telemetry And Recovery
**Task Title:** Keyboard Only Canvas Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for keyboard only canvas.
**Description:** Instrument ensure the core board can be navigated and edited without pointer input. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** tab through objects; keyboard move and resize; open context actions from keyboard.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KeyboardCommandModel.h
- /Users/ryanrentfro/code/markamp/src/ui/FocusManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp

### P09-T010: Keyboard Only Canvas Tests Documentation And Rollout Gate
**Task Title:** Keyboard Only Canvas Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship keyboard only canvas.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for ensure the core board can be navigated and edited without pointer input.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** tab through objects; keyboard move and resize; open context actions from keyboard.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KeyboardCommandModel.h
- /Users/ryanrentfro/code/markamp/src/ui/FocusManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp

## Workstream 02: Screen Reader Semantics
Expose objects, state, and actions with useful spoken semantics.

### P09-T011: Screen Reader Semantics UX Parity Audit
**Task Title:** Screen Reader Semantics UX Parity Audit
**Definition:** Define the implementation contract for screen reader semantics by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around expose objects, state, and actions with useful spoken semantics.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** announce selected object; read comment count; describe board region.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AccessibilityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.h
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityAudit.cpp

### P09-T012: Screen Reader Semantics Domain Model And State Contract
**Task Title:** Screen Reader Semantics Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support screen reader semantics.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around expose objects, state, and actions with useful spoken semantics.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** announce selected object; read comment count; describe board region.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AccessibilityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.h
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityAudit.cpp

### P09-T013: Screen Reader Semantics Commands Events And Context Keys
**Task Title:** Screen Reader Semantics Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for screen reader semantics.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make expose objects, state, and actions with useful spoken semantics. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** announce selected object; read comment count; describe board region.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AccessibilityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.h
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityAudit.cpp

### P09-T014: Screen Reader Semantics Workspace Surface And Controls
**Task Title:** Screen Reader Semantics Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose screen reader semantics in the main canvas workspace.
**Description:** Build the primary UI surfaces for expose objects, state, and actions with useful spoken semantics., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** announce selected object; read comment count; describe board region.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AccessibilityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.h
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityAudit.cpp

### P09-T015: Screen Reader Semantics Pointer Interaction Flow
**Task Title:** Screen Reader Semantics Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for screen reader semantics.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for expose objects, state, and actions with useful spoken semantics.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** announce selected object; read comment count; describe board region.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AccessibilityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.h
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityAudit.cpp

### P09-T016: Screen Reader Semantics Keyboard Gesture And Shortcut Flow
**Task Title:** Screen Reader Semantics Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for screen reader semantics.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for expose objects, state, and actions with useful spoken semantics.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** announce selected object; read comment count; describe board region.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AccessibilityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.h
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityAudit.cpp

### P09-T017: Screen Reader Semantics Visual Feedback And Rendering Polish
**Task Title:** Screen Reader Semantics Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make screen reader semantics legible in use.
**Description:** Implement the visible feedback for expose objects, state, and actions with useful spoken semantics.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** announce selected object; read comment count; describe board region.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AccessibilityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.h
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityAudit.cpp

### P09-T018: Screen Reader Semantics Persistence Preferences And Serialization
**Task Title:** Screen Reader Semantics Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by screen reader semantics.
**Description:** Identify what parts of expose objects, state, and actions with useful spoken semantics. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** announce selected object; read comment count; describe board region.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AccessibilityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.h
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityAudit.cpp

### P09-T019: Screen Reader Semantics Safeguards Telemetry And Recovery
**Task Title:** Screen Reader Semantics Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for screen reader semantics.
**Description:** Instrument expose objects, state, and actions with useful spoken semantics. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** announce selected object; read comment count; describe board region.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AccessibilityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.h
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityAudit.cpp

### P09-T020: Screen Reader Semantics Tests Documentation And Rollout Gate
**Task Title:** Screen Reader Semantics Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship screen reader semantics.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for expose objects, state, and actions with useful spoken semantics.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** announce selected object; read comment count; describe board region.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AccessibilityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.h
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityAudit.cpp

## Workstream 03: Contrast And Color Blindness
Preserve legibility and meaning across a wider range of visual abilities.

### P09-T021: Contrast And Color Blindness UX Parity Audit
**Task Title:** Contrast And Color Blindness UX Parity Audit
**Definition:** Define the implementation contract for contrast and color blindness by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around preserve legibility and meaning across a wider range of visual abilities.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** contrast-safe selection; color blind preview; pattern fallback for status.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/ContrastRatioValidator.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ColorBlindnessPreviewOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/core/ColorBlindnessSupport.cpp

### P09-T022: Contrast And Color Blindness Domain Model And State Contract
**Task Title:** Contrast And Color Blindness Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support contrast and color blindness.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around preserve legibility and meaning across a wider range of visual abilities.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** contrast-safe selection; color blind preview; pattern fallback for status.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/ContrastRatioValidator.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ColorBlindnessPreviewOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/core/ColorBlindnessSupport.cpp

### P09-T023: Contrast And Color Blindness Commands Events And Context Keys
**Task Title:** Contrast And Color Blindness Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for contrast and color blindness.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make preserve legibility and meaning across a wider range of visual abilities. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** contrast-safe selection; color blind preview; pattern fallback for status.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/ContrastRatioValidator.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ColorBlindnessPreviewOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/core/ColorBlindnessSupport.cpp

### P09-T024: Contrast And Color Blindness Workspace Surface And Controls
**Task Title:** Contrast And Color Blindness Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose contrast and color blindness in the main canvas workspace.
**Description:** Build the primary UI surfaces for preserve legibility and meaning across a wider range of visual abilities., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** contrast-safe selection; color blind preview; pattern fallback for status.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/ContrastRatioValidator.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ColorBlindnessPreviewOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/core/ColorBlindnessSupport.cpp

### P09-T025: Contrast And Color Blindness Pointer Interaction Flow
**Task Title:** Contrast And Color Blindness Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for contrast and color blindness.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for preserve legibility and meaning across a wider range of visual abilities.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** contrast-safe selection; color blind preview; pattern fallback for status.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/ContrastRatioValidator.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ColorBlindnessPreviewOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/core/ColorBlindnessSupport.cpp

### P09-T026: Contrast And Color Blindness Keyboard Gesture And Shortcut Flow
**Task Title:** Contrast And Color Blindness Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for contrast and color blindness.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for preserve legibility and meaning across a wider range of visual abilities.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** contrast-safe selection; color blind preview; pattern fallback for status.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/ContrastRatioValidator.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ColorBlindnessPreviewOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/core/ColorBlindnessSupport.cpp

### P09-T027: Contrast And Color Blindness Visual Feedback And Rendering Polish
**Task Title:** Contrast And Color Blindness Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make contrast and color blindness legible in use.
**Description:** Implement the visible feedback for preserve legibility and meaning across a wider range of visual abilities.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** contrast-safe selection; color blind preview; pattern fallback for status.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/ContrastRatioValidator.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ColorBlindnessPreviewOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/core/ColorBlindnessSupport.cpp

### P09-T028: Contrast And Color Blindness Persistence Preferences And Serialization
**Task Title:** Contrast And Color Blindness Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by contrast and color blindness.
**Description:** Identify what parts of preserve legibility and meaning across a wider range of visual abilities. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** contrast-safe selection; color blind preview; pattern fallback for status.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/ContrastRatioValidator.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ColorBlindnessPreviewOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/core/ColorBlindnessSupport.cpp

### P09-T029: Contrast And Color Blindness Safeguards Telemetry And Recovery
**Task Title:** Contrast And Color Blindness Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for contrast and color blindness.
**Description:** Instrument preserve legibility and meaning across a wider range of visual abilities. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** contrast-safe selection; color blind preview; pattern fallback for status.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/ContrastRatioValidator.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ColorBlindnessPreviewOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/core/ColorBlindnessSupport.cpp

### P09-T030: Contrast And Color Blindness Tests Documentation And Rollout Gate
**Task Title:** Contrast And Color Blindness Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship contrast and color blindness.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for preserve legibility and meaning across a wider range of visual abilities.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** contrast-safe selection; color blind preview; pattern fallback for status.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/ContrastRatioValidator.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ColorBlindnessPreviewOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/core/ColorBlindnessSupport.cpp

## Workstream 04: Focus And Announcements
Make focus transitions and live region events understandable during active editing.

### P09-T031: Focus And Announcements UX Parity Audit
**Task Title:** Focus And Announcements UX Parity Audit
**Definition:** Define the implementation contract for focus and announcements by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around make focus transitions and live region events understandable during active editing.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** announce mode change; visible focus ring; read undo result.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/FocusRingRenderer.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.h
- /Users/ryanrentfro/code/markamp/src/ui/NotificationCenter.cpp

### P09-T032: Focus And Announcements Domain Model And State Contract
**Task Title:** Focus And Announcements Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support focus and announcements.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around make focus transitions and live region events understandable during active editing.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** announce mode change; visible focus ring; read undo result.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/FocusRingRenderer.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.h
- /Users/ryanrentfro/code/markamp/src/ui/NotificationCenter.cpp

### P09-T033: Focus And Announcements Commands Events And Context Keys
**Task Title:** Focus And Announcements Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for focus and announcements.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make make focus transitions and live region events understandable during active editing. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** announce mode change; visible focus ring; read undo result.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/FocusRingRenderer.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.h
- /Users/ryanrentfro/code/markamp/src/ui/NotificationCenter.cpp

### P09-T034: Focus And Announcements Workspace Surface And Controls
**Task Title:** Focus And Announcements Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose focus and announcements in the main canvas workspace.
**Description:** Build the primary UI surfaces for make focus transitions and live region events understandable during active editing., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** announce mode change; visible focus ring; read undo result.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/FocusRingRenderer.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.h
- /Users/ryanrentfro/code/markamp/src/ui/NotificationCenter.cpp

### P09-T035: Focus And Announcements Pointer Interaction Flow
**Task Title:** Focus And Announcements Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for focus and announcements.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for make focus transitions and live region events understandable during active editing.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** announce mode change; visible focus ring; read undo result.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/FocusRingRenderer.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.h
- /Users/ryanrentfro/code/markamp/src/ui/NotificationCenter.cpp

### P09-T036: Focus And Announcements Keyboard Gesture And Shortcut Flow
**Task Title:** Focus And Announcements Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for focus and announcements.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for make focus transitions and live region events understandable during active editing.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** announce mode change; visible focus ring; read undo result.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/FocusRingRenderer.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.h
- /Users/ryanrentfro/code/markamp/src/ui/NotificationCenter.cpp

### P09-T037: Focus And Announcements Visual Feedback And Rendering Polish
**Task Title:** Focus And Announcements Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make focus and announcements legible in use.
**Description:** Implement the visible feedback for make focus transitions and live region events understandable during active editing.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** announce mode change; visible focus ring; read undo result.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/FocusRingRenderer.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.h
- /Users/ryanrentfro/code/markamp/src/ui/NotificationCenter.cpp

### P09-T038: Focus And Announcements Persistence Preferences And Serialization
**Task Title:** Focus And Announcements Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by focus and announcements.
**Description:** Identify what parts of make focus transitions and live region events understandable during active editing. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** announce mode change; visible focus ring; read undo result.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/FocusRingRenderer.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.h
- /Users/ryanrentfro/code/markamp/src/ui/NotificationCenter.cpp

### P09-T039: Focus And Announcements Safeguards Telemetry And Recovery
**Task Title:** Focus And Announcements Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for focus and announcements.
**Description:** Instrument make focus transitions and live region events understandable during active editing. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** announce mode change; visible focus ring; read undo result.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/FocusRingRenderer.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.h
- /Users/ryanrentfro/code/markamp/src/ui/NotificationCenter.cpp

### P09-T040: Focus And Announcements Tests Documentation And Rollout Gate
**Task Title:** Focus And Announcements Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship focus and announcements.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for make focus transitions and live region events understandable during active editing.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** announce mode change; visible focus ring; read undo result.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/FocusRingRenderer.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.h
- /Users/ryanrentfro/code/markamp/src/ui/NotificationCenter.cpp

## Workstream 05: Touch And Pen Parity
Treat touch and stylus as first-class creation inputs, not degraded mouse emulation.

### P09-T041: Touch And Pen Parity UX Parity Audit
**Task Title:** Touch And Pen Parity UX Parity Audit
**Definition:** Define the implementation contract for touch and pen parity by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around treat touch and stylus as first-class creation inputs, not degraded mouse emulation.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** two-finger pan; pen button erase; touch selection handles.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PointerEventRouter.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DpiScalingController.h

### P09-T042: Touch And Pen Parity Domain Model And State Contract
**Task Title:** Touch And Pen Parity Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support touch and pen parity.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around treat touch and stylus as first-class creation inputs, not degraded mouse emulation.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** two-finger pan; pen button erase; touch selection handles.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PointerEventRouter.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DpiScalingController.h

### P09-T043: Touch And Pen Parity Commands Events And Context Keys
**Task Title:** Touch And Pen Parity Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for touch and pen parity.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make treat touch and stylus as first-class creation inputs, not degraded mouse emulation. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** two-finger pan; pen button erase; touch selection handles.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PointerEventRouter.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DpiScalingController.h

### P09-T044: Touch And Pen Parity Workspace Surface And Controls
**Task Title:** Touch And Pen Parity Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose touch and pen parity in the main canvas workspace.
**Description:** Build the primary UI surfaces for treat touch and stylus as first-class creation inputs, not degraded mouse emulation., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** two-finger pan; pen button erase; touch selection handles.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PointerEventRouter.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DpiScalingController.h

### P09-T045: Touch And Pen Parity Pointer Interaction Flow
**Task Title:** Touch And Pen Parity Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for touch and pen parity.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for treat touch and stylus as first-class creation inputs, not degraded mouse emulation.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** two-finger pan; pen button erase; touch selection handles.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PointerEventRouter.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DpiScalingController.h

### P09-T046: Touch And Pen Parity Keyboard Gesture And Shortcut Flow
**Task Title:** Touch And Pen Parity Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for touch and pen parity.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for treat touch and stylus as first-class creation inputs, not degraded mouse emulation.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** two-finger pan; pen button erase; touch selection handles.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PointerEventRouter.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DpiScalingController.h

### P09-T047: Touch And Pen Parity Visual Feedback And Rendering Polish
**Task Title:** Touch And Pen Parity Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make touch and pen parity legible in use.
**Description:** Implement the visible feedback for treat touch and stylus as first-class creation inputs, not degraded mouse emulation.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** two-finger pan; pen button erase; touch selection handles.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PointerEventRouter.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DpiScalingController.h

### P09-T048: Touch And Pen Parity Persistence Preferences And Serialization
**Task Title:** Touch And Pen Parity Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by touch and pen parity.
**Description:** Identify what parts of treat touch and stylus as first-class creation inputs, not degraded mouse emulation. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** two-finger pan; pen button erase; touch selection handles.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PointerEventRouter.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DpiScalingController.h

### P09-T049: Touch And Pen Parity Safeguards Telemetry And Recovery
**Task Title:** Touch And Pen Parity Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for touch and pen parity.
**Description:** Instrument treat touch and stylus as first-class creation inputs, not degraded mouse emulation. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** two-finger pan; pen button erase; touch selection handles.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PointerEventRouter.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DpiScalingController.h

### P09-T050: Touch And Pen Parity Tests Documentation And Rollout Gate
**Task Title:** Touch And Pen Parity Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship touch and pen parity.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for treat touch and stylus as first-class creation inputs, not degraded mouse emulation.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** two-finger pan; pen button erase; touch selection handles.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PointerEventRouter.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DpiScalingController.h

## Workstream 06: Performance Budgets
Set and enforce objective performance targets for common board operations.

### P09-T051: Performance Budgets UX Parity Audit
**Task Title:** Performance Budgets UX Parity Audit
**Definition:** Define the implementation contract for performance budgets by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around set and enforce objective performance targets for common board operations.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** 16ms frame budget; search latency budget; drag latency budget.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BenchmarkModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/EditorPerformanceBudget.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.h

### P09-T052: Performance Budgets Domain Model And State Contract
**Task Title:** Performance Budgets Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support performance budgets.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around set and enforce objective performance targets for common board operations.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** 16ms frame budget; search latency budget; drag latency budget.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BenchmarkModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/EditorPerformanceBudget.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.h

### P09-T053: Performance Budgets Commands Events And Context Keys
**Task Title:** Performance Budgets Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for performance budgets.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make set and enforce objective performance targets for common board operations. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** 16ms frame budget; search latency budget; drag latency budget.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BenchmarkModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/EditorPerformanceBudget.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.h

### P09-T054: Performance Budgets Workspace Surface And Controls
**Task Title:** Performance Budgets Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose performance budgets in the main canvas workspace.
**Description:** Build the primary UI surfaces for set and enforce objective performance targets for common board operations., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** 16ms frame budget; search latency budget; drag latency budget.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BenchmarkModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/EditorPerformanceBudget.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.h

### P09-T055: Performance Budgets Pointer Interaction Flow
**Task Title:** Performance Budgets Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for performance budgets.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for set and enforce objective performance targets for common board operations.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** 16ms frame budget; search latency budget; drag latency budget.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BenchmarkModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/EditorPerformanceBudget.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.h

### P09-T056: Performance Budgets Keyboard Gesture And Shortcut Flow
**Task Title:** Performance Budgets Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for performance budgets.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for set and enforce objective performance targets for common board operations.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** 16ms frame budget; search latency budget; drag latency budget.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BenchmarkModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/EditorPerformanceBudget.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.h

### P09-T057: Performance Budgets Visual Feedback And Rendering Polish
**Task Title:** Performance Budgets Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make performance budgets legible in use.
**Description:** Implement the visible feedback for set and enforce objective performance targets for common board operations.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** 16ms frame budget; search latency budget; drag latency budget.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BenchmarkModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/EditorPerformanceBudget.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.h

### P09-T058: Performance Budgets Persistence Preferences And Serialization
**Task Title:** Performance Budgets Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by performance budgets.
**Description:** Identify what parts of set and enforce objective performance targets for common board operations. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** 16ms frame budget; search latency budget; drag latency budget.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BenchmarkModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/EditorPerformanceBudget.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.h

### P09-T059: Performance Budgets Safeguards Telemetry And Recovery
**Task Title:** Performance Budgets Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for performance budgets.
**Description:** Instrument set and enforce objective performance targets for common board operations. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** 16ms frame budget; search latency budget; drag latency budget.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BenchmarkModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/EditorPerformanceBudget.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.h

### P09-T060: Performance Budgets Tests Documentation And Rollout Gate
**Task Title:** Performance Budgets Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship performance budgets.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for set and enforce objective performance targets for common board operations.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** 16ms frame budget; search latency budget; drag latency budget.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BenchmarkModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/EditorPerformanceBudget.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.h

## Workstream 07: Virtualization
Render only what matters on very large boards without breaking interaction fidelity.

### P09-T061: Virtualization UX Parity Audit
**Task Title:** Virtualization UX Parity Audit
**Definition:** Define the implementation contract for virtualization by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around render only what matters on very large boards without breaking interaction fidelity.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** offscreen culling; deferred panel updates; virtualized outline.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/Quadtree.h
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp

### P09-T062: Virtualization Domain Model And State Contract
**Task Title:** Virtualization Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support virtualization.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around render only what matters on very large boards without breaking interaction fidelity.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** offscreen culling; deferred panel updates; virtualized outline.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/Quadtree.h
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp

### P09-T063: Virtualization Commands Events And Context Keys
**Task Title:** Virtualization Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for virtualization.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make render only what matters on very large boards without breaking interaction fidelity. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** offscreen culling; deferred panel updates; virtualized outline.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/Quadtree.h
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp

### P09-T064: Virtualization Workspace Surface And Controls
**Task Title:** Virtualization Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose virtualization in the main canvas workspace.
**Description:** Build the primary UI surfaces for render only what matters on very large boards without breaking interaction fidelity., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** offscreen culling; deferred panel updates; virtualized outline.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/Quadtree.h
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp

### P09-T065: Virtualization Pointer Interaction Flow
**Task Title:** Virtualization Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for virtualization.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for render only what matters on very large boards without breaking interaction fidelity.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** offscreen culling; deferred panel updates; virtualized outline.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/Quadtree.h
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp

### P09-T066: Virtualization Keyboard Gesture And Shortcut Flow
**Task Title:** Virtualization Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for virtualization.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for render only what matters on very large boards without breaking interaction fidelity.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** offscreen culling; deferred panel updates; virtualized outline.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/Quadtree.h
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp

### P09-T067: Virtualization Visual Feedback And Rendering Polish
**Task Title:** Virtualization Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make virtualization legible in use.
**Description:** Implement the visible feedback for render only what matters on very large boards without breaking interaction fidelity.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** offscreen culling; deferred panel updates; virtualized outline.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/Quadtree.h
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp

### P09-T068: Virtualization Persistence Preferences And Serialization
**Task Title:** Virtualization Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by virtualization.
**Description:** Identify what parts of render only what matters on very large boards without breaking interaction fidelity. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** offscreen culling; deferred panel updates; virtualized outline.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/Quadtree.h
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp

### P09-T069: Virtualization Safeguards Telemetry And Recovery
**Task Title:** Virtualization Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for virtualization.
**Description:** Instrument render only what matters on very large boards without breaking interaction fidelity. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** offscreen culling; deferred panel updates; virtualized outline.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/Quadtree.h
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp

### P09-T070: Virtualization Tests Documentation And Rollout Gate
**Task Title:** Virtualization Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship virtualization.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for render only what matters on very large boards without breaking interaction fidelity.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** offscreen culling; deferred panel updates; virtualized outline.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/Quadtree.h
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp

## Workstream 08: Crash Recovery
Detect bad states early and help users recover without data loss.

### P09-T071: Crash Recovery UX Parity Audit
**Task Title:** Crash Recovery UX Parity Audit
**Definition:** Define the implementation contract for crash recovery by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around detect bad states early and help users recover without data loss.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** recover last autosave; disable crashing feature flag; surface corruption warning.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/CrashReporter.h
- /Users/ryanrentfro/code/markamp/src/core/SafeMode.h

### P09-T072: Crash Recovery Domain Model And State Contract
**Task Title:** Crash Recovery Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support crash recovery.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around detect bad states early and help users recover without data loss.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** recover last autosave; disable crashing feature flag; surface corruption warning.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/CrashReporter.h
- /Users/ryanrentfro/code/markamp/src/core/SafeMode.h

### P09-T073: Crash Recovery Commands Events And Context Keys
**Task Title:** Crash Recovery Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for crash recovery.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make detect bad states early and help users recover without data loss. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** recover last autosave; disable crashing feature flag; surface corruption warning.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/CrashReporter.h
- /Users/ryanrentfro/code/markamp/src/core/SafeMode.h

### P09-T074: Crash Recovery Workspace Surface And Controls
**Task Title:** Crash Recovery Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose crash recovery in the main canvas workspace.
**Description:** Build the primary UI surfaces for detect bad states early and help users recover without data loss., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** recover last autosave; disable crashing feature flag; surface corruption warning.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/CrashReporter.h
- /Users/ryanrentfro/code/markamp/src/core/SafeMode.h

### P09-T075: Crash Recovery Pointer Interaction Flow
**Task Title:** Crash Recovery Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for crash recovery.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for detect bad states early and help users recover without data loss.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** recover last autosave; disable crashing feature flag; surface corruption warning.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/CrashReporter.h
- /Users/ryanrentfro/code/markamp/src/core/SafeMode.h

### P09-T076: Crash Recovery Keyboard Gesture And Shortcut Flow
**Task Title:** Crash Recovery Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for crash recovery.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for detect bad states early and help users recover without data loss.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** recover last autosave; disable crashing feature flag; surface corruption warning.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/CrashReporter.h
- /Users/ryanrentfro/code/markamp/src/core/SafeMode.h

### P09-T077: Crash Recovery Visual Feedback And Rendering Polish
**Task Title:** Crash Recovery Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make crash recovery legible in use.
**Description:** Implement the visible feedback for detect bad states early and help users recover without data loss.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** recover last autosave; disable crashing feature flag; surface corruption warning.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/CrashReporter.h
- /Users/ryanrentfro/code/markamp/src/core/SafeMode.h

### P09-T078: Crash Recovery Persistence Preferences And Serialization
**Task Title:** Crash Recovery Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by crash recovery.
**Description:** Identify what parts of detect bad states early and help users recover without data loss. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** recover last autosave; disable crashing feature flag; surface corruption warning.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/CrashReporter.h
- /Users/ryanrentfro/code/markamp/src/core/SafeMode.h

### P09-T079: Crash Recovery Safeguards Telemetry And Recovery
**Task Title:** Crash Recovery Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for crash recovery.
**Description:** Instrument detect bad states early and help users recover without data loss. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** recover last autosave; disable crashing feature flag; surface corruption warning.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/CrashReporter.h
- /Users/ryanrentfro/code/markamp/src/core/SafeMode.h

### P09-T080: Crash Recovery Tests Documentation And Rollout Gate
**Task Title:** Crash Recovery Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship crash recovery.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for detect bad states early and help users recover without data loss.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** recover last autosave; disable crashing feature flag; surface corruption warning.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/CrashReporter.h
- /Users/ryanrentfro/code/markamp/src/core/SafeMode.h

## Workstream 09: Data Integrity
Continuously validate board state so corruption is caught before export or sync.

### P09-T081: Data Integrity UX Parity Audit
**Task Title:** Data Integrity UX Parity Audit
**Definition:** Define the implementation contract for data integrity by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around continuously validate board state so corruption is caught before export or sync.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** missing asset detection; orphaned connector check; invalid frame membership.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/AuditModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardSerializer.cpp

### P09-T082: Data Integrity Domain Model And State Contract
**Task Title:** Data Integrity Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support data integrity.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around continuously validate board state so corruption is caught before export or sync.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** missing asset detection; orphaned connector check; invalid frame membership.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/AuditModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardSerializer.cpp

### P09-T083: Data Integrity Commands Events And Context Keys
**Task Title:** Data Integrity Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for data integrity.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make continuously validate board state so corruption is caught before export or sync. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** missing asset detection; orphaned connector check; invalid frame membership.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/AuditModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardSerializer.cpp

### P09-T084: Data Integrity Workspace Surface And Controls
**Task Title:** Data Integrity Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose data integrity in the main canvas workspace.
**Description:** Build the primary UI surfaces for continuously validate board state so corruption is caught before export or sync., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** missing asset detection; orphaned connector check; invalid frame membership.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/AuditModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardSerializer.cpp

### P09-T085: Data Integrity Pointer Interaction Flow
**Task Title:** Data Integrity Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for data integrity.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for continuously validate board state so corruption is caught before export or sync.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** missing asset detection; orphaned connector check; invalid frame membership.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/AuditModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardSerializer.cpp

### P09-T086: Data Integrity Keyboard Gesture And Shortcut Flow
**Task Title:** Data Integrity Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for data integrity.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for continuously validate board state so corruption is caught before export or sync.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** missing asset detection; orphaned connector check; invalid frame membership.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/AuditModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardSerializer.cpp

### P09-T087: Data Integrity Visual Feedback And Rendering Polish
**Task Title:** Data Integrity Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make data integrity legible in use.
**Description:** Implement the visible feedback for continuously validate board state so corruption is caught before export or sync.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** missing asset detection; orphaned connector check; invalid frame membership.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/AuditModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardSerializer.cpp

### P09-T088: Data Integrity Persistence Preferences And Serialization
**Task Title:** Data Integrity Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by data integrity.
**Description:** Identify what parts of continuously validate board state so corruption is caught before export or sync. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** missing asset detection; orphaned connector check; invalid frame membership.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/AuditModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardSerializer.cpp

### P09-T089: Data Integrity Safeguards Telemetry And Recovery
**Task Title:** Data Integrity Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for data integrity.
**Description:** Instrument continuously validate board state so corruption is caught before export or sync. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** missing asset detection; orphaned connector check; invalid frame membership.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/AuditModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardSerializer.cpp

### P09-T090: Data Integrity Tests Documentation And Rollout Gate
**Task Title:** Data Integrity Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship data integrity.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for continuously validate board state so corruption is caught before export or sync.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** missing asset detection; orphaned connector check; invalid frame membership.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/AuditModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardSerializer.cpp

## Workstream 10: Accessibility Tooling
Provide tooling that makes regressions visible during development.

### P09-T091: Accessibility Tooling UX Parity Audit
**Task Title:** Accessibility Tooling UX Parity Audit
**Definition:** Define the implementation contract for accessibility tooling by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around provide tooling that makes regressions visible during development.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** automated semantics audit; contrast checker; keyboard traversal report.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityAudit.h
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityCommandProvider.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ControlSnapshotTester.cpp

### P09-T092: Accessibility Tooling Domain Model And State Contract
**Task Title:** Accessibility Tooling Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support accessibility tooling.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around provide tooling that makes regressions visible during development.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** automated semantics audit; contrast checker; keyboard traversal report.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityAudit.h
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityCommandProvider.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ControlSnapshotTester.cpp

### P09-T093: Accessibility Tooling Commands Events And Context Keys
**Task Title:** Accessibility Tooling Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for accessibility tooling.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make provide tooling that makes regressions visible during development. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** automated semantics audit; contrast checker; keyboard traversal report.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityAudit.h
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityCommandProvider.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ControlSnapshotTester.cpp

### P09-T094: Accessibility Tooling Workspace Surface And Controls
**Task Title:** Accessibility Tooling Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose accessibility tooling in the main canvas workspace.
**Description:** Build the primary UI surfaces for provide tooling that makes regressions visible during development., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** automated semantics audit; contrast checker; keyboard traversal report.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityAudit.h
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityCommandProvider.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ControlSnapshotTester.cpp

### P09-T095: Accessibility Tooling Pointer Interaction Flow
**Task Title:** Accessibility Tooling Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for accessibility tooling.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for provide tooling that makes regressions visible during development.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** automated semantics audit; contrast checker; keyboard traversal report.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityAudit.h
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityCommandProvider.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ControlSnapshotTester.cpp

### P09-T096: Accessibility Tooling Keyboard Gesture And Shortcut Flow
**Task Title:** Accessibility Tooling Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for accessibility tooling.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for provide tooling that makes regressions visible during development.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** automated semantics audit; contrast checker; keyboard traversal report.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityAudit.h
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityCommandProvider.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ControlSnapshotTester.cpp

### P09-T097: Accessibility Tooling Visual Feedback And Rendering Polish
**Task Title:** Accessibility Tooling Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make accessibility tooling legible in use.
**Description:** Implement the visible feedback for provide tooling that makes regressions visible during development.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** automated semantics audit; contrast checker; keyboard traversal report.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityAudit.h
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityCommandProvider.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ControlSnapshotTester.cpp

### P09-T098: Accessibility Tooling Persistence Preferences And Serialization
**Task Title:** Accessibility Tooling Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by accessibility tooling.
**Description:** Identify what parts of provide tooling that makes regressions visible during development. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** automated semantics audit; contrast checker; keyboard traversal report.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityAudit.h
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityCommandProvider.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ControlSnapshotTester.cpp

### P09-T099: Accessibility Tooling Safeguards Telemetry And Recovery
**Task Title:** Accessibility Tooling Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for accessibility tooling.
**Description:** Instrument provide tooling that makes regressions visible during development. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** automated semantics audit; contrast checker; keyboard traversal report.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityAudit.h
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityCommandProvider.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ControlSnapshotTester.cpp

### P09-T100: Accessibility Tooling Tests Documentation And Rollout Gate
**Task Title:** Accessibility Tooling Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship accessibility tooling.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for provide tooling that makes regressions visible during development.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** automated semantics audit; contrast checker; keyboard traversal report.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityAudit.h
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityCommandProvider.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ControlSnapshotTester.cpp

## Workstream 11: Safe Degradation
Design graceful fallbacks for slow machines, failed embeds, and broken plugins.

### P09-T101: Safe Degradation UX Parity Audit
**Task Title:** Safe Degradation UX Parity Audit
**Definition:** Define the implementation contract for safe degradation by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around design graceful fallbacks for slow machines, failed embeds, and broken plugins.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** fallback media placeholder; reduced motion mode; disable live shadows.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/AdaptiveThrottle.h
- /Users/ryanrentfro/code/markamp/src/ui/ControlVisibilityPrefs.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExtensionHostRecovery.cpp

### P09-T102: Safe Degradation Domain Model And State Contract
**Task Title:** Safe Degradation Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support safe degradation.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around design graceful fallbacks for slow machines, failed embeds, and broken plugins.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** fallback media placeholder; reduced motion mode; disable live shadows.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/AdaptiveThrottle.h
- /Users/ryanrentfro/code/markamp/src/ui/ControlVisibilityPrefs.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExtensionHostRecovery.cpp

### P09-T103: Safe Degradation Commands Events And Context Keys
**Task Title:** Safe Degradation Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for safe degradation.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make design graceful fallbacks for slow machines, failed embeds, and broken plugins. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** fallback media placeholder; reduced motion mode; disable live shadows.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/AdaptiveThrottle.h
- /Users/ryanrentfro/code/markamp/src/ui/ControlVisibilityPrefs.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExtensionHostRecovery.cpp

### P09-T104: Safe Degradation Workspace Surface And Controls
**Task Title:** Safe Degradation Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose safe degradation in the main canvas workspace.
**Description:** Build the primary UI surfaces for design graceful fallbacks for slow machines, failed embeds, and broken plugins., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** fallback media placeholder; reduced motion mode; disable live shadows.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/AdaptiveThrottle.h
- /Users/ryanrentfro/code/markamp/src/ui/ControlVisibilityPrefs.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExtensionHostRecovery.cpp

### P09-T105: Safe Degradation Pointer Interaction Flow
**Task Title:** Safe Degradation Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for safe degradation.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for design graceful fallbacks for slow machines, failed embeds, and broken plugins.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** fallback media placeholder; reduced motion mode; disable live shadows.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/AdaptiveThrottle.h
- /Users/ryanrentfro/code/markamp/src/ui/ControlVisibilityPrefs.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExtensionHostRecovery.cpp

### P09-T106: Safe Degradation Keyboard Gesture And Shortcut Flow
**Task Title:** Safe Degradation Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for safe degradation.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for design graceful fallbacks for slow machines, failed embeds, and broken plugins.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** fallback media placeholder; reduced motion mode; disable live shadows.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/AdaptiveThrottle.h
- /Users/ryanrentfro/code/markamp/src/ui/ControlVisibilityPrefs.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExtensionHostRecovery.cpp

### P09-T107: Safe Degradation Visual Feedback And Rendering Polish
**Task Title:** Safe Degradation Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make safe degradation legible in use.
**Description:** Implement the visible feedback for design graceful fallbacks for slow machines, failed embeds, and broken plugins.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** fallback media placeholder; reduced motion mode; disable live shadows.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/AdaptiveThrottle.h
- /Users/ryanrentfro/code/markamp/src/ui/ControlVisibilityPrefs.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExtensionHostRecovery.cpp

### P09-T108: Safe Degradation Persistence Preferences And Serialization
**Task Title:** Safe Degradation Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by safe degradation.
**Description:** Identify what parts of design graceful fallbacks for slow machines, failed embeds, and broken plugins. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** fallback media placeholder; reduced motion mode; disable live shadows.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/AdaptiveThrottle.h
- /Users/ryanrentfro/code/markamp/src/ui/ControlVisibilityPrefs.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExtensionHostRecovery.cpp

### P09-T109: Safe Degradation Safeguards Telemetry And Recovery
**Task Title:** Safe Degradation Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for safe degradation.
**Description:** Instrument design graceful fallbacks for slow machines, failed embeds, and broken plugins. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** fallback media placeholder; reduced motion mode; disable live shadows.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/AdaptiveThrottle.h
- /Users/ryanrentfro/code/markamp/src/ui/ControlVisibilityPrefs.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExtensionHostRecovery.cpp

### P09-T110: Safe Degradation Tests Documentation And Rollout Gate
**Task Title:** Safe Degradation Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship safe degradation.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for design graceful fallbacks for slow machines, failed embeds, and broken plugins.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** fallback media placeholder; reduced motion mode; disable live shadows.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/AdaptiveThrottle.h
- /Users/ryanrentfro/code/markamp/src/ui/ControlVisibilityPrefs.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExtensionHostRecovery.cpp

## Workstream 12: Observability
Instrument the canvas well enough to explain regressions and field issues.

### P09-T111: Observability UX Parity Audit
**Task Title:** Observability UX Parity Audit
**Definition:** Define the implementation contract for observability by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around instrument the canvas well enough to explain regressions and field issues.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** render timing log; tool failure metric; autosave success event.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/src/core/BuildLogManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AuditModel.h

### P09-T112: Observability Domain Model And State Contract
**Task Title:** Observability Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support observability.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around instrument the canvas well enough to explain regressions and field issues.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** render timing log; tool failure metric; autosave success event.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/src/core/BuildLogManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AuditModel.h

### P09-T113: Observability Commands Events And Context Keys
**Task Title:** Observability Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for observability.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make instrument the canvas well enough to explain regressions and field issues. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** render timing log; tool failure metric; autosave success event.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/src/core/BuildLogManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AuditModel.h

### P09-T114: Observability Workspace Surface And Controls
**Task Title:** Observability Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose observability in the main canvas workspace.
**Description:** Build the primary UI surfaces for instrument the canvas well enough to explain regressions and field issues., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** render timing log; tool failure metric; autosave success event.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/src/core/BuildLogManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AuditModel.h

### P09-T115: Observability Pointer Interaction Flow
**Task Title:** Observability Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for observability.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for instrument the canvas well enough to explain regressions and field issues.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** render timing log; tool failure metric; autosave success event.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/src/core/BuildLogManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AuditModel.h

### P09-T116: Observability Keyboard Gesture And Shortcut Flow
**Task Title:** Observability Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for observability.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for instrument the canvas well enough to explain regressions and field issues.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** render timing log; tool failure metric; autosave success event.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/src/core/BuildLogManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AuditModel.h

### P09-T117: Observability Visual Feedback And Rendering Polish
**Task Title:** Observability Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make observability legible in use.
**Description:** Implement the visible feedback for instrument the canvas well enough to explain regressions and field issues.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** render timing log; tool failure metric; autosave success event.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/src/core/BuildLogManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AuditModel.h

### P09-T118: Observability Persistence Preferences And Serialization
**Task Title:** Observability Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by observability.
**Description:** Identify what parts of instrument the canvas well enough to explain regressions and field issues. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** render timing log; tool failure metric; autosave success event.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/src/core/BuildLogManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AuditModel.h

### P09-T119: Observability Safeguards Telemetry And Recovery
**Task Title:** Observability Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for observability.
**Description:** Instrument instrument the canvas well enough to explain regressions and field issues. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** render timing log; tool failure metric; autosave success event.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/src/core/BuildLogManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AuditModel.h

### P09-T120: Observability Tests Documentation And Rollout Gate
**Task Title:** Observability Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship observability.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for instrument the canvas well enough to explain regressions and field issues.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** render timing log; tool failure metric; autosave success event.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/src/core/BuildLogManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AuditModel.h

## Workstream 13: Feature Flags
Roll out high-risk canvas upgrades safely across teams and builds.

### P09-T121: Feature Flags UX Parity Audit
**Task Title:** Feature Flags UX Parity Audit
**Definition:** Define the implementation contract for feature flags by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around roll out high-risk canvas upgrades safely across teams and builds.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** flag new connector routing; beta-only pen engine; kill switch for embed preview.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/FeatureRegistry.cpp
- /Users/ryanrentfro/code/markamp/src/core/FeatureToggledEvent.h
- /Users/ryanrentfro/code/markamp/src/core/ContextKeyService.h

### P09-T122: Feature Flags Domain Model And State Contract
**Task Title:** Feature Flags Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support feature flags.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around roll out high-risk canvas upgrades safely across teams and builds.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** flag new connector routing; beta-only pen engine; kill switch for embed preview.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/FeatureRegistry.cpp
- /Users/ryanrentfro/code/markamp/src/core/FeatureToggledEvent.h
- /Users/ryanrentfro/code/markamp/src/core/ContextKeyService.h

### P09-T123: Feature Flags Commands Events And Context Keys
**Task Title:** Feature Flags Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for feature flags.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make roll out high-risk canvas upgrades safely across teams and builds. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** flag new connector routing; beta-only pen engine; kill switch for embed preview.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/FeatureRegistry.cpp
- /Users/ryanrentfro/code/markamp/src/core/FeatureToggledEvent.h
- /Users/ryanrentfro/code/markamp/src/core/ContextKeyService.h

### P09-T124: Feature Flags Workspace Surface And Controls
**Task Title:** Feature Flags Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose feature flags in the main canvas workspace.
**Description:** Build the primary UI surfaces for roll out high-risk canvas upgrades safely across teams and builds., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** flag new connector routing; beta-only pen engine; kill switch for embed preview.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/FeatureRegistry.cpp
- /Users/ryanrentfro/code/markamp/src/core/FeatureToggledEvent.h
- /Users/ryanrentfro/code/markamp/src/core/ContextKeyService.h

### P09-T125: Feature Flags Pointer Interaction Flow
**Task Title:** Feature Flags Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for feature flags.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for roll out high-risk canvas upgrades safely across teams and builds.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** flag new connector routing; beta-only pen engine; kill switch for embed preview.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/FeatureRegistry.cpp
- /Users/ryanrentfro/code/markamp/src/core/FeatureToggledEvent.h
- /Users/ryanrentfro/code/markamp/src/core/ContextKeyService.h

### P09-T126: Feature Flags Keyboard Gesture And Shortcut Flow
**Task Title:** Feature Flags Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for feature flags.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for roll out high-risk canvas upgrades safely across teams and builds.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** flag new connector routing; beta-only pen engine; kill switch for embed preview.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/FeatureRegistry.cpp
- /Users/ryanrentfro/code/markamp/src/core/FeatureToggledEvent.h
- /Users/ryanrentfro/code/markamp/src/core/ContextKeyService.h

### P09-T127: Feature Flags Visual Feedback And Rendering Polish
**Task Title:** Feature Flags Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make feature flags legible in use.
**Description:** Implement the visible feedback for roll out high-risk canvas upgrades safely across teams and builds.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** flag new connector routing; beta-only pen engine; kill switch for embed preview.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/FeatureRegistry.cpp
- /Users/ryanrentfro/code/markamp/src/core/FeatureToggledEvent.h
- /Users/ryanrentfro/code/markamp/src/core/ContextKeyService.h

### P09-T128: Feature Flags Persistence Preferences And Serialization
**Task Title:** Feature Flags Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by feature flags.
**Description:** Identify what parts of roll out high-risk canvas upgrades safely across teams and builds. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** flag new connector routing; beta-only pen engine; kill switch for embed preview.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/FeatureRegistry.cpp
- /Users/ryanrentfro/code/markamp/src/core/FeatureToggledEvent.h
- /Users/ryanrentfro/code/markamp/src/core/ContextKeyService.h

### P09-T129: Feature Flags Safeguards Telemetry And Recovery
**Task Title:** Feature Flags Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for feature flags.
**Description:** Instrument roll out high-risk canvas upgrades safely across teams and builds. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** flag new connector routing; beta-only pen engine; kill switch for embed preview.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/FeatureRegistry.cpp
- /Users/ryanrentfro/code/markamp/src/core/FeatureToggledEvent.h
- /Users/ryanrentfro/code/markamp/src/core/ContextKeyService.h

### P09-T130: Feature Flags Tests Documentation And Rollout Gate
**Task Title:** Feature Flags Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship feature flags.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for roll out high-risk canvas upgrades safely across teams and builds.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** flag new connector routing; beta-only pen engine; kill switch for embed preview.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/FeatureRegistry.cpp
- /Users/ryanrentfro/code/markamp/src/core/FeatureToggledEvent.h
- /Users/ryanrentfro/code/markamp/src/core/ContextKeyService.h

## Workstream 14: Snapshot And Harnesses
Expand deterministic tooling for visual and behavioral regressions.

### P09-T131: Snapshot And Harnesses UX Parity Audit
**Task Title:** Snapshot And Harnesses UX Parity Audit
**Definition:** Define the implementation contract for snapshot and harnesses by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around expand deterministic tooling for visual and behavioral regressions.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** canvas snapshot test; serialized board fixture; input replay harness.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ControlSnapshotTester.h
- /Users/ryanrentfro/code/markamp/src/canvas/BoardSerializerModel.h
- /Users/ryanrentfro/code/markamp/src/core/CrossModuleTestSuite.cpp

### P09-T132: Snapshot And Harnesses Domain Model And State Contract
**Task Title:** Snapshot And Harnesses Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support snapshot and harnesses.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around expand deterministic tooling for visual and behavioral regressions.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** canvas snapshot test; serialized board fixture; input replay harness.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ControlSnapshotTester.h
- /Users/ryanrentfro/code/markamp/src/canvas/BoardSerializerModel.h
- /Users/ryanrentfro/code/markamp/src/core/CrossModuleTestSuite.cpp

### P09-T133: Snapshot And Harnesses Commands Events And Context Keys
**Task Title:** Snapshot And Harnesses Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for snapshot and harnesses.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make expand deterministic tooling for visual and behavioral regressions. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** canvas snapshot test; serialized board fixture; input replay harness.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ControlSnapshotTester.h
- /Users/ryanrentfro/code/markamp/src/canvas/BoardSerializerModel.h
- /Users/ryanrentfro/code/markamp/src/core/CrossModuleTestSuite.cpp

### P09-T134: Snapshot And Harnesses Workspace Surface And Controls
**Task Title:** Snapshot And Harnesses Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose snapshot and harnesses in the main canvas workspace.
**Description:** Build the primary UI surfaces for expand deterministic tooling for visual and behavioral regressions., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** canvas snapshot test; serialized board fixture; input replay harness.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ControlSnapshotTester.h
- /Users/ryanrentfro/code/markamp/src/canvas/BoardSerializerModel.h
- /Users/ryanrentfro/code/markamp/src/core/CrossModuleTestSuite.cpp

### P09-T135: Snapshot And Harnesses Pointer Interaction Flow
**Task Title:** Snapshot And Harnesses Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for snapshot and harnesses.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for expand deterministic tooling for visual and behavioral regressions.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** canvas snapshot test; serialized board fixture; input replay harness.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ControlSnapshotTester.h
- /Users/ryanrentfro/code/markamp/src/canvas/BoardSerializerModel.h
- /Users/ryanrentfro/code/markamp/src/core/CrossModuleTestSuite.cpp

### P09-T136: Snapshot And Harnesses Keyboard Gesture And Shortcut Flow
**Task Title:** Snapshot And Harnesses Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for snapshot and harnesses.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for expand deterministic tooling for visual and behavioral regressions.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** canvas snapshot test; serialized board fixture; input replay harness.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ControlSnapshotTester.h
- /Users/ryanrentfro/code/markamp/src/canvas/BoardSerializerModel.h
- /Users/ryanrentfro/code/markamp/src/core/CrossModuleTestSuite.cpp

### P09-T137: Snapshot And Harnesses Visual Feedback And Rendering Polish
**Task Title:** Snapshot And Harnesses Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make snapshot and harnesses legible in use.
**Description:** Implement the visible feedback for expand deterministic tooling for visual and behavioral regressions.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** canvas snapshot test; serialized board fixture; input replay harness.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ControlSnapshotTester.h
- /Users/ryanrentfro/code/markamp/src/canvas/BoardSerializerModel.h
- /Users/ryanrentfro/code/markamp/src/core/CrossModuleTestSuite.cpp

### P09-T138: Snapshot And Harnesses Persistence Preferences And Serialization
**Task Title:** Snapshot And Harnesses Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by snapshot and harnesses.
**Description:** Identify what parts of expand deterministic tooling for visual and behavioral regressions. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** canvas snapshot test; serialized board fixture; input replay harness.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ControlSnapshotTester.h
- /Users/ryanrentfro/code/markamp/src/canvas/BoardSerializerModel.h
- /Users/ryanrentfro/code/markamp/src/core/CrossModuleTestSuite.cpp

### P09-T139: Snapshot And Harnesses Safeguards Telemetry And Recovery
**Task Title:** Snapshot And Harnesses Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for snapshot and harnesses.
**Description:** Instrument expand deterministic tooling for visual and behavioral regressions. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** canvas snapshot test; serialized board fixture; input replay harness.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ControlSnapshotTester.h
- /Users/ryanrentfro/code/markamp/src/canvas/BoardSerializerModel.h
- /Users/ryanrentfro/code/markamp/src/core/CrossModuleTestSuite.cpp

### P09-T140: Snapshot And Harnesses Tests Documentation And Rollout Gate
**Task Title:** Snapshot And Harnesses Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship snapshot and harnesses.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for expand deterministic tooling for visual and behavioral regressions.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** canvas snapshot test; serialized board fixture; input replay harness.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ControlSnapshotTester.h
- /Users/ryanrentfro/code/markamp/src/canvas/BoardSerializerModel.h
- /Users/ryanrentfro/code/markamp/src/core/CrossModuleTestSuite.cpp

## Workstream 15: Unit And Integration Gaps
Close the most expensive correctness gaps before broad UX expansion.

### P09-T141: Unit And Integration Gaps UX Parity Audit
**Task Title:** Unit And Integration Gaps UX Parity Audit
**Definition:** Define the implementation contract for unit and integration gaps by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around close the most expensive correctness gaps before broad ux expansion.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** selection edge-case test; undo history integration test; board lifecycle contract test.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/tests/unit
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasCommands.cpp
- /Users/ryanrentfro/code/markamp/src/core/CrossModuleTestSuite.h

### P09-T142: Unit And Integration Gaps Domain Model And State Contract
**Task Title:** Unit And Integration Gaps Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support unit and integration gaps.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around close the most expensive correctness gaps before broad ux expansion.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** selection edge-case test; undo history integration test; board lifecycle contract test.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/tests/unit
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasCommands.cpp
- /Users/ryanrentfro/code/markamp/src/core/CrossModuleTestSuite.h

### P09-T143: Unit And Integration Gaps Commands Events And Context Keys
**Task Title:** Unit And Integration Gaps Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for unit and integration gaps.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make close the most expensive correctness gaps before broad ux expansion. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** selection edge-case test; undo history integration test; board lifecycle contract test.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/tests/unit
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasCommands.cpp
- /Users/ryanrentfro/code/markamp/src/core/CrossModuleTestSuite.h

### P09-T144: Unit And Integration Gaps Workspace Surface And Controls
**Task Title:** Unit And Integration Gaps Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose unit and integration gaps in the main canvas workspace.
**Description:** Build the primary UI surfaces for close the most expensive correctness gaps before broad ux expansion., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** selection edge-case test; undo history integration test; board lifecycle contract test.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/tests/unit
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasCommands.cpp
- /Users/ryanrentfro/code/markamp/src/core/CrossModuleTestSuite.h

### P09-T145: Unit And Integration Gaps Pointer Interaction Flow
**Task Title:** Unit And Integration Gaps Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for unit and integration gaps.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for close the most expensive correctness gaps before broad ux expansion.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** selection edge-case test; undo history integration test; board lifecycle contract test.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/tests/unit
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasCommands.cpp
- /Users/ryanrentfro/code/markamp/src/core/CrossModuleTestSuite.h

### P09-T146: Unit And Integration Gaps Keyboard Gesture And Shortcut Flow
**Task Title:** Unit And Integration Gaps Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for unit and integration gaps.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for close the most expensive correctness gaps before broad ux expansion.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** selection edge-case test; undo history integration test; board lifecycle contract test.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/tests/unit
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasCommands.cpp
- /Users/ryanrentfro/code/markamp/src/core/CrossModuleTestSuite.h

### P09-T147: Unit And Integration Gaps Visual Feedback And Rendering Polish
**Task Title:** Unit And Integration Gaps Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make unit and integration gaps legible in use.
**Description:** Implement the visible feedback for close the most expensive correctness gaps before broad ux expansion.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** selection edge-case test; undo history integration test; board lifecycle contract test.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/tests/unit
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasCommands.cpp
- /Users/ryanrentfro/code/markamp/src/core/CrossModuleTestSuite.h

### P09-T148: Unit And Integration Gaps Persistence Preferences And Serialization
**Task Title:** Unit And Integration Gaps Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by unit and integration gaps.
**Description:** Identify what parts of close the most expensive correctness gaps before broad ux expansion. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** selection edge-case test; undo history integration test; board lifecycle contract test.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/tests/unit
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasCommands.cpp
- /Users/ryanrentfro/code/markamp/src/core/CrossModuleTestSuite.h

### P09-T149: Unit And Integration Gaps Safeguards Telemetry And Recovery
**Task Title:** Unit And Integration Gaps Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for unit and integration gaps.
**Description:** Instrument close the most expensive correctness gaps before broad ux expansion. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** selection edge-case test; undo history integration test; board lifecycle contract test.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/tests/unit
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasCommands.cpp
- /Users/ryanrentfro/code/markamp/src/core/CrossModuleTestSuite.h

### P09-T150: Unit And Integration Gaps Tests Documentation And Rollout Gate
**Task Title:** Unit And Integration Gaps Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship unit and integration gaps.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for close the most expensive correctness gaps before broad ux expansion.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** selection edge-case test; undo history integration test; board lifecycle contract test.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/tests/unit
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasCommands.cpp
- /Users/ryanrentfro/code/markamp/src/core/CrossModuleTestSuite.h

## Workstream 16: End To End Determinism
Build stable top-level workflows that tolerate timing variance and platform differences.

### P09-T151: End To End Determinism UX Parity Audit
**Task Title:** End To End Determinism UX Parity Audit
**Definition:** Define the implementation contract for end to end determinism by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around build stable top-level workflows that tolerate timing variance and platform differences.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** new board smoke; draw and export smoke; collaboration reconnect smoke.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BenchmarkModel.h

### P09-T152: End To End Determinism Domain Model And State Contract
**Task Title:** End To End Determinism Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support end to end determinism.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around build stable top-level workflows that tolerate timing variance and platform differences.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** new board smoke; draw and export smoke; collaboration reconnect smoke.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BenchmarkModel.h

### P09-T153: End To End Determinism Commands Events And Context Keys
**Task Title:** End To End Determinism Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for end to end determinism.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make build stable top-level workflows that tolerate timing variance and platform differences. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** new board smoke; draw and export smoke; collaboration reconnect smoke.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BenchmarkModel.h

### P09-T154: End To End Determinism Workspace Surface And Controls
**Task Title:** End To End Determinism Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose end to end determinism in the main canvas workspace.
**Description:** Build the primary UI surfaces for build stable top-level workflows that tolerate timing variance and platform differences., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** new board smoke; draw and export smoke; collaboration reconnect smoke.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BenchmarkModel.h

### P09-T155: End To End Determinism Pointer Interaction Flow
**Task Title:** End To End Determinism Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for end to end determinism.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for build stable top-level workflows that tolerate timing variance and platform differences.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** new board smoke; draw and export smoke; collaboration reconnect smoke.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BenchmarkModel.h

### P09-T156: End To End Determinism Keyboard Gesture And Shortcut Flow
**Task Title:** End To End Determinism Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for end to end determinism.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for build stable top-level workflows that tolerate timing variance and platform differences.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** new board smoke; draw and export smoke; collaboration reconnect smoke.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BenchmarkModel.h

### P09-T157: End To End Determinism Visual Feedback And Rendering Polish
**Task Title:** End To End Determinism Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make end to end determinism legible in use.
**Description:** Implement the visible feedback for build stable top-level workflows that tolerate timing variance and platform differences.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** new board smoke; draw and export smoke; collaboration reconnect smoke.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BenchmarkModel.h

### P09-T158: End To End Determinism Persistence Preferences And Serialization
**Task Title:** End To End Determinism Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by end to end determinism.
**Description:** Identify what parts of build stable top-level workflows that tolerate timing variance and platform differences. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** new board smoke; draw and export smoke; collaboration reconnect smoke.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BenchmarkModel.h

### P09-T159: End To End Determinism Safeguards Telemetry And Recovery
**Task Title:** End To End Determinism Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for end to end determinism.
**Description:** Instrument build stable top-level workflows that tolerate timing variance and platform differences. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** new board smoke; draw and export smoke; collaboration reconnect smoke.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BenchmarkModel.h

### P09-T160: End To End Determinism Tests Documentation And Rollout Gate
**Task Title:** End To End Determinism Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship end to end determinism.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for build stable top-level workflows that tolerate timing variance and platform differences.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** new board smoke; draw and export smoke; collaboration reconnect smoke.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BenchmarkModel.h

## Workstream 17: Security And Privacy
Audit the canvas surface for unsafe ingest, embeds, and collaboration leaks.

### P09-T161: Security And Privacy UX Parity Audit
**Task Title:** Security And Privacy UX Parity Audit
**Definition:** Define the implementation contract for security and privacy by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around audit the canvas surface for unsafe ingest, embeds, and collaboration leaks.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** sanitize pasted HTML; redact private metadata; restrict dangerous URLs.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/ClipboardSanitizer.h
- /Users/ryanrentfro/code/markamp/src/core/DataRedactionEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/ContentSecurityPolicy.h

### P09-T162: Security And Privacy Domain Model And State Contract
**Task Title:** Security And Privacy Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support security and privacy.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around audit the canvas surface for unsafe ingest, embeds, and collaboration leaks.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** sanitize pasted HTML; redact private metadata; restrict dangerous URLs.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/ClipboardSanitizer.h
- /Users/ryanrentfro/code/markamp/src/core/DataRedactionEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/ContentSecurityPolicy.h

### P09-T163: Security And Privacy Commands Events And Context Keys
**Task Title:** Security And Privacy Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for security and privacy.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make audit the canvas surface for unsafe ingest, embeds, and collaboration leaks. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** sanitize pasted HTML; redact private metadata; restrict dangerous URLs.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/ClipboardSanitizer.h
- /Users/ryanrentfro/code/markamp/src/core/DataRedactionEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/ContentSecurityPolicy.h

### P09-T164: Security And Privacy Workspace Surface And Controls
**Task Title:** Security And Privacy Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose security and privacy in the main canvas workspace.
**Description:** Build the primary UI surfaces for audit the canvas surface for unsafe ingest, embeds, and collaboration leaks., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** sanitize pasted HTML; redact private metadata; restrict dangerous URLs.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/ClipboardSanitizer.h
- /Users/ryanrentfro/code/markamp/src/core/DataRedactionEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/ContentSecurityPolicy.h

### P09-T165: Security And Privacy Pointer Interaction Flow
**Task Title:** Security And Privacy Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for security and privacy.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for audit the canvas surface for unsafe ingest, embeds, and collaboration leaks.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** sanitize pasted HTML; redact private metadata; restrict dangerous URLs.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/ClipboardSanitizer.h
- /Users/ryanrentfro/code/markamp/src/core/DataRedactionEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/ContentSecurityPolicy.h

### P09-T166: Security And Privacy Keyboard Gesture And Shortcut Flow
**Task Title:** Security And Privacy Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for security and privacy.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for audit the canvas surface for unsafe ingest, embeds, and collaboration leaks.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** sanitize pasted HTML; redact private metadata; restrict dangerous URLs.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/ClipboardSanitizer.h
- /Users/ryanrentfro/code/markamp/src/core/DataRedactionEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/ContentSecurityPolicy.h

### P09-T167: Security And Privacy Visual Feedback And Rendering Polish
**Task Title:** Security And Privacy Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make security and privacy legible in use.
**Description:** Implement the visible feedback for audit the canvas surface for unsafe ingest, embeds, and collaboration leaks.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** sanitize pasted HTML; redact private metadata; restrict dangerous URLs.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/ClipboardSanitizer.h
- /Users/ryanrentfro/code/markamp/src/core/DataRedactionEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/ContentSecurityPolicy.h

### P09-T168: Security And Privacy Persistence Preferences And Serialization
**Task Title:** Security And Privacy Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by security and privacy.
**Description:** Identify what parts of audit the canvas surface for unsafe ingest, embeds, and collaboration leaks. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** sanitize pasted HTML; redact private metadata; restrict dangerous URLs.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/ClipboardSanitizer.h
- /Users/ryanrentfro/code/markamp/src/core/DataRedactionEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/ContentSecurityPolicy.h

### P09-T169: Security And Privacy Safeguards Telemetry And Recovery
**Task Title:** Security And Privacy Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for security and privacy.
**Description:** Instrument audit the canvas surface for unsafe ingest, embeds, and collaboration leaks. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** sanitize pasted HTML; redact private metadata; restrict dangerous URLs.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/ClipboardSanitizer.h
- /Users/ryanrentfro/code/markamp/src/core/DataRedactionEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/ContentSecurityPolicy.h

### P09-T170: Security And Privacy Tests Documentation And Rollout Gate
**Task Title:** Security And Privacy Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship security and privacy.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for audit the canvas surface for unsafe ingest, embeds, and collaboration leaks.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** sanitize pasted HTML; redact private metadata; restrict dangerous URLs.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/ClipboardSanitizer.h
- /Users/ryanrentfro/code/markamp/src/core/DataRedactionEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/ContentSecurityPolicy.h

## Workstream 18: Localization And IME
Support multilingual authoring, bidirectional text, and input method editors.

### P09-T171: Localization And IME UX Parity Audit
**Task Title:** Localization And IME UX Parity Audit
**Definition:** Define the implementation contract for localization and ime by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around support multilingual authoring, bidirectional text, and input method editors.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** RTL text box; IME candidate flow; localized command labels.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/LocalizationManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CommandPaletteModel.h

### P09-T172: Localization And IME Domain Model And State Contract
**Task Title:** Localization And IME Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support localization and ime.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around support multilingual authoring, bidirectional text, and input method editors.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** RTL text box; IME candidate flow; localized command labels.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/LocalizationManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CommandPaletteModel.h

### P09-T173: Localization And IME Commands Events And Context Keys
**Task Title:** Localization And IME Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for localization and ime.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make support multilingual authoring, bidirectional text, and input method editors. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** RTL text box; IME candidate flow; localized command labels.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/LocalizationManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CommandPaletteModel.h

### P09-T174: Localization And IME Workspace Surface And Controls
**Task Title:** Localization And IME Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose localization and ime in the main canvas workspace.
**Description:** Build the primary UI surfaces for support multilingual authoring, bidirectional text, and input method editors., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** RTL text box; IME candidate flow; localized command labels.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/LocalizationManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CommandPaletteModel.h

### P09-T175: Localization And IME Pointer Interaction Flow
**Task Title:** Localization And IME Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for localization and ime.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for support multilingual authoring, bidirectional text, and input method editors.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** RTL text box; IME candidate flow; localized command labels.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/LocalizationManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CommandPaletteModel.h

### P09-T176: Localization And IME Keyboard Gesture And Shortcut Flow
**Task Title:** Localization And IME Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for localization and ime.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for support multilingual authoring, bidirectional text, and input method editors.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** RTL text box; IME candidate flow; localized command labels.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/LocalizationManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CommandPaletteModel.h

### P09-T177: Localization And IME Visual Feedback And Rendering Polish
**Task Title:** Localization And IME Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make localization and ime legible in use.
**Description:** Implement the visible feedback for support multilingual authoring, bidirectional text, and input method editors.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** RTL text box; IME candidate flow; localized command labels.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/LocalizationManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CommandPaletteModel.h

### P09-T178: Localization And IME Persistence Preferences And Serialization
**Task Title:** Localization And IME Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by localization and ime.
**Description:** Identify what parts of support multilingual authoring, bidirectional text, and input method editors. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** RTL text box; IME candidate flow; localized command labels.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/LocalizationManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CommandPaletteModel.h

### P09-T179: Localization And IME Safeguards Telemetry And Recovery
**Task Title:** Localization And IME Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for localization and ime.
**Description:** Instrument support multilingual authoring, bidirectional text, and input method editors. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** RTL text box; IME candidate flow; localized command labels.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/LocalizationManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CommandPaletteModel.h

### P09-T180: Localization And IME Tests Documentation And Rollout Gate
**Task Title:** Localization And IME Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship localization and ime.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for support multilingual authoring, bidirectional text, and input method editors.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** RTL text box; IME candidate flow; localized command labels.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/LocalizationManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CommandPaletteModel.h

## Workstream 19: Compliance Checklists
Turn accessibility and reliability standards into repeatable release checks.

### P09-T181: Compliance Checklists UX Parity Audit
**Task Title:** Compliance Checklists UX Parity Audit
**Definition:** Define the implementation contract for compliance checklists by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around turn accessibility and reliability standards into repeatable release checks.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** a11y exit criteria; performance release gate; embed security checklist.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/docs
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityAudit.cpp
- /Users/ryanrentfro/code/markamp/src/core/CompilerConfigValidator.cpp

### P09-T182: Compliance Checklists Domain Model And State Contract
**Task Title:** Compliance Checklists Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support compliance checklists.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around turn accessibility and reliability standards into repeatable release checks.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** a11y exit criteria; performance release gate; embed security checklist.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/docs
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityAudit.cpp
- /Users/ryanrentfro/code/markamp/src/core/CompilerConfigValidator.cpp

### P09-T183: Compliance Checklists Commands Events And Context Keys
**Task Title:** Compliance Checklists Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for compliance checklists.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make turn accessibility and reliability standards into repeatable release checks. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** a11y exit criteria; performance release gate; embed security checklist.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/docs
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityAudit.cpp
- /Users/ryanrentfro/code/markamp/src/core/CompilerConfigValidator.cpp

### P09-T184: Compliance Checklists Workspace Surface And Controls
**Task Title:** Compliance Checklists Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose compliance checklists in the main canvas workspace.
**Description:** Build the primary UI surfaces for turn accessibility and reliability standards into repeatable release checks., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** a11y exit criteria; performance release gate; embed security checklist.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/docs
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityAudit.cpp
- /Users/ryanrentfro/code/markamp/src/core/CompilerConfigValidator.cpp

### P09-T185: Compliance Checklists Pointer Interaction Flow
**Task Title:** Compliance Checklists Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for compliance checklists.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for turn accessibility and reliability standards into repeatable release checks.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** a11y exit criteria; performance release gate; embed security checklist.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/docs
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityAudit.cpp
- /Users/ryanrentfro/code/markamp/src/core/CompilerConfigValidator.cpp

### P09-T186: Compliance Checklists Keyboard Gesture And Shortcut Flow
**Task Title:** Compliance Checklists Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for compliance checklists.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for turn accessibility and reliability standards into repeatable release checks.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** a11y exit criteria; performance release gate; embed security checklist.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/docs
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityAudit.cpp
- /Users/ryanrentfro/code/markamp/src/core/CompilerConfigValidator.cpp

### P09-T187: Compliance Checklists Visual Feedback And Rendering Polish
**Task Title:** Compliance Checklists Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make compliance checklists legible in use.
**Description:** Implement the visible feedback for turn accessibility and reliability standards into repeatable release checks.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** a11y exit criteria; performance release gate; embed security checklist.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/docs
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityAudit.cpp
- /Users/ryanrentfro/code/markamp/src/core/CompilerConfigValidator.cpp

### P09-T188: Compliance Checklists Persistence Preferences And Serialization
**Task Title:** Compliance Checklists Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by compliance checklists.
**Description:** Identify what parts of turn accessibility and reliability standards into repeatable release checks. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** a11y exit criteria; performance release gate; embed security checklist.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/docs
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityAudit.cpp
- /Users/ryanrentfro/code/markamp/src/core/CompilerConfigValidator.cpp

### P09-T189: Compliance Checklists Safeguards Telemetry And Recovery
**Task Title:** Compliance Checklists Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for compliance checklists.
**Description:** Instrument turn accessibility and reliability standards into repeatable release checks. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** a11y exit criteria; performance release gate; embed security checklist.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/docs
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityAudit.cpp
- /Users/ryanrentfro/code/markamp/src/core/CompilerConfigValidator.cpp

### P09-T190: Compliance Checklists Tests Documentation And Rollout Gate
**Task Title:** Compliance Checklists Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship compliance checklists.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for turn accessibility and reliability standards into repeatable release checks.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** a11y exit criteria; performance release gate; embed security checklist.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/docs
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityAudit.cpp
- /Users/ryanrentfro/code/markamp/src/core/CompilerConfigValidator.cpp

## Workstream 20: Operational Dashboards
Expose board health signals to engineering and support teams.

### P09-T191: Operational Dashboards UX Parity Audit
**Task Title:** Operational Dashboards UX Parity Audit
**Definition:** Define the implementation contract for operational dashboards by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around expose board health signals to engineering and support teams.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** slow board cohorts; crash-free rate; failed export trend.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/BuildService.cpp
- /Users/ryanrentfro/code/markamp/src/core/ActivityFeed.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BenchmarkModel.cpp

### P09-T192: Operational Dashboards Domain Model And State Contract
**Task Title:** Operational Dashboards Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support operational dashboards.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around expose board health signals to engineering and support teams.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** slow board cohorts; crash-free rate; failed export trend.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/BuildService.cpp
- /Users/ryanrentfro/code/markamp/src/core/ActivityFeed.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BenchmarkModel.cpp

### P09-T193: Operational Dashboards Commands Events And Context Keys
**Task Title:** Operational Dashboards Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for operational dashboards.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make expose board health signals to engineering and support teams. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** slow board cohorts; crash-free rate; failed export trend.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/BuildService.cpp
- /Users/ryanrentfro/code/markamp/src/core/ActivityFeed.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BenchmarkModel.cpp

### P09-T194: Operational Dashboards Workspace Surface And Controls
**Task Title:** Operational Dashboards Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose operational dashboards in the main canvas workspace.
**Description:** Build the primary UI surfaces for expose board health signals to engineering and support teams., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** slow board cohorts; crash-free rate; failed export trend.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/BuildService.cpp
- /Users/ryanrentfro/code/markamp/src/core/ActivityFeed.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BenchmarkModel.cpp

### P09-T195: Operational Dashboards Pointer Interaction Flow
**Task Title:** Operational Dashboards Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for operational dashboards.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for expose board health signals to engineering and support teams.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** slow board cohorts; crash-free rate; failed export trend.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/BuildService.cpp
- /Users/ryanrentfro/code/markamp/src/core/ActivityFeed.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BenchmarkModel.cpp

### P09-T196: Operational Dashboards Keyboard Gesture And Shortcut Flow
**Task Title:** Operational Dashboards Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for operational dashboards.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for expose board health signals to engineering and support teams.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** slow board cohorts; crash-free rate; failed export trend.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/BuildService.cpp
- /Users/ryanrentfro/code/markamp/src/core/ActivityFeed.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BenchmarkModel.cpp

### P09-T197: Operational Dashboards Visual Feedback And Rendering Polish
**Task Title:** Operational Dashboards Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make operational dashboards legible in use.
**Description:** Implement the visible feedback for expose board health signals to engineering and support teams.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** slow board cohorts; crash-free rate; failed export trend.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/BuildService.cpp
- /Users/ryanrentfro/code/markamp/src/core/ActivityFeed.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BenchmarkModel.cpp

### P09-T198: Operational Dashboards Persistence Preferences And Serialization
**Task Title:** Operational Dashboards Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by operational dashboards.
**Description:** Identify what parts of expose board health signals to engineering and support teams. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** slow board cohorts; crash-free rate; failed export trend.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/BuildService.cpp
- /Users/ryanrentfro/code/markamp/src/core/ActivityFeed.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BenchmarkModel.cpp

### P09-T199: Operational Dashboards Safeguards Telemetry And Recovery
**Task Title:** Operational Dashboards Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for operational dashboards.
**Description:** Instrument expose board health signals to engineering and support teams. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** slow board cohorts; crash-free rate; failed export trend.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/BuildService.cpp
- /Users/ryanrentfro/code/markamp/src/core/ActivityFeed.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BenchmarkModel.cpp

### P09-T200: Operational Dashboards Tests Documentation And Rollout Gate
**Task Title:** Operational Dashboards Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship operational dashboards.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for expose board health signals to engineering and support teams.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** slow board cohorts; crash-free rate; failed export trend.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasAccessibility.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AccessibilitySemanticsController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/BuildService.cpp
- /Users/ryanrentfro/code/markamp/src/core/ActivityFeed.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BenchmarkModel.cpp

## Phase Exit Criteria
- All 200 tasks have either shipped, been explicitly deferred with rationale, or been converted into implementation issues with owners.
- Critical workflows in this phase have unit, integration, and at least one end-to-end validation path or a documented gap.
- The shipped work is theme-aware, accessibility-aware, serialization-safe, and undo/redo-safe by default.
- The phase produces measurable progress toward Miro-level editing, drawing, moving, and content authoring quality.
