#pragma once

// V11 Phase 15: Domain CodeFlow Runtime MVP
// Stub code/dataflow runtime implementing IDomainRuntime for GraphMode::CodeFlow.

#include "node_editor/IDomainRuntime.h"

namespace markamp::node_editor
{

/// CodeFlow domain runtime stub.
/// Validates contracts and returns placeholder results — real computation deferred to Tier 3+.
class CodeFlowRuntime final : public IDomainRuntime
{
public:
    [[nodiscard]] auto domain_id() const -> std::string override;
    [[nodiscard]] auto graph_mode() const -> GraphMode override;
    [[nodiscard]] auto capabilities() const -> DomainCapabilities override;
    auto execute(const NodeGraph& graph, const ExecutionPlan& plan, CancellationToken& token)
        -> ExecutionResult override;
    [[nodiscard]] auto validate(const NodeGraph& graph) const -> bool override;
    [[nodiscard]] auto display_name() const -> std::string override;
    [[nodiscard]] auto supported_node_types() const -> std::vector<std::string> override;
};

} // namespace markamp::node_editor
