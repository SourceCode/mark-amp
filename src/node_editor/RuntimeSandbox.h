#pragma once

// V11 Phase 20: Runtime Sandboxing, Resource Quotas & Safety
// Execution guards, cancellation, quotas, and safety controls.

#include "node_editor/IDomainRuntime.h"
#include "node_editor/NodeEditorTypes.h"

#include <chrono>
#include <string>

namespace markamp::node_editor
{

// Forward declarations
class NodeGraph;
class ExecutionPlan;

// ============================================================================
// Resource Quota
// ============================================================================

/// Configurable resource limits for sandboxed execution.
struct ResourceQuota
{
    std::chrono::milliseconds max_execution_time{5000};
    std::size_t max_memory_bytes{256UL * 1024UL * 1024UL}; // 256 MB
    int max_work_units{10000};
    int max_node_count{1000};
};

// ============================================================================
// Sandbox Violation
// ============================================================================

/// Types of resource limit violations.
enum class SandboxViolation : uint8_t
{
    None,
    TimeoutExceeded,
    MemoryExceeded,
    WorkUnitsExceeded,
    NodeCountExceeded,
    Cancelled
};

/// Convert SandboxViolation to a display-friendly string.
[[nodiscard]] constexpr auto sandbox_violation_name(SandboxViolation violation) noexcept
    -> std::string_view
{
    switch (violation)
    {
        case SandboxViolation::None:
            return "None";
        case SandboxViolation::TimeoutExceeded:
            return "TimeoutExceeded";
        case SandboxViolation::MemoryExceeded:
            return "MemoryExceeded";
        case SandboxViolation::WorkUnitsExceeded:
            return "WorkUnitsExceeded";
        case SandboxViolation::NodeCountExceeded:
            return "NodeCountExceeded";
        case SandboxViolation::Cancelled:
            return "Cancelled";
    }
    return "Unknown";
}

// ============================================================================
// Sandbox Result
// ============================================================================

/// Outcome of a sandboxed execution.
struct SandboxResult
{
    ExecutionResult execution_result;
    SandboxViolation violation{SandboxViolation::None};
    std::string violation_message;
};

// ============================================================================
// Runtime Sandbox
// ============================================================================

/// Wraps an IDomainRuntime with resource quota enforcement.
///
/// Pre-validates resource limits before execution and monitors
/// cancellation during execution. If limits are exceeded, execution
/// is halted and a violation is reported.
class RuntimeSandbox
{
public:
    explicit RuntimeSandbox(ResourceQuota quota = {});

    /// Execute with quota enforcement.
    auto execute(IDomainRuntime& runtime,
                 const NodeGraph& graph,
                 const ExecutionPlan& plan,
                 CancellationToken& token) -> SandboxResult;

    /// Get the current quota configuration.
    [[nodiscard]] auto quota() const -> const ResourceQuota&;

    /// Update the quota configuration.
    void set_quota(ResourceQuota new_quota);

    /// Pre-validate that a graph meets the sandbox constraints
    /// (e.g., node count limit) without executing.
    [[nodiscard]] auto pre_validate(const NodeGraph& graph) const -> SandboxViolation;

private:
    ResourceQuota quota_;
};

} // namespace markamp::node_editor
