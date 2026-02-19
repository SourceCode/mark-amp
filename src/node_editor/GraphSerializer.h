#pragma once

// V11 Phase 06: Node Graph Serialization & Version Migrations
// JSON-based graph serialization with schema versioning.

#include "node_editor/NodeGraph.h"

#include <string>

namespace markamp::node_editor
{

/// JSON-based serialization/deserialization for NodeGraph.
///
/// The serialized format includes a schema_version field for forward
/// compatibility. Migration logic will be added in later phases.
class GraphSerializer
{
public:
    /// Serialize a NodeGraph to a JSON string.
    [[nodiscard]] static auto serialize(const NodeGraph& graph) -> std::string;

    /// Deserialize a JSON string into a NodeGraph.
    /// Returns a default-constructed graph on parse failure.
    [[nodiscard]] static auto deserialize(const std::string& json_str) -> NodeGraph;

    /// Get the schema version from a JSON string without full parse.
    [[nodiscard]] static auto schema_version(const std::string& json_str) -> int;
};

} // namespace markamp::node_editor
