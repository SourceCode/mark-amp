# MarkAmp V26 UI Refinement And Premium Completion Assessment

## Purpose

`v26` is the next dedicated UI-only planning wave for MarkAmp. It is not a feature-completion pass, not a backend pass, and not a release-hardening pass except where visible quality depends on it. This package translates the earlier broad UI planning in `v22` and the release-path UI closure work in `v25` into the next tightly sequenced execution wave for elite interface quality.

## Inputs Reviewed

- Prior planning packages in:
  - `/Users/ryanrentfro/code/markamp/docs/v18_docs`
  - `/Users/ryanrentfro/code/markamp/docs/v19_docs`
  - `/Users/ryanrentfro/code/markamp/docs/v20_docs`
  - `/Users/ryanrentfro/code/markamp/docs/v21_docs`
  - `/Users/ryanrentfro/code/markamp/docs/v22_docs`
  - `/Users/ryanrentfro/code/markamp/docs/v23_docs`
  - `/Users/ryanrentfro/code/markamp/docs/v24_docs`
  - `/Users/ryanrentfro/code/markamp/docs/v25_docs`
- Current UI and theme implementation in:
  - `/Users/ryanrentfro/code/markamp/src/ui`
  - `/Users/ryanrentfro/code/markamp/src/ui/av`
  - `/Users/ryanrentfro/code/markamp/src/core`
  - `/Users/ryanrentfro/code/markamp/src/canvas`

## Assessment Summary

The repository no longer looks like an early-stage UI system. MarkAmp now has real visual infrastructure, custom shell chrome, theme-token expansion, spacing and typography primitives, and several high-value custom-painted surfaces. The main problem is no longer absence of UI systems. The problem is uneven adoption, incomplete convergence, and visible quality gaps between the strongest surfaces and the weakest ones.

`v22` was directionally correct but broad and inventory-heavy. `v25` narrowed UI work to release-path closure, but that was intentionally selective. `v26` should therefore treat the design system and core shell as partially landed, retire stale first-pass UI discovery tasks, and concentrate on the remaining premium-finish work:

- normalize token adoption across all visible surfaces
- close inconsistent layout rhythm and density
- replace lingering placeholder-looking UI
- raise notebook, canvas, settings, panel, and onboarding presentation to shell-quality parity
- improve editor readability and syntax presentation beyond functional correctness
- standardize iconography, transient surfaces, empty states, and feedback states
- unify motion, focus treatment, contrast, and micro-interaction language
- remove legacy or local styling fragments that block a cohesive premium finish

## What Changed Since Earlier Planning

### Completed Or Partially Landed Since Earlier UI Passes

- Foundational visual primitives now exist:
  - `/Users/ryanrentfro/code/markamp/src/ui/UILayoutTokens.h`
  - `/Users/ryanrentfro/code/markamp/src/ui/LayoutMetrics.h`
  - `/Users/ryanrentfro/code/markamp/src/ui/SpacingGrid.h`
  - `/Users/ryanrentfro/code/markamp/src/ui/TypographyScale.h`
  - `/Users/ryanrentfro/code/markamp/src/core/ThemeTokens.h`
  - `/Users/ryanrentfro/code/markamp/src/core/ThemeEngine.cpp`
- Core shell surfaces have stronger custom implementations than earlier generations:
  - `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/ActivityBar.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/TabBar.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/StatusBarPanel.cpp`
- Theme vocabulary has expanded well beyond early-editor-only coverage and now contains shell, tab, command palette, notebook, canvas, and status tokens.

### Still Partial, Stale, Or Visibly Uneven

- Several major surfaces still rely on local ad hoc styling instead of consistent tokenized UI language.
- Several surfaces still present placeholders, stubs, emoji/unicode stand-ins, or “for now” implementations in visible product paths.
- Some panels and dialogs are functional but visually underdeveloped relative to the shell.
- Editor and syntax surfaces are capable but not yet tuned to premium readability standards across themes.
- Notebook and canvas visual systems lag behind shell chrome in refinement and intentionality.

