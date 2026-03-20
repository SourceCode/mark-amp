# Phase 08: Templates Automation AI Assistance And Creation Flows

## Objective
Use templates, automation, AI, import/export workflows, and extensibility to accelerate content creation and reduce repetitive board labor.

## AI Agent Execution Rules
- Execute workstreams in order unless a later task is explicitly unblocked by completed model and command contracts.
- Prefer extending existing canvas, UI, and core services before introducing new parallel abstractions.
- Every implementation task should finish with tests or a documented testing gap.
- Preserve theme, accessibility, undo/redo, and serialization behavior as default quality bars rather than follow-up work.
- When adding references to new code during implementation, keep using full file paths in planning notes and PR write-ups.

## Workstreams
- W01 Board Templates: Make new-board creation start from strong, role-specific defaults.
- W02 Object Templates: Allow reusable object clusters and design modules inside the canvas.
- W03 Advanced Style Presets: Bring team-wide styling systems to the board layer.
- W04 Automation Rules: Automate low-risk repetitive actions so boards stay organized as they grow.
- W05 AI Board Generation: Generate starting structures from prompts without hiding the editable model.
- W06 AI Cleanup And Refinement: Use AI to normalize or restructure messy boards after a session.
- W07 AI Summaries And Explanations: Turn dense boards into digestible summaries and narratives.
- W08 AI Tags And Links: Suggest metadata and board connections to improve findability.
- W09 Batch Operations: Bundle repetitive edits into scalable multi-object workflows.
- W10 Markdown To Canvas: Bridge document and board workflows with meaningful imports.
- W11 CSV And Database Imports: Convert tabular data into useful board structures.
- W12 Export Workflows: Turn working boards into publishable artifacts with predictable quality.
- W13 Reusable Components: Support repeated insertion of high-value modules without external copy-paste hacks.
- W14 Plugin Hooks: Make canvas automation and creation flows extensible through stable integration seams.
- W15 Command Macros: Allow reusable multi-step board actions for power users and team admins.
- W16 Quick Insert: Make insertion faster through slash menus and context-aware quick-add surfaces.
- W17 Smart Defaults: Bias the UI toward likely next actions without becoming opaque.
- W18 Profile And Preference Sync: Carry team and personal defaults across boards and devices where appropriate.
- W19 Onboarding Programs: Use checklists and progressive disclosure to shorten time-to-value for new creators.
- W20 Agent Ready Scaffolds: Document creation flows so AI coding agents can implement them incrementally.

## Workstream 01: Board Templates
Make new-board creation start from strong, role-specific defaults.

### P08-T001: Board Templates UX Parity Audit
**Task Title:** Board Templates UX Parity Audit
**Definition:** Define the implementation contract for board templates by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around make new-board creation start from strong, role-specific defaults.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** brainstorm starter; journey map starter; team retro starter.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.h
- /Users/ryanrentfro/code/markamp/src/canvas/OnboardingModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DialogStackManager.cpp

### P08-T002: Board Templates Domain Model And State Contract
**Task Title:** Board Templates Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support board templates.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around make new-board creation start from strong, role-specific defaults.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** brainstorm starter; journey map starter; team retro starter.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.h
- /Users/ryanrentfro/code/markamp/src/canvas/OnboardingModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DialogStackManager.cpp

### P08-T003: Board Templates Commands Events And Context Keys
**Task Title:** Board Templates Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for board templates.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make make new-board creation start from strong, role-specific defaults. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** brainstorm starter; journey map starter; team retro starter.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.h
- /Users/ryanrentfro/code/markamp/src/canvas/OnboardingModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DialogStackManager.cpp

### P08-T004: Board Templates Workspace Surface And Controls
**Task Title:** Board Templates Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose board templates in the main canvas workspace.
**Description:** Build the primary UI surfaces for make new-board creation start from strong, role-specific defaults., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** brainstorm starter; journey map starter; team retro starter.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.h
- /Users/ryanrentfro/code/markamp/src/canvas/OnboardingModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DialogStackManager.cpp

### P08-T005: Board Templates Pointer Interaction Flow
**Task Title:** Board Templates Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for board templates.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for make new-board creation start from strong, role-specific defaults.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** brainstorm starter; journey map starter; team retro starter.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.h
- /Users/ryanrentfro/code/markamp/src/canvas/OnboardingModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DialogStackManager.cpp

### P08-T006: Board Templates Keyboard Gesture And Shortcut Flow
**Task Title:** Board Templates Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for board templates.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for make new-board creation start from strong, role-specific defaults.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** brainstorm starter; journey map starter; team retro starter.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.h
- /Users/ryanrentfro/code/markamp/src/canvas/OnboardingModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DialogStackManager.cpp

### P08-T007: Board Templates Visual Feedback And Rendering Polish
**Task Title:** Board Templates Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make board templates legible in use.
**Description:** Implement the visible feedback for make new-board creation start from strong, role-specific defaults.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** brainstorm starter; journey map starter; team retro starter.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.h
- /Users/ryanrentfro/code/markamp/src/canvas/OnboardingModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DialogStackManager.cpp

### P08-T008: Board Templates Persistence Preferences And Serialization
**Task Title:** Board Templates Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by board templates.
**Description:** Identify what parts of make new-board creation start from strong, role-specific defaults. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** brainstorm starter; journey map starter; team retro starter.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.h
- /Users/ryanrentfro/code/markamp/src/canvas/OnboardingModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DialogStackManager.cpp

### P08-T009: Board Templates Safeguards Telemetry And Recovery
**Task Title:** Board Templates Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for board templates.
**Description:** Instrument make new-board creation start from strong, role-specific defaults. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** brainstorm starter; journey map starter; team retro starter.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.h
- /Users/ryanrentfro/code/markamp/src/canvas/OnboardingModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DialogStackManager.cpp

### P08-T010: Board Templates Tests Documentation And Rollout Gate
**Task Title:** Board Templates Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship board templates.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for make new-board creation start from strong, role-specific defaults.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** brainstorm starter; journey map starter; team retro starter.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.h
- /Users/ryanrentfro/code/markamp/src/canvas/OnboardingModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DialogStackManager.cpp

## Workstream 02: Object Templates
Allow reusable object clusters and design modules inside the canvas.

### P08-T011: Object Templates UX Parity Audit
**Task Title:** Object Templates UX Parity Audit
**Definition:** Define the implementation contract for object templates by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around allow reusable object clusters and design modules inside the canvas.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** team card cluster; feature planning frame; service blueprint block.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectFactory.h
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp

### P08-T012: Object Templates Domain Model And State Contract
**Task Title:** Object Templates Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support object templates.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around allow reusable object clusters and design modules inside the canvas.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** team card cluster; feature planning frame; service blueprint block.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectFactory.h
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp

### P08-T013: Object Templates Commands Events And Context Keys
**Task Title:** Object Templates Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for object templates.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make allow reusable object clusters and design modules inside the canvas. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** team card cluster; feature planning frame; service blueprint block.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectFactory.h
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp

### P08-T014: Object Templates Workspace Surface And Controls
**Task Title:** Object Templates Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose object templates in the main canvas workspace.
**Description:** Build the primary UI surfaces for allow reusable object clusters and design modules inside the canvas., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** team card cluster; feature planning frame; service blueprint block.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectFactory.h
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp

### P08-T015: Object Templates Pointer Interaction Flow
**Task Title:** Object Templates Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for object templates.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for allow reusable object clusters and design modules inside the canvas.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** team card cluster; feature planning frame; service blueprint block.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectFactory.h
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp

### P08-T016: Object Templates Keyboard Gesture And Shortcut Flow
**Task Title:** Object Templates Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for object templates.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for allow reusable object clusters and design modules inside the canvas.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** team card cluster; feature planning frame; service blueprint block.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectFactory.h
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp

### P08-T017: Object Templates Visual Feedback And Rendering Polish
**Task Title:** Object Templates Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make object templates legible in use.
**Description:** Implement the visible feedback for allow reusable object clusters and design modules inside the canvas.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** team card cluster; feature planning frame; service blueprint block.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectFactory.h
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp

### P08-T018: Object Templates Persistence Preferences And Serialization
**Task Title:** Object Templates Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by object templates.
**Description:** Identify what parts of allow reusable object clusters and design modules inside the canvas. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** team card cluster; feature planning frame; service blueprint block.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectFactory.h
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp

### P08-T019: Object Templates Safeguards Telemetry And Recovery
**Task Title:** Object Templates Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for object templates.
**Description:** Instrument allow reusable object clusters and design modules inside the canvas. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** team card cluster; feature planning frame; service blueprint block.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectFactory.h
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp

### P08-T020: Object Templates Tests Documentation And Rollout Gate
**Task Title:** Object Templates Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship object templates.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for allow reusable object clusters and design modules inside the canvas.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** team card cluster; feature planning frame; service blueprint block.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectFactory.h
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp

## Workstream 03: Advanced Style Presets
Bring team-wide styling systems to the board layer.

### P08-T021: Advanced Style Presets UX Parity Audit
**Task Title:** Advanced Style Presets UX Parity Audit
**Definition:** Define the implementation contract for advanced style presets by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around bring team-wide styling systems to the board layer.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** brand sticky presets; diagram theme presets; presentation frame styles.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.h
- /Users/ryanrentfro/code/markamp/src/core/ConfigProfile.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DesignTokenRegistry.cpp

### P08-T022: Advanced Style Presets Domain Model And State Contract
**Task Title:** Advanced Style Presets Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support advanced style presets.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around bring team-wide styling systems to the board layer.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** brand sticky presets; diagram theme presets; presentation frame styles.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.h
- /Users/ryanrentfro/code/markamp/src/core/ConfigProfile.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DesignTokenRegistry.cpp

### P08-T023: Advanced Style Presets Commands Events And Context Keys
**Task Title:** Advanced Style Presets Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for advanced style presets.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make bring team-wide styling systems to the board layer. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** brand sticky presets; diagram theme presets; presentation frame styles.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.h
- /Users/ryanrentfro/code/markamp/src/core/ConfigProfile.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DesignTokenRegistry.cpp

