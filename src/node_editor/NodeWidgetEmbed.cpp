#include "NodeWidgetEmbed.h"

namespace markamp::node_editor
{

auto NodeWidgetEmbed::register_embed(NodeId node_id, EmbedDescriptor descriptor) -> EmbedId
{
    EmbedId eid{next_id_++};
    descriptor.embed_id = eid;
    descriptor.node_id = node_id;
    embeds_.emplace(eid.value, std::move(descriptor));
    return eid;
}

auto NodeWidgetEmbed::remove_embed(EmbedId embed_id) -> bool
{
    return embeds_.erase(embed_id.value) > 0;
}

auto NodeWidgetEmbed::embeds_for(NodeId node_id) const -> std::vector<EmbedId>
{
    std::vector<EmbedId> result;
    for (const auto& [key, desc] : embeds_)
    {
        if (desc.node_id == node_id)
        {
            result.emplace_back(EmbedId{key});
        }
    }
    return result;
}

auto NodeWidgetEmbed::find_embed(EmbedId embed_id) const -> const EmbedDescriptor*
{
    auto iter = embeds_.find(embed_id.value);
    if (iter == embeds_.end())
    {
        return nullptr;
    }
    return &iter->second;
}

auto NodeWidgetEmbed::embed_count() const -> std::size_t
{
    return embeds_.size();
}

auto NodeWidgetEmbed::is_sandboxed(EmbedId embed_id) const -> bool
{
    const auto* desc = find_embed(embed_id);
    if (desc == nullptr)
    {
        return true; // Default to sandboxed for safety
    }
    return desc->sandboxed;
}

void NodeWidgetEmbed::set_value(EmbedId embed_id, const std::string& val)
{
    auto iter = embeds_.find(embed_id.value);
    if (iter != embeds_.end())
    {
        iter->second.current_value = val;
    }
}

auto NodeWidgetEmbed::value(EmbedId embed_id) const -> std::string
{
    const auto* desc = find_embed(embed_id);
    if (desc == nullptr)
    {
        return {};
    }
    return desc->current_value;
}

auto NodeWidgetEmbed::embed_bounds(EmbedId embed_id) const -> Rect
{
    const auto* desc = find_embed(embed_id);
    if (desc == nullptr)
    {
        return {};
    }
    return desc->bounds;
}

void NodeWidgetEmbed::resize_embed(EmbedId embed_id, Rect new_bounds)
{
    auto iter = embeds_.find(embed_id.value);
    if (iter != embeds_.end())
    {
        iter->second.bounds = new_bounds;
    }
}

void NodeWidgetEmbed::clear_all()
{
    embeds_.clear();
}

} // namespace markamp::node_editor
