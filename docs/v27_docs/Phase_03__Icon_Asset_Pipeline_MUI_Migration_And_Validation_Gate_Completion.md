# Phase 03 - Icon Asset Pipeline MUI Migration And Validation Gate Completion

## Goal

Finish the canonical icon rendering and validation spine so the rest of the redesign can migrate with confidence and without reintroducing mixed icon systems.

## Definition Of Done

- MUI-led icon resolution is the canonical default for general UI iconography
- legacy icon paths are aliased temporarily but blocked from permanent use
- validation gates exist for banned icon residues and missing migration coverage

## Tasks

### Phase ID: V27-P03
### Task ID: V27-P03-T01
### Task Title: Make The MUI Led Pipeline The Canonical General UI Icon Path
- Priority: P0
- Category: Icon System
- Objective: Establish `MuiIconPipeline` and its associated mapping layers as the canonical path for general UI iconography.
- Why This Matters Now: Full-surface redesign will fragment again if the product keeps multiple “first-class” icon systems alive.
- Visual Gap Statement: The repo currently has MUI migration seams, a manifest-driven pipeline, and a custom inline SVG registry operating side by side.
- User Experience Impact: One canonical icon path produces better consistency, state behavior, and long-term visual stability.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/core/MuiIconPipeline.h`, `/Users/ryanrentfro/code/markamp/src/core/MuiIconMigration.h`, `/Users/ryanrentfro/code/markamp/src/ui/IconManager.h`, `/Users/ryanrentfro/code/markamp/src/ui/IconPipeline.h`, `/Users/ryanrentfro/code/markamp/src/ui/IconLibrary.cpp`
- Prior Plan References: `v18` Phase 10, `v19` Phase 10, `v20` Phase 09, `v26` Phase 16
- Scope: canonical pipeline selection for shell, controls, menus, tabs, panels, dialogs, settings, notifications, feedback, command surfaces
- Out of Scope: file-type and specialty custom-adapter cases that need non-MUI coverage
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/MuiIconPipeline.h`, `/Users/ryanrentfro/code/markamp/src/core/MuiIconMigration.h`, `/Users/ryanrentfro/code/markamp/src/ui/IconManager.h`, `/Users/ryanrentfro/code/markamp/src/ui/IconPipeline.h`, `/Users/ryanrentfro/code/markamp/src/ui/IconLibrary.cpp`
- Related Screens / Components / Surfaces: shell chrome, toolbar, menus, tabs, panel headers, settings controls, dialogs, notifications
- Current Visual Behavior / Appearance: different surfaces can still be fed by different icon systems with different semantics and rendering assumptions.
- Intended Visual Behavior / Appearance: general UI iconography resolves through one canonical MUI-led path, with specialty adapters explicitly separated.
- Icon / Visual Design Direction: one crisp modern icon family for general UI, not several overlapping families.
- Technical Styling Approach: redefine ownership boundaries between MUI, manifest, and custom specialty icons, then route general UI through the canonical path.
- Implementation Steps:
  1. Define which icon roles are canonical MUI roles versus custom specialty roles.
  2. Route general UI icon resolution through the MUI-led pipeline.
  3. Keep adapters only where file types or specialty artifacts require them.
  4. Document the canonical ownership model.
- Validation Steps:
  1. Verify shell, toolbar, menu, tab, panel, and settings icons all resolve through the canonical path.
  2. Check icon-state rendering consistency across those surfaces.
  3. Confirm specialty file and artifact icons are still supported through explicit adapter rules.
- Acceptance Criteria: there is one explicit canonical icon path for general UI and it is used across all redesigned surfaces.
- Dependencies: Phase 01 completion
- Parallelization Notes: foundational and should finish before broad icon usage migration.
- Risks / Failure Modes: treating all icons as MUI can break valid specialty cases; keep adapters explicit, not implicit.
- Accessibility / Readability Notes: canonical path must support accessible labels and stable states.
- Theme / Styling Notes: canonical path must be fully theme-aware and state-aware.
- Motion / Interaction Notes where relevant: stateful icon transitions must be compatible with the canonical path.
- Cleanup / Consolidation Notes where relevant: mark old parallel paths for retirement once migration is complete.
- Rollback / Safety Notes: preserve a compatibility layer while migrating surface by surface, but do not preserve multiple permanent defaults.
- References / Context: the repo already calls `MuiIconPipeline` unified and MUI-aware; `v27` makes that reality rather than aspiration.
- Example scenarios where useful: toolbar “save”, tab “close”, menu “settings”, and panel “filter” should all resolve through the same canonical icon path.

### Phase ID: V27-P03
### Task ID: V27-P03-T02
### Task Title: Expand Icon Inventory, Migration Tracking, And Validation Gates To Full Product Coverage
- Priority: P0
- Category: Icon System
- Objective: Upgrade the current inventory and validation files from partial migration helpers into full-product icon-governance infrastructure.
- Why This Matters Now: The redesign needs enforcement, not only intent, or legacy residue will reappear.
- Visual Gap Statement: current built-in mappings and migration categories are too small for full visible-product transformation.
- User Experience Impact: Strong validation keeps the visual redesign coherent as implementation spreads across many surfaces.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/core/IconInventory.cpp`, `/Users/ryanrentfro/code/markamp/src/core/MuiIconMigration.h`, `/Users/ryanrentfro/code/markamp/src/core/IconValidationGate.h`, `/Users/ryanrentfro/code/markamp/src/core/IconSemanticMapper.h`
- Prior Plan References: earlier icon migration phases and `v26` icon cleanup
- Scope: full-product icon inventory coverage, surface coverage expansion, banned legacy IDs, residue reporting, migration completeness metrics
- Out of Scope: visual restyling of surfaces themselves
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/IconInventory.cpp`, `/Users/ryanrentfro/code/markamp/src/core/MuiIconMigration.h`, `/Users/ryanrentfro/code/markamp/src/core/IconValidationGate.h`, `/Users/ryanrentfro/code/markamp/src/core/IconSemanticMapper.h`
- Related Screens / Components / Surfaces: all icon-bearing surfaces
- Current Visual Behavior / Appearance: inventory and validation exist, but current mapping breadth is too limited to govern the full redesign.
- Intended Visual Behavior / Appearance: icon governance infrastructure covers the entire visible app and actively prevents fallback drift.
- Icon / Visual Design Direction: systematic replacement with measurable completeness, not best-effort cleanup.
- Technical Styling Approach: make icon inventory, residue reporting, and validation first-class release criteria for the redesign effort.
- Implementation Steps:
  1. Expand inventory roles and migration surfaces to full product coverage.
  2. Add all visible legacy icon IDs, glyph paths, and icon-char APIs to banned or transitional sets.
  3. Record residue by surface and severity.
  4. Define migration-complete criteria by surface class.
