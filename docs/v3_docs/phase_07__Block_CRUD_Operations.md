# Phase 07 — Block CRUD Operations & Transaction System

## Objective

Implement the block manipulation service that handles creating, updating, moving, and deleting blocks within documents. This is the primary write path for all content changes in the knowledge base. Porting SiYuan's operation-based transaction system from `kernel/model/transaction.go`, where every content mutation is wrapped in a transaction containing one or more operations. The transaction log enables undo/redo support and eventual sync conflict resolution.

## Prerequisites

- Phase 01 (Block Data Model) — Block struct being manipulated
- Phase 02 (Block ID Generation) — IDs for newly created blocks
- Phase 04 (SQLite Storage Layer) — Database updates after mutations
- Phase 05 (Document File System) — .sy file persistence
- Phase 06 (Block Tree Index) — Index updates after mutations

## SiYuan Source Reference

- `kernel/model/transaction.go` — Core transaction system:
  - `Transaction` struct: `DoOperations []*Operation`, `UndoOperations []*Operation`, `Timestamp int64`
  - `Operation` struct: `Action string`, `ID string`, `ParentID string`, `PreviousID string`, `Data string`, `RetData interface{}`
  - `PerformTransactions(transactions []*Transaction)` — Applies a batch of transactions
  - Operation actions: `"insert"`, `"update"`, `"delete"`, `"move"`, `"foldHeading"`, `"unfoldHeading"`, `"setAttrs"`, `"insertAttrViewBlock"`, `"removeAttrViewBlock"`
- `kernel/model/block_op.go` — Individual block operations:
  - `InsertBlock(block, parent, previous)` — Insert after previous sibling
  - `PrependBlock(block, parent)` — Insert as first child
  - `AppendBlock(block, parent)` — Insert as last child
  - `UpdateBlock(id, data)` — Replace block content
  - `DeleteBlock(id)` — Remove block and children
  - `MoveBlock(id, parent, previous)` — Move to new position
- `kernel/api/block.go` — API endpoints that call into the block operations:
  - `insertBlock`, `prependBlock`, `appendBlock`, `updateBlock`, `deleteBlock`, `moveBlock`
  - `getBlockInfo`, `getBlockDOM`, `getBlockKramdown`, `getBlockBreadcrumb`
- `kernel/model/heading.go` — `FoldHeading(id)`, `UnfoldHeading(id)` — Fold/unfold heading blocks (hide/show children)

## MarkAmp Integration Points

- New header: `src/core/BlockService.h`
- New source: `src/core/BlockService.cpp`
- Depends on: Block.h, BlockID.h, BlockDatabase.h, DocumentFileSystem.h, BlockTreeIndex.h, EventBus
- Integrates with PluginContext as one of the 24 services (or as an addition)
- The transaction log is a new data structure persisted to enable undo/redo

## Data Structures to Implement

