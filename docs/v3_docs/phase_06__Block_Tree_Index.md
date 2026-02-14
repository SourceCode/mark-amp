# Phase 06 — Block Tree In-Memory Index

## Objective

Implement the in-memory block tree index, porting SiYuan's `treenode/blocktree.go`. This is a fast lookup table that maps every block ID in the knowledge base to its location metadata (root_id, parent_id, path, box_id, hpath, type, updated). The index is backed by SQLite for persistence but loaded entirely into memory on startup for O(1) lookups. This avoids the need for a database query every time the system needs to locate which document a block belongs to or what type it is.

## Prerequisites

- Phase 01 (Block Data Model) — Block struct and BlockType enum
- Phase 02 (Block ID Generation) — Block IDs used as keys
- Phase 04 (SQLite Storage Layer) — Persistence backend for the index

## SiYuan Source Reference

- `kernel/treenode/blocktree.go` — Core data structures and functions:
  - `BlockTree` struct: `ID string`, `RootID string`, `ParentID string`, `BoxID string`, `Path string`, `HPath string`, `Updated string`, `Type string`
  - `InitBlockTree()` — Loads all block trees from SQLite into memory at startup
  - `IndexBlockTree(tree *parse.Tree)` — Indexes all blocks in a parsed document tree
  - `GetBlockTree(id string)` — O(1) lookup by block ID
  - `GetBlockTreeByPath(box, path string)` — Lookup by notebook+path
  - `GetBlockTreesByBoxID(boxID string)` — Get all block trees in a notebook
  - `GetBlockTreeRootByPath(box, path string)` — Get root block tree for a document
  - `UpsertBlockTree(bt *BlockTree)` — Insert or update a single entry
  - `RemoveBlockTree(id string)` — Remove a single entry
  - `RemoveBlockTreeByBoxID(boxID string)` — Remove all entries for a notebook
  - `RemoveBlockTreeByRootID(rootID string)` — Remove all entries for a document
  - `CountBlockTrees()` — Total number of indexed blocks
  - `CountBlockTreesByBoxID(boxID string)` — Blocks in a specific notebook
- `kernel/treenode/blocktree.go` — Uses `sync.Mutex` for thread safety and a `map[string]*BlockTree` for O(1) lookups

## MarkAmp Integration Points

- New header: `src/core/BlockTreeIndex.h`
- New source: `src/core/BlockTreeIndex.cpp`
- Depends on: Block.h, BlockDatabase.h, EventBus
- Used by: DocumentFileSystem (Phase 05) for path resolution, BlockService (Phase 07) for block location, IAL system (Phase 08) for attribute lookups
- Subscribes to EventBus events: BlockCreatedEvent, BlockDeletedEvent, BlockMovedEvent, NotebookRemovedEvent, NotebookClosedEvent, DocumentRemovedEvent

## Data Structures to Implement

