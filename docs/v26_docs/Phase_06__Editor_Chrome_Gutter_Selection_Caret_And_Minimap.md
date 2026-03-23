# Phase 06 - Editor Chrome Gutter Selection Caret And Minimap

## Goal

Make the core editor experience feel premium, precise, and deeply readable through better chrome, spacing, selection, diagnostics, and minimap presentation.

## Definition Of Done

- editor chrome, gutter, active line, caret, selection, search highlighting, and diagnostics feel like one system
- readability is improved in both light and dark themes
- the minimap no longer feels MVP-level next to the main editor

## Tasks

### Phase ID: V26-P06
### Task ID: V26-P06-T01
### Task Title: Refine Editor Surface Chrome, Gutter Hierarchy, And Reading Rhythm
- Priority: P0
- Category: Editor UI
- Objective: Tune the editor’s static visual foundation for premium code and markdown readability.
- Why This Matters Now: The editor is the product’s primary surface and still carries visible signs of organic, local styling growth.
- UI Gap Statement: `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp` has rich functionality but heavy local font and style handling.
- User Experience Impact: Better reading rhythm increases comfort and makes the product feel significantly more polished.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/core/ThemeTokens.h`
- Prior Plan References: `v22` editor UI work, `v25` release-path editor hardening
- Scope: editor background layering, gutter width rhythm, line number styling, active-line treatment, whitespace/ruler styling
- Out of Scope: syntax token mapping, handled in Phase 07
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/core/ThemeTokens.h`
- Related Screens / Components / Surfaces: markdown editor, code editor, split editor states
- Current UI Behavior / Appearance: the editor is capable but visually closer to a well-tuned implementation than a deliberately composed premium reading surface.
- Intended UI Behavior / Appearance: the editor feels calm, crisp, and high-end with disciplined support chrome that never competes with content.
- Visual / Interaction Design Direction: quiet, high-clarity authoring surface with subtle support cues and excellent alignment.
- Technical Styling Approach: centralize editor UI roles and reduce local style branching inside the panel.
- Implementation Steps:
  1. Audit current editor chrome and supporting visual elements.
  2. Normalize gutter and line number hierarchy.
  3. Tune active-line and whitespace/ruler visibility.
  4. Validate markdown and code editing side by side.
- Validation Steps:
  1. Compare long-form markdown and dense code files.
  2. Review editor at common zoom and DPI scales.
  3. Check split-editor layouts for visual consistency.
- Acceptance Criteria: editor chrome recedes appropriately while retaining premium clarity and alignment.
- Dependencies: V26-P01-T01, V26-P01-T02, V26-P01-T03
- Parallelization Notes: should start early; later syntax and theme work depend on it.
- Risks / Failure Modes: over-minimizing support chrome can hurt orientation and diagnostics legibility.
- Accessibility / Readability Notes: line numbers and active line need adequate contrast without overwhelming content.
- Theme / Styling Notes: balance subtlety differently per theme while keeping shared roles intact.
- Motion / Interaction Notes where relevant: none
- Cleanup / Consolidation Notes where relevant: remove editor-local style branches that duplicate shared roles.
- Rollback / Safety Notes: preserve established editing behavior and shortcuts.
- References / Context: this is a premium readability task, not a feature task.
- Example scenarios where useful: a large markdown file and a dense C++ file should both feel comfortable for extended reading.

### Phase ID: V26-P06
### Task ID: V26-P06-T02
### Task Title: Upgrade Selection, Caret, Search Match, Bracket, And Diagnostics Presentation
- Priority: P0
- Category: Editor UI
- Objective: Make the editor’s dynamic states feel precise and premium without visual clutter.
- Why This Matters Now: Dynamic state styling is where users most strongly perceive responsiveness and trustworthiness.
- UI Gap Statement: Selection, match, and diagnostics styling often evolve independently and can easily become noisy or inconsistent.
- User Experience Impact: Better dynamic-state presentation improves confidence during editing, search, and error correction.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/core/ThemeTokens.h`
- Prior Plan References: `v22` editor polish, `v25` diagnostics and release-path trustworthiness
- Scope: caret, selection, search highlight, bracket match, diagnostics markers, current-line overlays
- Out of Scope: diagnostic engine logic
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/core/ThemeTokens.h`
- Related Screens / Components / Surfaces: code editing, markdown editing, find/replace, inline diagnostics
- Current UI Behavior / Appearance: state treatments work, but may not yet feel harmonized into a premium visual language.
- Intended UI Behavior / Appearance: dynamic states are immediately understandable, aesthetically restrained, and highly legible.
- Visual / Interaction Design Direction: precise emphasis with clear hierarchy and zero neon-like noise.
- Technical Styling Approach: derive all dynamic editor states from a coordinated token model and shared opacity/intensity rules.
- Implementation Steps:
  1. Audit dynamic state colors and overlays.
  2. Define relative intensities for caret, selection, search, bracket, and diagnostics.
  3. Apply revised tokens and painting behavior.
  4. Validate overlapping states and edge cases.
