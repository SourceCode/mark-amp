# Phase 10: Polish Parity Validation And Platform Completion

## Objective
Finish the Miro-parity program with audit, polish, platform fit, release gates, and long-term backlog shaping so the canvas can ship coherently.

## AI Agent Execution Rules
- Execute workstreams in order unless a later task is explicitly unblocked by completed model and command contracts.
- Prefer extending existing canvas, UI, and core services before introducing new parallel abstractions.
- Every implementation task should finish with tests or a documented testing gap.
- Preserve theme, accessibility, undo/redo, and serialization behavior as default quality bars rather than follow-up work.
- When adding references to new code during implementation, keep using full file paths in planning notes and PR write-ups.

## Workstreams
- W01 Parity Audit: Systematically compare the shipped canvas against target competitor behaviors and close the highest-value gaps.
- W02 Control Polish: Refine control density, labels, placement, and affordances so the UI reads as intentional.
- W03 Motion And Microinteraction: Use motion sparingly to improve clarity and perceived responsiveness.
- W04 Platform Conventions: Make the canvas feel native across macOS, Windows, and Linux input and chrome conventions.
- W05 Settings Migration: Ensure new canvas capabilities land without breaking existing user preferences.
- W06 Documentation: Ship human-facing docs that match the implemented workflows and shortcuts.
- W07 Benchmarks And Baselines: Track performance progress and prevent slow regressions from hiding in polish work.
- W08 Release Gates: Define objective exit criteria before promoting the new canvas UX broadly.
- W09 Beta Feedback Loops: Make user feedback actionable and tied to board states that engineers can replay.
- W10 Extension Ecosystem: Give plugins a coherent way to align with the improved canvas UX.
- W11 Advanced Drawing Polish: Finish rough edges in pen, shape, and text authoring so the surface feels production-grade.
- W12 Advanced Layout Polish: Refine transform and alignment behavior under complex board conditions.
- W13 Advanced Collaboration Polish: Resolve edge frictions in multi-user sessions before broad release.
- W14 Advanced Navigation Polish: Make large-board travel feel calm, not frantic.
- W15 Advanced Export Polish: Verify the last mile from board editing to artifact production.
- W16 Enterprise And Admin Fit: Cover governance and admin concerns needed for larger deployments.
- W17 Quality Backlog Triage: Turn defects found during rollout into a prioritized, bounded program.
- W18 Adoption Metrics: Measure whether the upgraded canvas is actually being used more effectively.
- W19 Support Playbooks: Prepare engineering and support for the issues users will actually report.
- W20 Architecture Follow Ups: Capture the longer-term technical debt and platform investments revealed by the parity push.

## Workstream 01: Parity Audit
Systematically compare the shipped canvas against target competitor behaviors and close the highest-value gaps.

### P10-T001: Parity Audit UX Parity Audit
**Task Title:** Parity Audit UX Parity Audit
**Definition:** Define the implementation contract for parity audit by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around systematically compare the shipped canvas against target competitor behaviors and close the highest-value gaps.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** miro feature matrix; gap severity scoring; cut list for release.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/docs/v9_docs/Phase_11__Canvas_Workbench_Shell.md
- /Users/ryanrentfro/code/markamp/docs/v9_docs/Phase_12__Canvas_Advanced_Objects_And_Diagrams.md
- /Users/ryanrentfro/code/markamp/src/canvas/AuditModel.cpp

### P10-T002: Parity Audit Domain Model And State Contract
**Task Title:** Parity Audit Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support parity audit.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around systematically compare the shipped canvas against target competitor behaviors and close the highest-value gaps.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** miro feature matrix; gap severity scoring; cut list for release.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/docs/v9_docs/Phase_11__Canvas_Workbench_Shell.md
- /Users/ryanrentfro/code/markamp/docs/v9_docs/Phase_12__Canvas_Advanced_Objects_And_Diagrams.md
- /Users/ryanrentfro/code/markamp/src/canvas/AuditModel.cpp

### P10-T003: Parity Audit Commands Events And Context Keys
**Task Title:** Parity Audit Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for parity audit.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make systematically compare the shipped canvas against target competitor behaviors and close the highest-value gaps. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** miro feature matrix; gap severity scoring; cut list for release.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/docs/v9_docs/Phase_11__Canvas_Workbench_Shell.md
- /Users/ryanrentfro/code/markamp/docs/v9_docs/Phase_12__Canvas_Advanced_Objects_And_Diagrams.md
- /Users/ryanrentfro/code/markamp/src/canvas/AuditModel.cpp

### P10-T004: Parity Audit Workspace Surface And Controls
**Task Title:** Parity Audit Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose parity audit in the main canvas workspace.
**Description:** Build the primary UI surfaces for systematically compare the shipped canvas against target competitor behaviors and close the highest-value gaps., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** miro feature matrix; gap severity scoring; cut list for release.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/docs/v9_docs/Phase_11__Canvas_Workbench_Shell.md
- /Users/ryanrentfro/code/markamp/docs/v9_docs/Phase_12__Canvas_Advanced_Objects_And_Diagrams.md
- /Users/ryanrentfro/code/markamp/src/canvas/AuditModel.cpp

### P10-T005: Parity Audit Pointer Interaction Flow
**Task Title:** Parity Audit Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for parity audit.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for systematically compare the shipped canvas against target competitor behaviors and close the highest-value gaps.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** miro feature matrix; gap severity scoring; cut list for release.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/docs/v9_docs/Phase_11__Canvas_Workbench_Shell.md
- /Users/ryanrentfro/code/markamp/docs/v9_docs/Phase_12__Canvas_Advanced_Objects_And_Diagrams.md
- /Users/ryanrentfro/code/markamp/src/canvas/AuditModel.cpp

### P10-T006: Parity Audit Keyboard Gesture And Shortcut Flow
**Task Title:** Parity Audit Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for parity audit.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for systematically compare the shipped canvas against target competitor behaviors and close the highest-value gaps.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** miro feature matrix; gap severity scoring; cut list for release.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/docs/v9_docs/Phase_11__Canvas_Workbench_Shell.md
- /Users/ryanrentfro/code/markamp/docs/v9_docs/Phase_12__Canvas_Advanced_Objects_And_Diagrams.md
- /Users/ryanrentfro/code/markamp/src/canvas/AuditModel.cpp

### P10-T007: Parity Audit Visual Feedback And Rendering Polish
**Task Title:** Parity Audit Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make parity audit legible in use.
**Description:** Implement the visible feedback for systematically compare the shipped canvas against target competitor behaviors and close the highest-value gaps.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** miro feature matrix; gap severity scoring; cut list for release.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/docs/v9_docs/Phase_11__Canvas_Workbench_Shell.md
- /Users/ryanrentfro/code/markamp/docs/v9_docs/Phase_12__Canvas_Advanced_Objects_And_Diagrams.md
- /Users/ryanrentfro/code/markamp/src/canvas/AuditModel.cpp

### P10-T008: Parity Audit Persistence Preferences And Serialization
**Task Title:** Parity Audit Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by parity audit.
**Description:** Identify what parts of systematically compare the shipped canvas against target competitor behaviors and close the highest-value gaps. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** miro feature matrix; gap severity scoring; cut list for release.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/docs/v9_docs/Phase_11__Canvas_Workbench_Shell.md
- /Users/ryanrentfro/code/markamp/docs/v9_docs/Phase_12__Canvas_Advanced_Objects_And_Diagrams.md
- /Users/ryanrentfro/code/markamp/src/canvas/AuditModel.cpp

### P10-T009: Parity Audit Safeguards Telemetry And Recovery
**Task Title:** Parity Audit Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for parity audit.
**Description:** Instrument systematically compare the shipped canvas against target competitor behaviors and close the highest-value gaps. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** miro feature matrix; gap severity scoring; cut list for release.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/docs/v9_docs/Phase_11__Canvas_Workbench_Shell.md
- /Users/ryanrentfro/code/markamp/docs/v9_docs/Phase_12__Canvas_Advanced_Objects_And_Diagrams.md
- /Users/ryanrentfro/code/markamp/src/canvas/AuditModel.cpp

### P10-T010: Parity Audit Tests Documentation And Rollout Gate
**Task Title:** Parity Audit Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship parity audit.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for systematically compare the shipped canvas against target competitor behaviors and close the highest-value gaps.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** miro feature matrix; gap severity scoring; cut list for release.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/docs/v9_docs/Phase_11__Canvas_Workbench_Shell.md
- /Users/ryanrentfro/code/markamp/docs/v9_docs/Phase_12__Canvas_Advanced_Objects_And_Diagrams.md
- /Users/ryanrentfro/code/markamp/src/canvas/AuditModel.cpp

## Workstream 02: Control Polish
Refine control density, labels, placement, and affordances so the UI reads as intentional.

### P10-T011: Control Polish UX Parity Audit
**Task Title:** Control Polish UX Parity Audit
**Definition:** Define the implementation contract for control polish by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around refine control density, labels, placement, and affordances so the ui reads as intentional.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** clean toolbar spacing; button grouping; less modal friction.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/ui/DesignSystemConstants.h
- /Users/ryanrentfro/code/markamp/src/ui/ComponentVariants.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CustomChrome.cpp

### P10-T012: Control Polish Domain Model And State Contract
**Task Title:** Control Polish Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support control polish.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around refine control density, labels, placement, and affordances so the ui reads as intentional.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** clean toolbar spacing; button grouping; less modal friction.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/ui/DesignSystemConstants.h
- /Users/ryanrentfro/code/markamp/src/ui/ComponentVariants.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CustomChrome.cpp

### P10-T013: Control Polish Commands Events And Context Keys
**Task Title:** Control Polish Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for control polish.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make refine control density, labels, placement, and affordances so the ui reads as intentional. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** clean toolbar spacing; button grouping; less modal friction.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/ui/DesignSystemConstants.h
- /Users/ryanrentfro/code/markamp/src/ui/ComponentVariants.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CustomChrome.cpp

### P10-T014: Control Polish Workspace Surface And Controls
**Task Title:** Control Polish Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose control polish in the main canvas workspace.
**Description:** Build the primary UI surfaces for refine control density, labels, placement, and affordances so the ui reads as intentional., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** clean toolbar spacing; button grouping; less modal friction.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/ui/DesignSystemConstants.h
- /Users/ryanrentfro/code/markamp/src/ui/ComponentVariants.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CustomChrome.cpp

### P10-T015: Control Polish Pointer Interaction Flow
**Task Title:** Control Polish Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for control polish.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for refine control density, labels, placement, and affordances so the ui reads as intentional.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** clean toolbar spacing; button grouping; less modal friction.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/ui/DesignSystemConstants.h
- /Users/ryanrentfro/code/markamp/src/ui/ComponentVariants.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CustomChrome.cpp

### P10-T016: Control Polish Keyboard Gesture And Shortcut Flow
**Task Title:** Control Polish Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for control polish.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for refine control density, labels, placement, and affordances so the ui reads as intentional.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** clean toolbar spacing; button grouping; less modal friction.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/ui/DesignSystemConstants.h
- /Users/ryanrentfro/code/markamp/src/ui/ComponentVariants.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CustomChrome.cpp

