# Phase 04 — SQLite Storage Layer & Schema

## Objective

Implement the SQLite database layer for block indexing and querying, porting SiYuan's `sql` package. This is the backbone of search, backlinks, attribute queries, and all block lookups that do not require loading the full document tree. Uses SQLite3 directly with prepared statements (no ORM) for maximum control and performance. Includes an asynchronous write queue that coalesces operations to avoid blocking the UI thread.

## Prerequisites

- Phase 01 (Block Data Model) — Block struct is stored in and retrieved from the database
- Phase 02 (Block ID Generation) — Block IDs used as primary keys

## SiYuan Source Reference

- `kernel/sql/database.go` — Database initialization (`InitDatabase`, `InitDBConnection`), schema management (`upgradeDatabase`), WAL mode, busy timeout, journaling. Uses `github.com/mattn/go-sqlite3` driver.
- `kernel/sql/block.go` — SQL Block struct: `id, parent_id, root_id, hash, box, path, hpath, name, alias, memo, tag, content, fcontent, markdown, length, type, subtype, ial, sort, created, updated`
- `kernel/sql/upsert.go` — `UpsertBlocks()`, `UpsertRefs()` — Bulk insert/update within transactions. Uses `INSERT OR REPLACE`.
- `kernel/sql/queue.go` — Async transaction queue (`ExecStmtsQueue`, `FlushQueue`). Write operations are queued and executed in batches on a dedicated goroutine.
- `kernel/sql/block_query.go` — `GetBlock()`, `GetBlocks()`, `GetChildBlocks()`, `GetRootBlocks()`, `GetBlocksByType()`, `QueryBlockByContent()`
- `kernel/sql/block_ref.go` — `UpsertRefs()`, `DeleteRefsByBlockID()`, refs table operations
- `kernel/sql/block_ref_query.go` — `QueryRefsByDefBlockID()`, `QueryRefsByBlockID()`, `DefRefs()`, `RefCount()`
- `kernel/sql/stat.go` — `CountBlocks()`, `CountBlocksByBox()`

## MarkAmp Integration Points

- New header: `src/core/BlockDatabase.h`
- New source: `src/core/BlockDatabase.cpp`
- SQLite3 dependency: link against system `sqlite3` or bundle `sqlite3.c` amalgamation
- Async write queue runs on a dedicated `std::jthread`
- Integrates with EventBus for index lifecycle events

## Data Structures to Implement