- Validation Steps:
  1. Test search in files with diagnostics present.
  2. Verify bracket matching in dense code.
  3. Compare selection visibility in both themes.
- Acceptance Criteria: dynamic editor states are distinct, coordinated, and visually refined across themes and file types.
- Dependencies: V26-P06-T01, V26-P01-T03
- Parallelization Notes: can run while minimap polish is underway.
- Risks / Failure Modes: overlapping highlights can become muddy if intensity rules are unclear.
- Accessibility / Readability Notes: selection and diagnostics must remain visible for low-vision users.
- Theme / Styling Notes: avoid over-saturated accents that break the shell’s restrained palette.
- Motion / Interaction Notes where relevant: cursor blink and any subtle highlight transitions should remain restrained.
- Cleanup / Consolidation Notes where relevant: remove local dynamic-state overrides where shared rules exist.
- Rollback / Safety Notes: test edge cases with multi-cursor, find, and diagnostics overlays before removing old styling.
- References / Context: this work directly affects perceived authoring quality.
- Example scenarios where useful: search results over a selected diagnostic line should remain readable and not visually collapse.

### Phase ID: V26-P06
### Task ID: V26-P06-T03
### Task Title: Bring The Minimap To Visual Parity With The Main Editor Surface
- Priority: P1
- Category: Editor UI
- Objective: Upgrade minimap styling, density, and token fidelity so it feels like a polished companion surface rather than an MVP add-on.
- Why This Matters Now: The minimap is a persistent editor-adjacent surface that can visibly lower overall quality if left simplified.
- UI Gap Statement: `/Users/ryanrentfro/code/markamp/src/ui/MinimapPanel.cpp` still uses simplified token mapping and reduced visual fidelity.
- User Experience Impact: A better minimap reinforces orientation and contributes to a premium editor experience.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/MinimapPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp`
- Prior Plan References: `v22` editor/minimap polish
- Scope: minimap background, code block rendering, selection/viewport highlight, line density, theme parity
- Out of Scope: minimap navigation logic changes
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/MinimapPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp`
- Related Screens / Components / Surfaces: main editor, minimap, split editor layouts
- Current UI Behavior / Appearance: minimap rendering is simplified enough to read as less mature than the editor itself.
- Intended UI Behavior / Appearance: minimap looks lightweight but premium, with token-aware rendering and clear viewport indication.
- Visual / Interaction Design Direction: refined auxiliary map with subtle structure and excellent relationship to the main editor.
- Technical Styling Approach: improve token mapping and shared editor-state interpretation without overcomplicating the minimap.
- Implementation Steps:
  1. Review current minimap token simplifications.
  2. Tune glyph/block rendering and background layering.
  3. Refine viewport and selection indicators.
  4. Validate performance while increasing fidelity.
- Validation Steps:
  1. Compare minimap appearance for markdown and code files.
  2. Check light/dark parity and viewport contrast.
  3. Ensure rendering remains performant on large files.
- Acceptance Criteria: minimap visually supports the premium editor instead of looking like an unfinished sidecar.
- Dependencies: V26-P06-T01, V26-P07-T01
- Parallelization Notes: best done after main editor chrome is stable and while syntax hierarchy work is in progress.
- Risks / Failure Modes: over-rendering can hurt performance or make the minimap too busy.
- Accessibility / Readability Notes: viewport highlight must remain visible without overpowering the minimap.
- Theme / Styling Notes: minimap colors should derive from editor/theme semantics, not custom one-offs.
- Motion / Interaction Notes where relevant: viewport movement should feel smooth but restrained if animated.
- Cleanup / Consolidation Notes where relevant: retire “MVP” style simplifications once the new mapping is validated.
- Rollback / Safety Notes: keep rendering paths simple enough to fall back if performance regresses.
- References / Context: minimap parity is a premium-quality differentiator, not merely a feature checkbox.
- Example scenarios where useful: scrolling through a long C++ file should show a minimap that feels crisp and intentionally designed.
