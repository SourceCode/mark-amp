# Phase 12: Creation Open Save And Import Export Cleanup

## Task Count

- 2 tasks

## Task V29-P12-T01

- Phase ID: V29-P12
- Task ID: V29-P12-T01
- Task Title: Remove file-type associations, creation/open/save logic, and reopen affordances for retired artifacts
- Priority: High
- Category: Model / Document-Type / Workflow Removal
- Objective: Eliminate notebook and board file-type assumptions and any creation/open/save/reopen logic that still branches for them.
- Why This Matters Now: Shared document workflows must stop pretending there are retained non-text artifact families.
- Removal Gap Statement: Artifact lifecycle and creation logic still encode notebook and canvas semantics, and prior plans expanded their open/save/reopen behavior.
- User / Product Impact: Simplifies open/save flows and reduces confusing mixed artifact handling.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/core/ArtifactCreationService.cpp`; `/Users/ryanrentfro/code/markamp/src/core/NotebookArtifactLifecycle.cpp`; `/Users/ryanrentfro/code/markamp/src/core/CanvasArtifactLifecycle.cpp`
- Prior Plan References: `/Users/ryanrentfro/code/markamp/docs/v20_docs/Phase_02__File_Creation_And_Text_Artifact_Lifecycle.md`; `/Users/ryanrentfro/code/markamp/docs/v24_docs/Phase_02__Artifact_Lifecycle_And_Unsaved_Document_Spine.md`
- Scope: New/open/save/reopen flows, file associations, naming rules, recent openers, and unsaved-artifact handling.
- Out of Scope: Low-level serializer cleanup already handled in Phase 11.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/ArtifactCreationService.cpp`; `/Users/ryanrentfro/code/markamp/src/core/ArtifactRegistry.h`; `/Users/ryanrentfro/code/markamp/src/core/NotebookArtifactLifecycle.cpp`; `/Users/ryanrentfro/code/markamp/src/core/CanvasArtifactLifecycle.cpp`
- Related Features / Systems / Components: File lifecycle, unsaved documents, recent items, save prompts.
- Current Behavior / Presence: Shared document workflows still branch for notebook and canvas creation/open/save.
- Intended Post-Removal Behavior: Creation and reopen logic focus on retained document flows only.
- Removal / Simplification Direction: Remove retired branches and simplify naming/default-language logic accordingly.
- Technical Approach: Delete retired branch points and normalize callers to text-document rules or compatibility drops.
- Implementation Steps: Remove create/open/save callers; simplify naming rules; remove file-type mapping references; adjust unsaved-artifact prompts and tests.
- Validation Steps: Exercise new, open, save, save-as, reopen, rename, and duplicate flows for retained documents.
- Acceptance Criteria: Shared document lifecycle code no longer branches for notebook or canvas artifacts.
- Dependencies: Phase 05 and Phase 11.
- Parallelization Notes: Can overlap with Phase 16 startup cleanup.
- Risks / Failure Modes: Untitled/new-file flow may regress if creation helpers were over-coupled to multi-artifact assumptions.
- Migration / Compatibility Notes: Reopen of removed artifacts should become a safe no-op with optional notice.
- UX / Layout Cleanup Notes: Simplify create/open labels to retained document terminology.
- Cleanup / Consolidation Notes: Collapse duplicate lifecycle code introduced for notebook/canvas.
- Rollback / Safety Notes: Retained file lifecycle should be kept under smoke coverage during the refactor.
- References / Context: This is the shared workflow cleanup counterpart to direct feature deletion.
- Example Scenarios: `New` only creates retained document types; reopening a board path does not create a dead tab.

## Task V29-P12-T02

- Phase ID: V29-P12
- Task ID: V29-P12-T02
- Task Title: Remove retired-feature import/export seams, search hooks, and help/document examples
- Priority: Medium
- Category: Model / Document-Type / Workflow Removal
- Objective: Delete any import/export, search, help, or example content that still teaches or routes into retired features.
- Why This Matters Now: Users often rediscover removed features through docs, examples, importer/exporter menus, and searchable help before they hit UI chrome.
- Removal Gap Statement: `/Users/ryanrentfro/code/markamp/src/core/HelpService.h` still documents task metadata, flashcards, canvas mode, and notebook mode.
- User / Product Impact: Prevents ghost references and ensures the smaller product feels intentional.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/core/HelpService.h:208`; `/Users/ryanrentfro/code/markamp/src/core/HelpService.h:227`; `/Users/ryanrentfro/code/markamp/src/core/HelpService.h:236`; `/Users/ryanrentfro/code/markamp/src/core/HelpService.h:335`
- Prior Plan References: `/Users/ryanrentfro/code/markamp/docs/v18_docs/Phase_05__Explorer_Search_Navigation_And_Workspace_Flows.md`; `/Users/ryanrentfro/code/markamp/docs/v22_docs/Phase_16__Startup_Welcome_Walkthrough_Empty_Loading_And_Error_States.md`
- Scope: Help topics, tooltips, inline docs, import/export labels, search index hints, and sample syntax tied to retired features.
- Out of Scope: Broader docs/code comment retirement covered in Phase 18.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/HelpService.h`; `/Users/ryanrentfro/code/markamp/src/core/SearchService.cpp`; `/Users/ryanrentfro/code/markamp/src/core/ExportService.cpp`; `/Users/ryanrentfro/code/markamp/src/core/ImportDialog.cpp`
- Related Features / Systems / Components: Help, tooltips, import/export, search guidance.
- Current Behavior / Presence: Help text and examples still suggest task and flashcard syntax and notebook/canvas modes.
- Intended Post-Removal Behavior: No built-in help or import/export copy points users at removed workflows.
- Removal / Simplification Direction: Delete or rewrite examples around retained features only.
- Technical Approach: Audit and remove feature-specific help topics, tooltips, samples, and any import/export branch copy tied to retired artifacts.
- Implementation Steps: Remove help topics; update tooltips; prune import/export menu text; update search/help tests and snapshots.
- Validation Steps: Search UI help and repository for `Notebook Mode`, `Canvas Mode`, `Flashcard`, and retired task examples.
- Acceptance Criteria: No user-facing help or example text teaches removed workflows.
- Dependencies: Phases 06 through 09.
- Parallelization Notes: Can run in parallel with Phase 14.
- Risks / Failure Modes: Internal developer-only comments can be mistaken for user-facing copy; focus first on shipped surfaces.
- Migration / Compatibility Notes: If old files are unsupported, help text should say so plainly.
- UX / Layout Cleanup Notes: Use retained editor terminology consistently after removal.
- Cleanup / Consolidation Notes: Simplify help-topic taxonomy after deleting removed categories.
- Rollback / Safety Notes: Keep retained help topics intact and searchable.
- References / Context: Product simplification fails if ghost documentation remains visible.
- Example Scenarios: Help no longer includes flashcard syntax examples or a canvas-mode tooltip.
