#include "AVRelationService.h"

#include <algorithm>

namespace markamp::core::av
{

AVRelationService::AVRelationService(AttributeViewStore& store)
    : store_(store)
{
}

auto AVRelationService::add_relation(const std::string& source_av_id,
                                     const std::string& source_block_id,
                                     const std::string& relation_key_id,
                                     const std::string& target_block_id)
    -> std::expected<void, std::string>
{
    auto load_result = store_.load(source_av_id);
    if (!load_result)
    {
        return std::unexpected(load_result.error());
    }

    auto& attribute_view = load_result.value();
    auto* value = attribute_view.get_value(relation_key_id, source_block_id);

    if (!value)
    {
        // Create a new relation value
        AVValueRelation relation_data;
        relation_data.block_ids.push_back(target_block_id);
        attribute_view.set_value(
            relation_key_id, source_block_id, AVValueVariant{std::move(relation_data)});
    }
    else
    {
        auto* relation_val = value->as_relation();
        if (!relation_val)
        {
            return std::unexpected("Value is not a relation type");
        }

        // Check for duplicates
        const auto iter = std::find(
            relation_val->block_ids.begin(), relation_val->block_ids.end(), target_block_id);
        if (iter == relation_val->block_ids.end())
        {
            relation_val->block_ids.push_back(target_block_id);
        }
    }

    return store_.save(attribute_view);
}

auto AVRelationService::remove_relation(const std::string& source_av_id,
                                        const std::string& source_block_id,
                                        const std::string& relation_key_id,
                                        const std::string& target_block_id)
    -> std::expected<void, std::string>
{
    auto load_result = store_.load(source_av_id);
    if (!load_result)
    {
        return std::unexpected(load_result.error());
    }

    auto& attribute_view = load_result.value();
    auto* value = attribute_view.get_value(relation_key_id, source_block_id);
    if (!value)
    {
        return {};
    }

    auto* relation_val = value->as_relation();
    if (!relation_val)
    {
        return {};
    }

    relation_val->block_ids.erase(std::remove(relation_val->block_ids.begin(),
                                              relation_val->block_ids.end(),
                                              target_block_id),
                                  relation_val->block_ids.end());

    return store_.save(attribute_view);
}

auto AVRelationService::resolve_relations(const std::string& av_id,
                                          const std::string& block_id,
                                          const std::string& key_id) const
    -> std::vector<ResolvedRelation>
{
    auto load_result = store_.load(av_id);
    if (!load_result)
    {
        return {};
    }

    const auto& attribute_view = load_result.value();
    const auto* value = attribute_view.get_value(key_id, block_id);
    if (!value)
    {
        return {};
    }

    const auto* relation_val = value->as_relation();
    if (!relation_val)
    {
        return {};
    }

    const auto* key = attribute_view.find_key(key_id);
    if (!key)
    {
        return {};
    }

    std::vector<ResolvedRelation> result;
    result.reserve(relation_val->block_ids.size());

    const auto& dest_av_id = key->relation_config.dest_av_id;

    for (const auto& target_bid : relation_val->block_ids)
    {
        ResolvedRelation resolved;
        resolved.block_id = target_bid;
        resolved.av_id = dest_av_id;

        // Try to resolve the title from the target AV
        if (!dest_av_id.empty())
        {
            auto target_result = store_.load(dest_av_id);
            if (target_result)
            {
                const auto& target_av = target_result.value();
                // Look for the Block key in the target AV
                if (!target_av.key_values.empty())
                {
                    const auto* block_val =
                        target_av.get_value(target_av.key_values.front().key.id, target_bid);
                    if (block_val)
                    {
                        resolved.title = block_val->to_display_string();
                    }
                }
            }
        }

        if (resolved.title.empty())
        {
            resolved.title = target_bid; // Fallback
        }

        result.push_back(std::move(resolved));
    }

    return result;
}

auto AVRelationService::create_two_way_relation(const std::string& source_av_id,
                                                const std::string& source_key_id,
                                                const std::string& target_av_id)
    -> std::expected<std::string, std::string>
{
    // Load source AV and set the relation config
    auto source_result = store_.load(source_av_id);
    if (!source_result)
    {
        return std::unexpected(source_result.error());
    }

    auto& source_av = source_result.value();
    auto* source_key = source_av.find_key(source_key_id);
    if (!source_key)
    {
        return std::unexpected("Source key not found: " + source_key_id);
    }

    // Load target AV and create back-relation key
    auto target_result = store_.load(target_av_id);
    if (!target_result)
    {
        return std::unexpected(target_result.error());
    }

    auto& target_av = target_result.value();

    AVKey back_key;
    back_key.name = source_av.name;
    back_key.type = AVKeyType::Relation;
    back_key.relation_config.dest_av_id = source_av_id;
    back_key.relation_config.is_two_way = true;

    const auto back_key_id = target_av.add_key(std::move(back_key));

    // Update source key config
    source_key->relation_config.dest_av_id = target_av_id;
    source_key->relation_config.is_two_way = true;
    source_key->relation_config.back_key_id = back_key_id;

    // Update target back-key config
    auto* target_back_key = target_av.find_key(back_key_id);
    if (target_back_key)
    {
        target_back_key->relation_config.back_key_id = source_key_id;
    }

    // Save both
    if (const auto save_result = store_.save(source_av); !save_result)
    {
        return std::unexpected(save_result.error());
    }
    if (const auto save_result = store_.save(target_av); !save_result)
    {
        return std::unexpected(save_result.error());
    }

    return back_key_id;
}

auto AVRelationService::sync_back_relations(const std::string& source_av_id,
                                            const std::string& source_block_id,
                                            const std::string& source_key_id)
    -> std::expected<void, std::string>
{
    auto source_result = store_.load(source_av_id);
    if (!source_result)
    {
        return std::unexpected(source_result.error());
    }

    const auto& source_av = source_result.value();
    const auto* source_key = source_av.find_key(source_key_id);
    if (!source_key || !source_key->relation_config.is_two_way)
    {
        return {}; // Not a two-way relation
    }

    const auto& dest_av_id = source_key->relation_config.dest_av_id;
    const auto& back_key_id = source_key->relation_config.back_key_id;

    if (dest_av_id.empty() || back_key_id.empty())
    {
        return {};
    }

    const auto* source_value = source_av.get_value(source_key_id, source_block_id);
    if (!source_value)
    {
        return {};
    }

    const auto* relation_val = source_value->as_relation();
    if (!relation_val)
    {
        return {};
    }

    // Ensure each target row has a back-relation to source_block_id
    for (const auto& target_bid : relation_val->block_ids)
    {
        auto add_result = add_relation(dest_av_id, target_bid, back_key_id, source_block_id);
        if (!add_result)
        {
            return add_result;
        }
    }

    return {};
}

} // namespace markamp::core::av
