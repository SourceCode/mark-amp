# Phase 19: Testing Fuzz E2E Smoke And Regression Completion

## Outcome

Convert the current test estate from broad-but-uneven coverage into a completion-grade safety net that closes placeholder tests, fuzz stubs, skipped areas, and missing smoke paths for the product’s intended feature set.

## Completion Count

300 atomic completion tasks across 5 execution tasks.

### P19-T01

- Phase ID: P19
- Task ID: P19-T01
- Task Title: Replace placeholder and stub-normalizing tests with real completion assertions
- Priority: P0
- Category: Testing / Regression Protection
- Atomic Completion Tasks Covered: 60
- Objective: Stop tests from legitimizing unfinished product behavior.
- Why This Matters Now: Placeholder-friendly tests are one of the biggest reasons unfinished behavior can persist safely.
- Completion Gap Statement: The suite still contains tests explicitly named placeholder or assertions that accept stub behavior for intended product features.
- User / Product Impact: CI can stay green while important features remain unfinished.
- Repository Evidence: [test_mark_amp_app.cpp](/Users/ryanrentfro/code/markamp/tests/unit/test_mark_amp_app.cpp), [test_outline.cpp](/Users/ryanrentfro/code/markamp/tests/unit/test_outline.cpp), [test_theme_marketplace.cpp](/Users/ryanrentfro/code/markamp/tests/unit/test_theme_marketplace.cpp)
- Scope: Placeholder tests, stub-return assertions, thin smoke tests, and renamed completion tests.
- Out of Scope: Legitimate unsupported-feature tests for intentionally gated capabilities.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/tests/unit/test_mark_amp_app.cpp`; `/Users/ryanrentfro/code/markamp/tests/unit/test_outline.cpp`; `/Users/ryanrentfro/code/markamp/tests/unit/test_theme_marketplace.cpp`; `/Users/ryanrentfro/code/markamp/tests/unit/test_service_registry.cpp`
- Related Features / Systems / Components: Entire test suite, CI, completion gates.
- Current Behavior: Some tests explicitly bless placeholder or stub behavior in product-facing areas.
- Intended Completed Behavior: Tests assert real behavior for completed features and explicit unsupported states where completion is deferred.
- Missing Pieces: Test replacement plans, fixture coverage, and suite hygiene.
- Technical Approach: Audit and replace placeholder-oriented tests subsystem by subsystem.
- Implementation Steps: Tag placeholder tests; map them to real workflows; rewrite assertions; remove or quarantine obsolete scaffolding tests.
- Validation Steps: Reintroduce a known placeholder behavior and confirm the new tests fail.
- Acceptance Criteria: No core user workflow is covered only by a placeholder or stub-normalizing test.
- Dependencies: Phase 01.
- Risks / Failure Modes: Some placeholder tests may still be needed temporarily while stronger coverage is built.
- Cleanup / Migration Notes where relevant: Delete renamed placeholder tests only after replacements are live.
- Observability / Diagnostics Notes where relevant: Publish a “placeholder tests remaining” count in CI.
- Rollback / Safety Notes: Keep old tests temporarily in a quarantined lane if replacement coverage is still maturing.
- References / Context: This phase aligns the test suite with the completion mission of `v23`.
- Example scenarios where useful: A search test asserts real results rather than accepting an empty stub result as valid.

### P19-T02

- Phase ID: P19
- Task ID: P19-T02
- Task Title: Finish fuzz harnesses and incomplete corpus coverage for parsing serialization and board workflows
- Priority: P1
- Category: Testing / Regression Protection
- Atomic Completion Tasks Covered: 60
- Objective: Replace fuzz stubs with real fuzz targets for high-risk parsers and serializers.
- Why This Matters Now: Several important parsers and serializers remain weakly protected despite being complex and security-relevant.
- Completion Gap Statement: Fuzz infrastructure exists, but at least some targets are still explicit stubs.
- User / Product Impact: Parser and serializer bugs can survive into release without enough adversarial coverage.
- Repository Evidence: [tests/fuzz/fuzz_markdown_parser.cpp](/Users/ryanrentfro/code/markamp/tests/fuzz/fuzz_markdown_parser.cpp), [tests/fuzz/fuzz_board_serializer.cpp](/Users/ryanrentfro/code/markamp/tests/fuzz/fuzz_board_serializer.cpp), [tests/CMakeLists.txt](/Users/ryanrentfro/code/markamp/tests/CMakeLists.txt)
- Scope: Markdown parsing, board serialization, importers, settings JSON, repository snapshots, and transport parsers.
- Out of Scope: UI logic not suitable for fuzzing.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/tests/fuzz/fuzz_markdown_parser.cpp`; `/Users/ryanrentfro/code/markamp/tests/fuzz/fuzz_board_serializer.cpp`; `/Users/ryanrentfro/code/markamp/tests/CMakeLists.txt`; `/Users/ryanrentfro/code/markamp/fuzz`
- Related Features / Systems / Components: Fuzzing, parsers, serializers, import/export, security.
- Current Behavior: Some fuzz targets are still placeholders rather than real stress harnesses.
- Intended Completed Behavior: High-risk parsing and serialization paths are covered by active fuzz harnesses with seed corpora.
- Missing Pieces: Real target wiring, corpus seeds, crash triage, and CI hooks.
- Technical Approach: Prioritize fuzzing for externally influenced and persistence-critical formats.
- Implementation Steps: Implement harnesses; build seed corpora; wire fuzz targets into scripts; add triage and minimization docs.
- Validation Steps: Run fuzz harnesses locally and verify they exercise real parsing code paths.
- Acceptance Criteria: Fuzz-target stubs are eliminated for the highest-risk parsing and serialization families.
- Dependencies: Phase 04, Phase 07, Phase 13.
- Risks / Failure Modes: Poor corpus design can give false confidence.
- Cleanup / Migration Notes where relevant: Remove comments and build entries that label fuzz targets as stubs.
- Observability / Diagnostics Notes where relevant: Publish corpus coverage and crash statistics from fuzz runs.
- Rollback / Safety Notes: Keep fuzzing isolated from user data and network dependencies.
- References / Context: Fuzz completeness is part of finishing a large document-heavy application responsibly.
- Example scenarios where useful: A malformed board payload is caught by a real fuzz harness instead of waiting for a user import crash.