```cpp
namespace markamp::core
{

// Lightweight index entry for a single block.
// Contains only the metadata needed for location and type lookups.
// Intentionally minimal — does NOT contain content or markdown.
// Size: approximately 200 bytes per entry on 64-bit systems.
struct BlockTreeEntry
{
    std::string id;         // Block ID (primary key in the map)
    std::string root_id;    // Document root block ID
    std::string parent_id;  // Parent block ID
    std::string box_id;     // Notebook ID
    std::string path;       // Document file path (e.g., "/20260101-abc.sy")
    std::string hpath;      // Human-readable path (e.g., "/Notebook/Document")
    std::string updated;    // Last update timestamp (YYYYMMDDHHmmss format)
    BlockType type = BlockType::Unknown;  // Block type for quick filtering
};

// In-memory index of all blocks in the knowledge base.
// Provides O(1) lookups by block ID and efficient lookups by root_id, box_id, path.
// Thread-safe via shared_mutex (multiple concurrent readers, exclusive writer).
//
// Memory usage estimate: ~200 bytes per block entry.
//   10,000 blocks ~= 2 MB
//  100,000 blocks ~= 20 MB
//  1,000,000 blocks ~= 200 MB
class BlockTreeIndex
{
public:
    explicit BlockTreeIndex(EventBus& event_bus, BlockDatabase& database);
    ~BlockTreeIndex();

    // --- Initialization ---

    // Load all block tree entries from the database into memory.
    // Called once at startup. Progress reported via events.
    auto init() -> std::expected<void, std::string>;

    // Clear the in-memory index (does NOT clear the database).
    auto clear() -> void;

    // --- Single Entry Operations ---

    // Look up a block by ID. O(1) average time.
    [[nodiscard]] auto get(const std::string& id) const -> std::optional<BlockTreeEntry>;

    // Insert or update a single entry.
    auto upsert(const BlockTreeEntry& entry) -> void;

    // Remove a single entry by ID.
    auto remove(const std::string& id) -> void;

    // --- Batch Operations ---

    // Index an entire document tree. Extracts BlockTreeEntry from each block
    // in the tree and upserts all entries.
    auto index_tree(
        const std::string& box_id,
        const std::string& path,
        const std::string& hpath,
        const std::shared_ptr<Block>& root
    ) -> void;

    // Remove all entries for a document (by root_id).
    auto remove_by_root(const std::string& root_id) -> void;

    // Remove all entries for a notebook (by box_id).
    auto remove_by_box(const std::string& box_id) -> void;

    // --- Query by Root ---

    // Get all block entries belonging to a document.
    [[nodiscard]] auto get_by_root(const std::string& root_id) const
        -> std::vector<BlockTreeEntry>;

    // Get just the root (Document) entry for a document.
    [[nodiscard]] auto get_root_entry(const std::string& root_id) const
        -> std::optional<BlockTreeEntry>;

    // --- Query by Path ---

    // Get the root entry for a document at a specific path in a notebook.
    [[nodiscard]] auto get_root_by_path(
        const std::string& box_id,
        const std::string& path
    ) const -> std::optional<BlockTreeEntry>;

    // Get all entries for documents at a specific path.
    [[nodiscard]] auto get_by_path(
        const std::string& box_id,
        const std::string& path
    ) const -> std::vector<BlockTreeEntry>;

    // --- Query by Notebook ---

    // Get all block entries in a notebook.
    [[nodiscard]] auto get_by_box(const std::string& box_id) const
        -> std::vector<BlockTreeEntry>;

    // Get all root (Document) entries in a notebook.
    [[nodiscard]] auto get_roots_by_box(const std::string& box_id) const
        -> std::vector<BlockTreeEntry>;

    // --- Query by Type ---

    // Get all block entries of a specific type across the entire index.
    [[nodiscard]] auto get_by_type(BlockType type) const -> std::vector<BlockTreeEntry>;

    // --- Statistics ---

    // Total number of indexed blocks.
    [[nodiscard]] auto count() const -> int64_t;

    // Number of indexed blocks in a specific notebook.
    [[nodiscard]] auto count_by_box(const std::string& box_id) const -> int64_t;

    // Number of indexed blocks in a specific document.
    [[nodiscard]] auto count_by_root(const std::string& root_id) const -> int64_t;

    // Number of indexed documents (root blocks).
    [[nodiscard]] auto count_documents() const -> int64_t;

    // --- Validation ---

    // Check if a block ID exists in the index.
    [[nodiscard]] auto contains(const std::string& id) const -> bool;

    // Verify index consistency: every entry's root_id and parent_id should
    // also exist in the index (except root blocks whose parent_id is empty).
    // Returns IDs of orphaned blocks.
    [[nodiscard]] auto find_orphans() const -> std::vector<std::string>;

private:
    EventBus& event_bus_;
    BlockDatabase& database_;

    // Primary index: block_id -> BlockTreeEntry
    // O(1) lookup by block ID.
    std::unordered_map<std::string, BlockTreeEntry> entries_;

    // Secondary indexes (maintained in parallel for query efficiency)

    // root_id -> set of block IDs belonging to that document
    std::unordered_map<std::string, std::unordered_set<std::string>> root_index_;

    // box_id -> set of block IDs belonging to that notebook
    std::unordered_map<std::string, std::unordered_set<std::string>> box_index_;

    // (box_id + ":" + path) -> set of block IDs at that path
    std::unordered_map<std::string, std::unordered_set<std::string>> path_index_;

    // Readers-writer lock. Reads are frequent and concurrent; writes are batched.
    mutable std::shared_mutex mutex_;

    // --- Internal helpers (caller must hold appropriate lock) ---

    // Add an entry to all indexes. Caller must hold exclusive lock.
    auto add_to_indexes_unlocked(const BlockTreeEntry& entry) -> void;

    // Remove an entry from all indexes. Caller must hold exclusive lock.
    auto remove_from_indexes_unlocked(const std::string& id) -> void;

    // Build a path key for the path_index_ map.
    [[nodiscard]] static auto path_key(
        const std::string& box_id,
        const std::string& path
    ) -> std::string;

    // Convert a Block (from a tree) to a BlockTreeEntry.
    [[nodiscard]] static auto block_to_entry(
        const Block& block,
        const std::string& box_id,
        const std::string& path,
        const std::string& hpath
    ) -> BlockTreeEntry;

    // --- Event handlers ---

    // Subscribe to relevant events during construction.
    auto subscribe_to_events() -> void;
};

} // namespace markamp::core
```

