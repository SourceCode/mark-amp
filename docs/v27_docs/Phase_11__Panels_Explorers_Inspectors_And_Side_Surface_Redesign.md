# Phase 11 - Panels Explorers Inspectors And Side Surface Redesign

## Goal

Redesign the full panel fleet so side surfaces, dense views, inspectors, and secondary tools all feel like first-class parts of the new visual system.

## Definition Of Done

- all panel classes share a clearly redesigned chrome language
- dense information surfaces become more readable and more premium
- panel-quality gaps no longer weaken the overall shell identity

## Tasks

### Phase ID: V27-P11
### Task ID: V27-P11-T01
### Task Title: Redesign Panel Chrome Headers Actions And Dense Shell Structure
- Priority: P0
- Category: Panel UI
- Objective: rebuild panel shells, headers, action rows, and section structure around the new visual system and icon strategy.
- Why This Matters Now: panels are among the biggest remaining sources of visual variance in the product.
- Visual Gap Statement: panel chrome is uneven across explorer, search, problems, output, terminal, and inspector-like surfaces.
- User Experience Impact: redesigned panel shells make the whole app feel more premium and more cohesive in multi-panel layouts.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/ExplorerPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/SearchSidebarPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/ProblemsPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/OutputPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/TerminalPanel.cpp`
- Prior Plan References: `v22` panel phases, `v25` panel release closure, `v26` Phase 05
- Scope: panel shell geometry, headers, action icons, section labels, metadata zones, dense-region framing
- Out of Scope: panel behavior or command logic
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/ExplorerPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/SearchSidebarPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/ProblemsPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/OutputPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/TerminalPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/PanelHeaderIcons.cpp`
- Related Screens / Components / Surfaces: side panels, bottom panels, inspector panels, output-like surfaces
- Current Visual Behavior / Appearance: some panels are decent, but the fleet still feels partly heterogeneous and evolution-heavy rather than redesign-led.
- Intended Visual Behavior / Appearance: all panels feel like members of one newly designed family with strong header and shell consistency.
- Icon / Visual Design Direction: sharper premium panels with canonical action icons, cleaner separators, and calmer density.
- Technical Styling Approach: redesign panel shell primitives first, then apply them across panel classes with surface-specific content rules.
- Implementation Steps:
  1. Redesign panel header and shell primitives.
  2. Replace mixed icon references with canonical mappings.
  3. Rebuild sectioning, metadata, and action density.
  4. Validate complex multi-panel workspace configurations.
- Validation Steps:
  1. Open explorer, search, problems, terminal, and output together.
  2. Compare headers, actions, and content framing.
  3. Verify the redesigned family holds in narrow and wide panels.
- Acceptance Criteria: the panel fleet feels coherently redesigned and no major panel looks visually older than the shell.
- Dependencies: Phase 01 through Phase 06
- Parallelization Notes: can proceed with structured data redesign once panel shell primitives are defined.
- Risks / Failure Modes: overly ornate panels can make multi-panel workspaces visually heavy.
- Accessibility / Readability Notes: action targets and title clarity must remain strong in compact sidebars.
- Theme / Styling Notes: panel shell roles must align with redesigned shell depth and divider language.
- Motion / Interaction Notes where relevant: later panel transitions should be built on these shells.
- Cleanup / Consolidation Notes where relevant: remove panel-local header variants that no longer belong in the redesigned family.
- Rollback / Safety Notes: preserve panel functionality while reauthoring presentation.
- References / Context: panel redesign is critical because panels make up so much of the visible app.
- Example scenarios where useful: explorer and problems should feel as if they were designed by the same team at the same time.

### Phase ID: V27-P11
### Task ID: V27-P11-T02
### Task Title: Redesign Explorer Search Problems And Inspector Style Dense Surfaces
- Priority: P0
- Category: Panel UI
- Objective: rework the product’s most important dense side surfaces for better hierarchy, scannability, and visual maturity.
- Why This Matters Now: dense surfaces are where products most often lose perceived quality even after shell polish.
- Visual Gap Statement: explorer, search, and problems are still among the most likely places to expose temporary cues, uneven row hierarchy, and low-premium treatment.
- User Experience Impact: better dense panels make real day-to-day workflows feel faster and more trustworthy.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/ExplorerPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/SearchSidebarPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/ProblemsPanel.cpp`
- Prior Plan References: `v22` panel/list phases, `v26` Phase 05 and Phase 12
- Scope: row hierarchy, badges, metadata, snippets, inspector grid presentation, empty sections, dense actions
- Out of Scope: search or diagnostic backend changes
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/ExplorerPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/SearchSidebarPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/ProblemsPanel.cpp`
- Related Screens / Components / Surfaces: file explorer, search results, problems list, property or inspector-like dense panels
- Current Visual Behavior / Appearance: these surfaces function, but they still carry signs of uneven maturity and local styling.
- Intended Visual Behavior / Appearance: dense panels feel more refined, better structured, and more aligned with high-end IDE standards.
- Icon / Visual Design Direction: compact but elegant rows with stronger hierarchy and more disciplined supporting metadata.
- Technical Styling Approach: redesign dense-row and dense-section patterns as reusable panel content systems.
- Implementation Steps:
  1. Redesign row density, hierarchy indicators, and metadata presentation.
  2. Replace any glyph/icon residue in dense rows or headers.
  3. Improve empty sections and inspector-like property groups.
  4. Validate scan speed and visual calm in populated states.