```cpp
namespace markamp::core
{

// Types of database operations that can be queued.
enum class DBOperation : uint8_t
{
    UpsertBlock,       // Insert or replace a block row
    DeleteBlock,       // Delete a block row by ID
    UpsertRef,         // Insert or replace a reference row
    DeleteRef,         // Delete reference rows by block ID
    DeleteBlocksByRoot,// Delete all blocks with a given root_id
    DeleteBlocksByBox, // Delete all blocks with a given box_id
    RebuildIndex,      // Full rebuild of FTS index
};

// A single queued database operation.
struct DBQueueItem
{
    DBOperation op;
    std::string primary_key;   // Block ID or root_id depending on operation
    std::string data_json;     // Serialized operation data (JSON string)
    int priority = 0;          // Higher priority items processed first
};

// Core SQLite database interface for block storage and querying.
// All read operations are synchronous (fast due to indexes).
// All write operations can be queued for async execution.
class BlockDatabase
{
public:
    // Construct with path to the SQLite database file.
    // The database is NOT opened in the constructor — call open() explicitly.
    explicit BlockDatabase(const std::filesystem::path& db_path);

    // Destructor calls close() if database is still open.
    ~BlockDatabase();

    // Non-copyable, non-movable (owns sqlite3* handle)
    BlockDatabase(const BlockDatabase&) = delete;
    auto operator=(const BlockDatabase&) -> BlockDatabase& = delete;
    BlockDatabase(BlockDatabase&&) = delete;
    auto operator=(BlockDatabase&&) -> BlockDatabase& = delete;

    // --- Lifecycle ---

    // Open the database. Creates the file if it doesn't exist.
    // Enables WAL mode, foreign keys, busy timeout of 10 seconds.
    auto open() -> std::expected<void, std::string>;

    // Close the database. Flushes pending writes, finalizes statements.
    auto close() -> void;

    // Returns true if the database is currently open.
    [[nodiscard]] auto is_open() const -> bool;

    // --- Schema Management ---

    // Create all tables, indexes, FTS virtual table, and schema_info.
    // Safe to call multiple times (uses IF NOT EXISTS).
    auto init_tables() -> std::expected<void, std::string>;

    // Upgrade schema from one version to another.
    // Each version step applies migration SQL.
    auto upgrade_schema(int from_version, int to_version) -> std::expected<void, std::string>;

    // Get the current schema version from the schema_info table.
    [[nodiscard]] auto schema_version() const -> int;

    // --- Block CRUD ---

    // Insert or replace a single block. Synchronous.
    auto upsert_block(const Block& block) -> std::expected<void, std::string>;

    // Insert or replace multiple blocks within a single transaction.
    auto upsert_blocks(const std::vector<Block>& blocks) -> std::expected<void, std::string>;

    // Delete a single block by ID.
    auto delete_block(const std::string& id) -> std::expected<void, std::string>;

    // Delete all blocks belonging to a document (by root_id).
    auto delete_blocks_by_root(const std::string& root_id) -> std::expected<void, std::string>;

    // Delete all blocks belonging to a notebook (by box_id).
    auto delete_blocks_by_box(const std::string& box_id) -> std::expected<void, std::string>;

    // --- Block Queries ---

    // Get a single block by ID. Returns nullopt if not found.
    [[nodiscard]] auto get_block(const std::string& id) -> std::optional<Block>;

    // Get multiple blocks by IDs. Returns only blocks that exist.
    [[nodiscard]] auto get_blocks(const std::vector<std::string>& ids) -> std::vector<Block>;

    // Get all direct children of a parent block, sorted by sort field.
    [[nodiscard]] auto get_child_blocks(const std::string& parent_id) -> std::vector<Block>;

    // Get all root (document) blocks in a notebook.
    [[nodiscard]] auto get_root_blocks(const std::string& box_id) -> std::vector<Block>;

    // Get all blocks of a specific type across the entire database.
    [[nodiscard]] auto get_blocks_by_type(BlockType type) -> std::vector<Block>;

    // --- Reference CRUD ---

    // Insert or replace a reference link (block_id references def_block_id).
    auto upsert_ref(
        const std::string& block_id,
        const std::string& def_block_id,
        const std::string& def_block_root_id,
        const std::string& content
    ) -> std::expected<void, std::string>;

    // Delete all outgoing references from a block.
    auto delete_refs_by_block(const std::string& block_id) -> std::expected<void, std::string>;

    // --- Reference Queries ---

    // Find all blocks that reference the given definition block (backlinks).
    [[nodiscard]] auto get_refs_by_def(const std::string& def_block_id) -> std::vector<BlockRef>;

    // Find all outgoing references from a block.
    [[nodiscard]] auto get_refs_by_block(const std::string& block_id) -> std::vector<BlockRef>;

    // Count how many blocks reference the given definition block.
    [[nodiscard]] auto get_ref_count(const std::string& def_block_id) -> int64_t;

    // --- Full-Text Search ---

    // Search blocks by content using FTS5 MATCH syntax.
    // Returns blocks ranked by relevance. Limit defaults to 64.
    [[nodiscard]] auto search_blocks(
        const std::string& query,
        int limit = 64
    ) -> std::vector<Block>;

    // Search blocks within a specific notebook.
    [[nodiscard]] auto search_blocks_in_box(
        const std::string& box_id,
        const std::string& query,
        int limit = 64
    ) -> std::vector<Block>;

    // --- Attribute Queries ---

    // Get the IAL for a specific block from the database.
    [[nodiscard]] auto get_block_attrs(const std::string& id) -> InlineAttributeList;

    // Update the IAL for a specific block. Merges with existing attributes.
    auto set_block_attrs(
        const std::string& id,
        const InlineAttributeList& attrs
    ) -> std::expected<void, std::string>;

    // --- Statistics ---

    // Total number of blocks in the database.
    [[nodiscard]] auto count_blocks() -> int64_t;

    // Number of blocks in a specific notebook.
    [[nodiscard]] auto count_blocks_by_box(const std::string& box_id) -> int64_t;

    // Total number of reference links in the database.
    [[nodiscard]] auto count_refs() -> int64_t;

    // --- Raw SQL ---

    // Execute an arbitrary SELECT query. Returns results as vector of row maps.
    // Each row is an unordered_map of column_name -> value_string.
    // WARNING: Only use for read-only queries. Caller is responsible for SQL safety.
    [[nodiscard]] auto query_raw(const std::string& sql)
        -> std::expected<std::vector<std::unordered_map<std::string, std::string>>, std::string>;

    // --- Async Write Queue ---

    // Add an operation to the write queue. Operations are processed in batch
    // on a background thread.
    auto queue_operation(DBQueueItem item) -> void;

    // Block until all queued operations have been processed.
    auto flush_queue() -> void;

    // Process all currently queued operations. Returns the number processed.
    // Called internally by the background thread, but can also be called
    // manually for testing.
    auto process_queue() -> int;

    // Start the background write thread.
    auto start_queue_worker() -> void;

    // Stop the background write thread (flushes first).
    auto stop_queue_worker() -> void;

private:
    std::filesystem::path db_path_;
    sqlite3* db_ = nullptr;

    // --- Prepared Statements Cache ---
    // Keyed by a descriptive name (e.g., "upsert_block", "get_block_by_id").
    std::unordered_map<std::string, sqlite3_stmt*> stmts_;

    // Prepare and cache a statement. Returns cached version if already prepared.
    auto prepare(const std::string& key, const std::string& sql) -> sqlite3_stmt*;

    // Finalize all cached prepared statements.
    auto finalize_statements() -> void;

    // --- Write Queue ---
    std::queue<DBQueueItem> write_queue_;
    std::mutex queue_mutex_;
    std::condition_variable queue_cv_;
    std::jthread queue_worker_;
    std::atomic<bool> queue_running_{false};

    // Background thread function.
    auto queue_worker_fn(std::stop_token stop_token) -> void;

    // --- Transaction Helpers ---
    auto begin_transaction() -> std::expected<void, std::string>;
    auto commit_transaction() -> std::expected<void, std::string>;
    auto rollback_transaction() -> void;

    // --- Row Mapping ---
    // Convert a sqlite3_stmt result row into a Block struct.
    [[nodiscard]] auto row_to_block(sqlite3_stmt* stmt) const -> Block;

    // Convert a sqlite3_stmt result row into a BlockRef struct.
    [[nodiscard]] auto row_to_ref(sqlite3_stmt* stmt) const -> BlockRef;

    // Serialize a Block to SQL bind parameters.
    auto bind_block(sqlite3_stmt* stmt, const Block& block) const -> void;

    // --- Constants ---
    static constexpr int CURRENT_SCHEMA_VERSION = 1;
    static constexpr int BUSY_TIMEOUT_MS = 10000;
    static constexpr int QUEUE_BATCH_SIZE = 500;
};

} // namespace markamp::core
```

