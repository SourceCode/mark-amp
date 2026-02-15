#include "core/AttributeService.h"

#include "core/IALParser.h"

#include <array>
#include <chrono>
#include <ctime>

namespace markamp::core
{

AttributeService::AttributeService(EventBus& event_bus,
                                   BlockDatabase& database,
                                   DocumentFileSystem& doc_fs,
                                   BlockTreeIndex& tree_index)
    : event_bus_(event_bus)
    , database_(database)
    , doc_fs_(doc_fs)
    , tree_index_(tree_index)
{
}

// ============================================================================
// Single Block Operations
// ============================================================================

auto AttributeService::set_attrs(const std::string& block_id, const InlineAttributeList& attrs)
    -> std::expected<void, std::string>
{
    auto current = load_block_ial(block_id);
    if (!current)
    {
        return std::unexpected(current.error());
    }

    auto merged = IALParser::merge(current.value(), attrs);
    touch_ial(merged);

    return persist_ial(block_id, merged);
}

auto AttributeService::get_attrs(const std::string& block_id)
    -> std::expected<InlineAttributeList, std::string>
{
    return load_block_ial(block_id);
}

auto AttributeService::remove_attrs(const std::string& block_id,
                                    const std::vector<std::string>& keys)
    -> std::expected<void, std::string>
{
    auto current = load_block_ial(block_id);
    if (!current)
    {
        return std::unexpected(current.error());
    }

    auto& attrs = current.value();
    for (const auto& key : keys)
    {
        attrs.erase(key);
    }
    touch_ial(attrs);

    return persist_ial(block_id, attrs);
}

auto AttributeService::reset_attrs(const std::string& block_id) -> std::expected<void, std::string>
{
    auto current = load_block_ial(block_id);
    if (!current)
    {
        return std::unexpected(current.error());
    }

    InlineAttributeList minimal;
    // Preserve only id and updated
    auto it_id = current.value().find("id");
    if (it_id != current.value().end())
    {
        minimal["id"] = it_id->second;
    }
    touch_ial(minimal);

    return persist_ial(block_id, minimal);
}

// ============================================================================
// Batch Operations
// ============================================================================

auto AttributeService::batch_set_attrs(const std::vector<std::string>& block_ids,
                                       const InlineAttributeList& attrs)
    -> std::expected<void, std::string>
{
    for (const auto& block_id : block_ids)
    {
        auto result = set_attrs(block_id, attrs);
        if (!result)
        {
            return result;
        }
    }
    return {};
}

auto AttributeService::batch_get_attrs(const std::vector<std::string>& block_ids)
    -> std::expected<std::vector<std::pair<std::string, InlineAttributeList>>, std::string>
{
    std::vector<std::pair<std::string, InlineAttributeList>> results;
    results.reserve(block_ids.size());

    for (const auto& block_id : block_ids)
    {
        auto attrs = load_block_ial(block_id);
        if (!attrs)
        {
            return std::unexpected(attrs.error());
        }
        results.emplace_back(block_id, attrs.value());
    }
    return results;
}

// ============================================================================
// Query by Attribute
// ============================================================================

auto AttributeService::get_blocks_by_attr(const std::string& key, const std::string& value)
    -> std::vector<Block>
{
    auto ial_search = key + "=\"" + value + "\"";
    return database_.search_blocks(ial_search);
}

auto AttributeService::get_blocks_with_attr(const std::string& key) -> std::vector<Block>
{
    auto ial_search = key + "=\"";
    return database_.search_blocks(ial_search);
}

auto AttributeService::get_blocks_with_custom_attrs() -> std::vector<Block>
{
    return database_.search_blocks("custom-");
}

// ============================================================================
// Named Block Operations
// ============================================================================

auto AttributeService::get_block_by_name(const std::string& name) -> std::optional<Block>
{
    auto blocks = get_blocks_by_attr("name", name);
    if (blocks.empty())
    {
        return std::nullopt;
    }
    return blocks[0];
}

auto AttributeService::get_blocks_by_alias(const std::string& alias) -> std::vector<Block>
{
    return database_.search_blocks(alias);
}

// ============================================================================
// Bookmark Operations
// ============================================================================

auto AttributeService::get_bookmarks() -> std::unordered_map<std::string, std::vector<Block>>
{
    std::unordered_map<std::string, std::vector<Block>> result;

    auto blocks = get_blocks_with_attr("bookmark");
    for (auto& block : blocks)
    {
        auto bookmark = block.get_ial("bookmark");
        if (bookmark)
        {
            result[*bookmark].push_back(std::move(block));
        }
    }

    return result;
}

auto AttributeService::set_bookmark(const std::string& block_id, const std::string& label)
    -> std::expected<void, std::string>
{
    InlineAttributeList attrs;
    attrs["bookmark"] = label;
    return set_attrs(block_id, attrs);
}

auto AttributeService::remove_bookmark(const std::string& block_id)
    -> std::expected<void, std::string>
{
    return remove_attrs(block_id, {"bookmark"});
}

// ============================================================================
// Memo Operations
// ============================================================================

auto AttributeService::get_memos() -> std::vector<Block>
{
    return get_blocks_with_attr("memo");
}

auto AttributeService::set_memo(const std::string& block_id, const std::string& memo)
    -> std::expected<void, std::string>
{
    InlineAttributeList attrs;
    attrs["memo"] = memo;
    return set_attrs(block_id, attrs);
}

// ============================================================================
// Private Helpers
// ============================================================================

auto AttributeService::load_block_ial(const std::string& block_id)
    -> std::expected<InlineAttributeList, std::string>
{
    auto block = database_.get_block(block_id);
    if (!block)
    {
        return std::unexpected("Block not found: " + block_id);
    }
    return block->ial;
}

auto AttributeService::persist_ial(const std::string& block_id, const InlineAttributeList& attrs)
    -> std::expected<void, std::string>
{
    return database_.set_block_attrs(block_id, attrs);
}

auto AttributeService::touch_ial(InlineAttributeList& attrs) -> void
{
    auto now = std::chrono::system_clock::now();
    auto time_t_val = std::chrono::system_clock::to_time_t(now);

    // Format as YYYYMMDDHHmmss
    std::tm tm_val{};
    localtime_r(&time_t_val, &tm_val);

    std::array<char, 15> buf{};
    auto written = std::strftime(buf.data(), buf.size(), "%Y%m%d%H%M%S", &tm_val);
    if (written > 0)
    {
        attrs["updated"] = std::string(buf.data(), written);
    }
}

} // namespace markamp::core
