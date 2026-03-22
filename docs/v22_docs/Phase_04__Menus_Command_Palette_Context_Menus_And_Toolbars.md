# Phase 04: Menus Command Palette Context Menus And Toolbars

## Outcome

Make every command surface feel cohesive, legible, premium, and discoverable through stronger spacing, alignment, iconography, hover states, hierarchy, and transient-surface polish.

## Improvement Count

360 atomic improvements across 6 execution tasks.

### P04-T01

- Phase ID: P04
- Task ID: P04-T01
- Task Title: Redesign top-level menu presentation, submenu rhythm, and accelerator alignment
- Priority: P0
- Category: Menu UI
- Atomic Improvements Covered: 60
- Objective: Raise the visual quality of the main menu system so it feels precise and premium instead of native-by-default plus custom content.
- Why This Matters Now: Menus are one of the most frequently seen command surfaces and immediately reveal design quality.
- UI Problem Statement: Menu content and command integrity are being handled in prior planning, but menu presentation still varies and lacks one polished visual bar.
- User Experience Impact: Dense command surfaces feel more cluttered and harder to scan than they should.
- Scope: Main menu bar, submenu spacing, icon columns, separators, accelerator text alignment, checkbox/radio item clarity, hover/selected treatments.
- Out of Scope: Command routing correctness.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/ContextMenuRegistry.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/ThemedContextMenu.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/ContextMenuBuilder.cpp`
- Related Screens / Components / Surfaces: Main menu, submenus, global command surfaces.
- Current UI Behavior / Appearance: Menu systems vary between native and custom surfaces, and visual hierarchy is not yet fully premium.
- Intended UI Behavior / Appearance: Menus are calm, dense, crisp, and easy to parse at a glance.
- Visual / Interaction Design Direction: Precision command surfaces with excellent alignment and zero visual sloppiness.
- Technical Styling Approach: Standardize menu metrics, icon columns, accelerator spacing, and item-state rendering in the shared menu system.
- Implementation Steps: Audit menu presentation variants; refine menu metrics; normalize submenu arrows and separator treatments; tune typography and icon balance; calibrate checked-state visuals.
- Validation Steps: Review main menu and submenu states across dark, light, and high-contrast themes.
- Acceptance Criteria: Menu surfaces look like one polished family rather than a mixture of native defaults and local customizations.
- Dependencies: Phase 01, Phase 03.
- Risks / Failure Modes: Platform-native menu constraints may limit parity and require selective customization.
- Accessibility / Readability Notes: Accelerators, checked states, and hover focus need strong visual clarity.
- Theme / Styling Notes: Menu background, hover, selection, and border treatments must maintain legibility and depth in all themes.
- Motion / Interaction Notes where relevant: Keep menu transitions restrained or absent unless they materially improve perceived quality.
- Observability / Diagnostics Notes where relevant: Add menu screenshot fixtures with representative separators, icons, and accelerator states.
- Rollback / Safety Notes: Preserve native fallbacks for platforms where a fully custom menu is inappropriate.
- References / Context: The repo already contains shared context-menu infrastructure but still lacks one final menu presentation standard.
- Example scenarios where useful: A submenu with check items, icons, and shortcuts remains visually tidy and easy to scan.

### P04-T02

- Phase ID: P04
- Task ID: P04-T02
- Task Title: Rebuild command palette presentation for stronger hierarchy, search focus, and premium transient feel
- Priority: P0
- Category: Menu UI
- Atomic Improvements Covered: 60
- Objective: Turn the command palette into an aspirational command surface rather than a useful but utilitarian overlay.
- Why This Matters Now: The command palette is one of the most brand-defining UI surfaces in modern IDEs.
- UI Problem Statement: [CommandPalette.cpp](/Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp) already custom-renders the palette, but still uses local sizing, padding, and hierarchy choices that stop short of elite quality.
- User Experience Impact: Search-driven command exploration feels more basic than it should.
- Scope: Dialog shell, backdrop, search field, result rows, category labels, icons, shortcuts, empty state, error state, keyboard focus, edge glow, density.
- Out of Scope: Command ranking or backend search algorithm changes.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/CommandPaletteModel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/RichTooltip.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/ThemedDialog.cpp`
- Related Screens / Components / Surfaces: Command palette, quick actions, search-driven command launch.
- Current UI Behavior / Appearance: The command palette is custom and animated, but its visual hierarchy, shell feel, and item polish remain conservative.
- Intended UI Behavior / Appearance: The palette feels like a flagship interaction surface with excellent focus, search clarity, and result rhythm.
- Visual / Interaction Design Direction: Refined spotlight-like command surface with strong search gravity and excellent scanability.
- Technical Styling Approach: Create a dedicated command-palette surface spec with tokenized shell, row, icon, shortcut, and backdrop treatments.
- Implementation Steps: Redesign shell geometry and spacing; tune input field treatment; refine result row density and category headers; strengthen empty and no-results states; calibrate palette animation and focus framing.
- Validation Steps: Exercise the palette with short and long queries, sparse and dense result sets, and keyboard-only navigation.
- Acceptance Criteria: The command palette feels premium enough to act as a signature surface, not just a utility dialog.
- Dependencies: Phase 01, Phase 02, Phase 14.
- Risks / Failure Modes: Over-styling can hurt search immediacy if the input region loses clarity.
- Accessibility / Readability Notes: Focus must be obvious, result contrast strong, and keyboard selection unmistakable.
- Theme / Styling Notes: Palette shell, backdrop, and row states must remain excellent in light and dark modes.
- Motion / Interaction Notes where relevant: Open/close and row-highlight motion should be elegant and restrained.
- Observability / Diagnostics Notes where relevant: Add screenshot and keyboard-state fixtures for the palette.
- Rollback / Safety Notes: Keep the existing palette logic while swapping visual presentation incrementally.
- References / Context: [CommandPalette.cpp](/Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp) already contains animation and custom drawing that can be elevated rather than replaced.
- Example scenarios where useful: Opening the palette with no query already feels composed, focused, and premium rather than just empty.

