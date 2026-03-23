# Phase 01 - Visual System Convergence And Premium Standards

## Goal

Turn the existing token, spacing, and typography primitives into a single enforced premium UI system that later phases can safely build on.

## Definition Of Done

- shared spacing, typography, radius, divider, elevation, and interaction-state rules are documented in code and adopted across shell-critical surfaces
- local one-off visual constants are reduced on the highest-traffic paths
- every later UI phase can reference a stable visual baseline instead of inventing local treatments

## Tasks

### Phase ID: V26-P01
### Task ID: V26-P01-T01
### Task Title: Audit And Normalize Shared Layout, Spacing, And Radius Tokens
- Priority: P0
- Category: Visual Language
- Objective: Convert existing spacing and shell metrics into a clearly enforced premium spacing and radius system.
- Why This Matters Now: Surface-level polish will drift again if core rhythm decisions remain optional.
- UI Gap Statement: The repository contains shared layout primitives, but many visible surfaces still apply local margins, padding, and rounded-corner choices.
- User Experience Impact: Users experience a more intentional, calm, and premium interface when spacing and shape language are consistent across surfaces.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/UILayoutTokens.h`, `/Users/ryanrentfro/code/markamp/src/ui/LayoutMetrics.h`, `/Users/ryanrentfro/code/markamp/src/ui/SpacingGrid.h`, `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/ThemeGallery.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp`
- Prior Plan References: `v22` shell/layout foundations, `v25` Phase 07 visible UI closure
- Scope: shared shell spacing, panel spacing, card spacing, control spacing, radius normalization, divider thickness, section gaps
- Out of Scope: behavioral layout rewrites unrelated to visible polish
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/UILayoutTokens.h`, `/Users/ryanrentfro/code/markamp/src/ui/LayoutMetrics.h`, `/Users/ryanrentfro/code/markamp/src/ui/SpacingGrid.h`, `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`
- Related Screens / Components / Surfaces: shell frame, panels, toolbars, settings, dialogs, onboarding surfaces
- Current UI Behavior / Appearance: rhythm varies by surface and some files still hardcode local spacing and panel geometry.
- Intended UI Behavior / Appearance: every major surface uses the same spacing steps, corner families, section spacing, and divider grammar.
- Visual / Interaction Design Direction: restrained premium desktop UI with confident negative space, clean groupings, and deliberate density.
- Technical Styling Approach: extend `UILayoutTokens` and related metrics as the authoritative source; remove local spacing constants from high-traffic surfaces first.
- Implementation Steps:
  1. Inventory local spacing, margin, padding, and corner usages in the shell-critical files.
  2. Consolidate preferred values into shared layout tokens.
  3. Refactor shell-critical surfaces to use those tokens.
  4. Add comments or helper wrappers where adoption patterns are otherwise easy to drift.
- Validation Steps:
  1. Review shell, settings, startup, and command palette side by side.
  2. Resize the app and verify rhythm remains coherent.
  3. Compare light and dark themes for divider and radius consistency.
- Acceptance Criteria: no shell-critical surface uses clearly arbitrary spacing or radius values where a shared token exists; spacing and divider rhythm looks consistent in screenshots across core surfaces.
- Dependencies: none
- Parallelization Notes: can begin immediately and should finish before most other phases.
- Risks / Failure Modes: over-normalizing without considering density differences can make dense surfaces feel bloated.
- Accessibility / Readability Notes: spacing changes must preserve scan speed in dense information views.
- Theme / Styling Notes: divider and radius tokens must remain theme-safe and not depend on hardcoded colors.
- Motion / Interaction Notes where relevant: interaction-state padding should remain stable to avoid jitter.
- Cleanup / Consolidation Notes where relevant: remove redundant local layout constants after adoption.
- Rollback / Safety Notes: keep token changes incremental and validate each adoption set visually.
- References / Context: use shell-critical files as the first convergence target, not low-traffic surfaces.
- Example scenarios where useful: switching from startup to editor to settings should feel like moving within one product, not between unrelated surfaces.

### Phase ID: V26-P01
### Task ID: V26-P01-T02
### Task Title: Normalize Typography Hierarchy Across Shell And Interior Surfaces
- Priority: P0
- Category: Typography
- Objective: Create a coherent typography hierarchy for titles, section labels, metadata, control labels, captions, and code-adjacent UI.
- Why This Matters Now: Typography inconsistency is one of the main remaining causes of “custom app” feel instead of premium product feel.
- UI Gap Statement: The repo has `TypographyScale`, but several surfaces still use local font sizing, weight, and emphasis patterns.
- User Experience Impact: Better hierarchy improves orientation, readability, and perceived polish on every screen.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/TypographyScale.h`, `/Users/ryanrentfro/code/markamp/src/ui/ThemeGallery.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/StartupPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp`
- Prior Plan References: `v22` typography and shell polish work, `v25` visible UI closure
- Scope: title levels, section headers, row labels, help text, empty-state copy, command-palette text hierarchy
- Out of Scope: editor content fonts and syntax colors, which are handled later
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/TypographyScale.h`, `/Users/ryanrentfro/code/markamp/src/ui/StartupPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/ThemeGallery.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp`
- Related Screens / Components / Surfaces: startup, settings, command palette, theme gallery, dialogs, panel headers
- Current UI Behavior / Appearance: text emphasis patterns vary by file and some headings rely on local font construction rather than shared style intent.
- Intended UI Behavior / Appearance: every surface follows a consistent hierarchy with premium restraint and predictable text roles.
- Visual / Interaction Design Direction: crisp desktop typography with strong title discipline and low-noise metadata styling.
- Technical Styling Approach: promote shared typography roles and refactor local `SetFont` usage toward those roles.
- Implementation Steps:
  1. Define named typography roles mapped to `TypographyScale`.
  2. Catalog major heading and metadata uses in high-traffic surfaces.
  3. Replace local font constructions with role-based styling helpers.
  4. Tune vertical spacing to preserve hierarchy after font normalization.
