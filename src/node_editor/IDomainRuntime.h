#pragma once

// V11 Phase 11: Domain Abstraction Layer & Runtime Contracts
// Abstract runtime interface for domain-agnostic graph execution.

#include "node_editor/NodeEditorTypes.h"

#include <chrono>
#include <functional>
#include <string>
#include <variant>
#include <vector>

namespace markamp::node_editor
{

// Forward declarations
class NodeGraph;
class ExecutionPlan;

// ============================================================================
// Cancellation Token
// ============================================================================

/// Cooperative cancellation flag for long-running operations.
class CancellationToken
{
public:
    CancellationToken() = default;

    void cancel()
    {
        cancelled_ = true;
    }
    [[nodiscard]] auto is_cancelled() const -> bool
    {
        return cancelled_;
    }
    void reset()
    {
        cancelled_ = false;
    }

private:
    bool cancelled_{false};
};

// ============================================================================
// Domain Capabilities
// ============================================================================

/// Feature flags describing what a domain runtime supports.
struct DomainCapabilities
{
    bool supports_async{false};
    bool supports_preview{false};
    bool supports_caching{false};
    bool supports_streaming{false};
    bool supports_debug{false};
    bool supports_cancellation{true};
};

// ============================================================================
// Execution Result
// ============================================================================

/// Outcome of a single node or graph execution.
struct ExecutionResult
{
    bool success{false};
    std::string error_message;
    int nodes_evaluated{0};
    std::chrono::microseconds elapsed{0};
};

// ============================================================================
// IDomainRuntime
// ============================================================================

/// Abstract interface for domain-specific graph execution.
///
/// Each GraphMode (Graphics, CodeFlow, DataTransform, etc.) has a corresponding
/// IDomainRuntime implementation that knows how to evaluate nodes of that domain.
class IDomainRuntime
{
public:
    IDomainRuntime() = default;
    virtual ~IDomainRuntime() = default;

    // Non-copyable, non-movable
    IDomainRuntime(const IDomainRuntime&) = delete;
    auto operator=(const IDomainRuntime&) -> IDomainRuntime& = delete;
    IDomainRuntime(IDomainRuntime&&) = delete;
    auto operator=(IDomainRuntime&&) -> IDomainRuntime& = delete;

    /// Unique string identifier for this domain (e.g., "graphics", "codeflow").
    [[nodiscard]] virtual auto domain_id() const -> std::string = 0;

    /// The GraphMode this runtime handles.
    [[nodiscard]] virtual auto graph_mode() const -> GraphMode = 0;

    /// Feature capability flags.
    [[nodiscard]] virtual auto capabilities() const -> DomainCapabilities = 0;

    /// Execute a graph using the given execution plan.
    virtual auto execute(const NodeGraph& graph,
                         const ExecutionPlan& plan,
                         CancellationToken& token) -> ExecutionResult = 0;

    /// Validate that a graph is compatible with this runtime.
    [[nodiscard]] virtual auto validate(const NodeGraph& graph) const -> bool = 0;

    /// Get the display name for this runtime.
    [[nodiscard]] virtual auto display_name() const -> std::string = 0;

    /// Get the set of node type_names this runtime can handle.
    [[nodiscard]] virtual auto supported_node_types() const -> std::vector<std::string> = 0;
};

} // namespace markamp::node_editor
