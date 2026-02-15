#pragma once

#include "core/Block.h"
#include "core/BlockDatabase.h"
#include "core/BlockTreeIndex.h"
#include "core/DocumentFileSystem.h"
#include "core/EventBus.h"

#include <expected>
#include <optional>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace markamp::core
{

// Service for batch attribute operations with persistence.
// All changes are persisted to: .sy file (Properties), SQLite (ial column),
// and in-memory index (BlockTreeEntry.updated).
class AttributeService
{
public:
    explicit AttributeService(EventBus& event_bus,
                              BlockDatabase& database,
                              DocumentFileSystem& doc_fs,
                              BlockTreeIndex& tree_index);

    // --- Single Block Operations ---

    auto set_attrs(const std::string& block_id, const InlineAttributeList& attrs)
        -> std::expected<void, std::string>;

    [[nodiscard]] auto get_attrs(const std::string& block_id)
        -> std::expected<InlineAttributeList, std::string>;

    auto remove_attrs(const std::string& block_id, const std::vector<std::string>& keys)
        -> std::expected<void, std::string>;

    auto reset_attrs(const std::string& block_id) -> std::expected<void, std::string>;

    // --- Batch Operations ---

    auto batch_set_attrs(const std::vector<std::string>& block_ids,
                         const InlineAttributeList& attrs) -> std::expected<void, std::string>;

    [[nodiscard]] auto batch_get_attrs(const std::vector<std::string>& block_ids)
        -> std::expected<std::vector<std::pair<std::string, InlineAttributeList>>, std::string>;

    // --- Query by Attribute ---

    [[nodiscard]] auto get_blocks_by_attr(const std::string& key, const std::string& value)
        -> std::vector<Block>;

    [[nodiscard]] auto get_blocks_with_attr(const std::string& key) -> std::vector<Block>;

    [[nodiscard]] auto get_blocks_with_custom_attrs() -> std::vector<Block>;

    // --- Named Block Operations ---

    [[nodiscard]] auto get_block_by_name(const std::string& name) -> std::optional<Block>;

    [[nodiscard]] auto get_blocks_by_alias(const std::string& alias) -> std::vector<Block>;

    // --- Bookmark Operations ---

    [[nodiscard]] auto get_bookmarks() -> std::unordered_map<std::string, std::vector<Block>>;

    auto set_bookmark(const std::string& block_id, const std::string& label)
        -> std::expected<void, std::string>;

    auto remove_bookmark(const std::string& block_id) -> std::expected<void, std::string>;

    // --- Memo Operations ---

    [[nodiscard]] auto get_memos() -> std::vector<Block>;

    auto set_memo(const std::string& block_id, const std::string& memo)
        -> std::expected<void, std::string>;

private:
    [[maybe_unused]] EventBus& event_bus_;
    BlockDatabase& database_;
    [[maybe_unused]] DocumentFileSystem& doc_fs_;
    [[maybe_unused]] BlockTreeIndex& tree_index_;

    [[nodiscard]] auto load_block_ial(const std::string& block_id)
        -> std::expected<InlineAttributeList, std::string>;

    auto persist_ial(const std::string& block_id, const InlineAttributeList& attrs)
        -> std::expected<void, std::string>;

    auto touch_ial(InlineAttributeList& attrs) -> void;
};

} // namespace markamp::core