## Key Functions to Implement

1. **`init()`** — Query the database for all blocks (just the columns needed for BlockTreeEntry, not full content). Load each row as a BlockTreeEntry into `entries_` and the secondary indexes. Log the total count. Fire `BlockTreeIndexInitializedEvent`. Expected to take < 1 second for 100,000 blocks.

2. **`get(id)`** — Acquire shared lock. Look up `id` in `entries_`. Return the entry or nullopt. Must be fast (this is the most-called function in the system).

3. **`upsert(entry)`** — Acquire exclusive lock. If entry already exists, remove from old secondary indexes. Insert/update in `entries_`. Add to secondary indexes. This is O(1) amortized.

4. **`remove(id)`** — Acquire exclusive lock. Look up entry. Remove from `entries_` and all secondary indexes. No-op if ID not found.

5. **`index_tree(box_id, path, hpath, root)`** — Acquire exclusive lock. Recursively traverse the block tree. For each block, create a BlockTreeEntry and call `add_to_indexes_unlocked()`. This replaces any existing entries for the same root_id (remove old entries first).

6. **`remove_by_root(root_id)`** — Acquire exclusive lock. Look up all IDs in `root_index_[root_id]`. Remove each from `entries_`, `box_index_`, and `path_index_`. Remove the root_index_ entry.

7. **`remove_by_box(box_id)`** — Acquire exclusive lock. Look up all IDs in `box_index_[box_id]`. Remove each from `entries_`, `root_index_`, and `path_index_`. Remove the box_index_ entry.

8. **`get_by_root(root_id)`** — Acquire shared lock. Look up IDs in `root_index_`. Collect corresponding entries from `entries_`. Return as vector.

9. **`get_root_by_path(box_id, path)`** — Acquire shared lock. Look up IDs via `path_index_[path_key(box_id, path)]`. Find the entry with type == Document. Return it or nullopt.

10. **`count()`** — Acquire shared lock. Return `entries_.size()`.

11. **`find_orphans()`** — Acquire shared lock. For each entry, check: if `parent_id` is not empty, does `entries_` contain `parent_id`? If not, the block is orphaned. Return list of orphan IDs. Used for index consistency validation.

12. **`subscribe_to_events()`** — Subscribe to:
    - `NotebookRemovedEvent` -> `remove_by_box()`
    - `NotebookClosedEvent` -> `remove_by_box()`
    - `DocumentRemovedEvent` -> `remove_by_root()`
    - Other events handled by the callers (DocumentFileSystem, BlockService) who explicitly call `index_tree()` / `remove()`.

## Events to Add (in Events.h)

```cpp
MARKAMP_DECLARE_EVENT(BlockTreeIndexInitializedEvent, int64_t /* total_entries */)
MARKAMP_DECLARE_EVENT(BlockTreeIndexUpdatedEvent, std::string /* root_id */, int /* entry_count */)
```

## Config Keys to Add

- None for this phase. The index uses the same database configured in Phase 04.

## Test Cases (Catch2)

File: `tests/unit/test_block_tree_index.cpp`

1. **Init from empty database** — Create an empty BlockDatabase. Create a BlockTreeIndex. Call `init()`. Verify `count()` returns 0.

2. **Init from populated database** — Insert 50 blocks into the database (across 5 documents in 2 notebooks). Create a BlockTreeIndex. Call `init()`. Verify `count()` returns 50. Verify `count_documents()` returns 5.

3. **Get by ID** — Index a tree. Call `get(block_id)` for each block. Verify all fields match: id, root_id, parent_id, box_id, path, hpath, type.

