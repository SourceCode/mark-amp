# Phase 05: Tabs Breadcrumbs Navigation And Workspace Wayfinding

## Outcome

Make tabs, breadcrumbs, navigation bars, switchers, and wayfinding surfaces feel clear, elegant, and visually premium across editor, notebook, canvas, and panel contexts.

## Improvement Count

360 atomic improvements across 6 execution tasks.

### P05-T01

- Phase ID: P05
- Task ID: P05-T01
- Task Title: Rebuild tab-strip visual hierarchy, active states, and density rhythm
- Priority: P0
- Category: Tab UI
- Atomic Improvements Covered: 60
- Objective: Make tabs feel precise, premium, and easier to scan in both sparse and crowded workspaces.
- Why This Matters Now: Tabs are the primary document-navigation surface and one of the strongest perceived-quality signals in an IDE.
- UI Problem Statement: [TabBar.cpp](/Users/ryanrentfro/code/markamp/src/ui/TabBar.cpp) already custom-renders tabs with fade animation and multiple states, but the visual finish still needs stronger hierarchy and consistency.
- User Experience Impact: Document navigation feels more serviceable than elite.
- Scope: Active/inactive/hover states, close button treatment, dirty indicators, pinned-state visuals, preview tab visuals, tab grouping, truncation, drag states.
- Out of Scope: Tab persistence or behavior correctness.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/TabBar.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/TabBar.h`; `/Users/ryanrentfro/code/markamp/src/ui/TabStripModel.cpp`; `/Users/ryanrentfro/code/markamp/src/core/IconMetricsPolicy.cpp`
- Related Screens / Components / Surfaces: Document tabs, split-view tabs, preview tabs.
- Current UI Behavior / Appearance: Tabs are functional and animated, but visual hierarchy and state cues are not yet at a premium bar.
- Intended UI Behavior / Appearance: Tabs are calm, legible, and easy to parse even in dense sessions.
- Visual / Interaction Design Direction: Refined strip with crisp active-state emphasis and strong metadata restraint.
- Technical Styling Approach: Define tab-surface variants and migrate local state styling to shared tab tokens.
- Implementation Steps: Audit tab states; retune active-background and foreground contrast; refine close button behavior; improve dirty/pinned affordances; normalize preview-tab treatment.
- Validation Steps: Test tabs with long names, many open files, dirty states, pinned tabs, and split groups.
- Acceptance Criteria: Tab navigation feels materially more premium and more readable than the current strip.
- Dependencies: Phase 01, Phase 02.
- Risks / Failure Modes: Over-emphasized active tabs can make inactive tabs too weak to scan.
- Accessibility / Readability Notes: Active, dirty, focused, and selected tabs must be distinguishable without relying on color alone.
- Theme / Styling Notes: Tab layering and foreground contrast need careful dark/light calibration.
- Motion / Interaction Notes where relevant: Existing fade and pulse behavior should be refined into a calmer motion language.
- Observability / Diagnostics Notes where relevant: Add tab-strip state matrices to the visual QA suite.
- Rollback / Safety Notes: Keep current geometry stable while visual state tuning is validated.
- References / Context: [TabBar.cpp](/Users/ryanrentfro/code/markamp/src/ui/TabBar.cpp) already contains strong custom infrastructure worth elevating.
- Example scenarios where useful: A dirty inactive tab, a pinned tab, and the active tab all remain immediately distinguishable at a glance.

### P05-T02

- Phase ID: P05
- Task ID: P05-T02
- Task Title: Improve tab overflow, close affordances, drag visuals, and tab-switch discoverability
- Priority: P1
- Category: Tab UI
- Atomic Improvements Covered: 60
- Objective: Make advanced tab interactions feel polished and understandable instead of merely available.
- Why This Matters Now: Tab overload and drag interactions are daily IDE workflows that expose polish gaps quickly.
- UI Problem Statement: Tab drag, overflow, close hit targets, and switcher overlays exist, but the visible cues and transitions can be made much stronger.
- User Experience Impact: Users struggle more than they should to manage many open documents gracefully.
- Scope: Overflow affordances, close hover states, drag previews, insertion indicators, scroll affordances, tab switcher overlay visuals.
- Out of Scope: Underlying tab-switch behavior and document routing.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/TabBar.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/TabSwitcherOverlay.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/TabSwitcherOverlay.h`; `/Users/ryanrentfro/code/markamp/src/ui/DropIndicatorOverlay.cpp`
- Related Screens / Components / Surfaces: Tab strip, switcher overlay, tab drag interactions.
- Current UI Behavior / Appearance: Advanced tab interactions exist but do not always telegraph themselves with elite clarity.
- Intended UI Behavior / Appearance: Overflow and drag scenarios remain visually controlled and highly legible.
- Visual / Interaction Design Direction: Confident technical interaction cues with minimal clutter.
- Technical Styling Approach: Define overflow and drag-state patterns shared between tab strip and switcher overlays.
- Implementation Steps: Refine overflow buttons and indicators; improve drag previews and insertion lines; enlarge and clarify close affordances; elevate the visual hierarchy of the tab switcher.
- Validation Steps: Test crowded tab strips, drag between groups, tab switching overlays, and keyboard switching paths.
- Acceptance Criteria: Tab-management interactions are more discoverable and more polished under stress conditions.
- Dependencies: P05-T01, Phase 19.
- Risks / Failure Modes: More visible affordances can increase clutter if not balanced carefully.
- Accessibility / Readability Notes: Close buttons and drag targets must remain clear at compact density.
- Theme / Styling Notes: Overflow and drag cues need strong visibility in low-contrast themes.
- Motion / Interaction Notes where relevant: Drag and switcher motion should feel smooth and grounded, not flashy.
- Observability / Diagnostics Notes where relevant: Add dense-tab screenshots and drag-state captures.
- Rollback / Safety Notes: Keep drag hit testing stable while visual cues evolve.
- References / Context: The tab strip is already sophisticated enough to justify this higher finish bar.
- Example scenarios where useful: Dragging a tab between split groups produces a premium insertion indicator instead of a weak or ambiguous cue.

