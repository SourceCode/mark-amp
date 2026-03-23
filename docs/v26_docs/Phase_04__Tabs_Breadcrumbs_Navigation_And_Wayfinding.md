# Phase 04 - Tabs Breadcrumbs Navigation And Wayfinding

## Goal

Refine tabs, breadcrumbs, section switchers, and contextual navigation so users always know where they are and what is active without visual noise.

## Definition Of Done

- active, inactive, hover, dirty, pinned, and temporary navigation states are visually clear
- breadcrumbs and location indicators feel integrated with tabs and shell chrome
- navigation density supports fast scanning without clutter

## Tasks

### Phase ID: V26-P04
### Task ID: V26-P04-T01
### Task Title: Finish Tab Strip Density, State Hierarchy, And Close-Affordance Polish
- Priority: P0
- Category: Tab UI
- Objective: Bring the custom tab bar to a premium standard for density, hierarchy, and state clarity.
- Why This Matters Now: Tabs are one of the most frequently viewed surfaces in an IDE-style workspace.
- UI Gap Statement: `/Users/ryanrentfro/code/markamp/src/ui/TabBar.cpp` is custom and advanced, but still has local styling complexity and likely uneven state emphasis.
- User Experience Impact: Better tabs improve orientation, reduce misclicks, and elevate the perceived quality of document workflows.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/TabBar.cpp`, `/Users/ryanrentfro/code/markamp/src/core/ThemeTokens.h`
- Prior Plan References: `v22` tabs and navigation polish, `v25` shell UI closure
- Scope: tab padding, active/inactive/hover/dirty styling, close affordance, pinned/temporary differentiation if supported, overflow behavior
- Out of Scope: document lifecycle logic
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/TabBar.cpp`, `/Users/ryanrentfro/code/markamp/src/core/ThemeTokens.h`
- Related Screens / Components / Surfaces: document tabs, notebook tabs, canvas tabs if shared
- Current UI Behavior / Appearance: tabs are custom-rendered but still feel closer to well-made custom controls than to elite premium navigation.
- Intended UI Behavior / Appearance: the tab strip feels calm, crisp, and unmistakably intentional, with excellent state clarity and optical alignment.
- Visual / Interaction Design Direction: refined desktop navigation bar with subtle layering and controlled emphasis.
- Technical Styling Approach: normalize tab metrics and state paint against shared navigation tokens.
- Implementation Steps:
  1. Audit tab states and padding.
  2. Tune active versus inactive contrast and dirty indicators.
  3. Refine close-button reveal and hover language.
  4. Validate pinned/temporary variants if present.
- Validation Steps:
  1. Open many tabs and compare scanning speed.
  2. Hover active and inactive tabs.
  3. Verify close affordance clarity without visual noise.
- Acceptance Criteria: users can distinguish active, hovered, dirty, and background tabs instantly and the strip remains visually calm.
- Dependencies: V26-P01-T03, V26-P02-T02
- Parallelization Notes: can run with breadcrumb work.
- Risks / Failure Modes: too much contrast can make tabs noisy; too little can make them ambiguous.
- Accessibility / Readability Notes: dirty and selection states must not rely on color alone.
- Theme / Styling Notes: active/background balance must be tuned per theme without changing the underlying state rules.
- Motion / Interaction Notes where relevant: any close-button reveal or hover animation should stay subtle.
- Cleanup / Consolidation Notes where relevant: reduce tab-specific local paint constants in favor of shared state metrics.
- Rollback / Safety Notes: preserve tab hit targets and close reliability while tuning visuals.
- References / Context: tabs should align visually with toolbar and breadcrumb polish.
- Example scenarios where useful: a crowded tab strip should remain readable without exaggerated separators.

### Phase ID: V26-P04
### Task ID: V26-P04-T02
### Task Title: Replace Remaining Breadcrumb And Location Hardcodes With Premium Navigation Styling
- Priority: P1
- Category: Navigation UI
- Objective: Make breadcrumbs and contextual location indicators feel fully integrated with the modern shell.
- Why This Matters Now: Wayfinding quality separates a feature-rich app from a polished workspace.
- UI Gap Statement: `/Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp` still contains hardcoded visual choices, which suggests incomplete navigation convergence.
- User Experience Impact: Clear, elegant breadcrumbs reduce cognitive load and make complex workspaces feel navigable.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/ExplorerPanel.cpp`
- Prior Plan References: `v22` navigation polish, `v25` visible UI closure
- Scope: breadcrumb chip spacing, separators, hover/focus states, truncation, location metadata styling
- Out of Scope: routing or file-navigation logic
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/ExplorerPanel.cpp`
- Related Screens / Components / Surfaces: breadcrumb bar, location headers, contextual navigation strips
- Current UI Behavior / Appearance: breadcrumb styling is partly custom but not fully aligned with shared token usage.
- Intended UI Behavior / Appearance: breadcrumbs are crisp, theme-correct, readable, and visually related to tabs and panel headers.
- Visual / Interaction Design Direction: subtle path navigation with clean truncation and strong location hierarchy.
- Technical Styling Approach: migrate breadcrumb visuals to shared token and state helpers.
- Implementation Steps:
  1. Remove hardcoded paint values.
  2. Standardize breadcrumb padding and separators.
  3. Refine hover, focus, and truncation behavior.
  4. Validate with long nested paths.