### P08-T024: Advanced Style Presets Workspace Surface And Controls
**Task Title:** Advanced Style Presets Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose advanced style presets in the main canvas workspace.
**Description:** Build the primary UI surfaces for bring team-wide styling systems to the board layer., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** brand sticky presets; diagram theme presets; presentation frame styles.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.h
- /Users/ryanrentfro/code/markamp/src/core/ConfigProfile.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DesignTokenRegistry.cpp

### P08-T025: Advanced Style Presets Pointer Interaction Flow
**Task Title:** Advanced Style Presets Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for advanced style presets.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for bring team-wide styling systems to the board layer.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** brand sticky presets; diagram theme presets; presentation frame styles.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.h
- /Users/ryanrentfro/code/markamp/src/core/ConfigProfile.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DesignTokenRegistry.cpp

### P08-T026: Advanced Style Presets Keyboard Gesture And Shortcut Flow
**Task Title:** Advanced Style Presets Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for advanced style presets.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for bring team-wide styling systems to the board layer.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** brand sticky presets; diagram theme presets; presentation frame styles.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.h
- /Users/ryanrentfro/code/markamp/src/core/ConfigProfile.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DesignTokenRegistry.cpp

### P08-T027: Advanced Style Presets Visual Feedback And Rendering Polish
**Task Title:** Advanced Style Presets Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make advanced style presets legible in use.
**Description:** Implement the visible feedback for bring team-wide styling systems to the board layer.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** brand sticky presets; diagram theme presets; presentation frame styles.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.h
- /Users/ryanrentfro/code/markamp/src/core/ConfigProfile.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DesignTokenRegistry.cpp

### P08-T028: Advanced Style Presets Persistence Preferences And Serialization
**Task Title:** Advanced Style Presets Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by advanced style presets.
**Description:** Identify what parts of bring team-wide styling systems to the board layer. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** brand sticky presets; diagram theme presets; presentation frame styles.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.h
- /Users/ryanrentfro/code/markamp/src/core/ConfigProfile.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DesignTokenRegistry.cpp

### P08-T029: Advanced Style Presets Safeguards Telemetry And Recovery
**Task Title:** Advanced Style Presets Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for advanced style presets.
**Description:** Instrument bring team-wide styling systems to the board layer. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** brand sticky presets; diagram theme presets; presentation frame styles.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.h
- /Users/ryanrentfro/code/markamp/src/core/ConfigProfile.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DesignTokenRegistry.cpp

### P08-T030: Advanced Style Presets Tests Documentation And Rollout Gate
**Task Title:** Advanced Style Presets Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship advanced style presets.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for bring team-wide styling systems to the board layer.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** brand sticky presets; diagram theme presets; presentation frame styles.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.h
- /Users/ryanrentfro/code/markamp/src/core/ConfigProfile.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DesignTokenRegistry.cpp

## Workstream 04: Automation Rules
Automate low-risk repetitive actions so boards stay organized as they grow.

### P08-T031: Automation Rules UX Parity Audit
**Task Title:** Automation Rules UX Parity Audit
**Definition:** Define the implementation contract for automation rules by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around automate low-risk repetitive actions so boards stay organized as they grow.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** auto-tag new sticky; snap comments into review frame; assign default owner.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.h
- /Users/ryanrentfro/code/markamp/src/core/AutomationCommandProvider.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectLifecycleEvents.cpp

### P08-T032: Automation Rules Domain Model And State Contract
**Task Title:** Automation Rules Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support automation rules.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around automate low-risk repetitive actions so boards stay organized as they grow.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** auto-tag new sticky; snap comments into review frame; assign default owner.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.h
- /Users/ryanrentfro/code/markamp/src/core/AutomationCommandProvider.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectLifecycleEvents.cpp

### P08-T033: Automation Rules Commands Events And Context Keys
**Task Title:** Automation Rules Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for automation rules.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make automate low-risk repetitive actions so boards stay organized as they grow. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** auto-tag new sticky; snap comments into review frame; assign default owner.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.h
- /Users/ryanrentfro/code/markamp/src/core/AutomationCommandProvider.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectLifecycleEvents.cpp

### P08-T034: Automation Rules Workspace Surface And Controls
**Task Title:** Automation Rules Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose automation rules in the main canvas workspace.
**Description:** Build the primary UI surfaces for automate low-risk repetitive actions so boards stay organized as they grow., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** auto-tag new sticky; snap comments into review frame; assign default owner.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.h
- /Users/ryanrentfro/code/markamp/src/core/AutomationCommandProvider.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectLifecycleEvents.cpp

### P08-T035: Automation Rules Pointer Interaction Flow
**Task Title:** Automation Rules Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for automation rules.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for automate low-risk repetitive actions so boards stay organized as they grow.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** auto-tag new sticky; snap comments into review frame; assign default owner.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.h
- /Users/ryanrentfro/code/markamp/src/core/AutomationCommandProvider.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectLifecycleEvents.cpp

### P08-T036: Automation Rules Keyboard Gesture And Shortcut Flow
**Task Title:** Automation Rules Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for automation rules.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for automate low-risk repetitive actions so boards stay organized as they grow.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** auto-tag new sticky; snap comments into review frame; assign default owner.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.h
- /Users/ryanrentfro/code/markamp/src/core/AutomationCommandProvider.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectLifecycleEvents.cpp

### P08-T037: Automation Rules Visual Feedback And Rendering Polish
**Task Title:** Automation Rules Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make automation rules legible in use.
**Description:** Implement the visible feedback for automate low-risk repetitive actions so boards stay organized as they grow.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** auto-tag new sticky; snap comments into review frame; assign default owner.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.h
- /Users/ryanrentfro/code/markamp/src/core/AutomationCommandProvider.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectLifecycleEvents.cpp

### P08-T038: Automation Rules Persistence Preferences And Serialization
**Task Title:** Automation Rules Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by automation rules.
**Description:** Identify what parts of automate low-risk repetitive actions so boards stay organized as they grow. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** auto-tag new sticky; snap comments into review frame; assign default owner.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.h
- /Users/ryanrentfro/code/markamp/src/core/AutomationCommandProvider.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectLifecycleEvents.cpp

### P08-T039: Automation Rules Safeguards Telemetry And Recovery
**Task Title:** Automation Rules Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for automation rules.
**Description:** Instrument automate low-risk repetitive actions so boards stay organized as they grow. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** auto-tag new sticky; snap comments into review frame; assign default owner.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.h
- /Users/ryanrentfro/code/markamp/src/core/AutomationCommandProvider.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectLifecycleEvents.cpp

### P08-T040: Automation Rules Tests Documentation And Rollout Gate
**Task Title:** Automation Rules Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship automation rules.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for automate low-risk repetitive actions so boards stay organized as they grow.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** auto-tag new sticky; snap comments into review frame; assign default owner.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.h
- /Users/ryanrentfro/code/markamp/src/core/AutomationCommandProvider.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectLifecycleEvents.cpp

## Workstream 05: AI Board Generation
Generate starting structures from prompts without hiding the editable model.

### P08-T041: AI Board Generation UX Parity Audit
**Task Title:** AI Board Generation UX Parity Audit
**Definition:** Define the implementation contract for ai board generation by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around generate starting structures from prompts without hiding the editable model.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** create workshop board from prompt; generate flowchart skeleton; build comparison matrix.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.h
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.h
- /Users/ryanrentfro/code/markamp/src/core/AITypes.h

### P08-T042: AI Board Generation Domain Model And State Contract
**Task Title:** AI Board Generation Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support ai board generation.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around generate starting structures from prompts without hiding the editable model.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** create workshop board from prompt; generate flowchart skeleton; build comparison matrix.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.h
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.h
- /Users/ryanrentfro/code/markamp/src/core/AITypes.h

### P08-T043: AI Board Generation Commands Events And Context Keys
**Task Title:** AI Board Generation Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for ai board generation.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make generate starting structures from prompts without hiding the editable model. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** create workshop board from prompt; generate flowchart skeleton; build comparison matrix.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.h
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.h
- /Users/ryanrentfro/code/markamp/src/core/AITypes.h

### P08-T044: AI Board Generation Workspace Surface And Controls
**Task Title:** AI Board Generation Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose ai board generation in the main canvas workspace.
**Description:** Build the primary UI surfaces for generate starting structures from prompts without hiding the editable model., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** create workshop board from prompt; generate flowchart skeleton; build comparison matrix.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.h
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.h
- /Users/ryanrentfro/code/markamp/src/core/AITypes.h

### P08-T045: AI Board Generation Pointer Interaction Flow
**Task Title:** AI Board Generation Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for ai board generation.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for generate starting structures from prompts without hiding the editable model.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** create workshop board from prompt; generate flowchart skeleton; build comparison matrix.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.h
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.h
- /Users/ryanrentfro/code/markamp/src/core/AITypes.h

### P08-T046: AI Board Generation Keyboard Gesture And Shortcut Flow
**Task Title:** AI Board Generation Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for ai board generation.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for generate starting structures from prompts without hiding the editable model.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** create workshop board from prompt; generate flowchart skeleton; build comparison matrix.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.h
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.h
- /Users/ryanrentfro/code/markamp/src/core/AITypes.h

### P08-T047: AI Board Generation Visual Feedback And Rendering Polish
**Task Title:** AI Board Generation Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make ai board generation legible in use.
**Description:** Implement the visible feedback for generate starting structures from prompts without hiding the editable model.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** create workshop board from prompt; generate flowchart skeleton; build comparison matrix.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.h
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.h
- /Users/ryanrentfro/code/markamp/src/core/AITypes.h

### P08-T048: AI Board Generation Persistence Preferences And Serialization
**Task Title:** AI Board Generation Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by ai board generation.
**Description:** Identify what parts of generate starting structures from prompts without hiding the editable model. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** create workshop board from prompt; generate flowchart skeleton; build comparison matrix.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.h
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.h
- /Users/ryanrentfro/code/markamp/src/core/AITypes.h

