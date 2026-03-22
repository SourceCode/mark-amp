# Phase 16: Startup Welcome Walkthrough Empty Loading And Error States

## Outcome

Make onboarding, startup, sparse-state, loading, and error experiences feel warm, premium, and visually intentional instead of like placeholder utility screens.

## Improvement Count

360 atomic improvements across 6 execution tasks.

### P16-T01

- Phase ID: P16
- Task ID: P16-T01
- Task Title: Redesign startup and welcome surfaces as flagship first-run experiences
- Priority: P0
- Category: Empty State / Onboarding UI
- Atomic Improvements Covered: 60
- Objective: Make the first screen users see feel premium, cohesive, and confident.
- Why This Matters Now: Startup quality shapes the user’s first impression of the whole product.
- UI Problem Statement: [StartupPanel.cpp](/Users/ryanrentfro/code/markamp/src/ui/StartupPanel.cpp) still hardcodes local button styling and [StartupPanel_styling.txt](/Users/ryanrentfro/code/markamp/src/ui/StartupPanel_styling.txt) explicitly says styling work is still pending.
- User Experience Impact: First-run experience feels less finished than the rest of the application should.
- Scope: Startup hero area, brand framing, recent workspaces layout, primary CTAs, typography hierarchy, shell integration, welcome-card treatment.
- Out of Scope: Startup workflow logic.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/StartupPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/StartupPanel.h`; `/Users/ryanrentfro/code/markamp/src/ui/StartupPanel_styling.txt`; `/Users/ryanrentfro/code/markamp/src/ui/WelcomeTab.cpp`
- Related Screens / Components / Surfaces: Startup panel, welcome tab, recent workspaces, first-run shell states.
- Current UI Behavior / Appearance: Startup uses local fonts, local accent colors, and utility-grade button construction.
- Intended UI Behavior / Appearance: Startup and welcome feel like premium branded experiences that still respect workbench seriousness.
- Visual / Interaction Design Direction: Warm, intentional, editorial first-run experience with crisp utility and no startup deadness.
- Technical Styling Approach: Rebuild startup and welcome using shared card, button, typography, and list systems plus a dedicated onboarding shell variant.
- Implementation Steps: Replace local button drawing; redesign layout and hierarchy; refine recent-item presentation; align startup shell with the main workbench; improve welcome cards and typography.
- Validation Steps: Launch from a clean profile and review startup/welcome in dark, light, and compact-width states.
- Acceptance Criteria: Startup and welcome feel clearly premium and no longer carry visible placeholder styling debt.
- Dependencies: Phase 01, Phase 02, Phase 15.
- Risks / Failure Modes: Over-branding startup can make it feel like marketing instead of a professional tool entry point.
- Accessibility / Readability Notes: Primary CTAs, recent items, and headings must remain easy to navigate and highly legible.
- Theme / Styling Notes: Startup should feel beautiful in all theme modes, not only the default theme.
- Motion / Interaction Notes where relevant: First-run transitions should feel calm and not delay task entry.
- Observability / Diagnostics Notes where relevant: Add startup and welcome screenshot boards for first-run and returning-user states.
- Rollback / Safety Notes: Preserve current open-folder and recent-workspace behavior while visual design is upgraded.
- References / Context: Startup remains one of the few places where the repo explicitly documents unfinished styling work.
- Example scenarios where useful: A brand-new user sees a startup screen that feels like a premium IDE, not a styled utility dialog.

### P16-T02

- Phase ID: P16
- Task ID: P16-T02
- Task Title: Redesign walkthroughs, helper cards, and learning cues for premium discoverability
- Priority: P2
- Category: Empty State / Onboarding UI
- Atomic Improvements Covered: 60
- Objective: Make educational and guided surfaces feel refined instead of secondary to the main interface.
- Why This Matters Now: Product discoverability depends on the quality of its learning surfaces, especially in a feature-rich app.
- UI Problem Statement: [WalkthroughPanel.cpp](/Users/ryanrentfro/code/markamp/src/ui/WalkthroughPanel.cpp) and related walkthrough assets provide content, but need stronger visual framing.
- User Experience Impact: Users may ignore or distrust learning surfaces if they look lower quality than the workbench.
- Scope: Walkthrough cards, progression indicators, action CTAs, informational callouts, iconography, section hierarchy, illustration placeholders.
- Out of Scope: Walkthrough content strategy.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/WalkthroughPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/WalkthroughPanel.h`; `/Users/ryanrentfro/code/markamp/src/ui/WelcomeTab.cpp`
- Related Screens / Components / Surfaces: Walkthroughs, helper cards, learning cues, getting-started content.
- Current UI Behavior / Appearance: Walkthrough content exists but does not yet carry flagship UI quality.
- Intended UI Behavior / Appearance: Learning surfaces feel elegantly integrated into the product’s visual language.
- Visual / Interaction Design Direction: Gentle guidance with strong hierarchy and premium restraint.
- Technical Styling Approach: Extend onboarding shell and card systems to walkthrough progress and helper content.
- Implementation Steps: Audit walkthrough layout; improve card hierarchy and progression; align CTA styling; refine helper blocks and callout cards; normalize typography and spacing.
- Validation Steps: Step through walkthrough content in multiple themes and compact widths.
- Acceptance Criteria: Guided-learning UI feels worthy of the product rather than like optional support material.
- Dependencies: Phase 01, P16-T01.
- Risks / Failure Modes: Over-designed walkthroughs can feel promotional or interruptive.
- Accessibility / Readability Notes: Progress and current-step cues must be obvious and keyboard-friendly.
- Theme / Styling Notes: Learning surfaces should remain visually calm and readable regardless of theme.
- Motion / Interaction Notes where relevant: Step transitions should be subtle and avoid theatrical motion.
- Observability / Diagnostics Notes where relevant: Add walkthrough state screenshots and progression variants.
- Rollback / Safety Notes: Keep content and action destinations stable while visual shells change.
- References / Context: Discoverability is part of UI quality, especially in a wide-scope application.
- Example scenarios where useful: A notebook walkthrough feels polished enough that users actually want to read it.