### P04-T03

- Phase ID: P04
- Task ID: P04-T03
- Task Title: Unify context-menu and quick-action visual treatments across shell and content surfaces
- Priority: P1
- Category: Menu UI
- Atomic Improvements Covered: 60
- Objective: Ensure right-click and inline action menus share one high-quality visual treatment regardless of source surface.
- Why This Matters Now: Context menus are high-frequency and currently span multiple implementation paths.
- UI Problem Statement: The repo contains shared context-menu infrastructure, but major surfaces still use ad hoc `wxMenu` rendering or local visual assumptions.
- User Experience Impact: Context actions feel visually inconsistent and less trustworthy.
- Scope: Context menu shells, icon columns, row density, submenu alignment, inline action menus, code-action menus, rich item states.
- Out of Scope: Context-target enablement correctness.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/ThemedContextMenu.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/ContextMenuRegistry.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/CodeActionMenu.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/FileTreeCtrl.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/TabBar.cpp`
- Related Screens / Components / Surfaces: File tree, tabs, editor, terminal, output, problems, panel headers.
- Current UI Behavior / Appearance: Context surfaces differ in density, icon handling, submenu feel, and row highlighting.
- Intended UI Behavior / Appearance: Contextual action surfaces are visually consistent and obviously part of the same command family.
- Visual / Interaction Design Direction: Highly legible, efficient menus with confident alignment and subtle transient polish.
- Technical Styling Approach: Expand the themed context-menu system and define quick-action menu variants that inherit from the same surface spec.
- Implementation Steps: Inventory context menu variants; unify row metrics and icon slots; refine menu shell and separators; harmonize inline action menus and code-action menus with the same visual system.
- Validation Steps: Compare context menus from editor, file tree, tab bar, and terminal in all major themes.
- Acceptance Criteria: A user can move between context menus without feeling like they switched UI subsystems.
- Dependencies: Phase 01, Phase 03.
- Risks / Failure Modes: Some native context menus may resist custom presentation on certain platforms.
- Accessibility / Readability Notes: Keyboard invocation and focus states must be visibly robust.
- Theme / Styling Notes: Context-menu contrast and active states must be strong without over-emphasizing transient surfaces.
- Motion / Interaction Notes where relevant: Context surfaces should feel responsive but not animated enough to slow utility.
- Observability / Diagnostics Notes where relevant: Add per-surface context-menu visual baselines.
- Rollback / Safety Notes: Preserve native menu fallbacks where platform constraints require them.
- References / Context: Many visible right-click paths are still visually inconsistent despite the presence of shared infrastructure.
- Example scenarios where useful: Right-clicking a tab and invoking a code action in the editor feels like using one command-surface family.

### P04-T04

- Phase ID: P04
- Task ID: P04-T04
- Task Title: Upgrade toolbar grouping, icon-label rhythm, overflow, and discoverability cues
- Priority: P0
- Category: Toolbar UI
- Atomic Improvements Covered: 60
- Objective: Make global and panel-local toolbars feel calmer, more intentional, and easier to parse.
- Why This Matters Now: Toolbars often drift into noisy control dumps without strict visual grouping and rhythm.
- UI Problem Statement: [Toolbar.cpp](/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp), [ToolbarModel.cpp](/Users/ryanrentfro/code/markamp/src/ui/ToolbarModel.cpp), [ExplorerToolbar.cpp](/Users/ryanrentfro/code/markamp/src/ui/ExplorerToolbar.cpp), and local panel toolbars vary in density and action treatment.
- User Experience Impact: Users see useful actions, but not always through a premium visual arrangement.
- Scope: Group spacing, icon-label balance, separators, icon-only controls, overflow surfaces, hover states, active toggles, discoverability cues.
- Out of Scope: Command wiring logic.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/ToolbarModel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/ExplorerToolbar.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/FloatingToolbar.cpp`
- Related Screens / Components / Surfaces: Global toolbar, panel toolbars, floating toolbars, overflow menus.
- Current UI Behavior / Appearance: Toolbars are functional but vary in density, grouping clarity, and icon-label strategy.
- Intended UI Behavior / Appearance: Toolbars feel editorially arranged rather than mechanically populated.
- Visual / Interaction Design Direction: Crisp tool clusters with excellent optical spacing and a premium action rhythm.
- Technical Styling Approach: Define toolbar family variants and shared rules for group spacing, separators, labels, and icon-only states.
- Implementation Steps: Audit toolbar families; define grouping and label rules; refine button container treatments; normalize overflow indicators; align local toolbars to the same family.
- Validation Steps: Review top toolbar, Explorer toolbar, and floating toolbars in dense and sparse modes.
- Acceptance Criteria: Toolbars across the product look intentionally related and easier to scan.
- Dependencies: Phase 01, Phase 03.
- Risks / Failure Modes: Over-grouping or excessive separators can make toolbars feel fragmented.
- Accessibility / Readability Notes: Icon-only actions require strong tooltip and focus support.
- Theme / Styling Notes: Toolbar surfaces need enough contrast from content without becoming visually heavy.
- Motion / Interaction Notes where relevant: Hover, active, and overflow transitions should feel brisk and tactile.
- Observability / Diagnostics Notes where relevant: Add toolbar family screenshots and alignment checks.
- Rollback / Safety Notes: Keep action ordering stable while visual grouping is refined.
- References / Context: Current toolbar implementations show multiple generations of visual assumptions.
- Example scenarios where useful: The main toolbar and the Explorer toolbar both feel carefully composed even though one is global and one is local.

