# MarkAmp V27 Full Icon And Visual Overhaul Master Index

## Purpose Of The V27 Pass

`v27` is the full icon-and-visual-overhaul pass for MarkAmp. It is the next implementation-ready package for changing and updating every icon and every visible visual aspect of the application so the product feels fully redesigned, highly coherent, and premium across the entire workspace.

This pass is explicitly about:

- replacing and normalizing every icon
- redesigning every visible UI surface
- modernizing shell chrome and interaction presentation
- eliminating mixed icon systems and legacy visual fragments
- reworking visual hierarchy, spacing, typography, density, borders, radii, depth, and state language
- pushing editor, notebook, canvas, panels, menus, toolbars, settings, and dialogs to best-in-class visual quality

This pass is not for backend completion, general architecture work, or functionality expansion except where visible UI depends on it.

## What Changed From Prior Planning

### From V22

`v22` treated all visible product quality as improvable and produced a very broad UI-only execution plan. `v27` does not repeat that inventory approach. It narrows the next move to a full visual redesign driven by a canonical icon replacement strategy and a more aggressive surface-by-surface overhaul.

### From V26

`v26` focused on premium refinement, convergence, and closure. `v27` assumes that refinement alone is no longer enough. The current state now supports a more aggressive move: replacing the mixed icon stack and redesigning the visible product around one stronger visual identity.

### From Earlier Icon Planning

Earlier icon work framed the problem as migration completion. Repository reality now shows that icon infrastructure exists, but the visible product still mixes multiple icon regimes. `v27` makes icon replacement and icon-driven control redesign foundational instead of secondary.

## What Was Merged, Retired, De-Scoped, Or Re-Sequenced

### Merged

- `v18` through `v20` MUI icon migration intent
- `v22` iconography and cleanup concerns
- `v24` and `v25` syntax/theme/icon visible closure
- `v26` iconography normalization, premium shell refinement, and visual cleanup

### Retired

- broad “discover all UI issues” backlog expansion
- first-pass design-system creation tasks
- first-pass shell modernization tasks
- incremental icon-completion framing that no longer matches repository reality

### De-Scoped

- backend and service work not required for the icon-and-visual overhaul
- invisible functional cleanup that does not materially improve visible quality

### Re-Sequenced

- icon-system replacement now precedes broad surface redesign
- visual-language redesign now sits beside icon replacement as a co-foundation
- icon-driven control redesign is pulled forward instead of being left as cleanup
- motion, accessibility visuals, and parity remain late, but now validate a redesigned interface instead of a refined one

## Highest-Priority Next Visual Workstreams

1. Canonical icon system replacement and semantic remapping.
2. Full visual-language redesign for spacing, typography, radii, borders, depth, density, and interaction states.
3. Shell, toolbar, menu, tab, panel, and dialog redesign using the new icon and visual foundations.
4. Editor, syntax, notebook, and canvas redesign as flagship premium surfaces.
5. Theme parity, accessibility visuals, motion, icon-driven control redesign, and full visual cleanup.

## Critical Path Dependencies

1. Phase 01 and Phase 02 define the canonical icon and visual systems needed by every later phase.
2. Phase 03 completes the icon asset and migration spine required by all action surfaces and navigation surfaces.
3. Phase 04 and Phase 05 establish shell, toolbar, and navigation language that downstream panels, settings, and overlays depend on.
4. Phase 07 through Phase 10 depend on the redesigned icon and visual foundations so editor, notebook, and canvas do not evolve new local languages.
5. Phase 17 through Phase 19 depend on redesigned core surfaces before parity, accessibility, and motion can be validated meaningfully.
6. Phase 20 is the final full-system cleanup and visual signoff gate.

## Parallelizable Workstreams

- Phase 04 and Phase 05 can overlap once the icon and visual foundations are stable.
- Phase 07 and Phase 08 can overlap once editor baseline and syntax direction are agreed.
- Phase 09 and Phase 10 can overlap once shell and control language are stable.
- Phase 11 through Phase 15 can run in parallel after the primary shell/control/icon language is established.
- Phase 17 and Phase 18 can run in parallel across completed surfaces.

## Highest-Risk Icon Inconsistencies

- emoji and unicode fallbacks still used in `/Users/ryanrentfro/code/markamp/src/ui/IconProvider.h`, `/Users/ryanrentfro/code/markamp/src/ui/FileIconResolver.h`, `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/StatusBarPanel.cpp`
- icon-character APIs still exposed through `/Users/ryanrentfro/code/markamp/src/ui/SidebarPanelRegistry.cpp`
- custom inline SVG icon registration in `/Users/ryanrentfro/code/markamp/src/ui/IconLibrary.cpp` coexists with manifest-driven and MUI-oriented systems
- only a limited built-in MUI mapping set currently exists in `/Users/ryanrentfro/code/markamp/src/core/IconInventory.cpp`

