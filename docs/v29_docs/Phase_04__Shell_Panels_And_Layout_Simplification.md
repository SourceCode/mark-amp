# Phase 04: Shell Panels And Layout Simplification

## Task Count

- 2 tasks

## Task V29-P04-T01

- Phase ID: V29-P04
- Task ID: V29-P04-T01
- Task Title: Remove retired activity-bar items, sidebar modes, and panel registrations
- Priority: Critical
- Category: Panel / Layout Simplification
- Objective: Delete notebooks, canvas, flashcards, and tasks from the activity bar, sidebar-mode model, and panel registry.
- Why This Matters Now: These are high-frequency navigation surfaces that make the removed features still look like core product pillars.
- Removal Gap Statement: `/Users/ryanrentfro/code/markamp/src/ui/ActivityBar.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/SidebarMode.h`, and `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp` still treat all four retired features as first-class sidebar families.
- User / Product Impact: Visibly simplifies navigation and reduces shell clutter immediately.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/ActivityBar.cpp:212`; `/Users/ryanrentfro/code/markamp/src/ui/ActivityBar.cpp:218`; `/Users/ryanrentfro/code/markamp/src/ui/ActivityBar.cpp:236`; `/Users/ryanrentfro/code/markamp/src/ui/ActivityBar.cpp:245`; `/Users/ryanrentfro/code/markamp/src/ui/SidebarMode.h:17`; `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp:2693`; `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp:2710`; `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp:2893`; `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp:3030`
- Prior Plan References: `/Users/ryanrentfro/code/markamp/docs/v21_docs/Phase_05__Panel_Registry_Primary_Sidebar_And_Controlled_Surface_Replacement.md`; `/Users/ryanrentfro/code/markamp/docs/v22_docs/Phase_03__Activity_Bar_Sidebars_Panel_Headers_And_Shell_Chrome.md`
- Scope: Activity bar items, sidebar mode constants, panel registry registrations, badges, selection restore, and panel labels.
- Out of Scope: Canvas workbench mode in the center content area handled in V29-P04-T02 and Phase 06.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/ActivityBar.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/SidebarMode.h`; `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/StatusBarPanel.cpp`
- Related Features / Systems / Components: Activity bar, primary sidebar, secondary sidebar, panel registry, status bar.
- Current Behavior / Presence: The shell still reserves dedicated navigation real estate for each removed feature family.
- Intended Post-Removal Behavior: Activity bar and sidebars represent only retained product areas.
- Removal / Simplification Direction: Delete removed modes entirely and reflow remaining navigation, rather than leaving empty slots or dormant IDs.
- Technical Approach: Remove model items and registry entries; update restore logic and status bar mappings; clean badge and selection handling.
- Implementation Steps: Delete activity bar registrations; remove sidebar mode constants; remove panel factories; prune secondary-sidebar references; adjust mode persistence readers.
- Validation Steps: Launch shell and verify no notebook, canvas, flashcard, or task icons or panels remain; check sidebar switching and status bar text.
- Acceptance Criteria: No shell navigation slot exists for retired features and sidebar switching remains stable.
- Dependencies: Phases 02 and 03.
- Parallelization Notes: Can proceed in parallel with retained-shell IA cleanup planning.
- Risks / Failure Modes: Restored sidebar-mode values may crash or select invalid panels if migration handling is missing.
- Migration / Compatibility Notes: Unknown persisted mode values should downgrade to a retained default.
- UX / Layout Cleanup Notes: Rebalance icon order, spacing, and emphasis around retained views.
- Cleanup / Consolidation Notes: Remove placeholder `make_feature_panel` uses that only supported retired panels.
- Rollback / Safety Notes: Keep a deterministic fallback sidebar mode during migration.
- References / Context: v21 and v22 shell plans highlighted these areas as central control authority.
- Example Scenarios: Activity bar no longer shows Canvas or Notebooks; sidebar persistence falls back cleanly to Explorer.

## Task V29-P04-T02

- Phase ID: V29-P04
- Task ID: V29-P04-T02
- Task Title: Remove canvas workbench mode and simplify central layout composition
- Priority: Critical
- Category: Panel / Layout Simplification
- Objective: Delete the editor-versus-canvas mode split and return the shell to one retained central workspace composition.
- Why This Matters Now: Canvas mode is the largest shell-level branch keeping the app multi-surface rather than editor-centric.
- Removal Gap Statement: `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp` still has `ShowCanvasWorkspace()`, `ShowEditorWorkspace()`, `is_canvas_mode()`, and workbench-mode branches tied to canvas.
- User / Product Impact: Removes a major conceptual split and makes the remaining product easier to understand.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp:3838`; `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp:3919`; `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp:3950`; `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp:1906`
- Prior Plan References: `/Users/ryanrentfro/code/markamp/docs/v19_docs/Phase_08__Canvas_Workbench_And_Cross_Surface_Integration.md`; `/Users/ryanrentfro/code/markamp/docs/v24_docs/Phase_09__Canvas_Artifact_Workbench_And_Interaction_Completion.md`; `/Users/ryanrentfro/code/markamp/docs/v25_docs/Phase_09__Canvas_Shell_Workbench_Persistence_And_Interaction_Integrity.md`
- Scope: LayoutManager canvas-mode fields and methods, workbench-mode routing, view-mode toggles, and center-area child handling.
- Out of Scope: Deleting all canvas engine files, handled in Phase 06.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.h`; `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`; `/Users/ryanrentfro/code/markamp/src/core/Events.h`
- Related Features / Systems / Components: Content area, workbench mode, view routing, event flow.
- Current Behavior / Presence: Central layout can switch to a lazily created canvas workspace and publishes canvas activation/deactivation events.
- Intended Post-Removal Behavior: The central layout is single-mode for retained editor/document experiences.
- Removal / Simplification Direction: Delete the workbench split instead of preserving a generic mode switcher for a removed surface.
- Technical Approach: Remove canvas workspace members and branches, collapse workbench-mode logic, and keep retained content layout always mounted.
- Implementation Steps: Delete `CanvasWorkspacePanel` ownership from layout; remove `kCanvas` workbench-mode handling; remove canvas mode event publication; simplify content-container show/hide rules.
- Validation Steps: Open the app, switch view modes, open documents, and confirm content layout no longer hides itself for canvas mode.
- Acceptance Criteria: No canvas workbench mode exists and central layout logic is materially smaller.
- Dependencies: Phase 02.
- Parallelization Notes: Coordinate with Phase 06 to avoid duplicate edits.
- Risks / Failure Modes: Tab/content layout could regress if canvas mode code was masking unrelated layout assumptions.
- Migration / Compatibility Notes: Old persisted `WorkbenchMode::kCanvas` should degrade to a retained mode.
- UX / Layout Cleanup Notes: Remove any label or status indicator that implies multiple workspace families still exist.
- Cleanup / Consolidation Notes: This phase should shrink state fields, event branches, and content-container complexity.
- Rollback / Safety Notes: Keep editor layout stable and covered by smoke tests while deleting the branch.
- References / Context: Earlier plans treated canvas mode as a premium surface; v29 intentionally deletes it.
- Example Scenarios: `Ctrl+4` no longer toggles anything because canvas mode no longer exists.