```cpp
namespace markamp::core
{

// A single atomic operation on a block.
// Operations are the granular unit of change.
enum class OperationAction : uint8_t
{
    Insert,           // Insert a new block after a sibling
    Update,           // Replace block content/properties
    Delete,           // Remove a block and its children
    Move,             // Move a block to a new parent/position
    FoldHeading,      // Collapse a heading (hide children)
    UnfoldHeading,    // Expand a heading (show children)
    SetAttrs,         // Update block IAL attributes
    Append,           // Insert as last child of parent
    Prepend,          // Insert as first child of parent
};

struct Operation
{
    OperationAction action;

    std::string id;             // Target block ID
    std::string parent_id;      // New parent block ID (for insert/move)
    std::string previous_id;    // Previous sibling ID (for insert/move; empty = first child)
    std::string data;           // Block content (markdown or DOM) for insert/update
    BlockType block_type = BlockType::Paragraph;  // Type of block to create (for insert)

    // For SetAttrs action
    InlineAttributeList attrs;

    // Populated by the system after execution (return data)
    std::string ret_data;       // Result data (e.g., the inserted block's ID)

    // Timestamps
    int64_t timestamp = 0;      // When this operation was created
};

// A transaction groups one or more operations that should be applied atomically.
// The do_operations are applied forward; undo_operations reverse them.
struct Transaction
{
    std::string id;                              // Transaction ID (block ID format)
    std::vector<Operation> do_operations;        // Forward operations
    std::vector<Operation> undo_operations;      // Reverse operations (populated after do)
    int64_t timestamp = 0;                       // When the transaction was created
    std::string doc_id;                          // Document root_id affected
};

// Manages all block mutations through the transaction system.
// Every content change goes through this service.
// Thread-safe. Maintains an undo/redo stack.
class BlockService
{
public:
    explicit BlockService(
        EventBus& event_bus,
        BlockDatabase& database,
        DocumentFileSystem& doc_fs,
        BlockTreeIndex& tree_index
    );

    // --- Block Insertion ---

    // Insert a new block after the specified previous sibling within a parent.
    // If previous_id is empty, inserts as the first child of parent_id.
    // Returns the new block's ID.
    [[nodiscard]] auto insert_block(
        const std::string& parent_id,
        const std::string& previous_id,
        const std::string& markdown,
        BlockType type = BlockType::Paragraph
    ) -> std::expected<std::string, std::string>;

    // Insert a new block as the last child of parent_id.
    [[nodiscard]] auto append_block(
        const std::string& parent_id,
        const std::string& markdown,
        BlockType type = BlockType::Paragraph
    ) -> std::expected<std::string, std::string>;

    // Insert a new block as the first child of parent_id.
    [[nodiscard]] auto prepend_block(
        const std::string& parent_id,
        const std::string& markdown,
        BlockType type = BlockType::Paragraph
    ) -> std::expected<std::string, std::string>;

    // --- Block Update ---

    // Replace the content of an existing block.
    auto update_block(
        const std::string& id,
        const std::string& markdown
    ) -> std::expected<void, std::string>;

    // --- Block Deletion ---

    // Delete a block and all its children.
    // Returns the list of deleted block IDs (for undo).
    [[nodiscard]] auto delete_block(const std::string& id)
        -> std::expected<std::vector<std::string>, std::string>;

    // --- Block Move ---

    // Move a block to a new position.
    // parent_id: new parent. previous_id: new previous sibling (empty = first child).
    auto move_block(
        const std::string& id,
        const std::string& new_parent_id,
        const std::string& new_previous_id
    ) -> std::expected<void, std::string>;

    // --- Heading Fold/Unfold ---

    // Fold a heading block: mark it as folded, hide all blocks
    // until the next heading of same or higher level.
    auto fold_heading(const std::string& heading_id) -> std::expected<void, std::string>;

    // Unfold a heading block: mark it as unfolded, show hidden blocks.
    auto unfold_heading(const std::string& heading_id) -> std::expected<void, std::string>;

    // --- Attribute Operations ---

    // Set attributes on a block (merge with existing IAL).
    auto set_block_attrs(
        const std::string& id,
        const InlineAttributeList& attrs
    ) -> std::expected<void, std::string>;

    // Get all attributes of a block.
    [[nodiscard]] auto get_block_attrs(const std::string& id)
        -> std::expected<InlineAttributeList, std::string>;

    // --- Block Info Queries ---

    // Get full block info including content and relationships.
    [[nodiscard]] auto get_block_info(const std::string& id)
        -> std::expected<Block, std::string>;

    // Get block content as Kramdown/Markdown source.
    [[nodiscard]] auto get_block_kramdown(const std::string& id)
        -> std::expected<std::string, std::string>;

    // Get the breadcrumb path for a block (list of ancestor blocks from root to block).
    [[nodiscard]] auto get_block_breadcrumb(const std::string& id)
        -> std::expected<std::vector<Block>, std::string>;

    // Get the document (root block) that contains a given block.
    [[nodiscard]] auto get_block_document(const std::string& id)
        -> std::expected<Block, std::string>;

    // --- Transaction System ---

    // Execute a transaction (apply all do_operations).
    // Populates undo_operations automatically.
    // Pushes the transaction onto the undo stack.
    auto perform_transaction(Transaction& txn) -> std::expected<void, std::string>;

    // Execute multiple transactions atomically.
    auto perform_transactions(std::vector<Transaction>& txns) -> std::expected<void, std::string>;

    // --- Undo/Redo ---

    // Undo the last transaction. Returns false if nothing to undo.
    [[nodiscard]] auto undo() -> std::expected<bool, std::string>;

    // Redo the last undone transaction. Returns false if nothing to redo.
    [[nodiscard]] auto redo() -> std::expected<bool, std::string>;

    // Check if undo/redo are available.
    [[nodiscard]] auto can_undo() const -> bool;
    [[nodiscard]] auto can_redo() const -> bool;

    // Clear the undo/redo history.
    auto clear_history() -> void;

    // Get the undo/redo stack sizes.
    [[nodiscard]] auto undo_stack_size() const -> size_t;
    [[nodiscard]] auto redo_stack_size() const -> size_t;

private:
    EventBus& event_bus_;
    BlockDatabase& database_;
    DocumentFileSystem& doc_fs_;
    BlockTreeIndex& tree_index_;

    // Undo/redo stacks
    std::vector<Transaction> undo_stack_;
    std::vector<Transaction> redo_stack_;
    mutable std::mutex history_mutex_;

    static constexpr size_t MAX_UNDO_HISTORY = 100;

    // --- Internal Operation Executors ---

    // Execute a single operation. Returns the inverse (undo) operation.
    [[nodiscard]] auto execute_operation(const Operation& op)
        -> std::expected<Operation, std::string>;

    // Execute insert: create block, add to parent's children, save tree.
    [[nodiscard]] auto execute_insert(const Operation& op)
        -> std::expected<Operation, std::string>;

    // Execute update: replace block content, save tree.
    [[nodiscard]] auto execute_update(const Operation& op)
        -> std::expected<Operation, std::string>;

    // Execute delete: remove block and children from tree, save.
    [[nodiscard]] auto execute_delete(const Operation& op)
        -> std::expected<Operation, std::string>;

    // Execute move: detach from old parent, attach to new parent.
    [[nodiscard]] auto execute_move(const Operation& op)
        -> std::expected<Operation, std::string>;

    // Execute fold heading: set folded=true on heading and mark children.
    [[nodiscard]] auto execute_fold_heading(const Operation& op)
        -> std::expected<Operation, std::string>;

    // Execute unfold heading: set folded=false and restore children.
    [[nodiscard]] auto execute_unfold_heading(const Operation& op)
        -> std::expected<Operation, std::string>;

    // Execute set attrs: merge attrs into block's IAL.
    [[nodiscard]] auto execute_set_attrs(const Operation& op)
        -> std::expected<Operation, std::string>;

    // --- Tree Manipulation Helpers ---

    // Load the document tree containing a block.
    [[nodiscard]] auto load_containing_tree(const std::string& block_id)
        -> std::expected<std::shared_ptr<Block>, std::string>;

    // Find a block within a loaded tree by ID.
    [[nodiscard]] auto find_block_in_tree(
        const std::shared_ptr<Block>& root,
        const std::string& block_id
    ) -> std::shared_ptr<Block>;

    // Find the parent of a block within a loaded tree.
    [[nodiscard]] auto find_parent_in_tree(
        const std::shared_ptr<Block>& root,
        const std::string& child_id
    ) -> std::shared_ptr<Block>;

    // Insert a child block after a sibling in the parent's children vector.
    auto insert_child_after(
        std::shared_ptr<Block>& parent,
        std::shared_ptr<Block> child,
        const std::string& after_id
    ) -> void;

    // Remove a child block from its parent's children vector.
    // Returns the removed block (with its subtree intact).
    [[nodiscard]] auto remove_child(
        std::shared_ptr<Block>& parent,
        const std::string& child_id
    ) -> std::shared_ptr<Block>;

    // Get all blocks under a heading until the next heading of same or higher level.
    [[nodiscard]] auto get_heading_children(
        const std::shared_ptr<Block>& parent,
        const std::string& heading_id
    ) -> std::vector<std::shared_ptr<Block>>;

    // Save a modified tree to disk and update all indexes.
    auto save_and_reindex(
        const std::string& box_id,
        const std::string& path,
        const std::shared_ptr<Block>& root
    ) -> std::expected<void, std::string>;

    // Recalculate sort values for children of a parent block.
    auto recalculate_sort_order(std::shared_ptr<Block>& parent) -> void;

    // Update timestamps on a block and its ancestors up to root.
    auto touch_block(
        const std::shared_ptr<Block>& root,
        const std::string& block_id
    ) -> void;
};

} // namespace markamp::core
```