### P08-T049: AI Board Generation Safeguards Telemetry And Recovery
**Task Title:** AI Board Generation Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for ai board generation.
**Description:** Instrument generate starting structures from prompts without hiding the editable model. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** create workshop board from prompt; generate flowchart skeleton; build comparison matrix.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.h
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.h
- /Users/ryanrentfro/code/markamp/src/core/AITypes.h

### P08-T050: AI Board Generation Tests Documentation And Rollout Gate
**Task Title:** AI Board Generation Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship ai board generation.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for generate starting structures from prompts without hiding the editable model.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** create workshop board from prompt; generate flowchart skeleton; build comparison matrix.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.h
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.h
- /Users/ryanrentfro/code/markamp/src/core/AITypes.h

## Workstream 06: AI Cleanup And Refinement
Use AI to normalize or restructure messy boards after a session.

### P08-T051: AI Cleanup And Refinement UX Parity Audit
**Task Title:** AI Cleanup And Refinement UX Parity Audit
**Definition:** Define the implementation contract for ai cleanup and refinement by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around use ai to normalize or restructure messy boards after a session.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** cluster related notes; rewrite sticky titles; convert raw notes into sections.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIWritingAssistant.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AutoLayoutModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearchModel.cpp

### P08-T052: AI Cleanup And Refinement Domain Model And State Contract
**Task Title:** AI Cleanup And Refinement Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support ai cleanup and refinement.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around use ai to normalize or restructure messy boards after a session.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** cluster related notes; rewrite sticky titles; convert raw notes into sections.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIWritingAssistant.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AutoLayoutModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearchModel.cpp

### P08-T053: AI Cleanup And Refinement Commands Events And Context Keys
**Task Title:** AI Cleanup And Refinement Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for ai cleanup and refinement.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make use ai to normalize or restructure messy boards after a session. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** cluster related notes; rewrite sticky titles; convert raw notes into sections.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIWritingAssistant.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AutoLayoutModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearchModel.cpp

### P08-T054: AI Cleanup And Refinement Workspace Surface And Controls
**Task Title:** AI Cleanup And Refinement Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose ai cleanup and refinement in the main canvas workspace.
**Description:** Build the primary UI surfaces for use ai to normalize or restructure messy boards after a session., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** cluster related notes; rewrite sticky titles; convert raw notes into sections.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIWritingAssistant.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AutoLayoutModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearchModel.cpp

### P08-T055: AI Cleanup And Refinement Pointer Interaction Flow
**Task Title:** AI Cleanup And Refinement Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for ai cleanup and refinement.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for use ai to normalize or restructure messy boards after a session.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** cluster related notes; rewrite sticky titles; convert raw notes into sections.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIWritingAssistant.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AutoLayoutModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearchModel.cpp

### P08-T056: AI Cleanup And Refinement Keyboard Gesture And Shortcut Flow
**Task Title:** AI Cleanup And Refinement Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for ai cleanup and refinement.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for use ai to normalize or restructure messy boards after a session.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** cluster related notes; rewrite sticky titles; convert raw notes into sections.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIWritingAssistant.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AutoLayoutModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearchModel.cpp

### P08-T057: AI Cleanup And Refinement Visual Feedback And Rendering Polish
**Task Title:** AI Cleanup And Refinement Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make ai cleanup and refinement legible in use.
**Description:** Implement the visible feedback for use ai to normalize or restructure messy boards after a session.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** cluster related notes; rewrite sticky titles; convert raw notes into sections.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIWritingAssistant.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AutoLayoutModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearchModel.cpp

### P08-T058: AI Cleanup And Refinement Persistence Preferences And Serialization
**Task Title:** AI Cleanup And Refinement Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by ai cleanup and refinement.
**Description:** Identify what parts of use ai to normalize or restructure messy boards after a session. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** cluster related notes; rewrite sticky titles; convert raw notes into sections.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIWritingAssistant.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AutoLayoutModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearchModel.cpp

### P08-T059: AI Cleanup And Refinement Safeguards Telemetry And Recovery
**Task Title:** AI Cleanup And Refinement Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for ai cleanup and refinement.
**Description:** Instrument use ai to normalize or restructure messy boards after a session. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** cluster related notes; rewrite sticky titles; convert raw notes into sections.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIWritingAssistant.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AutoLayoutModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearchModel.cpp

### P08-T060: AI Cleanup And Refinement Tests Documentation And Rollout Gate
**Task Title:** AI Cleanup And Refinement Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship ai cleanup and refinement.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for use ai to normalize or restructure messy boards after a session.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** cluster related notes; rewrite sticky titles; convert raw notes into sections.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIWritingAssistant.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AutoLayoutModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearchModel.cpp

## Workstream 07: AI Summaries And Explanations
Turn dense boards into digestible summaries and narratives.

### P08-T061: AI Summaries And Explanations UX Parity Audit
**Task Title:** AI Summaries And Explanations UX Parity Audit
**Definition:** Define the implementation contract for ai summaries and explanations by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around turn dense boards into digestible summaries and narratives.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** summarize selected frame; explain diagram path; write action-item recap.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIWritingAssistant.h
- /Users/ryanrentfro/code/markamp/src/core/AIConversationHistory.h
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp

### P08-T062: AI Summaries And Explanations Domain Model And State Contract
**Task Title:** AI Summaries And Explanations Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support ai summaries and explanations.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around turn dense boards into digestible summaries and narratives.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** summarize selected frame; explain diagram path; write action-item recap.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIWritingAssistant.h
- /Users/ryanrentfro/code/markamp/src/core/AIConversationHistory.h
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp

### P08-T063: AI Summaries And Explanations Commands Events And Context Keys
**Task Title:** AI Summaries And Explanations Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for ai summaries and explanations.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make turn dense boards into digestible summaries and narratives. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** summarize selected frame; explain diagram path; write action-item recap.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIWritingAssistant.h
- /Users/ryanrentfro/code/markamp/src/core/AIConversationHistory.h
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp

### P08-T064: AI Summaries And Explanations Workspace Surface And Controls
**Task Title:** AI Summaries And Explanations Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose ai summaries and explanations in the main canvas workspace.
**Description:** Build the primary UI surfaces for turn dense boards into digestible summaries and narratives., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** summarize selected frame; explain diagram path; write action-item recap.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIWritingAssistant.h
- /Users/ryanrentfro/code/markamp/src/core/AIConversationHistory.h
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp

### P08-T065: AI Summaries And Explanations Pointer Interaction Flow
**Task Title:** AI Summaries And Explanations Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for ai summaries and explanations.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for turn dense boards into digestible summaries and narratives.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** summarize selected frame; explain diagram path; write action-item recap.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIWritingAssistant.h
- /Users/ryanrentfro/code/markamp/src/core/AIConversationHistory.h
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp

### P08-T066: AI Summaries And Explanations Keyboard Gesture And Shortcut Flow
**Task Title:** AI Summaries And Explanations Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for ai summaries and explanations.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for turn dense boards into digestible summaries and narratives.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** summarize selected frame; explain diagram path; write action-item recap.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIWritingAssistant.h
- /Users/ryanrentfro/code/markamp/src/core/AIConversationHistory.h
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp

### P08-T067: AI Summaries And Explanations Visual Feedback And Rendering Polish
**Task Title:** AI Summaries And Explanations Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make ai summaries and explanations legible in use.
**Description:** Implement the visible feedback for turn dense boards into digestible summaries and narratives.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** summarize selected frame; explain diagram path; write action-item recap.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIWritingAssistant.h
- /Users/ryanrentfro/code/markamp/src/core/AIConversationHistory.h
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp

### P08-T068: AI Summaries And Explanations Persistence Preferences And Serialization
**Task Title:** AI Summaries And Explanations Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by ai summaries and explanations.
**Description:** Identify what parts of turn dense boards into digestible summaries and narratives. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** summarize selected frame; explain diagram path; write action-item recap.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIWritingAssistant.h
- /Users/ryanrentfro/code/markamp/src/core/AIConversationHistory.h
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp

### P08-T069: AI Summaries And Explanations Safeguards Telemetry And Recovery
**Task Title:** AI Summaries And Explanations Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for ai summaries and explanations.
**Description:** Instrument turn dense boards into digestible summaries and narratives. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** summarize selected frame; explain diagram path; write action-item recap.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIWritingAssistant.h
- /Users/ryanrentfro/code/markamp/src/core/AIConversationHistory.h
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp

### P08-T070: AI Summaries And Explanations Tests Documentation And Rollout Gate
**Task Title:** AI Summaries And Explanations Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship ai summaries and explanations.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for turn dense boards into digestible summaries and narratives.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** summarize selected frame; explain diagram path; write action-item recap.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIWritingAssistant.h
- /Users/ryanrentfro/code/markamp/src/core/AIConversationHistory.h
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp

## Workstream 08: AI Tags And Links
Suggest metadata and board connections to improve findability.

### P08-T071: AI Tags And Links UX Parity Audit
**Task Title:** AI Tags And Links UX Parity Audit
**Definition:** Define the implementation contract for ai tags and links by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around suggest metadata and board connections to improve findability.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** suggest tags for cluster; link related board; identify duplicate notes.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AITagSuggester.cpp
- /Users/ryanrentfro/code/markamp/src/core/AILinkSuggester.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CrossBoardLink.cpp

### P08-T072: AI Tags And Links Domain Model And State Contract
**Task Title:** AI Tags And Links Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support ai tags and links.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around suggest metadata and board connections to improve findability.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** suggest tags for cluster; link related board; identify duplicate notes.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AITagSuggester.cpp
- /Users/ryanrentfro/code/markamp/src/core/AILinkSuggester.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CrossBoardLink.cpp

### P08-T073: AI Tags And Links Commands Events And Context Keys
**Task Title:** AI Tags And Links Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for ai tags and links.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make suggest metadata and board connections to improve findability. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** suggest tags for cluster; link related board; identify duplicate notes.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AITagSuggester.cpp
- /Users/ryanrentfro/code/markamp/src/core/AILinkSuggester.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CrossBoardLink.cpp

