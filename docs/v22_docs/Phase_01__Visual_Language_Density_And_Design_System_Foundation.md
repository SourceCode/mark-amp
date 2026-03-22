# Phase 01: Visual Language Density And Design System Foundation

## Outcome

Create one enforceable visual language for spacing, density, typography, corners, borders, state treatments, and icon geometry so the rest of the UI wave can ship on top of a durable system.

## Improvement Count

360 atomic improvements across 6 execution tasks.

### P01-T01

- Phase ID: P01
- Task ID: P01-T01
- Task Title: Consolidate spacing, density, and geometry systems into one authoritative UI metrics layer
- Priority: P0
- Category: Visual Language
- Atomic Improvements Covered: 60
- Objective: Eliminate conflicting geometry systems and ensure every surface derives spacing, row heights, toolbar heights, icon slots, and hit targets from one UI metrics contract.
- Why This Matters Now: The repository already has multiple geometry layers, which guarantees drift if more UI polish is layered on top.
- UI Problem Statement: [UILayoutTokens.h](/Users/ryanrentfro/code/markamp/src/ui/UILayoutTokens.h), [LayoutMetrics.h](/Users/ryanrentfro/code/markamp/src/ui/LayoutMetrics.h), `SpacingGrid`, and many widget-local literals coexist.
- User Experience Impact: Users feel inconsistent density, spacing rhythm, and control sizing as they move across the product.
- Scope: Global spacing scale, row heights, shell chrome heights, panel paddings, toolbar rhythm, minimum hit targets, tab sizing, empty-state spacing.
- Out of Scope: Full theme color redesign.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/UILayoutTokens.h`; `/Users/ryanrentfro/code/markamp/src/ui/LayoutMetrics.h`; `/Users/ryanrentfro/code/markamp/src/ui/LayoutMetrics.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/ActivityBar.cpp`
- Related Screens / Components / Surfaces: Shell chrome, tabs, menus, panels, trees, dialogs, settings, onboarding.
- Current UI Behavior / Appearance: Shared tokens exist, but many surfaces still choose size and spacing locally.
- Intended UI Behavior / Appearance: Every visible surface reads from one density-aware geometry system and looks metrically related.
- Visual / Interaction Design Direction: Crisp, calm, intentional spacing with a clear compact-to-comfortable progression rather than ad hoc per-surface density.
- Technical Styling Approach: Merge or alias geometry systems behind one metrics API and replace surface literals with tokenized queries.
- Implementation Steps: Inventory geometry literals; define authoritative tokens; map legacy metrics to the new layer; migrate shell surfaces first; then migrate dense panel and dialog surfaces.
- Validation Steps: Compare row heights, padding, icon slots, and toolbar heights across shell, panel, dialog, and settings surfaces before and after migration.
- Acceptance Criteria: New and existing UI surfaces cannot justify custom geometry without an explicit documented exception.
- Dependencies: None.
- Risks / Failure Modes: Over-normalization can flatten legitimate surface distinctions if token families are too shallow.
- Accessibility / Readability Notes: Minimum hit targets and spacing must remain valid even in compact density profiles.
- Theme / Styling Notes: Geometry tokens must remain theme-agnostic so color and typography can vary independently.
- Motion / Interaction Notes where relevant: Geometry changes must not break hover/focus alignment and animation anchors.
- Observability / Diagnostics Notes where relevant: Add lint or audit checks for unauthorized geometry literals in key UI files.
- Rollback / Safety Notes: Support temporary compatibility aliases while migrating legacy widgets.
- References / Context: [UILayoutTokens.h](/Users/ryanrentfro/code/markamp/src/ui/UILayoutTokens.h) and [LayoutMetrics.h](/Users/ryanrentfro/code/markamp/src/ui/LayoutMetrics.h) already overlap in responsibility.
- Example scenarios where useful: A toolbar button, a settings row, and an explorer row all feel distinct but clearly belong to one density system.

### P01-T02

- Phase ID: P01
- Task ID: P01-T02
- Task Title: Establish a product-wide typography hierarchy and remove subsystem-local type styling
- Priority: P0
- Category: Typography
- Atomic Improvements Covered: 60
- Objective: Standardize headings, labels, captions, mono text, body text, metadata text, and shouty utility labels into one premium type system.
- Why This Matters Now: Typography is one of the fastest ways users detect whether a product feels designed or assembled.
- UI Problem Statement: Surfaces such as [SettingsPanel.cpp](/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp), [ThemeGallery.cpp](/Users/ryanrentfro/code/markamp/src/ui/ThemeGallery.cpp), [StartupPanel.cpp](/Users/ryanrentfro/code/markamp/src/ui/StartupPanel.cpp), and [CommandPalette.cpp](/Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp) apply fonts locally.
- User Experience Impact: Hierarchy, scannability, and perceived quality vary too much between screens.
- Scope: Shell headings, panel titles, body text, metadata text, settings descriptions, menu labels, notebook labels, canvas labels, status text, onboarding typography.
- Out of Scope: Syntax token colors for code text.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/StartupPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/ThemeGallery.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp`; `/Users/ryanrentfro/code/markamp/src/core/ThemeTokens.h`
- Related Screens / Components / Surfaces: Shell surfaces, dialogs, settings, startup, theme gallery, data views.
- Current UI Behavior / Appearance: Fonts and point sizes are often selected in-place and sometimes use uppercase or utility-oriented tone inconsistently.
- Intended UI Behavior / Appearance: Typography communicates hierarchy calmly and consistently across every screen.
- Visual / Interaction Design Direction: Premium editorial hierarchy with restrained headings, highly legible UI labels, and quieter metadata tones.
- Technical Styling Approach: Expand font token coverage, centralize typographic roles, and replace widget-local `SetFont` logic with semantic roles.
- Implementation Steps: Define type ramp; create role-based font tokens; map surfaces to roles; remove local point-size tweaks; normalize capitalization and text rhythm.
- Validation Steps: Audit headings, labels, descriptions, captions, and metadata across representative screens.
- Acceptance Criteria: Product-wide type usage is explainable via a small set of semantic roles, not local widget decisions.
- Dependencies: P01-T01.
- Risks / Failure Modes: Direct migration can expose wx-native controls that resist custom font treatment.
- Accessibility / Readability Notes: Preserve legibility at low vision sizes and ensure metadata is muted without becoming faint.
- Theme / Styling Notes: Typography roles should combine with theme colors consistently in light, dark, and high-contrast modes.
- Motion / Interaction Notes where relevant: Text truncation and transitions must remain stable when fonts normalize.
- Observability / Diagnostics Notes where relevant: Add a typography audit pass that reports raw point-size overrides in major UI files.
- Rollback / Safety Notes: Keep old type roles mapped temporarily while migrating surfaces in slices.
- References / Context: [ThemeGallery.cpp](/Users/ryanrentfro/code/markamp/src/ui/ThemeGallery.cpp) and [StartupPanel.cpp](/Users/ryanrentfro/code/markamp/src/ui/StartupPanel.cpp) show local typography choices instead of semantic roles.
- Example scenarios where useful: Settings headings, command palette group labels, and startup section titles all feel visually related rather than independently tuned.