### P19-T03

- Phase ID: P19
- Task ID: P19-T03
- Task Title: Expand smoke and end-to-end suites around mixed artifact workflows and restart behavior
- Priority: P0
- Category: Testing / Regression Protection
- Atomic Completion Tasks Covered: 60
- Objective: Cover the highest-value product paths across files, notebooks, canvases, panels, settings, and restore flows.
- Why This Matters Now: The product can feel broad in unit tests while still failing obvious real-user paths.
- Completion Gap Statement: Current smoke and E2E coverage is not yet broad enough across mixed-surface, mixed-artifact workflows.
- User / Product Impact: Users still become the first integrated test for many cross-surface flows.
- Repository Evidence: [scripts/smoke_test.sh](/Users/ryanrentfro/code/markamp/scripts/smoke_test.sh), [tests/integration/test_e2e.cpp](/Users/ryanrentfro/code/markamp/tests/integration/test_e2e.cpp), [tests/integration/test_preview_integration.cpp](/Users/ryanrentfro/code/markamp/tests/integration/test_preview_integration.cpp)
- Scope: Create/open/save/restore, editor-notebook-canvas transitions, panel activation, settings apply/restart, search and diagnostics, Git and build basics.
- Out of Scope: Full visual regression.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/scripts/smoke_test.sh`; `/Users/ryanrentfro/code/markamp/tests/integration/test_e2e.cpp`; `/Users/ryanrentfro/code/markamp/tests/integration/test_preview_integration.cpp`; `/Users/ryanrentfro/code/markamp/tests/e2e`
- Related Features / Systems / Components: Smoke tests, E2E, restart flows, mixed artifacts.
- Current Behavior: Mixed-workflow coverage is still too selective.
- Intended Completed Behavior: A finished product path is exercised end to end before release.
- Missing Pieces: Scenario breadth, restart harnesses, mixed-artifact fixtures, and panel/settings integration flows.
- Technical Approach: Add a small number of high-value, deterministic cross-surface scenarios rather than only many unit slices.
- Implementation Steps: Define mixed-artifact smoke paths; add restart/reopen scenarios; expand E2E fixtures; validate panel and settings behaviors in realistic sessions.
- Validation Steps: Run the suite on a clean temp workspace and confirm each scenario completes end to end.
- Acceptance Criteria: Core product workflows are protected by smoke or E2E coverage, not only unit tests.
- Dependencies: Earlier subsystem phases.
- Risks / Failure Modes: E2E tests can become flaky if UI synchronization and environment setup are not disciplined.
- Cleanup / Migration Notes where relevant: Retire low-value smoke steps once stronger mixed-workflow scenarios exist.
- Observability / Diagnostics Notes where relevant: Capture run traces, logs, and session artifacts on failure.
- Rollback / Safety Notes: Keep E2E fixtures deterministic and hermetic.
- References / Context: Completion needs workflow-level proof, not only component-level proof.
- Example scenarios where useful: A smoke test creates a file, a notebook, and a canvas, edits each, saves them, restarts, and restores the session.

### P19-T04

- Phase ID: P19
- Task ID: P19-T04
- Task Title: Add failure-mode and unsupported-state regression suites
- Priority: P1
- Category: Testing / Regression Protection
- Atomic Completion Tasks Covered: 60
- Objective: Protect the product’s failure and unsupported-state behavior now that silent stubs are being removed.
- Why This Matters Now: Completing features also means completing their failure contracts.
- Completion Gap Statement: Many unsupported or failure paths have been weakly modeled or silently stubbed and now need explicit regression coverage.
- User / Product Impact: Error handling can regress into misleading silence if not tested.
- Repository Evidence: [test_safe_mode.cpp](/Users/ryanrentfro/code/markamp/tests/unit/test_safe_mode.cpp), [test_telemetry.cpp](/Users/ryanrentfro/code/markamp/tests/unit/test_telemetry.cpp), [test_phase12_canvas_advanced.cpp](/Users/ryanrentfro/code/markamp/tests/unit/test_phase12_canvas_advanced.cpp)
- Scope: Unsupported features, missing backends, transport outages, import failures, parser failures, panel-gated states.
- Out of Scope: UI animation and styling behavior.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/tests/unit/test_safe_mode.cpp`; `/Users/ryanrentfro/code/markamp/tests/unit/test_telemetry.cpp`; `/Users/ryanrentfro/code/markamp/tests/unit/test_phase12_canvas_advanced.cpp`; `/Users/ryanrentfro/code/markamp/tests/unit/test_html_renderer.cpp`
- Related Features / Systems / Components: Failure handling, unsupported states, gating, tests.
- Current Behavior: Some unsupported-state coverage exists, but it is not yet systematically aligned with completed-service contracts.
- Intended Completed Behavior: Unsupported and failure behavior remains explicit, stable, and non-silent.
- Missing Pieces: Coverage matrices, subsystem-specific failure fixtures, and caller-facing assertions.
- Technical Approach: Pair each supported-capability contract with a tested unsupported/failure contract.
- Implementation Steps: Build failure fixtures; add unsupported-state tests; align error messages and codes; verify caller behavior and gating responses.
- Validation Steps: Force backend absence or transport failure and confirm the resulting state is explicit and correct.
- Acceptance Criteria: Silent placeholder or fake-success regressions are caught as test failures.
- Dependencies: Phase 17, Phase 18.
- Risks / Failure Modes: Overfitting tests to exact error strings can make them brittle.
- Cleanup / Migration Notes where relevant: Remove tests that only assert “empty result” in unsupported cases where a richer contract now exists.
- Observability / Diagnostics Notes where relevant: Capture failure traces and explicit unsupported-state outputs as artifacts.
- Rollback / Safety Notes: Keep unsupported-state tests separate from flaky environmental failures.
- References / Context: This is how `v23` ensures honesty after stub removal.
- Example scenarios where useful: An unavailable renderer produces an explicit unsupported error and a gated UI path, not placeholder content.