- Validation Steps:
  1. Test short and long breadcrumb chains.
  2. Compare against tab and panel-header styling.
  3. Validate light/dark appearance and focus visibility.
- Acceptance Criteria: breadcrumb visuals are fully token-driven, visually coherent, and readable in deep paths.
- Dependencies: V26-P01-T03
- Parallelization Notes: can run alongside tab refinement.
- Risks / Failure Modes: excessive breadcrumb decoration can make navigation noisy.
- Accessibility / Readability Notes: deep paths need clear truncation and keyboard focus treatment.
- Theme / Styling Notes: breadcrumb separators must stay low-noise in both themes.
- Motion / Interaction Notes where relevant: hover feedback should be immediate and restrained.
- Cleanup / Consolidation Notes where relevant: remove local color/font logic unique to breadcrumb paint.
- Rollback / Safety Notes: preserve navigation hit targets and text clarity.
- References / Context: this is a cleanup-heavy navigation polish task.
- Example scenarios where useful: a deeply nested note path should still feel elegant in the breadcrumb bar.

### Phase ID: V26-P04
### Task ID: V26-P04-T03
### Task Title: Harmonize Navigation Controls, Section Switchers, And Context Indicators
- Priority: P1
- Category: Navigation UI
- Objective: Standardize the visual treatment of back/forward controls, section switchers, and context-location cues across the shell.
- Why This Matters Now: Navigation quality suffers when adjacent controls use unrelated density or emphasis patterns.
- UI Gap Statement: Navigation is distributed across tabs, breadcrumbs, shell buttons, and panel headers, which makes visual drift likely.
- User Experience Impact: Consistent navigation controls improve orientation and speed while making the shell feel more coherent.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/ActivityBar.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`
- Prior Plan References: `v22` navigation and shell work, `v25` shell UI closure
- Scope: navigation button shape, section-switcher styling, context labels, compact metadata placement
- Out of Scope: navigation command implementation
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/ActivityBar.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`
- Related Screens / Components / Surfaces: toolbar navigation buttons, activity/section switchers, contextual labels
- Current UI Behavior / Appearance: controls exist but may not yet share a single navigation-specific visual grammar.
- Intended UI Behavior / Appearance: all navigation controls feel related, balanced, and intentionally placed.
- Visual / Interaction Design Direction: quiet utility controls with premium clarity and strong alignment.
- Technical Styling Approach: define a shared navigation-control role and map relevant controls to it.
- Implementation Steps:
  1. Catalog navigation-related controls and their current styling.
  2. Normalize size, icon treatment, and active/hover states.
  3. Align context indicators with breadcrumb and tab hierarchy.
  4. Validate narrow-width and high-density shells.
- Validation Steps:
  1. Compare all navigation controls in one pass.
  2. Verify location metadata remains readable without crowding.
  3. Test with mouse and keyboard focus.
- Acceptance Criteria: navigation controls and context indicators share one coherent visual language.
- Dependencies: V26-P03-T01, V26-P04-T01
- Parallelization Notes: can run after toolbar and tabs establish final control metrics.
- Risks / Failure Modes: too much normalization can erase useful contextual differences.
- Accessibility / Readability Notes: active location and keyboard focus must remain obvious.
- Theme / Styling Notes: icon contrast and metadata hierarchy must remain theme-safe.
- Motion / Interaction Notes where relevant: consistent hover/press feel should match toolbar standards.
- Cleanup / Consolidation Notes where relevant: consolidate duplicated navigation-control styles.
- Rollback / Safety Notes: keep semantic cues clear while standardizing appearance.
- References / Context: this is the final wayfinding coherence task for shell navigation.
- Example scenarios where useful: moving between panels, tabs, and file paths should feel like one navigation system.
