# Phase 02: Shell Chrome Window Frame And Layout Intelligence

## Outcome

Make the outer workbench feel premium through better frame integration, layout balance, split behavior, resize affordances, proportions, and shell-level responsiveness.

## Improvement Count

360 atomic improvements across 6 execution tasks.

### P02-T01

- Phase ID: P02
- Task ID: P02-T01
- Task Title: Refine top-level window frame, title-bar integration, and shell edge treatments
- Priority: P0
- Category: Layout System
- Atomic Improvements Covered: 60
- Objective: Make the app frame and top-level chrome feel intentional and premium instead of like a default host window plus custom widgets.
- Why This Matters Now: First impressions are formed before any document content is read.
- UI Problem Statement: Shell chrome is distributed across [MainFrame.cpp](/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp), [LayoutManager.cpp](/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp), [Toolbar.cpp](/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp), and theme-driven containers without one explicit frame language.
- User Experience Impact: The app can still feel like a styled utility rather than a premium integrated environment.
- Scope: Top strip balance, frame edge contrast, title affordances, top spacing, shell background transitions, top-area grouping.
- Out of Scope: Platform-specific native title-bar APIs beyond visible integration quality.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`; `/Users/ryanrentfro/code/markamp/src/core/ThemeEngine.cpp`
- Related Screens / Components / Surfaces: App frame, top bar, work area shell.
- Current UI Behavior / Appearance: Frame and top chrome feel assembled from separate parts with varying density and edge treatment.
- Intended UI Behavior / Appearance: The window frame reads as one deliberate workbench container with clear top-level hierarchy.
- Visual / Interaction Design Direction: Crisp integrated frame with controlled contrast and restrained chrome ornamentation.
- Technical Styling Approach: Define frame-layer tokens and move shell-edge drawing to a shared frame contract.
- Implementation Steps: Audit shell-edge painting; align top chrome proportions; normalize frame backgrounds and separators; tune theme-specific frame contrast.
- Validation Steps: Compare shell appearance across dark, light, and high-contrast themes on supported platforms.
- Acceptance Criteria: The product frame looks intentional before any document is opened.
- Dependencies: Phase 01.
- Risks / Failure Modes: Over-customization can fight host-platform expectations if native affordances are obscured.
- Accessibility / Readability Notes: Ensure top-level focus, drag regions, and active-window contrast remain clear.
- Theme / Styling Notes: Frame edges should adapt subtly by theme without changing structure.
- Motion / Interaction Notes where relevant: Window activation and mode transitions should not produce jarring chrome shifts.
- Observability / Diagnostics Notes where relevant: Capture shell snapshots for frame comparison across themes and platforms.
- Rollback / Safety Notes: Keep native-safe fallbacks for platform-specific frame behavior.
- References / Context: Shell chrome quality now lags behind the ambition of the internal workbench features.
- Example scenarios where useful: Opening the app into an empty workspace already feels more premium than a stock native frame plus embedded toolbars.

### P02-T02

- Phase ID: P02
- Task ID: P02-T02
- Task Title: Rebalance workbench proportions, content margins, and split-view intelligence
- Priority: P0
- Category: Layout System
- Atomic Improvements Covered: 60
- Objective: Make shell proportions feel intelligent and comfortable across sidebars, editor groups, bottom panels, and secondary sidebars.
- Why This Matters Now: Even visually polished components feel mediocre if the shell allocates space awkwardly.
- UI Problem Statement: Layout behavior across [LayoutManager.cpp](/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp), [SplitView.cpp](/Users/ryanrentfro/code/markamp/src/ui/SplitView.cpp), [SplitterBar.cpp](/Users/ryanrentfro/code/markamp/src/ui/SplitterBar.cpp), and workbench layout models does not yet express one premium proportion language.
- User Experience Impact: Users feel cramped in some areas and visually under-supported in others.
- Scope: Sidebar widths, bottom-panel heights, editor-group balance, notebook/canvas/editor content margins, panel collapse thresholds, responsive breakpoints.
- Out of Scope: Full persistence and restore logic beyond visible behavior and geometry.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/SplitView.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/SplitterBar.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/layout/WorkbenchLayoutModel.cpp`
- Related Screens / Components / Surfaces: Entire shell layout, split work areas, sidebars, bottom region.
- Current UI Behavior / Appearance: Work areas are functional but not always visually balanced.
- Intended UI Behavior / Appearance: The shell allocates space with confident, legible proportions that adapt gracefully to context and window size.
- Visual / Interaction Design Direction: Intelligent spatial hierarchy that prioritizes active content while preserving panel utility.
- Technical Styling Approach: Add shell proportion tokens, breakpoint rules, and context-sensitive layout presets.
- Implementation Steps: Audit default and restored sizes; define target ratios; retune split defaults; add breakpoint-aware adjustments; normalize content margins across workbench modes.
- Validation Steps: Exercise the shell at small, medium, and large window widths with editor, notebook, and canvas primary surfaces.
- Acceptance Criteria: No primary shell mode feels awkwardly starved, over-padded, or visually imbalanced by default.
- Dependencies: P01-T01.
- Risks / Failure Modes: Proportion tuning can conflict with persisted layout states if migration rules are weak.
- Accessibility / Readability Notes: Maintain readable panel minimums and avoid requiring precision resizing for core information.
- Theme / Styling Notes: Spatial changes should support, not replace, color and hierarchy improvements.
- Motion / Interaction Notes where relevant: Proportion changes should animate with restraint if motion is introduced.
- Observability / Diagnostics Notes where relevant: Record layout snapshots before and after the new ratio system.
- Rollback / Safety Notes: Preserve one release of compatibility mapping for persisted legacy sizes.
- References / Context: The shell has sophisticated layout plumbing but still lacks elite proportional polish.
- Example scenarios where useful: Opening an editor plus Problems plus Explorer yields a balanced workspace without manual adjustment.

