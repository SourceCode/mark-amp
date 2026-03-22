# Phase 15: Trees Lists Tables AV And Data Dense Presentation

## Outcome

Create one refined row, list, tree, and data-display language so dense information surfaces feel highly legible, premium, and product-wide consistent.

## Improvement Count

360 atomic improvements across 6 execution tasks.

### P15-T01

- Phase ID: P15
- Task ID: P15-T01
- Task Title: Define a unified dense-row system for trees, lists, and result rows
- Priority: P0
- Category: Tree / List / Table UI
- Atomic Improvements Covered: 60
- Objective: Make every row-based surface share one disciplined visual system for height, indentation, metadata, and actions.
- Why This Matters Now: Row inconsistency is one of the biggest remaining visual quality gaps across the app.
- UI Problem Statement: File trees, search rows, recent items, panel lists, and other dense surfaces all use slightly different row treatments.
- User Experience Impact: Dense data surfaces feel more fragmented and less expertly tuned.
- Scope: Row height families, indentation, icon slots, metadata lanes, badges, inline actions, selection states, current-row emphasis, hover rhythm.
- Out of Scope: Underlying tree/list behavior.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/FileTreeCtrl.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/SearchResultsTree.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/RecentItemsList.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/TreeViewItem.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/LayoutMetrics.h`
- Related Screens / Components / Surfaces: Trees, lists, recent items, result rows, explorer, search, panel rows.
- Current UI Behavior / Appearance: Row density and metadata placement vary substantially between surfaces.
- Intended UI Behavior / Appearance: Dense rows feel product-wide related while preserving role-specific nuance.
- Visual / Interaction Design Direction: Compact, highly legible row system with precise rhythm and metadata restraint.
- Technical Styling Approach: Create shared row tokens and row-variant patterns for tree, list, and grouped-result surfaces.
- Implementation Steps: Audit row families; define row variants; normalize icon/meta/action placement; migrate the highest-traffic surfaces first; align selection and hover states.
- Validation Steps: Compare row families side by side across explorer, search, recents, and panel lists.
- Acceptance Criteria: Dense rows no longer reveal subsystem-local styling.
- Dependencies: Phase 01, Phase 03.
- Risks / Failure Modes: One row system may be too generic unless it preserves a small number of role-specific variants.
- Accessibility / Readability Notes: Selection, indentation, and active-item cues must remain strong for keyboard and low-vision users.
- Theme / Styling Notes: Dense rows need careful contrast control to avoid muddy hover or selection states.
- Motion / Interaction Notes where relevant: Hover and expand/collapse states should remain subtle and stable.
- Observability / Diagnostics Notes where relevant: Add row-family comparison boards to the UI atlas.
- Rollback / Safety Notes: Keep semantics stable while row layout and styling are migrated incrementally.
- References / Context: Row quality is one of the biggest opportunities to make the product feel systematized.
- Example scenarios where useful: A file-tree row and a search-result row feel clearly related even though they serve different jobs.

### P15-T02

- Phase ID: P15
- Task ID: P15-T02
- Task Title: Improve list and tree selection, hover, expansion, and metadata affordances
- Priority: P1
- Category: Tree / List / Table UI
- Atomic Improvements Covered: 60
- Objective: Make row interactions clearer, more elegant, and more reliable visually.
- Why This Matters Now: Selection and expansion are constant interactions in information-dense UIs.
- UI Problem Statement: Selection states, expansion controls, and row metadata cues vary in weight and clarity between trees and lists.
- User Experience Impact: Users spend more effort parsing row state and hierarchy than they should.
- Scope: Selection fill, current row, expansion chevrons, branch guides, badges, muted metadata, row action reveal, drag markers.
- Out of Scope: Tree expansion logic or drag-drop semantics.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/FileTreeCtrl.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/SearchResultsTree.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/TreeViewItem.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/ProblemsTreeModel.cpp`
- Related Screens / Components / Surfaces: Explorer, search results, problems trees, grouped lists.
- Current UI Behavior / Appearance: States and metadata are visible, but not yet tuned into one premium information language.
- Intended UI Behavior / Appearance: Users can identify structure, selection, and current focus more quickly and more comfortably.
- Visual / Interaction Design Direction: Dense hierarchy with strong but quiet state cues.
- Technical Styling Approach: Extend row variants with state and hierarchy subcomponents for guides, chevrons, and metadata.
- Implementation Steps: Audit row states; refine expansion visuals; align metadata opacity and badges; improve row-action reveal patterns; retune selected/current distinctions.
- Validation Steps: Exercise grouped and nested rows with keyboard and pointer interactions.
- Acceptance Criteria: Dense row interactions feel less noisy and more trustworthy.
- Dependencies: P15-T01, Phase 17.
- Risks / Failure Modes: Overly subtle row-action reveal can hide important controls.
- Accessibility / Readability Notes: Expansion and current-row cues must not depend on color alone.
- Theme / Styling Notes: Tree guides and chevrons need enough contrast without becoming visual clutter.
- Motion / Interaction Notes where relevant: Expand/collapse and action-reveal motion should feel smooth and minimal.
- Observability / Diagnostics Notes where relevant: Add expanded/nested row-state captures.
- Rollback / Safety Notes: Preserve hit targets and interaction timings while visual tuning proceeds.
- References / Context: Dense row interactions are a defining element of professional IDE UX.
- Example scenarios where useful: A nested search result tree is easy to navigate without a forest of noisy separators and heavy badges.

