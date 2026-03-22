# Phase 08: Search Find Replace Diagnostics Peek And Editor Overlays

## Outcome

Make search, find/replace, diagnostics views, peek surfaces, and editor-adjacent overlays feel precise, discoverable, and deeply integrated into the premium editing experience.

## Improvement Count

360 atomic improvements across 6 execution tasks.

### P08-T01

- Phase ID: P08
- Task ID: P08-T01
- Task Title: Redesign in-editor find and replace UI for clarity, hierarchy, and reduced visual friction
- Priority: P0
- Category: Editor UI
- Atomic Improvements Covered: 60
- Objective: Make find/replace controls feel lighter, clearer, and more premium while preserving speed.
- Why This Matters Now: Find/replace is one of the most frequently invoked editor workflows.
- UI Problem Statement: The editing surface contains search highlighting and related state, but the visible find/replace UI still needs stronger product-level polish.
- User Experience Impact: Routine search tasks feel more mechanical and less refined than they should.
- Scope: Find bar shell, field styling, toggle buttons, result counts, replace controls, option chips, inline feedback, compact states.
- Out of Scope: Search algorithm correctness.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/SearchPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/SearchPanelModel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/ThemedTextInput.cpp`
- Related Screens / Components / Surfaces: In-editor search, replace UI, search overlays.
- Current UI Behavior / Appearance: Search controls exist, but the visual shell, density, and option treatments are not yet fully premium.
- Intended UI Behavior / Appearance: Find/replace looks compact, clear, and naturally integrated with the editor.
- Visual / Interaction Design Direction: High-utility micro-overlay with strong hierarchy and low clutter.
- Technical Styling Approach: Create search-control variants built from shared input, chip, and toolbar tokens.
- Implementation Steps: Audit current search UI; redesign shell and field treatments; refine count and option displays; align replace controls with the broader control system.
- Validation Steps: Exercise compact and expanded find/replace states across code and markdown files.
- Acceptance Criteria: Find/replace feels faster to scan and more polished to use.
- Dependencies: Phase 01, Phase 06, Phase 14.
- Risks / Failure Modes: Compacting the UI too much can hide options that matter to power users.
- Accessibility / Readability Notes: Match count, active option, and focus position must remain obvious.
- Theme / Styling Notes: Search surfaces must remain distinct from editor content without looking detached.
- Motion / Interaction Notes where relevant: Search bar show/hide transitions should be crisp and minimal.
- Observability / Diagnostics Notes where relevant: Add find/replace screenshot states and option combinations.
- Rollback / Safety Notes: Preserve existing option coverage while visual layout is refined.
- References / Context: Find and replace is a daily workflow and deserves first-class visual care.
- Example scenarios where useful: A user can open replace mode and understand every option instantly without crowding the editor.

### P08-T02

- Phase ID: P08
- Task ID: P08-T02
- Task Title: Upgrade search result trees, result previews, and replace-preview visual hierarchy
- Priority: P1
- Category: Panel UI
- Atomic Improvements Covered: 60
- Objective: Improve large-result search surfaces so users can scan, compare, and act faster.
- Why This Matters Now: Search result UIs often become cluttered and low-trust at scale.
- UI Problem Statement: [SearchSidebarPanel.cpp](/Users/ryanrentfro/code/markamp/src/ui/SearchSidebarPanel.cpp), [SearchResultsTree.cpp](/Users/ryanrentfro/code/markamp/src/ui/SearchResultsTree.cpp), and related panels need stronger row hierarchy, preview emphasis, and replace-preview styling.
- User Experience Impact: Search-heavy workflows feel denser and more fragile than necessary.
- Scope: Tree row rhythm, match emphasis, file vs result hierarchy, replace-preview badges, empty/loading/error states, section headers, metadata placement.
- Out of Scope: Search backend behavior.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/SearchSidebarPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/SearchResultsTree.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/SearchResultsPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/core/SearchReplacePreview.h`
- Related Screens / Components / Surfaces: Sidebar search results, replace previews, search panels.
- Current UI Behavior / Appearance: Search result presentation is useful but still visually closer to a functional tree than a premium information surface.
- Intended UI Behavior / Appearance: Search results become easier to scan, with strong grouping and low clutter.
- Visual / Interaction Design Direction: Dense but airy search result layout with excellent contrast between file rows, match rows, and replace metadata.
- Technical Styling Approach: Apply shared tree/list row tokens plus search-specific match emphasis and preview metadata styles.
- Implementation Steps: Audit row and preview states; retune hierarchy and padding; refine inline highlights; improve result-group separation; align replace-preview affordances.
- Validation Steps: Run sparse and dense searches with grouped results and replace-preview states.
- Acceptance Criteria: Search result surfaces feel significantly more readable under heavy use.
- Dependencies: Phase 01, Phase 11, Phase 15.
- Risks / Failure Modes: Too much whitespace can reduce information density unacceptably for expert users.
- Accessibility / Readability Notes: Match highlights and current-result focus must remain distinct for keyboard users.
- Theme / Styling Notes: Inline result highlights need strong theme calibration to avoid looking muddy.
- Motion / Interaction Notes where relevant: Expand/collapse and result-hover behaviors should be subtle.
- Observability / Diagnostics Notes where relevant: Add dense-result screenshots and match highlight references.
- Rollback / Safety Notes: Preserve current tree behavior while visual row systems are upgraded.
- References / Context: Search result quality is a major differentiator for knowledge-heavy tools.
- Example scenarios where useful: A search returning hundreds of matches remains visually parseable with clear file grouping and match emphasis.