### P10-T017: Control Polish Visual Feedback And Rendering Polish
**Task Title:** Control Polish Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make control polish legible in use.
**Description:** Implement the visible feedback for refine control density, labels, placement, and affordances so the ui reads as intentional.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** clean toolbar spacing; button grouping; less modal friction.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/ui/DesignSystemConstants.h
- /Users/ryanrentfro/code/markamp/src/ui/ComponentVariants.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CustomChrome.cpp

### P10-T018: Control Polish Persistence Preferences And Serialization
**Task Title:** Control Polish Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by control polish.
**Description:** Identify what parts of refine control density, labels, placement, and affordances so the ui reads as intentional. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** clean toolbar spacing; button grouping; less modal friction.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/ui/DesignSystemConstants.h
- /Users/ryanrentfro/code/markamp/src/ui/ComponentVariants.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CustomChrome.cpp

### P10-T019: Control Polish Safeguards Telemetry And Recovery
**Task Title:** Control Polish Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for control polish.
**Description:** Instrument refine control density, labels, placement, and affordances so the ui reads as intentional. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** clean toolbar spacing; button grouping; less modal friction.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/ui/DesignSystemConstants.h
- /Users/ryanrentfro/code/markamp/src/ui/ComponentVariants.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CustomChrome.cpp

### P10-T020: Control Polish Tests Documentation And Rollout Gate
**Task Title:** Control Polish Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship control polish.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for refine control density, labels, placement, and affordances so the ui reads as intentional.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** clean toolbar spacing; button grouping; less modal friction.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/ui/DesignSystemConstants.h
- /Users/ryanrentfro/code/markamp/src/ui/ComponentVariants.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CustomChrome.cpp

## Workstream 03: Motion And Microinteraction
Use motion sparingly to improve clarity and perceived responsiveness.

### P10-T021: Motion And Microinteraction UX Parity Audit
**Task Title:** Motion And Microinteraction UX Parity Audit
**Definition:** Define the implementation contract for motion and microinteraction by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around use motion sparingly to improve clarity and perceived responsiveness.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** toolbar reveal; selection settle animation; export progress transition.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.h
- /Users/ryanrentfro/code/markamp/src/ui/ControlMotionTokens.h
- /Users/ryanrentfro/code/markamp/src/ui/ControlAnimator.h

### P10-T022: Motion And Microinteraction Domain Model And State Contract
**Task Title:** Motion And Microinteraction Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support motion and microinteraction.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around use motion sparingly to improve clarity and perceived responsiveness.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** toolbar reveal; selection settle animation; export progress transition.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.h
- /Users/ryanrentfro/code/markamp/src/ui/ControlMotionTokens.h
- /Users/ryanrentfro/code/markamp/src/ui/ControlAnimator.h

### P10-T023: Motion And Microinteraction Commands Events And Context Keys
**Task Title:** Motion And Microinteraction Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for motion and microinteraction.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make use motion sparingly to improve clarity and perceived responsiveness. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** toolbar reveal; selection settle animation; export progress transition.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.h
- /Users/ryanrentfro/code/markamp/src/ui/ControlMotionTokens.h
- /Users/ryanrentfro/code/markamp/src/ui/ControlAnimator.h

### P10-T024: Motion And Microinteraction Workspace Surface And Controls
**Task Title:** Motion And Microinteraction Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose motion and microinteraction in the main canvas workspace.
**Description:** Build the primary UI surfaces for use motion sparingly to improve clarity and perceived responsiveness., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** toolbar reveal; selection settle animation; export progress transition.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.h
- /Users/ryanrentfro/code/markamp/src/ui/ControlMotionTokens.h
- /Users/ryanrentfro/code/markamp/src/ui/ControlAnimator.h

### P10-T025: Motion And Microinteraction Pointer Interaction Flow
**Task Title:** Motion And Microinteraction Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for motion and microinteraction.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for use motion sparingly to improve clarity and perceived responsiveness.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** toolbar reveal; selection settle animation; export progress transition.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.h
- /Users/ryanrentfro/code/markamp/src/ui/ControlMotionTokens.h
- /Users/ryanrentfro/code/markamp/src/ui/ControlAnimator.h

### P10-T026: Motion And Microinteraction Keyboard Gesture And Shortcut Flow
**Task Title:** Motion And Microinteraction Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for motion and microinteraction.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for use motion sparingly to improve clarity and perceived responsiveness.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** toolbar reveal; selection settle animation; export progress transition.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.h
- /Users/ryanrentfro/code/markamp/src/ui/ControlMotionTokens.h
- /Users/ryanrentfro/code/markamp/src/ui/ControlAnimator.h

### P10-T027: Motion And Microinteraction Visual Feedback And Rendering Polish
**Task Title:** Motion And Microinteraction Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make motion and microinteraction legible in use.
**Description:** Implement the visible feedback for use motion sparingly to improve clarity and perceived responsiveness.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** toolbar reveal; selection settle animation; export progress transition.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.h
- /Users/ryanrentfro/code/markamp/src/ui/ControlMotionTokens.h
- /Users/ryanrentfro/code/markamp/src/ui/ControlAnimator.h

### P10-T028: Motion And Microinteraction Persistence Preferences And Serialization
**Task Title:** Motion And Microinteraction Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by motion and microinteraction.
**Description:** Identify what parts of use motion sparingly to improve clarity and perceived responsiveness. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** toolbar reveal; selection settle animation; export progress transition.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.h
- /Users/ryanrentfro/code/markamp/src/ui/ControlMotionTokens.h
- /Users/ryanrentfro/code/markamp/src/ui/ControlAnimator.h

### P10-T029: Motion And Microinteraction Safeguards Telemetry And Recovery
**Task Title:** Motion And Microinteraction Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for motion and microinteraction.
**Description:** Instrument use motion sparingly to improve clarity and perceived responsiveness. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** toolbar reveal; selection settle animation; export progress transition.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.h
- /Users/ryanrentfro/code/markamp/src/ui/ControlMotionTokens.h
- /Users/ryanrentfro/code/markamp/src/ui/ControlAnimator.h

### P10-T030: Motion And Microinteraction Tests Documentation And Rollout Gate
**Task Title:** Motion And Microinteraction Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship motion and microinteraction.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for use motion sparingly to improve clarity and perceived responsiveness.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** toolbar reveal; selection settle animation; export progress transition.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.h
- /Users/ryanrentfro/code/markamp/src/ui/ControlMotionTokens.h
- /Users/ryanrentfro/code/markamp/src/ui/ControlAnimator.h

## Workstream 04: Platform Conventions
Make the canvas feel native across macOS, Windows, and Linux input and chrome conventions.

### P10-T031: Platform Conventions UX Parity Audit
**Task Title:** Platform Conventions UX Parity Audit
**Definition:** Define the implementation contract for platform conventions by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around make the canvas feel native across macos, windows, and linux input and chrome conventions.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** mac trackpad gestures; windows context menu expectation; linux window behavior.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/platform/MacPlatformIntegration.cpp
- /Users/ryanrentfro/code/markamp/src/platform/WindowsPlatformIntegration.cpp
- /Users/ryanrentfro/code/markamp/src/platform/LinuxPlatformIntegration.cpp

### P10-T032: Platform Conventions Domain Model And State Contract
**Task Title:** Platform Conventions Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support platform conventions.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around make the canvas feel native across macos, windows, and linux input and chrome conventions.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** mac trackpad gestures; windows context menu expectation; linux window behavior.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/platform/MacPlatformIntegration.cpp
- /Users/ryanrentfro/code/markamp/src/platform/WindowsPlatformIntegration.cpp
- /Users/ryanrentfro/code/markamp/src/platform/LinuxPlatformIntegration.cpp

### P10-T033: Platform Conventions Commands Events And Context Keys
**Task Title:** Platform Conventions Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for platform conventions.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make make the canvas feel native across macos, windows, and linux input and chrome conventions. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** mac trackpad gestures; windows context menu expectation; linux window behavior.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/platform/MacPlatformIntegration.cpp
- /Users/ryanrentfro/code/markamp/src/platform/WindowsPlatformIntegration.cpp
- /Users/ryanrentfro/code/markamp/src/platform/LinuxPlatformIntegration.cpp

### P10-T034: Platform Conventions Workspace Surface And Controls
**Task Title:** Platform Conventions Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose platform conventions in the main canvas workspace.
**Description:** Build the primary UI surfaces for make the canvas feel native across macos, windows, and linux input and chrome conventions., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** mac trackpad gestures; windows context menu expectation; linux window behavior.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/platform/MacPlatformIntegration.cpp
- /Users/ryanrentfro/code/markamp/src/platform/WindowsPlatformIntegration.cpp
- /Users/ryanrentfro/code/markamp/src/platform/LinuxPlatformIntegration.cpp

### P10-T035: Platform Conventions Pointer Interaction Flow
**Task Title:** Platform Conventions Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for platform conventions.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for make the canvas feel native across macos, windows, and linux input and chrome conventions.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** mac trackpad gestures; windows context menu expectation; linux window behavior.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/platform/MacPlatformIntegration.cpp
- /Users/ryanrentfro/code/markamp/src/platform/WindowsPlatformIntegration.cpp
- /Users/ryanrentfro/code/markamp/src/platform/LinuxPlatformIntegration.cpp

### P10-T036: Platform Conventions Keyboard Gesture And Shortcut Flow
**Task Title:** Platform Conventions Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for platform conventions.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for make the canvas feel native across macos, windows, and linux input and chrome conventions.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** mac trackpad gestures; windows context menu expectation; linux window behavior.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/platform/MacPlatformIntegration.cpp
- /Users/ryanrentfro/code/markamp/src/platform/WindowsPlatformIntegration.cpp
- /Users/ryanrentfro/code/markamp/src/platform/LinuxPlatformIntegration.cpp

### P10-T037: Platform Conventions Visual Feedback And Rendering Polish
**Task Title:** Platform Conventions Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make platform conventions legible in use.
**Description:** Implement the visible feedback for make the canvas feel native across macos, windows, and linux input and chrome conventions.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** mac trackpad gestures; windows context menu expectation; linux window behavior.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/platform/MacPlatformIntegration.cpp
- /Users/ryanrentfro/code/markamp/src/platform/WindowsPlatformIntegration.cpp
- /Users/ryanrentfro/code/markamp/src/platform/LinuxPlatformIntegration.cpp

### P10-T038: Platform Conventions Persistence Preferences And Serialization
**Task Title:** Platform Conventions Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by platform conventions.
**Description:** Identify what parts of make the canvas feel native across macos, windows, and linux input and chrome conventions. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** mac trackpad gestures; windows context menu expectation; linux window behavior.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/platform/MacPlatformIntegration.cpp
- /Users/ryanrentfro/code/markamp/src/platform/WindowsPlatformIntegration.cpp
- /Users/ryanrentfro/code/markamp/src/platform/LinuxPlatformIntegration.cpp

### P10-T039: Platform Conventions Safeguards Telemetry And Recovery
**Task Title:** Platform Conventions Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for platform conventions.
**Description:** Instrument make the canvas feel native across macos, windows, and linux input and chrome conventions. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** mac trackpad gestures; windows context menu expectation; linux window behavior.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/platform/MacPlatformIntegration.cpp
- /Users/ryanrentfro/code/markamp/src/platform/WindowsPlatformIntegration.cpp
- /Users/ryanrentfro/code/markamp/src/platform/LinuxPlatformIntegration.cpp

