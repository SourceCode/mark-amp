#pragma once

#include "core/Block.h"

#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <expected>
#include <filesystem>
#include <mutex>
#include <optional>
#include <queue>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

struct sqlite3;
struct sqlite3_stmt;

namespace markamp::core
{

// Types of database operations that can be queued.
enum class DBOperation : uint8_t
{
    UpsertBlock,
    DeleteBlock,
    UpsertRef,
    DeleteRef,
    DeleteBlocksByRoot,
    DeleteBlocksByBox,
    RebuildIndex,
};

// A single queued database operation.
struct DBQueueItem
{
    DBOperation operation;
    std::string primary_key;
    std::string data_json;
    int priority = 0;
};

// Core SQLite database interface for block storage and querying.
// All read operations are synchronous (fast due to indexes).
// All write operations can be queued for async execution.
class BlockDatabase
{
public:
    explicit BlockDatabase(const std::filesystem::path& db_path);
    ~BlockDatabase();

    BlockDatabase(const BlockDatabase&) = delete;
    auto operator=(const BlockDatabase&) -> BlockDatabase& = delete;
    BlockDatabase(BlockDatabase&&) = delete;
    auto operator=(BlockDatabase&&) -> BlockDatabase& = delete;

    // --- Lifecycle ---
    auto open() -> std::expected<void, std::string>;
    auto close() -> void;
    [[nodiscard]] auto is_open() const -> bool;

    // --- Schema Management ---
    auto init_tables() -> std::expected<void, std::string>;
    auto upgrade_schema(int from_version, int to_version) -> std::expected<void, std::string>;
    [[nodiscard]] auto schema_version() const -> int;

    // --- Block CRUD ---
    auto upsert_block(const Block& block) -> std::expected<void, std::string>;
    auto upsert_blocks(const std::vector<Block>& blocks) -> std::expected<void, std::string>;
    auto delete_block(const std::string& block_id) -> std::expected<void, std::string>;
    auto delete_blocks_by_root(const std::string& root_id) -> std::expected<void, std::string>;
    auto delete_blocks_by_box(const std::string& box_id) -> std::expected<void, std::string>;

    // --- Block Queries ---
    [[nodiscard]] auto get_block(const std::string& block_id) -> std::optional<Block>;
    [[nodiscard]] auto get_blocks(const std::vector<std::string>& ids) -> std::vector<Block>;
    [[nodiscard]] auto get_child_blocks(const std::string& parent_id) -> std::vector<Block>;
    [[nodiscard]] auto get_root_blocks(const std::string& box_id) -> std::vector<Block>;
    [[nodiscard]] auto get_blocks_by_type(BlockType type) -> std::vector<Block>;

    // --- Reference CRUD ---
    auto upsert_ref(const std::string& block_id,
                    const std::string& def_block_id,
                    const std::string& def_block_root_id,
                    const std::string& ref_content) -> std::expected<void, std::string>;
    auto delete_refs_by_block(const std::string& block_id) -> std::expected<void, std::string>;

    // --- Reference Queries ---
    [[nodiscard]] auto get_refs_by_def(const std::string& def_block_id) -> std::vector<BlockRef>;
    [[nodiscard]] auto get_refs_by_block(const std::string& block_id) -> std::vector<BlockRef>;
    [[nodiscard]] auto get_ref_count(const std::string& def_block_id) -> int64_t;

    // --- Full-Text Search ---
    [[nodiscard]] auto search_blocks(const std::string& query, int limit = 64)
        -> std::vector<Block>;
    [[nodiscard]] auto search_blocks_in_box(const std::string& box_id,
                                            const std::string& query,
                                            int limit = 64) -> std::vector<Block>;

    // --- Attribute Queries ---
    [[nodiscard]] auto get_block_attrs(const std::string& block_id) -> InlineAttributeList;
    auto set_block_attrs(const std::string& block_id, const InlineAttributeList& attrs)
        -> std::expected<void, std::string>;

    // --- Statistics ---
    [[nodiscard]] auto count_blocks() -> int64_t;
    [[nodiscard]] auto count_blocks_by_box(const std::string& box_id) -> int64_t;
    [[nodiscard]] auto count_refs() -> int64_t;

    // --- Raw SQL ---
    [[nodiscard]] auto query_raw(const std::string& sql_query)
        -> std::expected<std::vector<std::unordered_map<std::string, std::string>>, std::string>;

    // --- Async Write Queue ---
    auto queue_operation(DBQueueItem item) -> void;
    auto flush_queue() -> void;
    auto process_queue() -> int;
    auto start_queue_worker() -> void;
    auto stop_queue_worker() -> void;

    // --- Type Conversion (public for use by DocumentFileSystem, BlockTreeIndex) ---
    [[nodiscard]] static auto abbr_to_block_type(const std::string& abbr) -> BlockType;
    [[nodiscard]] static auto abbr_to_block_sub_type(const std::string& abbr) -> BlockSubType;

private:
    std::filesystem::path db_path_;
    sqlite3* db_ = nullptr; // NOLINT(cppcoreguidelines-non-private-member-variables-in-classes)

    // Prepared statements cache
    std::unordered_map<std::string, sqlite3_stmt*> stmts_;
    auto prepare(const std::string& key, const std::string& sql_text) -> sqlite3_stmt*;
    auto finalize_statements() -> void;

    // Write queue
    std::queue<DBQueueItem> write_queue_;
    std::mutex queue_mutex_;
    std::condition_variable queue_cv_;
    std::thread queue_worker_;
    std::atomic<bool> queue_running_{false};
    std::atomic<bool> queue_stop_requested_{false};
    auto queue_worker_fn() -> void;

    // Transaction helpers
    auto begin_transaction() -> std::expected<void, std::string>;
    auto commit_transaction() -> std::expected<void, std::string>;
    auto rollback_transaction() -> void;

    // Row mapping
    [[nodiscard]] auto row_to_block(sqlite3_stmt* stmt) const -> Block;
    [[nodiscard]] auto row_to_ref(sqlite3_stmt* stmt) const -> BlockRef;
    auto bind_block(sqlite3_stmt* stmt, const Block& block) const -> void;

    // Constants
    static constexpr int CURRENT_SCHEMA_VERSION = 1;
    static constexpr int BUSY_TIMEOUT_MS = 10000;
    static constexpr int QUEUE_BATCH_SIZE = 500;
};

} // namespace markamp::core
