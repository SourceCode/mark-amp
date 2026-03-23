# Phase 08 - Notebook Shell Cell Chrome Output And Mixed Content

## Goal

Raise notebook UI to shell-quality parity with refined cell chrome, output styling, execution status presentation, and mixed-content readability.

## Definition Of Done

- notebook shell, cell chrome, and output styling feel premium and consistent
- code, markdown, and output cells read as one notebook system
- empty, loading, and error notebook states are polished and informative

## Tasks

### Phase ID: V26-P08
### Task ID: V26-P08-T01
### Task Title: Establish A Premium Notebook Shell And Cell Chrome System
- Priority: P0
- Category: Notebook UI
- Objective: Create a cohesive notebook visual framework for containers, cell boundaries, focus states, drag affordances, and toolbars.
- Why This Matters Now: Notebook is a flagship surface and still appears visually less mature than the main shell.
- UI Gap Statement: Notebook infrastructure exists, but notebook chrome is not yet visibly at parity with the rest of the app’s custom shell.
- User Experience Impact: A stronger notebook shell improves trust and makes notebook workflows feel first-class rather than adjunct.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/core/NotebookChromeCoordinator.h`, `/Users/ryanrentfro/code/markamp/src/core/NotebookCellChrome.h`, `/Users/ryanrentfro/code/markamp/src/core/NotebookToolbar.h`
- Prior Plan References: `v22` notebook UI pass, `v25` release-readiness hardening for notebook trustworthiness
- Scope: notebook host shell, cell containers, focus rings, drag handles, run/action chrome, section spacing
- Out of Scope: execution pipeline or kernel logic
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/NotebookChromeCoordinator.h`, `/Users/ryanrentfro/code/markamp/src/core/NotebookCellChrome.h`, `/Users/ryanrentfro/code/markamp/src/core/NotebookToolbar.h`
- Related Screens / Components / Surfaces: notebook editor, code cells, markdown cells, toolbar chrome
- Current UI Behavior / Appearance: notebook visuals are likely serviceable but not yet fully aligned with the product’s premium shell language.
- Intended UI Behavior / Appearance: notebook cells and shell feel precise, spacious where needed, and deeply coherent with shell/editor styling.
- Visual / Interaction Design Direction: premium computational notebook with calm framing and strong focus hierarchy.
- Technical Styling Approach: define notebook-specific shell roles built on shared tokens rather than ad hoc styling within notebook components.
- Implementation Steps:
  1. Audit notebook container, cell, and toolbar visuals.
  2. Define shared notebook chrome roles and states.
  3. Refactor notebook cell and toolbar visuals to use those roles.
  4. Validate code, markdown, selected, and dragged cells.
- Validation Steps:
  1. Open notebooks with mixed cell types.
  2. Compare notebook visuals to main editor and shell chrome.
  3. Verify selection and focus clarity in both themes.
- Acceptance Criteria: notebook shell and cell chrome feel like a polished first-class MarkAmp surface.
- Dependencies: V26-P01-T01, V26-P01-T03, V26-P06-T01
- Parallelization Notes: can run in parallel with canvas polish after shell rules are stable.
- Risks / Failure Modes: heavy notebook chrome can reduce content density too much.
- Accessibility / Readability Notes: focus and active cell states must be clearly visible.
- Theme / Styling Notes: notebook should inherit shell and editor language without becoming visually identical.
- Motion / Interaction Notes where relevant: drag and focus transitions can be layered on later.
- Cleanup / Consolidation Notes where relevant: consolidate notebook-specific styling roles rather than scattering them across components.
- Rollback / Safety Notes: keep notebook interactions unchanged while chrome is refined.
- References / Context: notebook is a flagship premium-surface task in `v26`.
- Example scenarios where useful: moving from an editor tab to a notebook tab should feel like entering a specialized but equally polished surface.

### Phase ID: V26-P08
### Task ID: V26-P08-T02
### Task Title: Upgrade Notebook Output Blocks, Execution Status, And Mixed Content Readability
- Priority: P0
- Category: Notebook UI
- Objective: Make outputs, result blocks, and execution metadata visually rich enough to feel trustworthy and refined.
- Why This Matters Now: Output presentation is where notebook quality often collapses into utilitarian styling.
- UI Gap Statement: `/Users/ryanrentfro/code/markamp/src/ui/CellOutputRenderer.cpp` is functional, but premium output-shell styling remains underdeveloped.
- User Experience Impact: Better output blocks improve confidence, reduce clutter, and make notebooks more enjoyable to read and share.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/CellOutputRenderer.cpp`, `/Users/ryanrentfro/code/markamp/src/core/NotebookCellChrome.h`
- Prior Plan References: `v22` notebook output presentation, `v25` notebook hardening
- Scope: output block backgrounds, borders, spacing, execution metadata, error output styling, markdown-output rhythm
- Out of Scope: output generation logic
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/CellOutputRenderer.cpp`, `/Users/ryanrentfro/code/markamp/src/core/NotebookCellChrome.h`
- Related Screens / Components / Surfaces: notebook output blocks, execution states, inline results, markdown output
- Current UI Behavior / Appearance: outputs render, but visual hierarchy and mixed-content polish are likely behind premium notebook expectations.
- Intended UI Behavior / Appearance: output blocks feel intentional, readable, and clean, with clear but restrained execution metadata.
- Visual / Interaction Design Direction: rich but disciplined notebook output surface with strong content hierarchy.
- Technical Styling Approach: create shared output-shell styles tied to notebook chrome and feedback-state tokens.
- Implementation Steps:
  1. Audit output variants and metadata patterns.
  2. Define output container, caption, and error-state roles.
  3. Refine spacing and background treatment for mixed content.
  4. Validate code output, text output, and error output.