- Validation Steps:
  1. Populate explorer, search, and problems with realistic data.
  2. Compare row readability and severity treatment.
  3. Verify dense content remains readable in narrow sidebars.
- Acceptance Criteria: dense side surfaces feel redesigned, not like tuned legacy list controls.
- Dependencies: V27-P11-T01, Phase 14
- Parallelization Notes: can overlap with structured information redesign once row patterns are aligned.
- Risks / Failure Modes: too much spaciousness can damage density efficiency; too little will preserve cramped utility aesthetics.
- Accessibility / Readability Notes: selection, focus, and severity cues must remain distinct and visible.
- Theme / Styling Notes: metadata and badge contrast must remain subtle but legible.
- Motion / Interaction Notes where relevant: row hover and selection transitions later should stay low-noise.
- Cleanup / Consolidation Notes where relevant: retire local dense-row variations that no longer fit the new system.
- Rollback / Safety Notes: preserve content density where it materially affects workflow productivity.
- References / Context: these panels are a direct professionalism test for the redesign.
- Example scenarios where useful: a busy search-results list should feel cleaner and more premium, not more spacious but less useful.

### Phase ID: V27-P11
### Task ID: V27-P11-T03
### Task Title: Redesign Secondary And Lower Maturity Panels To Eliminate Visual Weak Links
- Priority: P1
- Category: Panel UI
- Objective: raise lower-maturity or stub-adjacent panels to a redesign-consistent visual standard so no surface breaks the new identity.
- Why This Matters Now: secondary surfaces can still destroy perceived premium quality if left visually old or thin.
- Visual Gap Statement: history, PDF, debug, build, and other less-central panels are more likely to preserve older or placeholder visual DNA.
- User Experience Impact: users feel a more complete and reliable product when even rare panels look intentional.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/HistoryPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/PDFViewerPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/BuildPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/DebugConsolePanel.cpp`
- Prior Plan References: `v23` and `v25` visible surface completion, `v26` Phase 05
- Scope: shell redesign, header alignment, empty/loading/error states, secondary surface parity with main panel family
- Out of Scope: deep feature completion for those panels
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/HistoryPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/PDFViewerPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/BuildPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/DebugConsolePanel.cpp`
- Related Screens / Components / Surfaces: history, PDF, build, debug console, secondary panel classes
- Current Visual Behavior / Appearance: these surfaces are more likely to show thin shells, older icon usage, or placeholder-like presentation.
- Intended Visual Behavior / Appearance: even low-frequency panels feel clearly redesigned and integrated into the new visual world.
- Icon / Visual Design Direction: honest but premium secondary surfaces with consistent shell and icon behavior.
- Technical Styling Approach: apply the new panel family plus canonical icon system, then tailor only the content-specific portions that need unique treatment.
- Implementation Steps:
  1. Identify lower-maturity panel shells and icon residue.
  2. Apply redesigned panel family and edge-state systems.
  3. Replace secondary-surface legacy icon and style paths.
  4. Validate parity with main panel classes.
- Validation Steps:
  1. Open each panel alone and alongside primary panels.
  2. Confirm no surface still looks clearly older or more temporary.
  3. Review theme parity and edge states.
- Acceptance Criteria: no secondary panel remains an obvious visual weak link after the redesign.
- Dependencies: V27-P11-T01, Phase 03
- Parallelization Notes: can run after core panel family patterns are established.
- Risks / Failure Modes: over-finishing visibly incomplete functionality can create false expectations; the visual design should remain honest but polished.
- Accessibility / Readability Notes: even rare surfaces need readable titles, actions, and edge states.
- Theme / Styling Notes: maintain parity with primary panel shells.
- Motion / Interaction Notes where relevant: future panel transitions should not assume only high-traffic panels deserve polish.
- Cleanup / Consolidation Notes where relevant: remove stale placeholder visuals and icon glyphs in secondary surfaces.
- Rollback / Safety Notes: avoid conflating visual redesign with unsupported feature claims.
- References / Context: `v27` must not leave a long tail of visually old side surfaces.
- Example scenarios where useful: opening the history panel should not feel like stepping into a different, older application.
