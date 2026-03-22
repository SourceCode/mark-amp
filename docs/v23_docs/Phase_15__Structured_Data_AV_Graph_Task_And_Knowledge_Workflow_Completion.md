# Phase 15: Structured Data AV Graph Task And Knowledge Workflow Completion

## Outcome

Finish the structured-data, knowledge, and productivity subsystems whose models are broad but whose runtime, persistence, or panel behavior still contains stubbed or shallow seams.

## Completion Count

300 atomic completion tasks across 5 execution tasks.

### P15-T01

- Phase ID: P15
- Task ID: P15-T01
- Task Title: Finish AV table editor and renderer paths still marked stub or deferred
- Priority: P1
- Category: Panel Completion
- Atomic Completion Tasks Covered: 60
- Objective: Make AV surfaces real structured-data tools rather than thin UI scaffolding.
- Why This Matters Now: AV breadth is impressive, but key editor/renderer interfaces still admit stub status.
- Completion Gap Statement: Table, cell editor, and cell renderer layers still contain explicit stub or deferred markers.
- User / Product Impact: Structured-data workflows remain less complete than the product surface implies.
- Repository Evidence: [AVTablePanel.h](/Users/ryanrentfro/code/markamp/src/ui/av/AVTablePanel.h), [AVCellRenderer.h](/Users/ryanrentfro/code/markamp/src/ui/av/AVCellRenderer.h), [AVCellEditor.h](/Users/ryanrentfro/code/markamp/src/ui/av/AVCellEditor.h)
- Scope: Table rows, cell editors, cell renderers, type-specific editing, validation, and save behavior.
- Out of Scope: Pure UI styling.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/av/AVTablePanel.h`; `/Users/ryanrentfro/code/markamp/src/ui/av/AVCellRenderer.h`; `/Users/ryanrentfro/code/markamp/src/ui/av/AVCellEditor.h`; `/Users/ryanrentfro/code/markamp/src/ui/av/AVTablePanel.cpp`
- Related Features / Systems / Components: AV, tables, galleries, kanban, structured data.
- Current Behavior: Key AV UI layers still identify themselves as stub or deferred.
- Intended Completed Behavior: AV editing uses complete type-aware editors and renderers with full data flow.
- Missing Pieces: Type-specific editors/renderers, interaction hooks, and persistence integration.
- Technical Approach: Finish AV component interfaces and align them with the existing AV model layer.
- Implementation Steps: Implement editor/render factory coverage; complete panel behavior; add validation and undo integration; wire persistence and filtering.
- Validation Steps: Edit dense AV data across supported column types and verify save/reload behavior.
- Acceptance Criteria: AV table workflows no longer depend on stub interfaces in production.
- Dependencies: Phase 04.
- Risks / Failure Modes: AV complexity can create many partially complete type combinations if rollout is not systematic.
- Cleanup / Migration Notes where relevant: Remove “full wxWidgets implementation deferred” comments once complete.
- Observability / Diagnostics Notes where relevant: Emit AV edit, validation, and renderer-fallback diagnostics.
- Rollback / Safety Notes: Gate unsupported column types explicitly if full coverage is not ready.
- References / Context: This phase turns AV from a broad subsystem into a finished one.
- Example scenarios where useful: Editing a relation or formula cell uses a real editor, validator, and persistence path.

### P15-T02

- Phase ID: P15
- Task ID: P15-T02
- Task Title: Finish knowledge-graph and backlink workflows beyond summary and local-shell behavior
- Priority: P2
- Category: Search / Navigation Completion
- Atomic Completion Tasks Covered: 60
- Objective: Complete graph and backlink workflows where shells exist but deeper behavior remains partial.
- Why This Matters Now: Knowledge tools are exposed as first-class workbench concepts.
- Completion Gap Statement: Graph-related surfaces and helpers still rely on placeholder shell wiring or shallow result depth.
- User / Product Impact: Knowledge-navigation features feel less complete than the UI suggests.
- Repository Evidence: [LayoutManager.cpp](/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp), [GraphService.cpp](/Users/ryanrentfro/code/markamp/src/core/GraphService.cpp), [BacklinkService.cpp](/Users/ryanrentfro/code/markamp/src/core/BacklinkService.cpp)
- Scope: Graph data retrieval, local graph panels, backlinks, navigation, and panel synchronization.
- Out of Scope: Pure rendering polish.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`; `/Users/ryanrentfro/code/markamp/src/core/GraphService.cpp`; `/Users/ryanrentfro/code/markamp/src/core/BacklinkService.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/BacklinksPanel.cpp`
- Related Features / Systems / Components: Knowledge graph, backlinks, local graph, navigation.
- Current Behavior: Some graph/backlink surfaces still operate as shallow adjuncts rather than complete tools.
- Intended Completed Behavior: Knowledge navigation is content-complete and interacts cleanly with editor and panel workflows.
- Missing Pieces: Deeper graph queries, panel content paths, navigation semantics, and state sync.
- Technical Approach: Finish graph/query services and route panel surfaces through them.
- Implementation Steps: Complete graph/local-graph queries; wire backlinks to real navigation; replace shell-level summary surfaces; add sparse/error-state handling.
- Validation Steps: Open linked documents, navigate graphs, and verify panel synchronization and deep links.
- Acceptance Criteria: Knowledge workflow surfaces no longer depend on summary-only or placeholder behavior.
- Dependencies: Phase 05, Phase 08.
- Risks / Failure Modes: Graph queries can become expensive if caching and incremental updates are weak.
- Cleanup / Migration Notes where relevant: Remove placeholder graph mini-map registrations after real graph surfaces land.
- Observability / Diagnostics Notes where relevant: Log graph query durations and panel refresh causes.
- Rollback / Safety Notes: Hide unsupported graph surfaces until their data paths are complete.
- References / Context: This phase completes the “knowledge IDE” story rather than just sketching it.
- Example scenarios where useful: Selecting a document updates backlinks and local graph surfaces with real data immediately.

