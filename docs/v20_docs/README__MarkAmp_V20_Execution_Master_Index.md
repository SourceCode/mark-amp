# MarkAmp V20 Execution Master Index

## Purpose

`v20` is the first planning pass that treats broken artifact creation as the top product-blocking defect. The application cannot function as a serious IDE until users can reliably create, open, edit, save, restore, rename, duplicate, and recover files, notebooks, and canvases through one cohesive shell.

This package turns the broad `v18` and `v19` direction into the next operating plan for finishing the product. It is intentionally more execution-focused, more dependency-aware, and more severe about systems that still only work in isolation.

## What Changed From V18 And V19

- `v18` mapped the broad integration problem.
- `v19` tightened the shell, command, restore, panel, notebook, canvas, and icon work into a sharper implementation wave.
- `v20` escalates artifact creation, lifecycle integrity, syntax-highlighting quality, and styling polish into the highest-priority product finish work.
- `v20` explicitly calls out that:
  - `MainFrame`, `TabBar`, `FileTreeCtrl`, `LayoutManager`, `NotebookManager`, `NotebookShellHost`, `CanvasWorkspacePanel`, and `CanvasWorkbench` still form separate creation paths.
  - the shell controller exists but does not yet own artifact lifecycle.
  - the session layer is still too memory-oriented to make restore trustworthy.
  - syntax highlighting architecture is split between `wxStyledTextCtrl` lexers and custom tokenization paths that are not fully married.
  - styling quality is inconsistent across editor, notebook, canvas, panels, dialogs, and icon systems.

## Top Product Blockers

1. New file creation is broken outside the explorer because `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp` and `/Users/ryanrentfro/code/markamp/src/ui/TabBar.cpp` inject fake untitled paths that `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp` then tries to open from disk.
2. New notebook creation is not a first-class shell workflow; `/Users/ryanrentfro/code/markamp/src/core/NotebookShellHost.cpp` and `/Users/ryanrentfro/code/markamp/src/core/NotebookDocumentLifecycle.cpp` are mostly notifications and bookkeeping.
3. New canvas creation is split between `/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp` and `/Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp`, and the panel creates a private event bus.
4. Save, restore, dirty tracking, and reopen behavior remain too thin because `/Users/ryanrentfro/code/markamp/src/core/DocumentPersistenceOrchestrator.cpp` and `/Users/ryanrentfro/code/markamp/src/core/WorkspaceSessionRestore.cpp` do not yet own durable lifecycle state.
5. Syntax highlighting quality is capped by a split stack across `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/core/SyntaxHighlighter.cpp`, `/Users/ryanrentfro/code/markamp/src/core/AsyncHighlighter.cpp`, `/Users/ryanrentfro/code/markamp/src/core/ThemeScopeMapper.cpp`, and `/Users/ryanrentfro/code/markamp/src/core/ThemeEngine.cpp`.
6. The visual system is still inconsistent across tabs, toolbars, trees, notebook chrome, canvas chrome, empty states, and icon semantics.

## Phase Order

1. `Phase_01__Artifact_Creation_Spine_And_Shell_Ownership.md`
2. `Phase_02__File_Creation_And_Text_Artifact_Lifecycle.md`
3. `Phase_03__Notebook_Creation_And_Computational_Document_Flows.md`
4. `Phase_04__Canvas_Creation_And_Board_Workbench_Integration.md`
5. `Phase_05__Persistence_Autosave_Restore_And_Recovery.md`
6. `Phase_06__Commands_Context_Focus_And_Surface_Entry_Points.md`
7. `Phase_07__Syntax_Highlighting_And_Language_Presentation.md`
8. `Phase_08__Editor_Notebook_Canvas_Rendering_And_Content_Styling.md`
9. `Phase_09__Premium_Visual_System_Polish_And_MUI_Icon_Completion.md`
10. `Phase_10__Diagnostics_Validation_Harnesses_And_Consolidation.md`

## Critical Path Dependencies

- Phase 01 must land before large workflow fixes because artifact identity, creation routing, and shell ownership are prerequisites for stable file, notebook, and canvas lifecycles.
- Phase 02, Phase 03, and Phase 04 depend on the artifact creation spine and can then progress partly in parallel.
- Phase 05 depends on the creation work because autosave, restore, and recovery must operate on real artifact records, not pseudo-paths.
- Phase 06 depends on the existence of canonical creation and open flows so menu, toolbar, tree, palette, context menu, shortcut, and empty-state entry points can all route to the same pipeline.
- Phase 07 and Phase 08 can start in parallel with late Phase 03 to Phase 06 work, but the shared theme/token decisions should be set before mass surface polish.
- Phase 09 depends on stabilization of interaction states, tokens, and major chrome behavior.
- Phase 10 spans the whole wave and becomes release-gating once the earlier phases are underway.

## Parallelizable Workstreams

- File creation lifecycle, notebook lifecycle, and canvas lifecycle after the Phase 01 artifact registry and creation service are defined.
- Theme token normalization and syntax token mapping after the highlighter contract is defined.
- Panel chrome polish and icon normalization after shared spacing, sizing, and semantic rules are documented.
- Regression harnesses, smoke paths, and observability work can run alongside implementation once critical flows are stable enough to capture.

## Creation Workflow Priorities

- Replace pseudo untitled-path behavior with first-class unsaved artifact records.
- Unify menu, toolbar, tree, tab-bar, command palette, keyboard, and empty-state creation entry points.
- Make new file, new notebook, and new canvas all create through one shell-owned artifact pipeline.
- Guarantee immediate mount, focus, dirty tracking, save, reopen, restore, rename, duplicate, and cleanup behavior.

## Syntax Highlighting And Styling Priorities

- Stop treating the editor as markdown-first when the product positioning is IDE-first.
- Define a single language-selection, tokenization, and theme-mapping contract.
- Raise baseline quality for code fonts, gutter treatment, cursor, selection, bracket matching, search, diagnostics, cell chrome, and embedded code surfaces.
- Complete MUI icon migration and remove mixed icon semantics across shell surfaces.

## Improvement Counts

| Phase | Improvements |
| --- | ---: |
| Phase 01 | 210 |
| Phase 02 | 204 |
| Phase 03 | 204 |
| Phase 04 | 204 |
| Phase 05 | 204 |
| Phase 06 | 204 |
| Phase 07 | 210 |
| Phase 08 | 210 |
| Phase 09 | 210 |
| Phase 10 | 210 |
| Total | 2070 |

## Overall Acceptance Criteria

- New file, notebook, and canvas creation work from every major entry point without fake paths, dead UI, or orphaned state.
- Each artifact type has a durable lifecycle contract covering create, open, dirty, save, save as, rename, duplicate, move, delete, autosave, restore, and recovery.
- Shell state, tab state, panel state, focus state, and workspace state all agree on the active artifact.
- Syntax highlighting is language-aware, theme-correct, readable, consistent, and fast across editor, notebook cells, markdown code fences, diffs, and canvas embeds.
- The visual system feels intentional and premium across all major surfaces, including empty states, errors, toolbars, trees, tabs, dialogs, and notebook/canvas chrome.
- Diagnostics, integration tests, smoke tests, and telemetry exist for the creation and restore paths that currently fail silently.

