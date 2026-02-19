#pragma once

// V11 Phase 19: Domain SDK & Custom Runtime Extensibility
// Extension points for custom domain implementers.

#include "node_editor/IDomainRuntime.h"
#include "node_editor/NodeEditorTypes.h"
#include "node_editor/NodeValue.h"

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::node_editor
{

// Forward declarations
class NodeGraph;
class ExecutionPlan;

// ============================================================================
// Domain Node Handler
// ============================================================================

/// Callback signatures for custom node execution and validation.
using NodeExecuteCallback =
    std::function<ExecutionResult(const NodeGraph&, NodeId, CancellationToken&)>;

using NodeValidateCallback = std::function<bool(const NodeGraph&, NodeId)>;

using NodePreviewCallback = std::function<std::string(const NodeGraph&, NodeId)>;

/// Bundle of callbacks for a custom node type within a domain.
struct DomainNodeHandler
{
    std::string type_name;
    NodeExecuteCallback on_execute;
    NodeValidateCallback on_validate;
    NodePreviewCallback on_preview; // Optional
};

// ============================================================================
// Domain SDK
// ============================================================================

/// SDK for registering custom node handlers within existing domain runtimes.
///
/// This allows extensions to add new node types to an existing domain
/// without modifying the runtime itself.
class DomainSDK
{
public:
    DomainSDK() = default;

    /// Register a custom node handler for a given node type_name.
    /// Returns false if a handler is already registered for that type.
    auto register_handler(DomainNodeHandler handler) -> bool;

    /// Find a handler by type_name. Returns nullptr if not found.
    [[nodiscard]] auto find_handler(const std::string& type_name) const -> const DomainNodeHandler*;

    /// List all registered handler type names.
    [[nodiscard]] auto all_handler_types() const -> std::vector<std::string>;

    /// Get the number of registered handlers.
    [[nodiscard]] auto handler_count() const -> std::size_t;

    /// Remove a handler by type_name. Returns false if not found.
    auto remove_handler(const std::string& type_name) -> bool;

    /// Clear all handlers.
    void clear();

private:
    std::unordered_map<std::string, DomainNodeHandler> handlers_;
};

} // namespace markamp::node_editor
