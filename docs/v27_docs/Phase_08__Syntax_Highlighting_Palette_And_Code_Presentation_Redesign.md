# Phase 08 - Syntax Highlighting Palette And Code Presentation Redesign

## Goal

Redesign syntax presentation so code, markup, and embedded-language content feel more premium, more readable, and more distinctive than the current state.

## Definition Of Done

- syntax hierarchy is visibly redesigned, not only contrast-tuned
- code presentation across editor, notebook, and embedded contexts feels more premium
- dark and light themes both support a stronger reading experience

## Tasks

### Phase ID: V27-P08
### Task ID: V27-P08-T01
### Task Title: Redesign The Global Syntax Palette And Semantic Color Hierarchy
- Priority: P0
- Category: Syntax Highlighting Presentation
- Objective: rebuild the syntax palette and semantic hierarchy for premium readability and stronger visual authorship.
- Why This Matters Now: syntax color is one of the most visible areas where professional users judge editor quality.
- Visual Gap Statement: syntax mapping works, but it still reads more like practical theme engineering than a decisive visual redesign.
- User Experience Impact: stronger syntax hierarchy improves scan speed, reading comfort, and premium perception.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/core/ThemeTokens.h`, `/Users/ryanrentfro/code/markamp/src/core/ThemeEngine.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp`
- Prior Plan References: `v22` Phase 07, `v24` and `v25` syntax/theme closure, `v26` Phase 07
- Scope: syntax semantic groups, punctuation emphasis, comment hierarchy, type/function/class balance, markdown code styling
- Out of Scope: parser or language service functionality
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/ThemeTokens.h`, `/Users/ryanrentfro/code/markamp/src/core/ThemeEngine.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp`
- Related Screens / Components / Surfaces: code editor, markdown editor, fenced code blocks, syntax-aware notebook cells
- Current Visual Behavior / Appearance: syntax is serviceable and theme-aware, but not yet unmistakably reauthored as a premium visual system.
- Intended Visual Behavior / Appearance: syntax hierarchy looks richer, calmer, and more intentional across languages and themes.
- Icon / Visual Design Direction: premium code palette with semantic clarity and restrained saturation.
- Technical Styling Approach: redesign semantic token roles and relative color/value hierarchy before per-language fine-tuning.
- Implementation Steps:
  1. Audit syntax token roles and visual hierarchy in representative languages.
  2. Redesign the semantic palette and relative emphasis model.
  3. Update theme-token mapping and syntax presentation rules.
  4. Validate across C++, Markdown, JSON, YAML, and scripting samples.
- Validation Steps:
  1. Review representative files in light and dark themes.
  2. Compare readability against the current implementation.
  3. Check syntax under active selection, search, and diagnostics overlays.
- Acceptance Criteria: syntax color and semantic hierarchy feel visibly redesigned and materially more premium than before.
- Dependencies: Phase 02 and Phase 07
- Parallelization Notes: can start once core editor shell direction is stable.
- Risks / Failure Modes: too much palette expressiveness can reduce calm and long-session comfort.
- Accessibility / Readability Notes: redesigned syntax must preserve safe contrast and long-session readability.
- Theme / Styling Notes: keep semantic roles stable across themes while varying values appropriately.
- Motion / Interaction Notes where relevant: none
- Cleanup / Consolidation Notes where relevant: retire old token mappings that no longer fit the redesigned palette.
- Rollback / Safety Notes: keep palette changes testable with representative sample sets.
- References / Context: syntax redesign is necessary to make the editor feel newly authored rather than tuned.
- Example scenarios where useful: a dense C++ implementation file should feel easier and more pleasant to read after the redesign.

### Phase ID: V27-P08
### Task ID: V27-P08-T02
### Task Title: Redesign Embedded Code And Cross Context Code Surface Presentation
- Priority: P1
- Category: Syntax Highlighting Presentation
- Objective: ensure code-looking content feels like one premium family across editor, markdown, notebook, and embedded contexts.
- Why This Matters Now: mixed-content products feel fragmented when embedded code surfaces do not visually relate to the main editor.
- Visual Gap Statement: code surfaces likely differ in background, border, spacing, and syntax emphasis depending on context.
- User Experience Impact: better consistency makes notebooks, markdown, and embedded code feel more trustworthy and premium.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/CellOutputRenderer.cpp`, `/Users/ryanrentfro/code/markamp/src/core/NotebookChromeCoordinator.h`
- Prior Plan References: `v22` syntax and notebook work, `v26` Phase 07
- Scope: markdown code fences, notebook code cells, embedded code blocks, mixed-content shell around syntax surfaces
- Out of Scope: notebook execution behavior
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/CellOutputRenderer.cpp`, `/Users/ryanrentfro/code/markamp/src/core/NotebookChromeCoordinator.h`
- Related Screens / Components / Surfaces: markdown code blocks, notebook code cells, output code surfaces, embedded editor-like regions
- Current Visual Behavior / Appearance: code surfaces are related conceptually but may not yet feel like one visually redesigned family.
- Intended Visual Behavior / Appearance: code in any context feels recognizably “MarkAmp” and premium, with context-appropriate shell only where needed.
- Icon / Visual Design Direction: unified code-surface identity with subtle contextual adaptation.
- Technical Styling Approach: define shared code-surface shell roles and syntax pairing rules used by all code-bearing surfaces.
- Implementation Steps:
  1. Inventory code-bearing contexts and their shells.
  2. Redesign shared code-surface frame, spacing, and support chrome rules.
  3. Align syntax presentation and surrounding chrome across contexts.
  4. Validate side-by-side code blocks in editor and notebook scenarios.
