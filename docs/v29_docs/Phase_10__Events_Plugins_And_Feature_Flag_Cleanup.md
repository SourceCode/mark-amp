# Phase 10: Events Plugins And Feature Flag Cleanup

## Task Count

- 2 tasks

## Task V29-P10-T01

- Phase ID: V29-P10
- Task ID: V29-P10-T01
- Task Title: Remove retired-feature event families and generic listeners from the event bus surface
- Priority: High
- Category: Cleanup / Dead Code Retirement
- Objective: Delete notebook, canvas, flashcard/FSRS, task, kanban, and calendar event declarations and their listeners once feature-owned callers are gone.
- Why This Matters Now: `Events.h` is a central dependency surface; leaving dead event families behind preserves compile, testing, and conceptual noise.
- Removal Gap Statement: `/Users/ryanrentfro/code/markamp/src/core/Events.h` still contains large dedicated event families across all four retired domains.
- User / Product Impact: Reduces internal complexity and prevents stale event routing assumptions.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/core/Events.h:1152`; `/Users/ryanrentfro/code/markamp/src/core/Events.h:1687`; `/Users/ryanrentfro/code/markamp/src/core/Events.h:2199`; `/Users/ryanrentfro/code/markamp/src/core/Events.h:2643`; `/Users/ryanrentfro/code/markamp/src/core/Events.h:6438`; `/Users/ryanrentfro/code/markamp/src/core/Events.h:7821`; `/Users/ryanrentfro/code/markamp/src/core/Events.h:7860`
- Prior Plan References: `/Users/ryanrentfro/code/markamp/docs/v19_docs/Phase_01__Workbench_Execution_Spine.md`; `/Users/ryanrentfro/code/markamp/docs/v23_docs/Phase_17__Service_Stub_Replacement_And_Interface_Adoption_Completion.md`
- Scope: Event declarations, event namespaces, listeners, switch statements, and event-driven telemetry or notifications specific to retired features.
- Out of Scope: Generic retained shell or editor events.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/Events.h`; `/Users/ryanrentfro/code/markamp/src/core/EventBus.h`; `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`
- Related Features / Systems / Components: Event bus, listeners, telemetry, notifications.
- Current Behavior / Presence: Removed domains still occupy large space in the canonical event catalog.
- Intended Post-Removal Behavior: Event surface reflects only retained product capabilities and explicit migration notifications.
- Removal / Simplification Direction: Delete event families after their owning code is removed.
- Technical Approach: Remove event declarations in batches aligned to feature deletion and fix remaining listeners by deleting dead branches.
- Implementation Steps: Inventory event types by retired domain; remove declarations; remove publish/subscribe sites; update type-count tests and docs.
- Validation Steps: Search for event type names and ensure remaining references are zero or migration-specific.
- Acceptance Criteria: No active event family remains for retired features.
- Dependencies: Phases 06 through 09.
- Parallelization Notes: Can be split by feature family.
- Risks / Failure Modes: Generic listeners may quietly depend on removed enums or constants.
- Migration / Compatibility Notes: Keep only explicit compatibility-notification events if needed.
- UX / Layout Cleanup Notes: Retire feature-specific notifications and badges with the events.
- Cleanup / Consolidation Notes: Shrink event tests and any event-type ID fixtures.
- Rollback / Safety Notes: Perform after feature code deletion to minimize churn.
- References / Context: Event sprawl is a key hidden dependency in this repository.
- Example Scenarios: No `CanvasModeActivatedEvent`, `NotebookCreatedEvent`, `FlashcardReviewSummaryEvent`, or `TaskReminderDueEvent` remain in production.

## Task V29-P10-T02

- Phase ID: V29-P10
- Task ID: V29-P10-T02
- Task Title: Remove built-in plugins, feature flags, and extension contribution seams tied only to retired features
- Priority: High
- Category: Cleanup / Dead Code Retirement
- Objective: Delete built-in plugins and feature toggles for canvas-derived subfeatures and any extension contribution surfaces that only exist for retired capabilities.
- Why This Matters Now: Plugin and feature-flag residue can keep removed features half-present and complicate settings, startup, and telemetry.
- Removal Gap Statement: `/Users/ryanrentfro/code/markamp/src/core/BuiltInPlugins.cpp` still registers canvas collaboration, canvas apps, kanban, and mind map built-in plugins.
- User / Product Impact: Prevents hidden or re-enabled feature fragments and simplifies product configuration.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/core/BuiltInPlugins.cpp:354`; `/Users/ryanrentfro/code/markamp/src/core/BuiltInPlugins.cpp:384`; `/Users/ryanrentfro/code/markamp/src/core/BuiltInPlugins.cpp:413`; `/Users/ryanrentfro/code/markamp/src/core/BuiltInPlugins.cpp:444`
- Prior Plan References: `/Users/ryanrentfro/code/markamp/docs/v24_docs/Phase_14__Plugin_Extension_Contribution_And_Theme_Marketplace_Completion.md`; `/Users/ryanrentfro/code/markamp/docs/v25_docs/Phase_14__Plugin_Extension_Contribution_And_Gating_Completion.md`
- Scope: Built-in plugins, feature constants, feature registry usage, extension documentation, and contribution validation branches only needed by retired features.
- Out of Scope: Retained plugin infrastructure and extension support.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/BuiltInPlugins.cpp`; `/Users/ryanrentfro/code/markamp/src/core/FeatureRegistry.h`; `/Users/ryanrentfro/code/markamp/src/core/PluginManager.cpp`; `/Users/ryanrentfro/code/markamp/src/core/ContributionValidator.cpp`
- Related Features / Systems / Components: Built-in plugins, feature flags, extension validation, settings visibility.
- Current Behavior / Presence: Canvas-derived features still look shippable through plugin and feature infrastructure.
- Intended Post-Removal Behavior: Plugin and feature systems expose only retained capabilities.
- Removal / Simplification Direction: Delete retired feature toggles and built-ins entirely instead of leaving them disabled.
- Technical Approach: Remove plugin classes and registration sites; prune any settings or validation branches that only served retired features.
- Implementation Steps: Delete built-in plugin classes; remove registration calls; prune feature constants; update plugin tests and docs; check extension contribution validators for dead retired-feature categories.
- Validation Steps: Search for retired plugin names and feature constants; verify no settings or startup code references them.
- Acceptance Criteria: No built-in plugin or feature flag exists solely for retired features.
- Dependencies: Phases 06 and 09.
- Parallelization Notes: Can run in parallel with event cleanup if the write set is separated.
- Risks / Failure Modes: Over-broad plugin cleanup could accidentally remove retained plugin infrastructure.
- Migration / Compatibility Notes: Old persisted feature-toggle values should be ignored safely.
- UX / Layout Cleanup Notes: Remove feature-toggle labels and categories from any settings or diagnostics views.
- Cleanup / Consolidation Notes: This is an opportunity to reduce feature-registry sprawl.
- Rollback / Safety Notes: Avoid touching unrelated built-in features.
- References / Context: Earlier plans elevated these plugin surfaces; v29 retires them as part of full removal.
- Example Scenarios: No `Canvas Collaboration` or `Canvas Apps & Widgets` built-in plugin remains.