### P05-T03

- Phase ID: P05
- Task ID: P05-T03
- Task Title: Redesign breadcrumbs for stronger hierarchy, path readability, and compact elegance
- Priority: P1
- Category: Navigation UI
- Atomic Improvements Covered: 60
- Objective: Make breadcrumbs more useful as a secondary navigation and context surface.
- Why This Matters Now: Breadcrumbs often become noisy or irrelevant unless their hierarchy is carefully tuned.
- UI Problem Statement: [BreadcrumbBar.cpp](/Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp), [BreadcrumbDropdown.cpp](/Users/ryanrentfro/code/markamp/src/ui/BreadcrumbDropdown.cpp), and [BreadcrumbModel.cpp](/Users/ryanrentfro/code/markamp/src/ui/BreadcrumbModel.cpp) need a stronger unified presentation spec.
- User Experience Impact: Users lose an opportunity for calm, compact orientation inside deep files and structures.
- Scope: Segment spacing, separators, icons, hover states, truncation, active crumb emphasis, dropdown treatment, symbol/file path distinction.
- Out of Scope: Breadcrumb data correctness.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/BreadcrumbDropdown.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/BreadcrumbModel.cpp`; `/Users/ryanrentfro/code/markamp/src/core/IconMetricsPolicy.cpp`
- Related Screens / Components / Surfaces: Breadcrumb bar, breadcrumb dropdowns, navigation strips.
- Current UI Behavior / Appearance: Breadcrumbs function, but still need stronger hierarchy and refinement.
- Intended UI Behavior / Appearance: Breadcrumbs become a compact, elegant orientation surface that supports scanning rather than distracting from it.
- Visual / Interaction Design Direction: Light-touch wayfinding with premium microhierarchy.
- Technical Styling Approach: Create breadcrumb-specific typography, separator, truncation, and icon rules within the navigation system.
- Implementation Steps: Audit crumb segment patterns; retune spacing and text hierarchy; refine separator iconography; improve active and hovered crumb visuals; normalize dropdown feel.
- Validation Steps: Review breadcrumbs with long paths, symbol-heavy files, and narrow widths.
- Acceptance Criteria: Breadcrumbs feel more legible and more intentional without becoming visually heavy.
- Dependencies: Phase 01, Phase 04.
- Risks / Failure Modes: Too much hierarchy can make breadcrumbs compete with tabs instead of complementing them.
- Accessibility / Readability Notes: Current crumb and focus-visible states must be obvious and keyboard-friendly.
- Theme / Styling Notes: Breadcrumbs need enough contrast to remain useful without dominating the shell.
- Motion / Interaction Notes where relevant: Dropdown and hover transitions should be subtle and responsive.
- Observability / Diagnostics Notes where relevant: Add breadcrumb width and truncation-state galleries.
- Rollback / Safety Notes: Keep symbol and file breadcrumb data paths stable while redesigning presentation.
- References / Context: Breadcrumbs are one of the most effective places to inject premium subtlety into IDE navigation.
- Example scenarios where useful: A deeply nested markdown heading path remains readable without turning into a cluttered strip.

### P05-T04

- Phase ID: P05
- Task ID: P05-T04
- Task Title: Polish navigation history, back/forward controls, recent-item lists, and switcher surfaces
- Priority: P2
- Category: Navigation UI
- Atomic Improvements Covered: 60
- Objective: Improve secondary navigation surfaces so workspace movement feels fluid and premium.
- Why This Matters Now: Wayfinding quality is not only about tabs and breadcrumbs; smaller navigation surfaces matter too.
- UI Problem Statement: Navigation-related UI is spread across [NavigationHistoryPanel.cpp](/Users/ryanrentfro/code/markamp/src/ui/NavigationHistoryPanel.cpp), [RecentItemsList.cpp](/Users/ryanrentfro/code/markamp/src/ui/RecentItemsList.cpp), [TabSwitcherOverlay.cpp](/Users/ryanrentfro/code/markamp/src/ui/TabSwitcherOverlay.cpp), and startup/welcome surfaces.
- User Experience Impact: Moving through history and recent context feels more mechanical than polished.
- Scope: Back/forward icon treatment, recent item row design, switcher overlay hierarchy, active-result styling, metadata display.
- Out of Scope: Navigation correctness logic.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/NavigationHistoryPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/RecentItemsList.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/TabSwitcherOverlay.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/StartupPanel.cpp`
- Related Screens / Components / Surfaces: Navigation history, recent items, switchers, quick open.
- Current UI Behavior / Appearance: Secondary navigation surfaces are present but not yet deeply polished.
- Intended UI Behavior / Appearance: History and recent-navigation surfaces become compact and premium rather than incidental lists.
- Visual / Interaction Design Direction: Lightweight but trustworthy wayfinding surfaces.
- Technical Styling Approach: Reuse list-row, badge, and overlay tokens from the wider navigation and data-display systems.
- Implementation Steps: Audit navigation surface variants; align row spacing and metadata treatment; improve selection and hover states; refine header and section hierarchy.
- Validation Steps: Exercise recent-item and history surfaces with sparse and dense datasets.
- Acceptance Criteria: Secondary navigation no longer feels visually secondary in quality.
- Dependencies: Phase 01, Phase 15.
- Risks / Failure Modes: Over-embellishing secondary navigation may slow scanning.
- Accessibility / Readability Notes: Row selection and current-item cues must be strong and keyboard-visible.
- Theme / Styling Notes: Metadata text and separators need careful contrast calibration.
- Motion / Interaction Notes where relevant: Switcher and overlay transitions should be crisp but minimal.
- Observability / Diagnostics Notes where relevant: Add recent-item and history screenshots to the navigation gallery.
- Rollback / Safety Notes: Keep row geometry stable while tuning hierarchy and metadata styling.
- References / Context: Navigation quality emerges from many small surfaces, not only the tab strip.
- Example scenarios where useful: Opening recent items from startup or history feels like one premium navigation language.