### P10-T040: Platform Conventions Tests Documentation And Rollout Gate
**Task Title:** Platform Conventions Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship platform conventions.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for make the canvas feel native across macos, windows, and linux input and chrome conventions.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** mac trackpad gestures; windows context menu expectation; linux window behavior.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/platform/MacPlatformIntegration.cpp
- /Users/ryanrentfro/code/markamp/src/platform/WindowsPlatformIntegration.cpp
- /Users/ryanrentfro/code/markamp/src/platform/LinuxPlatformIntegration.cpp

## Workstream 05: Settings Migration
Ensure new canvas capabilities land without breaking existing user preferences.

### P10-T041: Settings Migration UX Parity Audit
**Task Title:** Settings Migration UX Parity Audit
**Definition:** Define the implementation contract for settings migration by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around ensure new canvas capabilities land without breaking existing user preferences.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** migrate snap settings; default new token values; repair missing board prefs.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.h
- /Users/ryanrentfro/code/markamp/src/core/ConfigProfile.h
- /Users/ryanrentfro/code/markamp/src/core/ConfigAuditTrail.h

### P10-T042: Settings Migration Domain Model And State Contract
**Task Title:** Settings Migration Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support settings migration.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around ensure new canvas capabilities land without breaking existing user preferences.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** migrate snap settings; default new token values; repair missing board prefs.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.h
- /Users/ryanrentfro/code/markamp/src/core/ConfigProfile.h
- /Users/ryanrentfro/code/markamp/src/core/ConfigAuditTrail.h

### P10-T043: Settings Migration Commands Events And Context Keys
**Task Title:** Settings Migration Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for settings migration.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make ensure new canvas capabilities land without breaking existing user preferences. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** migrate snap settings; default new token values; repair missing board prefs.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.h
- /Users/ryanrentfro/code/markamp/src/core/ConfigProfile.h
- /Users/ryanrentfro/code/markamp/src/core/ConfigAuditTrail.h

### P10-T044: Settings Migration Workspace Surface And Controls
**Task Title:** Settings Migration Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose settings migration in the main canvas workspace.
**Description:** Build the primary UI surfaces for ensure new canvas capabilities land without breaking existing user preferences., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** migrate snap settings; default new token values; repair missing board prefs.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.h
- /Users/ryanrentfro/code/markamp/src/core/ConfigProfile.h
- /Users/ryanrentfro/code/markamp/src/core/ConfigAuditTrail.h

### P10-T045: Settings Migration Pointer Interaction Flow
**Task Title:** Settings Migration Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for settings migration.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for ensure new canvas capabilities land without breaking existing user preferences.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** migrate snap settings; default new token values; repair missing board prefs.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.h
- /Users/ryanrentfro/code/markamp/src/core/ConfigProfile.h
- /Users/ryanrentfro/code/markamp/src/core/ConfigAuditTrail.h

### P10-T046: Settings Migration Keyboard Gesture And Shortcut Flow
**Task Title:** Settings Migration Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for settings migration.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for ensure new canvas capabilities land without breaking existing user preferences.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** migrate snap settings; default new token values; repair missing board prefs.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.h
- /Users/ryanrentfro/code/markamp/src/core/ConfigProfile.h
- /Users/ryanrentfro/code/markamp/src/core/ConfigAuditTrail.h

### P10-T047: Settings Migration Visual Feedback And Rendering Polish
**Task Title:** Settings Migration Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make settings migration legible in use.
**Description:** Implement the visible feedback for ensure new canvas capabilities land without breaking existing user preferences.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** migrate snap settings; default new token values; repair missing board prefs.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.h
- /Users/ryanrentfro/code/markamp/src/core/ConfigProfile.h
- /Users/ryanrentfro/code/markamp/src/core/ConfigAuditTrail.h

### P10-T048: Settings Migration Persistence Preferences And Serialization
**Task Title:** Settings Migration Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by settings migration.
**Description:** Identify what parts of ensure new canvas capabilities land without breaking existing user preferences. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** migrate snap settings; default new token values; repair missing board prefs.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.h
- /Users/ryanrentfro/code/markamp/src/core/ConfigProfile.h
- /Users/ryanrentfro/code/markamp/src/core/ConfigAuditTrail.h

### P10-T049: Settings Migration Safeguards Telemetry And Recovery
**Task Title:** Settings Migration Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for settings migration.
**Description:** Instrument ensure new canvas capabilities land without breaking existing user preferences. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** migrate snap settings; default new token values; repair missing board prefs.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.h
- /Users/ryanrentfro/code/markamp/src/core/ConfigProfile.h
- /Users/ryanrentfro/code/markamp/src/core/ConfigAuditTrail.h

### P10-T050: Settings Migration Tests Documentation And Rollout Gate
**Task Title:** Settings Migration Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship settings migration.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for ensure new canvas capabilities land without breaking existing user preferences.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** migrate snap settings; default new token values; repair missing board prefs.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.h
- /Users/ryanrentfro/code/markamp/src/core/ConfigProfile.h
- /Users/ryanrentfro/code/markamp/src/core/ConfigAuditTrail.h

## Workstream 06: Documentation
Ship human-facing docs that match the implemented workflows and shortcuts.

### P10-T051: Documentation UX Parity Audit
**Task Title:** Documentation UX Parity Audit
**Definition:** Define the implementation contract for documentation by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around ship human-facing docs that match the implemented workflows and shortcuts.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** canvas getting started; diagram guide; collaboration facilitation guide.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/docs
- /Users/ryanrentfro/code/markamp/src/ui/ChangelogPanel.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandRegistry.cpp

### P10-T052: Documentation Domain Model And State Contract
**Task Title:** Documentation Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support documentation.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around ship human-facing docs that match the implemented workflows and shortcuts.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** canvas getting started; diagram guide; collaboration facilitation guide.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/docs
- /Users/ryanrentfro/code/markamp/src/ui/ChangelogPanel.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandRegistry.cpp

### P10-T053: Documentation Commands Events And Context Keys
**Task Title:** Documentation Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for documentation.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make ship human-facing docs that match the implemented workflows and shortcuts. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** canvas getting started; diagram guide; collaboration facilitation guide.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/docs
- /Users/ryanrentfro/code/markamp/src/ui/ChangelogPanel.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandRegistry.cpp

### P10-T054: Documentation Workspace Surface And Controls
**Task Title:** Documentation Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose documentation in the main canvas workspace.
**Description:** Build the primary UI surfaces for ship human-facing docs that match the implemented workflows and shortcuts., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** canvas getting started; diagram guide; collaboration facilitation guide.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/docs
- /Users/ryanrentfro/code/markamp/src/ui/ChangelogPanel.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandRegistry.cpp

### P10-T055: Documentation Pointer Interaction Flow
**Task Title:** Documentation Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for documentation.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for ship human-facing docs that match the implemented workflows and shortcuts.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** canvas getting started; diagram guide; collaboration facilitation guide.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/docs
- /Users/ryanrentfro/code/markamp/src/ui/ChangelogPanel.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandRegistry.cpp

### P10-T056: Documentation Keyboard Gesture And Shortcut Flow
**Task Title:** Documentation Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for documentation.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for ship human-facing docs that match the implemented workflows and shortcuts.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** canvas getting started; diagram guide; collaboration facilitation guide.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/docs
- /Users/ryanrentfro/code/markamp/src/ui/ChangelogPanel.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandRegistry.cpp

### P10-T057: Documentation Visual Feedback And Rendering Polish
**Task Title:** Documentation Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make documentation legible in use.
**Description:** Implement the visible feedback for ship human-facing docs that match the implemented workflows and shortcuts.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** canvas getting started; diagram guide; collaboration facilitation guide.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/docs
- /Users/ryanrentfro/code/markamp/src/ui/ChangelogPanel.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandRegistry.cpp

### P10-T058: Documentation Persistence Preferences And Serialization
**Task Title:** Documentation Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by documentation.
**Description:** Identify what parts of ship human-facing docs that match the implemented workflows and shortcuts. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** canvas getting started; diagram guide; collaboration facilitation guide.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/docs
- /Users/ryanrentfro/code/markamp/src/ui/ChangelogPanel.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandRegistry.cpp

### P10-T059: Documentation Safeguards Telemetry And Recovery
**Task Title:** Documentation Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for documentation.
**Description:** Instrument ship human-facing docs that match the implemented workflows and shortcuts. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** canvas getting started; diagram guide; collaboration facilitation guide.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/docs
- /Users/ryanrentfro/code/markamp/src/ui/ChangelogPanel.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandRegistry.cpp

### P10-T060: Documentation Tests Documentation And Rollout Gate
**Task Title:** Documentation Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship documentation.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for ship human-facing docs that match the implemented workflows and shortcuts.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** canvas getting started; diagram guide; collaboration facilitation guide.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/docs
- /Users/ryanrentfro/code/markamp/src/ui/ChangelogPanel.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandRegistry.cpp

## Workstream 07: Benchmarks And Baselines
Track performance progress and prevent slow regressions from hiding in polish work.

### P10-T061: Benchmarks And Baselines UX Parity Audit
**Task Title:** Benchmarks And Baselines UX Parity Audit
**Definition:** Define the implementation contract for benchmarks and baselines by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around track performance progress and prevent slow regressions from hiding in polish work.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** baseline render metrics; drag latency benchmark; search latency benchmark.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/canvas/BenchmarkModel.h
- /Users/ryanrentfro/code/markamp/scripts/benchmark_update_baseline.sh

### P10-T062: Benchmarks And Baselines Domain Model And State Contract
**Task Title:** Benchmarks And Baselines Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support benchmarks and baselines.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around track performance progress and prevent slow regressions from hiding in polish work.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** baseline render metrics; drag latency benchmark; search latency benchmark.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/canvas/BenchmarkModel.h
- /Users/ryanrentfro/code/markamp/scripts/benchmark_update_baseline.sh

### P10-T063: Benchmarks And Baselines Commands Events And Context Keys
**Task Title:** Benchmarks And Baselines Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for benchmarks and baselines.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make track performance progress and prevent slow regressions from hiding in polish work. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** baseline render metrics; drag latency benchmark; search latency benchmark.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/canvas/BenchmarkModel.h
- /Users/ryanrentfro/code/markamp/scripts/benchmark_update_baseline.sh

### P10-T064: Benchmarks And Baselines Workspace Surface And Controls
**Task Title:** Benchmarks And Baselines Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose benchmarks and baselines in the main canvas workspace.
**Description:** Build the primary UI surfaces for track performance progress and prevent slow regressions from hiding in polish work., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** baseline render metrics; drag latency benchmark; search latency benchmark.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/canvas/BenchmarkModel.h
- /Users/ryanrentfro/code/markamp/scripts/benchmark_update_baseline.sh

### P10-T065: Benchmarks And Baselines Pointer Interaction Flow
**Task Title:** Benchmarks And Baselines Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for benchmarks and baselines.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for track performance progress and prevent slow regressions from hiding in polish work.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** baseline render metrics; drag latency benchmark; search latency benchmark.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/canvas/BenchmarkModel.h
- /Users/ryanrentfro/code/markamp/scripts/benchmark_update_baseline.sh

### P10-T066: Benchmarks And Baselines Keyboard Gesture And Shortcut Flow
**Task Title:** Benchmarks And Baselines Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for benchmarks and baselines.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for track performance progress and prevent slow regressions from hiding in polish work.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** baseline render metrics; drag latency benchmark; search latency benchmark.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/canvas/BenchmarkModel.h
- /Users/ryanrentfro/code/markamp/scripts/benchmark_update_baseline.sh