## Repository Evidence

### Design-System Convergence Exists But Is Incomplete

- `/Users/ryanrentfro/code/markamp/src/ui/UILayoutTokens.h`
- `/Users/ryanrentfro/code/markamp/src/ui/LayoutMetrics.h`
- `/Users/ryanrentfro/code/markamp/src/ui/SpacingGrid.h`
- `/Users/ryanrentfro/code/markamp/src/ui/TypographyScale.h`
- `/Users/ryanrentfro/code/markamp/src/core/ThemeTokens.h`
- `/Users/ryanrentfro/code/markamp/src/core/ThemeEngine.cpp`

These files indicate that spacing, type scale, and theme-token systems are already present. `v26` should not reopen design-system invention. It should drive adoption, parity, and premium finish.

### High-Value UI Debt Still Visible

- `/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp`
  - visible unicode icon placeholders and placeholder inspector text
- `/Users/ryanrentfro/code/markamp/src/ui/StartupPanel.cpp`
  - duplicated shell logic, fallback accent behavior, local paint decisions
- `/Users/ryanrentfro/code/markamp/src/ui/ThemeGallery.cpp`
  - extensive local color/font treatment rather than unified gallery system
- `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp`
  - emoji icon prefixes and heavy local styling logic
- `/Users/ryanrentfro/code/markamp/src/ui/SearchSidebarPanel.cpp`
  - dense custom styling with placeholder and temporary language
- `/Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp`
  - still calls out empty-state polish work
- `/Users/ryanrentfro/code/markamp/src/ui/ThemedDialog.cpp`
  - thin dialog shell relative to premium target
- `/Users/ryanrentfro/code/markamp/src/ui/ExplorerPanel.cpp`
  - empty sections and visually uneven composition
- `/Users/ryanrentfro/code/markamp/src/ui/ProblemsPanel.cpp`
  - placeholder command-bus integration note and basic toolbar treatment
- `/Users/ryanrentfro/code/markamp/src/ui/PDFViewerPanel.cpp`
  - stub implementation still visible on the UI path
- `/Users/ryanrentfro/code/markamp/src/ui/HistoryPanel.cpp`
  - stub implementation still visible on the UI path
- `/Users/ryanrentfro/code/markamp/src/ui/MinimapPanel.cpp`
  - simplified presentation still aligned to MVP-level rendering
- `/Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp`
  - hardcoded paint choices still present

### Styling-Debt Hotspots From Repo Scan

The strongest remaining concentration of local UI styling and visible debt is in:

- `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`
- `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`
- `/Users/ryanrentfro/code/markamp/src/ui/ThemeGallery.cpp`
- `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp`
- `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp`
- `/Users/ryanrentfro/code/markamp/src/ui/SearchSidebarPanel.cpp`
- `/Users/ryanrentfro/code/markamp/src/ui/TerminalPanel.cpp`
- `/Users/ryanrentfro/code/markamp/src/ui/StartupPanel.cpp`
- `/Users/ryanrentfro/code/markamp/src/ui/OutputPanel.cpp`
- `/Users/ryanrentfro/code/markamp/src/ui/TabBar.cpp`
- `/Users/ryanrentfro/code/markamp/src/ui/ProblemsPanel.cpp`
- `/Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp`

## Reclassification Of Earlier UI Planning

### Retire As Complete Or Mostly Complete

- first-pass creation of spacing and typography primitives
- first-pass shell chrome creation for activity bar, toolbar, tab bar, and status bar
- first-pass theme token expansion
- broad “inventory every visible issue” discovery work from `v22`

### Merge And Narrow

- shell polish tasks from `v22` should now be merged into fewer convergence tasks centered on consistency and premium finish
- theme tasks from `v22` and `v25` should now be merged into parity and token-adoption work rather than more token proliferation
- settings, theme gallery, command palette, onboarding, and panel work should be clustered by visible consistency and surface maturity rather than treated as isolated components