### P01-T03

- Phase ID: P01
- Task ID: P01-T03
- Task Title: Standardize corner radii, border treatments, elevation, and container language
- Priority: P1
- Category: Visual Language
- Atomic Improvements Covered: 60
- Objective: Make cards, buttons, dialogs, menus, tabs, panels, overlays, and container surfaces use one coherent edge and elevation language.
- Why This Matters Now: Corner and border inconsistency is one of the clearest signs of local UI evolution.
- UI Problem Statement: Surfaces like [ThemedButton.cpp](/Users/ryanrentfro/code/markamp/src/ui/ThemedButton.cpp), [ThemedDialog.cpp](/Users/ryanrentfro/code/markamp/src/ui/ThemedDialog.cpp), [CommandPalette.cpp](/Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp), and [ThemeGallery.cpp](/Users/ryanrentfro/code/markamp/src/ui/ThemeGallery.cpp) each imply slightly different container rules.
- User Experience Impact: The interface looks assembled from different component families instead of one product.
- Scope: Buttons, pills, tabs, cards, dialogs, popovers, menus, toasts, side panels, empty states, split views.
- Out of Scope: New illustration systems.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/ThemedButton.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/ThemedDialog.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/ThemeGallery.cpp`; `/Users/ryanrentfro/code/markamp/src/core/ThemeTokens.h`
- Related Screens / Components / Surfaces: Buttons, dialogs, overlays, cards, panels.
- Current UI Behavior / Appearance: Rounded corners, borders, and depth cues vary by surface.
- Intended UI Behavior / Appearance: Containers share a clear visual family with deliberate variance by role rather than by implementation history.
- Visual / Interaction Design Direction: Refined, restrained container language with purposeful differences between chrome, content, overlay, and ephemeral surfaces.
- Technical Styling Approach: Introduce container variant tokens for radius, border weight, fill treatment, and shadow/elevation semantics.
- Implementation Steps: Inventory current radii and borders; define allowed container variants; migrate controls and overlays; remove local edge/elevation constants.
- Validation Steps: Compare container edges and shadows across shell, dialogs, menus, and toasts.
- Acceptance Criteria: Container treatments can be described by a small shared variant matrix.
- Dependencies: P01-T01, P01-T02.
- Risks / Failure Modes: wx-native widgets may need wrapper painting or custom hosts to fully align.
- Accessibility / Readability Notes: Border contrast must remain strong enough for low-vision separation.
- Theme / Styling Notes: Elevation and border variants must adapt cleanly to light and dark themes.
- Motion / Interaction Notes where relevant: Pressed and hover states must honor the container system instead of fighting it.
- Observability / Diagnostics Notes where relevant: Add a visual inventory of radius and border variants in use.
- Rollback / Safety Notes: Keep surface-level fallback variants while migrating native controls.
- References / Context: [ThemedDialog.cpp](/Users/ryanrentfro/code/markamp/src/ui/ThemedDialog.cpp) and [ThemedButton.cpp](/Users/ryanrentfro/code/markamp/src/ui/ThemedButton.cpp) already imply a container grammar that needs generalization.
- Example scenarios where useful: Dialog shells, command palette, tooltips, and notifications all look related without becoming visually repetitive.

### P01-T04

- Phase ID: P01
- Task ID: P01-T04
- Task Title: Define canonical control-state visuals for hover, focus, pressed, disabled, selected, and active modes
- Priority: P0
- Category: Control Styling
- Atomic Improvements Covered: 60
- Objective: Create one shared language for interaction states so controls stop expressing state differently by surface.
- Why This Matters Now: The UI cannot feel premium if every surface invents its own hover or focus semantics.
- UI Problem Statement: [ThemedButton.cpp](/Users/ryanrentfro/code/markamp/src/ui/ThemedButton.cpp), [ActivityBar.cpp](/Users/ryanrentfro/code/markamp/src/ui/ActivityBar.cpp), [Toolbar.cpp](/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp), [StatusBarPanel.cpp](/Users/ryanrentfro/code/markamp/src/ui/StatusBarPanel.cpp), and other custom-painted controls do not clearly share one state model.
- User Experience Impact: Interaction feel becomes noisy, inconsistent, and less trustworthy.
- Scope: Buttons, toggle controls, list rows, tabs, menu items, panel headers, inline actions, badges, overlays, tree rows.
- Out of Scope: Full accessibility behavior semantics outside visible state treatment.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/ThemedButton.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/ThemedCheckbox.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/ThemedToggleSwitch.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/ActivityBar.cpp`; `/Users/ryanrentfro/code/markamp/src/core/ThemeTokens.h`
- Related Screens / Components / Surfaces: Controls, rows, tabs, menus, action icons.
- Current UI Behavior / Appearance: State treatments differ by control family and by subsystem.
- Intended UI Behavior / Appearance: Users can recognize interaction states instantly regardless of surface.
- Visual / Interaction Design Direction: Clear but restrained state changes with strong focus visibility, calm hover feedback, and tactile pressed responses.
- Technical Styling Approach: Expand state tokens and shared control-state utilities, then refactor bespoke widgets to consume them.
- Implementation Steps: Define state vocabulary; add state tokens; update themed controls; migrate shell chrome controls; then migrate list and row surfaces.
- Validation Steps: Compare the same state transitions across buttons, rows, tabs, menus, and toggles.
- Acceptance Criteria: State visuals are consistent enough that they read as one interaction language.
- Dependencies: P01-T01 through P01-T03.
- Risks / Failure Modes: Overly subtle states may improve elegance but harm discoverability if not tested carefully.
- Accessibility / Readability Notes: Focus and selected states must not rely on color alone.
- Theme / Styling Notes: State intensity needs calibration by theme mode so dark themes do not become muddy and light themes do not become washed out.
- Motion / Interaction Notes where relevant: State transitions should be animated consistently where motion is used.
- Observability / Diagnostics Notes where relevant: Add screenshot-based regression references for each canonical state.
- Rollback / Safety Notes: Support one legacy state mapping layer while bespoke controls are migrated.
- References / Context: [ThemeTokens.h](/Users/ryanrentfro/code/markamp/src/core/ThemeTokens.h) already defines control-state tokens but usage is incomplete.
- Example scenarios where useful: A hover state on a toolbar icon-only button reads like the hover state on a settings row action, not like a different product.