### P10-T067: Benchmarks And Baselines Visual Feedback And Rendering Polish
**Task Title:** Benchmarks And Baselines Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make benchmarks and baselines legible in use.
**Description:** Implement the visible feedback for track performance progress and prevent slow regressions from hiding in polish work.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** baseline render metrics; drag latency benchmark; search latency benchmark.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/canvas/BenchmarkModel.h
- /Users/ryanrentfro/code/markamp/scripts/benchmark_update_baseline.sh

### P10-T068: Benchmarks And Baselines Persistence Preferences And Serialization
**Task Title:** Benchmarks And Baselines Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by benchmarks and baselines.
**Description:** Identify what parts of track performance progress and prevent slow regressions from hiding in polish work. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** baseline render metrics; drag latency benchmark; search latency benchmark.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/canvas/BenchmarkModel.h
- /Users/ryanrentfro/code/markamp/scripts/benchmark_update_baseline.sh

### P10-T069: Benchmarks And Baselines Safeguards Telemetry And Recovery
**Task Title:** Benchmarks And Baselines Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for benchmarks and baselines.
**Description:** Instrument track performance progress and prevent slow regressions from hiding in polish work. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** baseline render metrics; drag latency benchmark; search latency benchmark.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/canvas/BenchmarkModel.h
- /Users/ryanrentfro/code/markamp/scripts/benchmark_update_baseline.sh

### P10-T070: Benchmarks And Baselines Tests Documentation And Rollout Gate
**Task Title:** Benchmarks And Baselines Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship benchmarks and baselines.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for track performance progress and prevent slow regressions from hiding in polish work.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** baseline render metrics; drag latency benchmark; search latency benchmark.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/canvas/BenchmarkModel.h
- /Users/ryanrentfro/code/markamp/scripts/benchmark_update_baseline.sh

## Workstream 08: Release Gates
Define objective exit criteria before promoting the new canvas UX broadly.

### P10-T071: Release Gates UX Parity Audit
**Task Title:** Release Gates UX Parity Audit
**Definition:** Define the implementation contract for release gates by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around define objective exit criteria before promoting the new canvas ux broadly.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** all critical e2e pass; a11y checklist complete; no P0 parity gaps.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/scripts/smoke_test.sh
- /Users/ryanrentfro/code/markamp/scripts/run_chaos_tests.sh

### P10-T072: Release Gates Domain Model And State Contract
**Task Title:** Release Gates Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support release gates.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around define objective exit criteria before promoting the new canvas ux broadly.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** all critical e2e pass; a11y checklist complete; no P0 parity gaps.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/scripts/smoke_test.sh
- /Users/ryanrentfro/code/markamp/scripts/run_chaos_tests.sh

### P10-T073: Release Gates Commands Events And Context Keys
**Task Title:** Release Gates Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for release gates.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make define objective exit criteria before promoting the new canvas ux broadly. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** all critical e2e pass; a11y checklist complete; no P0 parity gaps.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/scripts/smoke_test.sh
- /Users/ryanrentfro/code/markamp/scripts/run_chaos_tests.sh

### P10-T074: Release Gates Workspace Surface And Controls
**Task Title:** Release Gates Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose release gates in the main canvas workspace.
**Description:** Build the primary UI surfaces for define objective exit criteria before promoting the new canvas ux broadly., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** all critical e2e pass; a11y checklist complete; no P0 parity gaps.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/scripts/smoke_test.sh
- /Users/ryanrentfro/code/markamp/scripts/run_chaos_tests.sh

### P10-T075: Release Gates Pointer Interaction Flow
**Task Title:** Release Gates Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for release gates.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for define objective exit criteria before promoting the new canvas ux broadly.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** all critical e2e pass; a11y checklist complete; no P0 parity gaps.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/scripts/smoke_test.sh
- /Users/ryanrentfro/code/markamp/scripts/run_chaos_tests.sh

### P10-T076: Release Gates Keyboard Gesture And Shortcut Flow
**Task Title:** Release Gates Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for release gates.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for define objective exit criteria before promoting the new canvas ux broadly.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** all critical e2e pass; a11y checklist complete; no P0 parity gaps.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/scripts/smoke_test.sh
- /Users/ryanrentfro/code/markamp/scripts/run_chaos_tests.sh

### P10-T077: Release Gates Visual Feedback And Rendering Polish
**Task Title:** Release Gates Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make release gates legible in use.
**Description:** Implement the visible feedback for define objective exit criteria before promoting the new canvas ux broadly.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** all critical e2e pass; a11y checklist complete; no P0 parity gaps.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/scripts/smoke_test.sh
- /Users/ryanrentfro/code/markamp/scripts/run_chaos_tests.sh

### P10-T078: Release Gates Persistence Preferences And Serialization
**Task Title:** Release Gates Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by release gates.
**Description:** Identify what parts of define objective exit criteria before promoting the new canvas ux broadly. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** all critical e2e pass; a11y checklist complete; no P0 parity gaps.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/scripts/smoke_test.sh
- /Users/ryanrentfro/code/markamp/scripts/run_chaos_tests.sh

### P10-T079: Release Gates Safeguards Telemetry And Recovery
**Task Title:** Release Gates Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for release gates.
**Description:** Instrument define objective exit criteria before promoting the new canvas ux broadly. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** all critical e2e pass; a11y checklist complete; no P0 parity gaps.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/scripts/smoke_test.sh
- /Users/ryanrentfro/code/markamp/scripts/run_chaos_tests.sh

### P10-T080: Release Gates Tests Documentation And Rollout Gate
**Task Title:** Release Gates Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship release gates.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for define objective exit criteria before promoting the new canvas ux broadly.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** all critical e2e pass; a11y checklist complete; no P0 parity gaps.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/scripts/smoke_test.sh
- /Users/ryanrentfro/code/markamp/scripts/run_chaos_tests.sh

## Workstream 09: Beta Feedback Loops
Make user feedback actionable and tied to board states that engineers can replay.

### P10-T081: Beta Feedback Loops UX Parity Audit
**Task Title:** Beta Feedback Loops UX Parity Audit
**Definition:** Define the implementation contract for beta feedback loops by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around make user feedback actionable and tied to board states that engineers can replay.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** feedback with board snapshot; session tag for beta features; in-app issue prompt.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/core/NotificationService.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.h
- /Users/ryanrentfro/code/markamp/src/ui/DialogModel.cpp

### P10-T082: Beta Feedback Loops Domain Model And State Contract
**Task Title:** Beta Feedback Loops Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support beta feedback loops.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around make user feedback actionable and tied to board states that engineers can replay.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** feedback with board snapshot; session tag for beta features; in-app issue prompt.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/core/NotificationService.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.h
- /Users/ryanrentfro/code/markamp/src/ui/DialogModel.cpp

### P10-T083: Beta Feedback Loops Commands Events And Context Keys
**Task Title:** Beta Feedback Loops Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for beta feedback loops.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make make user feedback actionable and tied to board states that engineers can replay. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** feedback with board snapshot; session tag for beta features; in-app issue prompt.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/core/NotificationService.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.h
- /Users/ryanrentfro/code/markamp/src/ui/DialogModel.cpp

### P10-T084: Beta Feedback Loops Workspace Surface And Controls
**Task Title:** Beta Feedback Loops Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose beta feedback loops in the main canvas workspace.
**Description:** Build the primary UI surfaces for make user feedback actionable and tied to board states that engineers can replay., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** feedback with board snapshot; session tag for beta features; in-app issue prompt.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/core/NotificationService.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.h
- /Users/ryanrentfro/code/markamp/src/ui/DialogModel.cpp

### P10-T085: Beta Feedback Loops Pointer Interaction Flow
**Task Title:** Beta Feedback Loops Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for beta feedback loops.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for make user feedback actionable and tied to board states that engineers can replay.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** feedback with board snapshot; session tag for beta features; in-app issue prompt.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/core/NotificationService.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.h
- /Users/ryanrentfro/code/markamp/src/ui/DialogModel.cpp

### P10-T086: Beta Feedback Loops Keyboard Gesture And Shortcut Flow
**Task Title:** Beta Feedback Loops Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for beta feedback loops.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for make user feedback actionable and tied to board states that engineers can replay.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** feedback with board snapshot; session tag for beta features; in-app issue prompt.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/core/NotificationService.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.h
- /Users/ryanrentfro/code/markamp/src/ui/DialogModel.cpp

### P10-T087: Beta Feedback Loops Visual Feedback And Rendering Polish
**Task Title:** Beta Feedback Loops Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make beta feedback loops legible in use.
**Description:** Implement the visible feedback for make user feedback actionable and tied to board states that engineers can replay.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** feedback with board snapshot; session tag for beta features; in-app issue prompt.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/core/NotificationService.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.h
- /Users/ryanrentfro/code/markamp/src/ui/DialogModel.cpp

### P10-T088: Beta Feedback Loops Persistence Preferences And Serialization
**Task Title:** Beta Feedback Loops Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by beta feedback loops.
**Description:** Identify what parts of make user feedback actionable and tied to board states that engineers can replay. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** feedback with board snapshot; session tag for beta features; in-app issue prompt.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/core/NotificationService.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.h
- /Users/ryanrentfro/code/markamp/src/ui/DialogModel.cpp

### P10-T089: Beta Feedback Loops Safeguards Telemetry And Recovery
**Task Title:** Beta Feedback Loops Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for beta feedback loops.
**Description:** Instrument make user feedback actionable and tied to board states that engineers can replay. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** feedback with board snapshot; session tag for beta features; in-app issue prompt.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/core/NotificationService.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.h
- /Users/ryanrentfro/code/markamp/src/ui/DialogModel.cpp

### P10-T090: Beta Feedback Loops Tests Documentation And Rollout Gate
**Task Title:** Beta Feedback Loops Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship beta feedback loops.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for make user feedback actionable and tied to board states that engineers can replay.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** feedback with board snapshot; session tag for beta features; in-app issue prompt.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/core/NotificationService.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.h
- /Users/ryanrentfro/code/markamp/src/ui/DialogModel.cpp

## Workstream 10: Extension Ecosystem
Give plugins a coherent way to align with the improved canvas UX.

### P10-T091: Extension Ecosystem UX Parity Audit
**Task Title:** Extension Ecosystem UX Parity Audit
**Definition:** Define the implementation contract for extension ecosystem by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around give plugins a coherent way to align with the improved canvas ux.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** extension contributes tool; extension adds inspector panel; extension reacts to selection.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/core/PluginManager.h
- /Users/ryanrentfro/code/markamp/src/core/ContributionValidator.h
- /Users/ryanrentfro/code/markamp/src/core/ContextKeyService.cpp

### P10-T092: Extension Ecosystem Domain Model And State Contract
**Task Title:** Extension Ecosystem Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support extension ecosystem.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around give plugins a coherent way to align with the improved canvas ux.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** extension contributes tool; extension adds inspector panel; extension reacts to selection.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/core/PluginManager.h
- /Users/ryanrentfro/code/markamp/src/core/ContributionValidator.h
- /Users/ryanrentfro/code/markamp/src/core/ContextKeyService.cpp

