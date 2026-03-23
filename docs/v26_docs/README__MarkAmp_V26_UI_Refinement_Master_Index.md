# MarkAmp V26 UI Refinement Master Index

## Purpose Of The V26 Pass

`v26` is the dedicated UI-only refinement and completion pass for MarkAmp. It converts the broad UI discovery from `v22` and the release-path UI closure from `v25` into the next implementation-ready wave for elite visual quality, tighter consistency, and unmistakably premium interaction presentation.

This pass is specifically about:

- visible interface quality
- layout rhythm and spacing refinement
- typography and hierarchy refinement
- control and chrome polish
- editor, notebook, canvas, panel, settings, menu, and overlay presentation
- syntax-highlighting presentation
- theme parity and token correctness
- iconography, motion, and micro-interactions
- empty, loading, error, onboarding, and feedback-state refinement
- visual cleanup and consolidation

This pass is not for backend completion, architecture cleanup, or general release-hardening except where those directly affect visible UI quality.

## What Changed From Prior Planning

### From V22

`v22` was the broad UI-only expansion pass. It covered the right terrain, but it was discovery-heavy and intentionally expansive. `v26` retires that breadth-first approach and replaces it with a tighter sequence built around convergence, parity, premium finish, and final visible cleanup.

### From V23 Through V24

Intermediate planning generations expanded subsystem completion and release-path work. UI items from those generations are merged here only where visible presentation still materially affects perceived product quality.

### From V25

`v25` handled release-readiness and UI only where it intersected with trustworthiness. `v26` reopens UI as its own track and focuses on premium finish rather than workflow hardening.

## What Was Merged, Retired, De-Scoped, Or Re-Sequenced

### Merged

- earlier shell polish work is merged into shell-convergence and layout-rhythm phases
- editor chrome and syntax presentation work is split into separate premium-readability phases
- settings, theme gallery, command palette, dialogs, overlays, and feedback work are clustered as related surface systems
- iconography, motion, accessibility-visuals, and cleanup are promoted into explicit completion phases

### Retired

- first-pass design-token creation work
- first-pass shell chrome creation work
- broad “inventory all UI issues” tasks from `v22`

### De-Scoped

- backend or architectural changes that do not materially improve visible UI
- feature completion work not required for visual completeness

### Re-Sequenced

- design-system adoption now comes before surface-level polish
- theme parity is pulled later in the sequence so it validates stabilized visual patterns
- motion, accessibility visuals, and micro-interactions are placed after structural polish so they reinforce a finished visual system
- cleanup and consolidation close the wave

## Highest-Priority Next UI Workstreams

1. Design-system convergence and premium standards adoption.
2. Shell, layout, toolbar, tab, and panel rhythm normalization.
3. Editor and syntax presentation tuning.
4. Notebook and canvas shell parity with the rest of the product.
5. Settings, startup, command palette, dialogs, feedback, and empty-state refinement.
6. Theme parity, iconography normalization, motion, accessibility visuals, and final cleanup.

## Critical Path Dependencies

1. Phase 01 defines visual standards and token adoption targets needed by all later work.
2. Phase 02 and Phase 03 stabilize shell rhythm and action-surface patterns used by tabs, panels, settings, and overlays.
3. Phase 06 and Phase 07 establish editor and syntax treatment that Phase 08 and Phase 15 depend on for notebook and theme parity work.
4. Phase 10 through Phase 13 depend on shared control, spacing, and feedback conventions from Phase 01 through Phase 05.
5. Phase 15 depends on most surface-level polish being complete so parity validation is meaningful.
6. Phase 17 through Phase 20 depend on stabilized visual language and should not run first.

## Parallelizable Workstreams

- Phase 04 and Phase 05 can run in parallel after Phase 02 and Phase 03 start.
- Phase 08 and Phase 09 can run in parallel once Phase 01, Phase 02, and Phase 06 have established shared chrome standards.
- Phase 10, Phase 11, and Phase 12 can run in parallel after shared control styling is stabilized.
- Phase 16 and Phase 18 can run in parallel across completed surfaces.
- Phase 17 and Phase 19 can overlap once the relevant surfaces are visually stable.

## Highest-Risk Visual Consistency Gaps

