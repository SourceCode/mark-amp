**Codebase Review Findings (UX-critical)**

1. `ActivityBar` exists in `/Volumes/SecondDrive/code2/mark-amp/src/ui/ActivityBar.cpp` but is not mounted into the main workbench flow in `/Volumes/SecondDrive/code2/mark-amp/src/ui/LayoutManager.cpp`.
2. `SettingsPanel` exists in `/Volumes/SecondDrive/code2/mark-amp/src/ui/SettingsPanel.cpp` but is not currently surfaced as a first-class, always-accessible settings experience from menus/workbench.
3. Canvas systems are deep (`/Volumes/SecondDrive/code2/mark-amp/src/canvas/*`) but UI integration is shallow (`/Volumes/SecondDrive/code2/mark-amp/src/ui/CanvasPanel.cpp` is isolated).
4. Sidebar mode switching in `/Volumes/SecondDrive/code2/mark-amp/src/ui/LayoutManager.cpp` is limited to explorer/extensions, which blocks the Miro + Notebook + IDE mental model.
5. Command system is broad in `/Volumes/SecondDrive/code2/mark-amp/src/ui/MainFrame.cpp`, but action discovery is not contextual to mode/workflow.

**Next UX Update Set (AI-Agent Optimized)**

**Phase 22 — Unified Workbench Navigation**

1. Goal: Make navigation feel like VSCode + JetBrains tool windows with Miro-like mode switching.
2. Files: `/Volumes/SecondDrive/code2/mark-amp/src/ui/LayoutManager.h`, `/Volumes/SecondDrive/code2/mark-amp/src/ui/LayoutManager.cpp`, `/Volumes/SecondDrive/code2/mark-amp/src/ui/ActivityBar.cpp`, `/Volumes/SecondDrive/code2/mark-amp/src/core/Events.h`.
3. Implement: Mount `ActivityBar` permanently; expand modes to Explorer, Search, Notebooks, Canvas, Graph, Extensions, Settings; add one canonical mode state and event-driven switching.
4. UX outcome: Users always know where they are and can jump surfaces in one click.
5. Tests: extend `/Volumes/SecondDrive/code2/mark-amp/tests/unit/test_layout.cpp` and `/Volumes/SecondDrive/code2/mark-amp/tests/unit/test_keyboard_navigation.cpp`.

**Phase 23 — Tool Window System (JetBrains-style)**

1. Goal: Dockable, pinnable, hideable panels with remembered layout.
2. Files: `/Volumes/SecondDrive/code2/mark-amp/src/ui/PaneManager.cpp`, `/Volumes/SecondDrive/code2/mark-amp/src/ui/WorkspaceLayout.cpp`, `/Volumes/SecondDrive/code2/mark-amp/src/ui/LayoutManager.cpp`, `/Volumes/SecondDrive/code2/mark-amp/src/core/Config.cpp`.
3. Implement: Left/right/bottom tool window containers, pin/auto-hide states, restore last layout per workspace, quick switch shortcuts.
4. UX outcome: Power users can shape the app like IDEs without losing Miro-style visual context.
5. Tests: `/Volumes/SecondDrive/code2/mark-amp/tests/unit/test_workspace_layout.cpp`, `/Volumes/SecondDrive/code2/mark-amp/tests/unit/test_pane_manager.cpp`.

**Phase 24 — Notebook-First Authoring**

1. Goal: Jupyter-grade notebook workflow embedded in the editor workbench.
2. Files: `/Volumes/SecondDrive/code2/mark-amp/src/core/NotebookCellManager.cpp`, `/Volumes/SecondDrive/code2/mark-amp/src/core/KernelManager.cpp`, `/Volumes/SecondDrive/code2/mark-amp/src/ui/EditorPanel.cpp`, `/Volumes/SecondDrive/code2/mark-amp/src/ui/CellOutputRenderer.cpp`, `/Volumes/SecondDrive/code2/mark-amp/src/ui/DataFrameRenderer.cpp`.
3. Implement: Cell boundaries, run controls, output folding, variable/metadata badges, inline result rendering, run-all and selective-run commands.
4. UX outcome: One flow for writing, executing, inspecting outputs without leaving the editor.
5. Tests: `/Volumes/SecondDrive/code2/mark-amp/tests/unit/test_notebook_cells.cpp`, `/Volumes/SecondDrive/code2/mark-amp/tests/unit/test_kernel_manager.cpp`, `/Volumes/SecondDrive/code2/mark-amp/tests/unit/test_cell_output.cpp`.

