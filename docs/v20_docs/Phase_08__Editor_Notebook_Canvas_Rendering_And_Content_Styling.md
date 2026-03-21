# Phase 08: Editor Notebook Canvas Rendering And Content Styling

## Outcome

Raise rendering quality, typography, spacing, selection, diagnostics, and embedded-content presentation across the editor, notebook, and canvas so the product feels like one premium environment.

## Improvement Count

210 atomic improvements across 6 execution tasks.

### P08-T01

- Phase ID: P08
- Task ID: P08-T01
- Task Title: Upgrade text editor rendering quality and chrome consistency
- Priority: P0
- Category: Styling / Visual Design
- Atomic Improvements Covered: 35
- Objective: Make the core code editor read and feel premium at rest and during editing.
- Why This Matters Now: The editor is still the quality bar users judge first.
- Problem Statement: Editor styling exists, but it still reads as markdown-first and uneven for IDE usage.
- User Impact: Weak visual quality makes the whole product feel less trustworthy.
- Scope: Fonts, line height, gutter, current line, selection, caret, bracket match, indent guides, whitespace, fold markers, minimap if present.
- Out of Scope: Editor engine replacement.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/core/ThemeEngine.cpp`; `/Users/ryanrentfro/code/markamp/themes`
- Related Systems / Components: Editor panel, theme engine, syntax highlighting, diagnostics.
- Current Behavior: Many style hooks exist, but the result is not yet elite IDE quality.
- Intended Behavior: The editor feels precise, calm, legible, and intentional in all major themes.
- Technical Approach: Normalize metric tokens, refine style application, and retune interactive states for code workloads.
- Implementation Steps: Tune fonts and spacing; refine gutter contrast; improve caret/selection/brackets; standardize search and diagnostic overlays.
- Validation Steps: Review editor under dense code, prose, markdown, and split view.
- Acceptance Criteria: Editor readability and state clarity materially improve in everyday use.
- Dependencies: Phase 07.
- Risks / Failure Modes: Overdesign reduces information density.
- UX Notes: Balance aesthetics with high-density professional editing.
- Styling / Highlighting Notes where relevant: Token colors must cooperate with current-line, selection, and diagnostics overlays.
- Observability / Diagnostics Notes: Add editor style and performance benchmarks for repaint-heavy states.
- Rollback / Safety Notes: Keep metrics tokenized and theme-driven.
- References / Context: `EditorPanel.cpp` already styles many markdown states but needs stronger code-editor baseline treatment.
- Example scenarios where useful: Reading a long C++ file in a split view remains comfortable for hours.

### P08-T02

- Phase ID: P08
- Task ID: P08-T02
- Task Title: Standardize notebook cell chrome, spacing, prompts, and execution-state styling
- Priority: P0
- Category: Notebook Workflow
- Atomic Improvements Covered: 35
- Objective: Give notebooks a polished, consistent cell presentation system that matches editor quality.
- Why This Matters Now: Notebook workflows will still feel second-class without visual parity.
- Problem Statement: Notebook shell integration is thin, and styling consistency is likely weak.
- User Impact: Computational workflows feel less serious than text editing workflows.
- Scope: Cell containers, active cell chrome, output areas, run states, error states, add-cell affordances, toolbar alignment.
- Out of Scope: Rich output rendering redesign.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/NotebookShellHost.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`; `/Users/ryanrentfro/code/markamp/themes`
- Related Systems / Components: Notebook host, theme engine, syntax highlighting, status surfaces.
- Current Behavior: Notebook visuals are not clearly governed by the same design system.
- Intended Behavior: Notebook cells feel premium, calm, and obviously part of the same IDE.
- Technical Approach: Apply shared spacing, focus, and token rules with notebook-specific chrome semantics.
- Implementation Steps: Define cell spacing; active/inactive cell states; prompt alignment; execution/output/error styling; hover and insert states.
- Validation Steps: Create and edit notebooks in light and dark themes with multiple cell types.
- Acceptance Criteria: Notebook styling feels deliberate and product-level, not add-on level.
- Dependencies: Phase 03, Phase 07.
- Risks / Failure Modes: Too much chrome competes with content.
- UX Notes: Active cell affordances should be discoverable but subtle.
- Styling / Highlighting Notes where relevant: Code cell highlighting must visually match editor semantics.
- Observability / Diagnostics Notes: Add notebook visual-state screenshot checks.
- Rollback / Safety Notes: Keep cell chrome tokenized to avoid per-theme drift.
- References / Context: Notebook shell code shows little evidence of a finished visual system today.
- Example scenarios where useful: Running a failing code cell surfaces a clear but elegant error style without visual chaos.

