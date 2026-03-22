# Phase 06: Editor Chrome Gutter Minimap And Inline Surface Polish

## Outcome

Raise the visible editor shell to elite quality through better gutter design, line-state styling, minimap polish, inline affordances, and overall code-reading comfort.

## Improvement Count

360 atomic improvements across 6 execution tasks.

### P06-T01

- Phase ID: P06
- Task ID: P06-T01
- Task Title: Rebuild editor frame, content margins, and surrounding chrome for calmer code reading
- Priority: P0
- Category: Editor UI
- Atomic Improvements Covered: 60
- Objective: Make the editor surface feel more intentional before token colors are even considered.
- Why This Matters Now: The editor is the product’s most scrutinized surface.
- UI Problem Statement: [EditorPanel.cpp](/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp) already applies strong styling, but many frame, margin, and shell decisions remain local and markdown-biased.
- User Experience Impact: Reading and editing code feels less premium and less focused than it should.
- Scope: Editor background layering, content margins, viewport padding, split-editor framing, editor-to-shell contrast, active-editor shell treatment.
- Out of Scope: Syntax token selection itself.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.h`; `/Users/ryanrentfro/code/markamp/src/ui/SplitView.cpp`; `/Users/ryanrentfro/code/markamp/src/core/ThemeTokens.h`
- Related Screens / Components / Surfaces: Main editor, split editors, markdown editor, code editor, diff-like code surfaces.
- Current UI Behavior / Appearance: The editor is capable but still carries locally tuned padding, edge, and shell treatments.
- Intended UI Behavior / Appearance: The editor feels balanced, focused, and premium in both single and split layouts.
- Visual / Interaction Design Direction: Quiet, confident editing surface with carefully tuned margins and crisp separation from surrounding chrome.
- Technical Styling Approach: Extract editor-frame tokens and align editor shell behavior to the broader workbench layout language.
- Implementation Steps: Audit editor frame and margin literals; define frame tokens; tune editor padding and shell contrast; align split-view editor framing; verify empty-content and active-editor visual states.
- Validation Steps: Compare editor appearance in single, split, and narrow layouts across dark and light themes.
- Acceptance Criteria: The editor frame and outer reading surface feel intentionally designed and less subsystem-local.
- Dependencies: Phase 01, Phase 02.
- Risks / Failure Modes: Too much shell framing can reduce available content width or make the editor feel boxed in.
- Accessibility / Readability Notes: Maintain generous but efficient margins for long reading sessions and low-vision support.
- Theme / Styling Notes: Background layering must keep focus without flattening the editor into the surrounding shell.
- Motion / Interaction Notes where relevant: Active-editor transitions should be subtle and not distract from code.
- Observability / Diagnostics Notes where relevant: Add editor-shell screenshots for single and split states.
- Rollback / Safety Notes: Keep content width and scroll behavior stable while visual shell changes are introduced.
- References / Context: [EditorPanel.cpp](/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp) already exposes many tuning knobs that now need system-level cleanup.
- Example scenarios where useful: A split editor session feels calm and balanced instead of tight and overly mechanical.

### P06-T02

- Phase ID: P06
- Task ID: P06-T02
- Task Title: Redesign gutters, line numbers, fold markers, and diagnostic lane hierarchy
- Priority: P0
- Category: Editor UI
- Atomic Improvements Covered: 60
- Objective: Make the gutter region more readable, less noisy, and more informative.
- Why This Matters Now: Gutter quality strongly shapes perceived editor maturity.
- UI Problem Statement: [EditorPanel.cpp](/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp) configures line numbers, fold markers, diagnostics, git markers, blame gutters, and margin visibility through many local decisions.
- User Experience Impact: Users receive information, but not always through a visually coherent gutter hierarchy.
- Scope: Line-number styling, fold markers, blame gutter, diagnostics glyphs, git markers, gutter separators, active-line gutter emphasis, margin widths.
- Out of Scope: Diagnostic semantics and VCS correctness.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/OverviewRulerPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/core/ThemeTokens.h`
- Related Screens / Components / Surfaces: Editor gutter, minimap edge, diagnostics gutter, blame gutter.
- Current UI Behavior / Appearance: The gutter contains many useful signals, but its hierarchy and style are still locally managed and uneven.
- Intended UI Behavior / Appearance: The gutter becomes a refined information lane with clear priorities and less clutter.
- Visual / Interaction Design Direction: Technical precision with stronger signal ordering and lower visual noise.
- Technical Styling Approach: Create gutter token families for line numbers, markers, folds, diagnostics, and VCS decorations.
- Implementation Steps: Audit every gutter element; define hierarchy rules; retune widths and iconography; reduce clutter; align fold and marker visuals with the broader icon system.
- Validation Steps: Open files with diagnostics, folds, and VCS changes and compare gutter readability under multiple themes.
- Acceptance Criteria: Gutter information is easier to read and visually more disciplined.
- Dependencies: Phase 01, Phase 07.
- Risks / Failure Modes: Simplifying the gutter too aggressively can hide needed cues for power users.
- Accessibility / Readability Notes: Line numbers and severity cues must remain visible at low-contrast and small-size conditions.
- Theme / Styling Notes: Diagnostics and VCS accents need enough contrast without turning the gutter into a rainbow strip.
- Motion / Interaction Notes where relevant: Gutter hover and fold-state changes should be calm and clear.
- Observability / Diagnostics Notes where relevant: Capture gutter screenshots for clean, folded, diagnostic-heavy, and VCS-heavy files.
- Rollback / Safety Notes: Keep all signals present while hierarchy is rebalanced.
- References / Context: The gutter is already feature-rich enough to deserve a full visual-information redesign.
- Example scenarios where useful: A file with folds, warnings, and git changes still reads cleanly in the gutter.

