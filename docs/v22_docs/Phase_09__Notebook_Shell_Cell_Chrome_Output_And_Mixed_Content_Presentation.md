# Phase 09: Notebook Shell Cell Chrome Output And Mixed Content Presentation

## Outcome

Create a premium notebook visual system with strong shell chrome, cell hierarchy, execution states, output rendering, and mixed prose/code rhythm.

## Improvement Count

360 atomic improvements across 6 execution tasks.

### P09-T01

- Phase ID: P09
- Task ID: P09-T01
- Task Title: Design and implement a real notebook shell UI family instead of model-only chrome intent
- Priority: P0
- Category: Notebook UI
- Atomic Improvements Covered: 60
- Objective: Promote notebook UI from model concepts and notifications into a visibly complete workbench surface.
- Why This Matters Now: The current notebook implementation has visual intent but not yet a premium notebook shell.
- UI Problem Statement: [NotebookShellHost.cpp](/Users/ryanrentfro/code/markamp/src/core/NotebookShellHost.cpp) is a thin host, [NotebookToolbar.cpp](/Users/ryanrentfro/code/markamp/src/core/NotebookToolbar.cpp) is a command registry, and notebook chrome concepts are mostly model-side.
- User Experience Impact: The notebook product promise feels under-realized visually.
- Scope: Notebook host shell, notebook top bar, notebook canvas background, notebook metadata strip, notebook-level empty/loading/error states.
- Out of Scope: Notebook execution or persistence logic.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/NotebookShellHost.cpp`; `/Users/ryanrentfro/code/markamp/src/core/NotebookShellHost.h`; `/Users/ryanrentfro/code/markamp/src/core/NotebookToolbar.cpp`; `/Users/ryanrentfro/code/markamp/src/core/NotebookCellChrome.h`
- Related Screens / Components / Surfaces: Notebook host, notebook root surface, notebook lists and tabs.
- Current UI Behavior / Appearance: Notebook host state exists, but the visible surface is not yet clearly product-grade.
- Intended UI Behavior / Appearance: Notebooks have a distinctive but cohesive shell that feels as finished as the editor.
- Visual / Interaction Design Direction: Editorial-computational workspace with strong structure and premium calm.
- Technical Styling Approach: Define notebook shell tokens and component families that inherit from shell and editor systems while adding notebook-specific identity.
- Implementation Steps: Define notebook shell composition; create host chrome variants; align notebook toolbar and root spacing; add notebook-level states; integrate notebook surface cues into tabs and navigation.
- Validation Steps: Review empty notebook, loaded notebook, and execution-heavy notebook shell states across themes.
- Acceptance Criteria: The notebook root surface no longer feels like a thin wrapper around model code.
- Dependencies: Phase 01, Phase 05, Phase 18.
- Risks / Failure Modes: Over-differentiating notebooks can make them feel separate from the rest of the IDE.
- Accessibility / Readability Notes: Notebook shell hierarchy must remain clear when many cells and outputs are visible.
- Theme / Styling Notes: Notebook shell needs distinct enough layering to separate cells without introducing visual noise.
- Motion / Interaction Notes where relevant: Notebook surface transitions should be gentle and support content continuity.
- Observability / Diagnostics Notes where relevant: Add notebook shell reference screens to the visual QA stack.
- Rollback / Safety Notes: Build notebook shell UI in layers so content and workflow logic are not blocked by visual refactors.
- References / Context: Notebook UI is one of the clearest places where the repository’s visual ambition still exceeds its visible finish.
- Example scenarios where useful: Opening a notebook feels like entering a first-class product mode, not just a different document type.

### P09-T02

- Phase ID: P09
- Task ID: P09-T02
- Task Title: Rebuild cell containers, prompt gutters, selection states, and add-cell affordances
- Priority: P0
- Category: Notebook UI
- Atomic Improvements Covered: 60
- Objective: Make notebook cells feel structurally clear, easy to manipulate, and visually polished.
- Why This Matters Now: Cell chrome defines whether notebooks feel premium or provisional.
- UI Problem Statement: [NotebookCellChrome.h](/Users/ryanrentfro/code/markamp/src/core/NotebookCellChrome.h) describes strong state concepts, but visible cell presentation is not yet fully realized and unified.
- User Experience Impact: Users cannot rely on a clean visual rhythm when moving among code, markdown, and raw cells.
- Scope: Cell shell backgrounds, prompt gutters, active cell emphasis, selected cell state, collapsed state, drag handles, insertion bars, add-cell buttons.
- Out of Scope: Cell execution semantics.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/NotebookCellChrome.h`; `/Users/ryanrentfro/code/markamp/src/core/NotebookCellChrome.cpp`; `/Users/ryanrentfro/code/markamp/src/core/NotebookCellController.cpp`; `/Users/ryanrentfro/code/markamp/src/core/NotebookCellManager.cpp`
- Related Screens / Components / Surfaces: Code cells, markdown cells, raw cells, notebook gutters.
- Current UI Behavior / Appearance: Cell-state concepts exist, but the visible cell system lacks a fully articulated premium presentation.
- Intended UI Behavior / Appearance: Cells have crisp boundaries, clear states, and effortless manipulation cues.
- Visual / Interaction Design Direction: Structured stacked documents with elegant cell separation and low-noise affordances.
- Technical Styling Approach: Create notebook cell container variants by cell type and state, reusing editor and panel state language where appropriate.
- Implementation Steps: Define cell shell variants; tune gutters and prompts; create insertion/add-cell affordances; align active and selected states; normalize collapsed and drag states.
- Validation Steps: Review notebooks with mixed cell types, multiple selections, and insertion interactions.
- Acceptance Criteria: Cell structure and manipulation affordances feel premium and self-explanatory.
- Dependencies: Phase 01, Phase 17, Phase 19.
- Risks / Failure Modes: Too much container chrome can make notebooks feel boxed and heavy.
- Accessibility / Readability Notes: Active and selected cells must be identifiable without subtle color-only cues.
- Theme / Styling Notes: Cell shells and prompts need enough contrast separation without fragmenting the notebook canvas.
- Motion / Interaction Notes where relevant: Insertion and drag cues should be smooth and highly legible.
- Observability / Diagnostics Notes where relevant: Add mixed-cell state screenshots and interaction captures.
- Rollback / Safety Notes: Keep cell content layout stable while chrome is redesigned.
- References / Context: Notebook cell chrome is one of the biggest UI opportunities in the product.
- Example scenarios where useful: Moving focus across ten cells makes the active cell instantly obvious without noisy borders everywhere.

