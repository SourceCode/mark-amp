# Phase 07 - Syntax Highlighting, Theme, Icon, And Visual System Closure

## Phase Goal

Complete the shared presentation systems only after the authoritative editor and shell surfaces are identified, so syntax, theme, icon, and visual language work stops fragmenting.

## Measurable Outcome

- Syntax-highlighting quality is consistent across editor, notebook, preview, and canvas code surfaces.
- Theme and icon systems are enforced rather than optional.
- Legacy local visual decisions are retired from critical shell surfaces.

### Task P07-T01

- Phase ID: `P07`
- Task ID: `P07-T01`
- Task Title: Make one highlighting pipeline authoritative across code-bearing surfaces
- Priority: `P0`
- Category: `Syntax Highlighting`
- Objective: Stop editor, notebook, preview, diff, and canvas code surfaces from drifting visually and semantically.
- Why This Matters Now: The repo already has multiple highlighting and theming seams that need convergence.
- Execution Gap Statement: Editor and auxiliary surfaces are not clearly driven by the same highlighter and theme mapping contracts.
- User / Product Impact: Code readability and trust vary too much by surface.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/core/SyntaxHighlighter.cpp`, `/Users/ryanrentfro/code/markamp/src/core/AsyncHighlighter.cpp`
- Prior Plan References: `v20` Phase 07-08, `v22` Phase 07 and 18
- Scope: Tokenization, semantic token mapping, surface adapters
- Out of Scope: New language feature scope beyond current supported stack
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/ThemeScopeMapper.cpp`, `/Users/ryanrentfro/code/markamp/src/core/ThemeEngine.cpp`
- Related Features / Systems / Components: Syntax highlighting, theme tokens, code surface adapters
- Current Behavior: Different code surfaces still risk different token sources and theme application paths.
- Intended Completed Behavior: One highlighting contract feeds all code-bearing surfaces.
- Missing Pieces: Shared token pipeline adoption and per-surface adapters
- Technical Approach: Treat token generation and token-to-theme mapping as shared services with surface-specific renderers only.
- Implementation Steps: Normalize token model; adopt across editor/notebook/diff/preview/canvas; unify theme lookup.
- Validation Steps: Compare the same code sample across all code-bearing surfaces in light and dark themes.
- Acceptance Criteria: Shared code samples present consistent token hierarchy across surfaces.
- Dependencies: `P06-T01`
- Parallelization Notes: Can progress alongside notebook and canvas surface work.
- Risks / Failure Modes: Performance regressions on large files or embedded-code surfaces.
- Cleanup / Migration Notes: Remove local token-color shortcuts from auxiliary surfaces.
- Observability / Diagnostics Notes: Emit tokenization source, latency, and fallback mode.
- Rollback / Safety Notes: Allow controlled fallback for very large files, but preserve readable syntax hierarchy.
- References / Context: Theme scope mapping and async highlighting
- Example scenarios where useful: A Python snippet looks the same in editor, notebook cell, and canvas embed.

### Task P07-T02

