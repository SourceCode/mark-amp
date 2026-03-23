# Phase 03 - Menus Toolbars Command Palette And Action Surfaces

## Goal

Make every top-level action surface feel deliberate, premium, and consistent in spacing, grouping, icon/text balance, and transient interaction quality.

## Definition Of Done

- toolbars, command palette, menus, and context surfaces share one action-surface grammar
- grouping, hover, pressed, selected, and overflow behavior are visually consistent
- no visible placeholder styling remains on primary command entry points

## Tasks

### Phase ID: V26-P03
### Task ID: V26-P03-T01
### Task Title: Refine Primary Toolbar Grouping, Density, And Interaction States
- Priority: P0
- Category: Toolbar UI
- Objective: Upgrade the main toolbar from a strong custom surface to a truly premium command strip with tighter grouping logic and better tactile states.
- Why This Matters Now: The toolbar is one of the most frequently scanned areas in the product.
- UI Gap Statement: `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp` is custom and advanced, but still carries significant local styling complexity and likely density drift.
- User Experience Impact: A refined toolbar improves confidence, scannability, and the perceived quality of all editing workflows.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/UILayoutTokens.h`, `/Users/ryanrentfro/code/markamp/src/core/ThemeTokens.h`
- Prior Plan References: `v22` toolbar and command surface polish, `v25` UI closure
- Scope: toolbar button spacing, group separators, icon sizing, text balance, hover/pressed/selected styling, overflow affordances
- Out of Scope: adding new commands
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/UILayoutTokens.h`
- Related Screens / Components / Surfaces: main toolbar, section toolbars, notebook toolbar if shared patterns apply
- Current UI Behavior / Appearance: custom toolbar styling exists but still appears partly hand-tuned and may not yet share full system rules with other action surfaces.
- Intended UI Behavior / Appearance: the toolbar feels clean, balanced, premium, and optically aligned with tabs, menus, and command surfaces.
- Visual / Interaction Design Direction: low-noise control strip with precise grouping and restrained feedback.
- Technical Styling Approach: replace local constants with shared action-surface metrics and state helpers.
- Implementation Steps:
  1. Audit toolbar spacing, button hit areas, and group separators.
  2. Normalize icon and label alignment.
  3. Standardize hover, pressed, and selected painting with shared helpers.
  4. Validate overflow and narrow-width behavior.
- Validation Steps:
  1. Compare toolbar states in light and dark themes.
  2. Hover and press across primary groups.
  3. Resize the shell to validate crowding and overflow behavior.
- Acceptance Criteria: toolbar grouping and interaction states align with the app’s shared visual language and no control feels visually orphaned.
- Dependencies: V26-P01-T01, V26-P01-T03
- Parallelization Notes: can begin early with shell work.
- Risks / Failure Modes: too much separation can fragment the toolbar; too little can make it unreadable.
- Accessibility / Readability Notes: preserve clear labels and state visibility.
- Theme / Styling Notes: state contrast must remain subtle but unambiguous.
- Motion / Interaction Notes where relevant: any hover animation added later should reinforce the same visual grouping.
- Cleanup / Consolidation Notes where relevant: remove toolbar-local styling branches that duplicate shared action-state logic.
- Rollback / Safety Notes: keep command affordances stable while visual tuning changes.
- References / Context: the toolbar should set the visual standard for other action surfaces.
- Example scenarios where useful: switching from editing to notebook work should not make the toolbar feel like a different subsystem.

### Phase ID: V26-P03
### Task ID: V26-P03-T02
### Task Title: Finish Command Palette Presentation, Search Rhythm, And Empty Results UX
- Priority: P0
- Category: Menu UI
- Objective: Turn the command palette into a premium transient command surface with excellent hierarchy, spacing, and empty-result behavior.
- Why This Matters Now: The command palette is one of the clearest signals of product sophistication in an IDE-style application.
- UI Gap Statement: `/Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp` still contains explicit empty-state polish notes and local presentation logic.
- User Experience Impact: A refined command palette improves discoverability, speed, and overall perceived product maturity.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/ThemedDialog.cpp`
- Prior Plan References: `v22` command palette UI work, `v25` shell and UI closure
- Scope: palette spacing, search field hierarchy, result row styling, metadata, section headers, empty state, keyboard-highlight visuals
- Out of Scope: command ranking logic changes
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/ThemedDialog.cpp`
- Related Screens / Components / Surfaces: command palette, quick-open style pickers, transient command UIs
- Current UI Behavior / Appearance: functional palette with local styling and an unfinished empty-state presentation direction.
- Intended UI Behavior / Appearance: premium command surface with crisp hierarchy, elegant empty/no-match states, and strong keyboard focus visibility.
- Visual / Interaction Design Direction: floating command lens with strong typography, subtle depth, and low-noise emphasis.
- Technical Styling Approach: align palette structure with shared overlay and action-surface tokens; extract row-state styling helpers.
- Implementation Steps:
  1. Normalize palette frame, padding, and search-row spacing.
  2. Refine result row typography and metadata balance.
  3. Implement polished empty and no-results states.
  4. Align highlight and selection states with shared conventions.