### P16-T03

- Phase ID: P16
- Task ID: P16-T03
- Task Title: Standardize empty-panel, no-data, and no-selection states across the app
- Priority: P0
- Category: Empty State / Onboarding UI
- Atomic Improvements Covered: 60
- Objective: Replace dead-looking or generic empty states with one excellent sparse-state language.
- Why This Matters Now: The product exposes many contextual panels that are frequently empty or selection-dependent.
- UI Problem Statement: [EmptyPanelState.cpp](/Users/ryanrentfro/code/markamp/src/ui/EmptyPanelState.cpp), [CanvasWorkspacePanel.cpp](/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp), and many panel-specific shells still show sparse states inconsistently.
- User Experience Impact: Empty surfaces can feel like unfinished product areas rather than intentional transitions.
- Scope: No selection, no results, no workspace, no outputs, no objects, no extensions, empty data views, sparse canvases, sparse notebooks.
- Out of Scope: Sparse-state business logic.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/EmptyPanelState.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/ExplorerPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/SearchSidebarPanel.cpp`
- Related Screens / Components / Surfaces: Empty panel states, no-selection states, no-results states, sparse shells.
- Current UI Behavior / Appearance: Sparse states vary in tone, spacing, icon usage, and action placement.
- Intended UI Behavior / Appearance: Sparse states become honest, useful, and visually consistent.
- Visual / Interaction Design Direction: Calm, intentional emptiness with purposeful next steps and no deadness.
- Technical Styling Approach: Define sparse-state composition variants shared by panels, editors, notebooks, and canvas-related surfaces.
- Implementation Steps: Inventory sparse states; align iconography and copy hierarchy; normalize action placement; distinguish no-results from empty and uninitialized states; remove temporary-looking filler.
- Validation Steps: Trigger sparse states across major panel families and core content surfaces.
- Acceptance Criteria: No major sparse surface still looks generic or provisional.
- Dependencies: Phase 01, Phase 03, Phase 14.
- Risks / Failure Modes: Overly decorative sparse states can distract from the user’s next action.
- Accessibility / Readability Notes: Empty-state text and CTAs must remain concise and clearly associated.
- Theme / Styling Notes: Sparse-state art direction must hold together across dark and light modes.
- Motion / Interaction Notes where relevant: Sparse-state appearance transitions should be gentle.
- Observability / Diagnostics Notes where relevant: Add sparse-state screenshot matrices across the main surface families.
- Rollback / Safety Notes: Preserve current CTA behavior while sparse-state visuals are normalized.
- References / Context: Sparse-state quality is part of the overall premium feel, especially in a panel-rich product.
- Example scenarios where useful: A no-selection inspector feels informative and calm rather than like a blank property sheet.

### P16-T04

- Phase ID: P16
- Task ID: P16-T04
- Task Title: Improve loading and error-state composition across onboarding and sparse-content surfaces
- Priority: P1
- Category: Empty State / Onboarding UI
- Atomic Improvements Covered: 60
- Objective: Make loading and error moments feel composed, consistent, and product-grade.
- Why This Matters Now: Rough loading and error presentation can negate the polish of the steady-state UI.
- UI Problem Statement: Loading skeletons, empty states, and error surfaces exist in multiple forms and are not yet visually unified.
- User Experience Impact: Users encounter jarring visual quality drops during slow or failed operations.
- Scope: Onboarding loading states, panel skeletons, shell loading, blocking and non-blocking errors, retry affordances, inline warnings.
- Out of Scope: Error propagation logic.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/SidebarSkeletonPlaceholder.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/NotificationManager.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/ThemedDialog.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/StartupPanel.cpp`
- Related Screens / Components / Surfaces: Startup, welcome, sidebars, dialogs, sparse content states.
- Current UI Behavior / Appearance: Loading and error styling varies by surface and often feels more functional than polished.
- Intended UI Behavior / Appearance: Loading and error moments feel clearly communicated and elegantly composed.
- Visual / Interaction Design Direction: Calm feedback even during failure or waiting.
- Technical Styling Approach: Reuse feedback and sparse-state systems while defining onboarding- and panel-specific state variants.
- Implementation Steps: Audit loading and error states; unify skeleton and retry UI; align error iconography and copy hierarchy; refine blocking vs inline error presentation.
- Validation Steps: Trigger representative loading and failure states in startup, search, panels, and dialogs.
- Acceptance Criteria: Loading and error states no longer feel like a drop in visual quality.
- Dependencies: Phase 14, Phase 16 sparse-state work.
- Risks / Failure Modes: Over-refined loading screens can feel overproduced and slow.
- Accessibility / Readability Notes: Error messages and retry actions must remain high-contrast and obvious.
- Theme / Styling Notes: Error and loading states must maintain parity across all theme modes.
- Motion / Interaction Notes where relevant: Loading motion should be reduced-motion aware and non-distracting.
- Observability / Diagnostics Notes where relevant: Add loading/error screenshot fixtures.
- Rollback / Safety Notes: Preserve current operational messaging while shells are redesigned.
- References / Context: Loading and error states are integral to whether the product feels complete.
- Example scenarios where useful: A failed workspace load looks composed and actionable instead of like a raw exception surface.