### P08-T03

- Phase ID: P08
- Task ID: P08-T03
- Task Title: Refine diagnostics presentation across editor annotations, problems overlays, and ruler marks
- Priority: P1
- Category: Editor UI
- Atomic Improvements Covered: 60
- Objective: Make diagnostic UI feel more integrated, more readable, and less noisy.
- Why This Matters Now: Diagnostics are one of the most visually charged editor features and easily become harsh.
- UI Problem Statement: Diagnostics appear across underlines, annotations, gutter markers, overview ruler marks, and Problems panel pathways, but their visible treatment still needs stronger integration.
- User Experience Impact: Error states can feel noisy and warning states can feel weak or inconsistent.
- Scope: Diagnostic severity palette, underline patterns, annotations, current-problem focus, ruler marks, inline quick-fix affordances, severity chips.
- Out of Scope: Diagnostic source correctness or quick-fix logic.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/OverviewRulerPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/ProblemsPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/core/ThemeTokens.h`
- Related Screens / Components / Surfaces: Editor diagnostics, Problems panel, overview ruler, quick-fix cues.
- Current UI Behavior / Appearance: Severity styling exists, but the visual relationship between channels is not yet premium.
- Intended UI Behavior / Appearance: Diagnostics look serious and actionable without destabilizing the visual calm of the editor.
- Visual / Interaction Design Direction: Layered severity system with refined urgency and strong cross-surface consistency.
- Technical Styling Approach: Create a diagnostics visual hierarchy shared across editor, rulers, and panels.
- Implementation Steps: Audit severity visuals; retune underline and marker styles; align annotation shells; coordinate Problems panel severity chips with editor cues.
- Validation Steps: Review files with mixed severities and compare editor, ruler, and panel representations.
- Acceptance Criteria: Diagnostics are easier to understand and less visually fatiguing.
- Dependencies: Phase 06, Phase 07, Phase 12.
- Risks / Failure Modes: Too much softness can reduce perceived urgency for real errors.
- Accessibility / Readability Notes: Severity must be encoded by more than hue alone.
- Theme / Styling Notes: Error red and warning amber need theme-specific balance to avoid harshness.
- Motion / Interaction Notes where relevant: Diagnostic reveal and quick-fix affordances should not flicker or distract.
- Observability / Diagnostics Notes where relevant: Add diagnostic severity screenshot references.
- Rollback / Safety Notes: Keep severity semantics unchanged while their visible treatment is improved.
- References / Context: Diagnostics are both functional and expressive; they need tighter design discipline.
- Example scenarios where useful: A file with multiple warnings and one error remains readable without turning into a warning-colored mess.

### P08-T04

- Phase ID: P08
- Task ID: P08-T04
- Task Title: Elevate peek editors, diff/merge overlays, and code-inspection sub-surfaces
- Priority: P2
- Category: Editor UI
- Atomic Improvements Covered: 60
- Objective: Make secondary code-inspection surfaces feel more integrated and less temporary.
- Why This Matters Now: Peek, diff, and merge surfaces often receive less polish despite being important advanced tools.
- UI Problem Statement: Files like [PeekEditorHost.cpp](/Users/ryanrentfro/code/markamp/src/ui/PeekEditorHost.cpp), [PeekResultList.cpp](/Users/ryanrentfro/code/markamp/src/ui/PeekResultList.cpp), [DiffPanel.cpp](/Users/ryanrentfro/code/markamp/src/ui/DiffPanel.cpp), and [MergeEditor.cpp](/Users/ryanrentfro/code/markamp/src/ui/MergeEditor.cpp) need a clearer common visual language.
- User Experience Impact: Advanced inspection surfaces can feel visually disconnected from the main editor.
- Scope: Peek container chrome, result list hierarchy, diff gutter styling, insertion/removal color balance, merge conflict framing, inline compare shells.
- Out of Scope: Diff engine or merge-resolution logic.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/PeekEditorHost.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/PeekResultList.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/DiffPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/MergeEditor.cpp`
- Related Screens / Components / Surfaces: Peek views, diffs, merge editors, code inspection overlays.
- Current UI Behavior / Appearance: Advanced code-inspection surfaces exist but feel less coherent than the main editor.
- Intended UI Behavior / Appearance: Secondary code-inspection surfaces feel like true members of the editor family.
- Visual / Interaction Design Direction: Focused inspection surfaces with clear hierarchy and reduced visual harshness.
- Technical Styling Approach: Reuse editor frame and diagnostics systems with specialized diff/peek tokens layered on top.
- Implementation Steps: Audit peek and diff shells; retune diff colors and gutter treatments; improve result list rhythm; align merge conflict blocks with the editor system.
- Validation Steps: Compare peek, diff, and merge states with main editor appearance in dark and light themes.
- Acceptance Criteria: These advanced surfaces no longer look like lower-priority UI.
- Dependencies: Phase 06, Phase 07.
- Risks / Failure Modes: Diff colors can become too subtle if readability is over-prioritized over contrast.
- Accessibility / Readability Notes: Insertions and deletions must remain distinguishable without relying on green/red alone.
- Theme / Styling Notes: Diff background fills need especially careful calibration in dark themes.
- Motion / Interaction Notes where relevant: Open/close transitions should remain minimal and not obscure context.
- Observability / Diagnostics Notes where relevant: Add peek/diff/merge screenshot suites.
- Rollback / Safety Notes: Preserve current diff semantics and selection behaviors during visual rework.
- References / Context: Advanced code-inspection surfaces are where elite tools often outperform average ones.
- Example scenarios where useful: Opening a peek editor feels like a refined nested editing surface, not a bolted-on popup.

