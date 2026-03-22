# Phase 13: Settings Preferences Keybindings And Theme Surface Presentation

## Outcome

Make settings and preference surfaces feel premium, highly scannable, and visually trustworthy, including keybindings, theme editing, compare views, and advanced configuration surfaces.

## Improvement Count

360 atomic improvements across 6 execution tasks.

### P13-T01

- Phase ID: P13
- Task ID: P13-T01
- Task Title: Redesign settings information architecture presentation and category navigation UI
- Priority: P0
- Category: Settings UI
- Atomic Improvements Covered: 60
- Objective: Turn settings into a high-quality preference environment rather than a utilitarian control dump.
- Why This Matters Now: Settings is one of the strongest trust surfaces in the product and currently reflects local widget assembly more than premium product design.
- UI Problem Statement: [SettingsPanel.cpp](/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp) uses native notebooks, a tree with emoji prefixes, and mixed local typography and spacing.
- User Experience Impact: Users can use settings, but the surface does not yet feel elegant or world-class.
- Scope: Category sidebar, scope tabs, search field, page title hierarchy, category grouping, top-level layout rhythm, information scent.
- Out of Scope: Settings architecture or persistence semantics from prior planning waves.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/SettingsDialog.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/SettingsNavModel.cpp`; `/Users/ryanrentfro/code/markamp/src/core/SettingsCatalog.cpp`
- Related Screens / Components / Surfaces: Settings dialog, settings panel, category tree, scope tabs.
- Current UI Behavior / Appearance: Settings layout is useful but visually mixed and not yet premium.
- Intended UI Behavior / Appearance: Settings feel editorially organized, easy to scan, and worthy of a flagship IDE.
- Visual / Interaction Design Direction: Preference workspace with calm hierarchy, strong discoverability, and low visual clutter.
- Technical Styling Approach: Create dedicated settings layout, navigation, and row tokens layered on the general panel and form-control systems.
- Implementation Steps: Replace emoji-led visual taxonomy; redesign category sidebar; refine scope navigation; improve top-level spacing and titles; align settings shell with dialog standards.
- Validation Steps: Review settings in user/workspace/project scopes with many categories and active search/filter states.
- Acceptance Criteria: Settings navigation feels premium and immediately understandable.
- Dependencies: Phase 01, Phase 03, Phase 14.
- Risks / Failure Modes: Over-designing navigation can reduce density and slow expert scanning.
- Accessibility / Readability Notes: Category focus, active scope, and current section cues must remain unmistakable.
- Theme / Styling Notes: Settings navigation needs excellent dark/light parity because users often stay in it for long sessions.
- Motion / Interaction Notes where relevant: Section switching and category focus should feel crisp and low-noise.
- Observability / Diagnostics Notes where relevant: Add settings-shell screenshots for category and scope states.
- Rollback / Safety Notes: Preserve current setting IDs and routing while redesigning presentation.
- References / Context: Settings should feel like a premium environment, not an afterthought control panel.
- Example scenarios where useful: Searching settings and switching scopes still feels calm and highly legible.

### P13-T02

- Phase ID: P13
- Task ID: P13-T02
- Task Title: Rebuild settings rows, descriptions, toggles, inputs, and grouped controls for better scannability
- Priority: P0
- Category: Settings UI
- Atomic Improvements Covered: 60
- Objective: Make individual setting entries more readable, more consistent, and visually more refined.
- Why This Matters Now: The quality of a settings system is largely decided row by row.
- UI Problem Statement: Setting rows in [SettingsPanel.cpp](/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp) still reflect mixed native control styling and inconsistent row composition.
- User Experience Impact: Users work harder than necessary to parse settings labels, descriptions, and associated controls.
- Scope: Setting rows, labels, help text, defaults, advanced hints, inline validation, grouped settings, section separators, modified-state tags.
- Out of Scope: Persistence and apply logic.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/SettingsControlModel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/ThemedCheckbox.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/ThemedDropdown.cpp`
- Related Screens / Components / Surfaces: Setting rows, grouped controls, form elements.
- Current UI Behavior / Appearance: Controls are present, but row rhythm and semantic grouping are not yet premium.
- Intended UI Behavior / Appearance: Every setting row reads clearly and gracefully, even in dense categories.
- Visual / Interaction Design Direction: Clean preference rows with strong label/help/control hierarchy and minimal noise.
- Technical Styling Approach: Define settings-row variants and map form controls to shared themed-control primitives.
- Implementation Steps: Audit setting row patterns; redesign label/help/control balance; improve form-control alignment; refine grouped rows and modified-state chips; normalize warning and restart cues.
- Validation Steps: Review categories with many toggles, dropdowns, text fields, and advanced descriptions.
- Acceptance Criteria: Settings rows become easier to scan and more visually coherent across categories.
- Dependencies: Phase 01, Phase 17.
- Risks / Failure Modes: Excessive spacing can reduce density for expert users with many settings to scan.
- Accessibility / Readability Notes: Descriptions and labels must remain legible and controls clearly associated with their text.
- Theme / Styling Notes: Form-control states must match the broader control-state language and stay readable in all themes.
- Motion / Interaction Notes where relevant: Validation and modified-state feedback should be subtle but clear.
- Observability / Diagnostics Notes where relevant: Add row-family screenshots and control-state matrices for settings.
- Rollback / Safety Notes: Preserve current settings functionality while moving rows to a more refined component system.
- References / Context: Settings quality often lives or dies on row layout and control rhythm.
- Example scenarios where useful: A long list of editor settings remains easy to skim without feeling sparse or cramped.

