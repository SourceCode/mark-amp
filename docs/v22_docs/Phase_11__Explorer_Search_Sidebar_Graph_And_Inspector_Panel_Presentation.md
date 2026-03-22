# Phase 11: Explorer Search Sidebar Graph And Inspector Panel Presentation

## Outcome

Bring sidebar and inspector-oriented workbench panels to one premium visual standard with stronger hierarchy, density discipline, metadata treatment, and panel-specific polish.

## Improvement Count

360 atomic improvements across 6 execution tasks.

### P11-T01

- Phase ID: P11
- Task ID: P11-T01
- Task Title: Redesign Explorer composition, section hierarchy, and file-tree framing
- Priority: P0
- Category: Panel UI
- Atomic Improvements Covered: 60
- Objective: Make Explorer feel like a flagship navigation panel rather than a dense utility region.
- Why This Matters Now: Explorer is one of the most used side panels and currently mixes real content with visually underdeveloped sections.
- UI Problem Statement: [ExplorerPanel.cpp](/Users/ryanrentfro/code/markamp/src/ui/ExplorerPanel.cpp), [ExplorerToolbar.cpp](/Users/ryanrentfro/code/markamp/src/ui/ExplorerToolbar.cpp), [OpenEditorsSection.cpp](/Users/ryanrentfro/code/markamp/src/ui/OpenEditorsSection.cpp), and [FileTreeCtrl.cpp](/Users/ryanrentfro/code/markamp/src/ui/FileTreeCtrl.cpp) need a more coherent visual hierarchy.
- User Experience Impact: File navigation works, but it does not yet feel elite, calm, or especially legible.
- Scope: Explorer header, section rhythm, open editors presentation, workspace root framing, toolbar styling, empty-state treatment.
- Out of Scope: File-system logic and creation workflows.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/ExplorerPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/ExplorerToolbar.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/OpenEditorsSection.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/FileTreeCtrl.cpp`
- Related Screens / Components / Surfaces: Explorer, open editors, file tree, explorer toolbar.
- Current UI Behavior / Appearance: Explorer is functional but visually uneven, with section composition that still feels utility-first.
- Intended UI Behavior / Appearance: Explorer becomes a calm, premium navigation instrument with excellent hierarchy and metadata rhythm.
- Visual / Interaction Design Direction: Dense but spacious navigation panel with confident tree framing and quiet chrome.
- Technical Styling Approach: Apply sidebar-shell standards plus a dedicated explorer row and section language.
- Implementation Steps: Audit explorer subsections; retune section spacing and typography; align toolbar and open editors; improve file tree framing and empty states.
- Validation Steps: Compare Explorer with empty, small, and large workspace states across themes.
- Acceptance Criteria: Explorer feels materially more premium and easier to scan.
- Dependencies: Phase 03, Phase 15.
- Risks / Failure Modes: Too much visual breathing room can hurt dense-project efficiency.
- Accessibility / Readability Notes: Root sections, active file cues, and selection states must remain easy to identify.
- Theme / Styling Notes: Tree layering and metadata contrast must be carefully tuned in all themes.
- Motion / Interaction Notes where relevant: Expand/collapse and reveal states should remain subtle.
- Observability / Diagnostics Notes where relevant: Add Explorer state screenshots to the panel family gallery.
- Rollback / Safety Notes: Keep file tree mechanics stable while composition and styling are updated.
- References / Context: Explorer is one of the biggest opportunities for a visible day-to-day polish improvement.
- Example scenarios where useful: A large workspace with nested folders still feels calm and readable inside Explorer.

### P11-T02

- Phase ID: P11
- Task ID: P11-T02
- Task Title: Improve sidebar search, outline, backlinks, graph, and metadata-heavy panel hierarchy
- Priority: P1
- Category: Panel UI
- Atomic Improvements Covered: 60
- Objective: Give non-Explorer sidebars the same visual maturity and hierarchy discipline as the primary navigation surfaces.
- Why This Matters Now: Search, outline, backlinks, graph, and metadata sidebars still vary too much in density and visual clarity.
- UI Problem Statement: [SearchSidebarPanel.cpp](/Users/ryanrentfro/code/markamp/src/ui/SearchSidebarPanel.cpp), [OutlinePanel.cpp](/Users/ryanrentfro/code/markamp/src/ui/OutlinePanel.cpp), [BacklinksPanel.cpp](/Users/ryanrentfro/code/markamp/src/ui/BacklinksPanel.cpp), [GraphSidebarPanel.cpp](/Users/ryanrentfro/code/markamp/src/ui/GraphSidebarPanel.cpp), and related inspector-style panels need a stronger common design language.
- User Experience Impact: Sidebars beyond Explorer feel more fragmented and lower-finish.
- Scope: Header/title treatment, row hierarchy, graph metadata blocks, backlinks row design, outline indentation, count chips, filters, sparse panel states.
- Out of Scope: Search or graph engine correctness.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/SearchSidebarPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/OutlinePanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/BacklinksPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/GraphSidebarPanel.cpp`
- Related Screens / Components / Surfaces: Search sidebar, outline, backlinks, graph sidebars, inspector-ish side panels.
- Current UI Behavior / Appearance: These panels work or partially work, but their visual structure is not uniformly premium.
- Intended UI Behavior / Appearance: Dense informational sidebars feel consistent and highly legible.
- Visual / Interaction Design Direction: Information-rich sidecars with precise hierarchy and low-noise metadata.
- Technical Styling Approach: Extend list/tree metadata rules and panel-shell variants to these specialized sidebars.
- Implementation Steps: Audit each sidebar surface; define shared row/meta patterns; improve title and filter treatment; normalize empty/loading/error visuals.
- Validation Steps: Compare multiple sidebar panel families with long labels, dense metadata, and empty states.
- Acceptance Criteria: Specialized sidebars look like they belong to the same product family as Explorer.
- Dependencies: Phase 03, Phase 15.
- Risks / Failure Modes: Over-normalizing specialized panels can remove useful identity where domain-specific cues matter.
- Accessibility / Readability Notes: Indentation, metadata chips, and focused rows must remain clearly distinguishable.
- Theme / Styling Notes: Rich sidebars need careful contrast control to avoid turning into flat text walls.
- Motion / Interaction Notes where relevant: Row reveal, expand, and hover states should remain restrained.
- Observability / Diagnostics Notes where relevant: Add sidebar family screenshot boards.
- Rollback / Safety Notes: Preserve existing data bindings while row and metadata presentation is upgraded.
- References / Context: These sidebars are visually important but often less polished than their functionality deserves.
- Example scenarios where useful: A backlinks panel with counts and excerpts feels as intentional as the Explorer tree next to it.

