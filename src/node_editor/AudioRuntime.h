#pragma once

// V11 Phase 18: Domain Audio Generation Runtime MVP
// Stub audio runtime implementing IDomainRuntime for GraphMode::Audio.

#include "node_editor/IDomainRuntime.h"

namespace markamp::node_editor
{

/// Audio domain runtime stub.
/// Validates contracts and returns placeholder results — real computation deferred to Tier 3+.
class AudioRuntime final : public IDomainRuntime
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
