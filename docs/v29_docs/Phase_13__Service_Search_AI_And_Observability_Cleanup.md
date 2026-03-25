# Phase 13: Service Search AI And Observability Cleanup

## Task Count

- 2 tasks

## Task V29-P13-T01

- Phase ID: V29-P13
- Task ID: V29-P13-T01
- Task Title: Remove AI, search, indexing, and background-service branches that only support retired features
- Priority: Medium
- Category: Service / Model Removal
- Objective: Delete service branches that still process notebooks, boards, flashcards, or tasks in search, AI, indexing, and background flows.
- Why This Matters Now: Hidden service branches can keep dead data models, background jobs, and feature vocabulary alive after UI deletion.
- Removal Gap Statement: AI and service layers still mention flashcards and task workflows, and notebook/canvas search or indexing helpers remain in core.
- User / Product Impact: Improves performance predictability and reduces stale background behavior.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/core/AICommandProvider.cpp:194`; `/Users/ryanrentfro/code/markamp/src/core/NotebookSearchIndex.cpp`; `/Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp`; `/Users/ryanrentfro/code/markamp/src/core/TaskAggregator.cpp`
- Prior Plan References: `/Users/ryanrentfro/code/markamp/docs/v23_docs/Phase_17__Service_Stub_Replacement_And_Interface_Adoption_Completion.md`; `/Users/ryanrentfro/code/markamp/docs/v24_docs/Phase_17__Advanced_Domain_Workflows_AV_Graph_Tasks_Presentation_And_Node_Editor.md`
- Scope: AI commands, search/indexing helpers, aggregators, background services, and service ownership tied only to retired features.
- Out of Scope: Retained AI writing and retained document search.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/AICommandProvider.cpp`; `/Users/ryanrentfro/code/markamp/src/core/NotebookSearchIndex.cpp`; `/Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp`; `/Users/ryanrentfro/code/markamp/src/core/TaskAggregator.cpp`
- Related Features / Systems / Components: AI, search, indexing, aggregators, background services.
- Current Behavior / Presence: Retired domains still influence AI menus and indexing/service branches.
- Intended Post-Removal Behavior: Services only process retained document and retained product data.
- Removal / Simplification Direction: Delete feature-specific service branches and collapse generic services to retained data models.
- Technical Approach: Remove feature-owned service classes, then simplify remaining callers and background registration points.
- Implementation Steps: Remove AI flashcard command; delete notebook/canvas search helpers; delete task aggregator service; update search and AI tests.
- Validation Steps: Search production code for `NotebookSearchIndex`, `CanvasSearch`, `AIFlashcardGenerator`, and task aggregators; confirm no active usage remains.
- Acceptance Criteria: No AI, search, or service branch exists solely for a retired feature.
- Dependencies: Phases 06 through 09.
- Parallelization Notes: Can run with observability cleanup below.
- Risks / Failure Modes: Some search or AI code may have shared abstractions that need trimming rather than total removal.
- Migration / Compatibility Notes: Search indexes for old removed artifacts should be ignored or purged.
- UX / Layout Cleanup Notes: Remove search suggestions or AI prompts that mention removed domains.
- Cleanup / Consolidation Notes: Shrink service-registration and background-loop complexity.
- Rollback / Safety Notes: Preserve retained AI and search coverage while deleting branches.
- References / Context: This phase removes hidden domain-specific intelligence after visible product retirement.
- Example Scenarios: Search no longer indexes notebook outputs or canvas objects; AI no longer suggests flashcards.

## Task V29-P13-T02

- Phase ID: V29-P13
- Task ID: V29-P13-T02
- Task Title: Remove retired-feature telemetry, logging, notification, and health-check paths
- Priority: Medium
- Category: Cleanup / Dead Code Retirement
- Objective: Delete observability and notification paths that only report removed-feature activity.
- Why This Matters Now: Logging and telemetry residue makes removed features look supported and preserves dead event handling.
- Removal Gap Statement: Event families and completion auditors for notebook and canvas, plus task and flashcard notifications, remain part of the observability surface.
- User / Product Impact: Prevents confusing notifications and reduces maintenance noise.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/core/CanvasCompletionAuditor.cpp`; `/Users/ryanrentfro/code/markamp/src/core/NotebookExecutionCompletionAuditor.cpp`; `/Users/ryanrentfro/code/markamp/src/core/Events.h`
- Prior Plan References: `/Users/ryanrentfro/code/markamp/docs/v18_docs/Phase_09__Feedback_Status_Recovery_And_Observability_UX.md`; `/Users/ryanrentfro/code/markamp/docs/v19_docs/Phase_09__Feedback_Observability_And_Regression_Harnesses.md`
- Scope: Feature-owned completion auditors, notifications, telemetry events, status indicators, and health checks for retired domains.
- Out of Scope: Generic retained observability infrastructure.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/CanvasCompletionAuditor.cpp`; `/Users/ryanrentfro/code/markamp/src/core/NotebookExecutionCompletionAuditor.cpp`; `/Users/ryanrentfro/code/markamp/src/core/Events.h`; `/Users/ryanrentfro/code/markamp/src/ui/StatusBarPanel.cpp`
- Related Features / Systems / Components: Logging, notifications, health checks, auditors, status bar.
- Current Behavior / Presence: Removed features still publish or prepare observability paths and status indicators.
- Intended Post-Removal Behavior: No runtime notification or telemetry pathway implies removed features are active.
- Removal / Simplification Direction: Delete feature-owned observability, keep only migration notices where explicitly needed.
- Technical Approach: Remove auditors and listeners after event family cleanup and shell cleanup.
- Implementation Steps: Delete completion auditors; remove status-bar references; prune telemetry/logging branches; update tests.
- Validation Steps: Search for retired-feature auditor classes and status labels; verify no removed-feature notifications fire during startup or shell use.
- Acceptance Criteria: Observability surface contains no active references to retired features.
- Dependencies: Phase 10.
- Parallelization Notes: Small, safe cleanup after feature deletion.
- Risks / Failure Modes: Hidden references in logging macros or tests may survive if not searched broadly.
- Migration / Compatibility Notes: A single startup notification for dropped legacy state is acceptable if explicitly designed.
- UX / Layout Cleanup Notes: Notification center and status bar should not mention removed features.
- Cleanup / Consolidation Notes: Remove dead metric counters and dashboards.
- Rollback / Safety Notes: Keep generic app health reporting intact.
- References / Context: Observability was expanded in prior passes and must now be narrowed.
- Example Scenarios: Startup no longer logs notebook or canvas completion audits.