## SQL Schema

The following SQL creates all tables, indexes, and the FTS5 virtual table. This is executed by `init_tables()`.

```sql
-- Enable WAL mode for concurrent readers + single writer
PRAGMA journal_mode=WAL;
PRAGMA foreign_keys=ON;
PRAGMA busy_timeout=10000;

-- Main blocks table. One row per block in the knowledge base.
CREATE TABLE IF NOT EXISTS blocks (
    id TEXT PRIMARY KEY,
    parent_id TEXT NOT NULL DEFAULT '',
    root_id TEXT NOT NULL DEFAULT '',
    hash TEXT NOT NULL DEFAULT '',
    box TEXT NOT NULL DEFAULT '',
    path TEXT NOT NULL DEFAULT '',
    hpath TEXT NOT NULL DEFAULT '',
    name TEXT NOT NULL DEFAULT '',
    alias TEXT NOT NULL DEFAULT '',
    memo TEXT NOT NULL DEFAULT '',
    tag TEXT NOT NULL DEFAULT '',
    content TEXT NOT NULL DEFAULT '',
    fcontent TEXT NOT NULL DEFAULT '',
    markdown TEXT NOT NULL DEFAULT '',
    length INTEGER NOT NULL DEFAULT 0,
    type TEXT NOT NULL DEFAULT '',
    subtype TEXT NOT NULL DEFAULT '',
    ial TEXT NOT NULL DEFAULT '',
    sort INTEGER NOT NULL DEFAULT 0,
    created TEXT NOT NULL DEFAULT '',
    updated TEXT NOT NULL DEFAULT ''
);

-- Indexes for common query patterns
CREATE INDEX IF NOT EXISTS idx_blocks_root_id ON blocks(root_id);
CREATE INDEX IF NOT EXISTS idx_blocks_parent_id ON blocks(parent_id);
CREATE INDEX IF NOT EXISTS idx_blocks_box ON blocks(box);
CREATE INDEX IF NOT EXISTS idx_blocks_type ON blocks(type);
CREATE INDEX IF NOT EXISTS idx_blocks_tag ON blocks(tag);
CREATE INDEX IF NOT EXISTS idx_blocks_created ON blocks(created);
CREATE INDEX IF NOT EXISTS idx_blocks_updated ON blocks(updated);

-- Reference links between blocks.
-- block_id contains a reference to def_block_id.
CREATE TABLE IF NOT EXISTS refs (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    block_id TEXT NOT NULL,
    def_block_id TEXT NOT NULL,
    def_block_root_id TEXT NOT NULL,
    content TEXT NOT NULL DEFAULT '',
    UNIQUE(block_id, def_block_id)
);

CREATE INDEX IF NOT EXISTS idx_refs_block_id ON refs(block_id);
CREATE INDEX IF NOT EXISTS idx_refs_def_block_id ON refs(def_block_id);
CREATE INDEX IF NOT EXISTS idx_refs_def_block_root_id ON refs(def_block_root_id);

-- FTS5 virtual table for full-text search.
-- Uses content-sync mode (content=blocks) so FTS is automatically
-- updated when the blocks table changes via triggers.
CREATE VIRTUAL TABLE IF NOT EXISTS blocks_fts USING fts5(
    id UNINDEXED,
    name,
    content,
    tag,
    memo,
    alias,
    content=blocks,
    content_rowid=rowid
);

-- Triggers to keep FTS in sync with blocks table.
CREATE TRIGGER IF NOT EXISTS blocks_ai AFTER INSERT ON blocks BEGIN
    INSERT INTO blocks_fts(rowid, id, name, content, tag, memo, alias)
    VALUES (new.rowid, new.id, new.name, new.content, new.tag, new.memo, new.alias);
END;

CREATE TRIGGER IF NOT EXISTS blocks_ad AFTER DELETE ON blocks BEGIN
    INSERT INTO blocks_fts(blocks_fts, rowid, id, name, content, tag, memo, alias)
    VALUES ('delete', old.rowid, old.id, old.name, old.content, old.tag, old.memo, old.alias);
END;

CREATE TRIGGER IF NOT EXISTS blocks_au AFTER UPDATE ON blocks BEGIN
    INSERT INTO blocks_fts(blocks_fts, rowid, id, name, content, tag, memo, alias)
    VALUES ('delete', old.rowid, old.id, old.name, old.content, old.tag, old.memo, old.alias);
    INSERT INTO blocks_fts(rowid, id, name, content, tag, memo, alias)
    VALUES (new.rowid, new.id, new.name, new.content, new.tag, new.memo, new.alias);
END;

-- Schema version tracking for migrations.
CREATE TABLE IF NOT EXISTS schema_info (
    key TEXT PRIMARY KEY,
    value TEXT NOT NULL
);

-- Insert initial schema version.
INSERT OR IGNORE INTO schema_info (key, value) VALUES ('version', '1');
```

