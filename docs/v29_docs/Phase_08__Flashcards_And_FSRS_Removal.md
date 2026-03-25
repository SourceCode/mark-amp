# Phase 08: Flashcards And FSRS Removal

## Task Count

- 2 tasks

## Task V29-P08-T01

- Phase ID: V29-P08
- Task ID: V29-P08-T01
- Task Title: Delete flashcard UI, commands, and document-extraction entry points
- Priority: High
- Category: Flashcards Removal
- Objective: Remove flashcard browser/review UI, command providers, extraction flows, import/export, and all shell affordances.
- Why This Matters Now: Flashcards still appear as a tool family and still integrate with documents and AI.
- Removal Gap Statement: `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/ActivityBar.cpp`, and `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp` still expose flashcards while command and extraction classes keep the workflow alive.
- User / Product Impact: Removes a side-product that competes with the retained editing experience.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp:1311`; `/Users/ryanrentfro/code/markamp/src/ui/ActivityBar.cpp:236`; `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp:2893`; `/Users/ryanrentfro/code/markamp/src/core/fsrs/FlashcardCommandProvider.cpp`; `/Users/ryanrentfro/code/markamp/src/core/fsrs/FlashcardExtractor.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/FlashcardBrowserPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/FlashcardReviewPanel.cpp`
- Prior Plan References: `/Users/ryanrentfro/code/markamp/docs/v23_docs/Phase_15__Structured_Data_AV_Graph_Task_And_Knowledge_Workflow_Completion.md`; `/Users/ryanrentfro/code/markamp/docs/v24_docs/Phase_17__Advanced_Domain_Workflows_AV_Graph_Tasks_Presentation_And_Node_Editor.md`
- Scope: UI panels, menu/tool entries, activity bar/sidebar entries, command providers, extraction flows, import/export, and document help/examples.
- Out of Scope: Underlying FSRS persistence/runtime deletion handled in V29-P08-T02.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/FlashcardBrowserPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/FlashcardReviewPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/core/fsrs/FlashcardCommandProvider.cpp`; `/Users/ryanrentfro/code/markamp/src/core/fsrs/FlashcardExtractor.cpp`; `/Users/ryanrentfro/code/markamp/src/core/HelpService.h`
- Related Features / Systems / Components: Tools menu, activity bar, command palette, document parsing, import/export.
- Current Behavior / Presence: Flashcards remain discoverable in the shell and can still be generated or reviewed.
- Intended Post-Removal Behavior: No user-facing flashcard workflow remains anywhere in the product.
- Removal / Simplification Direction: Delete the flashcard feature family rather than merely hiding review UI.
- Technical Approach: Remove UI surfaces and command/extraction entry points first, then delete backend runtime and persistence.
- Implementation Steps: Delete browser/review panels; remove menu/sidebar entries; remove command provider and extractor; remove flashcard help samples and import/export seams.
- Validation Steps: Search UI and help for `Flashcard`; verify no shell surface, command, or document help entry remains.
- Acceptance Criteria: Flashcards are absent from the user-facing product surface.
- Dependencies: Phases 02 through 04.
- Parallelization Notes: Can run alongside task removal if file ownership stays separate.
- Risks / Failure Modes: AI and help flows may still reference flashcards after UI deletion.
- Migration / Compatibility Notes: Old flashcard data directories are handled in V29-P08-T02 and Phase 11.
- UX / Layout Cleanup Notes: Remove card/deck labels, badges, and empty states.
- Cleanup / Consolidation Notes: Remove any flashcard-specific icons or theme hooks in later cleanup phases.
- Rollback / Safety Notes: Keep retained tools menu and activity bar stable while removing entries.
- References / Context: Flashcards were never a core retained mission in v29.
- Example Scenarios: No `Flashcard Browser`, no `Flashcard Review Session`, no `Flashcard: Start Review`.

## Task V29-P08-T02

