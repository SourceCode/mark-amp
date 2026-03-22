# Phase 03: Activity Bar Sidebars Panel Headers And Shell Chrome

## Outcome

Make activity navigation, sidebars, panel headers, section chrome, and shell-adjacent panel furniture feel visually unified, calm, and premium.

## Improvement Count

360 atomic improvements across 6 execution tasks.

### P03-T01

- Phase ID: P03
- Task ID: P03-T01
- Task Title: Refine activity bar visual language, slot rhythm, badges, and overflow treatment
- Priority: P0
- Category: Navigation UI
- Atomic Improvements Covered: 60
- Objective: Make the activity bar feel elegant, intentional, and perfectly aligned with the shell.
- Why This Matters Now: The activity bar is one of the product’s strongest identity surfaces.
- UI Problem Statement: [ActivityBar.cpp](/Users/ryanrentfro/code/markamp/src/ui/ActivityBar.cpp) custom-paints many states locally and currently balances density, badges, overflow, and selection without one final visual finish bar.
- User Experience Impact: Navigation feels more mechanical than premium.
- Scope: Slot sizing, active indicator shape, hover treatment, badge treatment, overflow button, drag states, tooltips, icon alignment.
- Out of Scope: Command routing or panel activation logic.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/ActivityBar.cpp`; `/Users/ryanrentfro/code/markamp/src/core/IconMetricsPolicy.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/TooltipWindow.cpp`
- Related Screens / Components / Surfaces: Activity bar, primary navigation, badge states.
- Current UI Behavior / Appearance: The activity bar is functional and custom, but still carries local spacing, badge, and hover decisions.
- Intended UI Behavior / Appearance: The activity bar becomes a signature navigation surface with impeccable icon rhythm and state clarity.
- Visual / Interaction Design Direction: Slim, confident, slightly futuristic shell navigation with premium icon focus.
- Technical Styling Approach: Move slot metrics, badge metrics, and state visuals into shared navigation tokens and variant helpers.
- Implementation Steps: Audit current slot metrics and state drawings; normalize icon and badge baselines; refine selection indicator; tune overflow presentation; unify tooltip offsets.
- Validation Steps: Review activity bar in dark, light, compact, and high-contrast states with varying badge counts.
- Acceptance Criteria: Activity bar states look balanced, intentional, and visually stable under all common conditions.
- Dependencies: Phase 01, Phase 02.
- Risks / Failure Modes: Over-decorating the activity bar could reduce its calm navigational clarity.
- Accessibility / Readability Notes: Active and focused items must remain distinct even without color sensitivity.
- Theme / Styling Notes: Badge contrast and icon treatment must retain legibility in both dark and light themes.
- Motion / Interaction Notes where relevant: Hover and activation transitions should be smooth but subtle.
- Observability / Diagnostics Notes where relevant: Add screenshot captures for zero-badge, dot-badge, count-badge, and overflow states.
- Rollback / Safety Notes: Keep the slot geometry stable while refining visuals to avoid destabilizing hit targets.
- References / Context: The activity bar already has strong infrastructure and deserves a higher visual finish.
- Example scenarios where useful: A Git badge, Search badge, and active Explorer state all feel harmonized instead of individually tuned.

### P03-T02

- Phase ID: P03
- Task ID: P03-T02
- Task Title: Rebuild primary and secondary sidebar chrome for stronger hierarchy and calmer density
- Priority: P0
- Category: Panel UI
- Atomic Improvements Covered: 60
- Objective: Make sidebar surfaces feel more intentional through better headers, spacing, section rhythm, and content framing.
- Why This Matters Now: Sidebars are where users spend most of their time outside the editor.
- UI Problem Statement: Sidebar chrome is spread across [SidebarHeader.cpp](/Users/ryanrentfro/code/markamp/src/ui/SidebarHeader.cpp), [SidebarSection.cpp](/Users/ryanrentfro/code/markamp/src/ui/SidebarSection.cpp), [SidebarFooter.cpp](/Users/ryanrentfro/code/markamp/src/ui/SidebarFooter.cpp), and panel-local content.
- User Experience Impact: Panel content competes with chrome instead of being framed by it.
- Scope: Header density, section dividers, footer rhythm, content padding, empty/loading states, sidebar background layering, secondary tab strips.
- Out of Scope: Panel business logic.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/SidebarHeader.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/SidebarSection.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/SidebarFooter.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/SecondarySidebarTabStrip.cpp`
- Related Screens / Components / Surfaces: Explorer, Search, Graph, Outline, secondary sidebar, panel sections.
- Current UI Behavior / Appearance: Sidebar shells vary by panel and do not always produce clear visual grouping.
- Intended UI Behavior / Appearance: Sidebars feel like premium instrument panels with clear structure and breathing room.
- Visual / Interaction Design Direction: Structured but quiet chrome that helps content scan faster.
- Technical Styling Approach: Create sidebar chrome variants and section tokens for padding, titles, dividers, and density.
- Implementation Steps: Audit sidebar shell composition; define header/section/footer variants; migrate primary and secondary sidebars; normalize section expansion visuals.
- Validation Steps: Compare multiple sidebar modes with dense and sparse content.
- Acceptance Criteria: Sidebar mode switches no longer feel like changes between separate UI systems.
- Dependencies: Phase 01, Phase 02.
- Risks / Failure Modes: Too much chrome can crowd already-dense sidebar content.
- Accessibility / Readability Notes: Section titles and boundaries must remain visible for keyboard and low-vision users.
- Theme / Styling Notes: Sidebar layering should stay subtle in dark themes and avoid washed-out flattening in light themes.
- Motion / Interaction Notes where relevant: Section expand/collapse should feel immediate and polished.
- Observability / Diagnostics Notes where relevant: Add sidebar-state screenshot references across modes.
- Rollback / Safety Notes: Migrate shell chrome separately from panel content logic to avoid coupling.
- References / Context: The shell already exposes multiple sidebar shells but not yet one final visual grammar.
- Example scenarios where useful: Switching from Explorer to Search to Graph feels like changing instruments on one console, not opening different mini-apps.