### De-Scope From V26

- backend functionality completion
- persistence, execution, or architecture changes that are not required to achieve visible UI quality
- invisible service cleanup unless it blocks durable UI consistency

## Highest-Risk Remaining UI Gaps

### 1. Design-System Adoption Gap

The app has tokens and metrics, but many visible surfaces still style themselves locally. That creates density drift, uneven spacing, inconsistent typography, and inconsistent interaction-state language.

### 2. Shell-To-Surface Quality Gap

The custom shell is stronger than several interior surfaces. Startup, settings, theme gallery, search sidebar, notebook output, canvas workspace, some dialogs, and structured data surfaces do not yet feel like they belong to the same premium product.

### 3. Placeholder And Temporary-Looking UI

Unicode icon stand-ins, emoji prefixes, stub panels, default or fallback paint logic, and placeholder text still appear on visible product paths. These directly undermine perceived quality.

### 4. Editor / Syntax Premium Readability Gap

The editor is feature-rich, but readability tuning, minimap quality, diagnostics styling, selection language, and cross-theme syntax hierarchy still look more practical than elite.

### 5. Notebook And Canvas Visual Maturity Gap

Both product pillars exist, but their chrome, focus states, spacing, overlays, empty states, output presentation, and interaction visuals are not yet at shell-quality parity.

### 6. Theme Parity Gap

Token breadth exists, but light/dark parity, cross-surface correctness, and component-level consistency are still uneven.

## Definitions Of Visually Complete

### App Shell

- spacing rhythm is consistent across top, side, central, and bottom zones
- shell surfaces share the same radius, border, divider, hover, selection, and elevation language
- resizing and overflow states stay composed

### Editor

- typography, gutter, caret, selection, diagnostics, search results, and minimap read as one intentional system
- syntax hierarchy is clear and premium in all supported themes
- no fallback or mismatched paint treatments remain in the reading path

### Notebook

- code, markdown, output, toolbar, and execution states share one coherent notebook chrome system
- dense mixed-content views remain readable without feeling heavy
- empty, loading, and error states are elegant and informative

### Canvas

- canvas shell, tool palette, object affordances, overlays, guides, and zoom/navigation controls feel precise and premium
- onboarding and empty states make the surface feel intentional rather than raw

### Panels And Settings

- panel headers, section headers, actions, rows, lists, trees, and settings controls use standardized rhythm and hierarchy
- no emoji/unicode placeholders or legacy visual fragments remain

### Menus / Overlays / Feedback

- dialogs, popovers, tooltips, command palette, notifications, progress states, and inline warnings all look related
- transient surfaces have consistent spacing, motion, and contrast behavior

## Recommended V26 Strategy

1. Start with convergence, not expansion.
   Standardize token adoption, spacing, typography, radii, icon sizing, and interaction states.

2. Upgrade the visible shell-to-surface transitions.
   Make settings, startup, theme gallery, command palette, panels, notebook, and canvas visually match the strength of the shell chrome.

3. Remove all placeholder-looking UI on visible paths.
   Replace emoji prefixes, unicode icon stand-ins, stubs, and ad hoc fallback appearance.

4. Tune the premium reading and authoring experience.
   Focus on editor readability, syntax hierarchy, notebook output styling, and canvas interaction visuals.

5. Finish with motion, accessibility visuals, and consolidation.
   Unify focus states, reduced-motion behavior, hover and press language, and retire legacy style fragments.

## Acceptance Standard For The V26 Wave

The `v26` wave is successful when:

- all primary user-facing surfaces share a coherent premium visual language
- shell, editor, notebook, canvas, panels, settings, menus, and transient surfaces feel clearly designed as one product
- visible placeholder, stub, emoji, unicode-stand-in, and local one-off styling debt on the main UX path is removed or retired
- light and dark themes reach reliable cross-surface parity
- motion and accessibility visuals are deliberate, consistent, and restrained
- the product feels visually complete rather than merely implemented
