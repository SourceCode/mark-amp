# MarkAmp V22 UI Execution Master Index

## Purpose

`v22` is a UI-only planning pass. It is not a general architecture pass, not a backend-first pass, and not a functionality-discovery pass. Its sole purpose is to define the next implementation wave required to make MarkAmp look and feel like an elite, premium IDE / canvas / notebook environment.

This package focuses entirely on visible product quality:

- visual cohesion
- layout intelligence
- spacing and density
- typography and hierarchy
- control styling and interaction states
- shell chrome and panel polish
- editor, notebook, and canvas presentation
- syntax-highlighting presentation quality
- theme parity
- micro-interactions and final fit-and-finish

## What Changed From Prior Planning Generations

- `v18` and `v19` focused on broad cohesion, shell integration, and dependency-aware sequencing.
- `v20` elevated broken creation workflows, syntax-highlighting quality, and premium styling into major finish work.
- `v21` narrowed to menus, controls, panels, and settings integrity.
- `v22` narrows even further in one dimension and expands dramatically in another:
  - it is UI-only,
  - it assumes every visible surface is improvable,
  - it treats every hardcoded spacing, type, color, icon, state, and motion decision as suspect until aligned to a premium visual system,
  - it converts the repository’s many UI seams into one 20-phase execution blueprint.

## Highest-Priority UI Workstreams

1. Replace subsystem-local visual decisions with one durable shell-wide visual language across [UILayoutTokens.h](/Users/ryanrentfro/code/markamp/src/ui/UILayoutTokens.h), [LayoutMetrics.h](/Users/ryanrentfro/code/markamp/src/ui/LayoutMetrics.h), [ThemeTokens.h](/Users/ryanrentfro/code/markamp/src/core/ThemeTokens.h), [ThemeEngine.cpp](/Users/ryanrentfro/code/markamp/src/core/ThemeEngine.cpp), and local widget code.
2. Rebuild the shell chrome quality of [ActivityBar.cpp](/Users/ryanrentfro/code/markamp/src/ui/ActivityBar.cpp), [Toolbar.cpp](/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp), [TabBar.cpp](/Users/ryanrentfro/code/markamp/src/ui/TabBar.cpp), [StatusBarPanel.cpp](/Users/ryanrentfro/code/markamp/src/ui/StatusBarPanel.cpp), and [LayoutManager.cpp](/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp).
3. Raise editor presentation quality in [EditorPanel.cpp](/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp), [MinimapPanel.cpp](/Users/ryanrentfro/code/markamp/src/ui/MinimapPanel.cpp), and [OverviewRulerPanel.cpp](/Users/ryanrentfro/code/markamp/src/ui/OverviewRulerPanel.cpp) to a visibly more intentional code-reading experience.
4. Turn notebook UI intent into a real visual system, because [NotebookCellChrome.h](/Users/ryanrentfro/code/markamp/src/core/NotebookCellChrome.h), [NotebookCellChrome.cpp](/Users/ryanrentfro/code/markamp/src/core/NotebookCellChrome.cpp), [NotebookToolbar.cpp](/Users/ryanrentfro/code/markamp/src/core/NotebookToolbar.cpp), [NotebookShellHost.cpp](/Users/ryanrentfro/code/markamp/src/core/NotebookShellHost.cpp), and [CellOutputRenderer.cpp](/Users/ryanrentfro/code/markamp/src/ui/CellOutputRenderer.cpp) still stop short of a premium notebook surface.
5. Replace the placeholder and low-fidelity canvas shell in [CanvasWorkspacePanel.cpp](/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp) and visually unify it with [CanvasWorkbench.cpp](/Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp), [CanvasRenderer.cpp](/Users/ryanrentfro/code/markamp/src/canvas/CanvasRenderer.cpp), [CanvasInspector.cpp](/Users/ryanrentfro/code/markamp/src/canvas/CanvasInspector.cpp), and related canvas UI.
6. Make settings, dialogs, notifications, onboarding, and data views feel premium instead of utilitarian or placeholder-backed.
7. Complete theme parity, iconography normalization, motion polish, and visual cleanup so the interface feels like one designed product rather than many local implementations.

## Highest-Risk Visual Consistency Gaps