4. **Get returns nullopt for missing ID** — Call `get("nonexistent-id")`. Verify returns nullopt. Call `contains("nonexistent-id")`. Verify returns false.

5. **Upsert new entry** — Upsert a single entry. Verify `get()` returns it. Verify `count()` increased by 1. Verify secondary indexes are updated (`get_by_root()`, `get_by_box()`).

6. **Upsert existing entry (update)** — Upsert an entry. Change its `updated` field. Upsert again. Verify `get()` returns the updated version. Verify `count()` did not change.

7. **Remove entry** — Index a tree. Remove one block ID. Verify `get()` returns nullopt for that ID. Verify `count()` decreased by 1. Verify removed ID is no longer in `get_by_root()` results.

8. **Remove by root** — Index a tree with 20 blocks. Call `remove_by_root(root_id)`. Verify all 20 blocks are gone from the index. Verify `count_by_root(root_id)` returns 0.

9. **Remove by box** — Index trees from 2 notebooks (25 blocks each). Call `remove_by_box(box_id_1)`. Verify: `count_by_box(box_id_1)` returns 0, `count_by_box(box_id_2)` returns 25, total `count()` returns 25.

10. **Index tree replaces existing** — Index a tree with root_id R and 10 blocks. Modify the tree (add 5 blocks, remove 3). Re-index. Verify `count_by_root(R)` reflects the new count (12). Verify old removed blocks are not in the index.

11. **Get root by path** — Index 3 documents with different paths in the same notebook. Call `get_root_by_path(box_id, path)` for each. Verify each returns the correct Document entry.

12. **Concurrent read/write** — Launch 4 reader threads (calling `get()` in a loop with random IDs) and 2 writer threads (calling `upsert()` and `remove()` in a loop). Run for 200ms. Verify no crashes or deadlocks. Verify index is in a consistent state at the end.

13. **Find orphans** — Index a tree. Manually remove a parent block (but leave its children). Call `find_orphans()`. Verify the children are reported as orphans.

14. **Count statistics** — Index 3 notebooks with 2, 3, and 5 documents respectively. Each document has 10 blocks. Verify: `count()` = 100, `count_documents()` = 10, `count_by_box()` for each notebook returns 20/30/50, `count_by_root()` for each document returns 10.

## Acceptance Criteria

- [ ] O(1) block ID lookup via `get()`
- [ ] Secondary indexes maintained for root_id, box_id, and path queries
- [ ] Thread-safe via `std::shared_mutex` (concurrent readers, exclusive writers)
- [ ] `init()` loads from database in under 1 second for 100,000 blocks
- [ ] `index_tree()` correctly replaces all entries for a document
- [ ] `remove_by_root()` and `remove_by_box()` clean up all secondary indexes
- [ ] Event subscriptions auto-clean index on notebook removal/close and document removal
- [ ] `find_orphans()` correctly detects inconsistent parent references
- [ ] All 14 test cases pass
- [ ] Memory usage is proportional to block count (~200 bytes per entry)
- [ ] Uses `[[nodiscard]]` on all query methods
- [ ] No use of `catch(...)` — use typed exception handlers
- [ ] Allman brace style, 4-space indent, trailing return types

## Files to Create/Modify

- CREATE: `src/core/BlockTreeIndex.h`
- CREATE: `src/core/BlockTreeIndex.cpp`
- MODIFY: `src/core/Events.h` (add 2 block tree index events)
- MODIFY: `src/CMakeLists.txt` (add `core/BlockTreeIndex.cpp` to source list)
- CREATE: `tests/unit/test_block_tree_index.cpp`
- MODIFY: `tests/CMakeLists.txt` (add `test_block_tree_index` target)

## Performance Notes

- The primary design goal is read performance. `get()` is called extremely frequently (every time a block reference is resolved, every time a backlink is followed, every time the UI needs to know which document a block belongs to).
- Write operations (upsert, remove) can be slightly slower since they require an exclusive lock, but they should still be fast enough for interactive editing (< 1ms per operation).
- The secondary indexes (root_index_, box_index_, path_index_) trade memory for query speed. They should be kept in sync with the primary index at all times.
- For very large knowledge bases (1M+ blocks), consider replacing `std::unordered_map` with a more memory-efficient hash map (e.g., `absl::flat_hash_map` or `robin_map`). This is an optional future optimization.