### P08-T074: AI Tags And Links Workspace Surface And Controls
**Task Title:** AI Tags And Links Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose ai tags and links in the main canvas workspace.
**Description:** Build the primary UI surfaces for suggest metadata and board connections to improve findability., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** suggest tags for cluster; link related board; identify duplicate notes.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AITagSuggester.cpp
- /Users/ryanrentfro/code/markamp/src/core/AILinkSuggester.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CrossBoardLink.cpp

### P08-T075: AI Tags And Links Pointer Interaction Flow
**Task Title:** AI Tags And Links Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for ai tags and links.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for suggest metadata and board connections to improve findability.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** suggest tags for cluster; link related board; identify duplicate notes.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AITagSuggester.cpp
- /Users/ryanrentfro/code/markamp/src/core/AILinkSuggester.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CrossBoardLink.cpp

### P08-T076: AI Tags And Links Keyboard Gesture And Shortcut Flow
**Task Title:** AI Tags And Links Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for ai tags and links.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for suggest metadata and board connections to improve findability.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** suggest tags for cluster; link related board; identify duplicate notes.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AITagSuggester.cpp
- /Users/ryanrentfro/code/markamp/src/core/AILinkSuggester.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CrossBoardLink.cpp

### P08-T077: AI Tags And Links Visual Feedback And Rendering Polish
**Task Title:** AI Tags And Links Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make ai tags and links legible in use.
**Description:** Implement the visible feedback for suggest metadata and board connections to improve findability.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** suggest tags for cluster; link related board; identify duplicate notes.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AITagSuggester.cpp
- /Users/ryanrentfro/code/markamp/src/core/AILinkSuggester.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CrossBoardLink.cpp

### P08-T078: AI Tags And Links Persistence Preferences And Serialization
**Task Title:** AI Tags And Links Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by ai tags and links.
**Description:** Identify what parts of suggest metadata and board connections to improve findability. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** suggest tags for cluster; link related board; identify duplicate notes.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AITagSuggester.cpp
- /Users/ryanrentfro/code/markamp/src/core/AILinkSuggester.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CrossBoardLink.cpp

### P08-T079: AI Tags And Links Safeguards Telemetry And Recovery
**Task Title:** AI Tags And Links Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for ai tags and links.
**Description:** Instrument suggest metadata and board connections to improve findability. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** suggest tags for cluster; link related board; identify duplicate notes.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AITagSuggester.cpp
- /Users/ryanrentfro/code/markamp/src/core/AILinkSuggester.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CrossBoardLink.cpp

### P08-T080: AI Tags And Links Tests Documentation And Rollout Gate
**Task Title:** AI Tags And Links Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship ai tags and links.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for suggest metadata and board connections to improve findability.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** suggest tags for cluster; link related board; identify duplicate notes.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AITagSuggester.cpp
- /Users/ryanrentfro/code/markamp/src/core/AILinkSuggester.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CrossBoardLink.cpp

## Workstream 09: Batch Operations
Bundle repetitive edits into scalable multi-object workflows.

### P08-T081: Batch Operations UX Parity Audit
**Task Title:** Batch Operations UX Parity Audit
**Definition:** Define the implementation contract for batch operations by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around bundle repetitive edits into scalable multi-object workflows.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** bulk rename selected notes; batch recolor lanes; replace icon set.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BulkActionController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInspector.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandRegistry.h

### P08-T082: Batch Operations Domain Model And State Contract
**Task Title:** Batch Operations Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support batch operations.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around bundle repetitive edits into scalable multi-object workflows.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** bulk rename selected notes; batch recolor lanes; replace icon set.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BulkActionController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInspector.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandRegistry.h

### P08-T083: Batch Operations Commands Events And Context Keys
**Task Title:** Batch Operations Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for batch operations.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make bundle repetitive edits into scalable multi-object workflows. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** bulk rename selected notes; batch recolor lanes; replace icon set.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BulkActionController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInspector.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandRegistry.h

### P08-T084: Batch Operations Workspace Surface And Controls
**Task Title:** Batch Operations Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose batch operations in the main canvas workspace.
**Description:** Build the primary UI surfaces for bundle repetitive edits into scalable multi-object workflows., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** bulk rename selected notes; batch recolor lanes; replace icon set.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BulkActionController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInspector.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandRegistry.h

### P08-T085: Batch Operations Pointer Interaction Flow
**Task Title:** Batch Operations Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for batch operations.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for bundle repetitive edits into scalable multi-object workflows.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** bulk rename selected notes; batch recolor lanes; replace icon set.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BulkActionController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInspector.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandRegistry.h

### P08-T086: Batch Operations Keyboard Gesture And Shortcut Flow
**Task Title:** Batch Operations Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for batch operations.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for bundle repetitive edits into scalable multi-object workflows.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** bulk rename selected notes; batch recolor lanes; replace icon set.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BulkActionController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInspector.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandRegistry.h

### P08-T087: Batch Operations Visual Feedback And Rendering Polish
**Task Title:** Batch Operations Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make batch operations legible in use.
**Description:** Implement the visible feedback for bundle repetitive edits into scalable multi-object workflows.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** bulk rename selected notes; batch recolor lanes; replace icon set.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BulkActionController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInspector.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandRegistry.h

### P08-T088: Batch Operations Persistence Preferences And Serialization
**Task Title:** Batch Operations Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by batch operations.
**Description:** Identify what parts of bundle repetitive edits into scalable multi-object workflows. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** bulk rename selected notes; batch recolor lanes; replace icon set.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BulkActionController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInspector.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandRegistry.h

### P08-T089: Batch Operations Safeguards Telemetry And Recovery
**Task Title:** Batch Operations Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for batch operations.
**Description:** Instrument bundle repetitive edits into scalable multi-object workflows. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** bulk rename selected notes; batch recolor lanes; replace icon set.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BulkActionController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInspector.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandRegistry.h

### P08-T090: Batch Operations Tests Documentation And Rollout Gate
**Task Title:** Batch Operations Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship batch operations.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for bundle repetitive edits into scalable multi-object workflows.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** bulk rename selected notes; batch recolor lanes; replace icon set.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BulkActionController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInspector.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandRegistry.h

## Workstream 10: Markdown To Canvas
Bridge document and board workflows with meaningful imports.

### P08-T091: Markdown To Canvas UX Parity Audit
**Task Title:** Markdown To Canvas UX Parity Audit
**Definition:** Define the implementation contract for markdown to canvas by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around bridge document and board workflows with meaningful imports.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** convert bullet list to notes; turn headings into sections; import checklist as kanban.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/MarkdownImporter.cpp
- /Users/ryanrentfro/code/markamp/src/core/BlockService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectFactory.cpp

### P08-T092: Markdown To Canvas Domain Model And State Contract
**Task Title:** Markdown To Canvas Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support markdown to canvas.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around bridge document and board workflows with meaningful imports.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** convert bullet list to notes; turn headings into sections; import checklist as kanban.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/MarkdownImporter.cpp
- /Users/ryanrentfro/code/markamp/src/core/BlockService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectFactory.cpp

### P08-T093: Markdown To Canvas Commands Events And Context Keys
**Task Title:** Markdown To Canvas Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for markdown to canvas.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make bridge document and board workflows with meaningful imports. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** convert bullet list to notes; turn headings into sections; import checklist as kanban.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/MarkdownImporter.cpp
- /Users/ryanrentfro/code/markamp/src/core/BlockService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectFactory.cpp

### P08-T094: Markdown To Canvas Workspace Surface And Controls
**Task Title:** Markdown To Canvas Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose markdown to canvas in the main canvas workspace.
**Description:** Build the primary UI surfaces for bridge document and board workflows with meaningful imports., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** convert bullet list to notes; turn headings into sections; import checklist as kanban.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/MarkdownImporter.cpp
- /Users/ryanrentfro/code/markamp/src/core/BlockService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectFactory.cpp

### P08-T095: Markdown To Canvas Pointer Interaction Flow
**Task Title:** Markdown To Canvas Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for markdown to canvas.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for bridge document and board workflows with meaningful imports.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** convert bullet list to notes; turn headings into sections; import checklist as kanban.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/MarkdownImporter.cpp
- /Users/ryanrentfro/code/markamp/src/core/BlockService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectFactory.cpp

### P08-T096: Markdown To Canvas Keyboard Gesture And Shortcut Flow
**Task Title:** Markdown To Canvas Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for markdown to canvas.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for bridge document and board workflows with meaningful imports.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** convert bullet list to notes; turn headings into sections; import checklist as kanban.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/MarkdownImporter.cpp
- /Users/ryanrentfro/code/markamp/src/core/BlockService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectFactory.cpp

### P08-T097: Markdown To Canvas Visual Feedback And Rendering Polish
**Task Title:** Markdown To Canvas Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make markdown to canvas legible in use.
**Description:** Implement the visible feedback for bridge document and board workflows with meaningful imports.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** convert bullet list to notes; turn headings into sections; import checklist as kanban.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/MarkdownImporter.cpp
- /Users/ryanrentfro/code/markamp/src/core/BlockService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectFactory.cpp

### P08-T098: Markdown To Canvas Persistence Preferences And Serialization
**Task Title:** Markdown To Canvas Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by markdown to canvas.
**Description:** Identify what parts of bridge document and board workflows with meaningful imports. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** convert bullet list to notes; turn headings into sections; import checklist as kanban.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/MarkdownImporter.cpp
- /Users/ryanrentfro/code/markamp/src/core/BlockService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectFactory.cpp

### P08-T099: Markdown To Canvas Safeguards Telemetry And Recovery
**Task Title:** Markdown To Canvas Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for markdown to canvas.
**Description:** Instrument bridge document and board workflows with meaningful imports. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** convert bullet list to notes; turn headings into sections; import checklist as kanban.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/MarkdownImporter.cpp
- /Users/ryanrentfro/code/markamp/src/core/BlockService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectFactory.cpp

### P08-T100: Markdown To Canvas Tests Documentation And Rollout Gate
**Task Title:** Markdown To Canvas Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship markdown to canvas.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for bridge document and board workflows with meaningful imports.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** convert bullet list to notes; turn headings into sections; import checklist as kanban.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/MarkdownImporter.cpp
- /Users/ryanrentfro/code/markamp/src/core/BlockService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectFactory.cpp

