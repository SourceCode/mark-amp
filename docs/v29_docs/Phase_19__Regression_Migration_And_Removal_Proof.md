# Phase 19: Regression Migration And Removal Proof

## Task Count

- 2 tasks

## Task V29-P19-T01

- Phase ID: V29-P19
- Task ID: V29-P19-T01
- Task Title: Add removal-proof tests that assert the four features are absent from shipped UI and command surfaces
- Priority: Critical
- Category: Testing / Regression Protection
- Objective: Build explicit tests that fail if Canvas, Notebook, Flashcards, or Tasks reappear in menus, navigation, command surfaces, or help.
- Why This Matters Now: After deleting large subsystems, the highest-value regression guard is proving they do not silently come back through shell or docs drift.
- Removal Gap Statement: Existing tests mostly validate presence and completion, not absence.
- User / Product Impact: Protects the smaller product shape over time.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/ActivityBar.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`; `/Users/ryanrentfro/code/markamp/src/core/HelpService.h`
- Prior Plan References: `/Users/ryanrentfro/code/markamp/docs/v24_docs/Phase_19__Diagnostics_Smoke_E2E_Fuzz_And_Regression_Gates.md`; `/Users/ryanrentfro/code/markamp/docs/v25_docs/Phase_19__Integrated_Validation_Recovery_Observability_And_Regression_Gates.md`
- Scope: Menu absence, command-palette absence, activity bar absence, startup/help absence, and build-registration absence tests.
- Out of Scope: Manual QA checklist handled in Phase 20.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/tests/unit`; `/Users/ryanrentfro/code/markamp/tests/integration`; `/Users/ryanrentfro/code/markamp/tests/CMakeLists.txt`
- Related Features / Systems / Components: Regression suite, shell, command surfaces, docs.
- Current Behavior / Presence: No dedicated removal-proof suite exists.
- Intended Post-Removal Behavior: Automated tests explicitly assert these features are gone from shipped surfaces.
- Removal / Simplification Direction: Shift from presence-validation to absence-validation for retired domains.
- Technical Approach: Add targeted unit/integration tests that inspect menu models, activity bar models, help registries, and command registrations.
- Implementation Steps: Add shell absence tests; add help-text absence tests; add CMake or source-registration guard tests where practical.
- Validation Steps: Run the targeted suite and confirm failures occur if removed strings or IDs are reintroduced.
- Acceptance Criteria: CI includes explicit removal-proof coverage for all four retired feature families.
- Dependencies: Phases 02 through 18.
- Parallelization Notes: Can begin once shell and docs surfaces stabilize.
- Risks / Failure Modes: String-only tests may be brittle unless paired with model-level assertions.
- Migration / Compatibility Notes: Absence tests should allow a single explicit migration notice if designed.
- UX / Layout Cleanup Notes: Ensure tests cover user-visible strings and models, not just internal types.
- Cleanup / Consolidation Notes: Replace broad removed-feature completion tests with lean absence guards.
- Rollback / Safety Notes: Keep tests narrow and deterministic.
- References / Context: This is the durable proof that the removal wave is complete.
- Example Scenarios: Test fails if `Canvas` returns to the menu bar or command palette.

## Task V29-P19-T02

- Phase ID: V29-P19
- Task ID: V29-P19-T02
- Task Title: Add upgrade and migration regression fixtures covering old persisted state with retired features present
- Priority: Critical
- Category: Persistence / Migration Cleanup
- Objective: Prove that old workspaces, layouts, and storage containing retired features are downgraded safely.
- Why This Matters Now: Removal is not complete until upgrade paths are stable for existing users.
- Removal Gap Statement: Old state may still contain notebook/canvas artifacts, flashcard stores, task panels, or removed sidebar modes.
- User / Product Impact: Protects real-world upgrades and prevents support regressions.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`; `/Users/ryanrentfro/code/markamp/src/core/ArtifactRegistry.h`; `/Users/ryanrentfro/code/markamp/src/core/fsrs/FlashcardStore.h`
- Prior Plan References: `/Users/ryanrentfro/code/markamp/docs/v25_docs/Phase_03__Persistence_Autosave_Restore_Recovery_And_Session_Trust.md`; `/Users/ryanrentfro/code/markamp/docs/v25_docs/Phase_18__Migration_Stub_And_Dead_Path_Retirement.md`
- Scope: Fixtures and tests for old workspace state, old layout state, old recent items, flashcard storage, and task caches.
- Out of Scope: General performance or fuzz coverage.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/tests/unit`; `/Users/ryanrentfro/code/markamp/tests/integration`; `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`
- Related Features / Systems / Components: Migration, startup, restore, storage compatibility.
- Current Behavior / Presence: Compatibility behavior is not yet proven for removed-feature state.
- Intended Post-Removal Behavior: Old state is downgraded deterministically and the app lands in retained views with no crash.
- Removal / Simplification Direction: Keep only the minimum compatibility needed and prove it with fixtures.
- Technical Approach: Add representative serialized-state fixtures, load them in tests, and assert fallback behavior.
- Implementation Steps: Create old-state fixtures; add tests for downgraded sidebar/workbench modes and old artifact records; add flashcard/task storage ignore tests.
- Validation Steps: Run migration tests under clean and existing-user style setups.
- Acceptance Criteria: Upgrade from old state containing retired features is covered by automated tests and passes.
- Dependencies: Phase 11.
- Parallelization Notes: Can run alongside Phase 20 release validation prep.
- Risks / Failure Modes: Fixtures may lag actual persisted formats if not taken from real examples.
- Migration / Compatibility Notes: This is the proof layer for the compatibility policy defined in Phase 01.
- UX / Layout Cleanup Notes: Validate any user notice appears once and is understandable.
- Cleanup / Consolidation Notes: Keep fixture set small but representative.
- Rollback / Safety Notes: Do not remove compatibility readers until these tests are passing.
- References / Context: Upgrade safety is the most important non-UI risk in the removal wave.
- Example Scenarios: A workspace file that previously restored Canvas now opens cleanly in an editor-only shell.
