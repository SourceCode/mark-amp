# Phase 07: Syntax, Theme, Icon, And Visible UI Closure

## Phase Intent

Close only the visual issues that materially affect release-path trust and cohesion.

## Release-Ready Exit Criteria

- visible shell surfaces use the intended theme and icon systems consistently,
- syntax presentation is dependable on release-path editor and notebook surfaces,
- placeholder-looking UI is gone from primary workflows.

## Task Count

3

## Task P07-T01

- Phase ID: P07
- Task ID: P07-T01
- Task Title: Remove release-path placeholder visuals and icon inconsistencies from shell-critical surfaces
- Priority: P1
- Category: UI Quality / Closure
- Objective: eliminate obvious placeholder iconography, placeholder labels, and provisional shell chrome from release-critical surfaces.
- Why This Matters Now: visual placeholder residue directly signals incompleteness to users.
- Release Gap Statement: several release-path surfaces still present placeholder-looking controls and content.
- User / Product Impact: even working features feel unfinished when visible shell chrome looks provisional.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp`
- Prior Plan References: `v22 Phase 20`; `v24 Phase 07`
- Scope: canvas shell controls, panel placeholders, release-path empty/loading/error states, icon consistency in shell chrome.
- Out of Scope: full visual redesign of non-critical surfaces.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/IconManifest.cpp`; `/Users/ryanrentfro/code/markamp/src/core/MuiIconPipeline.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/SidebarSkeletonPlaceholder.cpp`
- Related Features / Systems / Components: icon system; theme system; empty states; shell chrome
- Current Behavior: placeholder copy and Unicode icon placeholders are still visible in release-path UI.
- Intended Release-Ready Behavior: the visible shell feels intentionally complete, not staged or provisional.
- Missing Pieces: icon replacement; state-copy cleanup; shell state audit.
- Technical Approach: prioritize visible release-path surfaces and replace placeholder visuals with canonical themed/iconized components.
- Implementation Steps:
1. Inventory visible placeholder UI on the release path.
2. Replace with canonical themed and iconized controls.
3. Remove no-longer-used placeholder helper components from release-path wiring.
- Validation Steps:
1. Manual UI sweep of primary shell workflows.
2. Visual regression or screenshot audit for critical surfaces.
- Acceptance Criteria: no obvious placeholder visuals remain in core shell, editor, notebook, canvas, explorer, or settings flows.
- Dependencies: P05-T01; P09-T01
- Parallelization Notes: can proceed once authoritative surface ownership is stable.
- Risks / Failure Modes: local styling overrides survive; inconsistent icon semantics.
- Release Notes / Cleanup Notes: document gated surfaces left intentionally unfinished and hidden.
- Observability / Diagnostics Notes: use visual audit snapshots where available.
- Rollback / Safety Notes: cosmetic-only changes should not alter behavior.
- References / Context: `docs/v22_docs/ASSESSMENT__MarkAmp_V22_UI_Quality_And_Visual_Cohesion_Assessment.md`

## Task P07-T02

