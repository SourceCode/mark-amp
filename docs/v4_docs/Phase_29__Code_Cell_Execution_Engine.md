# Phase 29 -- Code Cell Execution Engine and Kernel Protocol

## Objective

Implement the core code execution infrastructure: a KernelManager that spawns and communicates with language kernel processes using a ZeroMQ-based protocol (compatible with Jupyter's messaging specification). Supports starting/stopping kernels, sending code execution requests, receiving output messages, and managing kernel state. This is the foundation for all notebook/code execution features.

## Prerequisites

- Existing EventBus, Config
- ZeroMQ library (new dependency)
- nlohmann/json (for message serialization)

## Feature References (PRD)

- PRD Notebook #1: Executable Code Cells
- PRD Notebook #2: Multi-Language Kernel Support
- PRD Notebook #7: Stateful Execution Model

## Data Structures to Implement

### File: `src/core/KernelManager.h`

```cpp
#pragma once

#include <atomic>
#include <chrono>
#include <expected>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

class EventBus;
class Config;

enum class KernelState : uint8_t
{
    Starting,
    Idle,
    Busy,
    Restarting,
    Dead
};

enum class MessageType : uint8_t
{
    ExecuteRequest,
    ExecuteReply,
    ExecuteResult,
    Stream,            // stdout/stderr
    DisplayData,
    UpdateDisplayData,
    Error,
    Status,
    KernelInfoRequest,
    KernelInfoReply,
    CompleteRequest,
    CompleteReply,
    InspectRequest,
    InspectReply,
    InterruptRequest,
    ShutdownRequest,
    ShutdownReply
};

struct MimeBundle
{
    std::unordered_map<std::string, std::string> data;  // mime_type -> content
    // Common: "text/plain", "text/html", "image/png" (base64), "application/json"

    [[nodiscard]] auto has(const std::string& mime_type) const -> bool;
    [[nodiscard]] auto get(const std::string& mime_type) const -> std::string;
    [[nodiscard]] auto best_format() const -> std::string;  // Prefer html > png > plain
};

struct ExecutionRequest
{
    std::string code;
    std::string cell_id;
    bool silent{false};
    bool store_history{true};
    bool allow_stdin{false};
};

struct ExecutionResult
{
    std::string cell_id;
    int execution_count{0};
    MimeBundle output;
    std::string stream_stdout;
    std::string stream_stderr;
    std::string error_name;
    std::string error_value;
    std::vector<std::string> traceback;
    bool success{false};
    double elapsed_ms{0.0};
};

struct KernelSpec
{
    std::string name;            // "python3", "r", "julia"
    std::string display_name;    // "Python 3.11"
    std::string language;        // "python"
    std::string executable;      // Path to kernel executable
    std::vector<std::string> argv;  // Command line arguments
    std::string env_path;        // Virtual environment path
};

struct KernelInfo
{
    std::string id;              // Unique kernel instance ID
    KernelSpec spec;
    KernelState state{KernelState::Starting};
    int execution_count{0};
    std::chrono::system_clock::time_point started_at;
    int pid{0};                  // OS process ID
};

class KernelManager
{
public:
    KernelManager(EventBus& event_bus, Config& config);
    ~KernelManager();

    /// Discover available kernel specs on the system.
    [[nodiscard]] auto discover_kernels() const -> std::vector<KernelSpec>;

    /// Start a kernel with the given spec.
    [[nodiscard]] auto start_kernel(const KernelSpec& spec)
        -> std::expected<std::string, std::string>;  // Returns kernel ID

    /// Stop a running kernel.
    [[nodiscard]] auto stop_kernel(const std::string& kernel_id)
        -> std::expected<void, std::string>;

    /// Restart a kernel (preserves connections, clears state).
    [[nodiscard]] auto restart_kernel(const std::string& kernel_id)
        -> std::expected<void, std::string>;

    /// Interrupt a running execution.
    [[nodiscard]] auto interrupt_kernel(const std::string& kernel_id)
        -> std::expected<void, std::string>;

    /// Execute code on a kernel.
    [[nodiscard]] auto execute(const std::string& kernel_id,
                                const ExecutionRequest& request)
        -> std::expected<std::string, std::string>;  // Returns message ID

    /// Request code completion.
    [[nodiscard]] auto complete(const std::string& kernel_id,
                                 const std::string& code,
                                 int cursor_pos) const
        -> std::expected<std::vector<std::string>, std::string>;

    /// Request variable inspection.
    [[nodiscard]] auto inspect(const std::string& kernel_id,
                                const std::string& code,
                                int cursor_pos) const
        -> std::expected<MimeBundle, std::string>;

    /// Get kernel info.
    [[nodiscard]] auto get_kernel_info(const std::string& kernel_id) const
        -> std::optional<KernelInfo>;

    /// Get all active kernels.
    [[nodiscard]] auto active_kernels() const -> std::vector<KernelInfo>;

    /// Register a callback for execution results.
    auto on_result(const std::string& message_id,
                    std::function<void(const ExecutionResult&)> callback) -> void;

private:
    EventBus& event_bus_;
    Config& config_;

    struct KernelProcess;
    std::unordered_map<std::string, std::unique_ptr<KernelProcess>> kernels_;
    mutable std::mutex kernels_mutex_;

    std::unordered_map<std::string, std::function<void(const ExecutionResult&)>> result_callbacks_;

    auto spawn_kernel_process(const KernelSpec& spec) -> std::expected<std::unique_ptr<KernelProcess>, std::string>;
    auto setup_zmq_channels(KernelProcess& kernel) -> std::expected<void, std::string>;
    auto io_loop(const std::string& kernel_id) -> void;
    auto handle_message(const std::string& kernel_id, MessageType type,
                         const std::string& content) -> void;
    auto discover_jupyter_kernels() const -> std::vector<KernelSpec>;
    auto discover_system_interpreters() const -> std::vector<KernelSpec>;

    [[nodiscard]] auto generate_kernel_id() const -> std::string;
};

} // namespace markamp::core
```

## Key Functions to Implement

1. **`discover_kernels()`** -- Scan standard Jupyter kernel directories (`~/.local/share/jupyter/kernels/`, `/usr/share/jupyter/kernels/`). Parse `kernel.json` files. Also detect system Python/R/Julia interpreters.

2. **`start_kernel(spec)`** -- Generate kernel ID. Spawn subprocess with kernel command. Set up ZeroMQ sockets (shell, iopub, stdin, control, heartbeat). Start I/O thread for receiving messages. Publish KernelStartedEvent.

3. **`execute(kernel_id, request)`** -- Build execute_request message per Jupyter protocol. Send on shell channel. Generate message ID. Store callback. Return message ID for tracking.

4. **`io_loop(kernel_id)`** -- Run on dedicated thread. Poll iopub channel for messages. Parse message type and content. Dispatch to handle_message(). Continue until kernel stops.

5. **`handle_message(kernel_id, type, content)`** -- For execute_result: build ExecutionResult, call registered callback. For stream: append to stdout/stderr. For display_data: build MimeBundle. For error: capture traceback. For status: update kernel state.

6. **`interrupt_kernel(kernel_id)`** -- Send interrupt signal (SIGINT on Unix, ctrl-break on Windows) to kernel process.

7. **`complete(kernel_id, code, cursor_pos)`** -- Send complete_request. Wait for reply. Return list of completion suggestions.

## Events to Add

```cpp
MARKAMP_DECLARE_EVENT_WITH_FIELDS(KernelStartedEvent)
std::string kernel_id;
std::string language;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(KernelStoppedEvent)
std::string kernel_id;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(KernelStateChangedEvent)
std::string kernel_id;
KernelState state;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(CellExecutionCompletedEvent)
std::string cell_id;
std::string kernel_id;
bool success{false};
double elapsed_ms{0.0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(CellOutputEvent)
std::string cell_id;
std::string kernel_id;
std::string mime_type;
std::string content;
MARKAMP_DECLARE_EVENT_END;
```

## Test Cases

File: `tests/unit/test_kernel_manager.cpp`

1. **Discover kernels** -- Verify at least one kernel found (Python if available).
2. **Kernel spec parsing** -- Parse a mock kernel.json. Verify fields.
3. **MimeBundle operations** -- Create bundle with text/plain and text/html. Verify has(), get(), best_format().
4. **ExecutionRequest construction** -- Build request with code. Verify fields.
5. **ExecutionResult success** -- Build successful result. Verify success=true.
6. **ExecutionResult error** -- Build error result with traceback. Verify fields.
7. **Message ID generation** -- Generate 100 IDs. Verify all unique.
8. **Kernel state transitions** -- Starting -> Idle -> Busy -> Idle -> Dead. Verify valid transitions.
9. **Kernel info retrieval** -- Start kernel (mock). Get info. Verify fields.
10. **Active kernels listing** -- Start 2 kernels. Verify active_kernels() returns 2.

## Acceptance Criteria

- [ ] Jupyter kernel discovery finds installed kernels
- [ ] Kernel processes spawn and connect via ZeroMQ
- [ ] Execute requests send code and receive results
- [ ] MimeBundle supports text, HTML, and image outputs
- [ ] Kernel state transitions are tracked and published
- [ ] Interrupt stops a running execution
- [ ] Code completion returns suggestions from kernel
- [ ] All 10 test cases pass

## Files to Create/Modify

| Action | File | Description |
|--------|------|-------------|
| CREATE | `src/core/KernelManager.h` | KernelManager, all kernel types |
| CREATE | `src/core/KernelManager.cpp` | Full implementation |
| MODIFY | `src/core/Events.h` | Add 5 kernel events |
| MODIFY | `src/core/PluginContext.h` | Add `KernelManager* kernel_manager{nullptr};` |
| MODIFY | `src/CMakeLists.txt` | Add KernelManager.cpp, link ZeroMQ |
| MODIFY | `CMakeLists.txt` | Add ZeroMQ dependency via FetchContent or find_package |
| CREATE | `tests/unit/test_kernel_manager.cpp` | 10 Catch2 test cases |
| MODIFY | `tests/CMakeLists.txt` | Add test_kernel_manager target |

## Architecture Notes

- ZeroMQ is the new external dependency (cppzmq header-only wrapper)
- Jupyter messaging protocol v5.3 is the target compatibility
- Each kernel has 5 ZeroMQ channels: shell, iopub, stdin, control, heartbeat
- I/O loop runs on a dedicated thread per kernel
- Constructor injection: KernelManager(EventBus&, Config&)

## Estimated Complexity

**XL** -- ZeroMQ integration, Jupyter protocol, subprocess management, multi-threaded I/O, 10 tests.