## Workstream 11: CSV And Database Imports
Convert tabular data into useful board structures.

### P08-T101: CSV And Database Imports UX Parity Audit
**Task Title:** CSV And Database Imports UX Parity Audit
**Definition:** Define the implementation contract for csv and database imports by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around convert tabular data into useful board structures.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** csv to table; database rows to cards; timeline import.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/av/AVImportExport.cpp
- /Users/ryanrentfro/code/markamp/src/core/AttributeService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanObjects.cpp

### P08-T102: CSV And Database Imports Domain Model And State Contract
**Task Title:** CSV And Database Imports Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support csv and database imports.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around convert tabular data into useful board structures.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** csv to table; database rows to cards; timeline import.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/av/AVImportExport.cpp
- /Users/ryanrentfro/code/markamp/src/core/AttributeService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanObjects.cpp

### P08-T103: CSV And Database Imports Commands Events And Context Keys
**Task Title:** CSV And Database Imports Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for csv and database imports.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make convert tabular data into useful board structures. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** csv to table; database rows to cards; timeline import.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/av/AVImportExport.cpp
- /Users/ryanrentfro/code/markamp/src/core/AttributeService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanObjects.cpp

### P08-T104: CSV And Database Imports Workspace Surface And Controls
**Task Title:** CSV And Database Imports Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose csv and database imports in the main canvas workspace.
**Description:** Build the primary UI surfaces for convert tabular data into useful board structures., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** csv to table; database rows to cards; timeline import.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/av/AVImportExport.cpp
- /Users/ryanrentfro/code/markamp/src/core/AttributeService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanObjects.cpp

### P08-T105: CSV And Database Imports Pointer Interaction Flow
**Task Title:** CSV And Database Imports Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for csv and database imports.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for convert tabular data into useful board structures.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** csv to table; database rows to cards; timeline import.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/av/AVImportExport.cpp
- /Users/ryanrentfro/code/markamp/src/core/AttributeService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanObjects.cpp

### P08-T106: CSV And Database Imports Keyboard Gesture And Shortcut Flow
**Task Title:** CSV And Database Imports Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for csv and database imports.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for convert tabular data into useful board structures.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** csv to table; database rows to cards; timeline import.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/av/AVImportExport.cpp
- /Users/ryanrentfro/code/markamp/src/core/AttributeService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanObjects.cpp

### P08-T107: CSV And Database Imports Visual Feedback And Rendering Polish
**Task Title:** CSV And Database Imports Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make csv and database imports legible in use.
**Description:** Implement the visible feedback for convert tabular data into useful board structures.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** csv to table; database rows to cards; timeline import.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/av/AVImportExport.cpp
- /Users/ryanrentfro/code/markamp/src/core/AttributeService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanObjects.cpp

### P08-T108: CSV And Database Imports Persistence Preferences And Serialization
**Task Title:** CSV And Database Imports Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by csv and database imports.
**Description:** Identify what parts of convert tabular data into useful board structures. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** csv to table; database rows to cards; timeline import.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/av/AVImportExport.cpp
- /Users/ryanrentfro/code/markamp/src/core/AttributeService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanObjects.cpp

### P08-T109: CSV And Database Imports Safeguards Telemetry And Recovery
**Task Title:** CSV And Database Imports Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for csv and database imports.
**Description:** Instrument convert tabular data into useful board structures. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** csv to table; database rows to cards; timeline import.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/av/AVImportExport.cpp
- /Users/ryanrentfro/code/markamp/src/core/AttributeService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanObjects.cpp

### P08-T110: CSV And Database Imports Tests Documentation And Rollout Gate
**Task Title:** CSV And Database Imports Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship csv and database imports.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for convert tabular data into useful board structures.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** csv to table; database rows to cards; timeline import.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/av/AVImportExport.cpp
- /Users/ryanrentfro/code/markamp/src/core/AttributeService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanObjects.cpp

## Workstream 12: Export Workflows
Turn working boards into publishable artifacts with predictable quality.

### P08-T111: Export Workflows UX Parity Audit
**Task Title:** Export Workflows UX Parity Audit
**Definition:** Define the implementation contract for export workflows by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around turn working boards into publishable artifacts with predictable quality.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** export frame as PNG; batch export sections; HTML export with links.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/BatchExportEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/HtmlExporter.cpp

### P08-T112: Export Workflows Domain Model And State Contract
**Task Title:** Export Workflows Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support export workflows.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around turn working boards into publishable artifacts with predictable quality.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** export frame as PNG; batch export sections; HTML export with links.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/BatchExportEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/HtmlExporter.cpp

### P08-T113: Export Workflows Commands Events And Context Keys
**Task Title:** Export Workflows Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for export workflows.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make turn working boards into publishable artifacts with predictable quality. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** export frame as PNG; batch export sections; HTML export with links.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/BatchExportEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/HtmlExporter.cpp

### P08-T114: Export Workflows Workspace Surface And Controls
**Task Title:** Export Workflows Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose export workflows in the main canvas workspace.
**Description:** Build the primary UI surfaces for turn working boards into publishable artifacts with predictable quality., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** export frame as PNG; batch export sections; HTML export with links.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/BatchExportEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/HtmlExporter.cpp

### P08-T115: Export Workflows Pointer Interaction Flow
**Task Title:** Export Workflows Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for export workflows.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for turn working boards into publishable artifacts with predictable quality.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** export frame as PNG; batch export sections; HTML export with links.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/BatchExportEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/HtmlExporter.cpp

### P08-T116: Export Workflows Keyboard Gesture And Shortcut Flow
**Task Title:** Export Workflows Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for export workflows.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for turn working boards into publishable artifacts with predictable quality.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** export frame as PNG; batch export sections; HTML export with links.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/BatchExportEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/HtmlExporter.cpp

### P08-T117: Export Workflows Visual Feedback And Rendering Polish
**Task Title:** Export Workflows Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make export workflows legible in use.
**Description:** Implement the visible feedback for turn working boards into publishable artifacts with predictable quality.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** export frame as PNG; batch export sections; HTML export with links.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/BatchExportEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/HtmlExporter.cpp

### P08-T118: Export Workflows Persistence Preferences And Serialization
**Task Title:** Export Workflows Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by export workflows.
**Description:** Identify what parts of turn working boards into publishable artifacts with predictable quality. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** export frame as PNG; batch export sections; HTML export with links.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/BatchExportEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/HtmlExporter.cpp

### P08-T119: Export Workflows Safeguards Telemetry And Recovery
**Task Title:** Export Workflows Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for export workflows.
**Description:** Instrument turn working boards into publishable artifacts with predictable quality. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** export frame as PNG; batch export sections; HTML export with links.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/BatchExportEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/HtmlExporter.cpp

### P08-T120: Export Workflows Tests Documentation And Rollout Gate
**Task Title:** Export Workflows Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship export workflows.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for turn working boards into publishable artifacts with predictable quality.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** export frame as PNG; batch export sections; HTML export with links.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/BatchExportEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/HtmlExporter.cpp

## Workstream 13: Reusable Components
Support repeated insertion of high-value modules without external copy-paste hacks.

### P08-T121: Reusable Components UX Parity Audit
**Task Title:** Reusable Components UX Parity Audit
**Definition:** Define the implementation contract for reusable components by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around support repeated insertion of high-value modules without external copy-paste hacks.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** saved widget cluster; meeting template block; diagram legend component.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CustomObjectTypeRegistry.cpp
- /Users/ryanrentfro/code/markamp/src/core/BuiltInPlugins.h
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp

### P08-T122: Reusable Components Domain Model And State Contract
**Task Title:** Reusable Components Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support reusable components.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around support repeated insertion of high-value modules without external copy-paste hacks.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** saved widget cluster; meeting template block; diagram legend component.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CustomObjectTypeRegistry.cpp
- /Users/ryanrentfro/code/markamp/src/core/BuiltInPlugins.h
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp

### P08-T123: Reusable Components Commands Events And Context Keys
**Task Title:** Reusable Components Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for reusable components.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make support repeated insertion of high-value modules without external copy-paste hacks. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** saved widget cluster; meeting template block; diagram legend component.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CustomObjectTypeRegistry.cpp
- /Users/ryanrentfro/code/markamp/src/core/BuiltInPlugins.h
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp

### P08-T124: Reusable Components Workspace Surface And Controls
**Task Title:** Reusable Components Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose reusable components in the main canvas workspace.
**Description:** Build the primary UI surfaces for support repeated insertion of high-value modules without external copy-paste hacks., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** saved widget cluster; meeting template block; diagram legend component.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CustomObjectTypeRegistry.cpp
- /Users/ryanrentfro/code/markamp/src/core/BuiltInPlugins.h
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp

### P08-T125: Reusable Components Pointer Interaction Flow
**Task Title:** Reusable Components Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for reusable components.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for support repeated insertion of high-value modules without external copy-paste hacks.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** saved widget cluster; meeting template block; diagram legend component.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CustomObjectTypeRegistry.cpp
- /Users/ryanrentfro/code/markamp/src/core/BuiltInPlugins.h
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp

### P08-T126: Reusable Components Keyboard Gesture And Shortcut Flow
**Task Title:** Reusable Components Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for reusable components.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for support repeated insertion of high-value modules without external copy-paste hacks.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** saved widget cluster; meeting template block; diagram legend component.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CustomObjectTypeRegistry.cpp
- /Users/ryanrentfro/code/markamp/src/core/BuiltInPlugins.h
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp

### P08-T127: Reusable Components Visual Feedback And Rendering Polish
**Task Title:** Reusable Components Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make reusable components legible in use.
**Description:** Implement the visible feedback for support repeated insertion of high-value modules without external copy-paste hacks.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** saved widget cluster; meeting template block; diagram legend component.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CustomObjectTypeRegistry.cpp
- /Users/ryanrentfro/code/markamp/src/core/BuiltInPlugins.h
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp

### P08-T128: Reusable Components Persistence Preferences And Serialization
**Task Title:** Reusable Components Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by reusable components.
**Description:** Identify what parts of support repeated insertion of high-value modules without external copy-paste hacks. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** saved widget cluster; meeting template block; diagram legend component.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CustomObjectTypeRegistry.cpp
- /Users/ryanrentfro/code/markamp/src/core/BuiltInPlugins.h
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp

### P08-T129: Reusable Components Safeguards Telemetry And Recovery
**Task Title:** Reusable Components Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for reusable components.
**Description:** Instrument support repeated insertion of high-value modules without external copy-paste hacks. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** saved widget cluster; meeting template block; diagram legend component.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CustomObjectTypeRegistry.cpp
- /Users/ryanrentfro/code/markamp/src/core/BuiltInPlugins.h
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp

### P08-T130: Reusable Components Tests Documentation And Rollout Gate
**Task Title:** Reusable Components Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship reusable components.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for support repeated insertion of high-value modules without external copy-paste hacks.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** saved widget cluster; meeting template block; diagram legend component.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CustomObjectTypeRegistry.cpp
- /Users/ryanrentfro/code/markamp/src/core/BuiltInPlugins.h
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp

## Workstream 14: Plugin Hooks
Make canvas automation and creation flows extensible through stable integration seams.

### P08-T131: Plugin Hooks UX Parity Audit
**Task Title:** Plugin Hooks UX Parity Audit
**Definition:** Define the implementation contract for plugin hooks by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around make canvas automation and creation flows extensible through stable integration seams.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** plugin adds canvas object; plugin contributes template; plugin registers slash action.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/ContributionValidator.cpp
- /Users/ryanrentfro/code/markamp/src/core/PluginManager.cpp
- /Users/ryanrentfro/code/markamp/src/core/ContextKeyService.cpp

### P08-T132: Plugin Hooks Domain Model And State Contract
**Task Title:** Plugin Hooks Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support plugin hooks.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around make canvas automation and creation flows extensible through stable integration seams.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** plugin adds canvas object; plugin contributes template; plugin registers slash action.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/ContributionValidator.cpp
- /Users/ryanrentfro/code/markamp/src/core/PluginManager.cpp
- /Users/ryanrentfro/code/markamp/src/core/ContextKeyService.cpp

### P08-T133: Plugin Hooks Commands Events And Context Keys
**Task Title:** Plugin Hooks Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for plugin hooks.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make make canvas automation and creation flows extensible through stable integration seams. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** plugin adds canvas object; plugin contributes template; plugin registers slash action.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/ContributionValidator.cpp
- /Users/ryanrentfro/code/markamp/src/core/PluginManager.cpp
- /Users/ryanrentfro/code/markamp/src/core/ContextKeyService.cpp

### P08-T134: Plugin Hooks Workspace Surface And Controls
**Task Title:** Plugin Hooks Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose plugin hooks in the main canvas workspace.
**Description:** Build the primary UI surfaces for make canvas automation and creation flows extensible through stable integration seams., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** plugin adds canvas object; plugin contributes template; plugin registers slash action.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/ContributionValidator.cpp
- /Users/ryanrentfro/code/markamp/src/core/PluginManager.cpp
- /Users/ryanrentfro/code/markamp/src/core/ContextKeyService.cpp

### P08-T135: Plugin Hooks Pointer Interaction Flow
**Task Title:** Plugin Hooks Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for plugin hooks.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for make canvas automation and creation flows extensible through stable integration seams.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** plugin adds canvas object; plugin contributes template; plugin registers slash action.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/ContributionValidator.cpp
- /Users/ryanrentfro/code/markamp/src/core/PluginManager.cpp
- /Users/ryanrentfro/code/markamp/src/core/ContextKeyService.cpp

### P08-T136: Plugin Hooks Keyboard Gesture And Shortcut Flow
**Task Title:** Plugin Hooks Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for plugin hooks.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for make canvas automation and creation flows extensible through stable integration seams.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** plugin adds canvas object; plugin contributes template; plugin registers slash action.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/ContributionValidator.cpp
- /Users/ryanrentfro/code/markamp/src/core/PluginManager.cpp
- /Users/ryanrentfro/code/markamp/src/core/ContextKeyService.cpp

### P08-T137: Plugin Hooks Visual Feedback And Rendering Polish
**Task Title:** Plugin Hooks Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make plugin hooks legible in use.
**Description:** Implement the visible feedback for make canvas automation and creation flows extensible through stable integration seams.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** plugin adds canvas object; plugin contributes template; plugin registers slash action.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/ContributionValidator.cpp
- /Users/ryanrentfro/code/markamp/src/core/PluginManager.cpp
- /Users/ryanrentfro/code/markamp/src/core/ContextKeyService.cpp

### P08-T138: Plugin Hooks Persistence Preferences And Serialization
**Task Title:** Plugin Hooks Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by plugin hooks.
**Description:** Identify what parts of make canvas automation and creation flows extensible through stable integration seams. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** plugin adds canvas object; plugin contributes template; plugin registers slash action.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/ContributionValidator.cpp
- /Users/ryanrentfro/code/markamp/src/core/PluginManager.cpp
- /Users/ryanrentfro/code/markamp/src/core/ContextKeyService.cpp

### P08-T139: Plugin Hooks Safeguards Telemetry And Recovery
**Task Title:** Plugin Hooks Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for plugin hooks.
**Description:** Instrument make canvas automation and creation flows extensible through stable integration seams. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** plugin adds canvas object; plugin contributes template; plugin registers slash action.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/ContributionValidator.cpp
- /Users/ryanrentfro/code/markamp/src/core/PluginManager.cpp
- /Users/ryanrentfro/code/markamp/src/core/ContextKeyService.cpp

### P08-T140: Plugin Hooks Tests Documentation And Rollout Gate
**Task Title:** Plugin Hooks Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship plugin hooks.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for make canvas automation and creation flows extensible through stable integration seams.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** plugin adds canvas object; plugin contributes template; plugin registers slash action.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/ContributionValidator.cpp
- /Users/ryanrentfro/code/markamp/src/core/PluginManager.cpp
- /Users/ryanrentfro/code/markamp/src/core/ContextKeyService.cpp

## Workstream 15: Command Macros
Allow reusable multi-step board actions for power users and team admins.

### P08-T141: Command Macros UX Parity Audit
**Task Title:** Command Macros UX Parity Audit
**Definition:** Define the implementation contract for command macros by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around allow reusable multi-step board actions for power users and team admins.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** prepare review board; normalize sticky wall; publish selected frame.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationCommandProvider.h
- /Users/ryanrentfro/code/markamp/src/core/Command.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.h

### P08-T142: Command Macros Domain Model And State Contract
**Task Title:** Command Macros Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support command macros.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around allow reusable multi-step board actions for power users and team admins.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** prepare review board; normalize sticky wall; publish selected frame.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationCommandProvider.h
- /Users/ryanrentfro/code/markamp/src/core/Command.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.h

### P08-T143: Command Macros Commands Events And Context Keys
**Task Title:** Command Macros Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for command macros.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make allow reusable multi-step board actions for power users and team admins. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** prepare review board; normalize sticky wall; publish selected frame.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationCommandProvider.h
- /Users/ryanrentfro/code/markamp/src/core/Command.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.h

### P08-T144: Command Macros Workspace Surface And Controls
**Task Title:** Command Macros Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose command macros in the main canvas workspace.
**Description:** Build the primary UI surfaces for allow reusable multi-step board actions for power users and team admins., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** prepare review board; normalize sticky wall; publish selected frame.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationCommandProvider.h
- /Users/ryanrentfro/code/markamp/src/core/Command.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.h

### P08-T145: Command Macros Pointer Interaction Flow
**Task Title:** Command Macros Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for command macros.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for allow reusable multi-step board actions for power users and team admins.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** prepare review board; normalize sticky wall; publish selected frame.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationCommandProvider.h
- /Users/ryanrentfro/code/markamp/src/core/Command.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.h

### P08-T146: Command Macros Keyboard Gesture And Shortcut Flow
**Task Title:** Command Macros Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for command macros.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for allow reusable multi-step board actions for power users and team admins.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** prepare review board; normalize sticky wall; publish selected frame.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationCommandProvider.h
- /Users/ryanrentfro/code/markamp/src/core/Command.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.h

### P08-T147: Command Macros Visual Feedback And Rendering Polish
**Task Title:** Command Macros Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make command macros legible in use.
**Description:** Implement the visible feedback for allow reusable multi-step board actions for power users and team admins.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** prepare review board; normalize sticky wall; publish selected frame.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationCommandProvider.h
- /Users/ryanrentfro/code/markamp/src/core/Command.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.h

### P08-T148: Command Macros Persistence Preferences And Serialization
**Task Title:** Command Macros Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by command macros.
**Description:** Identify what parts of allow reusable multi-step board actions for power users and team admins. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** prepare review board; normalize sticky wall; publish selected frame.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationCommandProvider.h
- /Users/ryanrentfro/code/markamp/src/core/Command.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.h

### P08-T149: Command Macros Safeguards Telemetry And Recovery
**Task Title:** Command Macros Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for command macros.
**Description:** Instrument allow reusable multi-step board actions for power users and team admins. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** prepare review board; normalize sticky wall; publish selected frame.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationCommandProvider.h
- /Users/ryanrentfro/code/markamp/src/core/Command.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.h

### P08-T150: Command Macros Tests Documentation And Rollout Gate
**Task Title:** Command Macros Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship command macros.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for allow reusable multi-step board actions for power users and team admins.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** prepare review board; normalize sticky wall; publish selected frame.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationCommandProvider.h
- /Users/ryanrentfro/code/markamp/src/core/Command.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.h

## Workstream 16: Quick Insert
Make insertion faster through slash menus and context-aware quick-add surfaces.

### P08-T151: Quick Insert UX Parity Audit
**Task Title:** Quick Insert UX Parity Audit
**Definition:** Define the implementation contract for quick insert by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around make insertion faster through slash menus and context-aware quick-add surfaces.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** slash create note; quick insert connector label; insert recent asset.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasContextMenu.h
- /Users/ryanrentfro/code/markamp/src/ui/FloatingToolbarManager.cpp