## Highest-Risk Visual-System Inconsistencies

- local style logic still competes with tokenized styling in `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/ThemeGallery.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/SearchSidebarPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp`
- shell quality still exceeds several interior surfaces, especially panels, settings, notebook, canvas, and some transient surfaces
- utility and stub-like surfaces still weaken overall product identity

## Highest-Risk Editor / Notebook / Canvas Presentation Gaps

- editor readability and dynamic-state treatment remain strong but not yet fully redesigned
- notebook shell, cell chrome, and output presentation still lag behind the shell
- canvas still exposes placeholder and stand-in chrome in `/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp`

## Highest-Risk Settings / Panel / Menu / Toolbar / Dialog Gaps

- settings still carries emoji-prefix and local styling debt in `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp`
- panel registry and panel shells still depend on icon chars and mixed presentation patterns in `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp` and `/Users/ryanrentfro/code/markamp/src/ui/SidebarPanelRegistry.cpp`
- dialogs and overlays remain less mature than the shell in `/Users/ryanrentfro/code/markamp/src/ui/ThemedDialog.cpp`
- toolbar, menu, and command surfaces still need redesign rather than just convergence

## Overall Acceptance Criteria For V27

- every visible icon is migrated to the canonical system or explicitly justified as a custom adapter case
- the visible product no longer mixes emoji/unicode, icon-char, manifest, and ad hoc icon semantics on the main path
- shell, editor, notebook, canvas, panels, settings, toolbars, menus, dialogs, lists, and feedback surfaces feel fully redesigned
- spacing, typography, depth, icon sizing, control styling, and interaction-state language are coherent across the entire app
- theme parity, accessibility visuals, and motion feel complete and premium
- legacy visual and icon residue is removed or placed behind explicit validation gates

## Phase Overview

| Phase | Title | Task Count | Summary |
| --- | --- | ---: | --- |
| 01 | Canonical Icon System Audit And Replacement Spine | 3 | Inventory every icon source, map every visible icon to the canonical system, and define replacement rules. |
| 02 | Global Visual Language Redesign And Premium System Reset | 3 | Redesign spacing, typography, depth, density, radii, borders, and interaction-state language. |
| 03 | Icon Asset Pipeline, MUI Migration, And Validation Gate Completion | 3 | Finish the MUI-led asset and validation spine and retire mixed icon paths. |
| 04 | Application Shell Workspace Chrome And Region Redesign | 3 | Redesign the main shell, side regions, splitters, and workspace composition. |
| 05 | Menus Command Surfaces Toolbars And Action Cluster Redesign | 3 | Redesign action surfaces around the new icon and visual systems. |
| 06 | Tabs Breadcrumbs Navigation History And Context Redesign | 3 | Redesign navigation and wayfinding surfaces for premium clarity. |
| 07 | Editor Chrome Gutter Overlay And Readability Redesign | 3 | Redesign the main authoring surface and its support chrome. |
| 08 | Syntax Highlighting Palette And Code Presentation Redesign | 3 | Rebuild syntax presentation for premium readability and theme balance. |
| 09 | Notebook Shell Cell Output And Mixed Content Redesign | 3 | Redesign notebook visuals as a flagship surface. |
| 10 | Canvas Shell Tooling Overlay And Creative Surface Redesign | 3 | Redesign canvas chrome, affordances, overlays, and onboarding. |
| 11 | Panels Explorers Inspectors And Side Surface Redesign | 3 | Redesign every panel class and dense information shell. |
| 12 | Settings Preferences Theme Management And Control Redesign | 3 | Redesign settings and control-heavy surfaces with premium hierarchy. |
| 13 | Dialogs Popovers Tooltips Notifications And Transient Surface Redesign | 3 | Redesign modal and transient surfaces into one premium family. |
| 14 | Trees Lists Tables AV And Structured Information Redesign | 3 | Redesign rows, hierarchies, badges, empties, and data-dense surfaces. |
| 15 | Feedback Status Progress Validation And Messaging Redesign | 3 | Redesign feedback language across persistent and transient states. |
| 16 | Startup Welcome Onboarding And Empty State Redesign | 3 | Redesign first-run and empty-state experiences across the product. |
| 17 | Theme Parity Token Normalization And Cross Surface Correctness | 3 | Validate and fix redesigned surfaces across themes. |
| 18 | Accessibility Visuals Contrast Focus And Reduced Motion Redesign | 3 | Rebuild accessibility-visible cues for the redesigned system. |
| 19 | Motion Micro Interactions And Icon Driven Control Redesign | 3 | Finish tactile feel and icon-driven control balance across the app. |
| 20 | Full System Cleanup Legacy Asset Removal And Visual QA Gates | 3 | Remove the old visual world and gate the redesigned one. |

