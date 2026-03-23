# Phase 07 - Syntax Highlighting Presentation And Code Readability

## Goal

Push syntax-highlighting presentation from “correct and themeable” to “premium, readable, and beautifully balanced” across editor, notebook, and embedded code contexts.

## Definition Of Done

- syntax hierarchy is clear and calm in light and dark themes
- markdown code fences, notebook code cells, and editor code blocks share a coherent presentation model
- theme-token mapping favors readability and premium finish rather than color novelty

## Tasks

### Phase ID: V26-P07
### Task ID: V26-P07-T01
### Task Title: Rebalance Syntax Token Hierarchy For Premium Readability
- Priority: P0
- Category: Syntax Highlighting Presentation
- Objective: Improve the relative visual hierarchy of comments, keywords, strings, types, functions, classes, numbers, and punctuation across supported themes.
- Why This Matters Now: Syntax presentation strongly influences whether the editor feels elite or merely functional.
- UI Gap Statement: Theme and token systems exist, but syntax balance can still feel practical rather than premium and language-aware.
- User Experience Impact: Better hierarchy reduces fatigue and makes code comprehension faster.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/core/ThemeTokens.h`, `/Users/ryanrentfro/code/markamp/src/core/ThemeEngine.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp`
- Prior Plan References: `v22` syntax-highlighting work, `v25` syntax/theme/icon visible closure
- Scope: syntax token roles, relative intensity, contrast tuning, code fence token mapping
- Out of Scope: parser or semantic token engine changes
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/ThemeTokens.h`, `/Users/ryanrentfro/code/markamp/src/core/ThemeEngine.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp`
- Related Screens / Components / Surfaces: code editor, markdown fenced blocks, embedded code surfaces
- Current UI Behavior / Appearance: syntax is theme-driven but may not yet optimize for a restrained premium hierarchy across languages.
- Intended UI Behavior / Appearance: syntax colors clearly communicate semantic roles while preserving calm, elegant readability.
- Visual / Interaction Design Direction: premium code palette with disciplined accents and excellent semantic hierarchy.
- Technical Styling Approach: tune token mappings and token intensities rather than adding ad hoc language-specific exceptions first.
- Implementation Steps:
  1. Audit current syntax token roles per theme.
  2. Define target hierarchy for major semantic groups.
  3. Tune theme token mappings and shared syntax presentation rules.
  4. Validate across representative code samples.
- Validation Steps:
  1. Compare C++, Markdown, JSON, YAML, and scripting samples.
  2. Review light and dark themes side by side.
  3. Verify comments remain readable but properly recessive.
- Acceptance Criteria: syntax hierarchy is materially more readable and premium across major file types and themes.
- Dependencies: V26-P06-T01
- Parallelization Notes: should run early in parallel with later editor-state polish.
- Risks / Failure Modes: over-flattening can reduce semantic clarity; over-coloring can feel noisy.
- Accessibility / Readability Notes: contrast must remain safe for long reading sessions.
- Theme / Styling Notes: prioritize shared semantic rules over theme-specific hacks.
- Motion / Interaction Notes where relevant: none
- Cleanup / Consolidation Notes where relevant: remove legacy or duplicated syntax role mappings where found.
- Rollback / Safety Notes: compare before/after screenshots and sample files before finalizing token changes.
- References / Context: syntax tuning should serve both code editing and markdown-authoring scenarios.
- Example scenarios where useful: a dense C++ header should feel readable without high-saturation distraction.

### Phase ID: V26-P07
### Task ID: V26-P07-T02
### Task Title: Unify Markdown Code Fences, Embedded Languages, And Notebook Code Cell Presentation
- Priority: P1
- Category: Syntax Highlighting Presentation
- Objective: Ensure code-looking content uses one coherent presentation language across editor, notebook, and mixed-content surfaces.
- Why This Matters Now: Embedded code is where theme and syntax drift becomes most visible.
- UI Gap Statement: Code presentation can diverge between the editor, markdown preview-style contexts, and notebook cells if those surfaces evolve independently.
- User Experience Impact: Mixed-content authoring feels substantially more premium when code blocks look related everywhere.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/CellOutputRenderer.cpp`, `/Users/ryanrentfro/code/markamp/src/core/NotebookChromeCoordinator.h`
- Prior Plan References: `v22` syntax and notebook work, `v25` visible UI closure for syntax/theme
- Scope: markdown code fences, notebook cell code styling, embedded language backgrounds, border/radius treatment
- Out of Scope: notebook execution behavior
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/CellOutputRenderer.cpp`, `/Users/ryanrentfro/code/markamp/src/core/NotebookChromeCoordinator.h`
- Related Screens / Components / Surfaces: markdown editor, notebook code cells, output blocks with code, embedded code surfaces
- Current UI Behavior / Appearance: code surfaces likely differ in background depth, spacing, and syntax treatment depending on context.
- Intended UI Behavior / Appearance: code content looks like one family across all contexts, with context-specific shell only where needed.
- Visual / Interaction Design Direction: coherent code-surface language with subtle contextual variation.
- Technical Styling Approach: define shared code-surface tokens and apply context wrappers rather than restyling syntax from scratch per surface.
- Implementation Steps:
  1. Audit code presentation in editor, notebook, and mixed-content contexts.
  2. Define shared code-surface background, border, and padding conventions.
  3. Align syntax token use and surrounding chrome.
  4. Validate markdown and notebook side by side.
