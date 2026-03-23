# Phase 09 - Notebook Shell Cell Output And Mixed Content Redesign

## Goal

Redesign notebook visuals so they feel fully first-class, premium, and unmistakably part of the new MarkAmp visual world.

## Definition Of Done

- notebook shell and cell chrome feel newly designed
- output, execution status, and mixed-content readability are materially stronger
- notebook interaction affordances match the rest of the redesigned product

## Tasks

### Phase ID: V27-P09
### Task ID: V27-P09-T01
### Task Title: Redesign Notebook Host And Cell Chrome As A Flagship Product Surface
- Priority: P0
- Category: Notebook UI
- Objective: reauthor notebook shell, cell containers, cell toolbars, focus treatments, and interaction chrome.
- Why This Matters Now: notebooks are a core differentiator and still visually lag behind the best shell surfaces.
- Visual Gap Statement: notebook infrastructure exists, but notebook visual identity still feels less complete and less premium than the editor shell.
- User Experience Impact: a stronger notebook surface improves trust, delight, and product distinctiveness.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/core/NotebookChromeCoordinator.h`, `/Users/ryanrentfro/code/markamp/src/core/NotebookCellChrome.h`, `/Users/ryanrentfro/code/markamp/src/core/NotebookToolbar.h`
- Prior Plan References: `v22` Phase 09, `v25` notebook release-path work, `v26` Phase 08
- Scope: notebook host shell, cell boundaries, cell headers/toolbars, focus rings, drag handles, collapse controls, spacing and density
- Out of Scope: kernel or execution logic
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/NotebookChromeCoordinator.h`, `/Users/ryanrentfro/code/markamp/src/core/NotebookCellChrome.h`, `/Users/ryanrentfro/code/markamp/src/core/NotebookToolbar.h`
- Related Screens / Components / Surfaces: notebook editor, code cells, markdown cells, notebook toolbar
- Current Visual Behavior / Appearance: notebook visuals are functional and partially modernized, but still not visibly on par with an elite standalone notebook product.
- Intended Visual Behavior / Appearance: notebooks feel like a premium first-class MarkAmp surface with a distinct but related identity.
- Icon / Visual Design Direction: sophisticated computational canvas with calm framing, stronger depth, and richer cell hierarchy.
- Technical Styling Approach: redesign notebook-specific chrome roles on top of the new icon and visual foundations instead of patching existing cell shells.
- Implementation Steps:
  1. Redesign notebook shell and cell geometry.
  2. Rebuild cell toolbar and canonical icon usage.
  3. Redesign focus, selection, collapse, and drag affordances.
  4. Validate mixed code and markdown notebooks.
- Validation Steps:
  1. Open notebooks with several cell types.
  2. Compare notebook identity against editor and canvas surfaces.
  3. Review selection, focus, and drag clarity in both themes.
- Acceptance Criteria: notebook shell and cell chrome feel visibly redesigned and at flagship quality.
- Dependencies: Phase 01 through Phase 05
- Parallelization Notes: can run in parallel with canvas redesign after shell and control language stabilize.
- Risks / Failure Modes: over-framed cells can make notebooks heavy and reduce scannability.
- Accessibility / Readability Notes: active cell, focus, and collapse states must remain clear.
- Theme / Styling Notes: notebook identity should harmonize with the shell while keeping its own content emphasis.
- Motion / Interaction Notes where relevant: later drag and collapse motion should support the new cell hierarchy.
- Cleanup / Consolidation Notes where relevant: remove legacy notebook shell fragments that no longer fit the redesign.
- Rollback / Safety Notes: keep notebook behavior unchanged while reauthoring visuals.
- References / Context: notebooks are a premium benchmark surface in `v27`.
- Example scenarios where useful: switching from a code editor tab to a notebook should feel like entering a specialized but equally luxurious workspace.

### Phase ID: V27-P09
### Task ID: V27-P09-T02
### Task Title: Redesign Notebook Output And Execution Surfaces For Trust And Visual Richness
- Priority: P0
- Category: Notebook UI
- Objective: rebuild output blocks, execution badges, and result presentation so they feel more trustworthy, readable, and premium.
- Why This Matters Now: notebook output is where many otherwise-polished notebook products still look utilitarian.
- Visual Gap Statement: output presentation works, but visual shell quality and mixed-content hierarchy still lag behind the notebook redesign target.
- User Experience Impact: better output presentation makes notebooks feel more professional and more pleasant to read and share.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/CellOutputRenderer.cpp`
- Prior Plan References: `v22` notebook output phase, `v26` Phase 08
- Scope: output block shells, result hierarchy, execution metadata, error/warning output treatment, mixed content spacing
- Out of Scope: output generation logic
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/CellOutputRenderer.cpp`, `/Users/ryanrentfro/code/markamp/src/core/NotebookCellChrome.h`
- Related Screens / Components / Surfaces: code output, markdown output, execution badges, error output, inline results
- Current Visual Behavior / Appearance: outputs render, but they do not yet feel fully reauthored as premium notebook artifacts.
- Intended Visual Behavior / Appearance: outputs feel clean, rich, and deeply integrated with notebook identity.
- Icon / Visual Design Direction: refined notebook result surfaces with elegant execution cues and well-controlled severity styling.
- Technical Styling Approach: define output-specific shells and feedback roles tied to the notebook redesign rather than reusing generic utility styling.
- Implementation Steps:
  1. Redesign output container geometry and visual hierarchy.
  2. Rebuild execution badges, status indicators, and error treatments using canonical icons.
  3. Tune mixed-content spacing between markdown, code, and output.
  4. Validate long and short outputs with several severity states.