## Total Task Counts

- Total phases: 20
- Total tasks: 60
- Tasks per phase: 3

## Documents In This Package

- Assessment:
  - `/Users/ryanrentfro/code/markamp/docs/v27_docs/ASSESSMENT__MarkAmp_V27_Full_Icon_And_Visual_Overhaul_Assessment.md`
- Phase files:
  - `/Users/ryanrentfro/code/markamp/docs/v27_docs/Phase_01__Canonical_Icon_System_Audit_And_Replacement_Spine.md`
  - `/Users/ryanrentfro/code/markamp/docs/v27_docs/Phase_02__Global_Visual_Language_Redesign_And_Premium_System_Reset.md`
  - `/Users/ryanrentfro/code/markamp/docs/v27_docs/Phase_03__Icon_Asset_Pipeline_MUI_Migration_And_Validation_Gate_Completion.md`
  - `/Users/ryanrentfro/code/markamp/docs/v27_docs/Phase_04__Application_Shell_Workspace_Chrome_And_Region_Redesign.md`
  - `/Users/ryanrentfro/code/markamp/docs/v27_docs/Phase_05__Menus_Command_Surfaces_Toolbars_And_Action_Cluster_Redesign.md`
  - `/Users/ryanrentfro/code/markamp/docs/v27_docs/Phase_06__Tabs_Breadcrumbs_Navigation_History_And_Context_Redesign.md`
  - `/Users/ryanrentfro/code/markamp/docs/v27_docs/Phase_07__Editor_Chrome_Gutter_Overlay_And_Readability_Redesign.md`
  - `/Users/ryanrentfro/code/markamp/docs/v27_docs/Phase_08__Syntax_Highlighting_Palette_And_Code_Presentation_Redesign.md`
  - `/Users/ryanrentfro/code/markamp/docs/v27_docs/Phase_09__Notebook_Shell_Cell_Output_And_Mixed_Content_Redesign.md`
  - `/Users/ryanrentfro/code/markamp/docs/v27_docs/Phase_10__Canvas_Shell_Tooling_Overlay_And_Creative_Surface_Redesign.md`
  - `/Users/ryanrentfro/code/markamp/docs/v27_docs/Phase_11__Panels_Explorers_Inspectors_And_Side_Surface_Redesign.md`
  - `/Users/ryanrentfro/code/markamp/docs/v27_docs/Phase_12__Settings_Preferences_Theme_Management_And_Control_Redesign.md`
  - `/Users/ryanrentfro/code/markamp/docs/v27_docs/Phase_13__Dialogs_Popovers_Tooltips_Notifications_And_Transient_Surface_Redesign.md`
  - `/Users/ryanrentfro/code/markamp/docs/v27_docs/Phase_14__Trees_Lists_Tables_AV_And_Structured_Information_Redesign.md`
  - `/Users/ryanrentfro/code/markamp/docs/v27_docs/Phase_15__Feedback_Status_Progress_Validation_And_Messaging_Redesign.md`
  - `/Users/ryanrentfro/code/markamp/docs/v27_docs/Phase_16__Startup_Welcome_Onboarding_And_Empty_State_Redesign.md`
  - `/Users/ryanrentfro/code/markamp/docs/v27_docs/Phase_17__Theme_Parity_Token_Normalization_And_Cross_Surface_Correctness.md`
  - `/Users/ryanrentfro/code/markamp/docs/v27_docs/Phase_18__Accessibility_Visuals_Contrast_Focus_And_Reduced_Motion_Redesign.md`
  - `/Users/ryanrentfro/code/markamp/docs/v27_docs/Phase_19__Motion_Micro_Interactions_And_Icon_Driven_Control_Redesign.md`
  - `/Users/ryanrentfro/code/markamp/docs/v27_docs/Phase_20__Full_System_Cleanup_Legacy_Asset_Removal_And_Visual_QA_Gates.md`

## Recommended Execution Start

Begin with:

1. Phase 01
2. Phase 02
3. Phase 03
4. Phase 04
5. Phase 05

That sequence establishes the canonical icon system, the redesigned visual language, the migration/validation spine, and the new shell/action-surface model that the rest of the visual overhaul depends on.