### P09-T03

- Phase ID: P09
- Task ID: P09-T03
- Task Title: Elevate execution chrome, run controls, and status indicators for notebook workflows
- Priority: P1
- Category: Notebook UI
- Atomic Improvements Covered: 60
- Objective: Make execution status feel professional, trustworthy, and visually integrated.
- Why This Matters Now: Execution chrome is a defining notebook affordance and must feel better than utility-grade widgets.
- UI Problem Statement: Notebook command and execution state models exist, but the visible run controls and execution cues need a true design system.
- User Experience Impact: Running cells feels less premium and less confidence-inspiring than it should.
- Scope: Run-cell controls, run-all controls, execution count badges, queued/running/success/error states, kernel status indicators, cell-level status chips.
- Out of Scope: Kernel plumbing.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/NotebookToolbar.cpp`; `/Users/ryanrentfro/code/markamp/src/core/NotebookExecutionPipeline.cpp`; `/Users/ryanrentfro/code/markamp/src/core/NotebookCellChrome.cpp`; `/Users/ryanrentfro/code/markamp/src/core/NotebookCommandRouter.cpp`
- Related Screens / Components / Surfaces: Notebook toolbar, cell run buttons, status cues, execution metadata.
- Current UI Behavior / Appearance: Execution state is tracked, but the visible chrome lacks one premium treatment language.
- Intended UI Behavior / Appearance: Execution cues are elegant, immediate, and easy to trust.
- Visual / Interaction Design Direction: Technical precision with restrained status color and clear hierarchy.
- Technical Styling Approach: Define notebook-execution component variants and align them with status, button, and badge systems.
- Implementation Steps: Inventory run and status cues; define state visuals; align notebook toolbar and cell-level controls; refine status placement and density.
- Validation Steps: Review idle, queued, running, success, error, and cancelled notebook states in mixed-content notebooks.
- Acceptance Criteria: Execution status and controls feel cohesive and premium at both notebook and cell levels.
- Dependencies: Phase 01, Phase 04, Phase 14.
- Risks / Failure Modes: Too many execution cues can clutter the notebook and reduce content focus.
- Accessibility / Readability Notes: Status severity and execution progress must not depend on color alone.
- Theme / Styling Notes: Running and error states require careful theme calibration to avoid neon noise.
- Motion / Interaction Notes where relevant: Running indicators and completion states should use restrained motion and clear reduced-motion alternatives.
- Observability / Diagnostics Notes where relevant: Add execution-state galleries for the notebook review board.
- Rollback / Safety Notes: Keep current execution semantics while evolving visible status treatments.
- References / Context: Notebook execution chrome is a decisive quality marker for computational documents.
- Example scenarios where useful: Running three cells in succession shows calm, legible progress without visual chaos.

### P09-T04

- Phase ID: P09
- Task ID: P09-T04
- Task Title: Redesign notebook output blocks, result renderers, and markdown/code mixed-content rhythm
- Priority: P0
- Category: Notebook UI
- Atomic Improvements Covered: 60
- Objective: Make output presentation feel as high quality as the code and markdown inputs that surround it.
- Why This Matters Now: Output blocks often make or break the perceived quality of a notebook product.
- UI Problem Statement: [CellOutputRenderer.cpp](/Users/ryanrentfro/code/markamp/src/ui/CellOutputRenderer.cpp) defines output types and state, but not yet a premium output surface language.
- User Experience Impact: Outputs can feel bolted on or visually uneven relative to the notebook shell.
- Scope: Stream outputs, errors, HTML outputs, images, JSON outputs, collapse states, output headers, error framing, scroll treatment, spacing after outputs.
- Out of Scope: Output execution correctness.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/CellOutputRenderer.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/CellOutputRenderer.h`; `/Users/ryanrentfro/code/markamp/src/core/NotebookOutputIntegration.cpp`; `/Users/ryanrentfro/code/markamp/src/core/NotebookCellChrome.h`
- Related Screens / Components / Surfaces: Notebook outputs, render blocks, error traces, result sections.
- Current UI Behavior / Appearance: Output data modeling exists, but output presentation still needs strong visual design.
- Intended UI Behavior / Appearance: Outputs feel integrated, readable, and well-separated by type and severity.
- Visual / Interaction Design Direction: Calm notebook result blocks with distinct but not noisy output identities.
- Technical Styling Approach: Define output block variants and align them with panel, code, and alert surface systems.
- Implementation Steps: Design output containers; style each output type; improve collapse and header treatments; tune error trace readability; normalize spacing and scroll behavior.
- Validation Steps: Review notebooks containing streams, rich HTML, images, JSON, and errors in one session.
- Acceptance Criteria: Outputs look like a deliberate notebook subsystem instead of generic dumped content.
- Dependencies: Phase 07, Phase 14, Phase 15.
- Risks / Failure Modes: Rich output styling can become too card-heavy and fragment notebook flow.
- Accessibility / Readability Notes: Error outputs and tracebacks require especially strong readability and contrast.
- Theme / Styling Notes: Output block backgrounds must sit within the notebook canvas without flattening.
- Motion / Interaction Notes where relevant: Output expansion and collapse should feel crisp and predictable.
- Observability / Diagnostics Notes where relevant: Add multi-output notebook screenshots to the notebook UI board.
- Rollback / Safety Notes: Keep output content faithful while refining visual framing.
- References / Context: Notebook outputs are central to premium notebook credibility.
- Example scenarios where useful: A code cell with a JSON output followed by an error output remains visually clean and easy to parse.

