# Phase 05 - Menus Command Surfaces Toolbars And Action Cluster Redesign

## Goal

Redesign every top-level action surface around the new icon system and visual language so command discovery and execution feel premium, precise, and cohesive.

## Definition Of Done

- toolbars, menus, command palette, context menus, and overflow surfaces look redesigned
- icon-text rhythm and grouping are coherent across all action surfaces
- action states feel richer, clearer, and more consistent than the current UI

## Tasks

### Phase ID: V27-P05
### Task ID: V27-P05-T01
### Task Title: Redesign The Main Toolbar And High Frequency Action Clusters Around Canonical Iconography
- Priority: P0
- Category: Toolbar UI
- Objective: transform the toolbar from a strong custom surface into a fully redesigned premium command strip.
- Why This Matters Now: The toolbar is one of the most visible icon-bearing surfaces and will anchor the new icon-driven identity.
- Visual Gap Statement: the current toolbar is customized and capable, but still inherits local spacing, grouping, and icon behavior patterns from earlier generations.
- User Experience Impact: A redesigned toolbar materially improves polish, discoverability, and day-to-day confidence.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/ThemedButton.h`, `/Users/ryanrentfro/code/markamp/src/core/IconMetricsPolicy.h`
- Prior Plan References: `v22` Phase 04, `v26` Phase 03 and Phase 16
- Scope: toolbar group architecture, icon-only and icon-plus-text actions, cluster rhythm, overflow behavior, action emphasis, visual hierarchy
- Out of Scope: adding or removing commands for functional reasons
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/ThemedButton.h`, `/Users/ryanrentfro/code/markamp/src/core/IconMetricsPolicy.h`
- Related Screens / Components / Surfaces: main toolbar, notebook toolbar if patterns are shared, shell action rows
- Current Visual Behavior / Appearance: the toolbar is polished but still reads as evolved custom chrome rather than a ground-up redesign.
- Intended Visual Behavior / Appearance: the toolbar feels newly designed, with superior icon rhythm, grouping, and interaction quality.
- Icon / Visual Design Direction: premium command strip using canonical icons, stronger grouping, and more disciplined negative space.
- Technical Styling Approach: redesign toolbar control families around canonical icon metrics and the new action-surface language.
- Implementation Steps:
  1. Redesign toolbar group architecture and emphasis levels.
  2. Replace all icons with canonical mappings.
  3. Rebuild spacing, hover, active, and overflow behavior around the new visual system.
  4. Validate compact and wide shell states.
- Validation Steps:
  1. Compare before/after toolbar screenshots.
  2. Interact with common actions repeatedly.
  3. Confirm icon sizing, alignment, and cluster hierarchy hold across themes.
- Acceptance Criteria: the toolbar reads as a clearly redesigned flagship action surface, not an incremental polish of the old one.
- Dependencies: Phase 01 through Phase 03
- Parallelization Notes: can proceed alongside menu and command palette redesign.
- Risks / Failure Modes: over-designing the toolbar can make it visually heavy or reduce discoverability.
- Accessibility / Readability Notes: icon-only actions require strong tooltip and focus behavior.
- Theme / Styling Notes: toolbar state treatment must align with the redesigned shell and control language.
- Motion / Interaction Notes where relevant: hover and press feel should later reinforce the new action hierarchy.
- Cleanup / Consolidation Notes where relevant: remove legacy icon-name assumptions and local group-spacing hacks.
- Rollback / Safety Notes: preserve established action availability and shortcuts.
- References / Context: toolbar redesign is one of the fastest ways to make the product feel “new.”
- Example scenarios where useful: save, undo, view mode, and theme actions should feel optically balanced and semantically consistent in one row.

### Phase ID: V27-P05
### Task ID: V27-P05-T02
### Task Title: Redesign Menus Context Menus And Overflow Surfaces With Better Icon And Accelerator Presentation
- Priority: P0
- Category: Menu UI
- Objective: redesign all menu-like surfaces so they feel more premium, precise, and consistent with the new icon system.
- Why This Matters Now: Menus expose icon, spacing, and typography inconsistency immediately in a desktop app.
- Visual Gap Statement: menu-like surfaces currently risk inheriting different spacing, icon treatment, and emphasis patterns depending on implementation path.
- User Experience Impact: Better menus improve discoverability, scanning speed, and overall product sophistication.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/ToolWindowTabControl.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/PanelHeaderIcons.cpp`
- Prior Plan References: `v22` Phase 04, `v26` Phase 03
- Scope: menu rows, accelerators, submenu affordances, overflow menus, context actions, menu icon column, separators
- Out of Scope: command routing logic
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/ToolWindowTabControl.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/PanelHeaderIcons.cpp`
- Related Screens / Components / Surfaces: main menus, context menus, tab menus, panel overflow menus, toolbar overflow
- Current Visual Behavior / Appearance: icon naming is more modern in some menu surfaces, but visual execution is not yet a fully redesigned system.
- Intended Visual Behavior / Appearance: all menu-like surfaces feel precise, premium, and iconographically consistent.
- Icon / Visual Design Direction: refined desktop menu system with disciplined icon columns and elegant accelerator treatment.
- Technical Styling Approach: redesign the menu row model around canonical icon semantics, shared metrics, and the new typography/spacing language.
- Implementation Steps:
  1. Replace any local or legacy icon references with canonical mappings.
  2. Redesign menu row geometry, icon column, text hierarchy, and accelerators.
  3. Align submenu, checked, disabled, and destructive states.
  4. Validate tab, panel, and shell menu variants.
