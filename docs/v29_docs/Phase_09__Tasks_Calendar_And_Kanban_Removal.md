# Phase 09: Tasks Calendar And Kanban Removal

## Task Count

- 2 tasks

## Task V29-P09-T01

- Phase ID: V29-P09
- Task ID: V29-P09-T01
- Task Title: Delete task panels, task commands, and user-facing task workflows
- Priority: High
- Category: Tasks Removal
- Objective: Remove all user-facing task list, task command, task board, gantt, and calendar UI/workflows from the product.
- Why This Matters Now: Tasks still occupy visible shell real estate and compete with the retained editor-centric experience.
- Removal Gap Statement: `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp` still constructs a dedicated tasks panel and `/Users/ryanrentfro/code/markamp/src/ui/ActivityBar.cpp` still exposes `Tasks`.
- User / Product Impact: Removes another secondary product line and clarifies what the app is for.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp:3030`; `/Users/ryanrentfro/code/markamp/src/ui/ActivityBar.cpp:245`; `/Users/ryanrentfro/code/markamp/src/ui/TaskListPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/core/TaskCommandProvider.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/KanbanEngine.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/CalendarEngine.cpp`
- Prior Plan References: `/Users/ryanrentfro/code/markamp/docs/v23_docs/Phase_15__Structured_Data_AV_Graph_Task_And_Knowledge_Workflow_Completion.md`; `/Users/ryanrentfro/code/markamp/docs/v24_docs/Phase_17__Advanced_Domain_Workflows_AV_Graph_Tasks_Presentation_And_Node_Editor.md`
- Scope: Task panel UI, commands, sidebar presence, calendar and task-board user surfaces, help text, and empty states.
- Out of Scope: Backend task parsing and scheduling removal handled in V29-P09-T02.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/TaskListPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/TaskListPanel.h`; `/Users/ryanrentfro/code/markamp/src/ui/KanbanEngine.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/CalendarEngine.cpp`; `/Users/ryanrentfro/code/markamp/src/core/TaskCommandProvider.cpp`
- Related Features / Systems / Components: Activity bar, sidebar, panels, commands, task visualization.
- Current Behavior / Presence: Users can still see and enter task-related surfaces.
- Intended Post-Removal Behavior: No shell or workflow surface exists for task management.
- Removal / Simplification Direction: Delete tasks as a product area rather than retaining partial markdown-task dashboards.
- Technical Approach: Remove UI surfaces and command routes, then clean backend services and scans.
- Implementation Steps: Delete task panel and sidebar hooks; remove task commands; remove calendar and task-board UI files; clean help text and status labels.
- Validation Steps: Search shell and command surfaces for `Task`, `Calendar`, and task-board references; verify nothing user-facing remains.
- Acceptance Criteria: No visible task-management workflow remains in the app.
- Dependencies: Phases 02 through 04.
- Parallelization Notes: Can run in parallel with flashcard removal.
- Risks / Failure Modes: AV kanban may be confused with task kanban; preserve structured-data views explicitly.
- Migration / Compatibility Notes: Persisted task panel mode must degrade to a retained panel.
- UX / Layout Cleanup Notes: Remove task badges, filters, and progress labels from the shell.
- Cleanup / Consolidation Notes: Prune task-only icons and labels later in Phase 14.
- Rollback / Safety Notes: Keep retained AV surfaces intact while removing task views.
- References / Context: Task workflows were previously treated as advanced domains; v29 retires them.
- Example Scenarios: No tasks panel in sidebar; no calendar task view; no task command palette items.

## Task V29-P09-T02

