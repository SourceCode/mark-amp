# Phase 01 - Canonical Icon System Audit And Replacement Spine

## Goal

Establish one canonical icon system for the entire product and define the exact replacement path for every visible icon source and usage site.

## Definition Of Done

- every visible icon source is inventoried and categorized
- a canonical MUI-led icon strategy is defined with custom-adapter exceptions only where necessary
- visible icon residue and banned legacy paths are explicitly enumerated for migration

## Tasks

### Phase ID: V27-P01
### Task ID: V27-P01-T01
### Task Title: Inventory Every Icon Source, Asset, Mapping Layer, And Usage Site
- Priority: P0
- Category: Icon System
- Objective: Build the authoritative inventory of every icon library, asset, registry, manifest entry, icon API, and icon usage site in the product.
- Why This Matters Now: The application cannot be fully transformed while multiple icon systems coexist without a precise inventory.
- Visual Gap Statement: MarkAmp currently mixes manifest icons, inline SVG registration, MUI migration seams, icon-char APIs, and emoji/unicode fallbacks.
- User Experience Impact: A unified icon system improves clarity, coherence, and premium quality across every visible surface.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/resources/icons/icon_manifest.json`, `/Users/ryanrentfro/code/markamp/src/ui/IconLibrary.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/IconProvider.h`, `/Users/ryanrentfro/code/markamp/src/ui/FileIconResolver.h`, `/Users/ryanrentfro/code/markamp/src/ui/SidebarPanelRegistry.cpp`, `/Users/ryanrentfro/code/markamp/src/core/IconInventory.h`, `/Users/ryanrentfro/code/markamp/src/core/MuiIconPipeline.h`
- Prior Plan References: `v18` Phase 10, `v19` Phase 10, `v22` Phase 20, `v26` Phase 16
- Scope: icon assets, icon registries, icon renderers, icon APIs, semantic mappings, visible usage sites, residue paths
- Out of Scope: non-UI emoji support in content-authoring features
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/resources/icons/icon_manifest.json`, `/Users/ryanrentfro/code/markamp/src/ui/IconLibrary.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/IconProvider.h`, `/Users/ryanrentfro/code/markamp/src/ui/FileIconResolver.h`, `/Users/ryanrentfro/code/markamp/src/ui/SidebarPanelRegistry.cpp`, `/Users/ryanrentfro/code/markamp/src/core/IconInventory.h`, `/Users/ryanrentfro/code/markamp/src/core/MuiIconPipeline.h`
- Related Screens / Components / Surfaces: shell chrome, toolbars, menus, tabs, file tree, panel headers, settings, dialogs, notebook, canvas, status bar
- Current Visual Behavior / Appearance: the product presents multiple icon styles and rendering paths, creating semantic, optical, and state inconsistency.
- Intended Visual Behavior / Appearance: the product uses one canonical icon system with documented exceptions only where specialty file-type or creative-surface needs require them.
- Icon / Visual Design Direction: MUI-led canonical semantic icon set for general UI; explicit custom file-type and specialty adapters only where MUI is not the right metaphor.
- Technical Styling Approach: produce a repository-backed icon inventory grouped by system, surface, role, and migration status.
- Implementation Steps:
  1. Enumerate every icon asset and icon-related source file.
  2. Catalog every icon rendering path and resolution seam.
  3. Map visible UI usage sites to those systems.
  4. Mark each usage as canonical, transitional, legacy, or banned.
- Validation Steps:
  1. Verify all visible icon-bearing surfaces are covered.
  2. Cross-check inventory against icon manifest, icon registry, and MUI mapping files.
  3. Review representative screenshots to confirm no visible icon family was missed.