### P03-T03

- Phase ID: P03
- Task ID: P03-T03
- Task Title: Upgrade panel headers, panel tab bars, and local toolbar presentation
- Priority: P1
- Category: Panel UI
- Atomic Improvements Covered: 60
- Objective: Make panel-local chrome as polished as the main toolbar and navigation.
- Why This Matters Now: Panel headers are everywhere and currently vary in density, icon spacing, and affordance clarity.
- UI Problem Statement: [PanelHeader.cpp](/Users/ryanrentfro/code/markamp/src/ui/PanelHeader.cpp), [PanelHeaderBar.cpp](/Users/ryanrentfro/code/markamp/src/ui/PanelHeaderBar.cpp), [PanelTabBar.cpp](/Users/ryanrentfro/code/markamp/src/ui/PanelTabBar.cpp), and local panel toolbars do not yet share one refined visual pattern.
- User Experience Impact: Panels feel more utilitarian and less polished than the shell should.
- Scope: Header typography, title/icon balance, action grouping, panel-tab states, drag affordances, overflow controls, local toolbar rhythm.
- Out of Scope: Panel command correctness.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/PanelHeader.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/PanelHeaderBar.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/PanelTabBar.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/SidebarToolbar.cpp`
- Related Screens / Components / Surfaces: Bottom panels, side panels, panel tabs, local panel toolbars.
- Current UI Behavior / Appearance: Headers and local toolbars work, but their visual rhythm varies too much.
- Intended UI Behavior / Appearance: Panel chrome is compact, elegant, and visually consistent regardless of panel family.
- Visual / Interaction Design Direction: Precision-instrument chrome with minimal noise and strong affordance clarity.
- Technical Styling Approach: Define header and panel-toolbar variants with shared tokens for height, spacing, icons, and states.
- Implementation Steps: Inventory header and panel toolbar variants; normalize title and action layout; align tab strip treatments; refine overflow and close states.
- Validation Steps: Compare primary, secondary, and bottom panel headers and tab bars in multiple content states.
- Acceptance Criteria: Panel chrome feels systematically designed across all host areas.
- Dependencies: Phase 01, Phase 02.
- Risks / Failure Modes: Excessive visual flattening may reduce panel-specific identity where a small amount is useful.
- Accessibility / Readability Notes: Header actions must remain discoverable and keyboard focus states must be obvious.
- Theme / Styling Notes: Header layering must avoid blending into panel content in low-contrast themes.
- Motion / Interaction Notes where relevant: Active tab and hover transitions should reinforce structure, not call attention to themselves.
- Observability / Diagnostics Notes where relevant: Add panel-header visual snapshots for every panel host area.
- Rollback / Safety Notes: Keep current panel action hit targets while refining layout.
- References / Context: Panel chrome quality is one of the fastest ways to make the workbench feel elite instead of merely functional.
- Example scenarios where useful: A Problems panel header and an Explorer panel header feel like siblings, not distant cousins.

### P03-T04

- Phase ID: P03
- Task ID: P03-T04
- Task Title: Normalize sidebar and panel loading, skeleton, and empty-state presentation
- Priority: P1
- Category: Panel UI
- Atomic Improvements Covered: 60
- Objective: Replace provisional or inconsistent panel placeholder visuals with one calm, premium loading and empty-state language.
- Why This Matters Now: Temporary-looking placeholders are a strong signal of unfinished UI.
- UI Problem Statement: [SidebarSkeletonPlaceholder.cpp](/Users/ryanrentfro/code/markamp/src/ui/SidebarSkeletonPlaceholder.cpp), [EmptyPanelState.cpp](/Users/ryanrentfro/code/markamp/src/ui/EmptyPanelState.cpp), and panel-local empty/loading visuals vary widely.
- User Experience Impact: Empty and loading panel states can look provisional or unrelated to the shell.
- Scope: Skeleton loaders, empty messaging, empty action placement, loading shimmer rhythm, content padding, sparse panel states.
- Out of Scope: Loading logic correctness.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/SidebarSkeletonPlaceholder.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/EmptyPanelState.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/ExplorerPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/SearchSidebarPanel.cpp`
- Related Screens / Components / Surfaces: Sidebars, panel empty states, loading panels, sparse sections.
- Current UI Behavior / Appearance: Placeholder and empty-state visuals vary by subsystem and sometimes look technical rather than polished.
- Intended UI Behavior / Appearance: Loading and empty panel states feel intentionally designed, honest, and visually calm.
- Visual / Interaction Design Direction: Soft but structured placeholder language that maintains shell dignity.
- Technical Styling Approach: Define shared skeleton and empty-state components with panel-specific size presets.
- Implementation Steps: Audit placeholder surfaces; standardize skeleton blocks and empty-state composition; align copy hierarchy and action placement; remove temporary-looking filler.
- Validation Steps: Trigger loading and empty states across Explorer, Search, Extensions, Settings, and utility panels.
- Acceptance Criteria: No panel loading or empty state feels like a temporary developer stub.
- Dependencies: Phase 01, Phase 02.
- Risks / Failure Modes: Overly decorative loaders can slow perceived performance.
- Accessibility / Readability Notes: Skeleton contrast and empty-state text hierarchy must remain readable in all themes.
- Theme / Styling Notes: Shimmer intensity and panel-empty background contrast need theme-specific calibration.
- Motion / Interaction Notes where relevant: Loading animation should be subtle and reduced-motion aware.
- Observability / Diagnostics Notes where relevant: Add panel-state screenshot fixtures for empty and loading conditions.
- Rollback / Safety Notes: Prefer honest static empty states over overly clever animated placeholders if motion is not ready.
- References / Context: Placeholder-heavy sidebars and sparse panel states are still visible throughout the repo.
- Example scenarios where useful: An empty Search sidebar looks polished and informative instead of like an uninitialized container.

