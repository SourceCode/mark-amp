# Phase 30 -- Multi-Language Kernel Support

## Objective

Extend the KernelManager (Phase 29) with specific support for multiple programming languages: Python, R, Julia, JavaScript/Node, and SQL. Includes kernel-specific configuration, virtual environment detection, language-specific magic commands, and a kernel selector UI. Each language has a KernelAdapter that handles language-specific behavior.

## Prerequisites

- Phase 29 (KernelManager -- core kernel protocol)

## Feature References (PRD)

- PRD Notebook #2: Multi-Language Kernel Support
- PRD Notebook #15: Magic Commands
- PRD Notebook #41: Environment Isolation

## Data Structures to Implement

### File: `src/core/KernelAdapters.h`

```cpp
#pragma once

#include "KernelManager.h"

#include <string>
#include <vector>

namespace markamp::core
{

/// Base interface for language-specific kernel behavior.
class IKernelAdapter
{
public:
    virtual ~IKernelAdapter() = default;

    [[nodiscard]] virtual auto language() const -> std::string = 0;
    [[nodiscard]] virtual auto detect_environments() const -> std::vector<std::string> = 0;
    [[nodiscard]] virtual auto preprocess_code(const std::string& code) const -> std::string = 0;
    [[nodiscard]] virtual auto supported_magic_commands() const -> std::vector<std::string> = 0;
    [[nodiscard]] virtual auto execute_magic(const std::string& magic,
                                              const std::string& args) const -> std::string = 0;
    [[nodiscard]] virtual auto default_kernel_spec() const -> KernelSpec = 0;
};

class PythonAdapter : public IKernelAdapter
{
public:
    [[nodiscard]] auto language() const -> std::string override;
    [[nodiscard]] auto detect_environments() const -> std::vector<std::string> override;
    [[nodiscard]] auto preprocess_code(const std::string& code) const -> std::string override;
    [[nodiscard]] auto supported_magic_commands() const -> std::vector<std::string> override;
    [[nodiscard]] auto execute_magic(const std::string& magic,
                                      const std::string& args) const -> std::string override;
    [[nodiscard]] auto default_kernel_spec() const -> KernelSpec override;

private:
    [[nodiscard]] auto find_python_interpreters() const -> std::vector<std::string>;
    [[nodiscard]] auto find_conda_envs() const -> std::vector<std::string>;
    [[nodiscard]] auto find_venv_dirs() const -> std::vector<std::string>;
};

class RAdapter : public IKernelAdapter { /* similar */ };
class JuliaAdapter : public IKernelAdapter { /* similar */ };
class NodeAdapter : public IKernelAdapter { /* similar */ };

class SqlAdapter : public IKernelAdapter
{
public:
    [[nodiscard]] auto language() const -> std::string override;
    [[nodiscard]] auto detect_environments() const -> std::vector<std::string> override;
    [[nodiscard]] auto preprocess_code(const std::string& code) const -> std::string override;
    [[nodiscard]] auto supported_magic_commands() const -> std::vector<std::string> override;
    [[nodiscard]] auto execute_magic(const std::string& magic,
                                      const std::string& args) const -> std::string override;
    [[nodiscard]] auto default_kernel_spec() const -> KernelSpec override;
};

/// Registry of kernel adapters.
class KernelAdapterRegistry
{
public:
    KernelAdapterRegistry();

    auto register_adapter(std::unique_ptr<IKernelAdapter> adapter) -> void;
    [[nodiscard]] auto get_adapter(const std::string& language) const -> const IKernelAdapter*;
    [[nodiscard]] auto supported_languages() const -> std::vector<std::string>;
    [[nodiscard]] auto all_kernel_specs() const -> std::vector<KernelSpec>;

private:
    std::vector<std::unique_ptr<IKernelAdapter>> adapters_;
};

} // namespace markamp::core
```

## Key Functions to Implement

1. **`PythonAdapter::detect_environments()`** -- Find Python interpreters: system Python, pyenv versions, conda environments, local .venv directories. Return list of paths.

2. **`PythonAdapter::preprocess_code(code)`** -- Handle magic commands: `%time` wraps code in timing block, `%matplotlib inline` sets backend, `%%timeit` wraps in timeit loop. Return preprocessed code.

3. **`PythonAdapter::supported_magic_commands()`** -- Return: `%time`, `%timeit`, `%%timeit`, `%matplotlib`, `%pip`, `%cd`, `%pwd`, `%who`, `%whos`, `%reset`.

4. **`SqlAdapter::preprocess_code(code)`** -- SQL adapter wraps SQL queries in a Python-side execution helper that uses a configured database connection and returns results as a pandas DataFrame.

5. **`KernelAdapterRegistry::all_kernel_specs()`** -- Iterate all adapters, collect their default_kernel_spec() and detected environments.

## Events to Add

```cpp
MARKAMP_DECLARE_EVENT_WITH_FIELDS(KernelLanguageDetectedEvent)
std::string language;
int environment_count{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(MagicCommandExecutedEvent)
std::string command;
std::string result;
MARKAMP_DECLARE_EVENT_END;
```

## Test Cases

File: `tests/unit/test_kernel_adapters.cpp`

1. **Python adapter language** -- Verify language() returns "python".
2. **Python environment detection** -- Mock filesystem. Verify conda/venv/system Python found.
3. **Python magic preprocessing** -- `%time x=1` preprocesses to timing wrapper.
4. **Python pip magic** -- `%pip install pandas` preprocesses to subprocess call.
5. **SQL adapter preprocessing** -- SQL query wraps in Python execution helper.
6. **Adapter registry** -- Register 3 adapters. get_adapter("python") returns Python.
7. **Supported languages** -- supported_languages() returns all registered.
8. **Unknown language** -- get_adapter("unknown") returns nullptr.
9. **All kernel specs** -- 3 adapters, each with 1+ specs. all_kernel_specs() returns union.
10. **Magic command list** -- Python adapter lists at least 5 magic commands.

## Acceptance Criteria

- [ ] Python, R, Julia, Node, SQL adapters implemented
- [ ] Environment detection finds conda, venv, system interpreters
- [ ] Magic commands preprocessed before kernel execution
- [ ] SQL adapter wraps queries for database execution
- [ ] Adapter registry manages all adapters
- [ ] All 10 test cases pass

## Files to Create/Modify

| Action | File | Description |
|--------|------|-------------|
| CREATE | `src/core/KernelAdapters.h` | IKernelAdapter, all adapters, registry |
| CREATE | `src/core/KernelAdapters.cpp` | Full implementation |
| MODIFY | `src/core/KernelManager.cpp` | Use adapter registry for kernel launch |
| MODIFY | `src/core/Events.h` | Add 2 adapter events |
| MODIFY | `src/CMakeLists.txt` | Add KernelAdapters.cpp |
| CREATE | `tests/unit/test_kernel_adapters.cpp` | 10 Catch2 test cases |
| MODIFY | `tests/CMakeLists.txt` | Add test_kernel_adapters target |

## Estimated Complexity

**L** -- 5 adapter implementations, environment scanning, magic command processing, 10 tests.