### P10-T093: Extension Ecosystem Commands Events And Context Keys
**Task Title:** Extension Ecosystem Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for extension ecosystem.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make give plugins a coherent way to align with the improved canvas ux. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** extension contributes tool; extension adds inspector panel; extension reacts to selection.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/core/PluginManager.h
- /Users/ryanrentfro/code/markamp/src/core/ContributionValidator.h
- /Users/ryanrentfro/code/markamp/src/core/ContextKeyService.cpp

### P10-T094: Extension Ecosystem Workspace Surface And Controls
**Task Title:** Extension Ecosystem Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose extension ecosystem in the main canvas workspace.
**Description:** Build the primary UI surfaces for give plugins a coherent way to align with the improved canvas ux., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** extension contributes tool; extension adds inspector panel; extension reacts to selection.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/core/PluginManager.h
- /Users/ryanrentfro/code/markamp/src/core/ContributionValidator.h
- /Users/ryanrentfro/code/markamp/src/core/ContextKeyService.cpp

### P10-T095: Extension Ecosystem Pointer Interaction Flow
**Task Title:** Extension Ecosystem Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for extension ecosystem.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for give plugins a coherent way to align with the improved canvas ux.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** extension contributes tool; extension adds inspector panel; extension reacts to selection.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/core/PluginManager.h
- /Users/ryanrentfro/code/markamp/src/core/ContributionValidator.h
- /Users/ryanrentfro/code/markamp/src/core/ContextKeyService.cpp

### P10-T096: Extension Ecosystem Keyboard Gesture And Shortcut Flow
**Task Title:** Extension Ecosystem Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for extension ecosystem.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for give plugins a coherent way to align with the improved canvas ux.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** extension contributes tool; extension adds inspector panel; extension reacts to selection.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/core/PluginManager.h
- /Users/ryanrentfro/code/markamp/src/core/ContributionValidator.h
- /Users/ryanrentfro/code/markamp/src/core/ContextKeyService.cpp

### P10-T097: Extension Ecosystem Visual Feedback And Rendering Polish
**Task Title:** Extension Ecosystem Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make extension ecosystem legible in use.
**Description:** Implement the visible feedback for give plugins a coherent way to align with the improved canvas ux.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** extension contributes tool; extension adds inspector panel; extension reacts to selection.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/core/PluginManager.h
- /Users/ryanrentfro/code/markamp/src/core/ContributionValidator.h
- /Users/ryanrentfro/code/markamp/src/core/ContextKeyService.cpp

### P10-T098: Extension Ecosystem Persistence Preferences And Serialization
**Task Title:** Extension Ecosystem Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by extension ecosystem.
**Description:** Identify what parts of give plugins a coherent way to align with the improved canvas ux. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** extension contributes tool; extension adds inspector panel; extension reacts to selection.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/core/PluginManager.h
- /Users/ryanrentfro/code/markamp/src/core/ContributionValidator.h
- /Users/ryanrentfro/code/markamp/src/core/ContextKeyService.cpp

### P10-T099: Extension Ecosystem Safeguards Telemetry And Recovery
**Task Title:** Extension Ecosystem Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for extension ecosystem.
**Description:** Instrument give plugins a coherent way to align with the improved canvas ux. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** extension contributes tool; extension adds inspector panel; extension reacts to selection.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/core/PluginManager.h
- /Users/ryanrentfro/code/markamp/src/core/ContributionValidator.h
- /Users/ryanrentfro/code/markamp/src/core/ContextKeyService.cpp

### P10-T100: Extension Ecosystem Tests Documentation And Rollout Gate
**Task Title:** Extension Ecosystem Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship extension ecosystem.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for give plugins a coherent way to align with the improved canvas ux.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** extension contributes tool; extension adds inspector panel; extension reacts to selection.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/core/PluginManager.h
- /Users/ryanrentfro/code/markamp/src/core/ContributionValidator.h
- /Users/ryanrentfro/code/markamp/src/core/ContextKeyService.cpp

## Workstream 11: Advanced Drawing Polish
Finish rough edges in pen, shape, and text authoring so the surface feels production-grade.

### P10-T101: Advanced Drawing Polish UX Parity Audit
**Task Title:** Advanced Drawing Polish UX Parity Audit
**Definition:** Define the implementation contract for advanced drawing polish by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around finish rough edges in pen, shape, and text authoring so the surface feels production-grade.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** stroke smoothing tune; text editing handoff polish; shape preview polish.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.h
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.h

### P10-T102: Advanced Drawing Polish Domain Model And State Contract
**Task Title:** Advanced Drawing Polish Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support advanced drawing polish.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around finish rough edges in pen, shape, and text authoring so the surface feels production-grade.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** stroke smoothing tune; text editing handoff polish; shape preview polish.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.h
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.h

### P10-T103: Advanced Drawing Polish Commands Events And Context Keys
**Task Title:** Advanced Drawing Polish Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for advanced drawing polish.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make finish rough edges in pen, shape, and text authoring so the surface feels production-grade. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** stroke smoothing tune; text editing handoff polish; shape preview polish.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.h
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.h

### P10-T104: Advanced Drawing Polish Workspace Surface And Controls
**Task Title:** Advanced Drawing Polish Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose advanced drawing polish in the main canvas workspace.
**Description:** Build the primary UI surfaces for finish rough edges in pen, shape, and text authoring so the surface feels production-grade., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** stroke smoothing tune; text editing handoff polish; shape preview polish.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.h
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.h

### P10-T105: Advanced Drawing Polish Pointer Interaction Flow
**Task Title:** Advanced Drawing Polish Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for advanced drawing polish.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for finish rough edges in pen, shape, and text authoring so the surface feels production-grade.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** stroke smoothing tune; text editing handoff polish; shape preview polish.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.h
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.h

### P10-T106: Advanced Drawing Polish Keyboard Gesture And Shortcut Flow
**Task Title:** Advanced Drawing Polish Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for advanced drawing polish.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for finish rough edges in pen, shape, and text authoring so the surface feels production-grade.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** stroke smoothing tune; text editing handoff polish; shape preview polish.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.h
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.h

### P10-T107: Advanced Drawing Polish Visual Feedback And Rendering Polish
**Task Title:** Advanced Drawing Polish Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make advanced drawing polish legible in use.
**Description:** Implement the visible feedback for finish rough edges in pen, shape, and text authoring so the surface feels production-grade.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** stroke smoothing tune; text editing handoff polish; shape preview polish.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.h
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.h

### P10-T108: Advanced Drawing Polish Persistence Preferences And Serialization
**Task Title:** Advanced Drawing Polish Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by advanced drawing polish.
**Description:** Identify what parts of finish rough edges in pen, shape, and text authoring so the surface feels production-grade. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** stroke smoothing tune; text editing handoff polish; shape preview polish.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.h
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.h

### P10-T109: Advanced Drawing Polish Safeguards Telemetry And Recovery
**Task Title:** Advanced Drawing Polish Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for advanced drawing polish.
**Description:** Instrument finish rough edges in pen, shape, and text authoring so the surface feels production-grade. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** stroke smoothing tune; text editing handoff polish; shape preview polish.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.h
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.h

### P10-T110: Advanced Drawing Polish Tests Documentation And Rollout Gate
**Task Title:** Advanced Drawing Polish Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship advanced drawing polish.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for finish rough edges in pen, shape, and text authoring so the surface feels production-grade.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** stroke smoothing tune; text editing handoff polish; shape preview polish.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.h
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.h

## Workstream 12: Advanced Layout Polish
Refine transform and alignment behavior under complex board conditions.

### P10-T111: Advanced Layout Polish UX Parity Audit
**Task Title:** Advanced Layout Polish UX Parity Audit
**Definition:** Define the implementation contract for advanced layout polish by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around refine transform and alignment behavior under complex board conditions.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** smarter guide filtering; less jumpy snap; cleaner group resize.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/canvas/AlignmentGuides.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SnapEngine.h
- /Users/ryanrentfro/code/markamp/src/canvas/GroupingService.cpp

### P10-T112: Advanced Layout Polish Domain Model And State Contract
**Task Title:** Advanced Layout Polish Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support advanced layout polish.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around refine transform and alignment behavior under complex board conditions.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** smarter guide filtering; less jumpy snap; cleaner group resize.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/canvas/AlignmentGuides.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SnapEngine.h
- /Users/ryanrentfro/code/markamp/src/canvas/GroupingService.cpp

### P10-T113: Advanced Layout Polish Commands Events And Context Keys
**Task Title:** Advanced Layout Polish Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for advanced layout polish.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make refine transform and alignment behavior under complex board conditions. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** smarter guide filtering; less jumpy snap; cleaner group resize.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/canvas/AlignmentGuides.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SnapEngine.h
- /Users/ryanrentfro/code/markamp/src/canvas/GroupingService.cpp

### P10-T114: Advanced Layout Polish Workspace Surface And Controls
**Task Title:** Advanced Layout Polish Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose advanced layout polish in the main canvas workspace.
**Description:** Build the primary UI surfaces for refine transform and alignment behavior under complex board conditions., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** smarter guide filtering; less jumpy snap; cleaner group resize.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/canvas/AlignmentGuides.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SnapEngine.h
- /Users/ryanrentfro/code/markamp/src/canvas/GroupingService.cpp

### P10-T115: Advanced Layout Polish Pointer Interaction Flow
**Task Title:** Advanced Layout Polish Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for advanced layout polish.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for refine transform and alignment behavior under complex board conditions.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** smarter guide filtering; less jumpy snap; cleaner group resize.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/canvas/AlignmentGuides.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SnapEngine.h
- /Users/ryanrentfro/code/markamp/src/canvas/GroupingService.cpp

### P10-T116: Advanced Layout Polish Keyboard Gesture And Shortcut Flow
**Task Title:** Advanced Layout Polish Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for advanced layout polish.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for refine transform and alignment behavior under complex board conditions.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** smarter guide filtering; less jumpy snap; cleaner group resize.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/canvas/AlignmentGuides.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SnapEngine.h
- /Users/ryanrentfro/code/markamp/src/canvas/GroupingService.cpp

### P10-T117: Advanced Layout Polish Visual Feedback And Rendering Polish
**Task Title:** Advanced Layout Polish Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make advanced layout polish legible in use.
**Description:** Implement the visible feedback for refine transform and alignment behavior under complex board conditions.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** smarter guide filtering; less jumpy snap; cleaner group resize.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/canvas/AlignmentGuides.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SnapEngine.h
- /Users/ryanrentfro/code/markamp/src/canvas/GroupingService.cpp

### P10-T118: Advanced Layout Polish Persistence Preferences And Serialization
**Task Title:** Advanced Layout Polish Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by advanced layout polish.
**Description:** Identify what parts of refine transform and alignment behavior under complex board conditions. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** smarter guide filtering; less jumpy snap; cleaner group resize.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/canvas/AlignmentGuides.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SnapEngine.h
- /Users/ryanrentfro/code/markamp/src/canvas/GroupingService.cpp

### P10-T119: Advanced Layout Polish Safeguards Telemetry And Recovery
**Task Title:** Advanced Layout Polish Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for advanced layout polish.
**Description:** Instrument refine transform and alignment behavior under complex board conditions. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** smarter guide filtering; less jumpy snap; cleaner group resize.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/canvas/AlignmentGuides.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SnapEngine.h
- /Users/ryanrentfro/code/markamp/src/canvas/GroupingService.cpp