- Phase ID: V29-P08
- Task ID: V29-P08-T02
- Task Title: Delete FSRS scheduler, review persistence, deck store, and AI flashcard generation
- Priority: High
- Category: Flashcards Removal
- Objective: Remove the backend flashcard/FSRS runtime and any AI generation or storage that supports it.
- Why This Matters Now: Leaving the FSRS store and review runtime behind preserves dead persistence, events, and build surface.
- Removal Gap Statement: `/Users/ryanrentfro/code/markamp/src/core/fsrs/FlashcardStore.h` persists cards, decks, and review logs; `/Users/ryanrentfro/code/markamp/src/core/AIFlashcardGenerator.cpp` and `/Users/ryanrentfro/code/markamp/src/core/AICommandProvider.cpp` keep generation hooks alive.
- User / Product Impact: Reduces storage complexity and removes a study-system backend from the editor.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/core/fsrs/FlashcardStore.h`; `/Users/ryanrentfro/code/markamp/src/core/fsrs/FSRSEngine.cpp`; `/Users/ryanrentfro/code/markamp/src/core/fsrs/FSRSReviewSession.cpp`; `/Users/ryanrentfro/code/markamp/src/core/AIFlashcardGenerator.cpp`; `/Users/ryanrentfro/code/markamp/src/core/AICommandProvider.cpp`
- Prior Plan References: `/Users/ryanrentfro/code/markamp/docs/v23_docs/Phase_17__Service_Stub_Replacement_And_Interface_Adoption_Completion.md`; `/Users/ryanrentfro/code/markamp/docs/v25_docs/Phase_18__Migration_Stub_And_Dead_Path_Retirement.md`
- Scope: FSRS engine, store, review session, card/deck types, leech detection, AI generation, and feature events.
- Out of Scope: Generic AI writing assistance unrelated to flashcards.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/fsrs/FlashcardStore.h`; `/Users/ryanrentfro/code/markamp/src/core/fsrs/FSRSEngine.cpp`; `/Users/ryanrentfro/code/markamp/src/core/fsrs/FSRSReviewSession.cpp`; `/Users/ryanrentfro/code/markamp/src/core/AIFlashcardGenerator.cpp`; `/Users/ryanrentfro/code/markamp/src/core/FlashcardTypes.h`
- Related Features / Systems / Components: Workspace storage, AI commands, document metadata, event bus.
- Current Behavior / Presence: Flashcard storage and review logic remain fully implemented.
- Intended Post-Removal Behavior: No flashcard or FSRS runtime or persisted state is actively used by the app.
- Removal / Simplification Direction: Delete the runtime and keep only narrow compatibility cleanup if old storage directories must be ignored.
- Technical Approach: Remove the subsystem from production code and scrub flashcard-specific event and AI references.
- Implementation Steps: Delete fsrs sources; remove `AI: Generate Flashcards`; remove flashcard data types; delete storage integration; clean event families in Phase 10.
- Validation Steps: Search production code for `FSRS`, `FlashcardStore`, `ReviewSession`, and `AIFlashcardGenerator`; confirm no active references remain.
- Acceptance Criteria: Flashcard backend code no longer ships and no flashcard storage is loaded.
- Dependencies: V29-P08-T01.
- Parallelization Notes: Coordinate with Phase 13 and Phase 11 for AI and persistence cleanup.
- Risks / Failure Modes: Old workspace storage might still be loaded on startup unless migration behavior is added first.
- Migration / Compatibility Notes: Existing flashcard storage directories should be safely ignored or optionally cleaned up with notification.
- UX / Layout Cleanup Notes: Remove any review statistics, deck counts, or AI prompts that mention flashcards.
- Cleanup / Consolidation Notes: Delete related event families, tests, and docs in later phases.
- Rollback / Safety Notes: Avoid touching unrelated AI service plumbing.
- References / Context: v29 treats flashcards as a full retirement, not a hidden experimental subsystem.
- Example Scenarios: No flashcard review logs load from workspace storage; no AI command suggests flashcard generation.
