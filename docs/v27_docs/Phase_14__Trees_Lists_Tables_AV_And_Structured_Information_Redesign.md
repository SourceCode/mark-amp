# Phase 14 - Trees Lists Tables AV And Structured Information Redesign

## Goal

Redesign every structured information surface so trees, lists, tables, and AV views feel cleaner, denser, and more premium without losing speed.

## Definition Of Done

- structured rows and tables feel newly designed
- hierarchy, badges, metadata, and inline actions are more legible and more elegant
- dense information no longer looks like a weak visual class

## Tasks

### Phase ID: V27-P14
### Task ID: V27-P14-T01
### Task Title: Redesign Structured Rows For Trees Lists Search Results And Similar Dense Views
- Priority: P0
- Category: Tree / List / Table UI
- Objective: reauthor dense row systems for better hierarchy, clearer icon use, and more premium scanning behavior.
- Why This Matters Now: dense structured rows are a daily-use surface and one of the easiest places for visual inconsistency to survive.
- Visual Gap Statement: many structured rows still rely on inherited list patterns, mixed icon assumptions, and uneven metadata styling.
- User Experience Impact: redesigned rows improve scan speed, reduce fatigue, and increase perceived quality in core workflows.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/ExplorerPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/SearchSidebarPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/ProblemsPanel.cpp`
- Prior Plan References: `v22` Phase 15, `v26` Phase 12
- Scope: row height, indentation, hierarchy indicators, icon placement, metadata labels, badges, inline actions, selection and hover states
- Out of Scope: tree/list data logic
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/ExplorerPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/SearchSidebarPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/ProblemsPanel.cpp`
- Related Screens / Components / Surfaces: file trees, search lists, problems lists, similar dense row surfaces
- Current Visual Behavior / Appearance: structured rows work, but they still feel more tuned than fully redesigned and can expose mixed icon and metadata treatment.
- Intended Visual Behavior / Appearance: structured rows feel cleaner, more premium, and more consistent across all dense surfaces.
- Icon / Visual Design Direction: disciplined row design with canonical icons, subtle badges, and stronger hierarchy.
- Technical Styling Approach: redesign dense-row primitives and propagate them across list-like surfaces.
- Implementation Steps:
  1. Redesign row geometry, indentation, and icon placement.
  2. Rebuild metadata and badge styling.
  3. Align selection, hover, and inline-action states.
  4. Validate narrow and wide dense surfaces.
- Validation Steps:
  1. Compare populated explorer, search, and problems rows.
  2. Check scan speed and visual calm in high-density data sets.
  3. Verify icon and metadata consistency across surface types.
- Acceptance Criteria: dense rows feel clearly redesigned and no longer reveal mixed visual systems.
- Dependencies: Phase 01 through Phase 03, Phase 11
- Parallelization Notes: can run while AV redesign proceeds if row primitives are agreed first.
- Risks / Failure Modes: too much visual richness can hurt density and scanning speed.
- Accessibility / Readability Notes: selection, focus, and severity cues must remain obvious even in compact rows.
- Theme / Styling Notes: row states and metadata tiers must remain subtle but readable in both themes.
- Motion / Interaction Notes where relevant: hover and selection responses later should remain understated.
- Cleanup / Consolidation Notes where relevant: retire list-specific row variants that no longer fit the redesigned system.
- Rollback / Safety Notes: preserve row density where it materially affects workflow efficiency.
- References / Context: dense row quality is a daily-use quality test for the redesign.
- Example scenarios where useful: a search result row should feel more premium and readable without becoming taller and less efficient.

### Phase ID: V27-P14
### Task ID: V27-P14-T02
### Task Title: Redesign Tables And AV Surfaces For Premium Data Readability
- Priority: P1
- Category: Tree / List / Table UI
- Objective: bring AV tables, gallery-like data surfaces, and tabular layouts up to the new premium design standard.
- Why This Matters Now: data-heavy interfaces often remain the weakest-looking surfaces after shell redesign.
- Visual Gap Statement: AV surfaces are more likely to preserve thin or utility-style presentation than core shell surfaces.
- User Experience Impact: better structured data UI increases trust in advanced workflows and makes the product feel more complete.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/av/AVTablePanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/av/AVGalleryPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/av/AVKanbanPanel.cpp`
- Prior Plan References: `v22` AV/data phase, `v26` Phase 12
- Scope: table headers, grid/divider language, row density, card density, inline actions, badges, empty states
- Out of Scope: AV engine or data-model work
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/av/AVTablePanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/av/AVGalleryPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/av/AVKanbanPanel.cpp`
- Related Screens / Components / Surfaces: AV table, AV gallery, AV kanban, dense structured views
- Current Visual Behavior / Appearance: AV surfaces likely feel less mature and less premium than the redesigned shell target requires.
- Intended Visual Behavior / Appearance: AV surfaces feel like polished high-end productivity interfaces, not internal tooling.
- Icon / Visual Design Direction: elegant data surfaces with strong hierarchy, restrained chrome, and controlled metadata emphasis.
- Technical Styling Approach: redesign headers, rows, cards, and state roles for structured data using the new system instead of patching local visuals.
- Implementation Steps:
  1. Audit AV visual classes and structural hierarchy.
  2. Redesign table header, row, and card systems.
  3. Align action icons, chips, badges, and metadata tiers.
  4. Validate populated, sparse, and empty datasets.