### P02-T03

- Phase ID: P02
- Task ID: P02-T03
- Task Title: Redesign splitter, resize-handle, collapse, and drag-affordance visuals
- Priority: P1
- Category: Layout System
- Atomic Improvements Covered: 60
- Objective: Make shell resizing and region manipulation feel precise, discoverable, and premium rather than incidental.
- Why This Matters Now: Resize affordances are touched constantly in an IDE and strongly shape perceived quality.
- UI Problem Statement: [SplitterBar.cpp](/Users/ryanrentfro/code/markamp/src/ui/SplitterBar.cpp), [SplitView.cpp](/Users/ryanrentfro/code/markamp/src/ui/SplitView.cpp), and panel containers need stronger visual and interaction feedback.
- User Experience Impact: Users struggle to notice, grab, or trust layout controls, especially on dense or low-contrast themes.
- Scope: Splitter visibility, hover states, drag states, collapse indicators, drop zones, resize handles, target preview lines.
- Out of Scope: Deep docking-model changes beyond visible treatment.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/SplitterBar.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/SplitView.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/PanelContainer.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/DropIndicatorOverlay.cpp`
- Related Screens / Components / Surfaces: Split views, sidebars, panel areas, drag/drop.
- Current UI Behavior / Appearance: Resizing works, but its visual affordances do not yet match premium IDE quality.
- Intended UI Behavior / Appearance: Users can immediately see where they can drag, what will resize, and what target state they are about to produce.
- Visual / Interaction Design Direction: Subtle but unmissable affordances with a precise, technical feel.
- Technical Styling Approach: Introduce splitter state tokens, drag-preview overlays, and consistent resize-handle rendering.
- Implementation Steps: Audit handle visibility; add hover/active visuals; refine hit-target indicators; normalize drag-preview feedback; tune thickness by density profile.
- Validation Steps: Resize and collapse regions with mouse and trackpad across multiple shell modes.
- Acceptance Criteria: Resize and collapse controls are easier to notice, easier to use, and visually consistent across the workbench.
- Dependencies: Phase 01, P02-T02.
- Risks / Failure Modes: Overly visible handles can add noise if not tuned carefully.
- Accessibility / Readability Notes: Handles must remain high-contrast enough for low-vision users and large enough for reliable selection.
- Theme / Styling Notes: Handle and preview colors must work in high-contrast as well as dark/light modes.
- Motion / Interaction Notes where relevant: Drag-preview motion should feel smooth but restrained.
- Observability / Diagnostics Notes where relevant: Add screenshot baselines for hover, drag, and target-preview states.
- Rollback / Safety Notes: Preserve functional resize behavior even if visual upgrade phases are staged.
- References / Context: Resize affordance quality is one of the largest perceived-polish gaps in workbench applications.
- Example scenarios where useful: Hovering the sidebar divider reveals a confident handle and drag preview instead of a nearly invisible line.

### P02-T04

- Phase ID: P02
- Task ID: P02-T04
- Task Title: Improve shell responsiveness, overflow behavior, and small-window adaptation
- Priority: P1
- Category: Layout System
- Atomic Improvements Covered: 60
- Objective: Ensure the interface remains elegant when horizontal space is constrained and shell regions compete for room.
- Why This Matters Now: Premium quality has to survive real laptop and split-screen usage, not just large monitors.
- UI Problem Statement: Many shell widgets assume generous width and do not yet express one responsive strategy.
- User Experience Impact: Controls crowd, labels truncate awkwardly, and panels can feel brittle at smaller sizes.
- Scope: Toolbar overflow, status item truncation, tab density fallback, sidebar collapse heuristics, command palette width, dialog scaling, panel header compactness.
- Out of Scope: Mobile or touch-first redesign.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/StatusBarPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/TabBar.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`
- Related Screens / Components / Surfaces: Shell chrome, overlays, dialogs, tabs, sidebars.
- Current UI Behavior / Appearance: Several shell surfaces assume fixed widths or degrade inelegantly.
- Intended UI Behavior / Appearance: Space-constrained layouts remain coherent, readable, and composed.
- Visual / Interaction Design Direction: Graceful density collapse and overflow handling without panic truncation.
- Technical Styling Approach: Add responsive layout states and compact variants for high-pressure width conditions.
- Implementation Steps: Define breakpoints; create compact treatment rules; normalize truncation and overflow menus; adjust icon-only fallbacks; tune dialog scaling and shell collapse priorities.
- Validation Steps: Exercise shell surfaces at narrow widths and in split-screen conditions.
- Acceptance Criteria: No high-frequency shell surface feels broken or claustrophobic at common laptop widths.
- Dependencies: P01-T01, P02-T02.
- Risks / Failure Modes: Compaction can hurt discoverability if labels disappear without compensating cues.
- Accessibility / Readability Notes: Truncation must preserve tooltip and keyboard discoverability.
- Theme / Styling Notes: Compact states should not remove needed contrast or hierarchy.
- Motion / Interaction Notes where relevant: Overflow transitions and compact-state changes should be smooth and predictable.
- Observability / Diagnostics Notes where relevant: Add width-based screenshot matrices for shell chrome.
- Rollback / Safety Notes: Keep default layouts intact while compact behaviors are tuned.
- References / Context: Several shell surfaces still use fixed dimensions or assume fixed label lengths.
- Example scenarios where useful: The command palette, toolbar, status bar, and tabs remain polished on a 13-inch laptop at comfortable scale.

