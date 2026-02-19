#pragma once

// V11 Phase 13: Typed Value Runtime & Conversion Engine
// Conversion rules engine for type coercion between socket data types.

#include "node_editor/NodeEditorTypes.h"
#include "node_editor/NodeValue.h"

#include <optional>

namespace markamp::node_editor
{

// ============================================================================
// Value Converter
// ============================================================================

/// Static conversion rules engine between SocketDataType pairs.
///
/// Mirrors the compatibility rules in NodeGraph::are_types_compatible()
/// but performs actual value conversion rather than just checking feasibility.
class ValueConverter
{
public:
    /// Check if conversion is possible between two types.
    [[nodiscard]] static auto can_convert(SocketDataType from, SocketDataType to) -> bool;

    /// Attempt to convert a value to a target type.
    /// Returns nullopt if conversion is not supported.
    [[nodiscard]] static auto convert(const NodeValue& value, SocketDataType target_type)
        -> std::optional<NodeValue>;
};

} // namespace markamp::node_editor
