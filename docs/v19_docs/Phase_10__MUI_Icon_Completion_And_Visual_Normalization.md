# Phase 10: MUI Icon Completion And Visual Normalization

## Phase Purpose
Finish the MUI migration, remove legacy icon residue, and eliminate icon-driven layout and accessibility inconsistencies across the shell.

## Measurable Outcome
- The icon system has one canonical resolution path.
- Emoji, unicode, and legacy icon aliases are removed from major visible workflows.
- Icon state, spacing, and accessibility labels are consistent across primary surfaces.

## Tasks

### P10-T01
- Phase ID: P10
- Task ID: P10-T01
- Task Class: Foundational
- Task Title: Consolidate The Canonical Icon Resolution Layer
- Priority: Critical
- Objective: Choose and enforce one authoritative icon resolution path for application UI.
- Why This Matters Now: The codebase still has overlapping icon systems, which guarantees migration drift.
- Problem Statement: `IconInventory`, `MuiIconPipeline`, `IconPipeline`, `IconProvider`, and `IconManager` overlap, while fallback behavior still returns legacy IDs or emoji placeholders.
- Scope: command icons, file/tree icons, shell icons, canonical ID resolution, and alias policy.
- Out of Scope: Extension-provided product icon themes beyond adapter boundaries.
- Relevant Full Filepaths:
  - `/Users/ryanrentfro/code/markamp/src/core/IconInventory.cpp`
  - `/Users/ryanrentfro/code/markamp/src/core/MuiIconPipeline.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/IconPipeline.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/IconProvider.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/IconManager.cpp`
- Related Systems / Components: icon resolution, aliasing, manifest loading, cache invalidation, command icons.
- Current Behavior: Multiple layers can resolve icons, and some still fall back to emoji or legacy names.
- Intended Behavior: UI code asks for canonical icon IDs and the same pipeline resolves them everywhere.
- Technical Approach: Pick one canonical runtime path and demote all other resolution logic to compatibility adapters.
- Implementation Steps:
  1. Define the authoritative icon ID scheme and resolver.
  2. Make `IconManager` consume that canonical layer.
  3. Restrict `IconProvider` and `MuiIconPipeline` to compatibility/alias translation if retained.
  4. Add validation that rejects new visible legacy residue in core surfaces.
- Validation Steps:
  1. Render representative icons across activity bar, toolbar, panels, tabs, and file tree.
  2. Verify theme changes still invalidate caches correctly.
- Acceptance Criteria: Core surfaces do not depend on mixed icon resolution paths.
- Dependencies: P02-T01.
- Risks / Failure Modes: Asset-manifest gaps may appear once fallback residue is removed.
- UX Notes: Icon consistency materially affects the perceived quality of every surface.
- Observability / Diagnostics Notes: Add an icon-resolution audit that reports canonical, aliased, and fallback paths.
- Rollback / Safety Notes: Keep compatibility aliases for extension or legacy data, but not as the visible first-party default path.
- References / Context:
  - `IconProvider.cpp` still falls back to emoji category icons.
  - `IconInventory.cpp` returns the legacy ID when no mapping exists.
- Example Scenarios Where Useful:
  - A command icon requested from the palette and the toolbar should resolve through the same canonical ID path.

### P10-T02
- Phase ID: P10
- Task ID: P10-T02
- Task Class: Cleanup
- Task Title: Remove Visible Legacy Icon Residue And Unicode Placeholders
- Priority: High
- Objective: Eliminate remaining emoji and unicode placeholder icons from major user-facing surfaces.
- Why This Matters Now: The migration is visibly incomplete in secondary panels, settings, canvas tools, and some shell affordances.
- Problem Statement: Several surfaces still hardcode emoji or unicode glyphs for icons, which creates visual inconsistency and layout drift.
- Scope: activity and secondary sidebars, canvas tool rail, settings tree/category labels, panel headers, presentation controls, and placeholder shell panels.
- Out of Scope: Rare debug-only or internal developer surfaces that are not user-facing in normal workflows.
- Relevant Full Filepaths:
  - `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/ExplorerPanel.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/StatusBarPanel.cpp`