- Duplicate geometry and density systems across [UILayoutTokens.h](/Users/ryanrentfro/code/markamp/src/ui/UILayoutTokens.h), [LayoutMetrics.h](/Users/ryanrentfro/code/markamp/src/ui/LayoutMetrics.h), `SpacingGrid`, and local literals.
- Widespread local styling in [Toolbar.cpp](/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp), [ThemeGallery.cpp](/Users/ryanrentfro/code/markamp/src/ui/ThemeGallery.cpp), [StartupPanel.cpp](/Users/ryanrentfro/code/markamp/src/ui/StartupPanel.cpp), [CommandPalette.cpp](/Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp), and panel implementations instead of enforced surface tokens.
- Mixed use of native widgets, custom-painted widgets, and themed controls with inconsistent hover, focus, pressed, disabled, and selected treatments.
- Incomplete icon normalization: MUI migration work exists, but [CanvasWorkspacePanel.cpp](/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp) still uses Unicode placeholders and many surfaces still use local optical spacing assumptions.
- Placeholder-heavy or visually thin surfaces such as [StartupPanel_styling.txt](/Users/ryanrentfro/code/markamp/src/ui/StartupPanel_styling.txt), [SidebarSkeletonPlaceholder.cpp](/Users/ryanrentfro/code/markamp/src/ui/SidebarSkeletonPlaceholder.cpp), [AVTablePanel.cpp](/Users/ryanrentfro/code/markamp/src/ui/av/AVTablePanel.cpp), and several secondary sidebar shells.

## Highest-Risk Theme / Styling Gaps

- Theme tokens exist, but many surfaces still hardcode fonts, sizes, colors, and corner radii instead of resolving through [ThemeEngine.cpp](/Users/ryanrentfro/code/markamp/src/core/ThemeEngine.cpp).
- [MinimapPanel.cpp](/Users/ryanrentfro/code/markamp/src/ui/MinimapPanel.cpp) uses simplified token mapping and local line rendering assumptions that fall short of editor-grade polish.
- [EditorPanel.cpp](/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp) has rich styling logic, but it remains local, markdown-leaning, and manually tuned rather than clearly systematized.
- [ThemeGallery.cpp](/Users/ryanrentfro/code/markamp/src/ui/ThemeGallery.cpp) and [ThemePreviewCard.cpp](/Users/ryanrentfro/code/markamp/src/ui/ThemePreviewCard.cpp) still feel like utility tooling rather than premium theme-management surfaces.
- Built-in and imported themes need stronger shell/editor/notebook/canvas parity.

## Highest-Risk Editor / Notebook / Canvas UI Gaps

- Editor chrome is powerful but visually uneven, with many local rules for caret, gutters, annotations, minimap, whitespace, diagnostics, and find highlights in [EditorPanel.cpp](/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp).
- Notebook chrome is mostly model-side; there is not yet a visibly complete notebook shell comparable to the editor or canvas.
- Canvas workspace UI remains basic and placeholder-heavy in [CanvasWorkspacePanel.cpp](/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp), especially tool buttons, inspector visuals, and minimap strip treatment.
- Panel surfaces such as Explorer, Search, Problems, Terminal, Output, Debug, Settings, and AV views vary too widely in density, hierarchy, and visual finish.

## Phase Order

