#pragma once

#include "core/Block.h"
#include "core/BlockDatabase.h"
#include "core/BlockTreeIndex.h"
#include "core/DocumentFileSystem.h"
#include "core/EventBus.h"

#include <expected>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

namespace markamp::core
{

// A single atomic operation on a block.
enum class OperationAction : uint8_t
{
    Insert,        // Insert a new block after a sibling
    Update,        // Replace block content/properties
    Delete,        // Remove a block and its children
    Move,          // Move a block to a new parent/position
    FoldHeading,   // Collapse a heading (hide children)
    UnfoldHeading, // Expand a heading (show children)
    SetAttrs,      // Update block IAL attributes
    Append,        // Insert as last child of parent
    Prepend,       // Insert as first child of parent
};

struct Operation
{
    OperationAction action;

    std::string id;          // Target block ID
    std::string parent_id;   // New parent block ID (for insert/move)
    std::string previous_id; // Previous sibling ID (for insert/move; empty = first child)
    std::string data;        // Block content (markdown) for insert/update

    BlockType block_type = BlockType::Paragraph; // Type of block to create

    // For SetAttrs action
    InlineAttributeList attrs;

    // Populated by the system after execution (return data)
    mutable std::string ret_data;

    // Timestamps
    int64_t timestamp = 0;
};

// A transaction groups one or more operations that should be applied atomically.
struct Transaction
{
    std::string id;
    std::vector<Operation> do_operations;
    std::vector<Operation> undo_operations;
    int64_t timestamp = 0;
    std::string doc_id; // Document root_id affected
};

// Manages all block mutations through the transaction system.
// Every content change goes through this service.
// Thread-safe. Maintains an undo/redo stack.
class BlockService
{
public:
    explicit BlockService(EventBus& event_bus,
                          BlockDatabase& database,
                          DocumentFileSystem& doc_fs,
                          BlockTreeIndex& tree_index);

    // --- Block Insertion ---

    [[nodiscard]] auto insert_block(const std::string& parent_id,
                                    const std::string& previous_id,
                                    const std::string& markdown,
                                    BlockType type = BlockType::Paragraph)
        -> std::expected<std::string, std::string>;

    [[nodiscard]] auto append_block(const std::string& parent_id,
                                    const std::string& markdown,
                                    BlockType type = BlockType::Paragraph)
        -> std::expected<std::string, std::string>;

    [[nodiscard]] auto prepend_block(const std::string& parent_id,
                                     const std::string& markdown,
                                     BlockType type = BlockType::Paragraph)
        -> std::expected<std::string, std::string>;

    // --- Block Update ---

    auto update_block(const std::string& block_id, const std::string& markdown)
        -> std::expected<void, std::string>;

    // --- Block Deletion ---

    [[nodiscard]] auto delete_block(const std::string& block_id)
        -> std::expected<std::vector<std::string>, std::string>;

    // --- Block Move ---

    auto move_block(const std::string& block_id,
                    const std::string& new_parent_id,
                    const std::string& new_previous_id) -> std::expected<void, std::string>;

    // --- Heading Fold/Unfold ---

    auto fold_heading(const std::string& heading_id) -> std::expected<void, std::string>;
    auto unfold_heading(const std::string& heading_id) -> std::expected<void, std::string>;

    // --- Attribute Operations ---

    auto set_block_attrs(const std::string& block_id, const InlineAttributeList& attrs)
        -> std::expected<void, std::string>;

    [[nodiscard]] auto get_block_attrs(const std::string& block_id)
        -> std::expected<InlineAttributeList, std::string>;

    // --- Block Info Queries ---

    [[nodiscard]] auto get_block_info(const std::string& block_id)
        -> std::expected<Block, std::string>;

    [[nodiscard]] auto get_block_kramdown(const std::string& block_id)
        -> std::expected<std::string, std::string>;

    [[nodiscard]] auto get_block_breadcrumb(const std::string& block_id)
        -> std::expected<std::vector<Block>, std::string>;

    [[nodiscard]] auto get_block_document(const std::string& block_id)
        -> std::expected<Block, std::string>;

    // --- Transaction System ---

    auto perform_transaction(Transaction& txn) -> std::expected<void, std::string>;
    auto perform_transactions(std::vector<Transaction>& txns) -> std::expected<void, std::string>;

    // --- Undo/Redo ---

    [[nodiscard]] auto undo() -> std::expected<bool, std::string>;
    [[nodiscard]] auto redo() -> std::expected<bool, std::string>;
    [[nodiscard]] auto can_undo() const -> bool;
    [[nodiscard]] auto can_redo() const -> bool;
    auto clear_history() -> void;
    [[nodiscard]] auto undo_stack_size() const -> size_t;
    [[nodiscard]] auto redo_stack_size() const -> size_t;

private:
    [[maybe_unused]] EventBus& event_bus_;
    BlockDatabase& database_;
    DocumentFileSystem& doc_fs_;
    BlockTreeIndex& tree_index_;

    std::vector<Transaction> undo_stack_;
    std::vector<Transaction> redo_stack_;
    mutable std::mutex history_mutex_;

    static constexpr size_t kMaxUndoHistory = 100;

    // Internal operation executors — each returns the inverse (undo) operation
    [[nodiscard]] auto execute_operation(const Operation& oper)
        -> std::expected<Operation, std::string>;
    [[nodiscard]] auto execute_insert(const Operation& oper)
        -> std::expected<Operation, std::string>;
    [[nodiscard]] auto execute_update(const Operation& oper)
        -> std::expected<Operation, std::string>;
    [[nodiscard]] auto execute_delete(const Operation& oper)
        -> std::expected<Operation, std::string>;
    [[nodiscard]] auto execute_move(const Operation& oper) -> std::expected<Operation, std::string>;
    [[nodiscard]] auto execute_fold_heading(const Operation& oper)
        -> std::expected<Operation, std::string>;
    [[nodiscard]] auto execute_unfold_heading(const Operation& oper)
        -> std::expected<Operation, std::string>;
    [[nodiscard]] auto execute_set_attrs(const Operation& oper)
        -> std::expected<Operation, std::string>;

    // Tree manipulation helpers
    [[nodiscard]] auto load_containing_tree(const std::string& block_id)
        -> std::expected<std::shared_ptr<Block>, std::string>;
    [[nodiscard]] auto find_block_in_tree(const std::shared_ptr<Block>& root,
                                          const std::string& block_id) -> std::shared_ptr<Block>;
    [[nodiscard]] auto find_parent_in_tree(const std::shared_ptr<Block>& root,
                                           const std::string& child_id) -> std::shared_ptr<Block>;
    auto insert_child_after(std::shared_ptr<Block>& parent,
                            std::shared_ptr<Block> child,
                            const std::string& after_id) -> void;
    [[nodiscard]] auto remove_child(std::shared_ptr<Block>& parent, const std::string& child_id)
        -> std::shared_ptr<Block>;
    [[nodiscard]] auto get_heading_children(const std::shared_ptr<Block>& parent,
                                            const std::string& heading_id)
        -> std::vector<std::shared_ptr<Block>>;
    auto save_and_reindex(const std::string& box_id,
                          const std::string& path,
                          const std::shared_ptr<Block>& root) -> std::expected<void, std::string>;
    auto recalculate_sort_order(std::shared_ptr<Block>& parent) -> void;
    auto touch_block(const std::shared_ptr<Block>& root, const std::string& block_id) -> void;
};

} // namespace markamp::core
