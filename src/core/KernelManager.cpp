/// @file KernelManager.cpp
/// @brief V4 Phase 29 – Code Cell Execution Engine implementation.
/// Data structures and lifecycle management. ZeroMQ process spawning is stubbed.

#include "core/KernelManager.h"

#include "core/Config.h"
#include "core/EventBus.h"
#include "core/Events.h"

#include <algorithm>
#include <chrono>
#include <random>
#include <sstream>

namespace markamp::core
{

// ============================================================================
// MimeBundle
// ============================================================================

auto MimeBundle::has(const std::string& mime_type) const -> bool
{
    return data.count(mime_type) > 0;
}

auto MimeBundle::get(const std::string& mime_type) const -> std::string
{
    auto iter = data.find(mime_type);
    if (iter == data.end())
    {
        return "";
    }
    return iter->second;
}

auto MimeBundle::best_format() const -> std::string
{
    // Priority order: text/html > image/png > image/svg+xml > application/json > text/plain
    static const std::vector<std::string> priority = {
        "text/html", "image/png", "image/svg+xml", "application/json", "text/plain"};

    for (const auto& mime : priority)
    {
        if (data.count(mime) > 0)
        {
            return mime;
        }
    }

    // Return first available.
    if (!data.empty())
    {
        return data.begin()->first;
    }
    return "";
}

// ============================================================================
// Constructor / Destructor
// ============================================================================

KernelManager::KernelManager(EventBus& event_bus, Config& config)
    : event_bus_(event_bus)
    , config_(config)
{
}

KernelManager::~KernelManager()
{
    // Stop all kernels on destruction.
    std::lock_guard<std::mutex> lock(kernels_mutex_);
    for (auto& [kernel_id, info] : kernels_)
    {
        info.state = KernelState::kDead;
    }
}

// ============================================================================
// ID generation
// ============================================================================

auto KernelManager::generate_kernel_id() const -> std::string
{
    auto now = std::chrono::steady_clock::now().time_since_epoch().count();
    std::mt19937_64 rng(static_cast<uint64_t>(now));
    std::uniform_int_distribution<uint64_t> dist;

    std::ostringstream oss;
    oss << std::hex << dist(rng) << "-" << dist(rng);
    return "kernel-" + oss.str();
}

auto KernelManager::generate_message_id() const -> std::string
{
    auto now = std::chrono::steady_clock::now().time_since_epoch().count();
    std::mt19937_64 rng(static_cast<uint64_t>(now) ^ 0xCAFEBABE);
    std::uniform_int_distribution<uint64_t> dist;

    std::ostringstream oss;
    oss << std::hex << dist(rng) << "-" << dist(rng);
    return "msg-" + oss.str();
}

// ============================================================================
// Discover kernels
// ============================================================================

auto KernelManager::discover_kernels() const -> std::vector<KernelSpec>
{
    std::vector<KernelSpec> specs;

    // Built-in specs for common languages.
    KernelSpec python_spec;
    python_spec.name = "python3";
    python_spec.display_name = "Python 3";
    python_spec.language = "python";
    python_spec.executable = "python3";
    python_spec.argv = {"-m", "ipykernel_launcher", "-f", "{connection_file}"};
    specs.push_back(std::move(python_spec));

    KernelSpec r_spec;
    r_spec.name = "ir";
    r_spec.display_name = "R";
    r_spec.language = "r";
    r_spec.executable = "R";
    r_spec.argv = {"--slave", "-e", "IRkernel::main()", "--args", "{connection_file}"};
    specs.push_back(std::move(r_spec));

    KernelSpec julia_spec;
    julia_spec.name = "julia-1.10";
    julia_spec.display_name = "Julia 1.10";
    julia_spec.language = "julia";
    julia_spec.executable = "julia";
    julia_spec.argv = {"-i", "--startup-file=yes", "{connection_file}"};
    specs.push_back(std::move(julia_spec));

    KernelSpec node_spec;
    node_spec.name = "javascript";
    node_spec.display_name = "Node.js";
    node_spec.language = "javascript";
    node_spec.executable = "node";
    node_spec.argv = {};
    specs.push_back(std::move(node_spec));

    return specs;
}

// ============================================================================
// Kernel lifecycle
// ============================================================================

auto KernelManager::start_kernel(const KernelSpec& spec) -> std::expected<std::string, std::string>
{
    std::lock_guard<std::mutex> lock(kernels_mutex_);

    auto kernel_id = generate_kernel_id();

    KernelInfo info;
    info.id = kernel_id;
    info.spec = spec;
    info.state = KernelState::kIdle;
    info.started_at = std::chrono::system_clock::now();

    kernels_[kernel_id] = std::move(info);

    events::KernelStartedEvent started_event;
    started_event.kernel_id = kernel_id;
    started_event.language = spec.language;
    event_bus_.publish(started_event);

    return kernel_id;
}

auto KernelManager::stop_kernel(const std::string& kernel_id) -> std::expected<void, std::string>
{
    std::lock_guard<std::mutex> lock(kernels_mutex_);

    auto iter = kernels_.find(kernel_id);
    if (iter == kernels_.end())
    {
        return std::unexpected("Kernel not found: " + kernel_id);
    }

    iter->second.state = KernelState::kDead;
    events::KernelStoppedEvent stopped_event;
    stopped_event.kernel_id = kernel_id;
    event_bus_.publish(stopped_event);

    kernels_.erase(iter);
    return {};
}

auto KernelManager::restart_kernel(const std::string& kernel_id) -> std::expected<void, std::string>
{
    std::lock_guard<std::mutex> lock(kernels_mutex_);

    auto iter = kernels_.find(kernel_id);
    if (iter == kernels_.end())
    {
        return std::unexpected("Kernel not found: " + kernel_id);
    }

    iter->second.state = KernelState::kRestarting;
    iter->second.execution_count = 0;

    // Simulated restart — immediately go to idle.
    iter->second.state = KernelState::kIdle;

    return {};
}

auto KernelManager::interrupt_kernel(const std::string& kernel_id)
    -> std::expected<void, std::string>
{
    std::lock_guard<std::mutex> lock(kernels_mutex_);

    auto iter = kernels_.find(kernel_id);
    if (iter == kernels_.end())
    {
        return std::unexpected("Kernel not found: " + kernel_id);
    }

    if (iter->second.state == KernelState::kBusy)
    {
        iter->second.state = KernelState::kIdle;
    }

    return {};
}

// ============================================================================
// Execution
// ============================================================================

auto KernelManager::execute(const std::string& kernel_id, const ExecutionRequest& request)
    -> std::expected<std::string, std::string>
{
    std::lock_guard<std::mutex> lock(kernels_mutex_);

    auto iter = kernels_.find(kernel_id);
    if (iter == kernels_.end())
    {
        return std::unexpected("Kernel not found: " + kernel_id);
    }

    if (iter->second.state != KernelState::kIdle)
    {
        return std::unexpected("Kernel is not idle (state: " +
                               std::to_string(static_cast<int>(iter->second.state)) + ")");
    }

    iter->second.state = KernelState::kBusy;
    ++iter->second.execution_count;

    auto message_id = generate_message_id();

    // In a real implementation, this would send the request to the kernel via ZeroMQ.
    // For the testable layer, we publish the event and set state back to idle.
    iter->second.state = KernelState::kIdle;

    events::CellExecutionCompletedEvent exec_event;
    exec_event.cell_id = request.cell_id;
    exec_event.kernel_id = kernel_id;
    exec_event.success = true;
    exec_event.elapsed_ms = 0.0;
    event_bus_.publish(exec_event);

    return message_id;
}

// ============================================================================
// Queries
// ============================================================================

auto KernelManager::get_kernel_info(const std::string& kernel_id) const -> std::optional<KernelInfo>
{
    std::lock_guard<std::mutex> lock(kernels_mutex_);

    auto iter = kernels_.find(kernel_id);
    if (iter == kernels_.end())
    {
        return std::nullopt;
    }
    return iter->second;
}

auto KernelManager::active_kernels() const -> std::vector<KernelInfo>
{
    std::lock_guard<std::mutex> lock(kernels_mutex_);

    std::vector<KernelInfo> result;
    result.reserve(kernels_.size());
    for (const auto& [kernel_id, info] : kernels_)
    {
        if (info.state != KernelState::kDead)
        {
            result.push_back(info);
        }
    }
    return result;
}

auto KernelManager::kernel_state(const std::string& kernel_id) const -> std::optional<KernelState>
{
    std::lock_guard<std::mutex> lock(kernels_mutex_);

    auto iter = kernels_.find(kernel_id);
    if (iter == kernels_.end())
    {
        return std::nullopt;
    }
    return iter->second.state;
}

// ============================================================================
// Callbacks
// ============================================================================

auto KernelManager::on_result(const std::string& message_id,
                              std::function<void(const ExecutionResult&)> callback) -> void
{
    result_callbacks_[message_id] = std::move(callback);
}

} // namespace markamp::core
