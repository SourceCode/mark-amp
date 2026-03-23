# Phase 06 - Tabs Breadcrumbs Navigation History And Context Redesign

## Goal

Redesign navigation and wayfinding surfaces so tabs, breadcrumbs, history controls, and contextual indicators feel cleaner, sharper, and more premium than the current UI.

## Definition Of Done

- tabs and navigation surfaces feel newly designed, not incrementally tuned
- icon and text treatment are coherent across all navigation surfaces
- active, inactive, preview, dirty, and focused states are clearer and more refined

## Tasks

### Phase ID: V27-P06
### Task ID: V27-P06-T01
### Task Title: Redesign Primary And Secondary Tab Systems Around A More Premium Navigation Language
- Priority: P0
- Category: Tab UI
- Objective: rebuild tab presentation, hierarchy, and state treatment for a sharper and more luxurious navigation experience.
- Why This Matters Now: Tabs are among the highest-frequency surfaces in the product and one of the strongest visual benchmarks against other IDEs.
- Visual Gap Statement: current tab surfaces are custom and capable, but still carry an evolved rather than fully redesigned navigation language.
- User Experience Impact: Better tabs improve orientation, reduce friction, and make the app feel materially more sophisticated.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/TabBar.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/ToolWindowTabControl.cpp`
- Prior Plan References: `v22` Phase 05, `v26` Phase 04
- Scope: tab geometry, active/inactive/hover/dirty states, close affordance, pinned/preview variants, tool-window tab actions
- Out of Scope: tab lifecycle logic
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/TabBar.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/ToolWindowTabControl.cpp`
- Related Screens / Components / Surfaces: editor tabs, notebook tabs, tool-window tabs, secondary tab strips
- Current Visual Behavior / Appearance: tab systems are strong but still feel like polished custom components rather than a fully reauthored navigation identity.
- Intended Visual Behavior / Appearance: tabs feel cleaner, more precise, and clearly redesigned around the new icon and state language.
- Icon / Visual Design Direction: premium tab system with subtle layering, excellent spacing, and restrained but unmistakable state cues.
- Technical Styling Approach: redesign tab metrics, content layout, icon roles, and state treatments using the new visual system and canonical icon usage rules.
- Implementation Steps:
  1. Redesign tab metrics and content hierarchy.
  2. Rebuild close/pin/preview visual treatments and iconography.
  3. Align main tabs and tool-window tabs under one stronger navigation language.
  4. Validate crowded, sparse, and preview-heavy tab strips.
- Validation Steps:
  1. Open many tabs and review readability.
  2. Hover active and inactive tabs.
  3. Check pinned, preview, and dirty variants if supported.
- Acceptance Criteria: the main and tool-window tab systems feel like a new, cohesive navigation design.
- Dependencies: Phase 01 through Phase 05
- Parallelization Notes: can run alongside breadcrumb and history redesign.
- Risks / Failure Modes: overly aggressive styling can make tabs too heavy or reduce scan speed.
- Accessibility / Readability Notes: state differentiation must remain visible for keyboard and mouse users.
- Theme / Styling Notes: redesigned tabs must keep strong parity in light and dark themes.
- Motion / Interaction Notes where relevant: later tab transitions should reinforce the new state hierarchy.
- Cleanup / Consolidation Notes where relevant: retire older per-tab-style assumptions and icon naming fragments.
- Rollback / Safety Notes: preserve tab usability and hit targets throughout redesign.
- References / Context: tabs are a primary quality comparison surface with top IDEs.
- Example scenarios where useful: a dirty markdown tab, a pinned code tab, and a preview notebook tab should all feel related but distinctly legible.

### Phase ID: V27-P06
### Task ID: V27-P06-T02
### Task Title: Redesign Breadcrumbs And Context Paths For Better Hierarchy And Premium Clarity
- Priority: P1
- Category: Navigation UI
- Objective: rebuild breadcrumbs and path/context surfaces to feel more elegant, precise, and integrated with tabs and panels.
- Why This Matters Now: Breadcrumbs are high-value wayfinding surfaces and currently still show hardcoded or inherited styling patterns.
- Visual Gap Statement: breadcrumb and path treatment remains less mature and less unified than the strongest shell surfaces.
- User Experience Impact: clearer wayfinding improves orientation and reduces cognitive load in complex workspaces.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/ExplorerPanel.cpp`
- Prior Plan References: `v22` Phase 05, `v26` Phase 04
- Scope: breadcrumb chips, separators, truncation, context metadata, path depth readability, icon integration where appropriate
- Out of Scope: path resolution logic
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/ExplorerPanel.cpp`
- Related Screens / Components / Surfaces: breadcrumb bar, path strips, context labels in panel and editor-adjacent surfaces
- Current Visual Behavior / Appearance: breadcrumb visuals exist, but they still carry local paint choices and less mature hierarchy than the redesigned shell target requires.
- Intended Visual Behavior / Appearance: breadcrumbs feel cleaner, more premium, and more naturally integrated into the navigation system.
- Icon / Visual Design Direction: elegant path UI with crisp separators, subtle hierarchy, and excellent truncation behavior.
- Technical Styling Approach: redesign breadcrumb geometry and semantics using canonical tokens and remove remaining hardcoded visual fragments.
- Implementation Steps:
  1. Replace local breadcrumb paint assumptions with redesigned semantic roles.
  2. Rebuild spacing, separators, hover, and selected states.
  3. Improve truncation and long-path readability.
  4. Validate alignment with tabs and editor chrome.
