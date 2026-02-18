# Phase 34: Testing Completeness

## Overview
170+ test files exist in tests/unit/ but coverage is uneven: some systems have comprehensive tests while others (UI panels, rendering pipeline, platform code, new resilience primitives) have minimal or no tests. This phase fills testing gaps and establishes testing infrastructure for ongoing quality.

## Prerequisites
- Phase 29 (Security primitives to test)
- Phase 30 (Performance infrastructure to benchmark)

## Tasks

### Task 1: Add Integration Test Framework
**Files:** `tests/integration/CMakeLists.txt`, `tests/integration/test_workspace_lifecycle.cpp`
**Description:** Existing tests are unit tests. Add integration test framework: tests that exercise multiple subsystems together, simulating real user workflows.
**Acceptance Criteria:**
- Integration test CMake target: `markamp_integration_tests`
- Test fixture: creates temporary workspace with sample files
- Workspace lifecycle: create, open, edit, save, close
- Cross-service tests: editor edit -> backlink update -> graph refresh
- Integration tests run separately from unit tests
- CI: integration tests run after unit tests

### Task 2: Add UI Panel Rendering Tests
**Files:** `tests/unit/test_panel_rendering.cpp`
**Description:** UI panels that exist as code but have no rendering tests. Test that panels create, layout, and display correctly.
**Acceptance Criteria:**
- OutputPanel: creates, shows text, channel switching
- ProblemsPanel: creates, shows diagnostics, navigation
- SearchPanel: creates, shows results, filtering
- BacklinksPanel: creates, shows backlinks, navigation
- Each panel: theme token usage verified
- Panel lifecycle: create, show, hide, destroy

### Task 3: Add Rendering Pipeline Tests
**Files:** `tests/unit/test_rendering_pipeline.cpp`
**Description:** Test rendering pipeline: HtmlRenderer, CodeBlockRenderer, MermaidBlockRenderer, EmbedBlockRenderer, and block rendering accuracy.
**Acceptance Criteria:**
- HtmlRenderer: all Markdown elements render correctly
- CodeBlockRenderer: syntax highlighting for 10+ languages
- MermaidBlockRenderer: valid Mermaid produces SVG
- EmbedBlockRenderer: file embeds resolved
- Super block renderer: complex nested structures
- Rendering round-trip: Markdown -> HTML -> verify

### Task 4: Add Canvas Object Tests
**Files:** `tests/unit/test_canvas_objects.cpp`
**Description:** Test all canvas object types: StickyNote, TextBox, ShapeData, ConnectorData, ImageObject, VideoEmbedObject, BookmarkCardObject, IconObject.
**Acceptance Criteria:**
- Each object: create, serialize, deserialize
- Property access: position, size, style, content
- Transform: move, resize, rotate
- Serialization round-trip preserves all data
- Object type detection from serialized data
- Edge cases: empty content, extreme sizes

### Task 5: Add EventBus Stress Tests
**Files:** `tests/unit/test_eventbus_stress.cpp`
**Description:** Stress test EventBus: high-frequency publishing, many subscribers, cross-thread delivery, memory under load.
**Acceptance Criteria:**
- 10,000 events/second: all delivered
- 100 subscribers per event: all receive
- Cross-thread: 4 threads publishing simultaneously
- Memory: bounded under sustained load
- RAII subscription cleanup under load
- No deadlocks under contention

### Task 6: Add Config System Tests
**Files:** `tests/unit/test_config_system.cpp`
**Description:** Test config system comprehensively: YAML parsing, type coercion, defaults, validation, change notifications, persistence.
**Acceptance Criteria:**
- YAML frontmatter parsing for all types
- Type coercion: string to int, bool, float
- Default values: all 40+ defaults tested
- Validation: invalid values rejected with message
- Change notification: listeners called on update
- Persistence: save/load round-trip

### Task 7: Add Plugin System Tests
**Files:** `tests/unit/test_plugin_lifecycle.cpp`
**Description:** Test plugin lifecycle: discovery, manifest parsing, activation, deactivation, dependency resolution, error handling.
**Acceptance Criteria:**
- Manifest parsing: valid and invalid manifests
- Dependency resolution: topological sort correctness
- Activation: lazy activation events tested
- Deactivation: cleanup verified
- Error: malformed extension handled gracefully
- PluginContext: all 25+ pointers validated

### Task 8: Add Theme System Tests
**Files:** `tests/unit/test_theme_system.cpp`
**Description:** Test theme system: theme loading, token resolution, cached brush/pen/font, VSCode theme import, theme switching.
**Acceptance Criteria:**
- All 64 themes load without error
- All theme tokens resolve to valid colors
- Cached wxBrush/wxPen/wxFont created correctly
- VSCode theme import produces valid MarkAmp theme
- Theme switch: all tokens update
- Missing token: fallback to default

### Task 9: Add Search System Tests
**Files:** `tests/unit/test_search_system.cpp`
**Description:** Test search end-to-end: indexing, querying, ranking, regex search, incremental update, performance.
**Acceptance Criteria:**
- Index: all Markdown elements indexed
- Query: exact match, fuzzy, regex
- Ranking: exact > prefix > fuzzy
- Incremental: file change updates index
- Performance: 1000-file query < 100ms
- Edge cases: empty query, very long query, special characters

### Task 10: Add Serialization Round-Trip Tests
**Files:** `tests/unit/test_serialization.cpp`
**Description:** Test serialization for all persistent data: boards, notebooks, config, workspace state, sync state, flashcard data.
**Acceptance Criteria:**
- Board: all object types preserve through serialize/deserialize
- Notebook: cells, outputs, metadata preserved
- Config: all setting types preserved
- Workspace state: tabs, layout, scroll positions preserved
- Flashcard: decks, cards, review history preserved
- No data loss or type corruption