### P02-T05

- Phase ID: P02
- Task ID: P02-T05
- Task Title: Polish shell empty-state composition and inactive-workbench visuals
- Priority: P1
- Category: Layout System
- Atomic Improvements Covered: 60
- Objective: Make the shell feel deliberately designed even when no artifact is open or when regions are empty.
- Why This Matters Now: Empty shell states expose product confidence more than feature-rich screens do.
- UI Problem Statement: Empty and placeholder regions across the shell still look functional-first or unfinished.
- User Experience Impact: First-run and transitional states feel less premium and less trustworthy.
- Scope: Empty editor groups, inactive panel areas, empty sidebar shells, no-document work area, startup-to-workbench transitions.
- Out of Scope: Onboarding content strategy beyond visual composition.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/EmptyPanelState.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/StartupPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/SidebarSkeletonPlaceholder.cpp`
- Related Screens / Components / Surfaces: Empty shell zones, empty groups, panel placeholders.
- Current UI Behavior / Appearance: Empty areas often look like utility placeholders rather than intentional states.
- Intended UI Behavior / Appearance: Empty workbench zones feel calm, branded, helpful, and coherent with the broader shell language.
- Visual / Interaction Design Direction: Premium restraint with clear next steps and minimal dead space awkwardness.
- Technical Styling Approach: Create shell-empty-state composition patterns with shared spacing, typography, iconography, and action placement rules.
- Implementation Steps: Inventory empty shell surfaces; define empty-state patterns; update inactive regions; align placeholders with honest loading/empty semantics.
- Validation Steps: Exercise startup, empty editor groups, empty sidebars, and empty bottom panels.
- Acceptance Criteria: Empty shell surfaces no longer look temporary or unconsidered.
- Dependencies: Phase 01.
- Risks / Failure Modes: Over-designing empty states can add noise or feel promotional rather than useful.
- Accessibility / Readability Notes: Empty states must remain clear and not rely on decorative illustration alone.
- Theme / Styling Notes: Empty-state backgrounds and iconography need subtle theme shifts without losing identity.
- Motion / Interaction Notes where relevant: Shell-empty transitions should feel calm, not flashy.
- Observability / Diagnostics Notes where relevant: Add empty-state screenshots to the shell regression suite.
- Rollback / Safety Notes: Keep content-neutral empty states for internal builds while final art direction settles.
- References / Context: Empty shell quality remains a visible gap in premium feel.
- Example scenarios where useful: Closing the last tab leaves a composed work area instead of a blank or awkwardly padded region.

### P02-T06

- Phase ID: P02
- Task ID: P02-T06
- Task Title: Add shell-layout visual regression baselines and proportion scorecards
- Priority: P0
- Category: Cleanup / Consolidation
- Atomic Improvements Covered: 60
- Objective: Make shell visual quality measurable so layout polish does not regress during later surface work.
- Why This Matters Now: Layout polish is highly vulnerable to slow drift once feature work resumes.
- UI Problem Statement: The shell lacks hard visual baselines for proportions, handle visibility, compact states, and empty-state composition.
- User Experience Impact: Regressions in shell elegance are hard to notice until the whole product starts feeling rougher.
- Scope: Shell screenshot baselines, ratio reports, responsive matrices, splitter state captures, empty-state captures.
- Out of Scope: Deep logic testing of panel persistence.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/tests/unit`; `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/SplitView.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`
- Related Screens / Components / Surfaces: Entire shell.
- Current UI Behavior / Appearance: Layout quality is evaluated informally.
- Intended UI Behavior / Appearance: Layout polish is guarded by explicit screenshot and proportion baselines.
- Visual / Interaction Design Direction: Operationally defensible shell quality, not taste-based guesswork.
- Technical Styling Approach: Add screenshot fixtures, shell state presets, and proportion assertions to the visual QA stack.
- Implementation Steps: Define baseline states; capture shell widths and mode combinations; publish proportion scorecards; fail visual gates for major drift.
- Validation Steps: Intentionally perturb shell ratios or handle visibility and verify the QA stack catches the change.
- Acceptance Criteria: Shell chrome quality becomes regression-gated rather than manually remembered.
- Dependencies: P02-T01 through P02-T05.
- Risks / Failure Modes: Screenshot baselines can become noisy if shell states are not deterministic.
- Accessibility / Readability Notes: Include focus-visible shell states and compact-width captures in the baseline set.
- Theme / Styling Notes: Baselines must cover at least dark, light, and high-contrast theme modes.
- Motion / Interaction Notes where relevant: Include hover and drag state captures where static screenshots matter.
- Observability / Diagnostics Notes where relevant: Store proportion metrics alongside images for easier triage.
- Rollback / Safety Notes: Start with advisory baselines locally, then promote to CI gates for release profiles.
- References / Context: Later phases will keep changing shell sub-surfaces; the shell needs a durable quality bar now.
- Example scenarios where useful: A future change that makes splitter handles nearly invisible again is caught automatically.