### P09-T05

- Phase ID: P09
- Task ID: P09-T05
- Task Title: Polish notebook-specific navigation, toolbar, and document-level metadata presentation
- Priority: P2
- Category: Notebook UI
- Atomic Improvements Covered: 60
- Objective: Ensure notebooks have polished document-level controls and orientation cues.
- Why This Matters Now: Notebook quality depends on more than cells; the host-level controls and metadata must also feel premium.
- UI Problem Statement: Notebook toolbar and shell state are underdeveloped visually, and notebook-specific navigation cues need stronger presence.
- User Experience Impact: Users may not feel grounded in notebook mode or may perceive it as a secondary surface.
- Scope: Notebook toolbar grouping, document title treatment, kernel identity, trust level surfaces, checkpoint/session metadata, notebook-level empty states.
- Out of Scope: Session manager logic or trust semantics.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/NotebookToolbar.cpp`; `/Users/ryanrentfro/code/markamp/src/core/NotebookSessionManager.cpp`; `/Users/ryanrentfro/code/markamp/src/core/NotebookOutputIntegration.cpp`; `/Users/ryanrentfro/code/markamp/src/core/NotebookShellHost.cpp`
- Related Screens / Components / Surfaces: Notebook toolbar, notebook title region, trust/checkpoint metadata, notebook navigation.
- Current UI Behavior / Appearance: Notebook-level UI cues exist conceptually, but not yet as a refined visible surface system.
- Intended UI Behavior / Appearance: Document-level notebook controls and metadata feel as polished as cell-level interactions.
- Visual / Interaction Design Direction: Clear, calm computational document identity with subtle technical metadata.
- Technical Styling Approach: Reuse shell-toolbar and metadata systems while defining notebook-specific presentation rules.
- Implementation Steps: Define notebook document-level UI roles; align title, toolbar, and metadata strips; style trust and checkpoint cues; improve notebook-level empty and loading states.
- Validation Steps: Review notebook shells with trusted/untrusted and checkpoint-rich scenarios.
- Acceptance Criteria: Notebook mode feels complete at the document level, not just within individual cells.
- Dependencies: Phase 01, Phase 04, Phase 18.
- Risks / Failure Modes: Too much document metadata can distract from content.
- Accessibility / Readability Notes: Trust and checkpoint states must be understandable without subtle color coding.
- Theme / Styling Notes: Metadata strips should remain understated and harmonized with notebook shell layers.
- Motion / Interaction Notes where relevant: Toolbar and metadata updates should not reflow the notebook jarringly.
- Observability / Diagnostics Notes where relevant: Add notebook-host screenshots with metadata variants.
- Rollback / Safety Notes: Keep notebook content area stable while host chrome is enhanced.
- References / Context: Premium notebooks require both cell-level and document-level UI excellence.
- Example scenarios where useful: Opening a notebook with unsaved changes and untrusted outputs still looks clear and elegant.

### P09-T06

- Phase ID: P09
- Task ID: P09-T06
- Task Title: Add notebook UI galleries and mixed-content regression suites
- Priority: P0
- Category: Cleanup / Consolidation
- Atomic Improvements Covered: 60
- Objective: Protect notebook visual quality with reference boards covering shell, cells, outputs, and execution states.
- Why This Matters Now: Notebook UI has many layers and currently lacks strong visible governance.
- UI Problem Statement: Notebook shell, cell, and output design work can easily drift without representative mixed-content fixtures.
- User Experience Impact: Notebook polish can regress even while editor and panel UI continue improving.
- Scope: Notebook shell states, mixed cell types, output variants, execution states, dense notebooks, collapsed cells, trusted/untrusted visuals.
- Out of Scope: Execution correctness testing.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/tests/unit`; `/Users/ryanrentfro/code/markamp/docs/v22_docs`; `/Users/ryanrentfro/code/markamp/src/core/NotebookCellChrome.h`; `/Users/ryanrentfro/code/markamp/src/ui/CellOutputRenderer.cpp`
- Related Screens / Components / Surfaces: Entire notebook UI family.
- Current UI Behavior / Appearance: Notebook visual quality is not yet protected by dedicated reference states.
- Intended UI Behavior / Appearance: Notebook UI decisions are reviewable, comparable, and regression-gated.
- Visual / Interaction Design Direction: Operationally mature notebook design governance.
- Technical Styling Approach: Build notebook-specific galleries and screenshot suites using mixed-content fixture notebooks.
- Implementation Steps: Create notebook fixtures; capture host, cell, and output states; publish galleries; add regression checks for critical notebook UI variants.
- Validation Steps: Intentionally perturb cell spacing or output framing and verify the regression suite catches the change.
- Acceptance Criteria: Notebook UI becomes a stable, reviewable surface family instead of a moving target.
- Dependencies: P09-T01 through P09-T05.
- Risks / Failure Modes: Fixture notebooks can become too narrow if they do not represent mixed real-world content.
- Accessibility / Readability Notes: Include low-vision and reduced-motion notebook scenarios in the gallery.
- Theme / Styling Notes: Cover notebook states in dark, light, and high-contrast themes.
- Motion / Interaction Notes where relevant: Include execution-state and collapse/expand captures where motion matters visually.
- Observability / Diagnostics Notes where relevant: Store fixture metadata with cell counts, output types, and execution states.
- Rollback / Safety Notes: Keep galleries additive while notebook UI is still growing.
- References / Context: Notebook UI maturity is now a direct brand question for MarkAmp.
- Example scenarios where useful: A notebook with prose, code, images, and errors has a documented target appearance across all key themes.

