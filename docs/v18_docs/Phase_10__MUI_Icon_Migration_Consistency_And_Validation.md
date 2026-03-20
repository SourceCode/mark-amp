# Phase 10: MUI Icon Migration Consistency And Validation

## Phase Goal
Replace the fragmented icon stack with a single MUI-based system that is consistent, themeable, and maintainable.

## Tasks
### Task 1
- Phase ID: P10
- Task ID: P10-T01
- Task Title: Build Complete Icon Inventory And Canonical MUI Mapping
- Objective: Produce the authoritative map from every current icon call site and asset to its MUI replacement.
- Problem Statement: Current icon usage spans `IconLibrary`, `IconPipeline`, `IconProvider`, file resolvers, placeholder IDs, emoji, and Lucide assets.
- Scope: Activity bar, toolbar, menus, status bar, file tree, panels, context menus, dialogs, empty states, command icons, file/folder icons.
- Out of Scope: Runtime replacement in this task.
- Relevant Full Filepaths:
  - `/Users/ryanrentfro/code/markamp/src/ui/IconLibrary.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/IconManager.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/IconPipeline.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/FileTypeIconResolver.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/FileTreeCtrl.cpp`
- Related Systems / Components: icon pipeline, command icons, file/folder icons, legacy icon registry.
- Current Behavior: Icon usage is fragmented and includes placeholder names and noncanonical sources.
- Intended Behavior: A single mapping document exists from legacy names/usages to MUI IDs/assets.
- Technical Approach: Generate a code-backed inventory and classify by semantic meaning, not by old asset name.
- Implementation Steps:
  1. Enumerate icon names and direct asset loads.
  2. Group by semantic role.
  3. Choose MUI replacements.
  4. Identify cases needing custom adapters or composite icons.
- Edge Cases / Failure Modes: One legacy icon serving multiple meanings; missing MUI equivalent; extension-contributed icons.
- UX Considerations: Semantic consistency matters more than visual one-to-one matching.
- Dependencies: P01-T05.
- Validation Steps: Review every visible icon category against the mapping.
- Acceptance Criteria: No icon migration work proceeds without canonical mapping.
- Rollback / Safety Notes: Documentation only.
- References / Context: Prior v16 docs targeted material-style icons but current code still contains mixed systems.
- Example interactions or usage scenarios where helpful: File-tree chevrons, activity icons, and settings icons should all have mapped MUI targets.

### Task 2
- Phase ID: P10
- Task ID: P10-T02
- Task Title: Collapse Runtime Icon Rendering Onto One MUI Aware Pipeline
- Objective: Ensure all icon rendering goes through one canonical pipeline with backward-compatible legacy aliases during migration.
- Problem Statement: Multiple runtime icon render paths block consistent sizing, tinting, caching, and asset replacement.
- Scope: Icon manager, icon pipeline, registry compatibility layer, file/folder/command icon resolution.
- Out of Scope: Final removal of all legacy aliases until migration is complete.
- Relevant Full Filepaths:
  - `/Users/ryanrentfro/code/markamp/src/ui/IconManager.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/IconPipeline.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/IconRegistry.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/IconProvider.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/IconManifest.cpp`
- Related Systems / Components: icon cache, icon manifest, rendering, theme change invalidation.
- Current Behavior: Pipeline and legacy registry both exist and callers still depend on both.
- Intended Behavior: One MUI-capable pipeline renders all icons; legacy names resolve through aliases only.
- Technical Approach: Make `IconManager` the exclusive runtime entry and route all other paths into it.
- Implementation Steps:
  1. Add canonical MUI asset source and manifest schema.
  2. Convert legacy entry points into adapters.
  3. Standardize size/color/state handling.
  4. Add alias map for old icon names.
- Edge Cases / Failure Modes: Missing asset causing blank bitmap; command/file/folder icons needing separate manifest rules.
- UX Considerations: Replacement must not shift hit targets or spacing unexpectedly.
- Dependencies: P10-T01.
- Validation Steps: Render icons from each category through the new pipeline and verify output.
- Acceptance Criteria: There is one real icon runtime path.
- Rollback / Safety Notes: Keep alias fallback during staged rollout.
- References / Context: `IconManager` already attempts a hybrid model that should be rationalized.
- Example interactions or usage scenarios where helpful: An activity bar icon and a file tree icon should share the same tint/cache/runtime rules.

### Task 3
- Phase ID: P10
- Task ID: P10-T03
- Task Title: Replace Visible Application Icons With MUI Assets
- Objective: Migrate all first-party visible UI icons to MUI.
- Problem Statement: The application currently uses legacy SVGs, Lucide assets, emojis, and placeholders.
- Scope: Activity bar, toolbar, panel headers, tab affordances, status bar icons, dialogs, file tree chevrons and file/folder assets, command icons.
- Out of Scope: Third-party extension-owned icons beyond compatibility support.
- Relevant Full Filepaths:
  - `/Users/ryanrentfro/code/markamp/src/ui/ActivityBar.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/PanelHeaderIcons.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/StatusBarPanel.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/FileTreeCtrl.cpp`
