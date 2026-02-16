Phase 2 Prompt (Project Workspace + Multi-Modal Panels)

1. Install a true project shell with a persistent activity rail in `/Volumes/SecondDrive/code2/mark-amp/src/ui/LayoutManager.cpp` and `/Volumes/SecondDrive/code2/mark-amp/src/ui/LayoutManager.h`. Mount `ActivityBar` as a fixed left rail outside sidebar content, make it the primary mode switch, and keep toolbar triggers as fallback.

2. Expand `SidebarMode` in `/Volumes/SecondDrive/code2/mark-amp/src/ui/LayoutManager.h` to `Explorer`, `Search`, `Notebooks`, `Canvas`, `Graph`, and `Extensions`. Ensure two-way sync so rail selection, menu commands, and internal mode changes always reflect one canonical active mode.

3. Replace one-off panel show/hide logic in `/Volumes/SecondDrive/code2/mark-amp/src/ui/LayoutManager.cpp` with a registry-based sidebar host that lazily initializes and preserves panel state. Integrate existing panels from `/Volumes/SecondDrive/code2/mark-amp/src/ui/FileTreeCtrl.cpp`, `/Volumes/SecondDrive/code2/mark-amp/src/ui/SearchPanel.cpp`, `/Volumes/SecondDrive/code2/mark-amp/src/ui/GraphViewPanel.cpp`, and `/Volumes/SecondDrive/code2/mark-amp/src/ui/ExtensionsBrowserPanel.cpp`.

4. Add a first-class notebook workspace UI by creating `/Volumes/SecondDrive/code2/mark-amp/src/ui/NotebookPanel.h` and `/Volumes/SecondDrive/code2/mark-amp/src/ui/NotebookPanel.cpp`. Back it with `/Volumes/SecondDrive/code2/mark-amp/src/core/NotebookCellManager.h`, implement add/delete/move/run/collapse cell actions, render outputs via `/Volumes/SecondDrive/code2/mark-amp/src/ui/CellOutputRenderer.cpp`, and render dataframe outputs via `/Volumes/SecondDrive/code2/mark-amp/src/ui/DataFrameRenderer.cpp`.

5. Add a first-class canvas workspace by creating `/Volumes/SecondDrive/code2/mark-amp/src/ui/CanvasWorkspacePanel.h` and `/Volumes/SecondDrive/code2/mark-amp/src/ui/CanvasWorkspacePanel.cpp` around `/Volumes/SecondDrive/code2/mark-amp/src/ui/CanvasPanel.cpp`. Include a compact tool strip for select/pan/text/shape/link and publish tool/selection actions through `/Volumes/SecondDrive/code2/mark-amp/src/core/Events.h`.

6. Promote graph mode into the project workflow by wiring `/Volumes/SecondDrive/code2/mark-amp/src/ui/GraphViewPanel.cpp` plus `/Volumes/SecondDrive/code2/mark-amp/src/ui/GraphFilterBar.cpp` into the shared mode system. Add context handoff from explorer selection to graph scope (`global`, `local`, `notebook`).

7. Normalize navigation events in `/Volumes/SecondDrive/code2/mark-amp/src/core/Events.h` by adding `ShowSearchRequestEvent`, `ShowNotebooksRequestEvent`, `ShowCanvasRequestEvent`, and `ShowGraphRequestEvent`. Route all show-\* events through one mode-switch path in `/Volumes/SecondDrive/code2/mark-amp/src/ui/LayoutManager.cpp`.

8. Overhaul command discoverability in `/Volumes/SecondDrive/code2/mark-amp/src/ui/MainFrame.cpp`. Add palette commands for opening each project mode, notebook actions (`Add Code Cell`, `Add Markdown Cell`, `Run Cell`, `Run All`, `Export Notebook`), and canvas actions (`New Canvas`, `Zoom to Fit`, `Center Selection`), with consistent categories and shortcuts.

9. Replace emoji activity icons in `/Volumes/SecondDrive/code2/mark-amp/src/ui/ActivityBar.cpp` with a consistent icon pipeline (existing app icon strategy or SVG-based glyph rendering). Add keyboard focus and arrow navigation for activity items, and keep hover/active/pressed/badge visuals fully token-driven.

10. Persist workspace state in `/Volumes/SecondDrive/code2/mark-amp/src/ui/LayoutManager.cpp` by storing last active mode and restored panel states in config keys. Keep lazy panel creation and add notebook virtualization and canvas repaint throttling to prevent startup and interaction regressions.

11. Add tests in `/Volumes/SecondDrive/code2/mark-amp/tests/unit/test_layout.cpp`, `/Volumes/SecondDrive/code2/mark-amp/tests/unit/test_workspace_layout.cpp`, `/Volumes/SecondDrive/code2/mark-amp/tests/unit/test_notebook_cells.cpp`, `/Volumes/SecondDrive/code2/mark-amp/tests/unit/test_canvas_input.cpp`, and `/Volumes/SecondDrive/code2/mark-amp/tests/unit/test_graph_view_panel.cpp` covering mode switching, state persistence, notebook cell lifecycle, canvas tool flow, and graph scope handoff.

12. Acceptance criteria: the activity rail is the canonical workspace navigator; Explorer/Search/Notebooks/Canvas/Graph/Extensions switch instantly without losing hidden panel state; notebook cells are editable/executable with visible typed outputs; canvas and graph are command-palette-accessible modes; no new hard-coded colors are introduced; build and tests pass with `cmake --preset debug`, `cmake --build /Volumes/SecondDrive/code2/mark-amp/build/debug -j$(sysctl -n hw.ncpu)`, and `cd /Volumes/SecondDrive/code2/mark-amp/build/debug && ctest --output-on-failure`.