### P10-T120: Advanced Layout Polish Tests Documentation And Rollout Gate
**Task Title:** Advanced Layout Polish Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship advanced layout polish.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for refine transform and alignment behavior under complex board conditions.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** smarter guide filtering; less jumpy snap; cleaner group resize.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/canvas/AlignmentGuides.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SnapEngine.h
- /Users/ryanrentfro/code/markamp/src/canvas/GroupingService.cpp

## Workstream 13: Advanced Collaboration Polish
Resolve edge frictions in multi-user sessions before broad release.

### P10-T121: Advanced Collaboration Polish UX Parity Audit
**Task Title:** Advanced Collaboration Polish UX Parity Audit
**Definition:** Define the implementation contract for advanced collaboration polish by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around resolve edge frictions in multi-user sessions before broad release.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** less noisy presence; faster comment resolve; clean presenter follow exit.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.h

### P10-T122: Advanced Collaboration Polish Domain Model And State Contract
**Task Title:** Advanced Collaboration Polish Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support advanced collaboration polish.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around resolve edge frictions in multi-user sessions before broad release.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** less noisy presence; faster comment resolve; clean presenter follow exit.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.h

### P10-T123: Advanced Collaboration Polish Commands Events And Context Keys
**Task Title:** Advanced Collaboration Polish Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for advanced collaboration polish.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make resolve edge frictions in multi-user sessions before broad release. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** less noisy presence; faster comment resolve; clean presenter follow exit.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.h

### P10-T124: Advanced Collaboration Polish Workspace Surface And Controls
**Task Title:** Advanced Collaboration Polish Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose advanced collaboration polish in the main canvas workspace.
**Description:** Build the primary UI surfaces for resolve edge frictions in multi-user sessions before broad release., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** less noisy presence; faster comment resolve; clean presenter follow exit.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.h

### P10-T125: Advanced Collaboration Polish Pointer Interaction Flow
**Task Title:** Advanced Collaboration Polish Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for advanced collaboration polish.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for resolve edge frictions in multi-user sessions before broad release.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** less noisy presence; faster comment resolve; clean presenter follow exit.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.h

### P10-T126: Advanced Collaboration Polish Keyboard Gesture And Shortcut Flow
**Task Title:** Advanced Collaboration Polish Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for advanced collaboration polish.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for resolve edge frictions in multi-user sessions before broad release.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** less noisy presence; faster comment resolve; clean presenter follow exit.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.h

### P10-T127: Advanced Collaboration Polish Visual Feedback And Rendering Polish
**Task Title:** Advanced Collaboration Polish Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make advanced collaboration polish legible in use.
**Description:** Implement the visible feedback for resolve edge frictions in multi-user sessions before broad release.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** less noisy presence; faster comment resolve; clean presenter follow exit.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.h

### P10-T128: Advanced Collaboration Polish Persistence Preferences And Serialization
**Task Title:** Advanced Collaboration Polish Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by advanced collaboration polish.
**Description:** Identify what parts of resolve edge frictions in multi-user sessions before broad release. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** less noisy presence; faster comment resolve; clean presenter follow exit.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.h

### P10-T129: Advanced Collaboration Polish Safeguards Telemetry And Recovery
**Task Title:** Advanced Collaboration Polish Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for advanced collaboration polish.
**Description:** Instrument resolve edge frictions in multi-user sessions before broad release. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** less noisy presence; faster comment resolve; clean presenter follow exit.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.h

### P10-T130: Advanced Collaboration Polish Tests Documentation And Rollout Gate
**Task Title:** Advanced Collaboration Polish Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship advanced collaboration polish.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for resolve edge frictions in multi-user sessions before broad release.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** less noisy presence; faster comment resolve; clean presenter follow exit.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.h

## Workstream 14: Advanced Navigation Polish
Make large-board travel feel calm, not frantic.

### P10-T131: Advanced Navigation Polish UX Parity Audit
**Task Title:** Advanced Navigation Polish UX Parity Audit
**Definition:** Define the implementation contract for advanced navigation polish by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around make large-board travel feel calm, not frantic.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** less minimap jitter; search focus settle; saved view transition.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.h
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.h

### P10-T132: Advanced Navigation Polish Domain Model And State Contract
**Task Title:** Advanced Navigation Polish Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support advanced navigation polish.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around make large-board travel feel calm, not frantic.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** less minimap jitter; search focus settle; saved view transition.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.h
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.h

### P10-T133: Advanced Navigation Polish Commands Events And Context Keys
**Task Title:** Advanced Navigation Polish Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for advanced navigation polish.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make make large-board travel feel calm, not frantic. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** less minimap jitter; search focus settle; saved view transition.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.h
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.h

### P10-T134: Advanced Navigation Polish Workspace Surface And Controls
**Task Title:** Advanced Navigation Polish Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose advanced navigation polish in the main canvas workspace.
**Description:** Build the primary UI surfaces for make large-board travel feel calm, not frantic., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** less minimap jitter; search focus settle; saved view transition.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.h
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.h

### P10-T135: Advanced Navigation Polish Pointer Interaction Flow
**Task Title:** Advanced Navigation Polish Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for advanced navigation polish.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for make large-board travel feel calm, not frantic.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** less minimap jitter; search focus settle; saved view transition.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.h
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.h

### P10-T136: Advanced Navigation Polish Keyboard Gesture And Shortcut Flow
**Task Title:** Advanced Navigation Polish Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for advanced navigation polish.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for make large-board travel feel calm, not frantic.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** less minimap jitter; search focus settle; saved view transition.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.h
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.h

### P10-T137: Advanced Navigation Polish Visual Feedback And Rendering Polish
**Task Title:** Advanced Navigation Polish Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make advanced navigation polish legible in use.
**Description:** Implement the visible feedback for make large-board travel feel calm, not frantic.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** less minimap jitter; search focus settle; saved view transition.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.h
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.h

### P10-T138: Advanced Navigation Polish Persistence Preferences And Serialization
**Task Title:** Advanced Navigation Polish Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by advanced navigation polish.
**Description:** Identify what parts of make large-board travel feel calm, not frantic. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** less minimap jitter; search focus settle; saved view transition.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.h
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.h

### P10-T139: Advanced Navigation Polish Safeguards Telemetry And Recovery
**Task Title:** Advanced Navigation Polish Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for advanced navigation polish.
**Description:** Instrument make large-board travel feel calm, not frantic. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** less minimap jitter; search focus settle; saved view transition.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.h
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.h

### P10-T140: Advanced Navigation Polish Tests Documentation And Rollout Gate
**Task Title:** Advanced Navigation Polish Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship advanced navigation polish.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for make large-board travel feel calm, not frantic.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** less minimap jitter; search focus settle; saved view transition.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.h
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.h

## Workstream 15: Advanced Export Polish
Verify the last mile from board editing to artifact production.

### P10-T141: Advanced Export Polish UX Parity Audit
**Task Title:** Advanced Export Polish UX Parity Audit
**Definition:** Define the implementation contract for advanced export polish by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around verify the last mile from board editing to artifact production.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** crisp PNG export; print-safe PDF; shareable HTML bundle.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasExportService.h
- /Users/ryanrentfro/code/markamp/src/core/PdfExporter.cpp
- /Users/ryanrentfro/code/markamp/src/core/HtmlExporter.h

### P10-T142: Advanced Export Polish Domain Model And State Contract
**Task Title:** Advanced Export Polish Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support advanced export polish.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around verify the last mile from board editing to artifact production.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** crisp PNG export; print-safe PDF; shareable HTML bundle.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasExportService.h
- /Users/ryanrentfro/code/markamp/src/core/PdfExporter.cpp
- /Users/ryanrentfro/code/markamp/src/core/HtmlExporter.h

### P10-T143: Advanced Export Polish Commands Events And Context Keys
**Task Title:** Advanced Export Polish Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for advanced export polish.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make verify the last mile from board editing to artifact production. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** crisp PNG export; print-safe PDF; shareable HTML bundle.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasExportService.h
- /Users/ryanrentfro/code/markamp/src/core/PdfExporter.cpp
- /Users/ryanrentfro/code/markamp/src/core/HtmlExporter.h

### P10-T144: Advanced Export Polish Workspace Surface And Controls
**Task Title:** Advanced Export Polish Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose advanced export polish in the main canvas workspace.
**Description:** Build the primary UI surfaces for verify the last mile from board editing to artifact production., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** crisp PNG export; print-safe PDF; shareable HTML bundle.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasExportService.h
- /Users/ryanrentfro/code/markamp/src/core/PdfExporter.cpp
- /Users/ryanrentfro/code/markamp/src/core/HtmlExporter.h

### P10-T145: Advanced Export Polish Pointer Interaction Flow
**Task Title:** Advanced Export Polish Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for advanced export polish.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for verify the last mile from board editing to artifact production.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** crisp PNG export; print-safe PDF; shareable HTML bundle.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasExportService.h
- /Users/ryanrentfro/code/markamp/src/core/PdfExporter.cpp
- /Users/ryanrentfro/code/markamp/src/core/HtmlExporter.h

### P10-T146: Advanced Export Polish Keyboard Gesture And Shortcut Flow
**Task Title:** Advanced Export Polish Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for advanced export polish.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for verify the last mile from board editing to artifact production.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** crisp PNG export; print-safe PDF; shareable HTML bundle.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasExportService.h
- /Users/ryanrentfro/code/markamp/src/core/PdfExporter.cpp
- /Users/ryanrentfro/code/markamp/src/core/HtmlExporter.h

### P10-T147: Advanced Export Polish Visual Feedback And Rendering Polish
**Task Title:** Advanced Export Polish Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make advanced export polish legible in use.
**Description:** Implement the visible feedback for verify the last mile from board editing to artifact production.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** crisp PNG export; print-safe PDF; shareable HTML bundle.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasExportService.h
- /Users/ryanrentfro/code/markamp/src/core/PdfExporter.cpp
- /Users/ryanrentfro/code/markamp/src/core/HtmlExporter.h

### P10-T148: Advanced Export Polish Persistence Preferences And Serialization
**Task Title:** Advanced Export Polish Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by advanced export polish.
**Description:** Identify what parts of verify the last mile from board editing to artifact production. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** crisp PNG export; print-safe PDF; shareable HTML bundle.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasExportService.h
- /Users/ryanrentfro/code/markamp/src/core/PdfExporter.cpp
- /Users/ryanrentfro/code/markamp/src/core/HtmlExporter.h

### P10-T149: Advanced Export Polish Safeguards Telemetry And Recovery
**Task Title:** Advanced Export Polish Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for advanced export polish.
**Description:** Instrument verify the last mile from board editing to artifact production. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** crisp PNG export; print-safe PDF; shareable HTML bundle.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasExportService.h
- /Users/ryanrentfro/code/markamp/src/core/PdfExporter.cpp
- /Users/ryanrentfro/code/markamp/src/core/HtmlExporter.h

### P10-T150: Advanced Export Polish Tests Documentation And Rollout Gate
**Task Title:** Advanced Export Polish Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship advanced export polish.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for verify the last mile from board editing to artifact production.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** crisp PNG export; print-safe PDF; shareable HTML bundle.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasExportService.h
- /Users/ryanrentfro/code/markamp/src/core/PdfExporter.cpp
- /Users/ryanrentfro/code/markamp/src/core/HtmlExporter.h

## Workstream 16: Enterprise And Admin Fit
Cover governance and admin concerns needed for larger deployments.