- Validation Steps:
  1. Open notebooks with successful, pending, and failed outputs.
  2. Compare long and short outputs for readability.
  3. Verify visual consistency with dialogs and feedback surfaces.
- Acceptance Criteria: notebook outputs feel premium and clearly related to notebook chrome and app feedback language.
- Dependencies: V26-P08-T01, V26-P13-T01
- Parallelization Notes: can proceed while notebook shell work is being finalized.
- Risks / Failure Modes: over-framing outputs can make notebooks feel heavy.
- Accessibility / Readability Notes: error and warning outputs need strong readability without harsh color blocks.
- Theme / Styling Notes: output contrast must remain safe in light and dark themes.
- Motion / Interaction Notes where relevant: execution-status transitions should be subtle if added.
- Cleanup / Consolidation Notes where relevant: avoid output-type-specific one-off shells where shared styling works.
- Rollback / Safety Notes: keep output semantics unchanged while refining appearance.
- References / Context: output quality is central to notebook trustworthiness.
- Example scenarios where useful: a notebook with markdown, code, charts, and error output should remain visually composed.

### Phase ID: V26-P08
### Task ID: V26-P08-T03
### Task Title: Polish Notebook Empty, Loading, Error, And Multi-Cell Selection States
- Priority: P1
- Category: Notebook UI
- Objective: Make notebook edge states feel designed, informative, and calm.
- Why This Matters Now: Premium quality is most visible when a surface is not in its happy path.
- UI Gap Statement: Notebook edge states often lag behind shell and editor polish unless explicitly targeted.
- User Experience Impact: Clear, elegant notebook states improve trust and reduce confusion during setup and recovery moments.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/core/NotebookChromeCoordinator.h`, `/Users/ryanrentfro/code/markamp/src/ui/CellOutputRenderer.cpp`
- Prior Plan References: `v22` notebook polish, `v25` notebook resilience and UI closure
- Scope: empty notebooks, loading placeholders, error surfaces, cell selection and drag-selection visuals
- Out of Scope: kernel startup behavior or recovery mechanics
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/NotebookChromeCoordinator.h`, `/Users/ryanrentfro/code/markamp/src/ui/CellOutputRenderer.cpp`
- Related Screens / Components / Surfaces: notebook startup view, empty notebooks, multi-cell interactions
- Current UI Behavior / Appearance: likely functional but not yet fully polished or systematized.
- Intended UI Behavior / Appearance: every notebook edge state looks intentional and aligned with the rest of the premium shell.
- Visual / Interaction Design Direction: elegant notebook scaffolding that never looks abandoned or temporary.
- Technical Styling Approach: reuse startup, feedback, and notebook-shell patterns rather than inventing special-case visuals.
- Implementation Steps:
  1. Inventory notebook empty/loading/error states.
  2. Define shared templates for edge-state layout and messaging hierarchy.
  3. Refine multi-cell selection and drag visuals to match notebook chrome.
  4. Validate with theme parity and reduced-density cases.
- Validation Steps:
  1. Open a new empty notebook.
  2. Simulate loading and execution failure states.
  3. Select multiple cells and observe focus/selection clarity.
- Acceptance Criteria: notebook edge states are polished, informative, and consistent with the app’s visual language.
- Dependencies: V26-P08-T01, V26-P14-T02
- Parallelization Notes: can run after the notebook chrome language is stable.
- Risks / Failure Modes: over-designed empty states can become distracting in a productivity product.
- Accessibility / Readability Notes: ensure status messaging stays legible and not icon-dependent.
- Theme / Styling Notes: keep notebook edge states aligned with feedback and onboarding surfaces.
- Motion / Interaction Notes where relevant: loading motion should remain subtle and optional.
- Cleanup / Consolidation Notes where relevant: consolidate notebook edge-state components where possible.
- Rollback / Safety Notes: do not imply unsupported notebook capabilities through overly specific empty-state cues.
- References / Context: notebook edge-state quality strongly affects perceived completeness.
- Example scenarios where useful: creating a new notebook should feel inviting and premium instead of blank or skeletal.