- Acceptance Criteria: there is a complete inventory of icon systems and visible usage sites with no unclassified icon source on the visible path.
- Dependencies: none
- Parallelization Notes: must begin first and can feed multiple downstream phases.
- Risks / Failure Modes: missing one icon path will allow visual drift to survive the overhaul.
- Accessibility / Readability Notes: inventory must include icons that communicate state or severity, not just navigation/action icons.
- Theme / Styling Notes: include theme-aware behavior and per-state rendering rules in the inventory.
- Motion / Interaction Notes where relevant: note any icons whose states change on hover, press, selected, disabled, warning, or loading.
- Cleanup / Consolidation Notes where relevant: record all legacy systems to be retired in Phase 20.
- Rollback / Safety Notes: none; this is inventory and planning structure.
- References / Context: repo scan shows at least 53 icon-related files/assets and at least 107 icon-residue matches.
- Example scenarios where useful: a panel-header icon, a command-palette icon, a file-tree icon, and a canvas tool icon should all appear in the inventory.

### Phase ID: V27-P01
### Task ID: V27-P01-T02
### Task Title: Define Canonical Semantic Icon Mapping For Every Major Product Concept
- Priority: P0
- Category: Icon System
- Objective: Create the canonical semantic icon map for actions, artifacts, severities, navigation concepts, panel types, settings groups, notebook states, and canvas tools.
- Why This Matters Now: Icon replacement will fail if semantics are not fixed before assets and controls are migrated.
- Visual Gap Statement: Similar concepts currently use different icon vocabularies and naming schemes depending on surface and implementation age.
- User Experience Impact: Stable icon semantics reduce hesitation and make the product feel deliberate and learnable.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/core/IconSemanticMapper.h`, `/Users/ryanrentfro/code/markamp/src/core/IconInventory.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/PanelHeaderIcons.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/ToolWindowTabControl.cpp`
- Prior Plan References: `v20` icon normalization, `v22` Phase 20, `v26` Phase 16
- Scope: canonical action icons, panel icons, status icons, severity icons, file/folder icons, empty-state support icons
- Out of Scope: content emoji shortcodes in editor features
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/IconSemanticMapper.h`, `/Users/ryanrentfro/code/markamp/src/core/IconInventory.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/PanelHeaderIcons.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/ToolWindowTabControl.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/IconProvider.h`
- Related Screens / Components / Surfaces: toolbar actions, menus, tab actions, panel headers, settings, notifications, empty states
- Current Visual Behavior / Appearance: icon names and meanings are partly semantic, partly local, partly transitional, and partly glyph-based.
- Intended Visual Behavior / Appearance: repeated concepts use one canonical icon and one canonical state behavior everywhere.
- Icon / Visual Design Direction: crisp modern semantic icons with strong role stability and zero novelty icon drift.
- Technical Styling Approach: use `IconSemanticMapper` and `IconInventory` as canonical mapping authorities instead of leaving mappings distributed in per-surface code.
- Implementation Steps:
  1. Define semantic concept groups across the product.
  2. Choose the canonical MUI-led icon for each concept.
  3. Mark true custom-adapter cases such as file-type and specialty artifact icons.
  4. Record deprecated metaphors and banned alternates.
- Validation Steps:
  1. Review repeated concepts across toolbar, menus, panels, settings, and feedback surfaces.
  2. Check for collisions where one icon could ambiguously represent multiple concepts.
  3. Verify the map supports light/dark, disabled, warning, and selected states.
- Acceptance Criteria: a canonical semantic map exists for all major product concepts and leaves no high-frequency concept unresolved.
- Dependencies: V27-P01-T01
- Parallelization Notes: can run while asset and render-path inventory is finalized.
- Risks / Failure Modes: overly literal icon choices may reduce elegance; overly abstract choices may reduce clarity.
- Accessibility / Readability Notes: the semantic map must include accessibility labels and non-color state expectations.
- Theme / Styling Notes: the same semantic icon should work across theme variants without bespoke remapping.
- Motion / Interaction Notes where relevant: canonical mapping should define whether an icon participates in interactive-state changes.
- Cleanup / Consolidation Notes where relevant: explicitly retire obsolete icon metaphors rather than leaving them as aliases indefinitely.
- Rollback / Safety Notes: keep alias support during rollout, but do not let aliases become permanent.
- References / Context: `IconSemanticMapper` already provides a natural home for this map.
- Example scenarios where useful: “search,” “settings,” “warning,” “new file,” and “run” should each have one canonical icon across every surface.