1. `Phase_01__Visual_Language_Density_And_Design_System_Foundation.md`
2. `Phase_02__Shell_Chrome_Window_Frame_And_Layout_Intelligence.md`
3. `Phase_03__Activity_Bar_Sidebars_Panel_Headers_And_Shell_Chrome.md`
4. `Phase_04__Menus_Command_Palette_Context_Menus_And_Toolbars.md`
5. `Phase_05__Tabs_Breadcrumbs_Navigation_And_Workspace_Wayfinding.md`
6. `Phase_06__Editor_Chrome_Gutter_Minimap_And_Inline_Surface_Polish.md`
7. `Phase_07__Syntax_Highlighting_Code_Readability_And_Token_Presentation.md`
8. `Phase_08__Search_Find_Replace_Diagnostics_Peek_And_Editor_Overlays.md`
9. `Phase_09__Notebook_Shell_Cell_Chrome_Output_And_Mixed_Content_Presentation.md`
10. `Phase_10__Canvas_Workspace_Tools_Object_Chrome_And_Interaction_Visuals.md`
11. `Phase_11__Explorer_Search_Sidebar_Graph_And_Inspector_Panel_Presentation.md`
12. `Phase_12__Terminal_Output_Problems_Build_Debug_And_Source_Control_Panels.md`
13. `Phase_13__Settings_Preferences_Keybindings_And_Theme_Surface_Presentation.md`
14. `Phase_14__Dialogs_Popovers_Tooltips_Notifications_And_Transient_Overlays.md`
15. `Phase_15__Trees_Lists_Tables_AV_And_Data_Dense_Presentation.md`
16. `Phase_16__Startup_Welcome_Walkthrough_Empty_Loading_And_Error_States.md`
17. `Phase_17__Accessibility_Visual_Clarity_Contrast_And_Reduced_Motion.md`
18. `Phase_18__Theme_Parity_Token_Application_And_Surface_Wide_Styling_Correctness.md`
19. `Phase_19__Motion_Microinteractions_Drag_Resize_And_Focus_Transfer_Polish.md`
20. `Phase_20__Iconography_Visual_Cleanup_Legacy_Style_Removal_And_UI_QA_Gates.md`

## Critical Path Dependencies

- Phase 01 must land first because spacing, density, type, color, corner radius, border, icon, and motion decisions need one system before surface-by-surface polish can scale.
- Phase 02 through Phase 05 depend on the visual-language foundation for shell chrome, layout balance, tabs, menus, and navigation consistency.
- Phase 06 through Phase 10 depend on shared tokens and shell geometry decisions so editor, syntax, notebook, and canvas improvements do not reintroduce local standards.
- Phase 11 through Phase 16 can run in parallel after the surface primitives are stable enough to reuse.
- Phase 17 and Phase 18 overlap the whole wave and should continuously pressure-check contrast, focus visibility, theme correctness, and parity.
- Phase 19 and Phase 20 are end-loaded for polish and cleanup, but they should start earlier as guardrails so new work does not reintroduce visual inconsistency.

## Parallelizable Workstreams

- Shell chrome and navigation polish after Phase 01.
- Editor presentation and syntax-highlighting presentation after the visual language and token contracts are defined.
- Notebook and canvas UI tracks after shell/chrome primitives are in place.
- Panel-heavy workstreams, settings UI, dialogs/notifications, and onboarding surfaces in parallel once foundational tokens and component variants are settled.
- Theme parity and accessibility audits can progress continuously alongside implementation.

## Improvement Counts

| Phase | Improvements |
| --- | ---: |
| Phase 01 | 360 |
| Phase 02 | 360 |
| Phase 03 | 360 |
| Phase 04 | 360 |
| Phase 05 | 360 |
| Phase 06 | 360 |
| Phase 07 | 360 |
| Phase 08 | 360 |
| Phase 09 | 360 |
| Phase 10 | 360 |
| Phase 11 | 360 |
| Phase 12 | 360 |
| Phase 13 | 360 |
| Phase 14 | 360 |
| Phase 15 | 360 |
| Phase 16 | 360 |
| Phase 17 | 360 |
| Phase 18 | 360 |
| Phase 19 | 360 |
| Phase 20 | 360 |
| Total | 7200 |

## Overall Acceptance Criteria

- The UI uses one coherent spacing, density, typography, color, border, icon, and motion language across the shell.
- No major visible surface still relies on ad hoc hardcoded styling when a shared design-system token or component should own it.
- Menus, toolbars, tabs, panels, dialogs, settings, notifications, editor chrome, notebook chrome, and canvas chrome feel like one product family.
- Syntax-highlighting presentation is readable, balanced, and premium in dark, light, and high-contrast modes.
- Editor, notebook, and canvas surfaces are visually excellent in both idle and interaction-heavy states.
- Accessibility-oriented visual clarity is improved materially through focus visibility, contrast, hit-target clarity, and reduced-motion parity.
- MUI icon migration is visually complete and legacy icon residue is removed or gated.
- Loading, empty, onboarding, error, and placeholder states are polished, intentional, and consistent.
- `v22` leaves the application with a visibly superior UI foundation and a direct execution path for finishing the remaining premium-polish work.