### P08-T03

- Phase ID: P08
- Task ID: P08-T03
- Task Title: Standardize canvas object, text, inspector, and embedded-content presentation
- Priority: P1
- Category: Canvas Workflow
- Atomic Improvements Covered: 35
- Objective: Bring canvas rendering and canvas-side content styling up to the same design bar.
- Why This Matters Now: The canvas is central to product differentiation and cannot look provisional.
- Problem Statement: Canvas shell uses placeholders and likely diverges from shared content styling rules.
- User Impact: Users perceive the canvas as a novelty surface rather than a serious work surface.
- Scope: Board background, grid, object selection states, text rendering, object inspector styling, embedded-code blocks, note cards, connection chrome.
- Out of Scope: New canvas object families.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp`
- Related Systems / Components: Canvas renderer, theme engine, syntax highlighting, icon system.
- Current Behavior: Canvas visual treatment is inconsistent and partially placeholder-based.
- Intended Behavior: Boards feel precise, expressive, and fully integrated with the IDE’s visual language.
- Technical Approach: Apply shared typography and tokens while giving canvas surfaces their own spatial identity.
- Implementation Steps: Normalize board chrome; refine object selection visuals; style embedded code/text; align inspector controls; improve minimap and zoom cues.
- Validation Steps: Review dense boards, empty boards, and boards with embedded text/code content.
- Acceptance Criteria: Canvas presentation feels productized and coherent with the rest of the app.
- Dependencies: Phase 04, Phase 07, Phase 09.
- Risks / Failure Modes: Visual richness harms performance or clarity.
- UX Notes: Selection, lock, and hover states must remain unmistakable.
- Styling / Highlighting Notes where relevant: Embedded code must inherit shared syntax and theme behavior.
- Observability / Diagnostics Notes: Add paint-cost and frame-time measurements for styled canvas states.
- Rollback / Safety Notes: Keep expensive effects optional and measurable.
- References / Context: Canvas shell styling today uses placeholder glyphs and sparse inspector/minimap treatment.
- Example scenarios where useful: A board containing notes, code blocks, and connectors remains readable and premium-looking.

### P08-T04

- Phase ID: P08
- Task ID: P08-T04
- Task Title: Normalize diagnostics, search, selection, and inline-feedback rendering across surfaces
- Priority: P1
- Category: Editor Workflow
- Atomic Improvements Covered: 35
- Objective: Make warnings, errors, search hits, current selection, and action feedback feel consistent everywhere.
- Why This Matters Now: Workflow trust depends on state treatment, not just normal-state styling.
- Problem Statement: Diagnostics and feedback are likely rendered differently in editor, notebook, canvas, and panels.
- User Impact: Users struggle to interpret system feedback quickly.
- Scope: Search-hit styling, find/replace highlights, error underlines, warning badges, inline save/error states, selection visuals.
- Out of Scope: Full diagnostics engine redesign.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/SearchSidebarPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/StatusBarPanel.cpp`
- Related Systems / Components: Diagnostics, search, status bar, theme engine.
- Current Behavior: Feedback styles appear subsystem-specific.
- Intended Behavior: Feedback semantics are visually consistent across the IDE.
- Technical Approach: Define shared feedback tokens and apply them through host adapters.
- Implementation Steps: Audit feedback states; define token set; align search highlights and diagnostics; add notebook/canvas adapters; test contrast and overlap.
- Validation Steps: Trigger errors, warnings, search matches, and selections in multiple surfaces.
- Acceptance Criteria: Feedback is easy to interpret and visually consistent.
- Dependencies: Phase 07 and Phase 09.
- Risks / Failure Modes: Too many visual layers reduce readability.
- UX Notes: Feedback should be prominent enough to help but quiet enough not to fatigue.
- Styling / Highlighting Notes where relevant: Search and diagnostics must remain legible atop syntax colors.
- Observability / Diagnostics Notes: Track theme-token collisions and unreadable-state reports.
- Rollback / Safety Notes: Preserve old tokens as debug fallback only.
- References / Context: Prior inspection already found split highlighting and fragmented status handling.
- Example scenarios where useful: Search hits in a notebook code cell and a text editor use the same semantic highlight family.

