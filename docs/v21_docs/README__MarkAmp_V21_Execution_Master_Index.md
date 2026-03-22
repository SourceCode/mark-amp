# MarkAmp V21 Execution Master Index

## Purpose

`v21` is a targeted execution-planning pass for control integrity, panel completeness, and settings completeness. It is not a general product plan. It assumes every visible affordance is broken or incomplete until repository inspection proves otherwise.

This package defines the next implementation wave required to make menus, buttons, command entry points, panels, and settings surfaces trustworthy and complete.

## What Changed From V18, V19, And V20

- `v18` and `v19` established broad shell, panel, settings, and interaction integration direction.
- `v20` escalated artifact creation, lifecycle reliability, syntax quality, and premium visual polish.
- `v21` narrows scope completely to:
  - menu and command execution integrity,
  - button and actionable-control integrity,
  - panel implementation and lifecycle completeness,
  - settings architecture, UI, persistence, and application integrity,
  - removal of dead or misleading UI.

## Highest-Priority Next Moves

1. Replace duplicated action registration across [MainFrame.cpp](/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp), [WorkbenchCommands.cpp](/Users/ryanrentfro/code/markamp/src/core/WorkbenchCommands.cpp), [Toolbar.cpp](/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp), and [CommandPalette.cpp](/Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp) with one canonical control-action model.
2. Audit the `MainFrame` menu bar and bind or remove every currently unbound Tools, Notebook, Canvas, Git, Data, and Sync menu entry.
3. Replace placeholder sidebar factories in [LayoutManager.cpp](/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp) with real panel hosts or remove them from production surfaces.
4. Rebuild settings around one staged-and-applied system, because [SettingsPanel.cpp](/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp), [SettingsDialog.cpp](/Users/ryanrentfro/code/markamp/src/ui/SettingsDialog.cpp), [SettingsStateOwner.cpp](/Users/ryanrentfro/code/markamp/src/core/SettingsStateOwner.cpp), [SettingsJsonEditor.cpp](/Users/ryanrentfro/code/markamp/src/ui/SettingsJsonEditor.cpp), [WorkspaceSettings.cpp](/Users/ryanrentfro/code/markamp/src/core/WorkspaceSettings.cpp), and [SettingsDeepLinkRouter.cpp](/Users/ryanrentfro/code/markamp/src/core/SettingsDeepLinkRouter.cpp) currently disagree on ownership and lifecycle.
5. Add control and settings validation harnesses that make dead buttons, no-op menus, stub panels, and non-persistent settings release-blocking.

## Highest-Risk Control Surfaces

- Main menu bar in [MainFrame.cpp](/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp): large surface area, many appended items, incomplete binding coverage.
- Toolbar in [Toolbar.cpp](/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp): hardcoded button behavior separate from [ToolbarModel.cpp](/Users/ryanrentfro/code/markamp/src/ui/ToolbarModel.cpp).
- Command palette in [MainFrame.cpp](/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp) and [CommandPalette.cpp](/Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp): separate command universe.
- Context menus across [FileTreeCtrl.cpp](/Users/ryanrentfro/code/markamp/src/ui/FileTreeCtrl.cpp), [TabBar.cpp](/Users/ryanrentfro/code/markamp/src/ui/TabBar.cpp), [OutputPanel.cpp](/Users/ryanrentfro/code/markamp/src/ui/OutputPanel.cpp), [SourceControlPanel.cpp](/Users/ryanrentfro/code/markamp/src/ui/SourceControlPanel.cpp), [TerminalPanel.cpp](/Users/ryanrentfro/code/markamp/src/ui/TerminalPanel.cpp), and [EditorPanel.cpp](/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp): still mostly ad hoc native `wxMenu` paths despite the presence of [ContextMenuRegistry.cpp](/Users/ryanrentfro/code/markamp/src/ui/ContextMenuRegistry.cpp) and [ThemedContextMenu.cpp](/Users/ryanrentfro/code/markamp/src/ui/ThemedContextMenu.cpp).

## Highest-Risk Panel Gaps

- Primary sidebar placeholder factories in [LayoutManager.cpp](/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp) for Settings, Themes, Notebooks, Canvas, Graph, and AI.
- Secondary sidebar placeholder panels in [LayoutManager.cpp](/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp) for outline, backlinks, and graph mini-map.
- Empty explorer subsections in [ExplorerPanel.cpp](/Users/ryanrentfro/code/markamp/src/ui/ExplorerPanel.cpp) for outline and timeline.
- Explicit stubs in [HistoryPanel.cpp](/Users/ryanrentfro/code/markamp/src/ui/HistoryPanel.cpp) and [PDFViewerPanel.cpp](/Users/ryanrentfro/code/markamp/src/ui/PDFViewerPanel.cpp).
- Partial workflow panels such as [ProblemsPanel.cpp](/Users/ryanrentfro/code/markamp/src/ui/ProblemsPanel.cpp) and [SearchSidebarPanel.cpp](/Users/ryanrentfro/code/markamp/src/ui/SearchSidebarPanel.cpp) that have UI but still contain placeholder execution seams.

