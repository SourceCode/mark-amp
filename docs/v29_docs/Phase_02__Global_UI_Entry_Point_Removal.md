# Phase 02: Global UI Entry Point Removal

## Task Count

- 2 tasks

## Task V29-P02-T01

- Phase ID: V29-P02
- Task ID: V29-P02-T01
- Task Title: Remove top-level menus, view modes, and global shell controls for retired features
- Priority: Critical
- Category: Menu / Command Removal
- Objective: Delete all main-menu and global shell affordances that expose Canvas, Notebook, Flashcards, and Tasks.
- Why This Matters Now: Users must stop being able to reach removed workflows before deeper subsystem deletion begins.
- Removal Gap Statement: `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp` still exposes `Canvas Mode`, top-level `Notebooks`, top-level `Canvas`, and flashcard tool entries.
- User / Product Impact: Removes dead-end product promises and immediately simplifies the app’s visible scope.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp:1244`; `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp:1311`; `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp:1318`; `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp:1327`
- Prior Plan References: `/Users/ryanrentfro/code/markamp/docs/v21_docs/Phase_02__Main_Menu_Command_Palette_And_Shortcut_Parity.md`; `/Users/ryanrentfro/code/markamp/docs/v22_docs/Phase_04__Menus_Command_Palette_Context_Menus_And_Toolbars.md`
- Scope: Main menu construction, menu IDs, menu handlers, view-mode toggles, enablement logic, and shell chrome labels.
- Out of Scope: Sidebar and activity bar entries handled in later phases.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`; `/Users/ryanrentfro/code/markamp/src/core/Events.h`
- Related Features / Systems / Components: Menu bar, view modes, global input routing.
- Current Behavior / Presence: Retired features still appear as primary product families in the menu bar.
- Intended Post-Removal Behavior: The menu bar reflects the smaller retained product with no notebook, canvas, flashcard, or task product headings.
- Removal / Simplification Direction: Remove the headings entirely rather than leaving disabled or empty menu groups.
- Technical Approach: Delete menu entries and handlers; remove corresponding IDs and conditional logic; re-balance remaining menus.
- Implementation Steps: Remove view check item for canvas mode; delete flashcard tool items; delete notebook and canvas menus; remove bound handlers and orphaned constants; update any menu assertions/tests.
- Validation Steps: Launch app; inspect menu bar; confirm no notebook/canvas/flashcard/task entries remain; verify no invalid menu-ID references compile.
- Acceptance Criteria: No top-level menu or view-mode entry remains for the retired features.
- Dependencies: V29-P01-T01.
- Parallelization Notes: Can run in parallel with Phase 03 once the removal ledger is fixed.
- Risks / Failure Modes: Leaving stale menu IDs or check-state code can cause compile or runtime errors.
- Migration / Compatibility Notes: None beyond preserving old session restore behavior until Phase 11.
- UX / Layout Cleanup Notes: Remaining menus should be re-clustered so the smaller product does not look like feature categories were simply torn out.
- Cleanup / Consolidation Notes: Remove unused `kMenu*` constants and handler stubs in the same pass.
- Rollback / Safety Notes: Keep changes isolated to menu authority code.
- References / Context: MainFrame remains the central shell authority in prior v21 planning.
- Example Scenarios: `View > Canvas Mode` disappears; `Tools > Flashcard Browser` disappears; the top menu no longer contains `Notebooks` or `Canvas`.

## Task V29-P02-T02

- Phase ID: V29-P02
- Task ID: V29-P02-T02
- Task Title: Remove startup, welcome, and global create/open affordances for retired artifact types
- Priority: High
- Category: UI Cleanup
- Objective: Eliminate notebook/board/task/flashcard references from startup, onboarding, quick-open, and new-item flows.
- Why This Matters Now: A smaller product cannot keep creation prompts or welcome hints for removed artifact families.
- Removal Gap Statement: Prior plans wired creation entry points through the shell and artifact services, and those flows must now be retired rather than improved.
- User / Product Impact: Prevents first-run confusion and keeps new-workspace flows aligned with the retained product.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/core/ArtifactCreationService.cpp`; `/Users/ryanrentfro/code/markamp/src/core/NotebookArtifactLifecycle.cpp`; `/Users/ryanrentfro/code/markamp/src/core/CanvasArtifactLifecycle.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/StartupPanel.cpp`
- Prior Plan References: `/Users/ryanrentfro/code/markamp/docs/v20_docs/Phase_01__Artifact_Creation_Spine_And_Shell_Ownership.md`; `/Users/ryanrentfro/code/markamp/docs/v24_docs/Phase_02__Artifact_Lifecycle_And_Unsaved_Document_Spine.md`; `/Users/ryanrentfro/code/markamp/docs/v26_docs/Phase_14__Startup_Welcome_Onboarding_And_Empty_State_Elegance.md`
- Scope: Startup CTA copy, quick-open lists, new-item affordances, welcome hints, and create/open entry points that mention removed features.
- Out of Scope: Deep artifact service collapse handled in Phase 05.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/StartupPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/core/ArtifactCreationService.cpp`; `/Users/ryanrentfro/code/markamp/src/core/NotebookArtifactLifecycle.cpp`; `/Users/ryanrentfro/code/markamp/src/core/CanvasArtifactLifecycle.cpp`
- Related Features / Systems / Components: Startup screen, new-item flows, artifact creation.
- Current Behavior / Presence: The repository still retains multi-artifact assumptions and prior planning expected notebook/canvas creation to deepen.
- Intended Post-Removal Behavior: Startup and creation flows only advertise retained document-centric workflows.
- Removal / Simplification Direction: Remove removed-artifact affordances rather than replacing them with hidden placeholders.
- Technical Approach: Strip create/open routes for retired kinds from startup and shared creation entry points, while preserving text-document creation.
- Implementation Steps: Remove startup CTAs and labels; remove create/open hooks for notebook/canvas; audit any quick-open or welcome surface references; update tests.
- Validation Steps: Open the app at startup; verify no board/notebook/flashcard/task terminology or buttons remain.
- Acceptance Criteria: No startup or welcome surface suggests creating or opening the retired feature types.
- Dependencies: V29-P01-T01.
- Parallelization Notes: Can proceed alongside V29-P02-T01.
- Risks / Failure Modes: Hidden secondary entry points may still create retired artifacts.
- Migration / Compatibility Notes: Startup restore of old sessions is handled later in Phase 11.
- UX / Layout Cleanup Notes: Replace removed CTAs with retained editor-focused guidance.
- Cleanup / Consolidation Notes: Delete dead onboarding copy and any obsolete icon references.
- Rollback / Safety Notes: Preserve startup screen stability while trimming options.
- References / Context: Earlier startup polish plans assumed these feature CTAs should be improved, not deleted.
- Example Scenarios: First launch offers only retained workflows; no “New Notebook” or “New Board” route remains.
