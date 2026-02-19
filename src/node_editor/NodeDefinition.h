#pragma once

// V11 Phase 05: Node Definition Registry & Reflection
// Declarative node definitions and a registry for runtime introspection.

#include "node_editor/NodeEditorTypes.h"

#include <string>
#include <vector>

namespace markamp::node_editor
{

// ============================================================================
// Socket Definition
// ============================================================================

/// Declarative definition of a socket within a node type.
struct SocketDefinition
{
    std::string name;
    SocketDirection direction{};
    SocketDataType data_type{};
    std::string default_value;
    std::string description;
};

// ============================================================================
// Node Definition
// ============================================================================

/// Declarative definition of a node type. Registered in NodeDefinitionRegistry.
///
/// Node definitions describe the "blueprint" for a node type: what sockets
/// it has, its category, display properties, etc. Instances of nodes
/// are created from definitions when the user adds a node to the graph.
struct NodeDefinition
{
    std::string type_name;    // Unique identifier, e.g. "math.Add"
    std::string display_name; // User-visible name, e.g. "Add"
    std::string category;     // Category path, e.g. "Math/Arithmetic"
    std::string description;  // Tooltip/help text
    std::string color_hint;   // Hex color for node header, e.g. "#4A90D9"

    std::vector<SocketDefinition> inputs;
    std::vector<SocketDefinition> outputs;

    NodeFlags default_flags{NodeFlags::None};

    /// Whether this node type supports preview thumbnails.
    bool supports_preview{false};
};

} // namespace markamp::node_editor