- Phase ID: `P07`
- Task ID: `P07-T02`
- Task Title: Enforce shared theme-token usage in shell chrome and critical panels
- Priority: `P1`
- Category: `UI Quality / Styling`
- Objective: Make shell and panel surfaces consume the shared design system rather than local paint rules.
- Why This Matters Now: UI quality work only becomes durable when local overrides stop winning.
- Execution Gap Statement: Toolbar, activity bar, tab bar, status bar, and panels still contain many local visual decisions.
- User / Product Impact: The product still looks like multiple systems.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/ActivityBar.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/TabBar.cpp`
- Prior Plan References: `v22` Phase 01-05 and 20
- Scope: Shared spacing, typography, control state, shell chrome, panel headers
- Out of Scope: Purely decorative experimentation
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/UILayoutTokens.h`, `/Users/ryanrentfro/code/markamp/src/ui/LayoutMetrics.h`
- Related Features / Systems / Components: Design system, shell UI, panel chrome
- Current Behavior: Shared tokens exist but are not consistently enforced.
- Intended Completed Behavior: High-visibility shell surfaces consume one design-system contract.
- Missing Pieces: Surface adoption and lint/audit enforcement
- Technical Approach: Replace local geometry/color/font constants with design-token reads and shared style helpers.
- Implementation Steps: Inventory local overrides; migrate shell surfaces; add consistency checks.
- Validation Steps: Inspect theme parity and density parity across shell surfaces.
- Acceptance Criteria: Critical shell surfaces no longer hardcode divergent spacing, colors, or icon metrics without documented exceptions.
- Dependencies: `P05-T01`
- Parallelization Notes: Can proceed with icon normalization and notebook/canvas visual convergence.
- Risks / Failure Modes: Aggressive token replacement can regress nuanced surface behavior if not validated carefully.
- Cleanup / Migration Notes: Retire duplicate token or metric layers when one source wins.
- Observability / Diagnostics Notes: Add style-audit reports for token usage coverage.
- Rollback / Safety Notes: Keep migration increments small per surface.
- References / Context: `v22` UI system findings
- Example scenarios where useful: Toolbar and panel headers share the same spacing rhythm and icon/button state language.

### Task P07-T03

- Phase ID: `P07`
- Task ID: `P07-T03`
- Task Title: Complete MUI icon normalization and remove legacy visual residue
- Priority: `P1`
- Category: `UI Quality / Styling`
- Objective: Finish icon migration as a semantic and visual system, not just a technical asset swap.
- Why This Matters Now: Mixed icon usage still signals incompletion.
- Execution Gap Statement: Some surfaces still use Unicode placeholders or legacy icon spacing assumptions.
- User / Product Impact: Inconsistent iconography makes the product look unfinished and reduces discoverability.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp`, `/Users/ryanrentfro/code/markamp/src/core/MuiIconPipeline.cpp`
- Prior Plan References: `v19` Phase 10, `v20` Phase 09, `v22` Phase 20
- Scope: Icon semantics, sizing, spacing, labels, theme behavior
- Out of Scope: New illustration system
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/IconMetricsPolicy.cpp`, `/Users/ryanrentfro/code/markamp/resources/icons/lucide/`
- Related Features / Systems / Components: Iconography, shell controls, canvas UI
- Current Behavior: Iconography is improved but not yet fully normalized.
- Intended Completed Behavior: Icon usage is semantically consistent and optically aligned across the product.
- Missing Pieces: Final migration inventory, replacement policy, per-surface cleanup
- Technical Approach: Audit every visible icon surface, replace residue, and standardize metrics and labels.
- Implementation Steps: Inventory legacy icons; replace Unicode placeholders; normalize icon-only button semantics; validate theme parity.
- Validation Steps: Visual review plus automated coverage report of legacy icon usage.
- Acceptance Criteria: No production-critical surface uses placeholder or semantically inconsistent icons.
- Dependencies: `P07-T02`
- Parallelization Notes: Can proceed surface-by-surface in parallel.
- Risks / Failure Modes: Blind replacement can degrade meaning if semantic mapping is not reviewed.
- Cleanup / Migration Notes: Remove unused legacy icon assets and helpers after migration.
- Observability / Diagnostics Notes: Report remaining non-MUI or placeholder icon usage.
- Rollback / Safety Notes: Preserve per-surface semantic mapping documentation.
- References / Context: Icon metrics and pipeline infrastructure
- Example scenarios where useful: Canvas toolbar no longer uses Unicode glyphs while shell toolbars use MUI icons.

### Task P07-T04

