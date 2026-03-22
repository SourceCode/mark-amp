# MarkAmp V21 Control Panels And Settings Assessment

## Executive Summary

The repository has substantial UI breadth, but the control layer is still too fragmented to be trusted. Menus, buttons, context menus, panel shells, and settings surfaces exist in large numbers, but they are not governed by one authoritative action model, one authoritative panel lifecycle, or one authoritative settings lifecycle.

The product problem in this pass is not absence of controls. It is that many controls still look complete while being duplicated, partially wired, placeholder-backed, or lifecycle-inconsistent.

## Menus And Action Surface Findings

### Main Menu Bar

- [MainFrame.cpp](/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp) appends a very large File/Edit/View/Tools/Notebooks/Canvas/Git/Data/Sync/Window/Help menu model.
- The same file still contains direct widget-level `Bind(wxEVT_MENU, ...)` handlers and direct event publications instead of routing everything through the central command system.
- Repository search shows that many `kMenuTools*`, `kMenuNotebook*`, `kMenuCanvas*`, `kMenuGit*`, `kMenuData*`, and `kMenuSync*` IDs are appended in the menu but do not appear elsewhere in the file as actual bound handlers.

Conclusion: the main menu is broad but not yet trustworthy. It likely contains no-op or inconsistent items and duplicates logic already present elsewhere.

### Command Palette And Commands

- [WorkbenchCommands.cpp](/Users/ryanrentfro/code/markamp/src/core/WorkbenchCommands.cpp) defines a central command registry.
- [MainFrame.cpp](/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp) still manually registers a separate command-palette command universe.
- The palette commands for notebooks and canvas still use direct event publications tied to older notebook/canvas flows.

Conclusion: command registration is still duplicated and can diverge by surface.

### Toolbar And Global Controls

- [Toolbar.cpp](/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp) hardcodes its own button set and click behavior.
- [ToolbarModel.cpp](/Users/ryanrentfro/code/markamp/src/ui/ToolbarModel.cpp) exists but does not drive the visible toolbar.
- The toolbar settings button only publishes `SettingsOpenRequestEvent`; it does not prove settings deep-link correctness or host availability.

Conclusion: the global toolbar is not yet backed by the same control model the rest of the shell needs.

### Context Menus

- [ContextMenuRegistry.cpp](/Users/ryanrentfro/code/markamp/src/ui/ContextMenuRegistry.cpp) and [ThemedContextMenu.cpp](/Users/ryanrentfro/code/markamp/src/ui/ThemedContextMenu.cpp) exist.
- Real surfaces such as [FileTreeCtrl.cpp](/Users/ryanrentfro/code/markamp/src/ui/FileTreeCtrl.cpp), [TabBar.cpp](/Users/ryanrentfro/code/markamp/src/ui/TabBar.cpp), [OutputPanel.cpp](/Users/ryanrentfro/code/markamp/src/ui/OutputPanel.cpp), [SourceControlPanel.cpp](/Users/ryanrentfro/code/markamp/src/ui/SourceControlPanel.cpp), [TerminalPanel.cpp](/Users/ryanrentfro/code/markamp/src/ui/TerminalPanel.cpp), [GitLogPanel.cpp](/Users/ryanrentfro/code/markamp/src/ui/GitLogPanel.cpp), and [EditorPanel.cpp](/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp) still use direct `wxMenu` + `PopupMenu(&menu)` code.

Conclusion: the context-menu system is duplicated and inconsistent.

## Panel Findings

### Registry And Shell

- [LayoutManager.cpp](/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp) registers many sidebar panels.
- Several of those registrations are placeholder factories using helper `make_feature_panel`, especially for Settings, Themes, Notebooks, Canvas, and Graph.
- Secondary sidebar registrations also include lightweight placeholder implementations for outline, backlinks, and graph mini-map.
- [SidebarPanelRegistry.cpp](/Users/ryanrentfro/code/markamp/src/ui/SidebarPanelRegistry.cpp) is a lazy widget factory, not a full lifecycle owner.
- [PanelService.cpp](/Users/ryanrentfro/code/markamp/src/core/PanelService.cpp) is only a thin wrapper over [PanelAreaModel.cpp](/Users/ryanrentfro/code/markamp/src/ui/PanelAreaModel.cpp).
- [PanelLifecycleManager.cpp](/Users/ryanrentfro/code/markamp/src/ui/PanelLifecycleManager.cpp) stores snapshots in memory and provides built-in defaults, but it is not a complete persisted panel-lifecycle system.