### P10-T151: Enterprise And Admin Fit UX Parity Audit
**Task Title:** Enterprise And Admin Fit UX Parity Audit
**Definition:** Define the implementation contract for enterprise and admin fit by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around cover governance and admin concerns needed for larger deployments.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** policy-controlled embeds; admin feature flags; audit-ready activity.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/core/RuntimePolicy.h
- /Users/ryanrentfro/code/markamp/src/core/SecurityAuditLog.cpp
- /Users/ryanrentfro/code/markamp/src/core/FeatureRegistry.h

### P10-T152: Enterprise And Admin Fit Domain Model And State Contract
**Task Title:** Enterprise And Admin Fit Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support enterprise and admin fit.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around cover governance and admin concerns needed for larger deployments.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** policy-controlled embeds; admin feature flags; audit-ready activity.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/core/RuntimePolicy.h
- /Users/ryanrentfro/code/markamp/src/core/SecurityAuditLog.cpp
- /Users/ryanrentfro/code/markamp/src/core/FeatureRegistry.h

### P10-T153: Enterprise And Admin Fit Commands Events And Context Keys
**Task Title:** Enterprise And Admin Fit Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for enterprise and admin fit.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make cover governance and admin concerns needed for larger deployments. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** policy-controlled embeds; admin feature flags; audit-ready activity.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/core/RuntimePolicy.h
- /Users/ryanrentfro/code/markamp/src/core/SecurityAuditLog.cpp
- /Users/ryanrentfro/code/markamp/src/core/FeatureRegistry.h

### P10-T154: Enterprise And Admin Fit Workspace Surface And Controls
**Task Title:** Enterprise And Admin Fit Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose enterprise and admin fit in the main canvas workspace.
**Description:** Build the primary UI surfaces for cover governance and admin concerns needed for larger deployments., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** policy-controlled embeds; admin feature flags; audit-ready activity.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/core/RuntimePolicy.h
- /Users/ryanrentfro/code/markamp/src/core/SecurityAuditLog.cpp
- /Users/ryanrentfro/code/markamp/src/core/FeatureRegistry.h

### P10-T155: Enterprise And Admin Fit Pointer Interaction Flow
**Task Title:** Enterprise And Admin Fit Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for enterprise and admin fit.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for cover governance and admin concerns needed for larger deployments.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** policy-controlled embeds; admin feature flags; audit-ready activity.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/core/RuntimePolicy.h
- /Users/ryanrentfro/code/markamp/src/core/SecurityAuditLog.cpp
- /Users/ryanrentfro/code/markamp/src/core/FeatureRegistry.h

### P10-T156: Enterprise And Admin Fit Keyboard Gesture And Shortcut Flow
**Task Title:** Enterprise And Admin Fit Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for enterprise and admin fit.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for cover governance and admin concerns needed for larger deployments.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** policy-controlled embeds; admin feature flags; audit-ready activity.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/core/RuntimePolicy.h
- /Users/ryanrentfro/code/markamp/src/core/SecurityAuditLog.cpp
- /Users/ryanrentfro/code/markamp/src/core/FeatureRegistry.h

### P10-T157: Enterprise And Admin Fit Visual Feedback And Rendering Polish
**Task Title:** Enterprise And Admin Fit Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make enterprise and admin fit legible in use.
**Description:** Implement the visible feedback for cover governance and admin concerns needed for larger deployments.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** policy-controlled embeds; admin feature flags; audit-ready activity.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/core/RuntimePolicy.h
- /Users/ryanrentfro/code/markamp/src/core/SecurityAuditLog.cpp
- /Users/ryanrentfro/code/markamp/src/core/FeatureRegistry.h

### P10-T158: Enterprise And Admin Fit Persistence Preferences And Serialization
**Task Title:** Enterprise And Admin Fit Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by enterprise and admin fit.
**Description:** Identify what parts of cover governance and admin concerns needed for larger deployments. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** policy-controlled embeds; admin feature flags; audit-ready activity.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/core/RuntimePolicy.h
- /Users/ryanrentfro/code/markamp/src/core/SecurityAuditLog.cpp
- /Users/ryanrentfro/code/markamp/src/core/FeatureRegistry.h

### P10-T159: Enterprise And Admin Fit Safeguards Telemetry And Recovery
**Task Title:** Enterprise And Admin Fit Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for enterprise and admin fit.
**Description:** Instrument cover governance and admin concerns needed for larger deployments. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** policy-controlled embeds; admin feature flags; audit-ready activity.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/core/RuntimePolicy.h
- /Users/ryanrentfro/code/markamp/src/core/SecurityAuditLog.cpp
- /Users/ryanrentfro/code/markamp/src/core/FeatureRegistry.h

### P10-T160: Enterprise And Admin Fit Tests Documentation And Rollout Gate
**Task Title:** Enterprise And Admin Fit Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship enterprise and admin fit.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for cover governance and admin concerns needed for larger deployments.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** policy-controlled embeds; admin feature flags; audit-ready activity.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/core/RuntimePolicy.h
- /Users/ryanrentfro/code/markamp/src/core/SecurityAuditLog.cpp
- /Users/ryanrentfro/code/markamp/src/core/FeatureRegistry.h

## Workstream 17: Quality Backlog Triage
Turn defects found during rollout into a prioritized, bounded program.

### P10-T161: Quality Backlog Triage UX Parity Audit
**Task Title:** Quality Backlog Triage UX Parity Audit
**Definition:** Define the implementation contract for quality backlog triage by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around turn defects found during rollout into a prioritized, bounded program.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** severity rubric; owner assignment; cut vs defer call.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/canvas/AuditModel.h
- /Users/ryanrentfro/code/markamp/src/core/ActivityFeed.h
- /Users/ryanrentfro/code/markamp/src/ui/ChangelogPanel.h

### P10-T162: Quality Backlog Triage Domain Model And State Contract
**Task Title:** Quality Backlog Triage Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support quality backlog triage.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around turn defects found during rollout into a prioritized, bounded program.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** severity rubric; owner assignment; cut vs defer call.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/canvas/AuditModel.h
- /Users/ryanrentfro/code/markamp/src/core/ActivityFeed.h
- /Users/ryanrentfro/code/markamp/src/ui/ChangelogPanel.h

### P10-T163: Quality Backlog Triage Commands Events And Context Keys
**Task Title:** Quality Backlog Triage Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for quality backlog triage.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make turn defects found during rollout into a prioritized, bounded program. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** severity rubric; owner assignment; cut vs defer call.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/canvas/AuditModel.h
- /Users/ryanrentfro/code/markamp/src/core/ActivityFeed.h
- /Users/ryanrentfro/code/markamp/src/ui/ChangelogPanel.h

### P10-T164: Quality Backlog Triage Workspace Surface And Controls
**Task Title:** Quality Backlog Triage Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose quality backlog triage in the main canvas workspace.
**Description:** Build the primary UI surfaces for turn defects found during rollout into a prioritized, bounded program., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** severity rubric; owner assignment; cut vs defer call.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/canvas/AuditModel.h
- /Users/ryanrentfro/code/markamp/src/core/ActivityFeed.h
- /Users/ryanrentfro/code/markamp/src/ui/ChangelogPanel.h

### P10-T165: Quality Backlog Triage Pointer Interaction Flow
**Task Title:** Quality Backlog Triage Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for quality backlog triage.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for turn defects found during rollout into a prioritized, bounded program.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** severity rubric; owner assignment; cut vs defer call.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/canvas/AuditModel.h
- /Users/ryanrentfro/code/markamp/src/core/ActivityFeed.h
- /Users/ryanrentfro/code/markamp/src/ui/ChangelogPanel.h

### P10-T166: Quality Backlog Triage Keyboard Gesture And Shortcut Flow
**Task Title:** Quality Backlog Triage Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for quality backlog triage.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for turn defects found during rollout into a prioritized, bounded program.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** severity rubric; owner assignment; cut vs defer call.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/canvas/AuditModel.h
- /Users/ryanrentfro/code/markamp/src/core/ActivityFeed.h
- /Users/ryanrentfro/code/markamp/src/ui/ChangelogPanel.h

### P10-T167: Quality Backlog Triage Visual Feedback And Rendering Polish
**Task Title:** Quality Backlog Triage Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make quality backlog triage legible in use.
**Description:** Implement the visible feedback for turn defects found during rollout into a prioritized, bounded program.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** severity rubric; owner assignment; cut vs defer call.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/canvas/AuditModel.h
- /Users/ryanrentfro/code/markamp/src/core/ActivityFeed.h
- /Users/ryanrentfro/code/markamp/src/ui/ChangelogPanel.h

### P10-T168: Quality Backlog Triage Persistence Preferences And Serialization
**Task Title:** Quality Backlog Triage Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by quality backlog triage.
**Description:** Identify what parts of turn defects found during rollout into a prioritized, bounded program. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** severity rubric; owner assignment; cut vs defer call.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/canvas/AuditModel.h
- /Users/ryanrentfro/code/markamp/src/core/ActivityFeed.h
- /Users/ryanrentfro/code/markamp/src/ui/ChangelogPanel.h

### P10-T169: Quality Backlog Triage Safeguards Telemetry And Recovery
**Task Title:** Quality Backlog Triage Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for quality backlog triage.
**Description:** Instrument turn defects found during rollout into a prioritized, bounded program. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** severity rubric; owner assignment; cut vs defer call.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/canvas/AuditModel.h
- /Users/ryanrentfro/code/markamp/src/core/ActivityFeed.h
- /Users/ryanrentfro/code/markamp/src/ui/ChangelogPanel.h

### P10-T170: Quality Backlog Triage Tests Documentation And Rollout Gate
**Task Title:** Quality Backlog Triage Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship quality backlog triage.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for turn defects found during rollout into a prioritized, bounded program.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** severity rubric; owner assignment; cut vs defer call.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/canvas/AuditModel.h
- /Users/ryanrentfro/code/markamp/src/core/ActivityFeed.h
- /Users/ryanrentfro/code/markamp/src/ui/ChangelogPanel.h

## Workstream 18: Adoption Metrics
Measure whether the upgraded canvas is actually being used more effectively.

### P10-T171: Adoption Metrics UX Parity Audit
**Task Title:** Adoption Metrics UX Parity Audit
**Definition:** Define the implementation contract for adoption metrics by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around measure whether the upgraded canvas is actually being used more effectively.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** tool adoption; board completion rate; export usage after session.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/core/ActivityTimeline.cpp
- /Users/ryanrentfro/code/markamp/src/core/AppState.cpp

### P10-T172: Adoption Metrics Domain Model And State Contract
**Task Title:** Adoption Metrics Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support adoption metrics.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around measure whether the upgraded canvas is actually being used more effectively.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** tool adoption; board completion rate; export usage after session.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/core/ActivityTimeline.cpp
- /Users/ryanrentfro/code/markamp/src/core/AppState.cpp

### P10-T173: Adoption Metrics Commands Events And Context Keys
**Task Title:** Adoption Metrics Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for adoption metrics.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make measure whether the upgraded canvas is actually being used more effectively. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** tool adoption; board completion rate; export usage after session.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/core/ActivityTimeline.cpp
- /Users/ryanrentfro/code/markamp/src/core/AppState.cpp