## Highest-Risk Settings Architecture Gaps

- Immediate-write behavior in [SettingsPanel.cpp](/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp) contradicts staged apply/cancel behavior in [SettingsDialog.cpp](/Users/ryanrentfro/code/markamp/src/ui/SettingsDialog.cpp).
- [SettingsStateOwner.cpp](/Users/ryanrentfro/code/markamp/src/core/SettingsStateOwner.cpp) exists as a canonical owner but is not the actual UI owner and still contains placeholder export behavior.
- [SettingsDeepLinkRouter.cpp](/Users/ryanrentfro/code/markamp/src/core/SettingsDeepLinkRouter.cpp) logs deep links but does not route them into a real host.
- [SettingsJsonEditor.cpp](/Users/ryanrentfro/code/markamp/src/ui/SettingsJsonEditor.cpp) uses weak validation and direct config mutation.
- [WorkspaceSettings.cpp](/Users/ryanrentfro/code/markamp/src/core/WorkspaceSettings.cpp) has stub persistence.
- [SettingsCatalogBootstrap.cpp](/Users/ryanrentfro/code/markamp/src/core/SettingsCatalogBootstrap.cpp), [SettingsCatalog.cpp](/Users/ryanrentfro/code/markamp/src/core/SettingsCatalog.cpp), and [SettingsPanel.cpp](/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp) overlap in schema ownership.

## Phase Order

1. `Phase_01__Control_Audit_Spine_And_Canonical_Action_Model.md`
2. `Phase_02__Main_Menu_Command_Palette_And_Shortcut_Parity.md`
3. `Phase_03__Toolbar_Status_Bar_And_Global_Button_Wiring.md`
4. `Phase_04__Context_Menus_Inline_Actions_And_Target_Aware_Enablement.md`
5. `Phase_05__Panel_Registry_Primary_Sidebar_And_Controlled_Surface_Replacement.md`
6. `Phase_06__Secondary_Bottom_And_Specialized_Panel_Lifecycle_Completion.md`
7. `Phase_07__Explorer_Search_Problems_Output_Terminal_And_Debug_Surface_Completion.md`
8. `Phase_08__Settings_Architecture_And_Schema_Consolidation.md`
9. `Phase_09__Settings_UI_Persistence_Application_And_JSON_Editor_Completion.md`
10. `Phase_10__Control_Panel_And_Settings_Validation_Harnesses_And_Dead_UI_Removal.md`

## Critical Path Dependencies

- Phase 01 must land first because canonical action IDs, control inventory, and control-state derivation are prerequisites for every other phase.
- Phase 02, Phase 03, and Phase 04 can then proceed partly in parallel once the control-action model is defined.
- Phase 05 and Phase 06 depend on the control model and panel registry cleanup because panel actions, visibility, and lifecycle state must flow through the same command rules.
- Phase 08 must start before Phase 09 because settings ownership and schema authority must be settled before the UI can be completed safely.
- Phase 10 spans the whole wave but becomes release-gating once the migration work is underway.

## Parallelizable Workstreams

- Main menu audit and toolbar/global-button audit after Phase 01.
- Primary-sidebar placeholder replacement and secondary/bottom-panel lifecycle repair after panel contracts are defined.
- Settings schema consolidation and settings UI completion after one canonical settings owner is chosen.
- Control-behavior smoke tests can be built alongside implementation once the canonical action model is in place.

## Improvement Counts

| Phase | Improvements |
| --- | ---: |
| Phase 01 | 126 |
| Phase 02 | 126 |
| Phase 03 | 126 |
| Phase 04 | 126 |
| Phase 05 | 126 |
| Phase 06 | 126 |
| Phase 07 | 132 |
| Phase 08 | 132 |
| Phase 09 | 132 |
| Phase 10 | 132 |
| Total | 1284 |

## Overall Acceptance Criteria

- Every visible menu item, toolbar action, button, and context action resolves to a real execution path or is removed.
- Control enablement and visibility are derived from one context-aware action system rather than widget-local assumptions.
- No production panel remains as a placeholder shell or dead empty state when the UI suggests implemented functionality.
- Panels participate correctly in registration, creation, activation, docking, visibility, restore, and command targeting.
- Settings have one authoritative schema, one authoritative staged/apply lifecycle, reliable persistence, real deep-link/search support, and correct runtime application behavior.
- No dead UI, no misleading affordances, and no pseudo-settings or pseudo-panel surfaces remain in production state.