### P11-T03

- Phase ID: P11
- Task ID: P11-T03
- Task Title: Redesign graph, inspector, and metadata panel cards, chips, and small-content modules
- Priority: P2
- Category: Panel UI
- Atomic Improvements Covered: 60
- Objective: Improve the small-card and metadata-module language inside inspector-style panels.
- Why This Matters Now: Premium quality often breaks down in the small modules inside complex side panels.
- UI Problem Statement: Graph, inspector, metadata, and assistive panels use varied small content blocks with inconsistent hierarchy and spacing.
- User Experience Impact: Side-panel information can feel cluttered or under-structured.
- Scope: Metadata cards, property chips, relation pills, badges, secondary descriptions, mini-stat blocks, inspector section dividers.
- Out of Scope: Metadata schema correctness.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/canvas/MetadataPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/canvas/TagPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/GraphSidebarPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/ExtensionDetailPanel.cpp`
- Related Screens / Components / Surfaces: Metadata panels, graph sidebars, extension detail surfaces, canvas inspectors.
- Current UI Behavior / Appearance: Small metadata modules are useful but visually inconsistent.
- Intended UI Behavior / Appearance: Inspector-like content modules are compact, elegant, and easier to scan.
- Visual / Interaction Design Direction: Quiet card and chip system with stronger label/value hierarchy.
- Technical Styling Approach: Create micro-card, chip, and metadata-block variants shared across panel families.
- Implementation Steps: Inventory metadata modules; define microcomponent family; migrate graph/inspector/detail panels; normalize padding, chips, badges, and tertiary text.
- Validation Steps: Review metadata-heavy side panels with sparse and dense content.
- Acceptance Criteria: Small content modules feel product-wide rather than surface-specific.
- Dependencies: Phase 01, Phase 15.
- Risks / Failure Modes: Too much micro-card structure can make dense panels visually fragmented.
- Accessibility / Readability Notes: Chips and labels must remain legible at compact densities.
- Theme / Styling Notes: Micro-card layering should remain subtle enough not to create panel clutter.
- Motion / Interaction Notes where relevant: Chip and row state changes should remain lightweight.
- Observability / Diagnostics Notes where relevant: Add microcomponent panel screenshots to the gallery.
- Rollback / Safety Notes: Keep domain-specific data density intact while styling improves.
- References / Context: Metadata-rich side panels are where premium structure matters most.
- Example scenarios where useful: An extension detail panel shows tags, version info, and actions with clear, elegant hierarchy instead of utility-style blocks.

### P11-T04

- Phase ID: P11
- Task ID: P11-T04
- Task Title: Improve panel-specific empty, loading, error, and no-results states for sidebar families
- Priority: P1
- Category: Panel UI
- Atomic Improvements Covered: 60
- Objective: Make every sidebar family communicate sparse or exceptional states with the same premium calm.
- Why This Matters Now: Many panel states still feel temporary or too generic.
- UI Problem Statement: Sidebars often have panel-local empty and loading visuals that do not yet conform to one excellent system.
- User Experience Impact: Under-filled panels can make the product feel unfinished or confusing.
- Scope: Empty search, empty explorer sub-sections, no backlinks, empty graph, empty metadata, loading graph/search, panel-specific error states.
- Out of Scope: Error handling logic itself.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/SearchSidebarPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/ExplorerPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/GraphSidebarPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/EmptyPanelState.cpp`
- Related Screens / Components / Surfaces: Sidebar panels and inspector families.
- Current UI Behavior / Appearance: Empty/loading/error states vary widely and sometimes look generic.
- Intended UI Behavior / Appearance: Every sparse or exceptional sidebar state feels intentional and well composed.
- Visual / Interaction Design Direction: Calm, informative, non-apologetic empty and error states.
- Technical Styling Approach: Apply shared state-surface patterns with sidebar-specific icons, copy hierarchy, and action placement.
- Implementation Steps: Inventory panel states; align empty and error compositions; improve loading rhythm; remove generic-looking placeholders; ensure no-results states are distinct from empty states.
- Validation Steps: Trigger empty, loading, and error conditions across representative sidebars.
- Acceptance Criteria: Sparse or exceptional states no longer feel like provisional filler.
- Dependencies: Phase 03, Phase 16.
- Risks / Failure Modes: Over-unification can erase useful panel-specific messaging.
- Accessibility / Readability Notes: State messages and calls to action must remain easy to parse.
- Theme / Styling Notes: Sparse-state illustrations and backgrounds must avoid low-contrast flattening.
- Motion / Interaction Notes where relevant: Loading animation should be subtle and consistent with other panel families.
- Observability / Diagnostics Notes where relevant: Add sparse-state screenshots to the sidebar board.
- Rollback / Safety Notes: Keep current state hooks intact while the visual system is unified.
- References / Context: Honest sparse-state design is essential for a product with many contextual panels.
- Example scenarios where useful: A no-results Search panel feels polished and distinct from an uninitialized Search panel.