### P03-T05

- Phase ID: P03
- Task ID: P03-T05
- Task Title: Improve shell-adjacent microfurniture such as footers, badges, separators, and section toggles
- Priority: P2
- Category: Control Styling
- Atomic Improvements Covered: 60
- Objective: Remove small but cumulative sources of visual roughness in shell-adjacent chrome.
- Why This Matters Now: Premium feel is often lost in tiny secondary elements rather than major components.
- UI Problem Statement: Small elements like section chevrons, dividers, footer metadata, status dots, and tiny badges remain inconsistent.
- User Experience Impact: The shell feels busier and less precise than it should.
- Scope: Sidebar footers, section toggles, metadata chips, badge geometry, tiny icons, separators, disclosure visuals.
- Out of Scope: Primary action-surface redesign.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/SidebarFooter.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/SidebarSection.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/PanelHeaderIcons.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/ActivityBar.cpp`
- Related Screens / Components / Surfaces: Sidebar sections, panel chrome, badges, metadata strips.
- Current UI Behavior / Appearance: Small shell elements are individually serviceable but not optically consistent.
- Intended UI Behavior / Appearance: Small elements contribute to precision rather than noise.
- Visual / Interaction Design Direction: Quiet technical detail with high optical discipline.
- Technical Styling Approach: Add microcomponent tokens for badge radius, separator opacity, chevron spacing, and footer metadata styling.
- Implementation Steps: Inventory microfurniture; define microcomponent variants; migrate shell-adjacent elements; remove redundant dividers and misaligned tiny icons.
- Validation Steps: Audit shell screenshots at 100% and 125% scale for small-element visual noise.
- Acceptance Criteria: Small shell details no longer undermine the otherwise premium composition.
- Dependencies: Phase 01.
- Risks / Failure Modes: Over-tuning tiny elements can consume time without visible return if larger shell problems remain unresolved.
- Accessibility / Readability Notes: Tiny indicators must remain legible and not rely solely on subtle low-contrast differences.
- Theme / Styling Notes: Separator and badge opacity need careful theme calibration to avoid disappearing or shouting.
- Motion / Interaction Notes where relevant: Tiny state changes must avoid flicker or shimmer.
- Observability / Diagnostics Notes where relevant: Include zoomed visual references for microcomponent alignment.
- Rollback / Safety Notes: Keep microcomponent cleanup non-disruptive to functionality and layout logic.
- References / Context: Many shell-adjacent elements currently reflect historical local tweaks rather than a finished design system.
- Example scenarios where useful: A sidebar chevron, status badge, and section divider all feel like part of one precision UI language.

### P03-T06

- Phase ID: P03
- Task ID: P03-T06
- Task Title: Add shell-chrome component galleries and panel-furniture regression coverage
- Priority: P0
- Category: Cleanup / Consolidation
- Atomic Improvements Covered: 60
- Objective: Make shell-chrome improvements durable by capturing standardized component galleries and regression checks.
- Why This Matters Now: Sidebars and panel furniture are highly reusable and highly prone to drift.
- UI Problem Statement: There is no authoritative gallery that shows approved activity bar, panel header, section, badge, and empty-state variants.
- User Experience Impact: Subtle regressions accumulate until the shell feels uneven again.
- Scope: Activity bar states, sidebar headers, section rows, panel headers, panel tabs, skeletons, empty states, badges, separators.
- Out of Scope: Deep content rendering inside panels.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/tests/unit`; `/Users/ryanrentfro/code/markamp/docs/v22_docs`; `/Users/ryanrentfro/code/markamp/src/ui/ActivityBar.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/PanelHeader.cpp`
- Related Screens / Components / Surfaces: Shell chrome primitives, sidebars, panel furniture.
- Current UI Behavior / Appearance: Shell-adjacent primitives are visually judged ad hoc.
- Intended UI Behavior / Appearance: Every shell-chrome primitive has an approved visual reference and regression check.
- Visual / Interaction Design Direction: Systematic shell quality management.
- Technical Styling Approach: Build screenshot galleries and fixture states for shell primitives, then connect them to visual QA tooling.
- Implementation Steps: Define gallery fixtures; capture state matrices; publish reference docs; add regression checks for key chrome components.
- Validation Steps: Intentionally alter a sidebar header or badge treatment and verify the regression suite catches it.
- Acceptance Criteria: Shell furniture quality is referenceable and regression-gated.
- Dependencies: P03-T01 through P03-T05.
- Risks / Failure Modes: Galleries become stale if not tied to executable component fixtures.
- Accessibility / Readability Notes: Include focus-visible and high-contrast variants in the gallery.
- Theme / Styling Notes: Capture component families in all supported theme modes.
- Motion / Interaction Notes where relevant: Add representative hover and active-state captures for animated surfaces.
- Observability / Diagnostics Notes where relevant: Store gallery metadata with component names, state IDs, and token references.
- Rollback / Safety Notes: Keep galleries additive and version-controlled.
- References / Context: `v22` needs shell polish to stick after the implementation wave, not just peak briefly.
- Example scenarios where useful: A future change to section headers can be reviewed against an approved component gallery instead of memory.