### P15-T03

- Phase ID: P15
- Task ID: P15-T03
- Task Title: Complete task calendar deck and study data flows that still defer parsing or persistence depth
- Priority: P2
- Category: Stub Service Completion
- Atomic Completion Tasks Covered: 60
- Objective: Finish adjacent productivity systems that remain structurally broad but runtime-light.
- Why This Matters Now: These systems are part of the intended product scope and still contain explicit deferred parsing or simplified behavior.
- Completion Gap Statement: Deck, flashcard, and related productivity flows still contain deferred parsing and simplified storage assumptions.
- User / Product Impact: Adjacent knowledge workflows feel less complete than their API surface suggests.
- Repository Evidence: [DeckStore.cpp](/Users/ryanrentfro/code/markamp/src/core/DeckStore.cpp), [FlashcardStore.cpp](/Users/ryanrentfro/code/markamp/src/core/fsrs/FlashcardStore.cpp), [ActivityTimeline.cpp](/Users/ryanrentfro/code/markamp/src/core/ActivityTimeline.cpp)
- Scope: Deck parsing, flashcard persistence, timeline semantics, task/calendar interoperability where present.
- Out of Scope: New product concepts beyond the existing subsystem set.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/DeckStore.cpp`; `/Users/ryanrentfro/code/markamp/src/core/fsrs/FlashcardStore.cpp`; `/Users/ryanrentfro/code/markamp/src/core/ActivityTimeline.cpp`; `/Users/ryanrentfro/code/markamp/src/core/TaskService.cpp`
- Related Features / Systems / Components: Decks, flashcards, activity, tasks, calendar.
- Current Behavior: Some productivity subsystems still use deferred parsing or simplified semantics.
- Intended Completed Behavior: Structured productivity workflows are durable and semantically complete where exposed.
- Missing Pieces: Full parsing, persistence depth, semantic correctness, and cross-feature integration.
- Technical Approach: Finish underlying stores and parsers before expanding UI exposure further.
- Implementation Steps: Replace deferred parsers; harden stores; align activity and task semantics; add import/export and session coverage.
- Validation Steps: Persist decks and flashcards, reload them, and verify study and task timelines.
- Acceptance Criteria: Exposed productivity features no longer rely on deferred parsing or simplified placeholder semantics.
- Dependencies: Phase 04.
- Risks / Failure Modes: Subsystems with lower current surface area may have hidden coupling that only appears when completed.
- Cleanup / Migration Notes where relevant: Retire temporary file formats or transitional store logic during completion.
- Observability / Diagnostics Notes where relevant: Emit parse-failure and store-migration diagnostics.
- Rollback / Safety Notes: Provide migration backups when changing deck or flashcard persistence formats.
- References / Context: `v23` is about full product completion, not only the headline editor/canvas/notebook features.
- Example scenarios where useful: Importing a deck preserves its cards and metadata through restart instead of dropping complex JSON content.

### P15-T04

- Phase ID: P15
- Task ID: P15-T04
- Task Title: Finish search, indexing, and query support for structured content families
- Priority: P1
- Category: Search / Navigation Completion
- Atomic Completion Tasks Covered: 60
- Objective: Ensure structured-content systems participate fully in search and navigation.
- Why This Matters Now: Partial indexing leaves broad subsystems effectively hidden from product workflows.
- Completion Gap Statement: Structured content support is uneven across indexing, query, and navigation systems.
- User / Product Impact: Users cannot discover or navigate all intended content uniformly.
- Repository Evidence: [ContentIndexer.cpp](/Users/ryanrentfro/code/markamp/src/core/ContentIndexer.cpp), [SearchService.cpp](/Users/ryanrentfro/code/markamp/src/core/SearchService.cpp), [NotebookSearchIndex.h](/Users/ryanrentfro/code/markamp/src/core/NotebookSearchIndex.h)
- Scope: AV, flashcards, decks, graph data, notebook content, and structured asset indexing/query participation.
- Out of Scope: Pure full-text search UI polish.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/ContentIndexer.cpp`; `/Users/ryanrentfro/code/markamp/src/core/SearchService.cpp`; `/Users/ryanrentfro/code/markamp/src/core/NotebookSearchIndex.h`; `/Users/ryanrentfro/code/markamp/src/core/AVQueryEngine.h`
- Related Features / Systems / Components: Indexing, search, structured content, navigation.
- Current Behavior: Structured systems are not yet uniformly represented in the search/query layer.
- Intended Completed Behavior: Structured content can be found and navigated consistently through finished index/query paths.
- Missing Pieces: Extractors, content adapters, query integration, and result shaping.
- Technical Approach: Extend the finished search/index infrastructure to all intended structured content families.
- Implementation Steps: Add index adapters; map structured queries to search surfaces; add navigation targets and result summaries; cover sparse/error states.
- Validation Steps: Search for AV entries, notebook cells, flashcards, and graph-linked content from unified search surfaces.
- Acceptance Criteria: Structured-content systems no longer sit outside the product’s main discovery flows.
- Dependencies: Phase 05.
- Risks / Failure Modes: Structured indexing can create stale or duplicated search state if adapters are not versioned and invalidated correctly.
- Cleanup / Migration Notes where relevant: Remove content-family exclusions that existed only because adapters were unfinished.
- Observability / Diagnostics Notes where relevant: Track index coverage by content family.
- Rollback / Safety Notes: Allow family-specific indexing to be disabled if a new adapter is unstable.
- References / Context: Completion requires structured subsystems to participate in the rest of the IDE, not live beside it.
- Example scenarios where useful: Unified search can find a notebook variable, an AV row, and a flashcard-backed note in one query flow.