**Phase 25 — Canvas Workbench Integration**

1. Goal: Miro-class endless canvas as a first-class workspace mode.
2. Files: `/Volumes/SecondDrive/code2/mark-amp/src/ui/CanvasPanel.cpp`, `/Volumes/SecondDrive/code2/mark-amp/src/canvas/CanvasInputManager.cpp`, `/Volumes/SecondDrive/code2/mark-amp/src/canvas/SelectionManager.cpp`, `/Volumes/SecondDrive/code2/mark-amp/src/canvas/SnapEngine.cpp`, `/Volumes/SecondDrive/code2/mark-amp/src/canvas/MinimapPanel.cpp`.
3. Implement: Tool rail + contextual top bar + inspector + minimap; full selection/transform/snapping; command palette parity for all canvas tools.
4. UX outcome: Whiteboarding, diagramming, planning all feel native, not bolted on.
5. Tests: `/Volumes/SecondDrive/code2/mark-amp/tests/unit/test_canvas_input.cpp`, `/Volumes/SecondDrive/code2/mark-amp/tests/unit/test_selection_manager.cpp`, `/Volumes/SecondDrive/code2/mark-amp/tests/unit/test_snap_engine.cpp`.

**Phase 26 — Cross-Surface Linking (Code ⇄ Notebook ⇄ Canvas ⇄ Graph)**

1. Goal: Turn app features into one connected product, not separate modules.
2. Files: `/Volumes/SecondDrive/code2/mark-amp/src/core/Events.h`, `/Volumes/SecondDrive/code2/mark-amp/src/ui/GraphViewPanel.cpp`, `/Volumes/SecondDrive/code2/mark-amp/src/ui/EditorPanel.cpp`, `/Volumes/SecondDrive/code2/mark-amp/src/ui/CanvasPanel.cpp`, `/Volumes/SecondDrive/code2/mark-amp/src/ui/PreviewPanel.cpp`.
3. Implement: Shared entity IDs and reveal events; “reveal in graph/canvas/source”; backlink navigation; preview click-to-source with bidirectional focus.
4. UX outcome: Users can move through ideas, code, outputs, and diagrams fluidly.
5. Tests: `/Volumes/SecondDrive/code2/mark-amp/tests/unit/test_panel_data_flow.cpp`, `/Volumes/SecondDrive/code2/mark-amp/tests/unit/test_graph_view_panel.cpp`, `/Volumes/SecondDrive/code2/mark-amp/tests/unit/test_live_preview.cpp`.

**Phase 27 — Settings, Profiles, and Discoverability**

1. Goal: Make configuration and onboarding as strong as JetBrains and VSCode.
2. Files: `/Volumes/SecondDrive/code2/mark-amp/src/ui/SettingsPanel.cpp`, `/Volumes/SecondDrive/code2/mark-amp/src/ui/MainFrame.cpp`, `/Volumes/SecondDrive/code2/mark-amp/src/ui/CommandPalette.cpp`, `/Volumes/SecondDrive/code2/mark-amp/src/ui/StartupPanel.cpp`.
3. Implement: Always-accessible settings from menu, palette, and workbench; role-based profiles (`Developer`, `Research`, `Whiteboard`, `Notebook`); first-run guided setup.
4. UX outcome: Faster time-to-value and less friction configuring the app.
5. Tests: `/Volumes/SecondDrive/code2/mark-amp/tests/unit/test_mainframe_menu.cpp`, `/Volumes/SecondDrive/code2/mark-amp/tests/unit/test_keyboard_navigation.cpp`, `/Volumes/SecondDrive/code2/mark-amp/tests/unit/test_accessibility.cpp`.

**Phase 28 — Quality Gate (Design + Usability + Performance)**