- Validation Steps:
  1. Compare identical code in editor, notebook, and markdown contexts.
  2. Review embedded language readability.
  3. Confirm context-specific shell differences feel intentional, not accidental.
- Acceptance Criteria: code surfaces across the app feel like one redesigned family rather than separate implementations.
- Dependencies: V27-P08-T01, Phase 09
- Parallelization Notes: can run in parallel with notebook redesign once shared syntax direction is approved.
- Risks / Failure Modes: over-unifying can erase legitimate contextual cues.
- Accessibility / Readability Notes: mixed-content code surfaces must remain highly legible and scannable.
- Theme / Styling Notes: code-surface shells should derive from semantic theme roles, not ad hoc colors.
- Motion / Interaction Notes where relevant: none
- Cleanup / Consolidation Notes where relevant: retire context-specific code shell hacks where shared roles suffice.
- Rollback / Safety Notes: preserve context-specific functionality while redesigning appearance.
- References / Context: this is the cross-surface code-presentation counterpart to core syntax redesign.
- Example scenarios where useful: a fenced code block in markdown and a notebook code cell should feel related at a glance.

### Phase ID: V27-P08
### Task ID: V27-P08-T03
### Task Title: Rebuild Dark Light Theme Syntax Balance For Long Session Comfort
- Priority: P1
- Category: Syntax Highlighting Presentation
- Objective: ensure the redesigned syntax system supports sustained reading comfort in both theme families.
- Why This Matters Now: a stronger visual identity is only successful if it remains comfortable in professional daily use.
- Visual Gap Statement: redesigned syntax can easily over-index on first impression if long-session balance is not deliberately tuned.
- User Experience Impact: sustained comfort increases trust and differentiates the product from merely stylish editors.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/core/ThemeEngine.cpp`, `/Users/ryanrentfro/code/markamp/src/core/ThemeTokens.h`
- Prior Plan References: `v22` Phase 18, `v26` Phase 07 and Phase 15
- Scope: light/dark syntax parity, saturation balance, contrast tiers, low-fatigue punctuation and comment treatment
- Out of Scope: new theme pack creation
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/ThemeEngine.cpp`, `/Users/ryanrentfro/code/markamp/src/core/ThemeTokens.h`, `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp`
- Related Screens / Components / Surfaces: code editor, notebook code cells, embedded code contexts
- Current Visual Behavior / Appearance: parity exists broadly, but the redesign must ensure both themes feel equally premium and livable.
- Intended Visual Behavior / Appearance: light and dark themes both feel intentionally designed for prolonged reading.
- Icon / Visual Design Direction: elegant low-fatigue palettes with refined semantic differentiation.
- Technical Styling Approach: treat parity as a visual-design calibration phase, not only a token-correctness checklist.
- Implementation Steps:
  1. Validate redesigned syntax in both theme families.
  2. Tune values and emphasis for comfort, not just novelty.
  3. Re-test under overlapping selection/search/diagnostic states.
  4. Document long-session readability criteria.
- Validation Steps:
  1. Review representative files for extended reading in both themes.
  2. Compare low-contrast and high-density segments.
  3. Confirm redesigned syntax remains comfortable under active editing states.
- Acceptance Criteria: redesigned syntax feels equally premium and comfortable in dark and light themes.
- Dependencies: V27-P08-T01, Phase 17
- Parallelization Notes: best finalized alongside broader theme parity work.
- Risks / Failure Modes: tuning by isolated screenshots can miss long-session fatigue.
- Accessibility / Readability Notes: preserve contrast and readability for users sensitive to glare or low contrast.
- Theme / Styling Notes: parity tuning must stay consistent with the larger redesigned theme system.
- Motion / Interaction Notes where relevant: none
- Cleanup / Consolidation Notes where relevant: document final syntax decisions to prevent drift after the redesign.
- Rollback / Safety Notes: keep a clear trail of token changes for quick reversions if comfort regresses.
- References / Context: long-session comfort is essential if `v27` aims to beat top IDEs in polish.
- Example scenarios where useful: reading a large header file for an hour in dark mode should feel calm and sustainable.
