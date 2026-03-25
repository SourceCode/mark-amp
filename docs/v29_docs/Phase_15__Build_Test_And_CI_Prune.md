# Phase 15: Build Test And CI Prune

## Task Count

- 2 tasks

## Task V29-P15-T01

- Phase ID: V29-P15
- Task ID: V29-P15-T01
- Task Title: Remove retired-feature source registration from CMake and build scripts
- Priority: Critical
- Category: Testing / Regression Protection
- Objective: Prune notebook, canvas, flashcard/FSRS, and task source files from the build graph once product code deletion is complete.
- Why This Matters Now: The build graph is one of the strongest remaining indicators that the removed features are still treated as shipping code.
- Removal Gap Statement: `/Users/ryanrentfro/code/markamp/src/CMakeLists.txt` and `/Users/ryanrentfro/code/markamp/tests/CMakeLists.txt` explicitly compile these feature families across many historical phases.
- User / Product Impact: Reduces build time, compile noise, and accidental dependency resurrection.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/CMakeLists.txt:387`; `/Users/ryanrentfro/code/markamp/src/CMakeLists.txt:691`; `/Users/ryanrentfro/code/markamp/src/CMakeLists.txt:852`; `/Users/ryanrentfro/code/markamp/src/CMakeLists.txt:1070`; `/Users/ryanrentfro/code/markamp/tests/CMakeLists.txt:342`; `/Users/ryanrentfro/code/markamp/tests/CMakeLists.txt:501`; `/Users/ryanrentfro/code/markamp/tests/CMakeLists.txt:519`
- Prior Plan References: `/Users/ryanrentfro/code/markamp/docs/v23_docs/Phase_20__Final_Placeholder_Eradication_Dead_Path_Removal_And_Release_Gates.md`; `/Users/ryanrentfro/code/markamp/docs/v25_docs/Phase_20__Release_Candidate_Signoff_Packaging_And_Final_Closure.md`
- Scope: `src/CMakeLists.txt`, `tests/CMakeLists.txt`, packaging/build scripts, and any phase-generated scripts that still assume these feature sources exist.
- Out of Scope: Test content quality handled in V29-P15-T02.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/CMakeLists.txt`; `/Users/ryanrentfro/code/markamp/tests/CMakeLists.txt`; `/Users/ryanrentfro/code/markamp/scripts`
- Related Features / Systems / Components: Build graph, packaging, compilation units.
- Current Behavior / Presence: Removed feature files remain explicitly registered for compile and link.
- Intended Post-Removal Behavior: Build graph only includes retained product code and any narrow migration readers still intentionally kept.
- Removal / Simplification Direction: Remove the source registrations completely, not just compile-guard them.
- Technical Approach: Delete source entries in lockstep with file deletion and keep build green after each batch.
- Implementation Steps: Remove source registrations; remove obsolete packaging references; prune phase-specific scripts that only generate removed-feature plans or fixtures.
- Validation Steps: Configure and build after each prune batch; verify no missing-source or dead-include errors remain.
- Acceptance Criteria: Build files contain no active source registration for retired feature implementations.
- Dependencies: Phases 06 through 10.
- Parallelization Notes: Works best after direct source deletion stabilizes.
- Risks / Failure Modes: Shared headers may still be transitively included by retained code and fail compilation.
- Migration / Compatibility Notes: If a compatibility reader remains, document it explicitly in build files.
- UX / Layout Cleanup Notes: None.
- Cleanup / Consolidation Notes: Remove stale phase comments in CMake that refer to retired feature milestones.
- Rollback / Safety Notes: Keep prune commits logically grouped to simplify troubleshooting.
- References / Context: Build registration is one of the authoritative proofs that the product is actually smaller.
- Example Scenarios: `src/CMakeLists.txt` no longer lists `core/Notebook*.cpp` or `canvas/*.cpp`.

## Task V29-P15-T02

- Phase ID: V29-P15
- Task ID: V29-P15-T02
- Task Title: Delete obsolete tests and replace them with removal guards plus retained-flow regression coverage
- Priority: Critical
- Category: Testing / Regression Protection
- Objective: Remove tests that validate retired features and add tests that prove those features are gone while retained startup/editor flows remain stable.
- Why This Matters Now: Old feature tests will fail once deletion begins, but deleting them without replacement leaves no proof that removal is complete and safe.
- Removal Gap Statement: The repository still contains extensive canvas and notebook test families plus flashcard and task tests registered in `tests/CMakeLists.txt`.
- User / Product Impact: Keeps QA focused on the smaller product rather than deleted capabilities.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/tests/unit/test_v19_phase07_notebook.cpp`; `/Users/ryanrentfro/code/markamp/tests/unit/test_v19_phase08_canvas.cpp`; `/Users/ryanrentfro/code/markamp/tests/unit/test_phase22_flashcard_system.cpp`; `/Users/ryanrentfro/code/markamp/tests/unit/test_phase23_task_calendar.cpp`
- Prior Plan References: `/Users/ryanrentfro/code/markamp/docs/v19_docs/Phase_09__Feedback_Observability_And_Regression_Harnesses.md`; `/Users/ryanrentfro/code/markamp/docs/v24_docs/Phase_19__Diagnostics_Smoke_E2E_Fuzz_And_Regression_Gates.md`
- Scope: Unit, integration, smoke, and release-signoff tests touching retired features; new removal-proof tests; migration tests.
- Out of Scope: Non-feature-specific retained editor tests except where they need updates.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/tests/unit`; `/Users/ryanrentfro/code/markamp/tests/integration`; `/Users/ryanrentfro/code/markamp/tests/CMakeLists.txt`
- Related Features / Systems / Components: Test suite, CI, release gates.
- Current Behavior / Presence: Tests still validate removed features as if they are required product capabilities.
- Intended Post-Removal Behavior: Test suite verifies absence of retired features and stability of retained flows.
- Removal / Simplification Direction: Delete feature tests and replace them with concise removal assertions, migration fixtures, and retained smoke coverage.
- Technical Approach: Remove obsolete test targets; add compile-time or search-based absence assertions where appropriate; add startup/restore downgrade tests.
- Implementation Steps: Delete feature-owned tests; update CMake; add regression tests for startup, menus, activity bar, restore downgrade, and command-palette absence.
- Validation Steps: Run targeted retained-flow tests and new migration/removal guards.
- Acceptance Criteria: CI no longer depends on deleted feature tests and does verify removal completeness plus retained stability.
- Dependencies: V29-P15-T01 and Phase 19 planning.
- Parallelization Notes: Test deletion can start early; replacement guards need stable post-removal behavior.
- Risks / Failure Modes: Removing too many shared tests can accidentally erase coverage for retained shell behavior.
- Migration / Compatibility Notes: Include fixtures representing old workspaces with retired features present.
- UX / Layout Cleanup Notes: Add screenshot or string assertions proving shell entry points are gone.
- Cleanup / Consolidation Notes: Re-baseline release-signoff suites around the smaller product.
- Rollback / Safety Notes: Keep retained regression coverage green before deleting broad test families.
- References / Context: This phase turns prior completion/test hardening work into explicit retirement-proof coverage.
- Example Scenarios: A smoke test proves the menu bar has no `Canvas` or `Notebooks` heading.