## Key Functions to Implement

1. **`open()`** — Call `sqlite3_open_v2()` with `SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE`. Set WAL mode, foreign keys, and busy timeout via PRAGMAs. Log the database path. Return error string if `sqlite3_open_v2` fails.

2. **`close()`** — Flush the write queue. Stop the queue worker. Finalize all prepared statements. Call `sqlite3_close_v2()`. Set `db_ = nullptr`.

3. **`init_tables()`** — Execute the full schema SQL. Use `sqlite3_exec()` for the DDL statements. Check return codes. Insert schema version if not present.

4. **`upsert_block(block)`** — Prepare and execute `INSERT OR REPLACE INTO blocks (id, parent_id, ..., updated) VALUES (?, ?, ..., ?)`. Bind all 21 fields from the Block struct. The `type` and `subtype` columns store the abbreviation strings (from `type_abbr()` / `sub_type_abbr()`), not the enum values. The `ial` column stores the serialized IAL string.

5. **`upsert_blocks(blocks)`** — Begin transaction. Call `upsert_block()` for each block. Commit. On any error, rollback and return the error.

6. **`get_block(id)`** — Prepare `SELECT * FROM blocks WHERE id = ?`. Bind id. Step. If row found, convert via `row_to_block()`. Reset statement.

7. **`get_child_blocks(parent_id)`** — `SELECT * FROM blocks WHERE parent_id = ? ORDER BY sort ASC`.