### P15-T03

- Phase ID: P15
- Task ID: P15-T03
- Task Title: Redesign AV and table-oriented surfaces for premium data density and control alignment
- Priority: P0
- Category: Tree / List / Table UI
- Atomic Improvements Covered: 60
- Objective: Make table and database-style views feel serious, elegant, and intentionally structured.
- Why This Matters Now: Data-dense views are currently some of the visually thinnest surfaces in the repo.
- UI Problem Statement: [AVTablePanel.cpp](/Users/ryanrentfro/code/markamp/src/ui/av/AVTablePanel.cpp) is extremely thin, and related AV panels and renderers need a stronger unified data-view language.
- User Experience Impact: Database and structured content workflows feel less product-grade than the shell around them.
- Scope: Table rows, headers, cell padding, sort/filter affordances, gallery cards, kanban card chrome, relation editors, aggregate rows, empty/loading states.
- Out of Scope: Database model semantics.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/av/AVTablePanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/av/AVGalleryPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/av/AVKanbanPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/av/AVHeaderRenderer.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/av/AVCellRenderer.cpp`
- Related Screens / Components / Surfaces: AV table, gallery, kanban, relation editors, data headers.
- Current UI Behavior / Appearance: AV surfaces exist but are not yet visually realized to match the product ambition.
- Intended UI Behavior / Appearance: Data views become elegant, powerful, and visually coherent with the rest of the workbench.
- Visual / Interaction Design Direction: Premium structured-data surfaces with crisp density and strong hierarchy.
- Technical Styling Approach: Build a data-view design language for headers, rows, cells, cards, and inline editors, tied to shared control and row systems.
- Implementation Steps: Audit AV and table surfaces; define data-view tokens; redesign headers, rows, cards, and inline editors; align empty and loading states; refine selection and filter controls.
- Validation Steps: Review table, gallery, and kanban views with dense data and sparse states.
- Acceptance Criteria: Data views no longer feel placeholder-backed or under-designed.
- Dependencies: Phase 01, Phase 13, Phase 14.
- Risks / Failure Modes: Over-designing data surfaces can reduce working density or obscure cell content.
- Accessibility / Readability Notes: Header contrast, row focus, and cell edit states must remain very clear.
- Theme / Styling Notes: Table gridlines and card layering need careful contrast tuning.
- Motion / Interaction Notes where relevant: Sort, filter, and card-hover states should remain subtle.
- Observability / Diagnostics Notes where relevant: Add AV/table/gallery/kanban screenshot boards.
- Rollback / Safety Notes: Preserve AV interaction semantics while visual design catches up.
- References / Context: AV surfaces are one of the largest visual opportunities in the product because they are currently under-realized.
- Example scenarios where useful: A dense database table feels as premium and confident as the editor rather than like a stub.

### P15-T04

- Phase ID: P15
- Task ID: P15-T04
- Task Title: Improve table overlays, filter bars, sort controls, and data-specific transient UI
- Priority: P2
- Category: Tree / List / Table UI
- Atomic Improvements Covered: 60
- Objective: Make data-specific controls feel as polished as the views they support.
- Why This Matters Now: Data tools lose credibility quickly when their filter and overlay surfaces look generic.
- UI Problem Statement: [TableEditorOverlay.cpp](/Users/ryanrentfro/code/markamp/src/ui/TableEditorOverlay.cpp), [AVFilterBar.cpp](/Users/ryanrentfro/code/markamp/src/ui/av/AVFilterBar.cpp), and related editors need stronger visual refinement and alignment.
- User Experience Impact: Filtering, editing, and sorting in structured views feel less fluid and less premium.
- Scope: Filter chips, sort dropdowns, overlay editors, inline cell actions, relation pickers, rollup config dialogs, metadata affordances.
- Out of Scope: Filter and formula logic.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/TableEditorOverlay.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/av/AVFilterBar.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/av/AVRelationEditor.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/av/AVRollupConfigDialog.cpp`
- Related Screens / Components / Surfaces: Data overlays, filter bars, sort controls, relation editors, config dialogs.
- Current UI Behavior / Appearance: Data-specific controls are present but do not yet feel like part of a premium structured-data toolkit.
- Intended UI Behavior / Appearance: Data interactions are supported by crisp, elegant micro-surfaces.
- Visual / Interaction Design Direction: Compact expert controls with strong alignment and minimal visual clutter.
- Technical Styling Approach: Reuse filter-chip, dropdown, dialog, and row systems while adding data-tool-specific variants.
- Implementation Steps: Audit filter and overlay surfaces; align shell and row styling; improve chip and dropdown treatment; refine config-dialog hierarchy; unify inline editor shells.
- Validation Steps: Use filter, relation, and rollup configuration surfaces in dense data scenarios.
- Acceptance Criteria: Data-specific transient controls feel product-grade rather than support-tool-grade.
- Dependencies: Phase 14, Phase 15 core work.
- Risks / Failure Modes: Tight controls can become cramped if spacing is over-optimized.
- Accessibility / Readability Notes: Filter and sort controls need strong focus and active-state visibility.
- Theme / Styling Notes: Chips and dropdowns need excellent contrast across panel backgrounds.
- Motion / Interaction Notes where relevant: Overlay open and chip-toggle states should feel precise and low-noise.
- Observability / Diagnostics Notes where relevant: Add filter and overlay state screenshots to the data-view gallery.
- Rollback / Safety Notes: Preserve data-edit semantics and keyboard flows while visual refinement is applied.
- References / Context: Premium data experiences are often won or lost in the control layer above the rows.
- Example scenarios where useful: Filtering a gallery or table feels elegant and lightweight instead of mechanically bolted on.