1. Goal: Ship with measurable standards, not subjective polish.
2. Files: `/Volumes/SecondDrive/code2/mark-amp/tests/unit/test_phase20_perf.cpp`, `/Volumes/SecondDrive/code2/mark-amp/tests/unit/test_accessibility.cpp`, `/Volumes/SecondDrive/code2/mark-amp/tests/unit/test_editor_qol.cpp`, `/Volumes/SecondDrive/code2/mark-amp/tests/unit/test_canvas_renderer.cpp`.
3. Implement: Define budgets for input latency, preview update latency, canvas pan/zoom FPS, contrast and keyboard coverage thresholds.
4. UX outcome: App feels premium under real workloads, not just demos.
5. Release gate: pass build and full tests via `cmake --preset debug`, `cmake --build /Volumes/SecondDrive/code2/mark-amp/build/debug -j$(sysctl -n hw.ncpu)`, `cd /Volumes/SecondDrive/code2/mark-amp/build/debug && ctest --output-on-failure`.

If you want, I can turn Phase 22 into a concrete implementation ticket pack next (file-by-file diffs, event additions, and test cases).

**Next UX Set: Cross-Surface Linking + Seamless Traversal**

1. **Phase 29: Unified Surface Link Contract**
   Create one canonical deep-link model so any surface can open/reveal any other surface without custom glue.
   Scope: define `SurfaceKind`, `EntityKind`, `LinkAnchor`, `SurfaceLink`.
   Files: `/Volumes/SecondDrive/code2/mark-amp/src/core/Events.h`, `/Volumes/SecondDrive/code2/mark-amp/src/ui/NavigationService.h`, `/Volumes/SecondDrive/code2/mark-amp/src/ui/NavigationService.cpp`, plus new `/Volumes/SecondDrive/code2/mark-amp/src/core/SurfaceLink.h`.
   Implementation:
   `NavigationEntry` currently stores only document/scroll/line; extend it to include `from_surface`, `to_surface`, `entity_id`, `file_path`, `line`, `column`, `board_id`, `object_id`, `cell_id`.
   Add events: `OpenSurfaceLinkRequestEvent`, `RevealInSurfaceRequestEvent`, `SurfaceLinkResolvedEvent`, `SurfaceTraversalFailedEvent`.
   Wire link emitters in `/Volumes/SecondDrive/code2/mark-amp/src/ui/EditorPanel.cpp`, `/Volumes/SecondDrive/code2/mark-amp/src/ui/PreviewPanel.cpp`, `/Volumes/SecondDrive/code2/mark-amp/src/ui/GraphViewPanel.cpp`, `/Volumes/SecondDrive/code2/mark-amp/src/ui/CanvasWorkspacePanel.cpp` (or current canvas host).
   Acceptance: one call path opens or reveals targets across editor/preview/canvas/graph; failures produce actionable diagnostics, not silent no-ops.

2. **Phase 30: Transition Orchestrator (Smooth Surface Switching)**
   Add a coordinator that handles animation, preload, focus handoff, and state persistence when moving between surfaces.
   Files: new `/Volumes/SecondDrive/code2/mark-amp/src/ui/SurfaceTransitionCoordinator.h` and `/Volumes/SecondDrive/code2/mark-amp/src/ui/SurfaceTransitionCoordinator.cpp`, integrate with `/Volumes/SecondDrive/code2/mark-amp/src/ui/LayoutManager.cpp` and `/Volumes/SecondDrive/code2/mark-amp/src/ui/SplitView.cpp`.
   Implementation:
   Before transition, capture source state snapshot: cursor/selection/scroll/zoom/active object.
   Preload target panel data before first paint to avoid blank frames.
   Animate container-level transitions only, not heavy child relayout loops.
   On completion, restore logical focus and publish `SurfaceTransitionCompletedEvent`.
   Acceptance: switching between two surfaces feels continuous with no flash/jump/focus loss.

3. **Phase 31: Two-Way Anchor Mapping (Editor ↔ Preview, Editor ↔ Canvas)**
   Build stable anchors so “go there” and “come back” are symmetric.
   Files: `/Volumes/SecondDrive/code2/mark-amp/src/rendering/HtmlRenderer.cpp`, `/Volumes/SecondDrive/code2/mark-amp/src/ui/PreviewPanel.cpp`, `/Volumes/SecondDrive/code2/mark-amp/src/ui/EditorPanel.cpp`, new `/Volumes/SecondDrive/code2/mark-amp/src/core/AnchorMapService.h`, `/Volumes/SecondDrive/code2/mark-amp/src/core/AnchorMapService.cpp`.
   Implementation:
   For editor-preview, emit source-location attributes in rendered blocks and preserve anchor IDs across incremental renders.
   For editor-canvas, allow canvas objects to store source anchors (`file + line-range + heading-id`) and resolve back to editor.
   Add reciprocal commands: `Reveal in Preview`, `Reveal in Editor`, `Reveal in Canvas`, `Return to Source`.
   Acceptance: any reveal action can return to origin in one action with preserved context.

