# Testing Guide

> Test strategy, execution, and tooling for MarkAmp v2.3.16.

---

## Overview

| Metric            | Value  |
| ----------------- | ------ |
| Test framework    | Catch2 |
| Test targets      | 451    |
| Test source files | 444    |
| Benchmark files   | 25     |
| Fuzz test files   | 6      |

---

## Test Strategy

### Unit Tests (`tests/unit/`)

Each component has a dedicated test file following the pattern `test_<component_name>.cpp`. Tests are isolated and do not require wxWidgets or a running application.

### Integration Tests

Cross-module tests validate component interactions:

- `test_phase49_integration_testing.cpp` — Cross-module validation
- `test_cross_module_test_suite.cpp` — Cross-module test harness

### Fuzz Tests (`fuzz/`)

Fuzz testing for security-critical components:

- Markdown parser
- HTML sanitizer
- Theme validator
- Configuration parser

### Benchmarks (`benchmarks/`)

Google Benchmark for performance-critical paths:

- Rendering pipeline
- Event bus throughput
- Arena allocator
- Quadtree spatial queries

---

## Running Tests

### Full Test Suite

```bash
# Build and run all tests
cmake --preset debug
cmake --build build/debug -j$(sysctl -n hw.ncpu)
cd build/debug && ctest --output-on-failure
```

### Specific Test Target

```bash
cd build/debug && ctest -R test_event_bus --output-on-failure
```

### Verbose Output

```bash
cd build/debug && ctest --output-on-failure --verbose
```

### List All Tests

```bash
cd build/debug && ctest -N
```

### Using Helper Script

```bash
./scripts/run_tests.sh
```

---

## Sanitizer Builds

### AddressSanitizer + UBSan

Detects memory errors, buffer overflows, and undefined behavior:

```bash
cmake --preset debug-asan
cmake --build build/debug-asan -j$(sysctl -n hw.ncpu)
cd build/debug-asan && ctest --output-on-failure
```

### ThreadSanitizer

Detects data races and deadlocks:

```bash
cmake --preset debug-tsan
cmake --build build/debug-tsan -j$(sysctl -n hw.ncpu)
cd build/debug-tsan && ctest --output-on-failure
```

### LeakSanitizer

Detects memory leaks:

```bash
cmake --preset debug-lsan
cmake --build build/debug-lsan -j$(sysctl -n hw.ncpu)
cd build/debug-lsan && ctest --output-on-failure
```

---

## Static Analysis

### clang-tidy

```bash
cmake --preset debug-clang-tidy
cmake --build build/debug-clang-tidy -j$(sysctl -n hw.ncpu)
```

Configuration in `.clang-tidy` at project root.

### cppcheck

```bash
cppcheck --project=build/debug/compile_commands.json \
    --suppressions-list=cppcheck-suppressions.txt \
    --enable=all
```

---

## Fuzz Testing

```bash
./scripts/run_fuzz.sh
```

Fuzz targets in `fuzz/`:

- Markdown parsing with malformed input
- Theme JSON with invalid structures
- HTML sanitizer with XSS payloads

---

## Chaos Testing

```bash
./scripts/run_chaos_tests.sh
```

Tests resilience under:

- Random subsystem failures
- Memory pressure
- CPU starvation
- I/O errors

---

## Load Testing

```bash
./scripts/run_load_tests.sh
```

Validates performance under:

- Large file handling
- Many concurrent operations
- Extension activation storms

---

## Writing Tests

### New Test File Template

```cpp
#include <catch2/catch_test_macros.hpp>
#include "core/MyComponent.h"

TEST_CASE("MyComponent basic operations", "[my_component]")
{
    MyComponent component;

    SECTION("initialization")
    {
        REQUIRE(component.is_ready());
    }

    SECTION("process data")
    {
        auto result = component.process("input");
        REQUIRE(result.has_value());
        CHECK(result.value() == "expected");
    }
}
```

### Test Registration

Add to `tests/CMakeLists.txt`:

```cmake
add_executable(test_my_component unit/test_my_component.cpp)
target_link_libraries(test_my_component PRIVATE Catch2::Catch2WithMain markamp_dependencies)
add_test(NAME test_my_component COMMAND test_my_component)
```

### Test Requirements

- All new features **must** have unit tests
- Minimum 80% coverage for new files
- Integration tests for cross-component interactions
- No `any` or `unknown` types in test code