- Validation Steps:
  1. Open multiple menu classes side by side.
  2. Compare icon alignment and accelerator readability.
  3. Verify hover, selected, checked, and disabled states in both themes.
- Acceptance Criteria: menu-like surfaces feel like one newly designed family and no longer expose mixed icon or row-language assumptions.
- Dependencies: Phase 01 through Phase 03
- Parallelization Notes: can run with toolbar redesign.
- Risks / Failure Modes: excessive iconization or decoration can slow scanning.
- Accessibility / Readability Notes: accelerators and checked states must remain obvious and keyboard-friendly.
- Theme / Styling Notes: menu backgrounds and separators must reflect the redesigned overlay system.
- Motion / Interaction Notes where relevant: entry and row-hover transitions later should be subtle and consistent.
- Cleanup / Consolidation Notes where relevant: retire local per-surface menu row variants where the shared family is sufficient.
- Rollback / Safety Notes: preserve platform-appropriate behavior and command semantics.
- References / Context: menus are one of the clearest “pro app” quality signals.
- Example scenarios where useful: a tab context menu and a panel overflow menu should feel clearly related despite different content.

### Phase ID: V27-P05
### Task ID: V27-P05-T03
### Task Title: Redesign The Command Palette As The Flagship Transient Command Surface
- Priority: P0
- Category: Menu UI
- Objective: rebuild the command palette so it becomes the most premium transient command surface in the application.
- Why This Matters Now: The command palette is one of the most comparison-prone surfaces against VS Code and JetBrains.
- Visual Gap Statement: the current command palette is functional and partly refined, but still carries local styling and unfinished empty-state polish history.
- User Experience Impact: A redesigned command palette improves discovery, speed, and premium feel across the whole app.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/IconProvider.h`, `/Users/ryanrentfro/code/markamp/src/ui/FileIconResolver.h`
- Prior Plan References: `v22` command palette work, `v26` Phase 03
- Scope: shell, search field, result rows, category labels, file/icon presentation, empty states, keyboard-highlight treatment
- Out of Scope: ranking and fuzzy-search logic changes
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/IconProvider.h`, `/Users/ryanrentfro/code/markamp/src/ui/FileIconResolver.h`
- Related Screens / Components / Surfaces: command palette, quick-open style pickers, command search overlays
- Current Visual Behavior / Appearance: command palette quality is good enough to use, but not yet a flagship “best in class” redesign.
- Intended Visual Behavior / Appearance: the command palette feels unmistakably elite, with redesigned row rhythm, canonical icons, and elegant empty/no-result states.
- Icon / Visual Design Direction: floating command lens with strong icon semantics, excellent hierarchy, and premium keyboard focus clarity.
- Technical Styling Approach: redesign the palette around canonical icon mapping and the new overlay/action-surface system, removing emoji or glyph fallback dependencies entirely.
- Implementation Steps:
  1. Replace command and file icon fallbacks with canonical semantic and specialty icon routes.
  2. Redesign the palette frame, search row, and results list.
  3. Rework empty states, metadata, and keyboard highlight visuals.
  4. Validate with command, file, and no-results modes.
- Validation Steps:
  1. Run short, long, and no-match queries.
  2. Compare command rows versus file rows.
  3. Verify the palette feels premium in both themes and with keyboard-only navigation.
- Acceptance Criteria: the command palette feels like a new flagship transient surface and does not depend on legacy icon fallback paths.
- Dependencies: Phase 01 through Phase 03, Phase 02
- Parallelization Notes: can proceed while shell and menu redesign are underway.
- Risks / Failure Modes: over-styling could reduce speed or introduce visual noise.
- Accessibility / Readability Notes: focus, selection, and no-results states must be highly readable.
- Theme / Styling Notes: the palette should preview the redesigned overlay language and icon semantics.
- Motion / Interaction Notes where relevant: later transitions should reinforce instant responsiveness.
- Cleanup / Consolidation Notes where relevant: retire `IconProvider` and `FileIconResolver` fallback ownership from the palette path.
- Rollback / Safety Notes: keep interaction speed as the non-negotiable constraint.
- References / Context: the palette is a direct benchmark surface against the best IDEs.
- Example scenarios where useful: quick-open and command search should both feel clearly redesigned, not just recolored.
