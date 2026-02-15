#include "core/BlockService.h"

#include "core/BlockID.h"

#include <algorithm>
#include <chrono>

namespace markamp::core
{

BlockService::BlockService(EventBus& event_bus,
                           BlockDatabase& database,
                           DocumentFileSystem& doc_fs,
                           BlockTreeIndex& tree_index)
    : event_bus_(event_bus)
    , database_(database)
    , doc_fs_(doc_fs)
    , tree_index_(tree_index)
{
}

// Helper: get current epoch seconds
static auto now_seconds() -> int64_t
{
    return std::chrono::duration_cast<std::chrono::seconds>(
               std::chrono::system_clock::now().time_since_epoch())
        .count();
}

// ============================================================================
// Block Insertion
// ============================================================================

auto BlockService::insert_block(const std::string& parent_id,
                                const std::string& previous_id,
                                const std::string& markdown,
                                BlockType type) -> std::expected<std::string, std::string>
{
    Transaction txn;
    txn.id = BlockIDGenerator::generate();
    txn.timestamp = now_seconds();

    Operation oper;
    oper.action = OperationAction::Insert;
    oper.parent_id = parent_id;
    oper.previous_id = previous_id;
    oper.data = markdown;
    oper.block_type = type;
    oper.timestamp = txn.timestamp;

    txn.do_operations.push_back(oper);

    auto result = perform_transaction(txn);
    if (!result)
    {
        return std::unexpected(result.error());
    }

    return txn.do_operations[0].ret_data;
}

auto BlockService::append_block(const std::string& parent_id,
                                const std::string& markdown,
                                BlockType type) -> std::expected<std::string, std::string>
{
    auto parent_block = database_.get_block(parent_id);
    if (!parent_block)
    {
        return std::unexpected("Parent block not found: " + parent_id);
    }

    auto children = database_.get_child_blocks(parent_id);
    std::string last_child_id;
    if (!children.empty())
    {
        std::sort(children.begin(), children.end(), compare_blocks_by_sort);
        last_child_id = children.back().id;
    }

    return insert_block(parent_id, last_child_id, markdown, type);
}

auto BlockService::prepend_block(const std::string& parent_id,
                                 const std::string& markdown,
                                 BlockType type) -> std::expected<std::string, std::string>
{
    return insert_block(parent_id, "", markdown, type);
}

// ============================================================================
// Block Update
// ============================================================================

auto BlockService::update_block(const std::string& block_id, const std::string& markdown)
    -> std::expected<void, std::string>
{
    Transaction txn;
    txn.id = BlockIDGenerator::generate();
    txn.timestamp = now_seconds();

    Operation oper;
    oper.action = OperationAction::Update;
    oper.id = block_id;
    oper.data = markdown;
    oper.timestamp = txn.timestamp;

    txn.do_operations.push_back(oper);

    return perform_transaction(txn);
}

// ============================================================================
// Block Deletion
// ============================================================================

auto BlockService::delete_block(const std::string& block_id)
    -> std::expected<std::vector<std::string>, std::string>
{
    Transaction txn;
    txn.id = BlockIDGenerator::generate();
    txn.timestamp = now_seconds();

    Operation oper;
    oper.action = OperationAction::Delete;
    oper.id = block_id;
    oper.timestamp = txn.timestamp;

    txn.do_operations.push_back(oper);

    auto result = perform_transaction(txn);
    if (!result)
    {
        return std::unexpected(result.error());
    }

    std::vector<std::string> deleted_ids;
    const auto& ret = txn.do_operations[0].ret_data;
    if (!ret.empty())
    {
        std::size_t start = 0;
        std::size_t end_pos = ret.find(',');
        while (end_pos != std::string::npos)
        {
            deleted_ids.push_back(ret.substr(start, end_pos - start));
            start = end_pos + 1;
            end_pos = ret.find(',', start);
        }
        deleted_ids.push_back(ret.substr(start));
    }

    return deleted_ids;
}

// ============================================================================
// Block Move
// ============================================================================

auto BlockService::move_block(const std::string& block_id,
                              const std::string& new_parent_id,
                              const std::string& new_previous_id)
    -> std::expected<void, std::string>
{
    Transaction txn;
    txn.id = BlockIDGenerator::generate();
    txn.timestamp = now_seconds();

    Operation oper;
    oper.action = OperationAction::Move;
    oper.id = block_id;
    oper.parent_id = new_parent_id;
    oper.previous_id = new_previous_id;
    oper.timestamp = txn.timestamp;

    txn.do_operations.push_back(oper);

    return perform_transaction(txn);
}

// ============================================================================
// Heading Fold/Unfold
// ============================================================================

auto BlockService::fold_heading(const std::string& heading_id) -> std::expected<void, std::string>
{
    Transaction txn;
    txn.id = BlockIDGenerator::generate();
    txn.timestamp = now_seconds();

    Operation oper;
    oper.action = OperationAction::FoldHeading;
    oper.id = heading_id;
    oper.timestamp = txn.timestamp;

    txn.do_operations.push_back(oper);

    return perform_transaction(txn);
}

auto BlockService::unfold_heading(const std::string& heading_id) -> std::expected<void, std::string>
{
    Transaction txn;
    txn.id = BlockIDGenerator::generate();
    txn.timestamp = now_seconds();

    Operation oper;
    oper.action = OperationAction::UnfoldHeading;
    oper.id = heading_id;
    oper.timestamp = txn.timestamp;

    txn.do_operations.push_back(oper);

    return perform_transaction(txn);
}

// ============================================================================
// Attribute Operations
// ============================================================================

auto BlockService::set_block_attrs(const std::string& block_id, const InlineAttributeList& attrs)
    -> std::expected<void, std::string>
{
    Transaction txn;
    txn.id = BlockIDGenerator::generate();
    txn.timestamp = now_seconds();

    Operation oper;
    oper.action = OperationAction::SetAttrs;
    oper.id = block_id;
    oper.attrs = attrs;
    oper.timestamp = txn.timestamp;

    txn.do_operations.push_back(oper);

    return perform_transaction(txn);
}

auto BlockService::get_block_attrs(const std::string& block_id)
    -> std::expected<InlineAttributeList, std::string>
{
    auto block = database_.get_block(block_id);
    if (!block)
    {
        return std::unexpected("Block not found: " + block_id);
    }
    return block->ial;
}

// ============================================================================
// Block Info Queries
// ============================================================================

auto BlockService::get_block_info(const std::string& block_id) -> std::expected<Block, std::string>
{
    auto block = database_.get_block(block_id);
    if (!block)
    {
        return std::unexpected("Block not found: " + block_id);
    }
    return *block;
}

auto BlockService::get_block_kramdown(const std::string& block_id)
    -> std::expected<std::string, std::string>
{
    auto block = database_.get_block(block_id);
    if (!block)
    {
        return std::unexpected("Block not found: " + block_id);
    }
    return block->markdown;
}

auto BlockService::get_block_breadcrumb(const std::string& block_id)
    -> std::expected<std::vector<Block>, std::string>
{
    std::vector<Block> breadcrumb;

    auto current = database_.get_block(block_id);
    if (!current)
    {
        return std::unexpected("Block not found: " + block_id);
    }

    breadcrumb.push_back(*current);

    while (!current->parent_id.empty() && current->parent_id != current->id)
    {
        current = database_.get_block(current->parent_id);
        if (!current)
        {
            break;
        }
        breadcrumb.push_back(*current);
    }

    std::reverse(breadcrumb.begin(), breadcrumb.end());
    return breadcrumb;
}

auto BlockService::get_block_document(const std::string& block_id)
    -> std::expected<Block, std::string>
{
    auto block = database_.get_block(block_id);
    if (!block)
    {
        return std::unexpected("Block not found: " + block_id);
    }

    if (block->root_id.empty() || block->root_id == block_id)
    {
        return *block;
    }

    auto root = database_.get_block(block->root_id);
    if (!root)
    {
        return std::unexpected("Root document not found: " + block->root_id);
    }
    return *root;
}

// ============================================================================
// Transaction System
// ============================================================================

auto BlockService::perform_transaction(Transaction& txn) -> std::expected<void, std::string>
{
    std::vector<Operation> completed_undo_ops;

    for (auto& oper : txn.do_operations)
    {
        auto result = execute_operation(oper);
        if (!result)
        {
            // Rollback in reverse
            for (auto rit = completed_undo_ops.rbegin(); rit != completed_undo_ops.rend(); ++rit)
            {
                auto rollback = execute_operation(*rit);
                (void)rollback; // Best-effort rollback
            }
            return std::unexpected("Transaction failed: " + result.error());
        }
        completed_undo_ops.push_back(result.value());
    }

    txn.undo_operations = completed_undo_ops;

    {
        const std::lock_guard<std::mutex> lock(history_mutex_);
        undo_stack_.push_back(txn);
        if (undo_stack_.size() > kMaxUndoHistory)
        {
            undo_stack_.erase(undo_stack_.begin());
        }
        redo_stack_.clear();
    }

    return {};
}

auto BlockService::perform_transactions(std::vector<Transaction>& txns)
    -> std::expected<void, std::string>
{
    for (auto& txn : txns)
    {
        auto result = perform_transaction(txn);
        if (!result)
        {
            return result;
        }
    }
    return {};
}

// ============================================================================
// Undo/Redo
// ============================================================================

auto BlockService::undo() -> std::expected<bool, std::string>
{
    const std::lock_guard<std::mutex> lock(history_mutex_);

    if (undo_stack_.empty())
    {
        return false;
    }

    auto txn = undo_stack_.back();
    undo_stack_.pop_back();

    for (auto rit = txn.undo_operations.rbegin(); rit != txn.undo_operations.rend(); ++rit)
    {
        auto result = execute_operation(*rit);
        if (!result)
        {
            return std::unexpected("Undo failed: " + result.error());
        }
    }

    redo_stack_.push_back(txn);
    return true;
}

auto BlockService::redo() -> std::expected<bool, std::string>
{
    const std::lock_guard<std::mutex> lock(history_mutex_);

    if (redo_stack_.empty())
    {
        return false;
    }

    auto txn = redo_stack_.back();
    redo_stack_.pop_back();

    for (auto& oper : txn.do_operations)
    {
        auto result = execute_operation(oper);
        if (!result)
        {
            return std::unexpected("Redo failed: " + result.error());
        }
    }

    undo_stack_.push_back(txn);
    return true;
}

auto BlockService::can_undo() const -> bool
{
    const std::lock_guard<std::mutex> lock(history_mutex_);
    return !undo_stack_.empty();
}

auto BlockService::can_redo() const -> bool
{
    const std::lock_guard<std::mutex> lock(history_mutex_);
    return !redo_stack_.empty();
}

auto BlockService::clear_history() -> void
{
    const std::lock_guard<std::mutex> lock(history_mutex_);
    undo_stack_.clear();
    redo_stack_.clear();
}

auto BlockService::undo_stack_size() const -> size_t
{
    const std::lock_guard<std::mutex> lock(history_mutex_);
    return undo_stack_.size();
}

auto BlockService::redo_stack_size() const -> size_t
{
    const std::lock_guard<std::mutex> lock(history_mutex_);
    return redo_stack_.size();
}

// ============================================================================
// Internal Operation Executors
// ============================================================================

auto BlockService::execute_operation(const Operation& oper) -> std::expected<Operation, std::string>
{
    switch (oper.action)
    {
        case OperationAction::Insert:
        case OperationAction::Append:
        case OperationAction::Prepend:
            return execute_insert(oper);
        case OperationAction::Update:
            return execute_update(oper);
        case OperationAction::Delete:
            return execute_delete(oper);
        case OperationAction::Move:
            return execute_move(oper);
        case OperationAction::FoldHeading:
            return execute_fold_heading(oper);
        case OperationAction::UnfoldHeading:
            return execute_unfold_heading(oper);
        case OperationAction::SetAttrs:
            return execute_set_attrs(oper);
    }
    return std::unexpected("Unknown operation action");
}

auto BlockService::execute_insert(const Operation& oper) -> std::expected<Operation, std::string>
{
    auto parent_entry = tree_index_.get(oper.parent_id);
    if (!parent_entry)
    {
        return std::unexpected("Parent block not found in index: " + oper.parent_id);
    }

    Block new_block;
    new_block.id = oper.id.empty() ? BlockIDGenerator::generate() : oper.id;
    new_block.parent_id = oper.parent_id;
    new_block.root_id = parent_entry->root_id;
    new_block.box = parent_entry->box_id;
    new_block.path = parent_entry->path;
    new_block.type = oper.block_type;
    new_block.content = oper.data;
    new_block.markdown = oper.data;
    new_block.fcontent = oper.data;

    auto now_str = BlockIDGenerator::current_timestamp_string();
    new_block.created_str = now_str;
    new_block.updated_str = now_str;

    auto db_result = database_.upsert_block(new_block);
    if (!db_result)
    {
        return std::unexpected("Failed to insert block: " + db_result.error());
    }

    BlockTreeEntry entry;
    entry.id = new_block.id;
    entry.parent_id = new_block.parent_id;
    entry.root_id = new_block.root_id;
    entry.box_id = new_block.box;
    entry.path = new_block.path;
    entry.type = new_block.type;
    entry.updated = now_str;
    tree_index_.upsert(entry);

    Operation undo_op;
    undo_op.action = OperationAction::Delete;
    undo_op.id = new_block.id;
    undo_op.timestamp = oper.timestamp;

    // Store the inserted block ID for caller access via ret_data
    oper.ret_data = new_block.id;

    return undo_op;
}

auto BlockService::execute_update(const Operation& oper) -> std::expected<Operation, std::string>
{
    auto old_block = database_.get_block(oper.id);
    if (!old_block)
    {
        return std::unexpected("Block not found for update: " + oper.id);
    }

    Operation undo_op;
    undo_op.action = OperationAction::Update;
    undo_op.id = oper.id;
    undo_op.data = old_block->markdown;
    undo_op.timestamp = oper.timestamp;

    old_block->content = oper.data;
    old_block->markdown = oper.data;
    old_block->fcontent = oper.data;
    old_block->updated_str = BlockIDGenerator::current_timestamp_string();

    auto db_result = database_.upsert_block(*old_block);
    if (!db_result)
    {
        return std::unexpected("Failed to update block: " + db_result.error());
    }

    return undo_op;
}

auto BlockService::execute_delete(const Operation& oper) -> std::expected<Operation, std::string>
{
    auto block = database_.get_block(oper.id);
    if (!block)
    {
        return std::unexpected("Block not found for delete: " + oper.id);
    }

    std::vector<std::string> deleted_ids;
    deleted_ids.push_back(oper.id);

    auto children = database_.get_child_blocks(oper.id);
    for (const auto& child : children)
    {
        deleted_ids.push_back(child.id);
    }

    Operation undo_op;
    undo_op.action = OperationAction::Insert;
    undo_op.id = oper.id;
    undo_op.parent_id = block->parent_id;
    undo_op.data = block->markdown;
    undo_op.block_type = block->type;
    undo_op.timestamp = oper.timestamp;

    for (const auto& deleted_id : deleted_ids)
    {
        auto db_result = database_.delete_block(deleted_id);
        (void)db_result; // Best-effort cascade delete
        tree_index_.remove(deleted_id);
    }

    std::string ret;
    for (std::size_t idx = 0; idx < deleted_ids.size(); ++idx)
    {
        if (idx > 0)
        {
            ret += ",";
        }
        ret += deleted_ids[idx];
    }
    oper.ret_data = ret;

    return undo_op;
}

auto BlockService::execute_move(const Operation& oper) -> std::expected<Operation, std::string>
{
    auto block = database_.get_block(oper.id);
    if (!block)
    {
        return std::unexpected("Block not found for move: " + oper.id);
    }

    Operation undo_op;
    undo_op.action = OperationAction::Move;
    undo_op.id = oper.id;
    undo_op.parent_id = block->parent_id;
    undo_op.previous_id = "";
    undo_op.timestamp = oper.timestamp;

    auto new_parent_entry = tree_index_.get(oper.parent_id);
    if (!new_parent_entry)
    {
        return std::unexpected("New parent block not found: " + oper.parent_id);
    }

    block->parent_id = oper.parent_id;
    block->root_id = new_parent_entry->root_id;
    block->box = new_parent_entry->box_id;
    block->path = new_parent_entry->path;
    block->updated_str = BlockIDGenerator::current_timestamp_string();

    auto db_result = database_.upsert_block(*block);
    if (!db_result)
    {
        return std::unexpected("Failed to move block: " + db_result.error());
    }

    BlockTreeEntry entry;
    entry.id = block->id;
    entry.parent_id = block->parent_id;
    entry.root_id = block->root_id;
    entry.box_id = block->box;
    entry.path = block->path;
    entry.type = block->type;
    entry.updated = block->updated_str;
    tree_index_.upsert(entry);

    return undo_op;
}

auto BlockService::execute_fold_heading(const Operation& oper)
    -> std::expected<Operation, std::string>
{
    auto block = database_.get_block(oper.id);
    if (!block)
    {
        return std::unexpected("Heading block not found: " + oper.id);
    }

    if (!block->is_heading())
    {
        return std::unexpected("Block is not a heading: " + oper.id);
    }

    block->folded = true;
    block->set_ial("heading-fold", "1");
    block->updated_str = BlockIDGenerator::current_timestamp_string();

    auto db_result = database_.upsert_block(*block);
    if (!db_result)
    {
        return std::unexpected("Failed to fold heading: " + db_result.error());
    }

    Operation undo_op;
    undo_op.action = OperationAction::UnfoldHeading;
    undo_op.id = oper.id;
    undo_op.timestamp = oper.timestamp;

    return undo_op;
}

auto BlockService::execute_unfold_heading(const Operation& oper)
    -> std::expected<Operation, std::string>
{
    auto block = database_.get_block(oper.id);
    if (!block)
    {
        return std::unexpected("Heading block not found: " + oper.id);
    }

    if (!block->is_heading())
    {
        return std::unexpected("Block is not a heading: " + oper.id);
    }

    block->folded = false;
    block->remove_ial("heading-fold");
    block->updated_str = BlockIDGenerator::current_timestamp_string();

    auto db_result = database_.upsert_block(*block);
    if (!db_result)
    {
        return std::unexpected("Failed to unfold heading: " + db_result.error());
    }

    Operation undo_op;
    undo_op.action = OperationAction::FoldHeading;
    undo_op.id = oper.id;
    undo_op.timestamp = oper.timestamp;

    return undo_op;
}

auto BlockService::execute_set_attrs(const Operation& oper) -> std::expected<Operation, std::string>
{
    auto block = database_.get_block(oper.id);
    if (!block)
    {
        return std::unexpected("Block not found for set attrs: " + oper.id);
    }

    Operation undo_op;
    undo_op.action = OperationAction::SetAttrs;
    undo_op.id = oper.id;
    undo_op.attrs = block->ial;
    undo_op.timestamp = oper.timestamp;

    for (const auto& [key, value] : oper.attrs)
    {
        block->set_ial(key, value);
    }

    block->updated_str = BlockIDGenerator::current_timestamp_string();

    auto db_result = database_.upsert_block(*block);
    if (!db_result)
    {
        return std::unexpected("Failed to set attrs: " + db_result.error());
    }

    return undo_op;
}

// ============================================================================
// Tree Manipulation Helpers
// ============================================================================

auto BlockService::load_containing_tree(const std::string& block_id)
    -> std::expected<std::shared_ptr<Block>, std::string>
{
    auto entry = tree_index_.get(block_id);
    if (!entry)
    {
        return std::unexpected("Block not found in index: " + block_id);
    }

    auto root_block = database_.get_block(entry->root_id);
    if (!root_block)
    {
        return std::unexpected("Root block not found: " + entry->root_id);
    }

    return std::make_shared<Block>(*root_block);
}

auto BlockService::find_block_in_tree(const std::shared_ptr<Block>& root,
                                      const std::string& block_id) -> std::shared_ptr<Block>
{
    if (!root)
    {
        return nullptr;
    }
    if (root->id == block_id)
    {
        return root;
    }

    for (auto& child : root->children)
    {
        auto found = find_block_in_tree(child, block_id);
        if (found)
        {
            return found;
        }
    }
    return nullptr;
}

auto BlockService::find_parent_in_tree(const std::shared_ptr<Block>& root,
                                       const std::string& child_id) -> std::shared_ptr<Block>
{
    if (!root)
    {
        return nullptr;
    }

    for (auto& child : root->children)
    {
        if (child->id == child_id)
        {
            return root;
        }
        auto found = find_parent_in_tree(child, child_id);
        if (found)
        {
            return found;
        }
    }
    return nullptr;
}

auto BlockService::insert_child_after(std::shared_ptr<Block>& parent,
                                      std::shared_ptr<Block> child,
                                      const std::string& after_id) -> void
{
    if (after_id.empty())
    {
        parent->children.insert(parent->children.begin(), std::move(child));
        return;
    }

    for (auto iter = parent->children.begin(); iter != parent->children.end(); ++iter)
    {
        if ((*iter)->id == after_id)
        {
            parent->children.insert(iter + 1, std::move(child));
            return;
        }
    }

    parent->children.push_back(std::move(child));
}

auto BlockService::remove_child(std::shared_ptr<Block>& parent, const std::string& child_id)
    -> std::shared_ptr<Block>
{
    for (auto iter = parent->children.begin(); iter != parent->children.end(); ++iter)
    {
        if ((*iter)->id == child_id)
        {
            auto removed = *iter;
            parent->children.erase(iter);
            return removed;
        }
    }
    return nullptr;
}

auto BlockService::get_heading_children(const std::shared_ptr<Block>& parent,
                                        const std::string& heading_id)
    -> std::vector<std::shared_ptr<Block>>
{
    std::vector<std::shared_ptr<Block>> result;
    bool found_heading = false;
    int heading_depth = 0;

    for (const auto& child : parent->children)
    {
        if (child->id == heading_id)
        {
            found_heading = true;
            heading_depth = child->depth;
            continue;
        }

        if (found_heading)
        {
            if (child->is_heading() && child->depth <= heading_depth)
            {
                break;
            }
            result.push_back(child);
        }
    }

    return result;
}

auto BlockService::save_and_reindex(const std::string& /*box_id*/,
                                    const std::string& /*path*/,
                                    const std::shared_ptr<Block>& root)
    -> std::expected<void, std::string>
{
    auto save_result = doc_fs_.save_tree(*root);
    if (!save_result)
    {
        return std::unexpected("Failed to save tree: " + save_result.error());
    }

    tree_index_.index_tree(*root);

    return {};
}

auto BlockService::recalculate_sort_order(std::shared_ptr<Block>& parent) -> void
{
    int64_t sort_value = 0;
    for (auto& child : parent->children)
    {
        child->sort = sort_value;
        sort_value += 10;
    }
}

auto BlockService::touch_block(const std::shared_ptr<Block>& root, const std::string& block_id)
    -> void
{
    auto now_str = BlockIDGenerator::current_timestamp_string();

    auto block = find_block_in_tree(root, block_id);
    if (block)
    {
        block->updated_str = now_str;
    }

    if (root && root->id != block_id)
    {
        root->updated_str = now_str;
    }
}

} // namespace markamp::core
