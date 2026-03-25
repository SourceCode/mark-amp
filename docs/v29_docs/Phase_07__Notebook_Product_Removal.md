# Phase 07: Notebook Product Removal

## Task Count

- 2 tasks

## Task V29-P07-T01

- Phase ID: V29-P07
- Task ID: V29-P07-T01
- Task Title: Delete notebook artifact lifecycle, shell host, and notebook-specific create/open/save/restore flows
- Priority: Critical
- Category: Notebook Removal
- Objective: Remove notebook artifact workflows and eliminate notebook-specific shell ownership.
- Why This Matters Now: Notebook still exists as a parallel document family with separate lifecycle rules and host abstractions.
- Removal Gap Statement: `/Users/ryanrentfro/code/markamp/src/core/NotebookArtifactLifecycle.cpp` and `/Users/ryanrentfro/code/markamp/src/core/NotebookShellHost.cpp` still keep notebook creation and shell behavior alive.
- User / Product Impact: Removes a major non-editor workflow family and simplifies document management.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/core/NotebookArtifactLifecycle.cpp`; `/Users/ryanrentfro/code/markamp/src/core/NotebookShellHost.cpp`; `/Users/ryanrentfro/code/markamp/src/core/NotebookDocumentLifecycle.cpp`
- Prior Plan References: `/Users/ryanrentfro/code/markamp/docs/v19_docs/Phase_07__Notebook_Host_Execution_And_Document_Trust.md`; `/Users/ryanrentfro/code/markamp/docs/v20_docs/Phase_03__Notebook_Creation_And_Computational_Document_Flows.md`; `/Users/ryanrentfro/code/markamp/docs/v25_docs/Phase_08__Notebook_Lifecycle_Runtime_And_Output_Integrity.md`
- Scope: Notebook artifact lifecycle, notebook host abstractions, open/save/restore hooks, and shell entry points.
- Out of Scope: Notebook runtime and cell/output system handled in V29-P07-T02.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/NotebookArtifactLifecycle.cpp`; `/Users/ryanrentfro/code/markamp/src/core/NotebookArtifactLifecycle.h`; `/Users/ryanrentfro/code/markamp/src/core/NotebookShellHost.cpp`; `/Users/ryanrentfro/code/markamp/src/core/NotebookDocumentLifecycle.cpp`
- Related Features / Systems / Components: Artifact lifecycle, shell host, tabs, restore, open/save.
- Current Behavior / Presence: Notebooks are still modeled as a separate artifact family with dedicated shell state.
- Intended Post-Removal Behavior: No production code can create, open, save, restore, or host notebooks.
- Removal / Simplification Direction: Delete notebook lifecycle and host code rather than freezing it.
- Technical Approach: Remove the classes and route old notebook restore entries into compatibility handling.
- Implementation Steps: Delete lifecycle and host classes; remove notebook creation/open callers; prune notebook-specific IDs and tests; wire migration drop paths.
- Validation Steps: Search production code for `NotebookArtifactLifecycle`, `NotebookShellHost`, and `NotebookArtifact*Event`; confirm no runtime callers remain.
- Acceptance Criteria: Notebooks are no longer first-class artifacts or hostable shell content.
- Dependencies: Phase 05.
- Parallelization Notes: Can run alongside Phase 06 with a disjoint file set.
- Risks / Failure Modes: Session restore or tab code may still assume notebooks exist.
- Migration / Compatibility Notes: Old notebook entries should be skipped without crashing startup.
- UX / Layout Cleanup Notes: Remove notebook-related recent-item, empty-state, or restore wording.
- Cleanup / Consolidation Notes: Delete notebook-specific routing helpers and placement contexts.
- Rollback / Safety Notes: Preserve retained file open/save flows while removing notebook branches.
- References / Context: Earlier notebook plans are explicitly retired here.
- Example Scenarios: Reopening a prior notebook session does not recreate a notebook host.

## Task V29-P07-T02