- Validation Steps:
  1. Compare table, gallery, and kanban variants.
  2. Check density, selection, and action discoverability.
  3. Verify parity with redesigned panels and settings controls.
- Acceptance Criteria: structured data surfaces feel clearly redesigned and consistent with the rest of the app.
- Dependencies: V27-P14-T01, Phase 12
- Parallelization Notes: can run once row and control redesign patterns are stable.
- Risks / Failure Modes: over-framing data views can reduce scan efficiency.
- Accessibility / Readability Notes: grid lines, selection, and focus treatment must remain usable and not rely solely on color.
- Theme / Styling Notes: tables and cards must remain controlled and low-noise in both themes.
- Motion / Interaction Notes where relevant: row/card interaction later should remain subtle and consistent.
- Cleanup / Consolidation Notes where relevant: unify AV visual variants where separate local patterns are no longer necessary.
- Rollback / Safety Notes: preserve editing affordances and data density while redesigning visuals.
- References / Context: AV redesign prevents advanced workflows from feeling like a different product.
- Example scenarios where useful: an AV table with many columns should feel premium and organized, not cramped or generic.

### Phase ID: V27-P14
### Task ID: V27-P14-T03
### Task Title: Redesign Structured Empty Loading And Error States To Match The New Dense Surface Language
- Priority: P1
- Category: Tree / List / Table UI
- Objective: ensure structured data surfaces feel designed and premium even when they are sparse, loading, or erroring.
- Why This Matters Now: empty and loading states are where dense surfaces often reveal generic or neglected visual treatment.
- Visual Gap Statement: list and table edge states likely still vary by surface and often remain less mature than steady-state populated views.
- User Experience Impact: better edge states improve trust and reduce the sense of unfinished or brittle UI.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/ExplorerPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/SearchSidebarPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/av/AVTablePanel.cpp`
- Prior Plan References: `v22` empty-state work, `v26` Phase 12 and Phase 14
- Scope: compact empty states, loading placeholders, error cards, retry affordances, contextual guidance for structured surfaces
- Out of Scope: backend loading/retry logic
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/ExplorerPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/SearchSidebarPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/av/AVTablePanel.cpp`
- Related Screens / Components / Surfaces: empty search, empty explorer sections, empty AV tables, structured error and loading states
- Current Visual Behavior / Appearance: these states are likely thin, inconsistent, or locally improvised.
- Intended Visual Behavior / Appearance: edge states feel designed, restrained, and fully integrated with the redesigned structured-surface system.
- Icon / Visual Design Direction: compact premium guidance with canonical iconography and calm hierarchy.
- Technical Styling Approach: design compact structured-state templates that reuse the redesigned feedback and onboarding systems.
- Implementation Steps:
  1. Inventory structured-surface edge-state patterns.
  2. Redesign compact empty/loading/error templates.
  3. Apply them consistently across structured surfaces.
  4. Validate narrow and wide presentation.
- Validation Steps:
  1. Trigger empty, loading, and no-results states across key structured surfaces.
  2. Compare readability and spacing in narrow sidebars and wide panels.
  3. Confirm edge states feel related to the new onboarding and feedback language.
- Acceptance Criteria: structured edge states no longer feel generic or visually disconnected from the redesign.
- Dependencies: V27-P14-T01, Phase 15, Phase 16
- Parallelization Notes: can proceed once the dense-row and feedback/onboarding systems are defined.
- Risks / Failure Modes: too much illustration or copy can make professional surfaces feel consumerish.
- Accessibility / Readability Notes: keep messages concise, readable, and action affordances explicit.
- Theme / Styling Notes: edge-state contrast must remain calm but visible.
- Motion / Interaction Notes where relevant: loading motion should later remain subtle and reduced-motion compatible.
- Cleanup / Consolidation Notes where relevant: retire local empty-state fragments used only because the old system lacked templates.
- Rollback / Safety Notes: preserve context-specific clarity while unifying presentation.
- References / Context: edge-state polish is part of making dense surfaces feel truly redesigned.
- Example scenarios where useful: a search with zero results should still look like a designed product state, not a blank list.