### P13-T03

- Phase ID: P13
- Task ID: P13-T03
- Task Title: Improve keybinding editor, theme editor, compare view, and advanced settings sub-surfaces
- Priority: P1
- Category: Settings UI
- Atomic Improvements Covered: 60
- Objective: Make secondary preference tools feel as premium as the main settings surface.
- Why This Matters Now: Advanced settings tools are high-signal surfaces for expert users and currently risk feeling utilitarian.
- UI Problem Statement: Files such as [ShortcutEditor.cpp](/Users/ryanrentfro/code/markamp/src/ui/ShortcutEditor.cpp), [ThemeEditorPanel.cpp](/Users/ryanrentfro/code/markamp/src/ui/ThemeEditorPanel.cpp), [ThemeTokenEditor.cpp](/Users/ryanrentfro/code/markamp/src/ui/ThemeTokenEditor.cpp), [SettingsCompareView.cpp](/Users/ryanrentfro/code/markamp/src/ui/SettingsCompareView.cpp), and [SettingsJsonEditor.cpp](/Users/ryanrentfro/code/markamp/src/ui/SettingsJsonEditor.cpp) need stronger UI family cohesion.
- User Experience Impact: Power-user tooling can feel visually less polished than the settings shell around it.
- Scope: Advanced editor layout, token grids, compare diff layout, JSON editor shell, keybinding rows, search/filter bars, action placement.
- Out of Scope: Advanced-settings semantics.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/ShortcutEditor.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/ThemeEditorPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/ThemeTokenEditor.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/SettingsCompareView.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/SettingsJsonEditor.cpp`
- Related Screens / Components / Surfaces: Keybinding editor, theme editor, compare view, JSON settings editor.
- Current UI Behavior / Appearance: These advanced surfaces are useful but do not yet obviously belong to one premium preference family.
- Intended UI Behavior / Appearance: Advanced preference tools feel high-end, specialized, and visually coherent.
- Visual / Interaction Design Direction: Expert tooling with crisp density and strong hierarchy rather than raw utility widgets.
- Technical Styling Approach: Extend settings shell tokens and data-dense view systems to advanced configuration tools.
- Implementation Steps: Audit advanced settings surfaces; align shells, headings, filters, and action bars; improve row/cell density and diff presentation; normalize editor framing and chips.
- Validation Steps: Review each advanced tool with realistic content and compare against the main settings shell.
- Acceptance Criteria: Advanced preference tools no longer feel like separate utility apps.
- Dependencies: Phase 13 row and shell work, Phase 15, Phase 18.
- Risks / Failure Modes: Over-normalization can remove useful domain-specific affordances in expert tools.
- Accessibility / Readability Notes: Dense token and keybinding grids must remain readable and focus-visible.
- Theme / Styling Notes: Theme editors and compare views especially need strong dark/light parity to be trustworthy.
- Motion / Interaction Notes where relevant: Diff focus and token preview changes should be calm and not visually jumpy.
- Observability / Diagnostics Notes where relevant: Add advanced-settings screenshot boards to the settings family gallery.
- Rollback / Safety Notes: Preserve tool functionality and keyboard-heavy workflows while redesigning shells.
- References / Context: Advanced configuration surfaces are where expert users judge product seriousness quickly.
- Example scenarios where useful: Editing theme tokens feels like using a premium design tool, not a raw property sheet.

### P13-T04

- Phase ID: P13
- Task ID: P13-T04
- Task Title: Redesign warnings, reset/revert/apply controls, and settings-state communication
- Priority: P1
- Category: Settings UI
- Atomic Improvements Covered: 60
- Objective: Make settings state transitions and warnings visually clear without adding clutter or anxiety.
- Why This Matters Now: Users need trustworthy visual cues about modified state, restart requirements, and destructive actions.
- UI Problem Statement: Settings already have significant apply/cancel complexity, but the visible communication layer needs more polish and coherence.
- User Experience Impact: Users can feel uncertain about what changed, what is pending, and what might reset or require restart.
- Scope: Modified chips, reset affordances, warning banners, restart-required hints, apply/cancel bars, unsaved indicators, revert messaging.
- Out of Scope: Backend apply logic.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/SettingsDialog.cpp`; `/Users/ryanrentfro/code/markamp/src/core/LiveSettingsReactor.cpp`
- Related Screens / Components / Surfaces: Settings rows, dialog footers, warning states, reset actions.
- Current UI Behavior / Appearance: Modified and warning states exist, but not yet with one premium visual language.
- Intended UI Behavior / Appearance: Users can see and trust settings state transitions at a glance.
- Visual / Interaction Design Direction: Clear, calm, non-alarmist state communication with strong action affordances.
- Technical Styling Approach: Define settings-state components for modified, restart, warning, reset, and footer action states.
- Implementation Steps: Audit state cues; redesign reset and warning surfaces; improve apply/cancel footer hierarchy; align destructive and restorative actions with the dialog system.
- Validation Steps: Trigger modified, reset, warning, and restart-required states in representative settings categories.
- Acceptance Criteria: Settings-state communication becomes more trustworthy and less visually awkward.
- Dependencies: Phase 13 shell and row work, Phase 14.
- Risks / Failure Modes: Too much warning UI can make the settings surface feel stressful.
- Accessibility / Readability Notes: State cues must remain understandable without color-only encoding.
- Theme / Styling Notes: Warning and restart cues need careful contrast tuning to stay visible but restrained.
- Motion / Interaction Notes where relevant: Modified-state transitions should be subtle and not flashy.
- Observability / Diagnostics Notes where relevant: Add warning/reset/apply screenshots to the settings board.
- Rollback / Safety Notes: Preserve existing state logic while message and footer UI is refined.
- References / Context: Trust in settings comes from both behavior and visible state communication.
- Example scenarios where useful: A modified setting with restart required and a Reset button still feels calm and easy to understand.