8. **`search_blocks(query, limit)`** — `SELECT blocks.* FROM blocks JOIN blocks_fts ON blocks.id = blocks_fts.id WHERE blocks_fts MATCH ? ORDER BY rank LIMIT ?`. Bind query and limit.

9. **`upsert_ref(block_id, def_block_id, def_block_root_id, content)`** — `INSERT OR REPLACE INTO refs (block_id, def_block_id, def_block_root_id, content) VALUES (?, ?, ?, ?)`.

10. **`get_refs_by_def(def_block_id)`** — `SELECT * FROM refs WHERE def_block_id = ?`. Convert rows to BlockRef.

11. **`queue_operation(item)`** — Lock `queue_mutex_`, push item, notify `queue_cv_`.

12. **`process_queue()`** — Lock mutex, swap queue to local. Process each item: switch on `op`, call the appropriate method (upsert_block, delete_block, etc.). Return count processed. All operations within a single transaction for efficiency.

13. **`queue_worker_fn(stop_token)`** — Loop: wait on `queue_cv_` with `stop_token`. When woken, call `process_queue()`. Exit when stop requested and queue is empty.

14. **`row_to_block(stmt)`** — Read each column from the sqlite3_stmt. Map `type` abbreviation string back to `BlockType` enum. Map `subtype` abbreviation string back to `BlockSubType` enum. Parse `ial` string back to `InlineAttributeList`. Parse `created`/`updated` strings to int64_t timestamps.

## Events to Add (in Events.h)

```cpp
MARKAMP_DECLARE_EVENT(DatabaseIndexedEvent, std::string /* root_id */, int /* block_count */)
MARKAMP_DECLARE_EVENT(DatabaseRebuildStartedEvent)
MARKAMP_DECLARE_EVENT(DatabaseRebuildCompletedEvent, int /* total_blocks */)
```

## Config Keys to Add

- `knowledgebase.db_path` — Path to the SQLite database file. Default: `~/.markamp/data/.markamp/siyuan.db`. Type: string (filesystem path).
- `knowledgebase.fts_enabled` — Enable or disable full-text search indexing. When disabled, FTS triggers are not created and `search_blocks()` returns empty results. Default: `true`. Type: bool.

## Test Cases (Catch2)

File: `tests/unit/test_block_database.cpp`

All tests should create a temporary database file (e.g., in `/tmp` or a Catch2 temp directory) and delete it after each test.

1. **Open and close** — Create a BlockDatabase with a temp path. Call `open()`. Verify `is_open()` returns true. Call `close()`. Verify `is_open()` returns false. Verify the database file exists on disk.

2. **Schema creation** — Open database, call `init_tables()`. Verify it succeeds. Verify `schema_version()` returns 1. Call `init_tables()` again (idempotent) — verify no error.

3. **Block upsert and retrieval** — Create a Block with all fields populated. Call `upsert_block()`. Call `get_block(id)`. Verify all 21 fields match the original. Pay special attention to type/subtype round-trip (enum -> abbr string -> enum).

4. **Batch block upsert** — Create 100 blocks with unique IDs. Call `upsert_blocks()`. Call `count_blocks()`. Verify count is 100.