### P11-T05

- Phase ID: P11
- Task ID: P11-T05
- Task Title: Harmonize panel-local actions, badges, and inline metadata surfaces
- Priority: P2
- Category: Panel UI
- Atomic Improvements Covered: 60
- Objective: Ensure small panel-local controls and metadata treatments match the new premium panel language.
- Why This Matters Now: Minor panel-local elements often drag down the overall polish more than large layouts do.
- UI Problem Statement: Panel-local actions, tiny badges, filter chips, count labels, and metadata strips vary noticeably across sidebars.
- User Experience Impact: The panels feel less coherent and less trustworthy.
- Scope: Header badges, filter chips, count labels, local toolbar buttons, inline row actions, panel metadata strips.
- Out of Scope: Action semantics.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/SearchSidebarPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/ExplorerToolbar.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/GraphSidebarPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/ExtensionDetailPanel.cpp`
- Related Screens / Components / Surfaces: Local panel controls, metadata chips, row actions.
- Current UI Behavior / Appearance: Small panel-specific elements are individually usable but not optically aligned or semantically consistent.
- Intended UI Behavior / Appearance: Every small panel-local affordance feels designed within one system.
- Visual / Interaction Design Direction: Precise micro-UI with quiet confidence.
- Technical Styling Approach: Reuse control-state, chip, and icon metrics systems for all panel-local microcontrols.
- Implementation Steps: Audit microcontrols and badges; normalize chip and metadata styles; align icon sizes and spacing; remove surface-specific oddities.
- Validation Steps: Compare microcontrols across multiple sidebar families and densities.
- Acceptance Criteria: Small panel-local elements no longer signal subsystem drift.
- Dependencies: Phase 01, Phase 03, Phase 20.
- Risks / Failure Modes: Over-consolidation may erase useful domain-specific affordances.
- Accessibility / Readability Notes: Small badges and chips must remain readable and keyboard-navigable.
- Theme / Styling Notes: Microcontrols need careful opacity tuning so they remain visible without shouting.
- Motion / Interaction Notes where relevant: Chip activation and badge updates should feel stable and not jittery.
- Observability / Diagnostics Notes where relevant: Add microcontrol panel comparisons to the UI atlas.
- Rollback / Safety Notes: Keep semantics stable while optical cleanup proceeds.
- References / Context: Premium panel polish lives in these details as much as in the main layout.
- Example scenarios where useful: Search filter chips and graph metadata tags feel like siblings in one UI family.

### P11-T06

- Phase ID: P11
- Task ID: P11-T06
- Task Title: Add sidebar and inspector family galleries plus regression baselines
- Priority: P0
- Category: Cleanup / Consolidation
- Atomic Improvements Covered: 60
- Objective: Protect sidebar-family polish through explicit cross-panel reference sets.
- Why This Matters Now: Sidebars are numerous, varied, and highly prone to slow visual divergence.
- UI Problem Statement: There is no dedicated gallery that compares Explorer, Search, Outline, Backlinks, Graph, inspector, and metadata panel families side by side.
- User Experience Impact: Panel-quality drift becomes visible to users before it becomes visible to the team.
- Scope: Dense sidebars, sparse states, metadata modules, local controls, row variants, graph and inspector states.
- Out of Scope: Panel behavior logic.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/tests/unit`; `/Users/ryanrentfro/code/markamp/docs/v22_docs`; `/Users/ryanrentfro/code/markamp/src/ui/ExplorerPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/SearchSidebarPanel.cpp`
- Related Screens / Components / Surfaces: Sidebar and inspector panel family.
- Current UI Behavior / Appearance: Panel quality is reviewed one surface at a time instead of comparatively.
- Intended UI Behavior / Appearance: The whole sidebar family can be reviewed and protected as one visual ecosystem.
- Visual / Interaction Design Direction: Comparative panel-governance tooling.
- Technical Styling Approach: Build side-by-side fixture captures and grouped regression boards for panel families.
- Implementation Steps: Create panel fixtures; capture dense and sparse states; publish side-by-side galleries; wire regression checks for representative panel families.
- Validation Steps: Alter sidebar row density or badge styling and verify the regression board exposes the divergence.
- Acceptance Criteria: Sidebar-family polish becomes easier to maintain over time.
- Dependencies: P11-T01 through P11-T05.
- Risks / Failure Modes: Too many panel fixtures can make review cumbersome if not grouped well.
- Accessibility / Readability Notes: Include focus and high-contrast panel states in the gallery.
- Theme / Styling Notes: Capture all panel families in dark, light, and high-contrast themes.
- Motion / Interaction Notes where relevant: Include section-expand and hover-state captures for key panels.
- Observability / Diagnostics Notes where relevant: Store fixture metadata with panel IDs and density profiles.
- Rollback / Safety Notes: Keep the gallery additive while panel design stabilizes.
- References / Context: Sidebars are a central MarkAmp experience and deserve first-class design governance.
- Example scenarios where useful: A future Search panel refactor can be checked instantly against Explorer and Outline for visual family consistency.