- local one-off color, font, and spacing decisions in `/Users/ryanrentfro/code/markamp/src/ui/ThemeGallery.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/SearchSidebarPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp`, and `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`
- placeholder-looking iconography and copy in `/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/ProblemsPanel.cpp`, and `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp`
- thin or stub-looking visible surfaces in `/Users/ryanrentfro/code/markamp/src/ui/PDFViewerPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/HistoryPanel.cpp`, and `/Users/ryanrentfro/code/markamp/src/ui/ThemedDialog.cpp`

## Highest-Risk Theme / Styling Gaps

- inconsistent token adoption across shell versus interior surfaces
- incomplete light/dark parity on settings, galleries, overlays, and structured panels
- syntax color hierarchy that is functional but not yet premium or consistently balanced across themes

## Highest-Risk Editor / Notebook / Canvas UI Gaps

- editor readability polish, gutter/selection/caret/minimap consistency, and diagnostics/search-result presentation
- notebook cell chrome, output block styling, and mixed-content density balance
- canvas tool palette, overlays, selection handles, guides, zoom controls, and onboarding quality

## Highest-Risk Settings / Panel / Menu Presentation Gaps

- settings rows, control hierarchy, and category presentation remain inconsistent
- panel headers, tree/list/table density, and empty states are uneven
- command palette, menus, popovers, and notifications do not yet fully share the same premium transient-surface language

## Overall Acceptance Criteria For V26

- all primary UI surfaces use consistent spacing, typography, radius, divider, icon, and interaction-state language
- no visible placeholder, stub, emoji, unicode-stand-in, or obviously temporary visual treatment remains on the main UX path
- editor, notebook, canvas, shell, settings, panels, dialogs, and feedback surfaces feel like one cohesive premium product
- theme parity is materially reliable across light and dark themes
- focus, contrast, motion, and reduced-motion behavior are deliberate and consistent
- visual tech debt that blocks premium quality is retired or consolidated

## Phase Overview

| Phase | Title | Task Count | Summary |
| --- | --- | ---: | --- |
| 01 | Visual System Convergence And Premium Standards | 3 | Normalize tokens, spacing, typography, radii, states, and adoption rules. |
| 02 | Shell Frame Layout Rhythm And Surface Layering | 3 | Refine window composition, panel balance, split views, and overall shell depth. |
| 03 | Menus Toolbars Command Palette And Action Surfaces | 3 | Polish top-level actions, command surfaces, grouping, spacing, and hover/press feel. |
| 04 | Tabs Breadcrumbs Navigation And Wayfinding | 3 | Finish tabs, breadcrumbs, navigation rhythm, and location/context clarity. |
| 05 | Panels Inspectors And Dense Information Chrome | 3 | Standardize panel headers, explorers, inspectors, and dense shell surfaces. |
| 06 | Editor Chrome Gutter Selection Caret And Minimap | 3 | Tune the core authoring surface for elite readability and precision. |
| 07 | Syntax Highlighting Presentation And Code Readability | 3 | Improve token mapping, contrast, and language hierarchy across themes and contexts. |
| 08 | Notebook Shell Cell Chrome Output And Mixed Content | 3 | Bring notebook visuals to premium shell-quality parity. |
| 09 | Canvas Shell Tools Overlays And Interaction Visuals | 3 | Upgrade canvas presentation, controls, overlays, and object affordances. |
| 10 | Settings Preferences Theme Gallery And Control Styling | 3 | Refine settings, preference rows, theme gallery, and high-density control surfaces. |
| 11 | Dialogs Popovers Tooltips Notifications And Overlays | 3 | Create a cohesive transient-surface language. |
| 12 | Trees Lists Tables AV And Structured Data Presentation | 3 | Improve scannability and cohesion for structured information surfaces. |
| 13 | Feedback Status Progress And Inline Validation UI | 3 | Unify status, progress, alerts, banners, and inline validation visuals. |
| 14 | Startup Welcome Onboarding And Empty State Elegance | 3 | Make first-run and empty states premium, restrained, and informative. |
| 15 | Theme Parity Token Adoption And Cross Surface Correctness | 3 | Validate and fix cross-theme visual correctness across all major surfaces. |
| 16 | Iconography Optical Alignment And Semantic Completion | 3 | Normalize icon system usage and eliminate stand-ins. |
| 17 | Motion Transitions And Responsiveness Feel | 3 | Add restrained motion and interaction continuity. |
| 18 | Accessibility Visual Contrast Focus And Reduced Motion | 3 | Improve visible accessibility without diluting the visual language. |
| 19 | Micro Interactions Drag Resize Focus Transfer And Tactile Polish | 3 | Finish the tactile feel of the application. |
| 20 | Visual Cleanup Consolidation Legacy Style Removal And UI QA Gates | 3 | Retire visual debt, unify remaining variants, and gate completion. |