### P01-T05

- Phase ID: P01
- Task ID: P01-T05
- Task Title: Normalize icon metrics, label spacing, and optical alignment across the UI system
- Priority: P1
- Category: Iconography
- Atomic Improvements Covered: 60
- Objective: Make icons feel optically aligned, semantically consistent, and metrically related across every visible surface.
- Why This Matters Now: MarkAmp already has icon infrastructure, but the visuals still reveal mixed icon families and local spacing assumptions.
- UI Problem Statement: [IconMetricsPolicy.cpp](/Users/ryanrentfro/code/markamp/src/core/IconMetricsPolicy.cpp) and MUI icon infrastructure exist, yet [CanvasWorkspacePanel.cpp](/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp) still uses Unicode placeholders and shell surfaces still vary in icon spacing.
- User Experience Impact: The interface feels less refined and less legible, especially in dense action surfaces.
- Scope: Activity bar, toolbar, tabs, trees, menus, status bar, settings, canvas tools, dialogs, notifications.
- Out of Scope: Brand illustration work.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/IconMetricsPolicy.cpp`; `/Users/ryanrentfro/code/markamp/src/core/MuiIconPipeline.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/IconManager.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/ActivityBar.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp`
- Related Screens / Components / Surfaces: Action surfaces, navigation surfaces, trees, notebooks, canvas, settings.
- Current UI Behavior / Appearance: Icon size, padding, semantic selection, and optical baseline vary by surface.
- Intended UI Behavior / Appearance: Icons feel cohesive, balanced, and readable everywhere.
- Visual / Interaction Design Direction: Confident icon usage with clean optical rhythm and no placeholder-looking symbolism in production surfaces.
- Technical Styling Approach: Expand icon-family metrics, define semantic icon usage tables, and migrate local icon assumptions to the central pipeline.
- Implementation Steps: Audit icon usage; remove Unicode placeholders; normalize slot sizes and label gaps; calibrate stroke/weight by family; align icon-only control affordances.
- Validation Steps: Compare icon alignment and spacing across representative shell and content surfaces in dark and light themes.
- Acceptance Criteria: No major visible surface still uses visually inconsistent or placeholder icon treatment.
- Dependencies: P01-T01, P01-T04.
- Risks / Failure Modes: Some local icons may not have MUI equivalents and need transitional semantic mapping.
- Accessibility / Readability Notes: Icon-only controls require clear hit targets and discoverable accessible labels.
- Theme / Styling Notes: Icon contrast and opacity rules must remain readable in low-contrast regions.
- Motion / Interaction Notes where relevant: Icon states should animate with the same restraint as the broader control system.
- Observability / Diagnostics Notes where relevant: Add icon-residue and icon-family audits to the visual QA stack.
- Rollback / Safety Notes: Maintain compatibility aliases for icon IDs during migration, but not visual fallback to placeholders in production.
- References / Context: The repo already contains icon metrics and migration tooling, which makes this a completion problem rather than a greenfield design problem.
- Example scenarios where useful: A 16px tree icon, 20px toolbar icon, and 14px status icon all feel like intentionally scaled variants of one icon system.

### P01-T06

- Phase ID: P01
- Task ID: P01-T06
- Task Title: Create a UI reference atlas and visual QA baseline for the v22 wave
- Priority: P0
- Category: Cleanup / Consolidation
- Atomic Improvements Covered: 60
- Objective: Turn the design-system decisions into a referenceable execution baseline for all later UI phases.
- Why This Matters Now: A UI wave of this size will drift quickly without explicit visual references and surface audits.
- UI Problem Statement: The repository contains many UI systems, but there is no single visual atlas that states what “correct” looks like across shell, panels, editor, notebook, canvas, settings, and overlays.
- User Experience Impact: Internal inconsistency persists because teams lack a shared UI finish bar.
- Scope: Visual inventory, before/after targets, component reference states, density references, theme references, screenshot baselines, surface readiness labels.
- Out of Scope: End-user documentation.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/docs/v22_docs`; `/Users/ryanrentfro/code/markamp/src/ui`; `/Users/ryanrentfro/code/markamp/src/core`; `/Users/ryanrentfro/code/markamp/src/canvas`
- Related Screens / Components / Surfaces: All visible UI systems.
- Current UI Behavior / Appearance: Visual intent is discoverable only by reading many files and manually comparing surfaces.
- Intended UI Behavior / Appearance: Teams can reference one visual baseline showing approved spacing, type, state, icon, and container behavior.
- Visual / Interaction Design Direction: Operationally rigorous design documentation tied to executable component states.
- Technical Styling Approach: Generate a surface/component atlas and pair it with automated screenshot and token audits.
- Implementation Steps: Capture reference surfaces; define baseline matrices; publish UI atlas docs; connect later phase work to the atlas; set update rules for the atlas.
- Validation Steps: Compare new surface work against the atlas and verify screenshot drift is intentional and approved.
- Acceptance Criteria: Later phases can rely on an agreed visual source of truth instead of subsystem memory.
- Dependencies: P01-T01 through P01-T05.
- Risks / Failure Modes: An atlas can become stale if it is not tied to executable states and screenshot capture.
- Accessibility / Readability Notes: Include contrast, focus, and reduced-motion reference states in the atlas.
- Theme / Styling Notes: Every reference needs dark, light, and high-contrast variants where applicable.
- Motion / Interaction Notes where relevant: Include micro-state transitions, not just static screenshots.
- Observability / Diagnostics Notes where relevant: Connect atlas states to screenshot regression tooling and UI audits.
- Rollback / Safety Notes: Keep the atlas additive and versioned so surfaces can migrate progressively.
- References / Context: `v22` is large enough that visual enforcement needs the same rigor as architectural enforcement in prior planning waves.
- Example scenarios where useful: A contributor updating the Search panel can compare row density, empty states, badges, and hover treatment to approved patterns instead of inventing local variants.

