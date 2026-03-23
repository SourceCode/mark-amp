# Phase 12 - Trees Lists Tables AV And Structured Data Presentation

## Goal

Make structured data surfaces feel clean, dense, and premium without sacrificing speed or readability.

## Definition Of Done

- trees, lists, tables, and attribute-view surfaces use shared row, metadata, and state patterns
- structured data remains highly scannable at dense sizes
- empty and loading variants feel product-grade rather than utility-grade

## Tasks

### Phase ID: V26-P12
### Task ID: V26-P12-T01
### Task Title: Standardize Tree And List Row Metrics, Hierarchy Indicators, And Inline Actions
- Priority: P0
- Category: Tree / List / Table UI
- Objective: Create one premium structured-row system for file trees, search lists, outline lists, and similar dense surfaces.
- Why This Matters Now: Tree/list inconsistency is one of the most visible remaining quality gaps in IDE-style products.
- UI Gap Statement: Dense rows exist across many panels, but they likely differ in height, indentation, metadata balance, and inline-action behavior.
- User Experience Impact: Better row design increases scan speed and makes complex navigation feel more professional.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/ExplorerPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/SearchSidebarPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/ProblemsPanel.cpp`
- Prior Plan References: `v22` list/tree polish, `v25` panel closure
- Scope: row height, indentation, expansion affordances, inline actions, selection and hover states, badges
- Out of Scope: tree data behavior
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/ExplorerPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/SearchSidebarPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/ProblemsPanel.cpp`
- Related Screens / Components / Surfaces: file tree, outline, search results, problems lists
- Current UI Behavior / Appearance: row systems exist but likely vary by surface and maturity.
- Intended UI Behavior / Appearance: every dense row surface shares one clear hierarchy and action language.
- Visual / Interaction Design Direction: compact professional list design with crisp indentation and soft metadata emphasis.
- Technical Styling Approach: define reusable structured-row roles and apply them across list-like panels.
- Implementation Steps:
  1. Inventory row variants across tree/list surfaces.
  2. Normalize row metrics, hierarchy indicators, and inline-action patterns.
  3. Align badges and metadata styling with shared dense-surface rules.
  4. Validate mixed-depth trees and crowded lists.
- Validation Steps:
  1. Compare explorer, search, and problems rows with realistic data.
  2. Verify keyboard selection and hover readability.
  3. Check narrow sidebar behavior.
- Acceptance Criteria: dense tree/list rows feel visually related and support fast scanning.
- Dependencies: V26-P05-T01, V26-P05-T02
- Parallelization Notes: can run alongside AV table work.
- Risks / Failure Modes: too much normalization can erase useful domain-specific cues.
- Accessibility / Readability Notes: focus and selection must remain clearly visible.
- Theme / Styling Notes: low-contrast metadata must remain readable in both themes.
- Motion / Interaction Notes where relevant: hover states should remain minimal in dense lists.
- Cleanup / Consolidation Notes where relevant: retire duplicated dense-row styling logic.
- Rollback / Safety Notes: keep list interaction performance and hit targets intact.
- References / Context: this task is central to overall IDE polish.
- Example scenarios where useful: a deep file tree should remain readable without feeling visually jagged.

### Phase ID: V26-P12
### Task ID: V26-P12-T02
### Task Title: Bring Attribute View Tables And Structured Data Panels To Premium Clarity
- Priority: P1
- Category: Tree / List / Table UI
- Objective: Raise AV tables and structured data surfaces to the same visual standard as the rest of the shell.
- Why This Matters Now: Structured data views often expose the most obvious visual debt if they remain thin or utility-like.
- UI Gap Statement: `/Users/ryanrentfro/code/markamp/src/ui/av/AVTablePanel.cpp` appears visually thin and likely lacks refined shell and row presentation.
- User Experience Impact: Premium data views increase trust in advanced workflows and reduce visual fatigue.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/av/AVTablePanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/av/AVGalleryPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/av/AVKanbanPanel.cpp`
- Prior Plan References: `v22` AV UI planning, `v25` release readiness for visible surfaces
- Scope: table headers, row density, grid lines, empty states, badges, inline actions, supporting metadata
- Out of Scope: AV engine or data-model work
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/av/AVTablePanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/av/AVGalleryPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/av/AVKanbanPanel.cpp`
- Related Screens / Components / Surfaces: attribute view table, gallery, kanban, property-rich data surfaces
- Current UI Behavior / Appearance: AV UI likely functions but may not yet visually match the premium shell or editor surfaces.
- Intended UI Behavior / Appearance: AV surfaces are crisp, modern, and highly readable even at dense information levels.
- Visual / Interaction Design Direction: premium productivity data UI with strong hierarchy and low-noise framing.
- Technical Styling Approach: build AV row/header/card styles on shared structured-surface rules rather than isolated styling.
- Implementation Steps:
  1. Audit AV table and related data-surface shells.
  2. Standardize header, row, and card density.
  3. Improve grid/divider treatment and metadata hierarchy.
  4. Validate empty and populated states across AV views.