## Key Functions to Implement

1. **`insert_block(parent_id, previous_id, markdown, type)`** — Create a Transaction with one Insert operation. Call `perform_transaction()`. The operation internally: generates a new block ID, creates a Block from the markdown, loads the containing document tree, finds the parent block, inserts the new block after `previous_id` (or as first child if empty), recalculates sort order, saves the tree, updates database and tree index. Returns the new block ID.

2. **`append_block(parent_id, markdown, type)`** — Same as insert but places the block after the last child. Implemented by calling `insert_block` with `previous_id` set to the last child's ID.

3. **`prepend_block(parent_id, markdown, type)`** — Same as insert but with `previous_id = ""` (inserts as first child).

4. **`update_block(id, markdown)`** — Create a Transaction with one Update operation. The operation internally: loads the containing tree, finds the target block, preserves the old content (for undo), replaces the markdown/content fields, saves the tree.

5. **`delete_block(id)`** — Create a Transaction with one Delete operation. The operation internally: loads the tree, finds the block, removes it from its parent's children, saves the tree. The undo operation stores the entire removed subtree as serialized data so it can be restored.

6. **`move_block(id, new_parent_id, new_previous_id)`** — Create a Transaction with one Move operation. The operation internally: loads the tree(s) — if moving across documents, load both trees. Remove from old parent. Insert into new parent after `new_previous_id`. Recalculate sort orders in both old and new parents. Save affected trees.

