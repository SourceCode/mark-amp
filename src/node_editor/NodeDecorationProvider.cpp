#include "NodeDecorationProvider.h"

#include <algorithm>

namespace markamp::node_editor
{

void NodeDecorationProvider::register_provider(const std::string& provider_id,
                                               const std::string& description)
{
    providers_[provider_id] = DecorationProviderInfo{provider_id, description};
}

auto NodeDecorationProvider::unregister_provider(const std::string& provider_id) -> bool
{
    clear_provider(provider_id);
    return providers_.erase(provider_id) > 0;
}

auto NodeDecorationProvider::provider_count() const -> std::size_t
{
    return providers_.size();
}

auto NodeDecorationProvider::add_decoration(NodeId node_id, NodeDecoration decoration)
    -> DecorationId
{
    DecorationId did{next_id_++};
    decoration.decoration_id = did;
    decoration.target_node = node_id;
    decorations_.emplace(did.value, std::move(decoration));
    return did;
}

auto NodeDecorationProvider::remove_decoration(DecorationId decoration_id) -> bool
{
    return decorations_.erase(decoration_id.value) > 0;
}

auto NodeDecorationProvider::decorations_for(NodeId node_id) const -> std::vector<NodeDecoration>
{
    std::vector<NodeDecoration> result;
    for (const auto& [_, dec] : decorations_)
    {
        if (dec.target_node == node_id)
        {
            result.push_back(dec);
        }
    }
    // Sort by priority (higher first)
    std::sort(result.begin(),
              result.end(),
              [](const NodeDecoration& left, const NodeDecoration& right)
              { return left.priority > right.priority; });
    return result;
}

auto NodeDecorationProvider::decorations_by_provider(const std::string& provider_id) const
    -> std::vector<NodeDecoration>
{
    std::vector<NodeDecoration> result;
    for (const auto& [_, dec] : decorations_)
    {
        if (dec.provider_id == provider_id)
        {
            result.push_back(dec);
        }
    }
    return result;
}

void NodeDecorationProvider::clear_provider(const std::string& provider_id)
{
    std::vector<uint64_t> to_remove;
    for (const auto& [key, dec] : decorations_)
    {
        if (dec.provider_id == provider_id)
        {
            to_remove.push_back(key);
        }
    }
    for (auto key : to_remove)
    {
        decorations_.erase(key);
    }
}

auto NodeDecorationProvider::decoration_count() const -> std::size_t
{
    return decorations_.size();
}

void NodeDecorationProvider::set_language_hint(NodeId node_id, const std::string& language_id)
{
    language_hints_[node_id.value] = language_id;
}

auto NodeDecorationProvider::language_hint(NodeId node_id) const -> std::optional<std::string>
{
    auto iter = language_hints_.find(node_id.value);
    if (iter == language_hints_.end())
    {
        return std::nullopt;
    }
    return iter->second;
}

void NodeDecorationProvider::clear_all()
{
    decorations_.clear();
    providers_.clear();
    language_hints_.clear();
}

} // namespace markamp::node_editor