### P16-T05

- Phase ID: P16
- Task ID: P16-T05
- Task Title: Create premium first-action affordances for create/open/recent flows across startup and empty states
- Priority: P1
- Category: Empty State / Onboarding UI
- Atomic Improvements Covered: 60
- Objective: Make the product’s invitation to action feel elegant and confident wherever users encounter a blank surface.
- Why This Matters Now: CTAs are one of the highest-leverage UI opportunities for perceived quality.
- UI Problem Statement: Many create/open/recent affordances exist, but their visual treatment varies and some still use local widget styling.
- User Experience Impact: Early interactions feel less premium and less trustworthy than they should.
- Scope: Startup CTAs, empty explorer CTAs, empty notebook/canvas prompts, recent items, quick-create affordances, first-action layout and copy hierarchy.
- Out of Scope: Action correctness or command routing.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/StartupPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/ExplorerPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/WelcomeTab.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp`
- Related Screens / Components / Surfaces: Startup, welcome, empty explorer, empty notebooks, empty canvases, no-project shells.
- Current UI Behavior / Appearance: CTA surfaces are uneven in button styling, spacing, and explanatory hierarchy.
- Intended UI Behavior / Appearance: Entry actions feel inviting, premium, and visibly connected to the rest of the interface.
- Visual / Interaction Design Direction: Confident action invitations with strong hierarchy and minimal visual strain.
- Technical Styling Approach: Reuse premium button, card, and sparse-state systems to build shared CTA compositions.
- Implementation Steps: Audit create/open/recent surfaces; align CTA button treatments; improve explanatory text and recent-item framing; tune icon-label alignment; reduce local widget hacks.
- Validation Steps: Compare CTAs across startup, empty panel, and welcome surfaces in all major themes.
- Acceptance Criteria: First-action moments feel product-grade and visually coherent everywhere they appear.
- Dependencies: Phase 01, Phase 04, Phase 16 shell work.
- Risks / Failure Modes: Overemphasizing CTAs can make sparse surfaces feel sales-like rather than professional.
- Accessibility / Readability Notes: CTA prominence must not come at the expense of readability or keyboard visibility.
- Theme / Styling Notes: Primary and secondary CTA hierarchy must remain clear across theme modes.
- Motion / Interaction Notes where relevant: CTA hover and reveal states should feel tactile but not flamboyant.
- Observability / Diagnostics Notes where relevant: Add CTA surface screenshots and hover-state captures.
- Rollback / Safety Notes: Preserve current entry-point behavior while visual shells are standardized.
- References / Context: First-action affordances are one of the most noticeable places where premium polish pays off.
- Example scenarios where useful: An empty explorer panel invites file and folder creation with the same level of polish as the startup screen.

### P16-T06

- Phase ID: P16
- Task ID: P16-T06
- Task Title: Add onboarding and sparse-state family galleries plus regression coverage
- Priority: P0
- Category: Cleanup / Consolidation
- Atomic Improvements Covered: 60
- Objective: Keep onboarding, sparse-state, loading, and error-state quality from drifting over time.
- Why This Matters Now: These states appear across many product areas and are easy to under-review.
- UI Problem Statement: The product lacks a comparative board for startup, welcome, sparse states, loading states, and first-action surfaces.
- User Experience Impact: Product-wide polish weakens if sparse and onboarding states regress quietly.
- Scope: Startup, welcome, walkthroughs, empty panels, loading skeletons, no-results states, retry states, CTA compositions.
- Out of Scope: Functional state correctness.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/tests/unit`; `/Users/ryanrentfro/code/markamp/docs/v22_docs`; `/Users/ryanrentfro/code/markamp/src/ui/StartupPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/EmptyPanelState.cpp`
- Related Screens / Components / Surfaces: Entire onboarding and sparse-state family.
- Current UI Behavior / Appearance: These surfaces are reviewed in isolation and often too late.
- Intended UI Behavior / Appearance: Onboarding and sparse-state quality is referenceable and regression-gated.
- Visual / Interaction Design Direction: Mature sparse-state governance.
- Technical Styling Approach: Create fixture-driven screenshot boards for startup, sparse-state, and loading/error families.
- Implementation Steps: Define state fixtures; capture major onboarding and sparse variants; publish comparison boards; add visual regression checks for high-value surfaces.
- Validation Steps: Perturb startup CTA styling or no-results spacing and verify the gallery catches the change.
- Acceptance Criteria: Onboarding and sparse-state polish becomes durable and easier to maintain.
- Dependencies: P16-T01 through P16-T05.
- Risks / Failure Modes: Sparse-state fixtures can become repetitive unless they are clearly grouped by purpose.
- Accessibility / Readability Notes: Include focus-visible, compact-width, and reduced-motion sparse-state captures.
- Theme / Styling Notes: Capture all major sparse-state families in dark, light, and high-contrast themes.
- Motion / Interaction Notes where relevant: Include representative loading motion references where appearance changes materially.
- Observability / Diagnostics Notes where relevant: Store fixture metadata with state type and originating surface family.
- Rollback / Safety Notes: Keep the gallery additive during ongoing sparse-state modernization.
- References / Context: Sparse and onboarding states are part of the interface’s emotional quality, not side concerns.
- Example scenarios where useful: A no-results screen in Search and the startup welcome screen can be reviewed as members of one polished design language.

