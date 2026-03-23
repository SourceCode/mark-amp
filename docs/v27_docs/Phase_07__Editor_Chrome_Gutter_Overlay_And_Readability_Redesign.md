# Phase 07 - Editor Chrome Gutter Overlay And Readability Redesign

## Goal

Redesign the editor’s visible chrome and support surfaces so the core reading and authoring experience looks freshly authored, premium, and unmistakably superior to the current state.

## Definition Of Done

- editor support chrome feels visually reauthored
- gutters, line numbers, overlays, and support elements no longer feel like layered legacy additions
- code and markdown reading comfort are materially improved

## Tasks

### Phase ID: V27-P07
### Task ID: V27-P07-T01
### Task Title: Redesign Editor Surface Shell Gutter And Reading Rhythm
- Priority: P0
- Category: Editor UI
- Objective: reauthor the editor’s shell and supporting chrome for a more premium reading experience.
- Why This Matters Now: The editor is the heart of the product and must feel fully redesigned for `v27` to be credible.
- Visual Gap Statement: the editor is capable and partly polished, but still reveals local styling growth and a convergence-era visual structure.
- User Experience Impact: Better editor presentation improves comfort, trust, and the product’s overall premium identity.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/MinimapPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/core/ThemeTokens.h`
- Prior Plan References: `v22` Phase 06, `v26` Phase 06
- Scope: editor shell, gutters, line numbers, active line support, ruler and whitespace support chrome, spacing and density around content
- Out of Scope: syntax token colors, handled in Phase 08
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/MinimapPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/core/ThemeTokens.h`
- Related Screens / Components / Surfaces: markdown editor, code editor, split editors, minimap-adjacent region
- Current Visual Behavior / Appearance: editor support chrome is strong but still feels like evolved implementation rather than a redesign-led reading system.
- Intended Visual Behavior / Appearance: the editor feels calmer, cleaner, and more luxurious while preserving productivity density.
- Icon / Visual Design Direction: quiet high-end editor with support chrome that is useful but visually recessive and precise.
- Technical Styling Approach: redesign editor-specific UI roles and support-surface tokens before revisiting secondary editor features.
- Implementation Steps:
  1. Audit visible editor support chrome and identify inherited visual seams.
  2. Redesign gutter, line number, spacing, and support-surface hierarchy.
  3. Update shell and support roles in the editor panel.
  4. Validate markdown and code views under the new geometry and visual language.
- Validation Steps:
  1. Compare long-form markdown and dense code samples.
  2. Review editor at standard and larger scale factors.
  3. Confirm the redesign remains calm in split editor layouts.
- Acceptance Criteria: editor support chrome feels visibly reauthored and materially more premium than the current version.
- Dependencies: Phase 02 and Phase 04
- Parallelization Notes: should run with syntax redesign planning but independently from token-color tuning.
- Risks / Failure Modes: over-minimizing support chrome can hurt orientation for heavy editing workflows.
- Accessibility / Readability Notes: line-number and active-region support still need enough contrast for low-vision use.
- Theme / Styling Notes: editor shell roles must be semantically grounded for later parity work.
- Motion / Interaction Notes where relevant: no motion dependency yet.
- Cleanup / Consolidation Notes where relevant: remove older local chrome branches after the new roles land.
- Rollback / Safety Notes: keep editing behavior unchanged while redesigning appearance.
- References / Context: editor redesign is a flagship quality requirement, not optional polish.
- Example scenarios where useful: a large C++ file should feel easier to read and more elegant even before syntax colors are reconsidered.

### Phase ID: V27-P07
### Task ID: V27-P07-T02
### Task Title: Redesign Dynamic Editor States For Selection Caret Diagnostics Search And Matching
- Priority: P0
- Category: Editor UI
- Objective: rebuild dynamic editor-state presentation so interactions feel more precise, premium, and coordinated.
- Why This Matters Now: The editor only feels truly redesigned when its live states are as thoughtfully reauthored as its static chrome.
- Visual Gap Statement: selections, carets, search matches, diagnostics, and bracket matching currently work, but may still feel locally tuned rather than systemically redesigned.
- User Experience Impact: Better dynamic states improve confidence and make long editing sessions feel more refined.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp`
- Prior Plan References: `v22` editor and overlay phases, `v26` Phase 06
- Scope: caret, selection, bracket matches, search results, diagnostics, current-line emphasis, overlapping-state behavior
- Out of Scope: diagnostics or search backend logic
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/core/ThemeTokens.h`
- Related Screens / Components / Surfaces: find/replace, diagnostics markers, selection ranges, bracket matching, current line
- Current Visual Behavior / Appearance: dynamic states are present but likely feel like layered feature treatments rather than one redesigned interaction language.
- Intended Visual Behavior / Appearance: dynamic editor states feel coordinated, premium, and easier to parse under real editing conditions.
- Icon / Visual Design Direction: sharp, refined emphasis with no noisy or over-saturated overlays.
- Technical Styling Approach: redesign all dynamic states as a coordinated family with clear intensity hierarchy and overlap rules.
- Implementation Steps:
  1. Audit current dynamic-state visuals and collisions.
  2. Define redesigned relative emphasis hierarchy for all major states.
  3. Update editor state rendering and blending behavior.
  4. Validate realistic combinations of selection, search, and diagnostics.