- Phase ID: `P07`
- Task ID: `P07-T04`
- Task Title: Normalize accessibility-critical visual states across the shell
- Priority: `P1`
- Category: `UI Quality / Styling`
- Objective: Make focus, selection, hover, disabled, and error states consistent and readable everywhere.
- Why This Matters Now: Visual polish is incomplete unless state communication is reliable.
- Execution Gap Statement: Different shell and content surfaces still express interaction states differently.
- User / Product Impact: Users can miss active context, keyboard focus, or disabled reasons.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/FocusRingRenderer.h`, `/Users/ryanrentfro/code/markamp/src/ui/accessibility/AccessibilityController.h`
- Prior Plan References: `v22` Phase 17 and 19
- Scope: Focus rings, selected rows, active tabs, disabled states, warning/error visuals
- Out of Scope: Screen-reader string authoring beyond visual state hooks
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/FileTreeCtrl.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/StatusBarPanel.cpp`
- Related Features / Systems / Components: Accessibility visuals, state styling, shell clarity
- Current Behavior: State language is surface-local and uneven.
- Intended Completed Behavior: State visuals are consistent enough to read the whole shell at a glance.
- Missing Pieces: Shared state tokens and surface adoption
- Technical Approach: Define shared state treatments and apply them to shell and dense panels first.
- Implementation Steps: Inventory state treatments; define tokenized patterns; migrate high-traffic surfaces; test contrast and clarity.
- Validation Steps: Keyboard navigation and high-contrast visual checks across major surfaces.
- Acceptance Criteria: Focus and selection states are visible, consistent, and theme-correct across the product.
- Dependencies: `P07-T02`
- Parallelization Notes: Can progress continuously once design-token enforcement starts.
- Risks / Failure Modes: Over-strong states can create visual noise if not balanced.
- Cleanup / Migration Notes: Remove local focus/selection paint rules where shared state helpers exist.
- Observability / Diagnostics Notes: Track token usage coverage for interaction states.
- Rollback / Safety Notes: Keep reduced-motion and accessibility theme overrides compatible.
- References / Context: Accessibility and focus rendering infrastructure
- Example scenarios where useful: Keyboard focus on toolbar, explorer tree, editor group, and settings controls all uses one clear visual language.

### Task P07-T05

- Phase ID: `P07`
- Task ID: `P07-T05`
- Task Title: Gate UI-system regressions with token and surface audits
- Priority: `P2`
- Category: `Testing / Regression Protection`
- Objective: Keep newly unified visual systems from drifting back into local overrides.
- Why This Matters Now: UI quality work will regress quickly without enforcement.
- Execution Gap Statement: The repository has design intent but weak enforcement.
- User / Product Impact: Visual drift would reappear as new work lands.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/UILayoutTokens.h`, `/Users/ryanrentfro/code/markamp/src/core/ThemeTokens.h`
- Prior Plan References: `v22` Phase 20
- Scope: Token-usage audits, icon audits, surface-consistency checks
- Out of Scope: Full design review automation
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/testing/VisualRegressionRunner.cpp`
- Related Features / Systems / Components: UI-system enforcement, regression testing
- Current Behavior: Visual consistency is manually maintained.
- Intended Completed Behavior: Automated checks catch major regressions in token usage and icon/state consistency.
- Missing Pieces: Audit scripts, CI hooks, baseline expectations
- Technical Approach: Add lightweight static and runtime audits for shared UI-system adoption.
- Implementation Steps: Build token-usage reports; add icon residue checks; add visual smoke targets for key surfaces.
- Validation Steps: Introduce controlled regressions and verify gate output.
- Acceptance Criteria: Major shell surfaces and code-bearing surfaces are covered by UI consistency audits.
- Dependencies: `P07-T02`, `P07-T03`, `P19-T02`
- Parallelization Notes: Can evolve while other surface work lands.
- Risks / Failure Modes: Overly noisy audits may be ignored if not tuned.
- Cleanup / Migration Notes: Replace manual checklist-only UI verification where audits are reliable.
- Observability / Diagnostics Notes: Publish per-build token/icon audit summaries.
- Rollback / Safety Notes: Start with warning mode and tighten thresholds gradually.
- References / Context: Visual regression runner and design-system layers
- Example scenarios where useful: A newly added toolbar button with wrong icon size is flagged before release.