- Phase ID: P07
- Task ID: P07-T02
- Task Title: Raise syntax-highlighting and token presentation to release-path dependability
- Priority: P1
- Category: Syntax Highlighting
- Objective: ensure editor and notebook code presentation reach dependable, non-regressing quality on the release path.
- Why This Matters Now: code readability and theme parity are core IDE trust signals.
- Release Gap Statement: syntax and token presentation still risk drift between editor, notebook, diff, and preview contexts.
- User / Product Impact: poor syntax presentation makes the product feel less than IDE-grade even when workflows function.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/core/SyntaxHighlighter.cpp`; `/Users/ryanrentfro/code/markamp/src/core/ThemeScopeMapper.cpp`
- Prior Plan References: `v20 Phase 07`; `v22 Phase 07`; `v24 Phase 07`
- Scope: editor tokens, notebook code cells, diff/read-only views, theme token parity.
- Out of Scope: broad language-engine expansion beyond current release-path languages.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/AsyncHighlighter.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/MinimapPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/CellOutputRenderer.cpp`
- Related Features / Systems / Components: syntax highlighter; theme engine; notebook code cells; minimap
- Current Behavior: syntax infrastructure exists, but parity and visible consistency still need closure.
- Intended Release-Ready Behavior: release-path code surfaces share dependable tokenization and readable theming.
- Missing Pieces: parity tests; notebook/editor token mapping alignment; theme regression checks.
- Technical Approach: define a release-path syntax matrix and validate token/color parity across main code surfaces.
- Implementation Steps:
1. Identify supported release-path language modes.
2. Align token mapping and visual treatment across editor and notebook code.
3. Add regression tests for theme token application and core syntax samples.
- Validation Steps:
1. Snapshot representative code in supported release-path modes.
2. Verify theme switching and minimap parity.
- Acceptance Criteria: supported release-path code surfaces present stable, readable, and theme-consistent syntax.
- Dependencies: P06-T02; P08-T02
- Parallelization Notes: can be split by editor and notebook owners.
- Risks / Failure Modes: large-file fallback regressions; unsupported-mode confusion.
- Observability / Diagnostics Notes: expose active lexer/highlighter and theme-scope mappings in debug info.
- Rollback / Safety Notes: degrade gracefully for unsupported modes, but document them clearly.
- References / Context: `docs/v20_docs/Phase_07__Syntax_Highlighting_And_Language_Presentation.md`

## Task P07-T03

- Phase ID: P07
- Task ID: P07-T03
- Task Title: Normalize release-path empty, loading, error, and disabled states across shell surfaces
- Priority: P1
- Category: UI Quality / Closure
- Objective: ensure state transitions look intentional and informative across the primary shell.
- Why This Matters Now: rough states are where incompleteness is most obvious.
- Release Gap Statement: state treatments remain inconsistent across panels, settings, notebook, and canvas surfaces.
- User / Product Impact: users read inconsistent states as instability or missing depth.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/SidebarSkeletonPlaceholder.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/StartupPanel.cpp`
- Prior Plan References: `v22 Phase 16`; `v22 Phase 20`; `v24 Phase 07`
- Scope: empty project state, empty panel state, notebook/canvas empty state, load and error messaging for release-path surfaces.
- Out of Scope: non-visible internal tools.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/NotificationManager.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/ThemedDialog.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/ExtensionsBrowserPanel.cpp`
- Related Features / Systems / Components: empty states; dialogs; shell messaging; error states
- Current Behavior: state treatments vary and sometimes still look placeholder-heavy.
- Intended Release-Ready Behavior: primary shell states are clear, themed, and consistent with the product’s finished feel.
- Missing Pieces: state-copy pass; token usage consistency; state-specific validation sweep.
- Technical Approach: define one state-treatment checklist for release-path surfaces and apply it selectively where state quality still affects trust.
- Implementation Steps:
1. Enumerate critical stateful shell surfaces.
2. Standardize message style and visual treatment.
3. Remove low-signal placeholder wording.
- Validation Steps:
1. Trigger empty/loading/error states manually or through tests.
2. Verify consistency with theme and panel conventions.
- Acceptance Criteria: release-path state surfaces no longer feel accidental or provisional.
- Dependencies: P05-T01
- Parallelization Notes: can proceed in parallel with final polish on individual surfaces.
- Risks / Failure Modes: copy drift; inconsistent fallback theming.
- Release Notes / Cleanup Notes: final state copy should be part of release-candidate cleanup.
- Observability / Diagnostics Notes: add state-transition traces for load/error states where helpful.
- Rollback / Safety Notes: UI-only unless tied to functional gating.
- References / Context: `docs/v22_docs/Phase_16__Startup_Welcome_Walkthrough_Empty_Loading_And_Error_States.md`