- Validation Steps:
  1. Test short and deeply nested paths.
  2. Compare breadcrumb alignment against tabs and shell controls.
  3. Review focus and hover visibility in both themes.
- Acceptance Criteria: breadcrumbs feel clearly redesigned and no longer read as a lower-maturity navigation surface.
- Dependencies: Phase 01 through Phase 04
- Parallelization Notes: can run with tab redesign.
- Risks / Failure Modes: over-stylized breadcrumb chips can reduce path readability.
- Accessibility / Readability Notes: deep paths require strong focus and truncation clarity.
- Theme / Styling Notes: separators and supporting metadata must remain low-noise but legible.
- Motion / Interaction Notes where relevant: hover/focus transitions should later remain restrained.
- Cleanup / Consolidation Notes where relevant: remove hardcoded colors and local breadcrumb state rules.
- Rollback / Safety Notes: preserve hit targets and path discoverability.
- References / Context: breadcrumb quality is an under-leveraged premium differentiator in this product.
- Example scenarios where useful: a deep file path should remain elegant and scannable in the redesigned breadcrumb bar.

### Phase ID: V27-P06
### Task ID: V27-P06-T03
### Task Title: Redesign History Controls Context Switchers And Navigation Utility Surfaces
- Priority: P1
- Category: Navigation UI
- Objective: unify navigation utility controls so back/forward, context switches, and related indicators feel more intentional and modern.
- Why This Matters Now: utility navigation surfaces often reveal visual inconsistency even after major shell work.
- Visual Gap Statement: navigation-related controls are distributed across toolbar, tabs, path surfaces, and panel headers, increasing drift risk.
- User Experience Impact: consistent utility navigation reduces friction and strengthens shell coherence.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/ActivityBar.cpp`
- Prior Plan References: `v22` shell/navigation phases, `v26` Phase 04
- Scope: back/forward controls, section switchers, context indicators, navigation utility buttons, lightweight location metadata
- Out of Scope: actual navigation history logic
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/ActivityBar.cpp`
- Related Screens / Components / Surfaces: toolbar navigation, shell history controls, section switchers, contextual utility controls
- Current Visual Behavior / Appearance: utility navigation controls exist, but likely still inherit mixed control and icon presentation patterns.
- Intended Visual Behavior / Appearance: utility navigation surfaces feel clearly related to tabs, breadcrumbs, and the redesigned action surfaces.
- Icon / Visual Design Direction: discreet but premium utility controls with better icon clarity and hierarchy.
- Technical Styling Approach: redesign navigation utility families around the canonical icon map and shared control language.
- Implementation Steps:
  1. Audit utility navigation controls and related context indicators.
  2. Redesign icon semantics, control geometry, and state treatment.
  3. Align these controls with tab and breadcrumb hierarchy.
  4. Validate in compact and wide shell layouts.
- Validation Steps:
  1. Compare navigation utility controls against toolbar and tab visuals.
  2. Check active, disabled, and focused states.
  3. Confirm controls remain discoverable without adding visual noise.
- Acceptance Criteria: navigation utility surfaces no longer look like leftover shell fragments from earlier visual generations.
- Dependencies: Phase 01 through Phase 05
- Parallelization Notes: can run after the redesigned action-surface family is stable.
- Risks / Failure Modes: over-emphasizing utility controls can make the shell cluttered.
- Accessibility / Readability Notes: disabled and current-context states must remain clear.
- Theme / Styling Notes: these controls must use the same icon-state and control-state rules as the rest of the redesign.
- Motion / Interaction Notes where relevant: later micro-interactions should use the same tactile language as toolbar controls.
- Cleanup / Consolidation Notes where relevant: consolidate utility control variants rather than keeping local exceptions.
- Rollback / Safety Notes: preserve functionality and shortcuts while redesigning visuals.
- References / Context: this task finishes navigation coherence beyond tabs and breadcrumbs.
- Example scenarios where useful: history navigation and section switching should feel like purpose-built parts of one premium navigation system.