- Validation Steps:
  1. Trigger search over selected and diagnostic-marked lines.
  2. Compare state readability in both light and dark themes.
  3. Verify caret and active-line visibility under varied content density.
- Acceptance Criteria: dynamic editor states feel coordinated, premium, and distinctly redesigned from the current implementation.
- Dependencies: V27-P07-T01, Phase 02
- Parallelization Notes: can run in parallel with syntax redesign as long as state hierarchy is separated from token-color hierarchy.
- Risks / Failure Modes: overlapping state changes can become visually muddy if hierarchy is unclear.
- Accessibility / Readability Notes: selections and diagnostics must remain obvious for low-vision users.
- Theme / Styling Notes: dynamic-state redesign should stay theme-semantic, not hardcoded.
- Motion / Interaction Notes where relevant: later subtle transitions should reinforce these state roles without adding latency.
- Cleanup / Consolidation Notes where relevant: remove old local dynamic-state overrides where shared roles replace them.
- Rollback / Safety Notes: validate edge cases before removing existing fallback styling.
- References / Context: dynamic-state quality strongly affects perceived editor sophistication.
- Example scenarios where useful: searching inside a selected code block with warnings present should remain legible and calm.

### Phase ID: V27-P07
### Task ID: V27-P07-T03
### Task Title: Redesign Minimap Overview And Editor Adjacent Utility Surfaces
- Priority: P1
- Category: Editor UI
- Objective: update the minimap and other editor-adjacent support surfaces so they no longer feel secondary or MVP-grade.
- Why This Matters Now: sidecar editor surfaces can visibly weaken the editor even when the main text area looks good.
- Visual Gap Statement: minimap and overview support still show signs of simplified mapping and lower-fidelity treatment relative to the premium editor target.
- User Experience Impact: a more refined minimap and support area improves orientation and completes the flagship editor experience.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/MinimapPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/OverviewRulerPanel.cpp`
- Prior Plan References: `v22` editor/minimap work, `v26` Phase 06
- Scope: minimap shell, viewport indicator, code density rendering, overview markers, adjacent utility chrome
- Out of Scope: navigation mechanics
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/MinimapPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/OverviewRulerPanel.cpp`
- Related Screens / Components / Surfaces: minimap, overview ruler, editor right-edge utilities
- Current Visual Behavior / Appearance: these surfaces are useful but still feel less mature than the main editor chrome.
- Intended Visual Behavior / Appearance: minimap and overview surfaces feel lightweight but premium, aligned with the redesigned editor identity.
- Icon / Visual Design Direction: quiet precision with stronger fidelity and better integration into the editor shell.
- Technical Styling Approach: redesign auxiliary editor surfaces as part of the editor family rather than as low-priority sidecars.
- Implementation Steps:
  1. Reassess minimap and overview visual roles.
  2. Redesign surface shells, indicators, and marker styling.
  3. Improve token and density fidelity where practical.
  4. Validate performance and readability on large files.
- Validation Steps:
  1. Compare long code and markdown files.
  2. Check viewport and marker clarity in both themes.
  3. Ensure redesigned surfaces remain visually lightweight.
- Acceptance Criteria: editor-adjacent utility surfaces no longer feel like a lower-quality visual tier than the editor itself.
- Dependencies: V27-P07-T01, Phase 08
- Parallelization Notes: can proceed once core editor shell redesign direction is stable.
- Risks / Failure Modes: too much detail can make sidecar surfaces noisy or slow.
- Accessibility / Readability Notes: viewport and markers must remain visible without overpowering content.
- Theme / Styling Notes: auxiliary surfaces should inherit editor semantics rather than invent their own.
- Motion / Interaction Notes where relevant: viewport movement and marker response should later remain subtle.
- Cleanup / Consolidation Notes where relevant: retire simplified “MVP” presentation assumptions where the redesign replaces them.
- Rollback / Safety Notes: preserve performance and navigation reliability.
- References / Context: editor redesign is not complete if the minimap still looks transitional.
- Example scenarios where useful: scrolling through a large file should show a minimap that feels crisp and intentionally designed.