5. **Block delete** — Insert a block. Delete it. Verify `get_block()` returns nullopt. Verify `count_blocks()` is 0.

6. **Delete blocks by root** — Insert 10 blocks with the same root_id. Insert 5 blocks with a different root_id. Call `delete_blocks_by_root(first_root_id)`. Verify count is 5 (only second group remains).

7. **Child block query** — Insert a parent block and 5 child blocks with different sort values. Call `get_child_blocks(parent_id)`. Verify 5 results, sorted by sort field ascending.

8. **Reference upsert and query** — Insert 3 blocks. Create references: block1 refs block2, block1 refs block3. Call `get_refs_by_block(block1_id)` — verify 2 results. Call `get_refs_by_def(block2_id)` — verify 1 result. Call `get_ref_count(block2_id)` — verify returns 1.

9. **Full-text search** — Insert blocks with content: "the quick brown fox", "lazy dog sleeps", "fox jumps over". Search for "fox". Verify 2 results returned. Search for "elephant". Verify 0 results.

10. **Search within notebook** — Insert blocks in box "nb1" with content "alpha beta" and blocks in box "nb2" with content "alpha gamma". Call `search_blocks_in_box("nb1", "alpha")`. Verify only nb1 blocks returned.

11. **Async write queue** — Queue 50 upsert operations. Call `flush_queue()`. Verify `count_blocks()` is 50.

12. **Queue worker thread** — Start queue worker. Queue 100 operations. Wait for flush. Stop worker. Verify all 100 blocks exist.

13. **Transaction rollback safety** — Attempt to upsert a block with a deliberately invalid field that causes a constraint violation mid-batch. Verify the entire batch is rolled back and the database is consistent.

14. **Raw SQL query** — Insert 3 blocks. Call `query_raw("SELECT id, name FROM blocks ORDER BY id")`. Verify 3 rows returned with correct id and name columns.

15. **Block attribute query** — Insert a block with IAL `{: id="..." custom-color="red" }`. Call `get_block_attrs(id)`. Verify `custom-color` key exists with value `"red"`.

## Acceptance Criteria

- [ ] SQLite database created with WAL mode, foreign keys enabled, 10-second busy timeout
- [ ] All tables, indexes, and FTS5 virtual table match the schema above
- [ ] FTS5 full-text search works correctly (triggers keep index in sync)
- [ ] Batch operations (upsert_blocks) use transactions for atomicity
- [ ] Async write queue runs on a dedicated `std::jthread` and processes operations in batches
- [ ] `flush_queue()` blocks until all pending operations are complete
- [ ] All SQL uses prepared statements (no string concatenation of user data)
- [ ] `row_to_block()` correctly round-trips all Block fields through SQL
- [ ] Type and SubType stored as abbreviation strings, not raw enum values
- [ ] All 15 test cases pass
- [ ] No use of `catch(...)` — use typed handlers for `std::exception` subclasses
- [ ] Allman brace style, 4-space indent, trailing return types

## Files to Create/Modify

- CREATE: `src/core/BlockDatabase.h`
- CREATE: `src/core/BlockDatabase.cpp`
- MODIFY: `src/core/Events.h` (add 3 database lifecycle events)
- MODIFY: `src/core/Config.cpp` (add `knowledgebase.db_path` and `knowledgebase.fts_enabled` defaults)
- MODIFY: `src/CMakeLists.txt` (add `core/BlockDatabase.cpp` to source list, link `sqlite3` library)
- CREATE: `tests/unit/test_block_database.cpp`
- MODIFY: `tests/CMakeLists.txt` (add `test_block_database` target linking Catch2 and sqlite3)

## Dependency Note

This phase introduces the first external C library dependency: SQLite3. The recommended approach:
- On macOS: Link against system SQLite3 (`find_package(SQLite3 REQUIRED)` or `-lsqlite3`)
- On Linux: `apt install libsqlite3-dev` or equivalent
- On Windows: Bundle the SQLite3 amalgamation (`sqlite3.c` + `sqlite3.h`) in `third_party/sqlite3/`
- CMake: `target_link_libraries(markamp PRIVATE SQLite::SQLite3)` or equivalent