4. **Phase 32: Global Traversal UX (Back/Forward Across Surfaces)**
   Turn navigation history into a product-level traversal system, not per-pane-only.
   Files: `/Volumes/SecondDrive/code2/mark-amp/src/ui/NavigationService.h`, `/Volumes/SecondDrive/code2/mark-amp/src/ui/NavigationService.cpp`, `/Volumes/SecondDrive/code2/mark-amp/src/ui/Toolbar.cpp`, `/Volumes/SecondDrive/code2/mark-amp/src/ui/StatusBarPanel.cpp`, `/Volumes/SecondDrive/code2/mark-amp/src/ui/MainFrame.cpp`.
   Implementation:
   Add cross-surface global history stack with back/forward commands.
   Show lightweight “navigation breadcrumb chips” with previous surface and anchor context.
   Add commands and menu entries for `Back`, `Forward`, `Open Recent Context`.
   Keyboard-first defaults should mirror IDE expectations.
   Acceptance: users can traverse thinking flow across two surfaces without manually re-finding context.

5. **Phase 33: Context-Preserving Handoffs**
   When jumping surfaces, preserve task context and reduce cognitive reset.
   Files: `/Volumes/SecondDrive/code2/mark-amp/src/ui/LayoutManager.cpp`, `/Volumes/SecondDrive/code2/mark-amp/src/ui/BreadcrumbBar.cpp`, `/Volumes/SecondDrive/code2/mark-amp/src/ui/ProblemsPanel.cpp`, `/Volumes/SecondDrive/code2/mark-amp/src/ui/GraphViewPanel.cpp`.
   Implementation:
   Carry “why user navigated” metadata (`debug`, `review`, `explore`, `fix`) with transition events.
   Use that reason to select initial target pane mode and highlights.
   Add transient in-target context banner (“Opened from Preview heading X”, “Opened from Canvas object Y”).
   Acceptance: users understand transition origin immediately and continue work without orientation loss.

6. **Phase 34: Reliability, Performance, and QA Gates**
   Files: new `/Volumes/SecondDrive/code2/mark-amp/tests/unit/test_surface_navigation.cpp`, `/Volumes/SecondDrive/code2/mark-amp/tests/unit/test_surface_transitions.cpp`, plus updates to `/Volumes/SecondDrive/code2/mark-amp/tests/unit/test_split_view_advanced.cpp`, `/Volumes/SecondDrive/code2/mark-amp/tests/unit/test_live_preview.cpp`, `/Volumes/SecondDrive/code2/mark-amp/tests/unit/test_canvas_input.cpp`, `/Volumes/SecondDrive/code2/mark-amp/tests/unit/test_keyboard_navigation.cpp`.
   Implementation:
   Add determinism tests for link resolution and return-to-source.
   Add transition timing budgets and no-flicker assertions.
   Add focus/keyboard traversal tests after every cross-surface jump.
   Acceptance: transitions are smooth under load, history is correct, and keyboard-only traversal is fully reliable.

7. **AI Coding Agent Execution Order**
   Do this in strict order for low regression risk.
   Step 1: ship Phase 29 event/model contract.
   Step 2: ship Phase 30 transition coordinator integration.
   Step 3: ship Phase 31 anchor mapping.
   Step 4: ship Phase 32 traversal UI/commands.
   Step 5: ship Phase 33 contextual handoffs.
   Step 6: enforce Phase 34 test/perf gates.
   Completion gate: `cmake --preset debug`, `cmake --build /Volumes/SecondDrive/code2/mark-amp/build/debug -j$(sysctl -n hw.ncpu)`, `cd /Volumes/SecondDrive/code2/mark-amp/build/debug && ctest --output-on-failure`.
