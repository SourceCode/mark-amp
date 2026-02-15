#include "core/RefResolver.h"

#include "core/BlockDatabase.h"
#include "core/RefParser.h"

namespace markamp::core
{

RefResolver::RefResolver(EventBus& event_bus, BlockDatabase& database, RefIndex& ref_index)
    : event_bus_(event_bus)
    , database_(database)
    , ref_index_(ref_index)
{
}

auto RefResolver::resolve_ref(const ParsedRef& ref, const BlockId& source_block) -> ResolvedRef
{
    ResolvedRef resolved;
    resolved.parsed = ref;
    resolved.source_block_id = source_block;

    // Look up source block to get root_id
    auto source = database_.get_block(source_block.value);
    if (source)
    {
        resolved.source_root_id = source->root_id;
    }

    // Look up the target block
    auto target = database_.get_block(ref.def_block_id.value);
    if (target)
    {
        resolved.target_exists = true;
        resolved.target_type = target->type_abbr();

        // Display text: prefer anchor text, fallback to block content
        if (ref.anchor_text)
        {
            resolved.display_text = *ref.anchor_text;
        }
        else
        {
            // Use first 100 chars of content
            resolved.display_text = target->content.substr(0, 100);
        }

        resolved.target_snippet = target->content.substr(0, 100);
    }
    else
    {
        resolved.target_exists = false;
        resolved.display_text = ref.def_block_id.value; // Fallback to raw ID
    }

    return resolved;
}

auto RefResolver::resolve_embed(const ParsedRef& ref) -> std::optional<std::string>
{
    auto target = database_.get_block(ref.def_block_id.value);
    if (!target)
    {
        return std::nullopt;
    }

    // Return content as HTML
    return R"(<div class="block-embed" data-block-id=")" + ref.def_block_id.value + R"(">)" +
           target->content + "</div>";
}

auto RefResolver::batch_resolve(const std::vector<ParsedRef>& refs, const BlockId& source_block)
    -> std::vector<ResolvedRef>
{
    std::vector<ResolvedRef> results;
    results.reserve(refs.size());

    for (const auto& ref : refs)
    {
        results.push_back(resolve_ref(ref, source_block));
    }

    return results;
}

void RefResolver::on_block_changed(const BlockId& block_id, std::string_view new_content)
{
    // Remove old refs from this source
    ref_index_.remove_refs_by_source(block_id);

    // Re-parse and add new refs
    const RefParser parser;
    auto refs = parser.parse_all(new_content);

    for (const auto& ref : refs)
    {
        ref_index_.add_ref(block_id, ref.def_block_id, ref.type);
    }
}

void RefResolver::rebuild_index()
{
    ref_index_.clear();

    // Scan all blocks in database for references
    const RefParser parser;

    // Get all blocks (simplified — in production would use cursor/batch)
    auto all_blocks = database_.search_blocks("*");
    for (const auto& block : all_blocks)
    {
        auto refs = parser.parse_all(block.content);
        const BlockId source_id{block.id};
        for (const auto& ref : refs)
        {
            ref_index_.add_ref(source_id, ref.def_block_id, ref.type);
        }
    }
}

void RefResolver::set_max_embed_depth(int depth)
{
    max_embed_depth_ = depth;
}

} // namespace markamp::core
