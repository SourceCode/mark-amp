/// @file KernelManager.h
/// @brief V4 Phase 29 – Code Cell Execution Engine.
/// Kernel management infrastructure: data structures for execution requests/results,
/// MIME bundles, kernel specs, kernel state machine, and the manager API.
/// ZeroMQ-dependent process spawning is stubbed for testability.

#pragma once

#include <chrono>
#include <cstdint>
#include <expected>
#include <functional>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

class EventBus;
class Config;

/// Kernel lifecycle states.
enum class KernelState : uint8_t
{
    kStarting,
    kIdle,
    kBusy,
    kRestarting,
    kDead
};

/// Jupyter-compatible message types.
enum class MessageType : uint8_t
{
    kExecuteRequest,
    kExecuteReply,
    kExecuteResult,
    kStream,
    kDisplayData,
    kUpdateDisplayData,
    kError,
    kStatus,
    kKernelInfoRequest,
    kKernelInfoReply,
    kCompleteRequest,
    kCompleteReply,
    kInspectRequest,
    kInspectReply,
    kInterruptRequest,
    kShutdownRequest,
    kShutdownReply
};

/// A collection of output data keyed by MIME type.
struct MimeBundle
{
    std::unordered_map<std::string, std::string> data;

    [[nodiscard]] auto has(const std::string& mime_type) const -> bool;
    [[nodiscard]] auto get(const std::string& mime_type) const -> std::string;

    /// Return the best available format (prefer html > png > plain).
    [[nodiscard]] auto best_format() const -> std::string;
};

/// A request to execute code on a kernel.
struct ExecutionRequest
{
    std::string code;
    std::string cell_id;
    bool silent{false};
    bool store_history{true};
    bool allow_stdin{false};
};

/// The result of a code execution.
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

/// Specification for a language kernel.
struct KernelSpec
{
    std::string name;              ///< e.g., "python3", "r", "julia"
    std::string display_name;      ///< e.g., "Python 3.11"
    std::string language;          ///< e.g., "python"
    std::string executable;        ///< Path to kernel executable
    std::vector<std::string> argv; ///< Command line arguments
    std::string env_path;          ///< Virtual environment path
};

/// Runtime information about a kernel instance.
struct KernelInfo
{
    std::string id;
    KernelSpec spec;
    KernelState state{KernelState::kStarting};
    int execution_count{0};
    std::chrono::system_clock::time_point started_at;
    int pid{0};
};

/// Manages kernel instances and code execution.
class KernelManager
{
public:
    KernelManager(EventBus& event_bus, Config& config);
    ~KernelManager();

    /// Discover available kernel specs on the system.
    [[nodiscard]] auto discover_kernels() const -> std::vector<KernelSpec>;

    /// Start a kernel with the given spec. Returns kernel ID.
    [[nodiscard]] auto start_kernel(const KernelSpec& spec)
        -> std::expected<std::string, std::string>;

    /// Stop a running kernel.
    [[nodiscard]] auto stop_kernel(const std::string& kernel_id)
        -> std::expected<void, std::string>;

    /// Restart a kernel.
    [[nodiscard]] auto restart_kernel(const std::string& kernel_id)
        -> std::expected<void, std::string>;

    /// Interrupt a running execution.
    [[nodiscard]] auto interrupt_kernel(const std::string& kernel_id)
        -> std::expected<void, std::string>;

    /// Execute code on a kernel. Returns message ID.
    [[nodiscard]] auto execute(const std::string& kernel_id, const ExecutionRequest& request)
        -> std::expected<std::string, std::string>;

    /// Get kernel info.
    [[nodiscard]] auto get_kernel_info(const std::string& kernel_id) const
        -> std::optional<KernelInfo>;

    /// Get all active kernels.
    [[nodiscard]] auto active_kernels() const -> std::vector<KernelInfo>;

    /// Register a callback for execution results.
    auto on_result(const std::string& message_id,
                   std::function<void(const ExecutionResult&)> callback) -> void;

    /// Get kernel state.
    [[nodiscard]] auto kernel_state(const std::string& kernel_id) const
        -> std::optional<KernelState>;

private:
    EventBus& event_bus_;
    Config& config_;

    std::unordered_map<std::string, KernelInfo> kernels_;
    mutable std::mutex kernels_mutex_;

    std::unordered_map<std::string, std::function<void(const ExecutionResult&)>> result_callbacks_;

    int next_execution_count_{1};

    /// Generate a unique kernel ID.
    [[nodiscard]] auto generate_kernel_id() const -> std::string;

    /// Generate a unique message ID.
    [[nodiscard]] auto generate_message_id() const -> std::string;
};

} // namespace markamp::core