- Validation Steps:
  1. Run inventory and residue review against shell, panels, editor-adjacent UI, notebook, canvas, settings, and dialogs.
  2. Verify banned-ID coverage includes emoji/unicode stand-ins and icon-char APIs on the visible path.
  3. Confirm migration metrics are meaningful per surface.
- Acceptance Criteria: icon governance files fully represent the visible product and can be used as actual enforcement tools during implementation.
- Dependencies: V27-P03-T01
- Parallelization Notes: can run while actual surface migrations begin, but the structure must exist first.
- Risks / Failure Modes: too-weak gates will let old icon paths survive; too-rigid gates may block valid specialty cases if adapters are not documented.
- Accessibility / Readability Notes: governance should require labels and state support for actionable icons.
- Theme / Styling Notes: validation should cover theme compatibility and missing state coverage where relevant.
- Motion / Interaction Notes where relevant: governance should note interactive-state expectations for icons.
- Cleanup / Consolidation Notes where relevant: record every legacy system that is scheduled for removal in Phase 20.
- Rollback / Safety Notes: keep transitional aliases while surface migrations are incomplete, but gate new usage immediately.
- References / Context: `IconValidationGate` already bans `emoji_placeholder`; `v27` should broaden that concept aggressively.
- Example scenarios where useful: using an emoji fallback in a new panel header should fail the redesign gate instead of silently shipping.

### Phase ID: V27-P03
### Task ID: V27-P03-T03
### Task Title: Retire Icon Char And Glyph Fallback APIs From Visible Surface Ownership
- Priority: P0
- Category: Icon System
- Objective: remove visible-surface dependence on icon chars, emoji prefixes, and text-rendered glyph icons.
- Why This Matters Now: No amount of surface redesign will feel complete if icons still enter the UI as text glyphs.
- Visual Gap Statement: visible icon ownership still leaks through `GetIconChar`, glyph-returning resolvers, and `wxStaticText` icon labels.
- User Experience Impact: Removing glyph icon paths is a direct quality upgrade and eliminates major inconsistency.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/SidebarPanelRegistry.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/FileIconResolver.h`, `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/IconProvider.h`
- Prior Plan References: `v22` icon cleanup, `v26` Phase 16
- Scope: panel icon chars, file icon chars, emoji category fallbacks, glyph-based status and panel prefixes on the visible path
- Out of Scope: text content features that intentionally display emoji as user content
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/SidebarPanelRegistry.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/FileIconResolver.h`, `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/IconProvider.h`, `/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/StatusBarPanel.cpp`
- Related Screens / Components / Surfaces: sidebars, panel headers, file pickers, command palette, settings, status bar, canvas tools
- Current Visual Behavior / Appearance: several visible surfaces still derive icons from strings that are rendered as glyph text.
- Intended Visual Behavior / Appearance: all visible icons are drawn via the canonical icon system rather than through text glyphs.
- Icon / Visual Design Direction: true iconography, not decorated text.
- Technical Styling Approach: replace icon-char APIs with semantic icon IDs and canonical renderer access.
- Implementation Steps:
  1. Enumerate all visible-surface icon-char and glyph APIs.
  2. Replace them with semantic icon IDs and canonical rendering access.
  3. Update call sites to stop treating icons as text.
  4. Add validation that blocks reintroduction on the visible path.
- Validation Steps:
  1. Search for `GetIconChar`, glyph-returning resolvers, and emoji prefixes after migration.
  2. Inspect sidebars, panel headers, file lists, settings, and status surfaces.
  3. Confirm no visible icon enters the UI through `wxStaticText` glyph labels except intentional text content.
- Acceptance Criteria: visible-surface icon ownership no longer depends on glyph strings or icon-char APIs.
- Dependencies: V27-P03-T01, V27-P03-T02
- Parallelization Notes: can migrate surface by surface after the canonical path is set.
- Risks / Failure Modes: removing glyph paths too early can leave surfaces icon-less if replacement hooks are not ready.
- Accessibility / Readability Notes: ensure glyph removal does not remove accessible labels or discoverability.
- Theme / Styling Notes: canonical replacement must preserve correct theme-aware coloring.
- Motion / Interaction Notes where relevant: migrated icons must still participate in hover/active/disabled states.
- Cleanup / Consolidation Notes where relevant: this is a major source of Phase 20 retirement work.
- Rollback / Safety Notes: keep temporary adapter shims until all high-traffic surfaces are migrated.
- References / Context: `LayoutManager.cpp` alone still shows multiple emoji-based sidebar/panel registrations and `wxStaticText` icon labels.
- Example scenarios where useful: a panel header should render a proper icon asset, not a font glyph or emoji prefix.