### P19-T05

- Phase ID: P19
- Task ID: P19-T05
- Task Title: Publish a completion-focused test scoreboard tied to remaining unfinished work
- Priority: P2
- Category: Diagnostics / Recovery Completion
- Atomic Completion Tasks Covered: 60
- Objective: Make the remaining completion debt visible through test status, not only documents.
- Why This Matters Now: A large completion program needs a concise operational scoreboard.
- Completion Gap Statement: The repo has many tests, but not one completion-oriented summary of what unfinished areas remain unprotected.
- User / Product Impact: Indirect but important; teams can miss where confidence is still low.
- Repository Evidence: [tests/CMakeLists.txt](/Users/ryanrentfro/code/markamp/tests/CMakeLists.txt), [scripts/run_tests.sh](/Users/ryanrentfro/code/markamp/scripts/run_tests.sh), [scripts/smoke_test.sh](/Users/ryanrentfro/code/markamp/scripts/smoke_test.sh)
- Scope: Coverage by subsystem, placeholder-test counts, fuzz status, smoke status, unsupported-state coverage, release blockers.
- Out of Scope: Third-party dashboard tooling.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/tests/CMakeLists.txt`; `/Users/ryanrentfro/code/markamp/scripts/run_tests.sh`; `/Users/ryanrentfro/code/markamp/scripts/smoke_test.sh`; `/Users/ryanrentfro/code/markamp/docs/v23_docs`
- Related Features / Systems / Components: Tests, CI, completion inventory, release readiness.
- Current Behavior: Test reporting is broad but not framed explicitly around unfinished-feature closure.
- Intended Completed Behavior: The team can see which completion categories still lack strong automated proof.
- Missing Pieces: Completion schema, per-phase mapping, and artifact publishing.
- Technical Approach: Map tests and coverage artifacts back to the `v23` completion categories and phases.
- Implementation Steps: Define scoreboard schema; tag tests by completion family; publish summary counts and gaps; integrate into release checks.
- Validation Steps: Mark a subsystem uncovered and confirm the scoreboard highlights it.
- Acceptance Criteria: Test readiness and completion readiness are visible in one place.
- Dependencies: P19-T01 through P19-T04.
- Risks / Failure Modes: Scoreboards can become vanity metrics if not tied to real blocker rules.
- Cleanup / Migration Notes where relevant: Replace ad hoc test summaries once the completion scoreboard is in use.
- Observability / Diagnostics Notes where relevant: Publish the scoreboard with every CI run and release candidate build.
- Rollback / Safety Notes: Keep scoreboard generation read-only and deterministic.
- References / Context: This phase makes the test story useful for actually finishing the product.
- Example scenarios where useful: The scoreboard shows cloud sync fully covered, but PDF annotations still missing restart-level tests.