- Related Systems / Components: sidebars, canvas tool rail, settings categories, status bar, panel headers.
- Current Behavior: Major surfaces still display emoji and unicode glyph placeholders.
- Intended Behavior: All visible first-party iconography comes from the canonical MUI icon set.
- Technical Approach: Inventory residue from code, replace with canonical IDs, and recheck layout metrics afterward.
- Implementation Steps:
  1. Run an icon residue audit across visible UI files.
  2. Replace hardcoded unicode/emoji with canonical icon references.
  3. Adjust layout metrics where icon dimensions change.
  4. Re-review panel and toolbar spacing after replacement.
- Validation Steps:
  1. Inspect activity bar, sidebars, canvas tools, settings, and status bar visually.
  2. Verify no clipped or misaligned controls after icon replacement.
- Acceptance Criteria: Primary workflows no longer expose legacy icon residue.
- Dependencies: P10-T01.
- Risks / Failure Modes: Some placeholder panels may need to be demoted instead of restyled if their icons imply false readiness.
- UX Notes: Complete icon normalization is part of product cohesion, not just a cosmetic sweep.
- Observability / Diagnostics Notes: Save the residue audit output as a CI-visible artifact if possible.
- Rollback / Safety Notes: Leave temporary alias support for serialized legacy IDs, not for live first-party chrome.
- References / Context:
  - `CanvasWorkspacePanel.cpp` still uses unicode glyphs for tools.
  - `LayoutManager.cpp` still registers several sidebar labels with emoji.
- Example Scenarios Where Useful:
  - Switching from explorer to search to canvas should no longer visibly jump between icon languages.

### P10-T03
- Phase ID: P10
- Task ID: P10-T03
- Task Class: Polish
- Task Title: Normalize Icon State Behavior Accessibility Labels And Layout Metrics
- Priority: High
- Objective: Make icon usage consistent for hover, selected, disabled, loading, and accessibility states.
- Why This Matters Now: Replacing the icon asset alone will not fix cohesion if icon semantics and spacing still differ by surface.
- Problem Statement: There is not yet one clear rule set for icon size, padding, semantic color changes, or accessible naming across the shell.
- Scope: icon size tokens, state-specific rendering, tooltip and label consistency, accessible names, and layout regression review.
- Out of Scope: Full theme-specific icon packs.
- Relevant Full Filepaths:
  - `/Users/ryanrentfro/code/markamp/src/core/IconMetricsPolicy.cpp`
  - `/Users/ryanrentfro/code/markamp/src/core/IconValidationGate.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/IconRenderer.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/PanelHeaderIcons.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/FileTypeIconRegistry.cpp`
- Related Systems / Components: icon metrics, validation gate, accessibility labeling, hover/selected states, panel header icons.
- Current Behavior: The codebase has metrics and validation helpers, but usage rules are not yet consistently enforced across the UI.
- Intended Behavior: Icons behave consistently in all states and do not cause layout or accessibility regressions.
- Technical Approach: Define state and metric policies once and make renderers and consumers use them.
- Implementation Steps:
  1. Define canonical icon size and padding tokens per surface class.
  2. Standardize selected, hover, disabled, and busy-state icon treatment.
  3. Ensure icon-bearing controls have clear accessible names or labels.
  4. Add validation gates and a small screenshot or visual regression checklist for major surfaces.
- Validation Steps:
  1. Test keyboard navigation and screen-reader labels on icon-only controls.
  2. Review primary shell surfaces at normal and compact density.
  3. Run icon validation and visual regression checks.
- Acceptance Criteria: Icon behavior is visually and semantically consistent across the product.
- Dependencies: P10-T01, P10-T02.
- Risks / Failure Modes: Overly rigid policies may not fit file-type icons and product icons equally well.
- UX Notes: Icon-only controls must still be legible, discoverable, and accessible.
- Observability / Diagnostics Notes: Extend `IconValidationGate` output to include state and accessibility coverage.
- Rollback / Safety Notes: Where full accessibility naming is blocked, prefer adding visible labels rather than shipping ambiguous icon-only controls.
- References / Context:
  - `IconMetricsPolicy.cpp` and `IconValidationGate.cpp` are the right anchors for enforcement work.
- Example Scenarios Where Useful:
  - An icon-only panel close button, toolbar action, and status item should all expose stable accessible names and consistent hover/selected behavior.