### P05-T05

- Phase ID: P05
- Task ID: P05-T05
- Task Title: Create cross-surface wayfinding parity for editor, notebook, canvas, and panel transitions
- Priority: P1
- Category: Navigation UI
- Atomic Improvements Covered: 60
- Objective: Ensure navigation surfaces adapt gracefully when the active content surface changes.
- Why This Matters Now: MarkAmp’s product promise spans editor, notebook, canvas, and many panel modes, and wayfinding should feel coherent across them.
- UI Problem Statement: Tabs, breadcrumbs, top controls, and local navigation cues are currently editor-biased and not fully rethought for notebooks and canvases.
- User Experience Impact: Switching content modes can feel like switching products rather than perspectives.
- Scope: Tab labels and icons by artifact type, breadcrumb visibility rules, notebook/canvas-specific wayfinding surfaces, panel-to-document navigation cues.
- Out of Scope: Artifact lifecycle logic.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/TabBar.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp`; `/Users/ryanrentfro/code/markamp/src/core/NotebookShellHost.cpp`
- Related Screens / Components / Surfaces: Editor, notebook, canvas, panels, mixed workbench flows.
- Current UI Behavior / Appearance: Wayfinding surfaces are more mature for text editing than for notebooks or canvases.
- Intended UI Behavior / Appearance: Users always understand where they are and what kind of artifact they are in without stylistic whiplash.
- Visual / Interaction Design Direction: Cross-surface navigation language that remains coherent while reflecting artifact type.
- Technical Styling Approach: Define navigation variants by surface kind while preserving shared geometry, type, and state rules.
- Implementation Steps: Audit navigation cues by artifact type; refine surface-specific icons and metadata; tune breadcrumb and tab variants; add notebook and canvas-specific wayfinding polish.
- Validation Steps: Switch repeatedly among editor, notebook, canvas, and panel-driven navigation targets.
- Acceptance Criteria: Wayfinding feels coherent across all primary content surfaces.
- Dependencies: Phase 01, Phase 09, Phase 10.
- Risks / Failure Modes: Over-specializing artifact-specific navigation can reintroduce subsystem drift.
- Accessibility / Readability Notes: Surface-type cues must not rely on color or iconography alone.
- Theme / Styling Notes: Artifact-specific wayfinding accents must still sit within the global theme language.
- Motion / Interaction Notes where relevant: Surface transitions should reinforce navigation continuity.
- Observability / Diagnostics Notes where relevant: Add cross-surface navigation screenshot journeys.
- Rollback / Safety Notes: Keep common navigation scaffolding intact while surface-specific cues are tuned.
- References / Context: Cross-surface coherence is one of the product’s biggest UI differentiators if executed well.
- Example scenarios where useful: Moving from a code file to a notebook cell to a canvas board still feels like navigation inside one premium environment.

### P05-T06

- Phase ID: P05
- Task ID: P05-T06
- Task Title: Add tab and wayfinding visual QA fixtures for dense and mixed-surface sessions
- Priority: P0
- Category: Cleanup / Consolidation
- Atomic Improvements Covered: 60
- Objective: Protect navigation-surface polish with explicit mixed-session screenshot and state coverage.
- Why This Matters Now: Tabs and wayfinding surfaces are under constant pressure from new features and artifact types.
- UI Problem Statement: Navigation polish will regress unless mixed editor/notebook/canvas states are captured as approved references.
- User Experience Impact: Small navigation regressions compound quickly into a less trustworthy workbench.
- Scope: Tab states, breadcrumb states, switcher overlays, history/recent surfaces, mixed artifact iconography, overflow conditions.
- Out of Scope: Deep navigation logic.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/tests/unit`; `/Users/ryanrentfro/code/markamp/docs/v22_docs`; `/Users/ryanrentfro/code/markamp/src/ui/TabBar.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp`
- Related Screens / Components / Surfaces: Tabs, breadcrumbs, history, switchers, mixed-surface wayfinding.
- Current UI Behavior / Appearance: Navigation quality is not yet guarded by a dedicated visual matrix.
- Intended UI Behavior / Appearance: Navigation polish is durable and regression-gated.
- Visual / Interaction Design Direction: Operationally mature navigation quality management.
- Technical Styling Approach: Build mixed-surface navigation fixtures and add screenshot regression checks for common and stressful scenarios.
- Implementation Steps: Define representative sessions; capture sparse and dense tab strips; include notebook and canvas tabs; add breadcrumb truncation and overflow states; publish the gallery.
- Validation Steps: Perturb tab icon spacing or breadcrumb hierarchy and verify the regression suite catches the drift.
- Acceptance Criteria: Navigation UI regressions become easier to catch before release.
- Dependencies: P05-T01 through P05-T05.
- Risks / Failure Modes: Mixed-surface fixtures can become brittle if they rely on unstable artifact metadata.
- Accessibility / Readability Notes: Include focus-visible navigation and compact-width variants.
- Theme / Styling Notes: Capture navigation fixtures in all key theme modes.
- Motion / Interaction Notes where relevant: Include drag and active-switch states where motion affects appearance.
- Observability / Diagnostics Notes where relevant: Store fixture metadata with artifact types and layout density.
- Rollback / Safety Notes: Keep the QA suite additive while navigation design stabilizes.
- References / Context: Cross-surface navigation is a signature quality area for MarkAmp.
- Example scenarios where useful: A regression that weakens notebook-tab readability is caught even though file tabs still look fine.