- Related Systems / Components: shell chrome, file tree, panel headers, status bar, command surfaces.
- Current Behavior: Mixed iconography causes visual and semantic inconsistency.
- Intended Behavior: All first-party UI icons come from the same MUI system with consistent semantics.
- Technical Approach: Replace by semantic category in batches and verify layout after each batch.
- Implementation Steps:
  1. Migrate navigation/shell icons.
  2. Migrate file tree and command icons.
  3. Migrate panel/status/dialog icons.
  4. Remove placeholder glyph usage.
- Edge Cases / Failure Modes: Layout shifts, clipping, mismatched stroke/fill weight, ambiguous icon meaning.
- UX Considerations: Iconography must improve clarity, not just swap assets.
- Dependencies: P10-T02.
- Validation Steps: Manual UI sweep across all major workbench surfaces.
- Acceptance Criteria: No first-party visible control relies on legacy or placeholder icon sources.
- Rollback / Safety Notes: Migrate by surface cluster and keep alias fallback until verified.
- References / Context: `FileTreeCtrl.cpp` currently references placeholder icon comments and fallback IDs.
- Example interactions or usage scenarios where helpful: Activity bar, explorer rows, and panel controls should all read as one coherent family.

### Task 4
- Phase ID: P10
- Task ID: P10-T04
- Task Title: Standardize Icon Sizing Alignment States And Theme Compatibility
- Objective: Make icon behavior consistent across hover, active, disabled, selected, error, and high-DPI states.
- Problem Statement: Mixed icon sources make it hard to preserve alignment, tinting, and state styling.
- Scope: Component metrics, icon size tokens, padding/alignment, hover/active/disabled colors, high-DPI rendering, contrast.
- Out of Scope: Broad component redesign unrelated to icons.
- Relevant Full Filepaths:
  - `/Users/ryanrentfro/code/markamp/src/ui/ComponentSizeResolver.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/LayoutMetrics.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/ActivityBar.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/ThemeAwareWindow.cpp`
- Related Systems / Components: component metrics, theme engine, icon pipeline, shell controls.
- Current Behavior: Icon metrics and state treatment vary by control and source path.
- Intended Behavior: Icons follow one metric/state system across the app.
- Technical Approach: Define shared icon metrics and state color application rules, then apply them by component family.
- Implementation Steps:
  1. Define icon tokens for size and state colors.
  2. Update control families to consume them.
  3. Verify alignment and hit target preservation.
  4. Validate theme contrast and high-DPI rendering.
- Edge Cases / Failure Modes: Small controls clipping icons; theme colors too low contrast; retina scaling artifacts.
- UX Considerations: Icons should look sharp, centered, and semantically stable.
- Dependencies: P10-T03.
- Validation Steps: Compare across normal/hover/disabled/active states and multiple DPI scales.
- Acceptance Criteria: Icon presentation is consistent and theme-compatible across all major controls.
- Rollback / Safety Notes: Prefer shared metric tokens over per-control overrides.
- References / Context: Required to make MUI migration feel like a product improvement rather than an asset swap.
- Example interactions or usage scenarios where helpful: A disabled toolbar icon and a disabled panel action icon should follow the same visual rules.

### Task 5
- Phase ID: P10
- Task ID: P10-T05
- Task Title: Remove Legacy Icon Debt And Add Migration Validation Gates
- Objective: Finish the migration by cleaning legacy code paths and adding validation that prevents regressions.
- Problem Statement: Without cleanup and checks, the codebase will drift back into mixed icon usage.
- Scope: Remove dead icon assets/code paths, add tests or static checks for legacy icon usage, document extension compatibility.
- Out of Scope: Policing third-party extension asset choices beyond API boundaries.
- Relevant Full Filepaths:
  - `/Users/ryanrentfro/code/markamp/src/ui/IconLibrary.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/IconProvider.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/IconRegistry.cpp`
  - `/Users/ryanrentfro/code/markamp/resources/icons/lucide/folder.svg`
  - `/Users/ryanrentfro/code/markamp/docs/v16_docs/Phase_02__Material_Icon_Set_Licensing_And_Architecture.md`
- Related Systems / Components: build assets, runtime icon aliases, test/validation strategy.
- Current Behavior: Legacy icon paths remain easy to reuse accidentally.
- Intended Behavior: MUI is enforced as the canonical first-party icon source.
- Technical Approach: Delete or quarantine obsolete paths and add validation checks in CI or local tooling.
- Implementation Steps:
  1. Remove obsolete first-party icon code/assets after migration.
  2. Add static checks for banned legacy icon IDs/assets.
  3. Add visual or smoke validation coverage for key surfaces.
  4. Document extension-facing compatibility boundaries.
- Edge Cases / Failure Modes: Deleting assets still used by extension compatibility adapters; false positives in icon checks.
- UX Considerations: Validation should protect consistency without slowing normal iteration excessively.
- Dependencies: P10-T03 and P10-T04.
- Validation Steps: Run icon usage checks and key UI smoke tests.
- Acceptance Criteria: Legacy first-party icon debt is removed or isolated behind deliberate adapters.
- Rollback / Safety Notes: Remove old assets only after alias coverage and smoke validation are in place.
- References / Context: The repository still contains Lucide assets and material-style planning from prior iterations.
- Example interactions or usage scenarios where helpful: A new toolbar action added later should fail validation if it tries to use a deprecated legacy icon path.

