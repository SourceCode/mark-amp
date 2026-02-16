Phase 4 Prompt (Project Intelligence + Automation Workflows)

1. Add a project-level orchestration service in `/Volumes/SecondDrive/code2/mark-amp/src/core/ProjectWorkspaceService.h` and `/Volumes/SecondDrive/code2/mark-amp/src/core/ProjectWorkspaceService.cpp` to unify documents, notebook cells, canvas objects, graph nodes, and tasks under one project context ID.

2. Extend `/Volumes/SecondDrive/code2/mark-amp/src/core/Events.h` with typed project workflow events: `ProjectContextChanged`, `OpenEntityRequest`, `RevealEntityRequest`, `RunWorkflowRequest`, `WorkflowStatusChanged`, and `ProjectInsightUpdated`, and route all mode surfaces through this event contract.

3. Build a “Projects Hub” sidebar mode in `/Volumes/SecondDrive/code2/mark-amp/src/ui/LayoutManager.h` and `/Volumes/SecondDrive/code2/mark-amp/src/ui/LayoutManager.cpp` that shows project-level entities and lets users jump between Explorer, Notebook, Canvas, Graph, and Extensions without losing local state.

4. Add a new panel `/Volumes/SecondDrive/code2/mark-amp/src/ui/ProjectsHubPanel.h` and `/Volumes/SecondDrive/code2/mark-amp/src/ui/ProjectsHubPanel.cpp` with sections for `Recent Workflows`, `Pinned Assets`, `Running Jobs`, and `Suggested Next Actions`, fully theme-token driven.

5. Implement reusable workflow definitions in `/Volumes/SecondDrive/code2/mark-amp/src/core/TaskRunnerService.h` and `/Volumes/SecondDrive/code2/mark-amp/src/core/TaskRunnerService.cpp` (currently stubbed), supporting notebook execution chains, export chains, and canvas snapshot pipelines.

6. Upgrade `/Volumes/SecondDrive/code2/mark-amp/src/ui/CommandPalette.cpp` and `/Volumes/SecondDrive/code2/mark-amp/src/ui/MainFrame.cpp` to add workflow commands (`Run Notebook Pipeline`, `Publish Project Snapshot`, `Sync Graph From Notes`, `Open Projects Hub`) with context-aware ranking by active mode and project state.

7. Connect notebook, canvas, and graph interactions by wiring `/Volumes/SecondDrive/code2/mark-amp/src/ui/NotebookPanel.cpp`, `/Volumes/SecondDrive/code2/mark-amp/src/ui/CanvasWorkspacePanel.cpp`, and `/Volumes/SecondDrive/code2/mark-amp/src/ui/GraphViewPanel.cpp` to shared entity IDs so each surface can reveal and focus the same object.

8. Add an execution timeline model in `/Volumes/SecondDrive/code2/mark-amp/src/core/NotebookCellManager.cpp` and `/Volumes/SecondDrive/code2/mark-amp/src/core/KernelManager.cpp` so notebook runs produce structured run records that can be surfaced in Projects Hub and status bar.

9. Extend `/Volumes/SecondDrive/code2/mark-amp/src/ui/StatusBarPanel.cpp` with project workflow status slots (`active workflow`, `last run result`, `running task count`) using semantic tokens only.

10. Add a plugin-facing workflow contribution point in `/Volumes/SecondDrive/code2/mark-amp/src/core/PluginContext.h` and `/Volumes/SecondDrive/code2/mark-amp/src/core/ExtensionEventBus.h` so extensions can register workflow actions and project insights without custom UI hacks.

11. Add tests in `/Volumes/SecondDrive/code2/mark-amp/tests/unit/test_workspace_layout.cpp`, `/Volumes/SecondDrive/code2/mark-amp/tests/unit/test_panel_data_flow.cpp`, `/Volumes/SecondDrive/code2/mark-amp/tests/unit/test_notebook_cells.cpp`, `/Volumes/SecondDrive/code2/mark-amp/tests/unit/test_canvas_input.cpp`, `/Volumes/SecondDrive/code2/mark-amp/tests/unit/test_graph_view_panel.cpp`, and new `/Volumes/SecondDrive/code2/mark-amp/tests/unit/test_project_workflows.cpp` for orchestration, workflow execution, cross-surface reveal, and command routing.

12. Acceptance criteria: Projects Hub is a first-class mode; workflow commands run end-to-end through TaskRunnerService; notebook/canvas/graph entity linking is bidirectional; status bar reflects workflow state; no hard-coded UI colors are introduced; and build/tests pass with `cmake --preset debug`, `cmake --build /Volumes/SecondDrive/code2/mark-amp/build/debug -j$(sysctl -n hw.ncpu)`, and `cd /Volumes/SecondDrive/code2/mark-amp/build/debug && ctest --output-on-failure`.