### P08-T152: Quick Insert Domain Model And State Contract
**Task Title:** Quick Insert Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support quick insert.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around make insertion faster through slash menus and context-aware quick-add surfaces.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** slash create note; quick insert connector label; insert recent asset.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasContextMenu.h
- /Users/ryanrentfro/code/markamp/src/ui/FloatingToolbarManager.cpp

### P08-T153: Quick Insert Commands Events And Context Keys
**Task Title:** Quick Insert Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for quick insert.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make make insertion faster through slash menus and context-aware quick-add surfaces. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** slash create note; quick insert connector label; insert recent asset.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasContextMenu.h
- /Users/ryanrentfro/code/markamp/src/ui/FloatingToolbarManager.cpp

### P08-T154: Quick Insert Workspace Surface And Controls
**Task Title:** Quick Insert Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose quick insert in the main canvas workspace.
**Description:** Build the primary UI surfaces for make insertion faster through slash menus and context-aware quick-add surfaces., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** slash create note; quick insert connector label; insert recent asset.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasContextMenu.h
- /Users/ryanrentfro/code/markamp/src/ui/FloatingToolbarManager.cpp

### P08-T155: Quick Insert Pointer Interaction Flow
**Task Title:** Quick Insert Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for quick insert.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for make insertion faster through slash menus and context-aware quick-add surfaces.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** slash create note; quick insert connector label; insert recent asset.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasContextMenu.h
- /Users/ryanrentfro/code/markamp/src/ui/FloatingToolbarManager.cpp

### P08-T156: Quick Insert Keyboard Gesture And Shortcut Flow
**Task Title:** Quick Insert Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for quick insert.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for make insertion faster through slash menus and context-aware quick-add surfaces.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** slash create note; quick insert connector label; insert recent asset.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasContextMenu.h
- /Users/ryanrentfro/code/markamp/src/ui/FloatingToolbarManager.cpp

### P08-T157: Quick Insert Visual Feedback And Rendering Polish
**Task Title:** Quick Insert Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make quick insert legible in use.
**Description:** Implement the visible feedback for make insertion faster through slash menus and context-aware quick-add surfaces.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** slash create note; quick insert connector label; insert recent asset.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasContextMenu.h
- /Users/ryanrentfro/code/markamp/src/ui/FloatingToolbarManager.cpp

### P08-T158: Quick Insert Persistence Preferences And Serialization
**Task Title:** Quick Insert Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by quick insert.
**Description:** Identify what parts of make insertion faster through slash menus and context-aware quick-add surfaces. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** slash create note; quick insert connector label; insert recent asset.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasContextMenu.h
- /Users/ryanrentfro/code/markamp/src/ui/FloatingToolbarManager.cpp

### P08-T159: Quick Insert Safeguards Telemetry And Recovery
**Task Title:** Quick Insert Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for quick insert.
**Description:** Instrument make insertion faster through slash menus and context-aware quick-add surfaces. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** slash create note; quick insert connector label; insert recent asset.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasContextMenu.h
- /Users/ryanrentfro/code/markamp/src/ui/FloatingToolbarManager.cpp

### P08-T160: Quick Insert Tests Documentation And Rollout Gate
**Task Title:** Quick Insert Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship quick insert.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for make insertion faster through slash menus and context-aware quick-add surfaces.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** slash create note; quick insert connector label; insert recent asset.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasContextMenu.h
- /Users/ryanrentfro/code/markamp/src/ui/FloatingToolbarManager.cpp

## Workstream 17: Smart Defaults
Bias the UI toward likely next actions without becoming opaque.

### P08-T161: Smart Defaults UX Parity Audit
**Task Title:** Smart Defaults UX Parity Audit
**Definition:** Define the implementation contract for smart defaults by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around bias the ui toward likely next actions without becoming opaque.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** remember last note color; default connector style by diagram; auto-open text editor after create.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AppState.cpp
- /Users/ryanrentfro/code/markamp/src/core/Config.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.h

### P08-T162: Smart Defaults Domain Model And State Contract
**Task Title:** Smart Defaults Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support smart defaults.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around bias the ui toward likely next actions without becoming opaque.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** remember last note color; default connector style by diagram; auto-open text editor after create.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AppState.cpp
- /Users/ryanrentfro/code/markamp/src/core/Config.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.h

### P08-T163: Smart Defaults Commands Events And Context Keys
**Task Title:** Smart Defaults Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for smart defaults.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make bias the ui toward likely next actions without becoming opaque. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** remember last note color; default connector style by diagram; auto-open text editor after create.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AppState.cpp
- /Users/ryanrentfro/code/markamp/src/core/Config.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.h

### P08-T164: Smart Defaults Workspace Surface And Controls
**Task Title:** Smart Defaults Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose smart defaults in the main canvas workspace.
**Description:** Build the primary UI surfaces for bias the ui toward likely next actions without becoming opaque., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** remember last note color; default connector style by diagram; auto-open text editor after create.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AppState.cpp
- /Users/ryanrentfro/code/markamp/src/core/Config.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.h

### P08-T165: Smart Defaults Pointer Interaction Flow
**Task Title:** Smart Defaults Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for smart defaults.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for bias the ui toward likely next actions without becoming opaque.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** remember last note color; default connector style by diagram; auto-open text editor after create.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AppState.cpp
- /Users/ryanrentfro/code/markamp/src/core/Config.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.h

### P08-T166: Smart Defaults Keyboard Gesture And Shortcut Flow
**Task Title:** Smart Defaults Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for smart defaults.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for bias the ui toward likely next actions without becoming opaque.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** remember last note color; default connector style by diagram; auto-open text editor after create.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AppState.cpp
- /Users/ryanrentfro/code/markamp/src/core/Config.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.h

### P08-T167: Smart Defaults Visual Feedback And Rendering Polish
**Task Title:** Smart Defaults Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make smart defaults legible in use.
**Description:** Implement the visible feedback for bias the ui toward likely next actions without becoming opaque.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** remember last note color; default connector style by diagram; auto-open text editor after create.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AppState.cpp
- /Users/ryanrentfro/code/markamp/src/core/Config.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.h

### P08-T168: Smart Defaults Persistence Preferences And Serialization
**Task Title:** Smart Defaults Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by smart defaults.
**Description:** Identify what parts of bias the ui toward likely next actions without becoming opaque. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** remember last note color; default connector style by diagram; auto-open text editor after create.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AppState.cpp
- /Users/ryanrentfro/code/markamp/src/core/Config.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.h

### P08-T169: Smart Defaults Safeguards Telemetry And Recovery
**Task Title:** Smart Defaults Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for smart defaults.
**Description:** Instrument bias the ui toward likely next actions without becoming opaque. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** remember last note color; default connector style by diagram; auto-open text editor after create.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AppState.cpp
- /Users/ryanrentfro/code/markamp/src/core/Config.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.h

### P08-T170: Smart Defaults Tests Documentation And Rollout Gate
**Task Title:** Smart Defaults Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship smart defaults.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for bias the ui toward likely next actions without becoming opaque.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** remember last note color; default connector style by diagram; auto-open text editor after create.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AppState.cpp
- /Users/ryanrentfro/code/markamp/src/core/Config.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.h

## Workstream 18: Profile And Preference Sync
Carry team and personal defaults across boards and devices where appropriate.

### P08-T171: Profile And Preference Sync UX Parity Audit
**Task Title:** Profile And Preference Sync UX Parity Audit
**Definition:** Define the implementation contract for profile and preference sync by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around carry team and personal defaults across boards and devices where appropriate.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** sync style presets; workspace template pack; shared board defaults.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigProfile.h
- /Users/ryanrentfro/code/markamp/src/core/CloudSyncService.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasThemeModel.cpp

### P08-T172: Profile And Preference Sync Domain Model And State Contract
**Task Title:** Profile And Preference Sync Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support profile and preference sync.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around carry team and personal defaults across boards and devices where appropriate.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** sync style presets; workspace template pack; shared board defaults.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigProfile.h
- /Users/ryanrentfro/code/markamp/src/core/CloudSyncService.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasThemeModel.cpp

### P08-T173: Profile And Preference Sync Commands Events And Context Keys
**Task Title:** Profile And Preference Sync Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for profile and preference sync.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make carry team and personal defaults across boards and devices where appropriate. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** sync style presets; workspace template pack; shared board defaults.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigProfile.h
- /Users/ryanrentfro/code/markamp/src/core/CloudSyncService.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasThemeModel.cpp

### P08-T174: Profile And Preference Sync Workspace Surface And Controls
**Task Title:** Profile And Preference Sync Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose profile and preference sync in the main canvas workspace.
**Description:** Build the primary UI surfaces for carry team and personal defaults across boards and devices where appropriate., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** sync style presets; workspace template pack; shared board defaults.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigProfile.h
- /Users/ryanrentfro/code/markamp/src/core/CloudSyncService.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasThemeModel.cpp

### P08-T175: Profile And Preference Sync Pointer Interaction Flow
**Task Title:** Profile And Preference Sync Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for profile and preference sync.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for carry team and personal defaults across boards and devices where appropriate.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** sync style presets; workspace template pack; shared board defaults.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigProfile.h
- /Users/ryanrentfro/code/markamp/src/core/CloudSyncService.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasThemeModel.cpp

### P08-T176: Profile And Preference Sync Keyboard Gesture And Shortcut Flow
**Task Title:** Profile And Preference Sync Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for profile and preference sync.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for carry team and personal defaults across boards and devices where appropriate.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** sync style presets; workspace template pack; shared board defaults.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigProfile.h
- /Users/ryanrentfro/code/markamp/src/core/CloudSyncService.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasThemeModel.cpp

### P08-T177: Profile And Preference Sync Visual Feedback And Rendering Polish
**Task Title:** Profile And Preference Sync Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make profile and preference sync legible in use.
**Description:** Implement the visible feedback for carry team and personal defaults across boards and devices where appropriate.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** sync style presets; workspace template pack; shared board defaults.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigProfile.h
- /Users/ryanrentfro/code/markamp/src/core/CloudSyncService.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasThemeModel.cpp