### P08-T05

- Phase ID: P08
- Task ID: P08-T05
- Task Title: Improve typography, spacing, and density tokens for content-heavy workflows
- Priority: P1
- Category: Styling / Visual Design
- Atomic Improvements Covered: 35
- Objective: Give the product a coherent content-density system instead of ad hoc control sizes.
- Why This Matters Now: Visual polish depends on repeatable spacing and type decisions.
- Problem Statement: Trees, tabs, toolbars, panels, notebooks, and inspectors likely use inconsistent metrics.
- User Impact: The product feels stitched together even when functions are present.
- Scope: Type scale, mono/proportional pairing, spacing scale, icon spacing, control heights, row density, panel padding.
- Out of Scope: Radical branding change.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/ThemeEngine.cpp`; `/Users/ryanrentfro/code/markamp/src/ui`; `/Users/ryanrentfro/code/markamp/themes`
- Related Systems / Components: Theme engine, icon system, shell chrome, editor, notebook, canvas.
- Current Behavior: Density and spacing vary visibly by subsystem.
- Intended Behavior: The entire app reads as one visually disciplined system.
- Technical Approach: Define spacing/type tokens and migrate major surfaces to them.
- Implementation Steps: Audit metrics; define shared scale; refit shell surfaces; refit content hosts; validate across DPI and platforms.
- Validation Steps: Review full app at multiple sizes and on multiple OS conventions.
- Acceptance Criteria: Major surfaces share coherent spacing, alignment, and typography.
- Dependencies: Phase 09.
- Risks / Failure Modes: Platform quirks may require tuned exceptions.
- UX Notes: Density options should support both focus and information-rich workflows.
- Styling / Highlighting Notes where relevant: Typography choices must support code legibility and markdown readability equally well.
- Observability / Diagnostics Notes: Add style inventory reports for off-scale controls.
- Rollback / Safety Notes: Keep old metrics only as migration shims.
- References / Context: Current UI surface quality still varies too much to beat premium IDEs.
- Example scenarios where useful: Explorer rows, notebook cells, tabs, and toolbars feel aligned and intentionally scaled together.

### P08-T06

- Phase ID: P08
- Task ID: P08-T06
- Task Title: Add rendering and visual-regression harnesses for content surfaces
- Priority: P0
- Category: Diagnostics / Regression Protection
- Atomic Improvements Covered: 35
- Objective: Keep editor, notebook, and canvas rendering quality stable as polish work lands.
- Why This Matters Now: Rendering polish is easy to erode with later functional fixes.
- Problem Statement: There is no visible release gate for cross-surface rendering coherence.
- User Impact: Subtle readability and chrome regressions can accumulate unnoticed.
- Scope: Screenshot smoke paths, metric assertions, contrast checks, DPI coverage, theme coverage, split-view coverage.
- Out of Scope: Exhaustive pixel diffs for every screen.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/tests/unit`; `/Users/ryanrentfro/code/markamp/scripts/smoke_test.sh`; `/Users/ryanrentfro/code/markamp/themes`
- Related Systems / Components: Testing, themes, editor, notebook, canvas.
- Current Behavior: Rendering quality is likely judged manually.
- Intended Behavior: CI can catch visible regressions in core content surfaces.
- Technical Approach: Add targeted golden screenshots and metrics-based visual assertions.
- Implementation Steps: Define screenshot set; capture baseline states; assert metrics and contrast; wire into smoke runs.
- Validation Steps: Run visual suite after theme and chrome changes.
- Acceptance Criteria: Regressions in content readability and state styling become obvious before release.
- Dependencies: P08-T01 through P08-T05.
- Risks / Failure Modes: Screenshot noise across platforms.
- UX Notes: Validate both normal and stressed states, not just pretty idle screens.
- Styling / Highlighting Notes where relevant: Include syntax-heavy, diagnostic-heavy, and empty-state-heavy scenes.
- Observability / Diagnostics Notes: Store visual diffs with commit metadata.
- Rollback / Safety Notes: Use tolerance windows and platform-specific baselines where needed.
- References / Context: Premium visual quality requires release gates, not taste alone.
- Example scenarios where useful: A theme change that shifts notebook prompt spacing or editor gutter contrast is caught automatically.

