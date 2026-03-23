# Phase 05 - Panels Inspectors And Dense Information Chrome

## Goal

Bring panel headers, explorers, inspectors, search panels, output surfaces, and dense information views to shell-quality parity.

## Definition Of Done

- panel headers and action areas are standardized
- dense information surfaces are readable and visually calm
- empty, loading, and underdeveloped panel states no longer feel temporary

## Tasks

### Phase ID: V26-P05
### Task ID: V26-P05-T01
### Task Title: Standardize Panel Header Chrome, Section Hierarchy, And Action Density
- Priority: P0
- Category: Panel UI
- Objective: Create one premium panel-header system covering titles, actions, section dividers, and supporting metadata.
- Why This Matters Now: Panels are everywhere, and inconsistent header treatment makes the app feel stitched together.
- UI Gap Statement: Explorer, problems, search, terminal, and output surfaces likely use different header rhythm and action density.
- User Experience Impact: Better panel chrome improves scan speed and makes multi-panel workspaces feel cleaner and more trustworthy.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/ExplorerPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/ProblemsPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/SearchSidebarPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/OutputPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/TerminalPanel.cpp`
- Prior Plan References: `v22` panel work, `v25` release-path panel closure
- Scope: panel title rows, action-button placement, header spacing, section separators, metadata/subtitle styling
- Out of Scope: panel command logic
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/ExplorerPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/ProblemsPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/SearchSidebarPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/OutputPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/TerminalPanel.cpp`
- Related Screens / Components / Surfaces: explorer, problems, search sidebar, terminal, output, inspector panels
- Current UI Behavior / Appearance: panel headers and actions appear to be styled per surface with varying rhythm and emphasis.
- Intended UI Behavior / Appearance: panel headers all feel like members of one design family with predictable action density and hierarchy.
- Visual / Interaction Design Direction: crisp, compact panel chrome with calm sectioning and clear affordances.
- Technical Styling Approach: define a shared panel-header role system and migrate high-traffic panels first.
- Implementation Steps:
  1. Inventory panel header variants and action patterns.
  2. Define standard title, subtitle, action, and divider metrics.
  3. Refactor panel headers to shared helpers.
  4. Validate mixed panel layouts in the shell.
- Validation Steps:
  1. Open explorer, search, problems, terminal, and output together.
  2. Compare header spacing and button balance.
  3. Validate long titles and action overflow.
- Acceptance Criteria: core panel surfaces share one consistent header and action language.
- Dependencies: V26-P01-T01, V26-P01-T02, V26-P03-T01
- Parallelization Notes: can begin once toolbar action metrics are stable.
- Risks / Failure Modes: over-compressing headers can hurt discoverability.
- Accessibility / Readability Notes: maintain clear title contrast and adequate action targets.
- Theme / Styling Notes: panel chrome must align with shell layering.
- Motion / Interaction Notes where relevant: any hover or activation states should match shell control behavior.
- Cleanup / Consolidation Notes where relevant: remove per-panel header constants after migration.
- Rollback / Safety Notes: keep command targets stable while changing layout.
- References / Context: panel chrome is one of the main remaining consistency gaps.
- Example scenarios where useful: switching between explorer and problems should not feel like moving between unrelated tools.

### Phase ID: V26-P05
### Task ID: V26-P05-T02
### Task Title: Refine Explorer, Search Sidebar, And Problems Panel For Premium Dense Information Readability
- Priority: P0
- Category: Panel UI
- Objective: Improve scan rhythm, row hierarchy, metadata presentation, and empty-state quality in the highest-traffic dense panels.
- Why This Matters Now: Dense information surfaces are where custom apps most often lose polish.
- UI Gap Statement: `/Users/ryanrentfro/code/markamp/src/ui/SearchSidebarPanel.cpp` and `/Users/ryanrentfro/code/markamp/src/ui/ProblemsPanel.cpp` still show placeholder and temporary cues, while `/Users/ryanrentfro/code/markamp/src/ui/ExplorerPanel.cpp` contains visibly incomplete sections.
- User Experience Impact: Cleaner dense panels make professional workflows feel faster and more trustworthy.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/ExplorerPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/SearchSidebarPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/ProblemsPanel.cpp`
- Prior Plan References: `v22` panel/list polish, `v25` release blocker closure for panel trustworthiness
- Scope: row height, indentation, metadata labels, badges, section empties, placeholder cleanup
- Out of Scope: search or diagnostic engine changes
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/ExplorerPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/SearchSidebarPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/ProblemsPanel.cpp`
- Related Screens / Components / Surfaces: file explorer, outline area, timeline section, search results, problems list
- Current UI Behavior / Appearance: dense surfaces function, but visual rhythm and completion quality remain uneven.
- Intended UI Behavior / Appearance: rows, badges, metadata, and empty states are scannable, elegant, and consistent with the shell.
- Visual / Interaction Design Direction: compact but breathable density with disciplined metadata and soft emphasis.
- Technical Styling Approach: create shared dense-row metrics and reusable metadata/badge styles.
- Implementation Steps:
  1. Remove visible placeholder and temporary visual language on the UI path.
  2. Standardize row density, hierarchy indicators, and metadata treatment.
  3. Add elegant empty/loading/error treatments where sections are currently thin.
  4. Validate panel cohabitation in narrow and wide sidebars.