### P08-T178: Profile And Preference Sync Persistence Preferences And Serialization
**Task Title:** Profile And Preference Sync Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by profile and preference sync.
**Description:** Identify what parts of carry team and personal defaults across boards and devices where appropriate. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** sync style presets; workspace template pack; shared board defaults.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigProfile.h
- /Users/ryanrentfro/code/markamp/src/core/CloudSyncService.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasThemeModel.cpp

### P08-T179: Profile And Preference Sync Safeguards Telemetry And Recovery
**Task Title:** Profile And Preference Sync Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for profile and preference sync.
**Description:** Instrument carry team and personal defaults across boards and devices where appropriate. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** sync style presets; workspace template pack; shared board defaults.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigProfile.h
- /Users/ryanrentfro/code/markamp/src/core/CloudSyncService.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasThemeModel.cpp

### P08-T180: Profile And Preference Sync Tests Documentation And Rollout Gate
**Task Title:** Profile And Preference Sync Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship profile and preference sync.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for carry team and personal defaults across boards and devices where appropriate.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** sync style presets; workspace template pack; shared board defaults.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigProfile.h
- /Users/ryanrentfro/code/markamp/src/core/CloudSyncService.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasThemeModel.cpp

## Workstream 19: Onboarding Programs
Use checklists and progressive disclosure to shorten time-to-value for new creators.

### P08-T181: Onboarding Programs UX Parity Audit
**Task Title:** Onboarding Programs UX Parity Audit
**Definition:** Define the implementation contract for onboarding programs by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around use checklists and progressive disclosure to shorten time-to-value for new creators.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** template recommendations; first board checklist; coachmark sequencing.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OnboardingModel.h
- /Users/ryanrentfro/code/markamp/src/ui/FirstRunWizard.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DialogModel.h

### P08-T182: Onboarding Programs Domain Model And State Contract
**Task Title:** Onboarding Programs Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support onboarding programs.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around use checklists and progressive disclosure to shorten time-to-value for new creators.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** template recommendations; first board checklist; coachmark sequencing.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OnboardingModel.h
- /Users/ryanrentfro/code/markamp/src/ui/FirstRunWizard.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DialogModel.h

### P08-T183: Onboarding Programs Commands Events And Context Keys
**Task Title:** Onboarding Programs Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for onboarding programs.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make use checklists and progressive disclosure to shorten time-to-value for new creators. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** template recommendations; first board checklist; coachmark sequencing.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OnboardingModel.h
- /Users/ryanrentfro/code/markamp/src/ui/FirstRunWizard.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DialogModel.h

### P08-T184: Onboarding Programs Workspace Surface And Controls
**Task Title:** Onboarding Programs Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose onboarding programs in the main canvas workspace.
**Description:** Build the primary UI surfaces for use checklists and progressive disclosure to shorten time-to-value for new creators., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** template recommendations; first board checklist; coachmark sequencing.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OnboardingModel.h
- /Users/ryanrentfro/code/markamp/src/ui/FirstRunWizard.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DialogModel.h

### P08-T185: Onboarding Programs Pointer Interaction Flow
**Task Title:** Onboarding Programs Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for onboarding programs.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for use checklists and progressive disclosure to shorten time-to-value for new creators.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** template recommendations; first board checklist; coachmark sequencing.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OnboardingModel.h
- /Users/ryanrentfro/code/markamp/src/ui/FirstRunWizard.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DialogModel.h

### P08-T186: Onboarding Programs Keyboard Gesture And Shortcut Flow
**Task Title:** Onboarding Programs Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for onboarding programs.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for use checklists and progressive disclosure to shorten time-to-value for new creators.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** template recommendations; first board checklist; coachmark sequencing.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OnboardingModel.h
- /Users/ryanrentfro/code/markamp/src/ui/FirstRunWizard.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DialogModel.h

### P08-T187: Onboarding Programs Visual Feedback And Rendering Polish
**Task Title:** Onboarding Programs Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make onboarding programs legible in use.
**Description:** Implement the visible feedback for use checklists and progressive disclosure to shorten time-to-value for new creators.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** template recommendations; first board checklist; coachmark sequencing.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OnboardingModel.h
- /Users/ryanrentfro/code/markamp/src/ui/FirstRunWizard.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DialogModel.h

### P08-T188: Onboarding Programs Persistence Preferences And Serialization
**Task Title:** Onboarding Programs Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by onboarding programs.
**Description:** Identify what parts of use checklists and progressive disclosure to shorten time-to-value for new creators. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** template recommendations; first board checklist; coachmark sequencing.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OnboardingModel.h
- /Users/ryanrentfro/code/markamp/src/ui/FirstRunWizard.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DialogModel.h

### P08-T189: Onboarding Programs Safeguards Telemetry And Recovery
**Task Title:** Onboarding Programs Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for onboarding programs.
**Description:** Instrument use checklists and progressive disclosure to shorten time-to-value for new creators. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** template recommendations; first board checklist; coachmark sequencing.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OnboardingModel.h
- /Users/ryanrentfro/code/markamp/src/ui/FirstRunWizard.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DialogModel.h

### P08-T190: Onboarding Programs Tests Documentation And Rollout Gate
**Task Title:** Onboarding Programs Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship onboarding programs.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for use checklists and progressive disclosure to shorten time-to-value for new creators.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** template recommendations; first board checklist; coachmark sequencing.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OnboardingModel.h
- /Users/ryanrentfro/code/markamp/src/ui/FirstRunWizard.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DialogModel.h

## Workstream 20: Agent Ready Scaffolds
Document creation flows so AI coding agents can implement them incrementally.

### P08-T191: Agent Ready Scaffolds UX Parity Audit
**Task Title:** Agent Ready Scaffolds UX Parity Audit
**Definition:** Define the implementation contract for agent ready scaffolds by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around document creation flows so ai coding agents can implement them incrementally.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** task scaffolds per template; acceptance checklist blocks; reference mapping.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/docs
- /Users/ryanrentfro/code/markamp/src/core/CrossModuleTestSuite.h

### P08-T192: Agent Ready Scaffolds Domain Model And State Contract
**Task Title:** Agent Ready Scaffolds Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support agent ready scaffolds.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around document creation flows so ai coding agents can implement them incrementally.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** task scaffolds per template; acceptance checklist blocks; reference mapping.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/docs
- /Users/ryanrentfro/code/markamp/src/core/CrossModuleTestSuite.h

### P08-T193: Agent Ready Scaffolds Commands Events And Context Keys
**Task Title:** Agent Ready Scaffolds Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for agent ready scaffolds.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make document creation flows so ai coding agents can implement them incrementally. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** task scaffolds per template; acceptance checklist blocks; reference mapping.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/docs
- /Users/ryanrentfro/code/markamp/src/core/CrossModuleTestSuite.h

### P08-T194: Agent Ready Scaffolds Workspace Surface And Controls
**Task Title:** Agent Ready Scaffolds Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose agent ready scaffolds in the main canvas workspace.
**Description:** Build the primary UI surfaces for document creation flows so ai coding agents can implement them incrementally., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** task scaffolds per template; acceptance checklist blocks; reference mapping.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/docs
- /Users/ryanrentfro/code/markamp/src/core/CrossModuleTestSuite.h

### P08-T195: Agent Ready Scaffolds Pointer Interaction Flow
**Task Title:** Agent Ready Scaffolds Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for agent ready scaffolds.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for document creation flows so ai coding agents can implement them incrementally.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** task scaffolds per template; acceptance checklist blocks; reference mapping.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/docs
- /Users/ryanrentfro/code/markamp/src/core/CrossModuleTestSuite.h

### P08-T196: Agent Ready Scaffolds Keyboard Gesture And Shortcut Flow
**Task Title:** Agent Ready Scaffolds Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for agent ready scaffolds.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for document creation flows so ai coding agents can implement them incrementally.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** task scaffolds per template; acceptance checklist blocks; reference mapping.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/docs
- /Users/ryanrentfro/code/markamp/src/core/CrossModuleTestSuite.h

### P08-T197: Agent Ready Scaffolds Visual Feedback And Rendering Polish
**Task Title:** Agent Ready Scaffolds Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make agent ready scaffolds legible in use.
**Description:** Implement the visible feedback for document creation flows so ai coding agents can implement them incrementally.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** task scaffolds per template; acceptance checklist blocks; reference mapping.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/docs
- /Users/ryanrentfro/code/markamp/src/core/CrossModuleTestSuite.h

### P08-T198: Agent Ready Scaffolds Persistence Preferences And Serialization
**Task Title:** Agent Ready Scaffolds Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by agent ready scaffolds.
**Description:** Identify what parts of document creation flows so ai coding agents can implement them incrementally. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** task scaffolds per template; acceptance checklist blocks; reference mapping.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/docs
- /Users/ryanrentfro/code/markamp/src/core/CrossModuleTestSuite.h

### P08-T199: Agent Ready Scaffolds Safeguards Telemetry And Recovery
**Task Title:** Agent Ready Scaffolds Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for agent ready scaffolds.
**Description:** Instrument document creation flows so ai coding agents can implement them incrementally. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** task scaffolds per template; acceptance checklist blocks; reference mapping.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/docs
- /Users/ryanrentfro/code/markamp/src/core/CrossModuleTestSuite.h

### P08-T200: Agent Ready Scaffolds Tests Documentation And Rollout Gate
**Task Title:** Agent Ready Scaffolds Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship agent ready scaffolds.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for document creation flows so ai coding agents can implement them incrementally.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** task scaffolds per template; acceptance checklist blocks; reference mapping.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AIDocumentGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/docs
- /Users/ryanrentfro/code/markamp/src/core/CrossModuleTestSuite.h

## Phase Exit Criteria
- All 200 tasks have either shipped, been explicitly deferred with rationale, or been converted into implementation issues with owners.
- Critical workflows in this phase have unit, integration, and at least one end-to-end validation path or a documented gap.
- The shipped work is theme-aware, accessibility-aware, serialization-safe, and undo/redo-safe by default.
- The phase produces measurable progress toward Miro-level editing, drawing, moving, and content authoring quality.