### P13-T05

- Phase ID: P13
- Task ID: P13-T05
- Task Title: Upgrade theme gallery, preview cards, and theme-management surfaces to flagship quality
- Priority: P1
- Category: Settings UI
- Atomic Improvements Covered: 60
- Objective: Make theme-management UIs look like premium product surfaces rather than support utilities.
- Why This Matters Now: Theme editing and browsing are central to UI identity and currently expose several local styling choices.
- UI Problem Statement: [ThemeGallery.cpp](/Users/ryanrentfro/code/markamp/src/ui/ThemeGallery.cpp), [ThemePreviewCard.cpp](/Users/ryanrentfro/code/markamp/src/ui/ThemePreviewCard.cpp), and related theme tools still use local fonts, uppercase subtitles, and basic button treatments.
- User Experience Impact: Theme customization feels less premium than the product’s ambition suggests.
- Scope: Gallery shell, header, preview-card layout, search/filter controls, import/export controls, card hover states, theme comparison visuals.
- Out of Scope: Theme import/export backend logic.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/ThemeGallery.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/ThemePreviewCard.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/ThemeGalleryModel.cpp`; `/Users/ryanrentfro/code/markamp/src/core/ThemeRegistry.cpp`
- Related Screens / Components / Surfaces: Theme gallery, theme previews, theme management workflows.
- Current UI Behavior / Appearance: Theme tools are useful, but the shell and cards are not yet flagship-caliber.
- Intended UI Behavior / Appearance: Theme management becomes one of the most beautiful settings-related surfaces in the product.
- Visual / Interaction Design Direction: Premium gallery with strong previews, clean typography, and elegant browsing controls.
- Technical Styling Approach: Reuse card, dialog, filter, and settings-shell systems while defining theme-preview-specific rendering rules.
- Implementation Steps: Audit gallery and preview visuals; retune shell hierarchy; redesign preview cards; align filters and actions; improve import/export affordance styling.
- Validation Steps: Review theme gallery with many cards, filtered results, and import/export workflows.
- Acceptance Criteria: Theme management looks like a flagship product capability.
- Dependencies: Phase 01, Phase 14, Phase 18.
- Risks / Failure Modes: Over-styling the gallery can make it feel heavier than the act of quick theme switching warrants.
- Accessibility / Readability Notes: Theme previews and labels must remain readable and not rely solely on miniature color swatches.
- Theme / Styling Notes: The gallery itself must look great under every theme, not only the default one.
- Motion / Interaction Notes where relevant: Card hover and selection transitions should feel premium but restrained.
- Observability / Diagnostics Notes where relevant: Add gallery and card-state screenshot boards.
- Rollback / Safety Notes: Preserve current theme-preview functionality while visual shells are upgraded.
- References / Context: Theme surfaces are brand statements for a UI-focused product.
- Example scenarios where useful: Browsing themes feels like using a high-quality marketplace/gallery surface rather than a plain file picker dialog.

### P13-T06

- Phase ID: P13
- Task ID: P13-T06
- Task Title: Add settings family galleries and preference-surface regression coverage
- Priority: P0
- Category: Cleanup / Consolidation
- Atomic Improvements Covered: 60
- Objective: Keep settings, advanced editors, and theme-management surfaces visually coherent over time.
- Why This Matters Now: Settings surfaces span many component families and are especially vulnerable to local visual drift.
- UI Problem Statement: The product needs one comparative board for settings shell, rows, advanced tools, warnings, and theme-management surfaces.
- User Experience Impact: Preference quality can erode quickly without visible governance.
- Scope: Settings shell, category nav, rows, warnings, advanced tools, theme gallery, keybindings, compare view, JSON editor.
- Out of Scope: Settings logic correctness.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/tests/unit`; `/Users/ryanrentfro/code/markamp/docs/v22_docs`; `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/ThemeGallery.cpp`
- Related Screens / Components / Surfaces: Entire preference family.
- Current UI Behavior / Appearance: Settings surfaces are reviewed individually rather than as one visual ecosystem.
- Intended UI Behavior / Appearance: Preference surfaces are referenceable, comparable, and regression-gated as a family.
- Visual / Interaction Design Direction: Mature preference-surface governance.
- Technical Styling Approach: Build grouped screenshot suites and family galleries for all major settings variants and advanced tools.
- Implementation Steps: Define fixtures; capture main and advanced preference states; publish boards; add visual regression checks for high-value preference surfaces.
- Validation Steps: Alter settings-row spacing or theme-gallery card styling and verify the family board catches the drift.
- Acceptance Criteria: Preference-surface polish is durable and easier to maintain.
- Dependencies: P13-T01 through P13-T05.
- Risks / Failure Modes: Too many fixtures can slow review if not grouped by user journey and tool family.
- Accessibility / Readability Notes: Include focus-visible and high-contrast preference states.
- Theme / Styling Notes: Capture every settings family in dark, light, and high-contrast modes.
- Motion / Interaction Notes where relevant: Include representative hover and dialog transition captures for preference tools.
- Observability / Diagnostics Notes where relevant: Store fixture metadata with category, scope, and tool family.
- Rollback / Safety Notes: Start with reference boards before strict CI promotion if needed.
- References / Context: Settings surfaces are directly tied to user trust and deserve first-class visual governance.
- Example scenarios where useful: A refactor that improves keybinding rows but weakens settings navigation balance is exposed immediately.