### P06-T03

- Phase ID: P06
- Task ID: P06-T03
- Task Title: Refine caret, selection, active line, bracket matching, and whitespace visibility styling
- Priority: P0
- Category: Editor UI
- Atomic Improvements Covered: 60
- Objective: Make core reading and editing states easier on the eyes and more precise.
- Why This Matters Now: Tiny editor-state visuals define day-to-day comfort.
- UI Problem Statement: [EditorPanel.cpp](/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp) directly configures caret, selection alpha, active line, brace highlight, whitespace dots, trailing whitespace indicators, and multiple cursor states.
- User Experience Impact: Editing remains usable, but not yet as legible or premium as top-tier IDEs.
- Scope: Caret width and color, selection fill and edge, multi-selection parity, active line band, brace match and mismatch styling, whitespace dots, trailing whitespace visibility, current search hit styling.
- Out of Scope: Language-semantic token mapping.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/core/ThemeTokens.h`; `/Users/ryanrentfro/code/markamp/src/rendering/CaretOverlay.h`
- Related Screens / Components / Surfaces: Main text editing lane, search states, diagnostics overlays.
- Current UI Behavior / Appearance: These states are present, but their visual weight and relationship are not yet fully refined.
- Intended UI Behavior / Appearance: Editing state cues are crisp, readable, and balanced for long sessions.
- Visual / Interaction Design Direction: High-confidence code-reading ergonomics with subtle emphasis and no muddy overlaps.
- Technical Styling Approach: Consolidate editor-state tokens and retune state interactions so selection, active line, and diagnostics do not visually compete.
- Implementation Steps: Audit editor-state overlaps; retune alpha and contrast; differentiate multi-cursor and brace states; improve whitespace modes; calibrate find and trailing-space highlights.
- Validation Steps: Test selection, multi-cursor, whitespace modes, and brace matching in long code files and markdown files.
- Acceptance Criteria: The editor feels less visually noisy and more precise during active editing.
- Dependencies: Phase 01, Phase 07.
- Risks / Failure Modes: Excessively subtle states may harm editing confidence.
- Accessibility / Readability Notes: Selection and active-line cues must remain visible for low-vision and color-sensitive users.
- Theme / Styling Notes: Dark themes especially need careful alpha tuning to avoid muddy overlays.
- Motion / Interaction Notes where relevant: Smooth-caret or state-transition effects should never reduce editing clarity.
- Observability / Diagnostics Notes where relevant: Add editor-state screenshots for selection, multi-cursor, brace, and whitespace modes.
- Rollback / Safety Notes: Keep state toggles user-configurable while visual defaults are improved.
- References / Context: The editor already exposes many visual controls; `v22` should make them coherent instead of merely available.
- Example scenarios where useful: A user selecting several occurrences across a code file gets clear, elegant multi-selection without overpowering the syntax colors.

### P06-T04

- Phase ID: P06
- Task ID: P06-T04
- Task Title: Redesign minimap, overview ruler, and edge-column visuals for higher information quality
- Priority: P1
- Category: Editor UI
- Atomic Improvements Covered: 60
- Objective: Turn auxiliary editor navigation aids into polished, readable tools instead of simplified sidecars.
- Why This Matters Now: Minimap and overview quality separates high-end editors from merely capable ones.
- UI Problem Statement: [MinimapPanel.cpp](/Users/ryanrentfro/code/markamp/src/ui/MinimapPanel.cpp) uses simplified token mapping and local rendering assumptions, while overview ruler and edge-column visuals also need stronger integration.
- User Experience Impact: Auxiliary navigation aids are less trustworthy and less pleasant than the main editor.
- Scope: Minimap rendering fidelity, viewport slider, token color compression, overview ruler density, edge-column styling, minimap hover toolbar.
- Out of Scope: Full high-performance rendering rewrite beyond visible quality targets.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/MinimapPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/OverviewRulerPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/MinimapHoverToolbar.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp`
- Related Screens / Components / Surfaces: Editor minimap, overview ruler, right-edge navigation aids.
- Current UI Behavior / Appearance: These tools work, but visually they read as simpler and rougher than the main editor.
- Intended UI Behavior / Appearance: Auxiliary navigation surfaces feel integrated, reliable, and polished.
- Visual / Interaction Design Direction: Compressed but high-fidelity code navigation with elegant viewport cues.
- Technical Styling Approach: Expand minimap/overview token mapping, unify navigation-sidecar shell styling, and improve signal compression logic.
- Implementation Steps: Audit minimap token compression; improve line rendering and viewport visuals; refine overview ruler marks; align hover toolbar styling; retune edge-column rendering.
- Validation Steps: Compare minimap and overview behavior on short files, long files, and diagnostic-heavy files.
- Acceptance Criteria: Minimap and overview surfaces look like first-class editor features rather than optional utilities.
- Dependencies: Phase 07, Phase 18.
- Risks / Failure Modes: Raising fidelity can increase rendering cost if not tuned carefully.
- Accessibility / Readability Notes: Viewport and severity indicators must remain visible even at tiny scales.
- Theme / Styling Notes: Compressed token colors need theme-specific calibration to remain readable.
- Motion / Interaction Notes where relevant: Minimap viewport movement and hover controls should feel smooth but quiet.
- Observability / Diagnostics Notes where relevant: Add visual snapshots for minimap and overview states plus performance notes.
- Rollback / Safety Notes: Stage fidelity increases behind performance-safe fallbacks for large files.
- References / Context: [MinimapPanel.cpp](/Users/ryanrentfro/code/markamp/src/ui/MinimapPanel.cpp) explicitly contains simplified MVP assumptions that `v22` should retire visually.
- Example scenarios where useful: A minimap in a long code file gives meaningful structure, search hits, and diagnostics cues without looking muddy.