- Validation Steps:
  1. Test short, long, and no-match queries.
  2. Navigate via keyboard and verify focus clarity.
  3. Compare with dialogs and menus for transient-surface consistency.
- Acceptance Criteria: the command palette feels premium in both populated and empty states and matches the app’s transient-surface language.
- Dependencies: V26-P01-T02, V26-P02-T02
- Parallelization Notes: can run in parallel with toolbar polish after overlay surface rules are stable.
- Risks / Failure Modes: over-styling could hurt scan speed or keyboard efficiency.
- Accessibility / Readability Notes: selected row state must remain highly legible.
- Theme / Styling Notes: empty-state art and metadata must remain contrast-safe across themes.
- Motion / Interaction Notes where relevant: palette opening and row highlight motion should be minimal and responsive.
- Cleanup / Consolidation Notes where relevant: remove palette-local state paint duplication once shared helpers exist.
- Rollback / Safety Notes: keep the fast interaction path intact while adjusting visuals.
- References / Context: command palette quality should now exceed generic IDE defaults.
- Example scenarios where useful: a no-results query should still feel like a designed state, not an unhandled absence.

### Phase ID: V26-P03
### Task ID: V26-P03-T03
### Task Title: Unify Menus, Context Menus, And Overflow Action Surfaces
- Priority: P1
- Category: Menu UI
- Objective: Bring all menu-like surfaces into one polished presentation family.
- Why This Matters Now: Menu inconsistency is especially visible in a desktop app with many entry points.
- UI Gap Statement: Menus, overflow surfaces, and context actions risk drifting because they are often implemented incrementally and locally.
- User Experience Impact: Consistent menus reduce friction and make the app feel intentional rather than assembled feature by feature.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/ProblemsPanel.cpp`
- Prior Plan References: `v22` menu and action-surface work, `v25` visible UI closure
- Scope: menu padding, row height, accelerator alignment, submenu affordance, icon alignment, overflow-menu styling
- Out of Scope: changing command availability logic
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/ProblemsPanel.cpp`
- Related Screens / Components / Surfaces: app menus, context menus, overflow buttons, panel action menus
- Current UI Behavior / Appearance: menu-like surfaces likely inherit different spacing and alignment conventions across implementations.
- Intended UI Behavior / Appearance: all menu rows align visually, accelerator columns read cleanly, and overflow surfaces feel like first-class UI.
- Visual / Interaction Design Direction: disciplined desktop menu design with refined alignment and restrained emphasis.
- Technical Styling Approach: define shared menu metrics and state rules; apply them to custom and semi-custom menu surfaces.
- Implementation Steps:
  1. Audit menu and overflow entry points.
  2. Define row metrics, accelerator spacing, and submenu indicators.
  3. Update custom menu-like surfaces to match.
  4. Verify parity with command palette and toolbar overflow.
- Validation Steps:
  1. Open several menus and compare row height and alignment.
  2. Check accelerator readability and icon spacing.
  3. Verify hover and selected states are consistent.
- Acceptance Criteria: menus, context menus, and overflow surfaces share one coherent presentation language.
- Dependencies: V26-P01-T01, V26-P01-T03
- Parallelization Notes: can run after primary action-surface conventions are settled.
- Risks / Failure Modes: forcing one layout onto all menus may clash with platform-native constraints.
- Accessibility / Readability Notes: keep menu scanning fast and keyboard cues visible.
- Theme / Styling Notes: separators and highlights must read clearly but softly in both themes.
- Motion / Interaction Notes where relevant: menu opening motion should stay platform-appropriate and subtle.
- Cleanup / Consolidation Notes where relevant: remove one-off menu spacing rules from panel implementations.
- Rollback / Safety Notes: preserve platform compliance where native behavior is expected.
- References / Context: this task should improve custom menus without fighting platform-native menu behavior.
- Example scenarios where useful: a toolbar overflow menu and a panel context menu should feel related even if implemented differently.