### P15-T05

- Phase ID: P15
- Task ID: P15-T05
- Task Title: Add structured-domain completion harnesses and data-migration coverage
- Priority: P0
- Category: Testing / Regression Protection
- Atomic Completion Tasks Covered: 60
- Objective: Protect structured-data and knowledge subsystem completion with realistic data fixtures.
- Why This Matters Now: These subsystems are broad enough that regressions can hide behind narrow model tests.
- Completion Gap Statement: Structured-content features still lack enough fixture-driven coverage proving they are complete rather than present.
- User / Product Impact: Data-heavy regressions can corrupt or flatten user workflows quietly.
- Repository Evidence: [tests/unit/test_phase27_cloud_sync.cpp](/Users/ryanrentfro/code/markamp/tests/unit/test_phase27_cloud_sync.cpp), [tests/unit/test_backlinks_panel.cpp](/Users/ryanrentfro/code/markamp/tests/unit/test_backlinks_panel.cpp), [tests/unit/test_fsrs](/Users/ryanrentfro/code/markamp/tests/unit)
- Scope: AV editing, graph/backlink updates, deck persistence, search/index participation, migrations.
- Out of Scope: Visual-only tests.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/tests/unit`; `/Users/ryanrentfro/code/markamp/tests/integration`; `/Users/ryanrentfro/code/markamp/src/ui/av`; `/Users/ryanrentfro/code/markamp/src/core/fsrs`
- Related Features / Systems / Components: Structured data, graph, tasks, decks, tests.
- Current Behavior: Coverage still leans more toward individual model slices than finished workflow families.
- Intended Completed Behavior: Fixture-based tests prove structured domains work end to end and survive migrations.
- Missing Pieces: Realistic fixtures, migration tests, unified search/index scenarios, and persistence round trips.
- Technical Approach: Build representative data sets that span multiple structured subsystems and exercise their shared contracts.
- Implementation Steps: Create mixed structured fixtures; add migration tests; validate index/search coverage; add save/reopen scenarios and panel integration tests.
- Validation Steps: Break a structured adapter or parser and confirm the suite fails on real workflows.
- Acceptance Criteria: Structured-domain completion is guarded by realistic, multi-system regression coverage.
- Dependencies: P15-T01 through P15-T04.
- Risks / Failure Modes: Large fixtures can become slow or hard to maintain if not curated well.
- Cleanup / Migration Notes where relevant: Remove tests that only prove thin placeholder panels exist.
- Observability / Diagnostics Notes where relevant: Store fixture snapshots and migration logs with failures.
- Rollback / Safety Notes: Keep fixture data isolated and versioned.
- References / Context: This phase stops structured subsystems from remaining second-class completion citizens.
- Example scenarios where useful: Editing an AV table updates search results and survives export/import in one integrated test.
