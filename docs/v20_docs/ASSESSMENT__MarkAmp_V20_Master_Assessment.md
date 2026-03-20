# MarkAmp V20 Master Assessment

## Executive Summary

The repository already contains a large amount of shell, editor, notebook, canvas, theme, and command infrastructure. The product problem is no longer lack of subsystems. The product problem is that the subsystems still do not share one authoritative artifact lifecycle, one trustworthy shell contract, one consistent rendering model, or one premium visual system.

The most severe failures are the broken creation workflows for files, notebooks, and canvases. Those are not secondary UX issues. They are hard blockers to the product being considered an IDE.

## Why Core Creation Workflows Are Broken

### New File Creation

- `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp` registers a `New File` command that generates `Untitled-N.md` and calls `layout_->OpenFileInTab(path)`.
- `/Users/ryanrentfro/code/markamp/src/ui/TabBar.cpp` duplicates that behavior from the new-tab button and empty-tab-bar double click.
- `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp` `OpenFileInTab` attempts to open the path from disk with `std::ifstream` and returns on failure.
- `/Users/ryanrentfro/code/markamp/src/ui/FileTreeCtrl.cpp` contains a separate file-creation path that does write a real file to disk.

Conclusion: there is no first-class unsaved text-document model. There are at least two incompatible creation strategies, and the shell cannot reason about them coherently.

### New Notebook Creation

- `/Users/ryanrentfro/code/markamp/src/core/Notebook.cpp` `NotebookManager::create_notebook` is the most real creation implementation, but it stores notebook data under `knowledgebase.data_dir`, not through the workspace artifact flow that IDE users expect.
- `/Users/ryanrentfro/code/markamp/src/core/NotebookShellHost.cpp` only changes state and emits notifications.
- `/Users/ryanrentfro/code/markamp/src/core/NotebookDocumentLifecycle.cpp` tracks open/dirty sets but does not perform authoritative persistence, shell mounting, or restore.
- `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp` palette commands publish notebook events directly instead of going through a deterministic creation service.

Conclusion: notebook support has model code but not a product-grade shell workflow.

### New Canvas Creation

- `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp` handles `BoardOpenRequestEvent` by showing canvas workspace and calling `canvas_workspace_->NewBoard()` when no board ID is provided.
- `/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp` `NewBoard()` just increments a counter and updates the title label.
- `/Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp` separately implements real board creation, open, duplicate, rename, and save functions.
- `/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp` creates a private `std::make_shared<core::EventBus>()` for `CanvasPanel`, cutting the panel off from the main application event bus.

Conclusion: canvas creation is split between UI placeholder behavior and a more real workbench model, and the event topology is wrong.

## Highest-Risk Architecture Gaps

1. No shell-owned artifact registry spans files, notebooks, and canvases.
2. `/Users/ryanrentfro/code/markamp/src/core/WorkbenchShellController.cpp` owns surface switching, not artifact lifecycle.
3. Persistence is still event-thin: `/Users/ryanrentfro/code/markamp/src/core/DocumentPersistenceOrchestrator.cpp` mostly emits save events and marks dirty state optimistically.
4. Session restore is not durable enough: `/Users/ryanrentfro/code/markamp/src/core/WorkspaceSessionRestore.cpp` is snapshot-oriented but still effectively memory-first and only partially applied.
5. Command entry points remain duplicated between command registry, palette registration, menu handlers, toolbar buttons, and direct widget callbacks.
6. Canvas and notebook shell integrations remain shallow and bypass the same lifecycle guarantees the editor expects.

## Syntax Highlighting And Rendering Weaknesses

- `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp` configures `wxStyledTextCtrl` primarily as a markdown editor.
- `ApplyLargeFileOptimizations()` falls back to `wxSTC_LEX_NULL`, which avoids freezes but also removes language quality instead of degrading gracefully.
- `/Users/ryanrentfro/code/markamp/src/core/SyntaxHighlighter.cpp` and `/Users/ryanrentfro/code/markamp/src/core/AsyncHighlighter.cpp` provide a custom tokenization stack, but the editor is not clearly driven by that stack.
- `/Users/ryanrentfro/code/markamp/src/core/ThemeScopeMapper.cpp` and `/Users/ryanrentfro/code/markamp/src/core/ThemeEngine.cpp` show intent to support richer token theming, but the surface contract is incomplete.
- Notebook code cells, markdown code fences, diff views, and canvas-embedded code all risk diverging because the highlighter pipeline is not unified.

## Styling And Visual Quality Weaknesses

- Panel chrome, toolbar density, tree row spacing, button treatment, tab styling, notebook cell chrome, canvas rail styling, and dialog states still vary too much.
- Mixed icon systems remain in use even after the MUI migration work started.
- Empty, loading, error, hover, focus, pressed, selected, and disabled states are inconsistent.
- Typography and spacing choices still feel subsystem-local instead of product-wide.

## Most Important Next Moves

1. Introduce one artifact creation and lifecycle contract for text files, notebooks, and canvases.
2. Move all creation entry points behind shell-owned creation services instead of direct UI hacks.
3. Replace pseudo untitled paths with unsaved artifact records plus save-target resolution.
4. Rebase notebook and canvas creation on workspace-aware artifact registration and restore.
5. Create one language/highlighting/theming contract used by editor, notebook, markdown, diff, and canvas code surfaces.
6. Standardize spacing, sizing, chrome, state styling, and icon semantics before more surface-specific polish is layered on top.

## What Still Blocks MarkAmp From Being A Finished IDE

- Users cannot trust creation of primary artifact types.
- Users cannot trust save, reopen, autosave, restore, and recovery flows.
- The shell still lacks a single active-artifact truth across surfaces.
- Syntax highlighting quality is not IDE-grade.
- Styling quality is not yet premium or cohesive.
- Too many interaction paths still work by direct widget callbacks instead of durable, testable workflow services.

## V20 Acceptance Lens

`v20` succeeds only if the implementation team can use it as a direct operating plan to:

- make file, notebook, and canvas creation reliable from all major entry points,
- unify persistence and restore behavior,
- deliver materially better syntax highlighting and content presentation,
- and raise the entire application to a visibly more polished, more trustworthy IDE experience.