- Phase ID: V29-P09
- Task ID: V29-P09-T02
- Task Title: Delete task parsing, aggregation, recurrence, reminders, scheduling, gantt, and board engines
- Priority: High
- Category: Tasks Removal
- Objective: Remove the backend task-management subsystem and any background loops or analytics tied to it.
- Why This Matters Now: Leaving the backend in place keeps markdown scans, reminders, background scheduling, and event families alive.
- Removal Gap Statement: `/Users/ryanrentfro/code/markamp/src/core/TaskService.h`, `/Users/ryanrentfro/code/markamp/src/core/TaskAggregator.cpp`, `/Users/ryanrentfro/code/markamp/src/core/TaskBoardEngine.cpp`, `/Users/ryanrentfro/code/markamp/src/core/TaskGanttEngine.cpp`, `/Users/ryanrentfro/code/markamp/src/core/TaskReminder.cpp`, and `/Users/ryanrentfro/code/markamp/src/core/TaskScheduler.cpp` show a broad backend footprint.
- User / Product Impact: Reduces hidden background work and simplifies the markdown/document model.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/core/TaskService.h`; `/Users/ryanrentfro/code/markamp/src/core/TaskAggregator.cpp`; `/Users/ryanrentfro/code/markamp/src/core/TaskBoardEngine.cpp`; `/Users/ryanrentfro/code/markamp/src/core/TaskGanttEngine.cpp`; `/Users/ryanrentfro/code/markamp/src/core/TaskReminder.cpp`; `/Users/ryanrentfro/code/markamp/src/core/TaskScheduler.cpp`
- Prior Plan References: `/Users/ryanrentfro/code/markamp/docs/v24_docs/Phase_17__Advanced_Domain_Workflows_AV_Graph_Tasks_Presentation_And_Node_Editor.md`; `/Users/ryanrentfro/code/markamp/docs/v25_docs/Phase_17__Advanced_Domain_Release_Triage_And_Gating.md`
- Scope: Task parsing, query services, recurrence, reminders, schedulers, aggregation, gantt, task-board backend, and related events.
- Out of Scope: Build task runner and generic extension task definitions if they serve retained build/debug tooling; those must be separated carefully.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/TaskService.h`; `/Users/ryanrentfro/code/markamp/src/core/TaskAggregator.cpp`; `/Users/ryanrentfro/code/markamp/src/core/TaskBoardEngine.cpp`; `/Users/ryanrentfro/code/markamp/src/core/TaskGanttEngine.cpp`; `/Users/ryanrentfro/code/markamp/src/core/TaskReminder.cpp`; `/Users/ryanrentfro/code/markamp/src/core/TaskScheduler.cpp`
- Related Features / Systems / Components: Markdown scans, background jobs, notifications, events, calendar/task views.
- Current Behavior / Presence: Tasks are extracted from markdown and can be aggregated, scheduled, reminded, and rendered in dedicated views.
- Intended Post-Removal Behavior: Markdown documents are no longer scanned into a task-management subsystem and no task background jobs run.
- Removal / Simplification Direction: Delete the subsystem while preserving unrelated build-task/extension task infrastructure only if still needed.
- Technical Approach: Remove task-management classes, separate them from generic task-runner nomenclature, and prune task-specific event listeners.
- Implementation Steps: Delete task services and engines; remove scheduler/reminder hooks; keep or rename build-task infrastructure if still required for retained product; update tests and build.
- Validation Steps: Search production code for task-management types and ensure only retained build-runner concepts remain where intended.
- Acceptance Criteria: No task-management backend subsystem remains in shipping code.
- Dependencies: V29-P09-T01 and Phase 10 planning.
- Parallelization Notes: Needs careful ownership separation from build/debug task-runner code.
- Risks / Failure Modes: Generic names such as `TaskRunnerService` can be confused with product task management and accidentally deleted or retained incorrectly.
- Migration / Compatibility Notes: Persisted task caches or reminder state should be ignored safely.
- UX / Layout Cleanup Notes: Remove reminder or overdue notifications sourced from retired task logic.
- Cleanup / Consolidation Notes: Rename ambiguous retained “task runner” concepts if necessary to avoid product confusion.
- Rollback / Safety Notes: Audit build/debug workflows before changing generic task-runner code.
- References / Context: v29 distinguishes task management removal from possible retained build execution concepts.
- Example Scenarios: A markdown checkbox no longer populates a tasks sidebar or reminder engine.