- Validation Steps:
  1. Load representative AV datasets.
  2. Compare table, gallery, and kanban surfaces.
  3. Verify readability and theme correctness.
- Acceptance Criteria: AV surfaces no longer feel visually secondary or prototype-like.
- Dependencies: V26-P12-T01, V26-P15-T01
- Parallelization Notes: can proceed once shared structured-surface rules are defined.
- Risks / Failure Modes: too much chrome can make dense data views heavy.
- Accessibility / Readability Notes: grid and selection states must remain distinguishable without excessive contrast.
- Theme / Styling Notes: table/grid line intensity should be subtle and theme-safe.
- Motion / Interaction Notes where relevant: card and row interaction states should match structured-surface standards.
- Cleanup / Consolidation Notes where relevant: unify AV visual patterns across table, gallery, and kanban where appropriate.
- Rollback / Safety Notes: preserve editing and interaction semantics while changing presentation.
- References / Context: advanced structured data surfaces must not be the weak visual link.
- Example scenarios where useful: a filled AV table should feel as polished as a modern premium productivity app.

### Phase ID: V26-P12
### Task ID: V26-P12-T03
### Task Title: Define Structured Empty, Loading, And Error Patterns For Lists And Tables
- Priority: P1
- Category: Tree / List / Table UI
- Objective: Ensure structured information surfaces look designed even when sparse, loading, or erroring.
- Why This Matters Now: Utility-style empty states are a common polish failure in desktop productivity apps.
- UI Gap Statement: Dense surfaces often handle loaded states better than empty or error states, leading to visible inconsistency.
- User Experience Impact: Users perceive stronger quality and lower friction when every state is composed and understandable.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/ExplorerPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/SearchSidebarPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/av/AVTablePanel.cpp`
- Prior Plan References: `v22` empty-state work, `v25` release-path validation surfaces
- Scope: empty list/table states, loading placeholders, compact error messaging, retry affordance placement
- Out of Scope: backend loading/retry mechanics
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/ExplorerPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/SearchSidebarPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/av/AVTablePanel.cpp`
- Related Screens / Components / Surfaces: explorer empty sections, search no-results, AV empty datasets, structured error states
- Current UI Behavior / Appearance: these states likely vary widely and can still feel thin or temporary.
- Intended UI Behavior / Appearance: empty/loading/error states are elegant, compact, and consistent across structured surfaces.
- Visual / Interaction Design Direction: restrained instructional states that preserve productivity tone.
- Technical Styling Approach: create reusable structured-state templates linked to feedback and onboarding patterns.
- Implementation Steps:
  1. Inventory empty/loading/error variants across structured surfaces.
  2. Define compact templates and typography roles.
  3. Replace per-surface temporary empty states.
  4. Validate width changes and dense-shell fit.
- Validation Steps:
  1. Trigger empty and no-results states in core panels.
  2. Review readability in narrow sidebars and large panels.
  3. Confirm visual consistency with onboarding and feedback components.
- Acceptance Criteria: all major structured surfaces have compact, premium edge-state presentation.
- Dependencies: V26-P12-T01, V26-P14-T02, V26-P13-T01
- Parallelization Notes: can run after structured row standards are established.
- Risks / Failure Modes: too much empty-state illustration or copy can feel consumerish in a professional tool.
- Accessibility / Readability Notes: ensure messages remain readable and action affordances are clear.
- Theme / Styling Notes: empty-state contrast must remain calm but visible.
- Motion / Interaction Notes where relevant: loading indicators should be subtle and optional.
- Cleanup / Consolidation Notes where relevant: consolidate list/table edge-state components.
- Rollback / Safety Notes: do not hide important technical detail behind overly abstract empty-state messaging.
- References / Context: this is the structured-data counterpart to startup and notebook edge-state polish.
- Example scenarios where useful: a search sidebar with no results should still look polished and informative.