### P08-T05

- Phase ID: P08
- Task ID: P08-T05
- Task Title: Unify editor-adjacent overlays such as spotlight, shortcut, and mode hints
- Priority: P2
- Category: Editor UI
- Atomic Improvements Covered: 60
- Objective: Harmonize cross-editor overlays that currently feel stylistically independent.
- Why This Matters Now: Auxiliary overlays shape expert-user perception of polish.
- UI Problem Statement: [SpotlightOverlay.cpp](/Users/ryanrentfro/code/markamp/src/ui/SpotlightOverlay.cpp), [ShortcutOverlay.cpp](/Users/ryanrentfro/code/markamp/src/ui/ShortcutOverlay.cpp), and related overlay models are useful but not clearly part of one transient UI family.
- User Experience Impact: Expert overlays can feel like separate utilities rather than refined editor companions.
- Scope: Overlay shells, backdrop treatment, typography, iconography, instructional text hierarchy, overlay spacing, dismiss affordances.
- Out of Scope: Shortcut or help data correctness.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/SpotlightOverlay.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/ShortcutOverlay.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/ShortcutOverlayModel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/DialogStackManager.cpp`
- Related Screens / Components / Surfaces: Spotlight overlay, shortcut hints, mode overlays, teaching surfaces.
- Current UI Behavior / Appearance: Overlays exist, but they are not yet part of one premium transient design family.
- Intended UI Behavior / Appearance: Overlays feel consistent, calm, and visually more sophisticated.
- Visual / Interaction Design Direction: Lightweight but deliberate guidance surfaces with crisp framing and minimal visual clutter.
- Technical Styling Approach: Define overlay shell variants and helper typography rules shared across transient guidance surfaces.
- Implementation Steps: Audit overlay family; align shell, backdrop, and text hierarchy; standardize icon usage; refine dismiss and focus cues; ensure shared spacing rhythm.
- Validation Steps: Trigger shortcut, spotlight, and other teaching overlays across themes and window sizes.
- Acceptance Criteria: Auxiliary overlays feel like one coherent system rather than several historical experiments.
- Dependencies: Phase 01, Phase 14, Phase 19.
- Risks / Failure Modes: Over-polishing utility overlays can make them feel heavier than the tasks they support.
- Accessibility / Readability Notes: Overlays must remain legible and focus-aware, especially for keyboard users.
- Theme / Styling Notes: Backdrop and panel contrast must remain balanced with surrounding editor content.
- Motion / Interaction Notes where relevant: Overlay entry/exit should feel precise and restrained.
- Observability / Diagnostics Notes where relevant: Add overlay state captures to the transient-surface gallery.
- Rollback / Safety Notes: Keep existing overlay activation behavior intact while visual standardization is applied.
- References / Context: Auxiliary overlays are small but visible markers of overall interface maturity.
- Example scenarios where useful: A shortcut overlay looks like a polished product teaching surface instead of a debug helper.

### P08-T06

- Phase ID: P08
- Task ID: P08-T06
- Task Title: Add search and editor-overlay visual regression boards
- Priority: P0
- Category: Cleanup / Consolidation
- Atomic Improvements Covered: 60
- Objective: Protect search, diagnostics, peek, diff, and overlay polish with explicit visual references.
- Why This Matters Now: These surfaces cut across editor, panel, and transient UI families and are therefore easy to regress.
- UI Problem Statement: Editor-adjacent overlays and search surfaces currently lack one visual review board.
- User Experience Impact: Cross-surface polish drifts and inconsistency returns quietly.
- Scope: Find bar, result trees, diagnostics overlays, peek editor, diff/merge, spotlight, shortcut overlays.
- Out of Scope: Backend search correctness.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/tests/unit`; `/Users/ryanrentfro/code/markamp/docs/v22_docs`; `/Users/ryanrentfro/code/markamp/src/ui/SearchSidebarPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/PeekEditorHost.cpp`
- Related Screens / Components / Surfaces: Search and inspection surface family.
- Current UI Behavior / Appearance: These surfaces are reviewed piecemeal rather than as one coherent family.
- Intended UI Behavior / Appearance: Search and overlay surfaces have visual baselines, galleries, and regression protection.
- Visual / Interaction Design Direction: Coherent auxiliary-surface governance.
- Technical Styling Approach: Build grouped screenshot suites covering in-editor and side-panel search plus overlay states.
- Implementation Steps: Define fixtures; capture representative states; publish review boards; add visual drift checks for major auxiliary surfaces.
- Validation Steps: Intentionally alter find-bar density or diff styling and verify the regression board flags it.
- Acceptance Criteria: Search and overlay polish is durable and easier to review.
- Dependencies: P08-T01 through P08-T05.
- Risks / Failure Modes: Fixture sprawl can make review noisy if the family taxonomy is not well organized.
- Accessibility / Readability Notes: Include focus-visible and keyboard-selected states.
- Theme / Styling Notes: Cover dark, light, and high-contrast variants of all major auxiliary surfaces.
- Motion / Interaction Notes where relevant: Include representative open/close states for transient overlays.
- Observability / Diagnostics Notes where relevant: Store fixture metadata with surface family and state IDs.
- Rollback / Safety Notes: Start with reference boards before promoting to stricter gating.
- References / Context: Cross-cutting UI families need governance as much as primary shell surfaces do.
- Example scenarios where useful: A future change that improves find-bar contrast but harms peek-editor balance is surfaced immediately.