### Task 11: Add Error Handling Tests
**Files:** `tests/unit/test_error_handling.cpp`
**Description:** Test error handling paths: file not found, invalid input, network failure, disk full, permission denied, corrupt data.
**Acceptance Criteria:**
- File not found: clear error message, no crash
- Invalid Markdown: partial render, error diagnostic
- Corrupt board JSON: error message, load empty board
- Disk full: error on save, data not lost
- Permission denied: clear error, suggest fix
- All errors: Result<T> with proper error type

### Task 12: Add Memory Leak Tests
**Files:** `tests/unit/test_memory_leaks.cpp`
**Description:** Test for memory leaks in key scenarios: open/close 100 files, create/delete 1000 canvas objects, start/stop 100 search queries.
**Acceptance Criteria:**
- File open/close: memory returns to baseline
- Canvas object create/delete: memory returns to baseline
- Search query cycle: no memory growth
- EventBus subscribe/unsubscribe: no leaked subscriptions
- Plugin load/unload: no leaked services
- Test with address sanitizer enabled

### Task 13: Add Thread Safety Tests
**Files:** `tests/unit/test_thread_safety.cpp`
**Description:** Test thread safety: concurrent EventBus access, concurrent file operations, concurrent config reads, concurrent search queries.
**Acceptance Criteria:**
- EventBus: 4 threads publishing, no data race
- File operations: concurrent read/write, no corruption
- Config: concurrent read + single write
- Search: concurrent queries, no crash
- ThreadSanitizer clean for all tests
- No deadlocks under contention

### Task 14: Add Regression Test Suite
**Files:** `tests/regression/CMakeLists.txt`, `tests/regression/test_known_issues.cpp`
**Description:** Regression tests for known issues: crashes, data corruption, and edge cases that were previously bugs.
**Acceptance Criteria:**
- Each known issue has a test case
- Test prevents regression of the fix
- Test documents the original issue
- Regression tests run in CI
- New bugs automatically get regression tests
- At least 20 regression test cases

### Task 15: Add Property-Based Tests
**Files:** `tests/unit/test_properties.cpp`
**Description:** Property-based tests for core algorithms: serialization (serialize -> deserialize = identity), search (results always contain matches), sort (output always sorted).
**Acceptance Criteria:**
- Serialization: round-trip property for all types
- Search: results contain query term
- Sort: output is ordered
- Path validation: valid paths accepted, invalid rejected
- Config: set then get returns set value
- Random input generation for each property

### Task 16: Add Performance Benchmark Suite
**Files:** `tests/benchmark/CMakeLists.txt`, `tests/benchmark/bench_editor.cpp`, `tests/benchmark/bench_canvas.cpp`, `tests/benchmark/bench_search.cpp`
**Description:** Benchmark suite using Google Benchmark: measure critical path performance and detect regressions.
**Acceptance Criteria:**
- Editor: typing latency, file open time, scroll performance
- Canvas: render time, hit test time, object creation
- Search: query time, index build time
- Benchmark results saved as JSON
- CI: benchmark comparison against baseline
- Performance regression detection

### Task 17: Add Fuzz Testing
**Files:** `tests/fuzz/CMakeLists.txt`, `tests/fuzz/fuzz_markdown_parser.cpp`, `tests/fuzz/fuzz_board_serializer.cpp`
**Description:** Fuzz testing for parsers and serializers: Markdown parser, board serializer, config parser, extension manifest parser.
**Acceptance Criteria:**
- Markdown parser: no crash on arbitrary input
- Board serializer: no crash on malformed JSON
- Config parser: no crash on invalid YAML
- Manifest parser: no crash on invalid JSON
- SafeRegex: no crash on pathological patterns
- Fuzz corpus: seed with valid + edge case inputs

### Task 18: Add Test Coverage Reporting
**Files:** `CMakeLists.txt`, `CMakePresets.json`
**Description:** Add coverage build preset: compile with coverage instrumentation, generate HTML coverage report, identify untested code.
**Acceptance Criteria:**
- `coverage` CMake preset with `-fprofile-instr-generate -fcoverage-mapping`
- Coverage report: HTML with line-by-line highlighting
- Coverage target: > 70% line coverage for core
- Coverage target: > 50% for UI (harder to test)
- CI: coverage report generated and published
- Untested files listed in report

### Task 19: Add Test Documentation
**Files:** `tests/README.md`
**Description:** Document test organization: unit vs integration vs regression vs benchmark vs fuzz. How to run, how to add new tests, coverage expectations.
**Acceptance Criteria:**
- Test directory structure documented
- How to run each test category
- How to add a new test
- Coverage expectations per subsystem
- CI test pipeline documented
- Known test limitations documented

### Task 20: Add CI Test Pipeline
**Files:** `.github/workflows/tests.yml`
**Description:** GitHub Actions CI pipeline: build, unit tests, integration tests, benchmarks, coverage. Matrix: macOS, Windows, Linux.
**Acceptance Criteria:**
- Build on all 3 platforms
- Unit tests on all platforms
- Integration tests on all platforms
- Benchmarks on Linux (consistent environment)
- Coverage on Linux with report
- Fuzz testing: 60-second runs in CI

## Testing Requirements
- This phase IS about testing itself
- All test categories have at least 10 test cases
- CI pipeline passes on all platforms
- Coverage > 70% for core, > 50% for UI

## Phase Completion Criteria
- Integration test framework established
- UI panel rendering tests
- EventBus stress tests
- Serialization round-trip tests
- Memory leak and thread safety tests
- Benchmark suite with regression detection
- Fuzz testing for parsers
- CI pipeline on all platforms
- All tests pass