Conclusion: panel registration exists, but panel completeness and lifecycle authority are still weak.

### Panel Implementations

- [ExplorerPanel.cpp](/Users/ryanrentfro/code/markamp/src/ui/ExplorerPanel.cpp) has real explorer content but its outline and timeline sections are empty container panels.
- [SearchSidebarPanel.cpp](/Users/ryanrentfro/code/markamp/src/ui/SearchSidebarPanel.cpp) still uses workspace-root `"."` and temporary async management placeholders.
- [ProblemsPanel.cpp](/Users/ryanrentfro/code/markamp/src/ui/ProblemsPanel.cpp) has real UI, but navigation and quick-fix paths remain placeholders.
- [HistoryPanel.cpp](/Users/ryanrentfro/code/markamp/src/ui/HistoryPanel.cpp) is explicitly marked stub.
- [PDFViewerPanel.cpp](/Users/ryanrentfro/code/markamp/src/ui/PDFViewerPanel.cpp) is explicitly marked stub.

Conclusion: multiple panels are visually present but not yet complete by IDE standards.

## Settings Findings

### Lifecycle Contradictions

- [SettingsDialog.cpp](/Users/ryanrentfro/code/markamp/src/ui/SettingsDialog.cpp) presents staged edit behavior with Apply, Cancel, unsaved indicators, and change confirmation.
- [SettingsPanel.cpp](/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp) still writes directly to `Config` in `OnSettingChanged()` and `ResetSettingToDefault()`, and saves immediately.
- The same panel also has a separate `pending_changes_` API with `ApplyPendingChanges()` and `DiscardPendingChanges()`.

Conclusion: the settings UI promises staged behavior while the underlying panel still performs immediate mutation.

### Split Ownership

- [SettingsStateOwner.cpp](/Users/ryanrentfro/code/markamp/src/core/SettingsStateOwner.cpp) exists as a canonical owner but is not the actual owner of the visible settings UI.
- [SettingsDeepLinkRouter.cpp](/Users/ryanrentfro/code/markamp/src/core/SettingsDeepLinkRouter.cpp) logs and sends an activity-bar event, but does not route deep links into a real settings host.
- [SettingsJsonEditor.cpp](/Users/ryanrentfro/code/markamp/src/ui/SettingsJsonEditor.cpp) uses weak brace-count validation, a fake format operation, and direct config mutation.
- [WorkspaceSettings.cpp](/Users/ryanrentfro/code/markamp/src/core/WorkspaceSettings.cpp) still has stub save/load behavior.

Conclusion: settings ownership, persistence, and deep-link behavior are split across too many incomplete pathways.

### Schema Duplication

- [SettingsCatalog.cpp](/Users/ryanrentfro/code/markamp/src/core/SettingsCatalog.cpp) defines a richer catalog model.
- [SettingsCatalogBootstrap.cpp](/Users/ryanrentfro/code/markamp/src/core/SettingsCatalogBootstrap.cpp) separately registers built-in settings.
- [SettingsPanel.cpp](/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp) also has `RegisterBuiltinSettings()`.

Conclusion: settings definitions are duplicated, which makes completeness and validation drift likely.

## Highest-Risk Product Gaps For V21

1. Menus that look complete but are not bound.
2. Buttons that trigger direct widget callbacks instead of canonical commands.
3. Context menus that bypass the registry/model system.
4. Panels that mount as placeholders or empty shells but are visible in navigation.
5. Settings controls that apply, persist, and revert inconsistently.
6. Settings deep links, settings search, and settings scope changes that are visually present but not truly integrated.

## V21 Acceptance Lens

`v21` succeeds only if the implementation team can use it to turn every visible control, every panel shell, and every settings surface into a real, testable, trustworthy part of the IDE. The bar is not “present in the UI.” The bar is “wired, correct, persistent where needed, context-aware, and release-gated.”