7. **`fold_heading(heading_id)`** — Verify the block is a heading. Set `folded = true` on the heading block. Mark all blocks between this heading and the next heading of same or higher level as hidden (set `fold` attribute in IAL). Save tree.

8. **`unfold_heading(heading_id)`** — Verify the block is a folded heading. Set `folded = false`. Remove `fold` attribute from affected blocks. Save tree.

9. **`perform_transaction(txn)`** — For each operation in `do_operations`: call `execute_operation()`, which returns the inverse operation. Collect all inverse operations into `undo_operations`. If any operation fails, rollback by executing the already-collected undo operations in reverse order. On success, push transaction onto undo stack (clearing redo stack).

10. **`undo()`** — Pop from undo stack. Execute the `undo_operations`. Push the transaction (with do/undo swapped) onto redo stack.

11. **`redo()`** — Pop from redo stack. Execute the `do_operations`. Push back onto undo stack.

12. **`load_containing_tree(block_id)`** — Look up block in BlockTreeIndex to get box_id and path. Call `doc_fs_.load_tree(box_id, path)`. Return the root.

13. **`find_block_in_tree(root, block_id)`** — Recursive depth-first search through the tree. Return the block with matching ID, or nullptr.

14. **`get_heading_children(parent, heading_id)`** — Find the heading block in parent's children. Collect all subsequent siblings until encountering a heading of same or higher level (lower number). Return the collected blocks.

15. **`save_and_reindex(box_id, path, root)`** — Call `doc_fs_.save_tree()`. Call `tree_index_.index_tree()`. Fire appropriate events.

## Events to Add (in Events.h)

```cpp
MARKAMP_DECLARE_EVENT(TransactionPerformedEvent, std::string /* txn_id */, int /* operation_count */)
MARKAMP_DECLARE_EVENT(TransactionUndoneEvent, std::string /* txn_id */)
MARKAMP_DECLARE_EVENT(TransactionRedoneEvent, std::string /* txn_id */)
MARKAMP_DECLARE_EVENT(BlockInsertedEvent, std::string /* block_id */, std::string /* parent_id */, std::string /* doc_id */)
MARKAMP_DECLARE_EVENT(BlockContentUpdatedEvent, std::string /* block_id */, std::string /* doc_id */)
MARKAMP_DECLARE_EVENT(HeadingFoldedEvent, std::string /* heading_id */, bool /* folded */, int /* affected_block_count */)
```

## Config Keys to Add

- `knowledgebase.max_undo_history` — Maximum number of transactions in the undo stack. Default: `100`. Type: int.
- `knowledgebase.auto_save_interval_ms` — Milliseconds between automatic saves of modified documents. Default: `3000`. Type: int.

## Test Cases (Catch2)

File: `tests/unit/test_block_service.cpp`

All tests create a temporary data directory with a notebook and a document to operate on.

1. **Insert block after sibling** — Create a document with 3 paragraphs (A, B, C). Insert a new block after B. Verify: document now has 4 children in order A, B, NEW, C. Verify NEW has correct parent_id and sort order.

2. **Append block** — Create a document with 2 paragraphs. Append a new block. Verify: 3 children, new block is last.