### P10-T174: Adoption Metrics Workspace Surface And Controls
**Task Title:** Adoption Metrics Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose adoption metrics in the main canvas workspace.
**Description:** Build the primary UI surfaces for measure whether the upgraded canvas is actually being used more effectively., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** tool adoption; board completion rate; export usage after session.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/core/ActivityTimeline.cpp
- /Users/ryanrentfro/code/markamp/src/core/AppState.cpp

### P10-T175: Adoption Metrics Pointer Interaction Flow
**Task Title:** Adoption Metrics Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for adoption metrics.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for measure whether the upgraded canvas is actually being used more effectively.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** tool adoption; board completion rate; export usage after session.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/core/ActivityTimeline.cpp
- /Users/ryanrentfro/code/markamp/src/core/AppState.cpp

### P10-T176: Adoption Metrics Keyboard Gesture And Shortcut Flow
**Task Title:** Adoption Metrics Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for adoption metrics.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for measure whether the upgraded canvas is actually being used more effectively.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** tool adoption; board completion rate; export usage after session.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/core/ActivityTimeline.cpp
- /Users/ryanrentfro/code/markamp/src/core/AppState.cpp

### P10-T177: Adoption Metrics Visual Feedback And Rendering Polish
**Task Title:** Adoption Metrics Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make adoption metrics legible in use.
**Description:** Implement the visible feedback for measure whether the upgraded canvas is actually being used more effectively.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** tool adoption; board completion rate; export usage after session.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/core/ActivityTimeline.cpp
- /Users/ryanrentfro/code/markamp/src/core/AppState.cpp

### P10-T178: Adoption Metrics Persistence Preferences And Serialization
**Task Title:** Adoption Metrics Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by adoption metrics.
**Description:** Identify what parts of measure whether the upgraded canvas is actually being used more effectively. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** tool adoption; board completion rate; export usage after session.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/core/ActivityTimeline.cpp
- /Users/ryanrentfro/code/markamp/src/core/AppState.cpp

### P10-T179: Adoption Metrics Safeguards Telemetry And Recovery
**Task Title:** Adoption Metrics Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for adoption metrics.
**Description:** Instrument measure whether the upgraded canvas is actually being used more effectively. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** tool adoption; board completion rate; export usage after session.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/core/ActivityTimeline.cpp
- /Users/ryanrentfro/code/markamp/src/core/AppState.cpp

### P10-T180: Adoption Metrics Tests Documentation And Rollout Gate
**Task Title:** Adoption Metrics Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship adoption metrics.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for measure whether the upgraded canvas is actually being used more effectively.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** tool adoption; board completion rate; export usage after session.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/core/ActivityTimeline.cpp
- /Users/ryanrentfro/code/markamp/src/core/AppState.cpp

## Workstream 19: Support Playbooks
Prepare engineering and support for the issues users will actually report.

### P10-T181: Support Playbooks UX Parity Audit
**Task Title:** Support Playbooks UX Parity Audit
**Definition:** Define the implementation contract for support playbooks by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around prepare engineering and support for the issues users will actually report.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** recovery steps for broken board; embed troubleshooting guide; slow board diagnosis.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/docs
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.h
- /Users/ryanrentfro/code/markamp/src/core/CrashReporter.h

### P10-T182: Support Playbooks Domain Model And State Contract
**Task Title:** Support Playbooks Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support support playbooks.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around prepare engineering and support for the issues users will actually report.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** recovery steps for broken board; embed troubleshooting guide; slow board diagnosis.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/docs
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.h
- /Users/ryanrentfro/code/markamp/src/core/CrashReporter.h

### P10-T183: Support Playbooks Commands Events And Context Keys
**Task Title:** Support Playbooks Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for support playbooks.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make prepare engineering and support for the issues users will actually report. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** recovery steps for broken board; embed troubleshooting guide; slow board diagnosis.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/docs
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.h
- /Users/ryanrentfro/code/markamp/src/core/CrashReporter.h

### P10-T184: Support Playbooks Workspace Surface And Controls
**Task Title:** Support Playbooks Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose support playbooks in the main canvas workspace.
**Description:** Build the primary UI surfaces for prepare engineering and support for the issues users will actually report., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** recovery steps for broken board; embed troubleshooting guide; slow board diagnosis.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/docs
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.h
- /Users/ryanrentfro/code/markamp/src/core/CrashReporter.h

### P10-T185: Support Playbooks Pointer Interaction Flow
**Task Title:** Support Playbooks Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for support playbooks.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for prepare engineering and support for the issues users will actually report.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** recovery steps for broken board; embed troubleshooting guide; slow board diagnosis.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/docs
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.h
- /Users/ryanrentfro/code/markamp/src/core/CrashReporter.h

### P10-T186: Support Playbooks Keyboard Gesture And Shortcut Flow
**Task Title:** Support Playbooks Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for support playbooks.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for prepare engineering and support for the issues users will actually report.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** recovery steps for broken board; embed troubleshooting guide; slow board diagnosis.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/docs
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.h
- /Users/ryanrentfro/code/markamp/src/core/CrashReporter.h

### P10-T187: Support Playbooks Visual Feedback And Rendering Polish
**Task Title:** Support Playbooks Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make support playbooks legible in use.
**Description:** Implement the visible feedback for prepare engineering and support for the issues users will actually report.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** recovery steps for broken board; embed troubleshooting guide; slow board diagnosis.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/docs
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.h
- /Users/ryanrentfro/code/markamp/src/core/CrashReporter.h

### P10-T188: Support Playbooks Persistence Preferences And Serialization
**Task Title:** Support Playbooks Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by support playbooks.
**Description:** Identify what parts of prepare engineering and support for the issues users will actually report. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** recovery steps for broken board; embed troubleshooting guide; slow board diagnosis.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/docs
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.h
- /Users/ryanrentfro/code/markamp/src/core/CrashReporter.h

### P10-T189: Support Playbooks Safeguards Telemetry And Recovery
**Task Title:** Support Playbooks Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for support playbooks.
**Description:** Instrument prepare engineering and support for the issues users will actually report. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** recovery steps for broken board; embed troubleshooting guide; slow board diagnosis.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/docs
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.h
- /Users/ryanrentfro/code/markamp/src/core/CrashReporter.h

### P10-T190: Support Playbooks Tests Documentation And Rollout Gate
**Task Title:** Support Playbooks Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship support playbooks.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for prepare engineering and support for the issues users will actually report.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** recovery steps for broken board; embed troubleshooting guide; slow board diagnosis.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/docs
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.h
- /Users/ryanrentfro/code/markamp/src/core/CrashReporter.h

## Workstream 20: Architecture Follow Ups
Capture the longer-term technical debt and platform investments revealed by the parity push.

### P10-T191: Architecture Follow Ups UX Parity Audit
**Task Title:** Architecture Follow Ups UX Parity Audit
**Definition:** Define the implementation contract for architecture follow ups by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around capture the longer-term technical debt and platform investments revealed by the parity push.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** rendering abstraction backlog; input model cleanup; plugin surface simplification.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.h
- /Users/ryanrentfro/code/markamp/src/core/CrossModuleTestSuite.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DesignSystemContext.h

### P10-T192: Architecture Follow Ups Domain Model And State Contract
**Task Title:** Architecture Follow Ups Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support architecture follow ups.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around capture the longer-term technical debt and platform investments revealed by the parity push.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** rendering abstraction backlog; input model cleanup; plugin surface simplification.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.h
- /Users/ryanrentfro/code/markamp/src/core/CrossModuleTestSuite.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DesignSystemContext.h

### P10-T193: Architecture Follow Ups Commands Events And Context Keys
**Task Title:** Architecture Follow Ups Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for architecture follow ups.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make capture the longer-term technical debt and platform investments revealed by the parity push. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** rendering abstraction backlog; input model cleanup; plugin surface simplification.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.h
- /Users/ryanrentfro/code/markamp/src/core/CrossModuleTestSuite.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DesignSystemContext.h

### P10-T194: Architecture Follow Ups Workspace Surface And Controls
**Task Title:** Architecture Follow Ups Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose architecture follow ups in the main canvas workspace.
**Description:** Build the primary UI surfaces for capture the longer-term technical debt and platform investments revealed by the parity push., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** rendering abstraction backlog; input model cleanup; plugin surface simplification.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.h
- /Users/ryanrentfro/code/markamp/src/core/CrossModuleTestSuite.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DesignSystemContext.h

### P10-T195: Architecture Follow Ups Pointer Interaction Flow
**Task Title:** Architecture Follow Ups Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for architecture follow ups.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for capture the longer-term technical debt and platform investments revealed by the parity push.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** rendering abstraction backlog; input model cleanup; plugin surface simplification.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.h
- /Users/ryanrentfro/code/markamp/src/core/CrossModuleTestSuite.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DesignSystemContext.h

### P10-T196: Architecture Follow Ups Keyboard Gesture And Shortcut Flow
**Task Title:** Architecture Follow Ups Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for architecture follow ups.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for capture the longer-term technical debt and platform investments revealed by the parity push.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** rendering abstraction backlog; input model cleanup; plugin surface simplification.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.h
- /Users/ryanrentfro/code/markamp/src/core/CrossModuleTestSuite.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DesignSystemContext.h

### P10-T197: Architecture Follow Ups Visual Feedback And Rendering Polish
**Task Title:** Architecture Follow Ups Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make architecture follow ups legible in use.
**Description:** Implement the visible feedback for capture the longer-term technical debt and platform investments revealed by the parity push.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** rendering abstraction backlog; input model cleanup; plugin surface simplification.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.h
- /Users/ryanrentfro/code/markamp/src/core/CrossModuleTestSuite.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DesignSystemContext.h

### P10-T198: Architecture Follow Ups Persistence Preferences And Serialization
**Task Title:** Architecture Follow Ups Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by architecture follow ups.
**Description:** Identify what parts of capture the longer-term technical debt and platform investments revealed by the parity push. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** rendering abstraction backlog; input model cleanup; plugin surface simplification.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.h
- /Users/ryanrentfro/code/markamp/src/core/CrossModuleTestSuite.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DesignSystemContext.h

### P10-T199: Architecture Follow Ups Safeguards Telemetry And Recovery
**Task Title:** Architecture Follow Ups Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for architecture follow ups.
**Description:** Instrument capture the longer-term technical debt and platform investments revealed by the parity push. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** rendering abstraction backlog; input model cleanup; plugin surface simplification.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.h
- /Users/ryanrentfro/code/markamp/src/core/CrossModuleTestSuite.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DesignSystemContext.h

### P10-T200: Architecture Follow Ups Tests Documentation And Rollout Gate
**Task Title:** Architecture Follow Ups Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship architecture follow ups.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for capture the longer-term technical debt and platform investments revealed by the parity push.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** rendering abstraction backlog; input model cleanup; plugin surface simplification.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/scripts/run_tests.sh
- /Users/ryanrentfro/code/markamp/scripts/benchmark_compare.py
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.h
- /Users/ryanrentfro/code/markamp/src/core/CrossModuleTestSuite.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DesignSystemContext.h

## Phase Exit Criteria
- All 200 tasks have either shipped, been explicitly deferred with rationale, or been converted into implementation issues with owners.
- Critical workflows in this phase have unit, integration, and at least one end-to-end validation path or a documented gap.
- The shipped work is theme-aware, accessibility-aware, serialization-safe, and undo/redo-safe by default.
- The phase produces measurable progress toward Miro-level editing, drawing, moving, and content authoring quality.