- Validation Steps:
  1. Compare settings, startup, theme gallery, and dialogs in one review session.
  2. Confirm long labels and translated strings still wrap cleanly.
  3. Validate readability at common desktop scaling factors.
- Acceptance Criteria: shell and settings surfaces expose a recognizable typographic ladder with no obviously ad hoc emphasis patterns.
- Dependencies: V26-P01-T01
- Parallelization Notes: can run with token work if the role naming is agreed first.
- Risks / Failure Modes: aggressive hierarchy flattening can reduce discoverability.
- Accessibility / Readability Notes: preserve legibility for low-vision users; avoid tiny metadata text.
- Theme / Styling Notes: typography roles should be independent from theme colors.
- Motion / Interaction Notes where relevant: none
- Cleanup / Consolidation Notes where relevant: remove file-local font helper duplication once shared roles land.
- Rollback / Safety Notes: keep old and new screenshots during migration to catch regression in hierarchy.
- References / Context: prioritize surfaces with the highest amount of local `SetFont` usage.
- Example scenarios where useful: the command palette and settings page should both communicate primary action, supporting detail, and metadata with the same hierarchy rules.

### Phase ID: V26-P01
### Task ID: V26-P01-T03
### Task Title: Standardize Color, Border, Divider, Elevation, And Interaction-State Language
- Priority: P0
- Category: Color / Theme
- Objective: Define one visual grammar for surfaces, borders, hover, selected, pressed, focused, and disabled states across the app shell.
- Why This Matters Now: Premium UI quality depends on interaction-state consistency as much as static styling.
- UI Gap Statement: Theme tokens exist, but local color and border decisions still diverge across shell, gallery, settings, and panel surfaces.
- User Experience Impact: Users gain stronger trust, clarity, and tactile confidence when interactive states behave visually the same way everywhere.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/core/ThemeTokens.h`, `/Users/ryanrentfro/code/markamp/src/core/ThemeEngine.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/TabBar.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/StatusBarPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp`
- Prior Plan References: `v22` theme consistency work, `v25` syntax/theme/icon closure
- Scope: surface layers, dividers, hover states, selected states, focus treatment hooks, disabled contrast baselines
- Out of Scope: language-specific syntax highlighting palettes
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/ThemeTokens.h`, `/Users/ryanrentfro/code/markamp/src/core/ThemeEngine.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/TabBar.cpp`
- Related Screens / Components / Surfaces: toolbar, tabs, breadcrumbs, panels, settings rows, dialogs, notifications
- Current UI Behavior / Appearance: some surfaces already use rich tokenization while others still rely on local color choices or hardcoded paint values.
- Intended UI Behavior / Appearance: the entire product shares one surface stack, one divider language, and one interaction-state vocabulary.
- Visual / Interaction Design Direction: subtle but confident states with premium restraint, strong clarity, and no noisy over-highlighting.
- Technical Styling Approach: map all common state treatments to theme tokens and helper paint utilities; remove hardcoded surface colors in shell-critical surfaces.
- Implementation Steps:
  1. Audit high-traffic surfaces for hardcoded or locally derived state colors.
  2. Add missing state tokens only where current vocabulary is insufficient.
  3. Refactor shell-critical components to use shared state painting helpers.
  4. Document state usage conventions in code comments near the token definitions.
- Validation Steps:
  1. Hover, select, press, and disable controls across shell surfaces in both themes.
  2. Compare screenshots of tabs, toolbar items, breadcrumbs, and settings rows.
  3. Verify border contrast and elevation remain subtle but visible.
- Acceptance Criteria: hardcoded shell-state colors are removed from critical surfaces and shared interaction states read consistently across the app.
- Dependencies: V26-P01-T01
- Parallelization Notes: should complete before large-scale surface polish.
- Risks / Failure Modes: too many new tokens can recreate the drift problem; prefer reuse over token proliferation.
- Accessibility / Readability Notes: state differentiation must not depend only on color.
- Theme / Styling Notes: keep light and dark parity in scope while defining state conventions.
- Motion / Interaction Notes where relevant: state transitions added later should reinforce, not replace, static state clarity.
- Cleanup / Consolidation Notes where relevant: retire old helper color derivations after state helpers are adopted.
- Rollback / Safety Notes: compare token diffs carefully to avoid unintended theme-wide regressions.
- References / Context: this task underpins visual trust across every later phase.
- Example scenarios where useful: toolbar hover, tab active, settings row selection, and breadcrumb focus should all feel related.
