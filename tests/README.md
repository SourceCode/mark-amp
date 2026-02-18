# MarkAmp Test Suite

## Directory Structure

```
tests/
├── unit/             # Unit tests for individual classes and functions
├── integration/      # Cross-subsystem integration tests
├── regression/       # Tests for previously-fixed bugs
├── performance/      # Performance regression tests
├── benchmark/        # Catch2 benchmark suites
├── fuzz/             # Fuzz testing stubs (seed corpus)
├── chaos/            # Chaos testing (fault injection)
├── load/             # Load testing
└── CMakeLists.txt    # Test build configuration
```

## Running Tests

### Build All Tests

```bash
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
cmake --build . -j$(sysctl -n hw.ncpu)
```

### Run All Tests

```bash
cd build
ctest --output-on-failure -j$(sysctl -n hw.ncpu)
```

### Run a Specific Test Target

```bash
cd build
./tests/test_phase34_theme            # Run theme tests
./tests/test_phase34_regression       # Run regression tests
```

### Run Tests by Tag

```bash
cd build
./tests/test_phase34_regression "[regression][parser]"
./tests/test_phase34_properties "[property][config]"
```

### Run Benchmarks

```bash
cd build
./tests/test_phase34_bench            # All benchmarks
./tests/test_phase34_bench "[benchmark][parse]"   # Parse benchmarks only
```

## Adding Tests

### 1. Create the Test File

Place your test file in the appropriate directory:

- **Unit tests** → `tests/unit/test_<component>.cpp`
- **Regression tests** → `tests/regression/test_<issue>.cpp`
- **Fuzz tests** → `tests/fuzz/fuzz_<component>.cpp`
- **Benchmarks** → `tests/benchmark/bench_<component>.cpp`

### 2. Include Catch2

```cpp
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Component — behavior under test", "[component][tag]")
{
    // Arrange
    // Act
    // Assert
    REQUIRE(result == expected);
}
```

### 3. Register in CMakeLists.txt

Add a new target in `tests/CMakeLists.txt`:

```cmake
add_executable(test_<name>
    unit/test_<name>.cpp
)
target_include_directories(test_<name> PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_<name> PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_<name> COMMAND test_<name>)
```

## Test Categories

| Tag            | Description                    |
| -------------- | ------------------------------ |
| `[regression]` | Previously-fixed bugs          |
| `[property]`   | Property-based invariant tests |
| `[fuzz]`       | Seed corpus fuzz tests         |
| `[benchmark]`  | Performance benchmarks         |
| `[theme]`      | Theme system                   |
| `[parser]`     | Markdown parser                |
| `[render]`     | HTML renderer                  |
| `[board]`      | Canvas board/serializer        |
| `[config]`     | Config system                  |
| `[eventbus]`   | EventBus                       |
| `[search]`     | Search system                  |
| `[plugin]`     | Plugin system                  |

## Coverage Expectations

- **Core infrastructure** (EventBus, Config, Theme): >90%
- **Serialization paths**: 100% round-trip coverage
- **Error handling**: All error codes tested
- **Parser/Renderer**: All node types covered