### P04-T05

- Phase ID: P04
- Task ID: P04-T05
- Task Title: Polish rich tooltips, shortcut hints, and command discoverability surfaces
- Priority: P2
- Category: Menu UI
- Atomic Improvements Covered: 60
- Objective: Improve command discoverability through better tooltip hierarchy, helper text, and transient affordance quality.
- Why This Matters Now: Premium IDEs teach as they operate, not only through documentation.
- UI Problem Statement: Tooltips and helper surfaces exist in [TooltipWindow.cpp](/Users/ryanrentfro/code/markamp/src/ui/TooltipWindow.cpp) and [RichTooltip.cpp](/Users/ryanrentfro/code/markamp/src/ui/RichTooltip.cpp), but discoverability remains uneven across command surfaces.
- User Experience Impact: Users miss useful features or do not build confidence in shortcut-driven workflows.
- Scope: Toolbar tooltips, status item tooltips, activity bar tooltips, rich tooltips, shortcut hint styling, contextual helper text.
- Out of Scope: Full command education or walkthrough strategy.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/TooltipWindow.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/RichTooltip.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/ActivityBar.cpp`
- Related Screens / Components / Surfaces: Tooltips, helper overlays, hover-driven discoverability.
- Current UI Behavior / Appearance: Tooltips exist but vary in offset, density, information hierarchy, and perceived polish.
- Intended UI Behavior / Appearance: Tooltips are elegant, correctly timed, and genuinely useful for discoverability.
- Visual / Interaction Design Direction: Compact, premium helper surfaces with clear hierarchy and restraint.
- Technical Styling Approach: Create tooltip variants and content rules for plain, rich, shortcut-heavy, and instructional tooltip surfaces.
- Implementation Steps: Audit tooltip usages; normalize shell, spacing, and typography; refine timing and offsets; add richer variants where warranted; remove noisy or redundant hints.
- Validation Steps: Hover across shell surfaces and verify consistent tooltip behavior and visual quality.
- Acceptance Criteria: Tooltips and helper cues improve discoverability without cluttering the UI.
- Dependencies: Phase 01, Phase 14.
- Risks / Failure Modes: Too much tooltip information can slow expert use and clutter novice discovery.
- Accessibility / Readability Notes: Tooltip timing and focus handling must remain keyboard- and screen-reader-friendly where applicable.
- Theme / Styling Notes: Tooltip contrast must remain strong against varied backgrounds.
- Motion / Interaction Notes where relevant: Tooltip fade and movement should be subtle and reduced-motion aware.
- Observability / Diagnostics Notes where relevant: Add tooltip screenshots and timing documentation to the UI atlas.
- Rollback / Safety Notes: Preserve simple tooltip fallback paths if rich variants are not yet available everywhere.
- References / Context: Discoverability is part of UI quality, not separate from it.
- Example scenarios where useful: Hovering a toolbar control shows a crisp label, shortcut, and one-line purpose without looking noisy.

### P04-T06

- Phase ID: P04
- Task ID: P04-T06
- Task Title: Add command-surface visual galleries and regression checks
- Priority: P0
- Category: Cleanup / Consolidation
- Atomic Improvements Covered: 60
- Objective: Guard menu, palette, toolbar, tooltip, and context-menu polish with explicit visual references.
- Why This Matters Now: Command surfaces are high-frequency, high-visibility, and easy to regress as functional work continues.
- UI Problem Statement: There is no one gallery or baseline set for the full family of command surfaces.
- User Experience Impact: Command surfaces gradually drift, becoming less cohesive and less premium.
- Scope: Menus, submenus, command palette, toolbars, context menus, tooltips, rich helper surfaces.
- Out of Scope: Deep command semantics.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/tests/unit`; `/Users/ryanrentfro/code/markamp/docs/v22_docs`; `/Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`
- Related Screens / Components / Surfaces: All command-entry surfaces.
- Current UI Behavior / Appearance: Command-surface quality depends on localized review rather than durable baselines.
- Intended UI Behavior / Appearance: Each command-surface family has approved reference states and visual regression protection.
- Visual / Interaction Design Direction: System-level command-surface discipline.
- Technical Styling Approach: Build a component gallery and screenshot matrix for all command-surface variants and states.
- Implementation Steps: Define representative fixtures; capture menu, toolbar, palette, tooltip, and context-menu states; publish gallery docs; wire visual tests into CI.
- Validation Steps: Perturb spacing or icon alignment in a command surface and verify the QA stack catches the regression.
- Acceptance Criteria: Command-surface polish becomes regression-gated instead of dependent on memory.
- Dependencies: P04-T01 through P04-T05.
- Risks / Failure Modes: Platform differences can make menu screenshots noisy unless fixtures are tightly controlled.
- Accessibility / Readability Notes: Include keyboard-selected and focus-visible states in the gallery.
- Theme / Styling Notes: Capture all command-surface families in dark, light, and high-contrast themes.
- Motion / Interaction Notes where relevant: Include motion references for palette open/close and tooltip transitions.
- Observability / Diagnostics Notes where relevant: Store visual fixture metadata with token IDs and surface family names.
- Rollback / Safety Notes: Start with reference galleries before promoting all checks to strict CI.
- References / Context: `v22` is large enough that command-surface polish needs durable protection.
- Example scenarios where useful: A future toolbar change that misaligns icon-only buttons is caught automatically.