- Validation Steps:
  1. Populate explorer, search, and problems with realistic data.
  2. Compare row alignment, indentation, and selection states.
  3. Verify empty sections no longer feel broken or abandoned.
- Acceptance Criteria: high-traffic dense panels feel mature, readable, and free of obviously unfinished presentation.
- Dependencies: V26-P05-T01
- Parallelization Notes: can proceed while other panel types are being standardized.
- Risks / Failure Modes: too much whitespace can hurt information density goals.
- Accessibility / Readability Notes: selection, error, and focus states must remain distinct without relying only on color.
- Theme / Styling Notes: badges and metadata should use theme-safe contrast tiers.
- Motion / Interaction Notes where relevant: row hover should stay understated.
- Cleanup / Consolidation Notes where relevant: retire local placeholder visuals and repeated badge styling logic.
- Rollback / Safety Notes: preserve row hit targets and list performance while changing visuals.
- References / Context: these panels are among the clearest remaining “custom app versus premium app” differentiators.
- Example scenarios where useful: a search results list with file paths and snippets should remain scannable at a glance.

### Phase ID: V26-P05
### Task ID: V26-P05-T03
### Task Title: Bring Terminal, Output, History, And PDF Panel Shells Up To Product-Level Visual Quality
- Priority: P1
- Category: Panel UI
- Objective: Upgrade lower-maturity panels so they no longer feel visually secondary or unfinished.
- Why This Matters Now: A premium app is judged by weak surfaces as much as by strong ones.
- UI Gap Statement: `/Users/ryanrentfro/code/markamp/src/ui/HistoryPanel.cpp` and `/Users/ryanrentfro/code/markamp/src/ui/PDFViewerPanel.cpp` are still visibly stub-like, while terminal/output surfaces carry heavy local styling.
- User Experience Impact: Secondary panels feel trustworthy and integrated instead of auxiliary or temporary.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/TerminalPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/OutputPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/HistoryPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/PDFViewerPanel.cpp`
- Prior Plan References: `v22` panel completion, `v25` release-path closure for unfinished visible surfaces
- Scope: shell chrome, header treatment, empty/loading/error styling, placeholder-shell replacement
- Out of Scope: full feature implementation for history or PDF behavior
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/TerminalPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/OutputPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/HistoryPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/PDFViewerPanel.cpp`
- Related Screens / Components / Surfaces: terminal, output, history, PDF viewer
- Current UI Behavior / Appearance: these panels vary significantly in maturity and can expose obvious visual debt.
- Intended UI Behavior / Appearance: even lower-frequency panels feel cohesive, premium, and intentionally designed.
- Visual / Interaction Design Direction: strong shell continuity with subdued specialized chrome.
- Technical Styling Approach: apply shared panel shell, header, empty-state, and feedback patterns before considering deeper feature polish.
- Implementation Steps:
  1. Replace stub-looking or placeholder shell treatments.
  2. Normalize headers and content host styling.
  3. Add polished empty/loading/error states where necessary.
  4. Validate consistency with explorer, output, and notebook surfaces.
- Validation Steps:
  1. Open each panel standalone and in multi-panel layouts.
  2. Verify no visible panel looks like a test harness or placeholder.
  3. Review theme parity and dense-shell readability.
- Acceptance Criteria: secondary panels no longer break the product’s premium visual illusion.
- Dependencies: V26-P05-T01
- Parallelization Notes: can run after shared panel shell standards are defined.
- Risks / Failure Modes: styling around a stub can mask deeper product gaps; keep scope visibly honest but polished.
- Accessibility / Readability Notes: empty and error states must remain readable and clear.
- Theme / Styling Notes: keep specialized hosts visually integrated with shell depth rules.
- Motion / Interaction Notes where relevant: transition polish can be added later once shells are stable.
- Cleanup / Consolidation Notes where relevant: remove placeholder copy and stand-in visual fragments.
- Rollback / Safety Notes: avoid implying unsupported functionality through over-finished visuals.
- References / Context: the goal is polished honesty, not deceptive completeness.
- Example scenarios where useful: opening the PDF viewer should feel like a coherent product surface even if deeper PDF features remain limited.
