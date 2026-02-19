// V11 Phase 05: Node Definition Registry & Reflection
// Implementation of NodeDefinitionRegistry.

#include "node_editor/NodeDefinitionRegistry.h"

#include <algorithm>

namespace markamp::node_editor
{

auto NodeDefinitionRegistry::register_definition(NodeDefinition definition) -> bool
{
    const std::lock_guard lock(mutex_);
    auto [iter, inserted] = definitions_.try_emplace(definition.type_name, std::move(definition));
    return inserted;
}

auto NodeDefinitionRegistry::find(const std::string& type_name) const -> const NodeDefinition*
{
    const std::lock_guard lock(mutex_);
    auto iter = definitions_.find(type_name);
    return iter != definitions_.end() ? &iter->second : nullptr;
}

auto NodeDefinitionRegistry::all() const -> std::vector<const NodeDefinition*>
{
    const std::lock_guard lock(mutex_);
    std::vector<const NodeDefinition*> result;
    result.reserve(definitions_.size());
    for (const auto& [name, def] : definitions_)
    {
        result.push_back(&def);
    }
    return result;
}

auto NodeDefinitionRegistry::by_category(const std::string& category_prefix) const
    -> std::vector<const NodeDefinition*>
{
    const std::lock_guard lock(mutex_);
    std::vector<const NodeDefinition*> result;
    for (const auto& [name, def] : definitions_)
    {
        if (def.category.starts_with(category_prefix))
        {
            result.push_back(&def);
        }
    }
    return result;
}

auto NodeDefinitionRegistry::count() const -> std::size_t
{
    const std::lock_guard lock(mutex_);
    return definitions_.size();
}

auto NodeDefinitionRegistry::contains(const std::string& type_name) const -> bool
{
    const std::lock_guard lock(mutex_);
    return definitions_.contains(type_name);
}

void NodeDefinitionRegistry::clear()
{
    const std::lock_guard lock(mutex_);
    definitions_.clear();
}

} // namespace markamp::node_editor
