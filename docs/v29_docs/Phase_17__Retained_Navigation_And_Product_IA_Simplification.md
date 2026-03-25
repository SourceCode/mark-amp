# Phase 17: Retained Navigation And Product IA Simplification

## Task Count

- 2 tasks

## Task V29-P17-T01

- Phase ID: V29-P17
- Task ID: V29-P17-T01
- Task Title: Re-cluster retained navigation and terminology after the four product lines are removed
- Priority: Medium
- Category: Navigation Simplification
- Objective: Update retained navigation, labels, and product terminology so the shell no longer reflects the old multi-product map.
- Why This Matters Now: Removing four large pillars leaves navigation hierarchy and terminology that may still overfit a broader product.
- Removal Gap Statement: The app previously had dedicated categories for Notebooks, Canvas, Flashcards, and Tasks; removing them changes the meaning of remaining sections.
- User / Product Impact: Makes the remaining app easier to scan and understand.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/ActivityBar.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/StatusBarPanel.cpp`
- Prior Plan References: `/Users/ryanrentfro/code/markamp/docs/v22_docs/Phase_03__Activity_Bar_Sidebars_Panel_Headers_And_Shell_Chrome.md`; `/Users/ryanrentfro/code/markamp/docs/v26_docs/Phase_04__Tabs_Breadcrumbs_Navigation_And_Wayfinding.md`
- Scope: Navigation labels, hierarchy, ordering, shell grouping, and terminology across retained surfaces.
- Out of Scope: Pure visual styling changes without a removal rationale.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/ActivityBar.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/StatusBarPanel.cpp`
- Related Features / Systems / Components: Navigation, terminology, labels.
- Current Behavior / Presence: The navigation model still reflects a much broader feature map.
- Intended Post-Removal Behavior: Navigation is centered on retained editor/document workflows and their supporting tools.
- Removal / Simplification Direction: Re-cluster retained views so they feel primary rather than what is left over.
- Technical Approach: Reorder and rename retained navigation after removed items are gone; simplify category boundaries and labels.
- Implementation Steps: Audit retained shell labels; reorder activity/sidebar items if needed; adjust status text and panel titles; update tests and docs.
- Validation Steps: Review navigation hierarchy in a fresh session and after restore.
- Acceptance Criteria: Retained navigation reads as intentional and no label implies removed sibling categories.
- Dependencies: Phase 04 and Phase 14.
- Parallelization Notes: Can run with startup simplification.
- Risks / Failure Modes: Over-aggressive relabeling may reduce familiarity for retained users.
- Migration / Compatibility Notes: Persisted panel labels are not expected to survive; only mode IDs matter.
- UX / Layout Cleanup Notes: Favor clarity and reduced category count over novelty.
- Cleanup / Consolidation Notes: Collapse duplicated labels that only existed to differentiate removed product families.
- Rollback / Safety Notes: Preserve stable access to retained major views.
- References / Context: Product simplification must be visible in IA, not only in code deletion.
- Example Scenarios: The remaining activity bar sequence no longer looks like several missing product modules were torn out.

## Task V29-P17-T02

- Phase ID: V29-P17
- Task ID: V29-P17-T02
- Task Title: Simplify shell composition, badges, and status messaging around the retained product only
- Priority: Medium
- Category: Release / Product Simplification
- Objective: Remove shell furniture and status semantics that only made sense in the broader multi-surface product.
- Why This Matters Now: Even after navigation cleanup, status indicators, badges, and shell microfurniture can still imply removed product breadth.
- Removal Gap Statement: Status-bar and sidebar-mode indicators currently encode retired modes, and prior visual plans assumed a denser shell.
- User / Product Impact: Reduces ambient complexity and reinforces the smaller product shape.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/StatusBarPanel.cpp:1423`; `/Users/ryanrentfro/code/markamp/src/ui/ActivityBar.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`
- Prior Plan References: `/Users/ryanrentfro/code/markamp/docs/v26_docs/Phase_02__Shell_Frame_Layout_Rhythm_And_Surface_Layering.md`; `/Users/ryanrentfro/code/markamp/docs/v26_docs/Phase_13__Feedback_Status_Progress_And_Inline_Validation_UI.md`
- Scope: Status-bar text, badges, panel titles, shell secondary labels, and cross-surface microfurniture.
- Out of Scope: Animation polish or purely aesthetic refinement.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/StatusBarPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/ActivityBar.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`
- Related Features / Systems / Components: Status bar, badges, panel headers, shell furniture.
- Current Behavior / Presence: Shell microfurniture still reflects retired sidebar modes and product families.
- Intended Post-Removal Behavior: Shell messaging only describes retained states and fewer surface categories.
- Removal / Simplification Direction: Reduce microfurniture that only served product breadth no longer present.
- Technical Approach: Remove retired labels and badge sources; simplify status mappings and headers.
- Implementation Steps: Remove retired status mappings; adjust badge models; simplify panel headers; update tests.
- Validation Steps: Exercise retained shell actions and inspect status/badge output for ghost references.
- Acceptance Criteria: No status or badge text references retired features and the shell reads as simpler.
- Dependencies: Phase 04 and Phase 13.
- Parallelization Notes: Small cleanup after major shell changes settle.
- Risks / Failure Modes: Badge routing may still assume removed panel IDs exist.
- Migration / Compatibility Notes: Old badge records should be ignored.
- UX / Layout Cleanup Notes: Use this pass to make the remaining shell calmer and less crowded.
- Cleanup / Consolidation Notes: Remove dead badge and status helpers.
- Rollback / Safety Notes: Keep essential retained status reporting intact.
- References / Context: This is the UX cohesion pass unlocked by feature retirement.
- Example Scenarios: Status bar never displays `Canvas` or `Notebooks` as current sidebar mode.