- Validation Steps:
  1. Review successful, running, and failed output states.
  2. Compare text, code, and mixed outputs for readability.
  3. Verify redesigned outputs remain calm and theme-correct.
- Acceptance Criteria: notebook output feels materially more premium and trustworthy than the current implementation.
- Dependencies: V27-P09-T01, Phase 15
- Parallelization Notes: can proceed as notebook chrome stabilizes.
- Risks / Failure Modes: heavy output shells can make notebooks visually dense and exhausting.
- Accessibility / Readability Notes: error and warning outputs must remain legible without harsh or overwhelming color blocks.
- Theme / Styling Notes: output-state roles must harmonize with the redesigned feedback system.
- Motion / Interaction Notes where relevant: execution transitions later should remain subtle and fast.
- Cleanup / Consolidation Notes where relevant: remove generic utility-style output shells where notebook-specific treatment is warranted.
- Rollback / Safety Notes: preserve output content fidelity and readability while redesigning chrome.
- References / Context: output styling is central to notebook credibility.
- Example scenarios where useful: a notebook with code, logs, warnings, and rendered markdown should feel visually composed.

### Phase ID: V27-P09
### Task ID: V27-P09-T03
### Task Title: Redesign Notebook Empty Loading Error And Multi Cell Interaction States
- Priority: P1
- Category: Notebook UI
- Objective: ensure notebook edge states and multi-cell interactions feel fully designed, not merely handled.
- Why This Matters Now: notebooks only feel complete when edge states and selection flows match the quality of the steady-state shell.
- Visual Gap Statement: loading, empty, error, and multi-cell interaction states often remain less mature than core cell chrome unless explicitly redesigned.
- User Experience Impact: better edge-state quality increases trust and makes notebooks feel complete and premium.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/core/NotebookChromeCoordinator.h`, `/Users/ryanrentfro/code/markamp/src/ui/CellOutputRenderer.cpp`
- Prior Plan References: `v22` notebook and empty-state phases, `v26` notebook edge-state work
- Scope: empty notebooks, loading placeholders, notebook-specific error surfaces, multi-cell selection, drag-selection, batch action affordances
- Out of Scope: kernel recovery behavior
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/NotebookChromeCoordinator.h`, `/Users/ryanrentfro/code/markamp/src/ui/CellOutputRenderer.cpp`
- Related Screens / Components / Surfaces: empty notebook, loading notebook, failed notebook state, multi-cell interactions
- Current Visual Behavior / Appearance: these states are likely functional but not yet fully reauthored as part of the notebook redesign.
- Intended Visual Behavior / Appearance: edge states feel designed, premium, and aligned with the notebook’s flagship identity.
- Icon / Visual Design Direction: elegant notebook scaffolding with restrained guidance and strong interaction clarity.
- Technical Styling Approach: build notebook edge-state templates and interaction-state roles from the redesigned notebook shell and feedback systems.
- Implementation Steps:
  1. Inventory notebook empty/loading/error/multi-cell states.
  2. Redesign those states with notebook-specific shells and canonical iconography.
  3. Rework multi-cell selection and batch-action affordances.
  4. Validate narrow, wide, sparse, and content-rich notebooks.
- Validation Steps:
  1. Open a new empty notebook.
  2. Simulate loading and failure states.
  3. Select multiple cells and review action and selection clarity.
- Acceptance Criteria: notebook edge states and multi-cell interactions no longer reveal a lower-quality visual tier.
- Dependencies: V27-P09-T01, Phase 16
- Parallelization Notes: can run after core notebook shell redesign direction is stable.
- Risks / Failure Modes: over-designed empties can feel consumerish; under-designed states will continue to feel incomplete.
- Accessibility / Readability Notes: edge-state messaging and selection affordances must remain obvious without relying solely on color.
- Theme / Styling Notes: edge states should align with redesigned feedback and onboarding systems.
- Motion / Interaction Notes where relevant: loading and selection transitions should later remain minimal and responsive.
- Cleanup / Consolidation Notes where relevant: consolidate notebook-specific state fragments into one system.
- Rollback / Safety Notes: preserve notebook semantics and action availability during redesign.
- References / Context: this closes the gap between notebook capability and notebook perceived quality.
- Example scenarios where useful: creating a fresh notebook should feel premium and intentional before the first cell is added.