- Phase ID: V29-P07
- Task ID: V29-P07-T02
- Task Title: Delete notebook runtime, cell system, output integration, export, search, and chrome coordination
- Priority: Critical
- Category: Notebook Removal
- Objective: Remove the entire notebook runtime stack, cell models/controllers, output surfaces, toolbar, variable inspector, and related search/export paths.
- Why This Matters Now: Notebook remains spread across parsing, execution, UI chrome, and completion-auditor layers.
- Removal Gap Statement: Notebook-specific core files total 42 and include execution, session, toolbar, diff, export, output, and chrome coordination.
- User / Product Impact: Prevents runtime dead code and removes a major maintenance and testing burden.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/core/NotebookExecutionPipeline.cpp`; `/Users/ryanrentfro/code/markamp/src/core/NotebookSessionManager.cpp`; `/Users/ryanrentfro/code/markamp/src/core/NotebookVariableInspector.cpp`; `/Users/ryanrentfro/code/markamp/src/core/NotebookToolbar.cpp`; `/Users/ryanrentfro/code/markamp/src/core/NotebookExportEngine.cpp`; `/Users/ryanrentfro/code/markamp/src/core/NotebookSearchIndex.cpp`
- Prior Plan References: `/Users/ryanrentfro/code/markamp/docs/v24_docs/Phase_08__Notebook_Artifact_Runtime_And_Execution_Completion.md`; `/Users/ryanrentfro/code/markamp/docs/v26_docs/Phase_08__Notebook_Shell_Cell_Chrome_Output_And_Mixed_Content.md`; `/Users/ryanrentfro/code/markamp/docs/v27_docs/Phase_09__Notebook_Shell_Cell_Output_And_Mixed_Content_Redesign.md`
- Scope: Notebook runtime, cell controllers/managers, chrome coordinators, output integration, export, diff, search index, and variable inspection.
- Out of Scope: Generic export and search systems that still serve retained text workflows.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/NotebookExecutionPipeline.cpp`; `/Users/ryanrentfro/code/markamp/src/core/NotebookCellManager.cpp`; `/Users/ryanrentfro/code/markamp/src/core/NotebookCellController.cpp`; `/Users/ryanrentfro/code/markamp/src/core/NotebookOutputIntegration.cpp`; `/Users/ryanrentfro/code/markamp/src/core/NotebookToolbar.cpp`; `/Users/ryanrentfro/code/markamp/src/core/NotebookChromeCoordinator.cpp`
- Related Features / Systems / Components: Cells, execution, output, search, export, chrome, variable inspection.
- Current Behavior / Presence: Notebook runtime remains a substantial active subsystem, not a thin adapter.
- Intended Post-Removal Behavior: No notebook runtime, cell, output, or export behavior remains in production code.
- Removal / Simplification Direction: Delete the subsystem and collapse retained code paths back to ordinary documents.
- Technical Approach: Remove notebook-owned files from build and callers; replace any mixed-content assumptions with retained editor behavior.
- Implementation Steps: Delete notebook runtime and chrome files; prune notebook-specific event listeners; update build and tests; clean retained export/search code of notebook branches.
- Validation Steps: Search for `NotebookExecution`, `NotebookCell`, `NotebookOutput`, `NotebookToolbar`, and `NotebookVariableInspector`; confirm only migration notes remain if required.
- Acceptance Criteria: Notebook runtime and cell systems are absent from the shipping codebase.
- Dependencies: V29-P07-T01.
- Parallelization Notes: Can overlap with Phase 13 for search/export/AI branch cleanup.
- Risks / Failure Modes: Mixed-content rendering code may need retained-path refactoring rather than naive deletion.
- Migration / Compatibility Notes: Old persisted notebook outputs should be ignored rather than loaded.
- UX / Layout Cleanup Notes: Remove help text and hints implying executable notebooks still exist.
- Cleanup / Consolidation Notes: Delete completion-auditor and chrome-coordinator classes that only existed for notebooks.
- Rollback / Safety Notes: Keep retained editor rendering and export tests active through the deletion.
- References / Context: This explicitly retires notebook completion and redesign work from v24-v27.
- Example Scenarios: No run-cell behavior, no notebook toolbar, no notebook search index.