3. **Prepend block** — Create a document with 2 paragraphs. Prepend a new block. Verify: 3 children, new block is first.

4. **Update block content** — Create a document with a paragraph "old text". Update to "new text". Reload from disk. Verify content is "new text".

5. **Delete block** — Create a document with 3 paragraphs. Delete the second. Verify: 2 children remain (first and third). Verify deleted block and children are removed from index.

6. **Delete block with children** — Create a document with a list containing 3 list items. Delete the list block. Verify: all 4 blocks (list + 3 items) are removed.

7. **Move block within document** — Create a document with paragraphs A, B, C. Move C to after A. Verify order: A, C, B.

8. **Move block between documents** — Create two documents in the same notebook. Move a block from doc 1 to doc 2. Verify: block gone from doc 1, present in doc 2. Verify box_id, path, root_id updated.

9. **Fold heading** — Create a document: H2, P1, P2, H2, P3. Fold the first H2. Verify: H2.folded == true, P1 and P2 have fold attribute. P3 does not (it belongs to the second H2 section).

10. **Unfold heading** — Fold a heading (as above). Unfold it. Verify: folded == false, fold attribute removed from P1 and P2.

11. **Set block attrs** — Create a block. Set attrs `{custom-color: "red", name: "MyBlock"}`. Verify `get_block_attrs()` returns both. Set `{custom-color: "blue"}`. Verify custom-color is now "blue" and name is still "MyBlock" (merge, not replace).

12. **Undo insert** — Insert a block. Call `undo()`. Verify the block is gone. Verify `can_redo()` is true.

13. **Redo insert** — Insert, undo, redo. Verify the block is back with the same ID and content.

14. **Undo delete** — Delete a block. Undo. Verify the block is restored with its original content and position.

15. **Undo move** — Move a block from position A to B. Undo. Verify block is back at position A.

16. **Multiple undo** — Perform 5 operations (insert, update, insert, delete, move). Undo all 5 in order. Verify document is back to original state.

17. **Undo clears redo stack** — Insert A. Undo. Insert B. Verify `can_redo()` is false (redo stack cleared because a new operation was performed).

18. **Transaction atomicity** — Create a transaction with 3 operations where the third will fail (e.g., move to a nonexistent parent). Verify: none of the 3 operations were applied (all rolled back).

19. **Get block breadcrumb** — Create a nested structure: Document > List > ListItem > Paragraph. Call `get_block_breadcrumb(paragraph_id)`. Verify: returns [Document, List, ListItem, Paragraph] in order.

20. **Concurrent operations** — Launch 4 threads, each performing insert operations on the same document. Verify no crashes. Verify final block count equals expected.

## Acceptance Criteria

- [ ] All 9 operation types (Insert, Append, Prepend, Update, Delete, Move, FoldHeading, UnfoldHeading, SetAttrs) work correctly
- [ ] Every mutation saves the document to disk and updates all indexes (database, tree index)
- [ ] Transaction system captures undo operations automatically for every do operation
- [ ] Undo/redo works correctly for all operation types
- [ ] Undo stack limited to configurable max (default 100) — oldest transactions discarded
- [ ] New action clears the redo stack
- [ ] Transaction atomicity: if any operation fails, all previous operations in the same transaction are rolled back
- [ ] Fold/unfold heading correctly identifies blocks in the heading's section
- [ ] Cross-document move updates all block metadata (box_id, path, root_id)
- [ ] Events fired for all mutations
- [ ] All 20 test cases pass
- [ ] Uses `[[nodiscard]]` on all query methods
- [ ] No use of `catch(...)` — use typed exception handlers
- [ ] Allman brace style, 4-space indent, trailing return types

## Files to Create/Modify

- CREATE: `src/core/BlockService.h`
- CREATE: `src/core/BlockService.cpp`
- MODIFY: `src/core/Events.h` (add 6 transaction/block operation events)
- MODIFY: `src/core/Config.cpp` (add `knowledgebase.max_undo_history` and `knowledgebase.auto_save_interval_ms` defaults)
- MODIFY: `src/core/PluginContext.h` (add BlockService pointer if extending PluginContext)
- MODIFY: `src/CMakeLists.txt` (add `core/BlockService.cpp` to source list)
- CREATE: `tests/unit/test_block_service.cpp`
- MODIFY: `tests/CMakeLists.txt` (add `test_block_service` target)