### P06-T05

- Phase ID: P06
- Task ID: P06-T05
- Task Title: Polish inline editor furniture such as code actions, annotations, rulers, and floating toolbars
- Priority: P1
- Category: Editor UI
- Atomic Improvements Covered: 60
- Objective: Make inline editor utilities feel fully integrated into the editing experience.
- Why This Matters Now: Inline tools often reveal the largest quality gap between a functional editor and a premium one.
- UI Problem Statement: [CodeActionMenu.cpp](/Users/ryanrentfro/code/markamp/src/ui/CodeActionMenu.cpp), [FloatingToolbar.cpp](/Users/ryanrentfro/code/markamp/src/ui/FloatingToolbar.cpp), annotations, rulers, and peek-related UI need stronger visual alignment with the editor.
- User Experience Impact: Helpful inline utilities feel add-on-ish instead of native to the editing flow.
- Scope: Code actions, floating format bar, annotation boxes, inline markers, ruler hints, current execution markers, inline status tags.
- Out of Scope: LSP or formatting logic correctness.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/CodeActionMenu.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/FloatingToolbar.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/OverviewRulerPanel.cpp`
- Related Screens / Components / Surfaces: Inline controls, editor overlays, code action menus, annotations.
- Current UI Behavior / Appearance: Inline utilities exist, but their shell language and density are not fully harmonized.
- Intended UI Behavior / Appearance: Inline tools feel like natural extensions of the editor rather than foreign overlays.
- Visual / Interaction Design Direction: Tight, polished inline utilities with low visual intrusion and high clarity.
- Technical Styling Approach: Create editor-inline surface variants that inherit typography, spacing, and color rules from the editor and overlay systems.
- Implementation Steps: Inventory inline surfaces; unify shell and row styling; refine annotation and popup layouts; align icons and keyboard hints; tune overlap behavior with the editor content.
- Validation Steps: Trigger inline actions, floating toolbars, annotations, and execution markers across markdown and code documents.
- Acceptance Criteria: Inline editor utilities feel fully integrated and visually trustworthy.
- Dependencies: Phase 01, Phase 14.
- Risks / Failure Modes: Too much inline chrome can distract from code.
- Accessibility / Readability Notes: Inline controls need strong focus visibility and adequate hit targets.
- Theme / Styling Notes: Inline overlays should be distinct from content but not detach visually from the editor.
- Motion / Interaction Notes where relevant: Inline pop and fade behavior should feel restrained and responsive.
- Observability / Diagnostics Notes where relevant: Add overlay-state screenshots and compact-density cases.
- Rollback / Safety Notes: Keep inline utilities optically lightweight while functional parity is preserved.
- References / Context: Editor inline tools are a key premium differentiator if they stop looking incidental.
- Example scenarios where useful: Triggering a code action produces a compact, elegant inline menu that visually belongs to the editor.

### P06-T06

- Phase ID: P06
- Task ID: P06-T06
- Task Title: Add editor-chrome visual baselines and long-session readability review gates
- Priority: P0
- Category: Cleanup / Consolidation
- Atomic Improvements Covered: 60
- Objective: Ensure editor polish survives future feature and token changes.
- Why This Matters Now: The editor has too many local styling paths to rely on memory alone.
- UI Problem Statement: Editor visual quality spans dozens of manually tuned subsystems and currently lacks one regression-protected baseline set.
- User Experience Impact: Small readability regressions can accumulate silently until the editor feels rougher.
- Scope: Editor shell, gutter, selection states, minimap, whitespace modes, diagnostics states, inline tools, split editors.
- Out of Scope: Performance benchmarking beyond representative editor surface captures.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/tests/unit`; `/Users/ryanrentfro/code/markamp/docs/v22_docs`; `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/MinimapPanel.cpp`
- Related Screens / Components / Surfaces: Editor variants and sub-surfaces.
- Current UI Behavior / Appearance: Editor polish is rich but fragile and difficult to review systematically.
- Intended UI Behavior / Appearance: Major editor visual states are documented, referenceable, and regression-gated.
- Visual / Interaction Design Direction: Operationally mature editor quality management.
- Technical Styling Approach: Build screenshot fixtures and reading-comfort reviews for key editor states and densities.
- Implementation Steps: Define editor fixture documents; capture states; publish visual baselines; add regression and manual readability review checklists.
- Validation Steps: Intentionally perturb line-number contrast or selection styling and verify the review stack catches the drift.
- Acceptance Criteria: Editor-surface regressions become visible and actionable before release.
- Dependencies: P06-T01 through P06-T05.
- Risks / Failure Modes: Screenshot baselines can become noisy unless editor content and theme states are deterministic.
- Accessibility / Readability Notes: Include low-vision, high-contrast, and reduced-motion captures.
- Theme / Styling Notes: Cover dark, light, and high-contrast editor variants explicitly.
- Motion / Interaction Notes where relevant: Capture representative animated states where appearance changes materially.
- Observability / Diagnostics Notes where relevant: Pair image baselines with token and state metadata.
- Rollback / Safety Notes: Start with advisory baselines if early editor redesign is still in flux.
- References / Context: The editor deserves the strongest visual QA discipline in the product.
- Example scenarios where useful: A future change that weakens active-line readability is caught before it ships.

