# Implementation Guide

> Repository layout, coding conventions, and source organization for MarkAmp v2.3.16.

---

## Repository Layout

```
markamp/
├── CMakeLists.txt           # Top-level CMake (version, presets)
├── CMakePresets.json         # 9 CMake presets
├── vcpkg.json               # Dependency manifest
├── vcpkg-configuration.json # vcpkg registry config
├── AGENTS.md                # AI agent context
├── CLAUDE.md                # Claude Code context
├── CONTRIBUTING.md          # Contributor guide
├── CHANGELOG.md             # Release history
├── README.md                # Project readme
├── .clang-format            # Code formatting rules
├── .clang-tidy              # Static analysis config
├── src/
│   ├── CMakeLists.txt       # Source file registration
│   ├── main.cpp             # Application entry point
│   ├── app/                 # MarkAmpApp (2 files)
│   ├── core/                # Core logic (512 h, 383 cpp)
│   │   ├── av/              # Attribute View database (16 h)
│   │   ├── fsrs/            # Flashcard/FSRS engine (9 h)
│   │   └── loader/          # Theme loader (1 h)
│   ├── canvas/              # Infinite canvas (98 h)
│   ├── rendering/           # Rendering & FX (36 h)
│   ├── ui/                  # UI components (88 h)
│   │   └── av/              # AV UI panels (14 h)
│   └── platform/            # Platform abstraction (4 h)
├── tests/
│   ├── CMakeLists.txt       # Test target registration
│   └── unit/                # Catch2 tests (444 files)
├── benchmarks/              # Performance benchmarks (25 files)
├── fuzz/                    # Fuzz testing (6 files)
├── scripts/                 # Automation (23 scripts)
├── themes/                  # Theme definitions (64 files)
├── resources/               # Icons, fonts, assets
├── docs/                    # Documentation (539 files)
├── external/                # vcpkg and external deps
├── packaging/               # Distribution packaging
├── cmake/                   # CMake modules
└── archive/                 # Archived code
```

---

## Module Boundaries

### Core Principles

1. **`src/core/`** must NEVER depend on wxWidgets — keep all logic platform-independent
2. **`src/ui/`** contains wxWidgets-dependent code — wraps core logic for display
3. **`src/canvas/`** contains canvas-specific logic; renderers may import wx
4. **`src/rendering/`** handles document and FX rendering
5. **`src/platform/`** abstracts OS-specific operations

### Dependency Graph

```
Platform ← Core → Rendering
              ↑       ↑
              UI → Canvas
              ↑
            App (entry point)
```

---

## Naming Conventions

| Element           | Convention              | Example                           |
| ----------------- | ----------------------- | --------------------------------- |
| Classes/Structs   | PascalCase              | `EventBus`, `PluginManager`       |
| Functions/Methods | snake_case              | `publish_event()`, `get_config()` |
| Constants/Enums   | kPascalCase             | `kMaxImageSize`, `kOnCommand`     |
| Member Variables  | snake*case + trailing * | `event_bus_`, `config_`           |
| Namespaces        | lower::case             | `markamp::core`                   |
| File Names        | PascalCase.h/.cpp       | `EventBus.h`, `PluginManager.cpp` |
| Test Files        | test_snake_case.cpp     | `test_event_bus.cpp`              |

---

## Code Patterns

### Event Declaration

```cpp
struct MyNewEvent : Event
{
    std::string data;
    [[nodiscard]] auto type_name() const -> std::string override
    {
        return "MyNewEvent";
    }
};
```

### Return Types

Trailing return types are preferred:

```cpp
[[nodiscard]] auto calculate_score() const -> double;
auto process_file(const std::filesystem::path& path) -> std::expected<Document, std::string>;
```

### Error Handling

Use `std::expected<T, std::string>` for fallible operations:

```cpp
auto load_config(const std::filesystem::path& path)
    -> std::expected<Config, std::string>
{
    if (!std::filesystem::exists(path))
        return std::unexpected("Config file not found: " + path.string());
    // ...
}
```

### Header Guards

Always use `#pragma once`:

```cpp
#pragma once
#include <string>
// ...
```

### Include Order

1. Own header
2. Project headers (alphabetically)
3. Third-party headers (alphabetically)
4. Standard library headers (alphabetically)

---

## Build System Details

### Source Registration

All source files are registered in `src/CMakeLists.txt` via `target_sources()`:

```cmake
target_sources(markamp PRIVATE
    core/EventBus.cpp
    core/Config.cpp
    # ...
)
```

### Adding a New Source File

1. Create the `.h` and `.cpp` in the appropriate directory
2. Add the `.cpp` to `target_sources()` in `src/CMakeLists.txt`
3. If the file defines events, add them to `src/core/Events.h`

### Adding a New Test

1. Create `tests/unit/test_<name>.cpp` using Catch2
2. Add the test target in `tests/CMakeLists.txt`:

```cmake
add_executable(test_my_feature test_my_feature.cpp)
target_link_libraries(test_my_feature PRIVATE Catch2::Catch2WithMain markamp_dependencies)
add_test(NAME test_my_feature COMMAND test_my_feature)
```

### Common Build Pitfalls

| Pitfall                        | Solution                                                                                                                                |
| ------------------------------ | --------------------------------------------------------------------------------------------------------------------------------------- |
| Duplicate source in CMakeLists | Some implementations exist in aggregate files (`PlatformServices.cpp`, `TracingService.cpp`) — don't also compile the standalone `.cpp` |
| `-Werror` unused params        | Use `(void)param;` in stub methods                                                                                                      |
| `-Werror` unused fields        | Add `[[maybe_unused]]` attribute                                                                                                        |
| `-Werror` sign conversion      | Use `size_t` for indices, not `int`                                                                                                     |
| wx in tests                    | Exclude wx-dependent sources using GLOB + `list(FILTER EXCLUDE)`                                                                        |
| Linker errors in tests         | Link `markamp_dependencies` for transitive deps                                                                                         |

---

## Observability

### Logging

`spdlog` via `StructuredLogger`:

```cpp
auto logger = StructuredLogger::create("component_name");
logger->info("Processing {} files", count);
logger->error("Failed to load: {}", path.string());
```

### Profiling

Tracy integration via `MARKAMP_ENABLE_TRACY`:

```cpp
#include "core/TracyIntegration.h"
MARKAMP_ZONE_SCOPED;  // Annotate hot functions
```

### Tracing

OpenTelemetry-compatible spans:

```cpp
TraceSpan span("operation_name");
// ... do work ...
span.set_attribute("result", "success");
```

---

## Feature Flags

`FeatureRegistry` provides runtime toggles:

```cpp
registry.register_feature("my_feature", true);  // default enabled
if (registry.is_enabled("my_feature")) {
    // feature code
}
registry.set_enabled("my_feature", false);  // disable at runtime
```

Publishes `FeatureToggledEvent` on state changes.
