# Test Coverage Report

> Coverage analysis and gap identification for MarkAmp v2.3.16.

---

## Coverage Summary

| Metric                 | Value                                              |
| ---------------------- | -------------------------------------------------- |
| Test targets           | 451                                                |
| Test source files      | 444                                                |
| Source headers         | 734 (core: 512, canvas: 98, UI: 88, rendering: 36) |
| Source implementations | 383+                                               |

> [!NOTE]
> MarkAmp uses Catch2 which does not include built-in coverage instrumentation. To generate line-level coverage, use `gcov`/`llvm-cov` with the debug build. See instructions below.

---

## Generating Coverage

### Using llvm-cov (macOS)

```bash
# Build with coverage flags
cmake -S . -B build/coverage \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_CXX_FLAGS="--coverage -fprofile-instr-generate -fcoverage-mapping" \
    -DMARKAMP_BUILD_TESTS=ON \
    -G Ninja

cmake --build build/coverage -j$(sysctl -n hw.ncpu)

# Run tests to generate profile data
cd build/coverage
LLVM_PROFILE_FILE="markamp.profraw" ctest --output-on-failure

# Merge and generate report
llvm-profdata merge -sparse markamp.profraw -o markamp.profdata
llvm-cov report ./markamp -instr-profile=markamp.profdata
llvm-cov show ./markamp -instr-profile=markamp.profdata -format=html -output-dir=coverage_html
```

### Using gcov (Linux)

```bash
cmake -S . -B build/coverage \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_CXX_FLAGS="--coverage" \
    -DMARKAMP_BUILD_TESTS=ON \
    -G Ninja

cmake --build build/coverage -j$(nproc)
cd build/coverage && ctest --output-on-failure

# Generate report
gcovr --html --html-details -o coverage.html -r ../../src
```

---

## Coverage by Module

| Module               | Test Files | Source Headers | Est. Coverage | Notes                                               |
| -------------------- | ---------- | -------------- | ------------- | --------------------------------------------------- |
| Core / Event System  | 3          | 4              | High          | `test_event_bus`, `test_flat_event_bus`             |
| Config               | 2          | 8              | High          | `test_config`, `test_settings_catalog`              |
| Theme System         | 5          | 14             | High          | `test_theme`, `test_theme_*`                        |
| Plugin System        | 6          | 15             | High          | `test_plugin_*`, `test_when_clause`                 |
| Extension Management | 4          | 13             | High          | `test_extension_*`                                  |
| Canvas Core          | 8          | 30             | High          | `test_phase11_canvas` through `test_phase14`        |
| Canvas Objects       | 10         | 20             | High          | `test_sticky_note`, `test_shape_data`, etc.         |
| Canvas Collaboration | 2          | 10             | Medium        | `test_phase13_canvas_collab`                        |
| Attribute View (AV)  | 2          | 16             | Medium        | `test_phase21_av_database`, `test_properties`       |
| FSRS / Flashcards    | 3          | 9              | High          | `test_phase22_flashcard_system`, `test_fsrs_engine` |
| Search               | 3          | 15             | Medium        | `test_search_*`, `test_phase10_search`              |
| Knowledge Graph      | 2          | 14             | Medium        | `test_phase16_knowledge_graph`                      |
| AI Integration       | 1          | 7              | Low           | `test_phase26_ai_integration`                       |
| Export/Import        | 3          | 14             | Medium        | `test_phase24_export_publishing`                    |
| Rendering Pipeline   | 6          | 12             | High          | `test_html_renderer`, `test_rendering_*`            |
| FX Engine            | 2          | 12             | Medium        | `test_phase28_fx_system`                            |
| Security             | 3          | 10             | High          | `test_phase29_security`, `test_safe_*`              |
| Performance          | 4          | 20             | Medium        | `test_phase30_performance`, `test_profiler`         |
| Notebook             | 2          | 11             | Medium        | `test_phase15_notebook_completion`                  |
| Version Control      | 2          | 9              | Medium        | `test_phase25_version_control`                      |
| Cloud Sync           | 1          | 9              | Low           | `test_phase27_cloud_sync`                           |
| Markdown Parser      | 2          | 6              | High          | `test_markdown_parser`, `test_piece_table`          |
| Presentation         | 1          | 5              | Low           | `test_phase43_presentation`                         |
| WYSIWYG              | 1          | 4              | Low           | `test_phase45_wysiwyg`                              |
| Localization         | 1          | 4              | Low           | `test_phase47_localization`                         |

---

## Critical Gaps

| Area           | Gap                                                          | Priority |
| -------------- | ------------------------------------------------------------ | -------- |
| AI Integration | Only phase-level integration test, no unit tests per service | Medium   |
| Cloud Sync     | Minimal test coverage, sync engine untested                  | Medium   |
| WYSIWYG        | Stub-level testing                                           | Low      |
| Presentation   | Basic phase tests only                                       | Low      |
| Localization   | Basic phase tests only                                       | Low      |

---

## Recommended Coverage Targets

| Tier     | Threshold | Applies To                                           |
| -------- | --------- | ---------------------------------------------------- |
| Critical | ≥ 90%     | EventBus, Config, Theme, Plugin, Security, Rendering |
| Standard | ≥ 80%     | Canvas, Search, Graph, AV database, FSRS             |
| Minimum  | ≥ 60%     | AI, Cloud, Presentation, WYSIWYG, i18n               |

---

## CI/CD Coverage Integration

> [!IMPORTANT]
> Coverage gates are defined in `.github/workflows/`. The recommended approach is to enforce minimum thresholds per module using `llvm-cov` and fail the build if coverage drops below the target.

```yaml
# Example GitHub Actions step
- name: Check coverage
  run: |
    llvm-cov report ./build/coverage/markamp \
      -instr-profile=markamp.profdata \
      | grep -E "TOTAL" | awk '{if ($4 < 80.0) exit 1}'
```