### P15-T05

- Phase ID: P15
- Task ID: P15-T05
- Task Title: Normalize recent-items, search-result, extension-list, and task-list card/list hybrids
- Priority: P2
- Category: Tree / List / Table UI
- Atomic Improvements Covered: 60
- Objective: Harmonize hybrid list/card surfaces that sit between simple rows and full data tables.
- Why This Matters Now: The app contains many mixed-density surfaces whose quality varies substantially.
- UI Problem Statement: [RecentItemsList.cpp](/Users/ryanrentfro/code/markamp/src/ui/RecentItemsList.cpp), [ExtensionsBrowserPanel.cpp](/Users/ryanrentfro/code/markamp/src/ui/ExtensionsBrowserPanel.cpp), [TaskListPanel.cpp](/Users/ryanrentfro/code/markamp/src/ui/TaskListPanel.cpp), and similar surfaces need stronger consistency.
- User Experience Impact: Semi-structured lists feel product-wide inconsistent and visually weaker than core surfaces.
- Scope: Hybrid rows/cards, metadata chips, avatars/icons, secondary actions, compact cards, empty states, loading skeletons.
- Out of Scope: Data sourcing and sorting logic.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/RecentItemsList.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/ExtensionsBrowserPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/TaskListPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/ExtensionDetailPanel.cpp`
- Related Screens / Components / Surfaces: Recent items, extensions browser, task lists, hybrid cards/lists.
- Current UI Behavior / Appearance: These surfaces vary in spacing, metadata, icon treatment, and row/card boundaries.
- Intended UI Behavior / Appearance: Hybrid information surfaces feel part of one polished card/list design family.
- Visual / Interaction Design Direction: Structured hybrid surfaces with clear hierarchy and restrained card framing.
- Technical Styling Approach: Define card-list hybrids as a bridge family between rows and full cards, reusing metadata and chip systems.
- Implementation Steps: Audit hybrid surfaces; align spacing and metadata hierarchy; normalize card shells and action placement; improve empty/loading variants.
- Validation Steps: Review recent items, extensions, and task lists under sparse and dense content conditions.
- Acceptance Criteria: Hybrid list/card surfaces no longer feel visually isolated from the rest of the product.
- Dependencies: Phase 01, Phase 14, Phase 16.
- Risks / Failure Modes: Over-carding hybrid surfaces can add unnecessary visual weight.
- Accessibility / Readability Notes: Metadata and action placement must remain clear for keyboard and screen magnification users.
- Theme / Styling Notes: Card shells and row separators need careful contrast tuning.
- Motion / Interaction Notes where relevant: Hover and reveal states should remain subtle.
- Observability / Diagnostics Notes where relevant: Add hybrid-surface galleries to the data-view family board.
- Rollback / Safety Notes: Preserve current interaction density while visual structure is improved.
- References / Context: Many secondary product surfaces depend on high-quality hybrid list/card treatment.
- Example scenarios where useful: Extensions browsing feels visually in-family with task lists and recent items instead of using unrelated card logic.

### P15-T06

- Phase ID: P15
- Task ID: P15-T06
- Task Title: Add dense-information surface galleries and row-system regression checks
- Priority: P0
- Category: Cleanup / Consolidation
- Atomic Improvements Covered: 60
- Objective: Protect row, tree, list, table, and hybrid-surface polish with explicit comparative references.
- Why This Matters Now: Dense information surfaces span huge parts of the app and are the most likely place for slow drift.
- UI Problem Statement: There is no one comparison board for rows, trees, tables, AV views, and hybrid data surfaces.
- User Experience Impact: The product can quietly lose visual discipline in the very surfaces users scan most heavily.
- Scope: Explorer rows, search rows, recent items, AV rows/cards, extension lists, task lists, dense panel rows, empty/loading states.
- Out of Scope: Backend correctness of represented data.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/tests/unit`; `/Users/ryanrentfro/code/markamp/docs/v22_docs`; `/Users/ryanrentfro/code/markamp/src/ui/FileTreeCtrl.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/av/AVTablePanel.cpp`
- Related Screens / Components / Surfaces: Entire dense-information family.
- Current UI Behavior / Appearance: Dense-information quality is currently reviewed surface by surface rather than as one ecosystem.
- Intended UI Behavior / Appearance: The team can compare and protect all dense data surfaces under one visual governance model.
- Visual / Interaction Design Direction: Comparative dense-surface quality management.
- Technical Styling Approach: Build fixture boards and screenshot comparisons for row families, table families, and hybrid list/card surfaces.
- Implementation Steps: Define dense-data fixtures; capture representative states; publish galleries; add visual drift checks for key row and data-view families.
- Validation Steps: Intentionally perturb row height or metadata contrast and verify the regression suite flags the issue.
- Acceptance Criteria: Dense-information surface quality becomes a managed, durable product asset.
- Dependencies: P15-T01 through P15-T05.
- Risks / Failure Modes: The board can become too broad unless grouped by family and density profile.
- Accessibility / Readability Notes: Include compact density, focus states, and high-contrast captures.
- Theme / Styling Notes: Capture all dense-data families in dark, light, and high-contrast modes.
- Motion / Interaction Notes where relevant: Include expand/collapse and row-hover states where they affect appearance materially.
- Observability / Diagnostics Notes where relevant: Store fixture metadata with family, density, and state tags.
- Rollback / Safety Notes: Keep the galleries additive while dense-surface redesign is still in progress.
- References / Context: Row and data-view systems are fundamental to whether the product feels unified.
- Example scenarios where useful: A future tweak to Explorer row density can be checked immediately against AV and search row systems.

