# MarkAmp V22 UI Quality And Visual Cohesion Assessment

## Executive Summary

MarkAmp already contains a large amount of UI infrastructure: theme tokens, density metrics, custom-painted controls, shell chrome, tokenized editor styling, icon pipelines, themed dialogs, skeleton loaders, and panel frameworks. The problem is not the absence of UI systems. The problem is that the visible product still does not behave like one designed interface.

The repository shows a split between:

- shared UI infrastructure that intends consistency,
- and many local surfaces that still hardcode their own geometry, fonts, colors, motion, labels, placeholder treatments, and interaction styling.

`v22` therefore focuses entirely on making the interface intentional, premium, and cohesive.

## Core UI Findings

### 1. Design-System Intent Exists, But Enforcement Is Weak

- [UILayoutTokens.h](/Users/ryanrentfro/code/markamp/src/ui/UILayoutTokens.h) defines spacing, icon, toolbar, menu, tab, empty-state, and interaction timing tokens.
- [LayoutMetrics.h](/Users/ryanrentfro/code/markamp/src/ui/LayoutMetrics.h) defines another density-driven geometry layer.
- [ThemeTokens.h](/Users/ryanrentfro/code/markamp/src/core/ThemeTokens.h) provides shell, syntax, editor, minimap, notebook, and control-state tokens.
- [ThemeEngine.cpp](/Users/ryanrentfro/code/markamp/src/core/ThemeEngine.cpp) maps those tokens into runtime colors.

Conclusion: the codebase contains the beginnings of a real UI system, but many visible surfaces do not reliably consume it.

### 2. High-Visibility Shell Surfaces Still Style Themselves Locally

- [Toolbar.cpp](/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp) hardcodes labeled button sets, tooltip strings, local animation rules, and surface composition.
- [ActivityBar.cpp](/Users/ryanrentfro/code/markamp/src/ui/ActivityBar.cpp) custom-paints and manages its own spacing, badges, overflow, and tooltip logic.
- [TabBar.cpp](/Users/ryanrentfro/code/markamp/src/ui/TabBar.cpp) contains its own fade animation, tab geometry, drag/drop behavior, and context-menu presentation.
- [StatusBarPanel.cpp](/Users/ryanrentfro/code/markamp/src/ui/StatusBarPanel.cpp) custom-builds a dense event-driven status surface with its own tooltip and item layout logic.

Conclusion: shell chrome is ambitious, but the product still lacks one enforced visual grammar for the workbench frame.

### 3. Editor Styling Is Powerful But Still Local And Uneven

- [EditorPanel.cpp](/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp) contains a very large amount of local presentation logic for margins, whitespace, fold markers, diagnostics, minimap, selection, caret, active line, annotations, indicators, and markdown styling.
- The editor clearly has polish work in progress, but it is still manually tuned subsystem code rather than a clearly extracted surface system.
- [MinimapPanel.cpp](/Users/ryanrentfro/code/markamp/src/ui/MinimapPanel.cpp) uses a simplified syntax-color mapping and tiny local rendering assumptions that visually lag behind the complexity of the main editor.

Conclusion: editor presentation has capability, but not enough visual discipline or parity across its sub-surfaces.

### 4. Notebook Presentation Is Mostly Intent, Not Yet a Premium Surface

- [NotebookCellChrome.h](/Users/ryanrentfro/code/markamp/src/core/NotebookCellChrome.h) and [NotebookCellChrome.cpp](/Users/ryanrentfro/code/markamp/src/core/NotebookCellChrome.cpp) define visual state concepts, but those concepts are still model-level.
- [NotebookToolbar.cpp](/Users/ryanrentfro/code/markamp/src/core/NotebookToolbar.cpp) is a command registry, not a visible premium toolbar.
- [NotebookShellHost.cpp](/Users/ryanrentfro/code/markamp/src/core/NotebookShellHost.cpp) is still a thin state/notification host.
- [CellOutputRenderer.cpp](/Users/ryanrentfro/code/markamp/src/ui/CellOutputRenderer.cpp) models output rendering decisions, but not a visually complete notebook output surface.

Conclusion: notebook UI is not yet visually comparable to elite notebook products.

### 5. Canvas Shell UI Is Still Visually Primitive

- [CanvasWorkspacePanel.cpp](/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp) uses Unicode placeholder icons, generic `wxButton` controls, a simple context bar, a placeholder inspector, and a thin minimap strip.
- This is not yet a premium canvas shell.
- Meanwhile [CanvasWorkbench.cpp](/Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp), [CanvasRenderer.cpp](/Users/ryanrentfro/code/markamp/src/canvas/CanvasRenderer.cpp), [CanvasInspector.cpp](/Users/ryanrentfro/code/markamp/src/canvas/CanvasInspector.cpp), [CanvasToolHost.cpp](/Users/ryanrentfro/code/markamp/src/canvas/CanvasToolHost.cpp), and related canvas files imply a much richer product direction than the visible shell currently expresses.

Conclusion: canvas needs a major UI promotion from functional shell to world-class visual environment.

