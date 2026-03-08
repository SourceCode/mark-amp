# Performance and Reliability Test Cases

Test cases derived from **v6**, **v7**: Performance optimization, error handling, resilience.

## TC-PERF-001: Performance

| ID        | Test Case                      | Unit                       | E2E | Regression |
| --------- | ------------------------------ | -------------------------- | --- | ---------- |
| PERF-001a | Cold start < 500ms             | ✅ `test_app_init.cpp`     | ❌  | ❌         |
| PERF-001b | Theme switch < 50ms            | ✅ `test_theme_engine.cpp` | ❌  | ❌         |
| PERF-001c | File open < 100ms              | ✅ `test_editor.cpp`       | ❌  | ❌         |
| PERF-001d | Scroll performance (60fps)     | ✅ performance tests       | ❌  | ❌         |
| PERF-001e | Memory usage stable under load | ✅ `test_allocator.cpp`    | ❌  | ❌         |

## TC-PERF-002: Error Handling

| ID        | Test Case                           | Unit                                | E2E                         | Regression |
| --------- | ----------------------------------- | ----------------------------------- | --------------------------- | ---------- |
| PERF-002a | No crash on missing file open       | ✅ `test_file_system.cpp`           | ✅ `error_handling.spec.ts` | ✅         |
| PERF-002b | No crash on invalid markdown        | ✅ `test_markdown_parser.cpp`       | ❌                          | ✅         |
| PERF-002c | Graceful handling of corrupt config | ✅ `test_config.cpp`                | ❌                          | ✅         |
| PERF-002d | Recovery from extension crash       | ✅ `test_extension_integration.cpp` | ❌                          | ❌         |
| PERF-002e | No crash on rapid operations        | ➖                                  | ✅ `error_handling.spec.ts` | ✅         |

## Coverage Summary

| Category       | Unit     | E2E      | Regression |
| -------------- | -------- | -------- | ---------- |
| Performance    | 5/5 ✅   | 0/5 ❌   | 0/5 ❌     |
| Error Handling | 4/5 ✅   | 2/5 ✅   | 4/5 ✅     |
| **Total**      | **9/10** | **2/10** | **4/10**   |