### Phase ID: V27-P01
### Task ID: V27-P01-T03
### Task Title: Define Canonical Icon Usage Rules For Size, Placement, State, And Surface Role
- Priority: P0
- Category: Icon System
- Objective: Standardize icon metrics, padding, placement, stroke/weight expectations, and per-state behavior across every control family.
- Why This Matters Now: Even perfect icon replacement will still feel inconsistent if icon sizing and state treatment vary by surface.
- Visual Gap Statement: The repo has icon metrics policy infrastructure, but the visible product still mixes icon glyph text, SVG render paths, and different optical spacing assumptions.
- User Experience Impact: Consistent icon usage makes the app feel more premium and significantly more cohesive.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/core/IconMetricsPolicy.h`, `/Users/ryanrentfro/code/markamp/src/ui/LayoutMetrics.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/ThemedButton.h`, `/Users/ryanrentfro/code/markamp/src/ui/PanelHeaderIcons.cpp`
- Prior Plan References: `v20` icon polish, `v22` icon cleanup, `v26` optical alignment and semantic completion
- Scope: size tiers, padding, icon-text gap, icon-only button rules, disabled opacity, selected/hover behavior, severity state behavior
- Out of Scope: raw asset drawing implementation details beyond what is needed to define usage
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/IconMetricsPolicy.h`, `/Users/ryanrentfro/code/markamp/src/ui/LayoutMetrics.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/ThemedButton.h`, `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/TabBar.cpp`
- Related Screens / Components / Surfaces: activity bar, toolbar, tabs, menus, trees, lists, dialogs, settings controls, status bar, notebook and canvas tool surfaces
- Current Visual Behavior / Appearance: icon sizes and states vary by control family and some surfaces still route icons through text labels instead of icon rendering.
- Intended Visual Behavior / Appearance: every control family uses defined icon metrics and state behavior, with no local ad hoc icon spacing or opacity logic.
- Icon / Visual Design Direction: disciplined iconography with precise optical alignment and consistent state language.
- Technical Styling Approach: extend `IconMetricsPolicy` and related layout/control roles into a single canonical usage contract.
- Implementation Steps:
  1. Define icon metrics by component family.
  2. Define interaction-state behavior for icons across normal, hover, focus, active, selected, disabled, warning, error, and loading.
  3. Define icon-only versus icon-plus-text rules.
  4. Publish usage rules for AI coding agents to follow during migration.
- Validation Steps:
  1. Compare toolbar, tabs, lists, menus, settings, and notifications side by side.
  2. Verify icon spacing and visual weight remain consistent across themes and densities.
  3. Check that icon-only controls stay discoverable and aligned.
- Acceptance Criteria: there is one explicit icon usage contract covering size, padding, state, and placement for all major control families.
- Dependencies: V27-P01-T01, V27-P01-T02
- Parallelization Notes: should complete before broad control and surface redesign begins.
- Risks / Failure Modes: over-standardization can ignore valid differences between tiny and high-emphasis contexts.
- Accessibility / Readability Notes: icon-only controls must define tooltip or label expectations.
- Theme / Styling Notes: icon state behavior must be theme-safe and avoid hardcoded colors.
- Motion / Interaction Notes where relevant: icon-state transitions should be defined here even if implemented later.
- Cleanup / Consolidation Notes where relevant: mark all local icon-size hacks and glyph-font icon paths for retirement.
- Rollback / Safety Notes: preserve a compatibility layer during migration, but treat it as temporary.
- References / Context: `IconMetricsPolicy` already exists and should become authoritative instead of advisory.
- Example scenarios where useful: a toolbar icon, a menu icon, a list-row icon, and a settings icon should all follow predictable size and placement rules.