## Total Task Counts

- Total phases: 20
- Total tasks: 60
- Tasks per phase: 3

## Documents In This Package

- Assessment:
  - `/Users/ryanrentfro/code/markamp/docs/v26_docs/ASSESSMENT__MarkAmp_V26_UI_Refinement_And_Premium_Completion_Assessment.md`
- Phase files:
  - `/Users/ryanrentfro/code/markamp/docs/v26_docs/Phase_01__Visual_System_Convergence_And_Premium_Standards.md`
  - `/Users/ryanrentfro/code/markamp/docs/v26_docs/Phase_02__Shell_Frame_Layout_Rhythm_And_Surface_Layering.md`
  - `/Users/ryanrentfro/code/markamp/docs/v26_docs/Phase_03__Menus_Toolbars_Command_Palette_And_Action_Surfaces.md`
  - `/Users/ryanrentfro/code/markamp/docs/v26_docs/Phase_04__Tabs_Breadcrumbs_Navigation_And_Wayfinding.md`
  - `/Users/ryanrentfro/code/markamp/docs/v26_docs/Phase_05__Panels_Inspectors_And_Dense_Information_Chrome.md`
  - `/Users/ryanrentfro/code/markamp/docs/v26_docs/Phase_06__Editor_Chrome_Gutter_Selection_Caret_And_Minimap.md`
  - `/Users/ryanrentfro/code/markamp/docs/v26_docs/Phase_07__Syntax_Highlighting_Presentation_And_Code_Readability.md`
  - `/Users/ryanrentfro/code/markamp/docs/v26_docs/Phase_08__Notebook_Shell_Cell_Chrome_Output_And_Mixed_Content.md`
  - `/Users/ryanrentfro/code/markamp/docs/v26_docs/Phase_09__Canvas_Shell_Tools_Overlays_And_Interaction_Visuals.md`
  - `/Users/ryanrentfro/code/markamp/docs/v26_docs/Phase_10__Settings_Preferences_Theme_Gallery_And_Control_Styling.md`
  - `/Users/ryanrentfro/code/markamp/docs/v26_docs/Phase_11__Dialogs_Popovers_Tooltips_Notifications_And_Overlays.md`
  - `/Users/ryanrentfro/code/markamp/docs/v26_docs/Phase_12__Trees_Lists_Tables_AV_And_Structured_Data_Presentation.md`
  - `/Users/ryanrentfro/code/markamp/docs/v26_docs/Phase_13__Feedback_Status_Progress_And_Inline_Validation_UI.md`
  - `/Users/ryanrentfro/code/markamp/docs/v26_docs/Phase_14__Startup_Welcome_Onboarding_And_Empty_State_Elegance.md`
  - `/Users/ryanrentfro/code/markamp/docs/v26_docs/Phase_15__Theme_Parity_Token_Adoption_And_Cross_Surface_Correctness.md`
  - `/Users/ryanrentfro/code/markamp/docs/v26_docs/Phase_16__Iconography_Optical_Alignment_And_Semantic_Completion.md`
  - `/Users/ryanrentfro/code/markamp/docs/v26_docs/Phase_17__Motion_Transitions_And_Responsiveness_Feel.md`
  - `/Users/ryanrentfro/code/markamp/docs/v26_docs/Phase_18__Accessibility_Visual_Contrast_Focus_And_Reduced_Motion.md`
  - `/Users/ryanrentfro/code/markamp/docs/v26_docs/Phase_19__Micro_Interactions_Drag_Resize_Focus_Transfer_And_Tactile_Polish.md`
  - `/Users/ryanrentfro/code/markamp/docs/v26_docs/Phase_20__Visual_Cleanup_Consolidation_Legacy_Style_Removal_And_UI_QA_Gates.md`

## Recommended Execution Start

Begin with:

1. Phase 01
2. Phase 02
3. Phase 03
4. Phase 06
5. Phase 10

That sequence establishes the visual system, shell rhythm, action-surface standards, editor readability baseline, and settings/control language needed for the rest of the package.
