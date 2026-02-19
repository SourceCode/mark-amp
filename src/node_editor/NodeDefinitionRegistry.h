#pragma once

// V11 Phase 05: Node Definition Registry & Reflection
// Thread-safe registry for node type definitions with lookup and introspection.

#include "node_editor/NodeDefinition.h"

#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::node_editor
{

/// Central registry for node type definitions.
///
/// All node definitions must be registered before they can be instantiated.
/// Thread-safe: reads and writes are protected by a mutex.
class NodeDefinitionRegistry
{
public:
    NodeDefinitionRegistry() = default;

    /// Register a node type definition. Returns false if type_name already exists.
    auto register_definition(NodeDefinition definition) -> bool;

    /// Look up a definition by type_name. Returns nullptr if not found.
    [[nodiscard]] auto find(const std::string& type_name) const -> const NodeDefinition*;

    /// Get all registered definitions.
    [[nodiscard]] auto all() const -> std::vector<const NodeDefinition*>;

    /// Get definitions filtered by category prefix.
    [[nodiscard]] auto by_category(const std::string& category_prefix) const
        -> std::vector<const NodeDefinition*>;

    /// Number of registered definitions.
    [[nodiscard]] auto count() const -> std::size_t;

    /// Check if a type_name is registered.
    [[nodiscard]] auto contains(const std::string& type_name) const -> bool;

    /// Remove all registrations.
    void clear();

private:
    mutable std::mutex mutex_;
    std::unordered_map<std::string, NodeDefinition> definitions_;
};

} // namespace markamp::node_editor