- Validation Steps:
  1. Compare fenced markdown blocks and notebook code cells with the same sample code.
  2. Review embedded language blocks in both themes.
  3. Check contrast and spacing on large and small displays.
- Acceptance Criteria: code surfaces feel related and premium across all major contexts.
- Dependencies: V26-P07-T01, V26-P08-T01
- Parallelization Notes: can overlap with notebook shell work if the shared code-surface rules are agreed first.
- Risks / Failure Modes: over-unifying can erase important contextual differences between editor and notebook.
- Accessibility / Readability Notes: code blocks in mixed-content documents must remain highly legible.
- Theme / Styling Notes: background contrast and border treatments must be theme-safe.
- Motion / Interaction Notes where relevant: none
- Cleanup / Consolidation Notes where relevant: retire context-specific one-off code block styling.
- Rollback / Safety Notes: validate representative notebooks and markdown docs before removing older styling branches.
- References / Context: this is a cross-surface visual coherence task.
- Example scenarios where useful: a code block in markdown and a code cell in a notebook should feel like they belong to the same product.

### Phase ID: V26-P07
### Task ID: V26-P07-T03
### Task Title: Tune Cross-Theme Syntax Contrast And Long-Session Readability
- Priority: P1
- Category: Syntax Highlighting Presentation
- Objective: Validate syntax and editor colors for extended-session comfort in both light and dark themes.
- Why This Matters Now: Premium color balance is about long-term comfort, not only first impression.
- UI Gap Statement: Token coverage exists, but cross-theme readability tuning is often left incomplete until late.
- User Experience Impact: Better long-session comfort improves professional credibility and day-to-day trust.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/core/ThemeEngine.cpp`, `/Users/ryanrentfro/code/markamp/src/core/ThemeTokens.h`, `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp`
- Prior Plan References: `v22` theme and syntax work, `v25` theme closure
- Scope: cross-theme contrast tuning, low-noise punctuation, comment legibility, selection-overlay compatibility
- Out of Scope: adding new themes
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/ThemeEngine.cpp`, `/Users/ryanrentfro/code/markamp/src/core/ThemeTokens.h`, `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp`
- Related Screens / Components / Surfaces: light theme editor, dark theme editor, notebook code cells
- Current UI Behavior / Appearance: theme correctness is broad, but premium parity and extended-session comfort are not yet guaranteed.
- Intended UI Behavior / Appearance: both light and dark themes support long reading sessions without washed-out or over-saturated syntax.
- Visual / Interaction Design Direction: balanced, low-fatigue palettes with controlled accent distribution.
- Technical Styling Approach: use representative sample files and screenshot review to tune relative contrast rather than isolated token edits.
- Implementation Steps:
  1. Prepare representative code and markdown samples.
  2. Review syntax contrast and fatigue points across themes.
  3. Adjust token values and derived editor-state blending where needed.
  4. Re-test with search, diagnostics, and selections active.
- Validation Steps:
  1. Read sample files for extended periods in both themes.
  2. Compare screenshots with the shell and notebook surfaces visible.
  3. Confirm syntax remains distinct under active selection and diagnostics overlays.
- Acceptance Criteria: light and dark syntax palettes feel equally deliberate, readable, and premium over long sessions.
- Dependencies: V26-P07-T01, V26-P15-T01
- Parallelization Notes: best finalized alongside broader theme parity work.
- Risks / Failure Modes: theme tuning in isolation can break component-level contrast elsewhere.
- Accessibility / Readability Notes: preserve minimum contrast and avoid relying on saturation alone for semantics.
- Theme / Styling Notes: this is a parity task, not a new-theme task.
- Motion / Interaction Notes where relevant: none
- Cleanup / Consolidation Notes where relevant: document theme decisions to avoid future drift.
- Rollback / Safety Notes: keep token revisions incremental and reversible.
- References / Context: long-session comfort is a key elite-quality differentiator.
- Example scenarios where useful: reading a C++ implementation file for an hour in dark mode should remain comfortable and clear.