### 6. Settings, Theme, Dialog, and Startup Surfaces Still Feel Utility-Grade

- [SettingsPanel.cpp](/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp) uses native notebooks, search controls, category trees with emoji labels, and mixed local styling.
- [ThemeGallery.cpp](/Users/ryanrentfro/code/markamp/src/ui/ThemeGallery.cpp) is useful but visually inconsistent in typography, uppercase language, local sizing, and button treatment.
- [ThemedDialog.cpp](/Users/ryanrentfro/code/markamp/src/ui/ThemedDialog.cpp) provides a basic shell, but dialog hierarchy, footers, content rhythm, and state treatments are still minimal.
- [StartupPanel.cpp](/Users/ryanrentfro/code/markamp/src/ui/StartupPanel.cpp) still hardcodes a default purple button color and local fonts, and [StartupPanel_styling.txt](/Users/ryanrentfro/code/markamp/src/ui/StartupPanel_styling.txt) explicitly says styling updates are still pending.

Conclusion: important high-trust surfaces still look interim.

### 7. Data-Dense Views And Specialized Panels Are Uneven Or Visually Thin

- [AVTablePanel.cpp](/Users/ryanrentfro/code/markamp/src/ui/av/AVTablePanel.cpp) remains extremely thin.
- Explorer, Search, Problems, Output, Terminal, Debug, and Source Control surfaces each carry their own visual assumptions.
- [SidebarSkeletonPlaceholder.cpp](/Users/ryanrentfro/code/markamp/src/ui/SidebarSkeletonPlaceholder.cpp) and multiple placeholder-heavy panel shells indicate unfinished UI treatment in several workbench zones.

Conclusion: dense workbench surfaces need systematic hierarchy, rhythm, and state polish.

### 8. Notifications, Overlays, And Motion Are Present But Not Yet Unified

- [NotificationManager.cpp](/Users/ryanrentfro/code/markamp/src/ui/NotificationManager.cpp) custom-renders toasts with local layout and animation.
- [CommandPalette.cpp](/Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp), [TooltipWindow.cpp](/Users/ryanrentfro/code/markamp/src/ui/TooltipWindow.cpp), [RichTooltip.cpp](/Users/ryanrentfro/code/markamp/src/ui/RichTooltip.cpp), [SpotlightOverlay.cpp](/Users/ryanrentfro/code/markamp/src/ui/SpotlightOverlay.cpp), and [TabSwitcherOverlay.cpp](/Users/ryanrentfro/code/markamp/src/ui/TabSwitcherOverlay.cpp) all express transient UI in slightly different ways.

Conclusion: motion and transient-surface quality is promising but fragmented.

### 9. Iconography Is More Mature Than Before, But Still Not Finished

- [IconMetricsPolicy.cpp](/Users/ryanrentfro/code/markamp/src/core/IconMetricsPolicy.cpp) and [MuiIconPipeline.cpp](/Users/ryanrentfro/code/markamp/src/core/MuiIconPipeline.cpp) show real icon-system work.
- But icon semantics and visual alignment remain inconsistent across workbench surfaces.
- Canvas still uses Unicode placeholders.
- Some shell surfaces still appear to assume local icon spacing instead of one optical system.

Conclusion: icon migration is not just a technical migration; it still needs visual completion and semantic normalization.

## Highest-Priority UI Problems To Solve Next

1. Consolidate design tokens, density rules, typography scales, icon metrics, and control states into one truly enforced UI language.
2. Rebalance the shell layout, toolbar, activity bar, tabs, status bar, and sidebars so the product immediately feels more premium.
3. Raise editor readability and syntax-highlighting presentation to elite quality with better code color hierarchy, minimap polish, diagnostics styling, and line-state treatments.
4. Create a real notebook visual system instead of leaving notebook chrome and output presentation mostly in model form.
5. Rebuild the canvas shell UI so tools, properties, minimap, object chrome, and empty states feel intentional and polished.
6. Make settings, dialogs, notifications, startup, and data views visually strong enough to match the ambition of the rest of the product.
7. Use accessibility, theme parity, motion restraint, and icon normalization as product-wide quality gates rather than afterthought polish.

## What Still Blocks The UI From Feeling Finished

- Too many visible surfaces still hardcode visual decisions locally.
- Layout rhythm and density vary noticeably between shell, panels, dialogs, and content surfaces.
- Typography and hierarchy are not product-wide enough.
- Notebook and canvas presentation still lag behind the editor in maturity.
- Dense workbench panels do not yet share one row, state, and metadata language.
- Theme tokens are not applied consistently enough across all surfaces.
- Micro-interactions, transitions, and feedback surfaces are not unified.
- Placeholder or temporary-looking UI still exists in visible pathways.

## V22 Acceptance Lens

`v22` succeeds only if the implementation team can use it to:

- make the whole interface feel like one intentional product,
- remove subsystem-local visual drift,
- materially improve editor, notebook, and canvas presentation,
- raise panels, dialogs, settings, and onboarding to premium quality,
- complete theme and icon parity,
- and establish enough UI-system discipline that future work does not regress back into local styling.

