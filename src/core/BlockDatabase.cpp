#include "core/BlockDatabase.h"

#include <sqlite3.h>
#include <sstream>

namespace markamp::core
{

namespace
{

// -- SQL Schema --

constexpr const char* kSchemaSql = R"SQL(
PRAGMA journal_mode=WAL;
PRAGMA foreign_keys=ON;
PRAGMA busy_timeout=10000;

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

CREATE INDEX IF NOT EXISTS idx_blocks_root_id ON blocks(root_id);
CREATE INDEX IF NOT EXISTS idx_blocks_parent_id ON blocks(parent_id);
CREATE INDEX IF NOT EXISTS idx_blocks_box ON blocks(box);
CREATE INDEX IF NOT EXISTS idx_blocks_type ON blocks(type);
CREATE INDEX IF NOT EXISTS idx_blocks_tag ON blocks(tag);
CREATE INDEX IF NOT EXISTS idx_blocks_created ON blocks(created);
CREATE INDEX IF NOT EXISTS idx_blocks_updated ON blocks(updated);

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

CREATE TABLE IF NOT EXISTS schema_info (
    key TEXT PRIMARY KEY,
    value TEXT NOT NULL
);

INSERT OR IGNORE INTO schema_info (key, value) VALUES ('version', '1');
)SQL";

// -- SQL Statements --

constexpr const char* kSqlUpsertBlock =
    "INSERT OR REPLACE INTO blocks "
    "(id, parent_id, root_id, hash, box, path, hpath, name, alias, memo, tag, "
    "content, fcontent, markdown, length, type, subtype, ial, sort, created, updated) "
    "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";

constexpr const char* kSqlDeleteBlock = "DELETE FROM blocks WHERE id = ?";
constexpr const char* kSqlDeleteByRoot = "DELETE FROM blocks WHERE root_id = ?";
constexpr const char* kSqlDeleteByBox = "DELETE FROM blocks WHERE box = ?";

constexpr const char* kSqlGetBlock = "SELECT * FROM blocks WHERE id = ?";
constexpr const char* kSqlGetChildren =
    "SELECT * FROM blocks WHERE parent_id = ? ORDER BY sort ASC";
constexpr const char* kSqlGetRootBlocks = "SELECT * FROM blocks WHERE box = ? AND type = 'd'";
constexpr const char* kSqlGetByType = "SELECT * FROM blocks WHERE type = ?";

constexpr const char* kSqlUpsertRef =
    "INSERT OR REPLACE INTO refs (block_id, def_block_id, def_block_root_id, content) "
    "VALUES (?, ?, ?, ?)";
constexpr const char* kSqlDeleteRefs = "DELETE FROM refs WHERE block_id = ?";
constexpr const char* kSqlGetRefsByDef = "SELECT * FROM refs WHERE def_block_id = ?";
constexpr const char* kSqlGetRefsByBlock = "SELECT * FROM refs WHERE block_id = ?";
constexpr const char* kSqlCountRefsByDef = "SELECT COUNT(*) FROM refs WHERE def_block_id = ?";

constexpr const char* kSqlSearchBlocks = "SELECT blocks.* FROM blocks "
                                         "JOIN blocks_fts ON blocks.id = blocks_fts.id "
                                         "WHERE blocks_fts MATCH ? ORDER BY rank LIMIT ?";
constexpr const char* kSqlSearchBlocksInBox =
    "SELECT blocks.* FROM blocks "
    "JOIN blocks_fts ON blocks.id = blocks_fts.id "
    "WHERE blocks_fts MATCH ? AND blocks.box = ? ORDER BY rank LIMIT ?";

constexpr const char* kSqlCountBlocks = "SELECT COUNT(*) FROM blocks";
constexpr const char* kSqlCountBlocksByBox = "SELECT COUNT(*) FROM blocks WHERE box = ?";
constexpr const char* kSqlCountRefs = "SELECT COUNT(*) FROM refs";
constexpr const char* kSqlGetSchemaVersion = "SELECT value FROM schema_info WHERE key = 'version'";
constexpr const char* kSqlGetBlockIal = "SELECT ial FROM blocks WHERE id = ?";
constexpr const char* kSqlSetBlockIal = "UPDATE blocks SET ial = ? WHERE id = ?";

// -- Helper functions --

auto col_text(sqlite3_stmt* stmt, int col) -> std::string
{
    const auto* text = sqlite3_column_text(stmt, col);
    if (text == nullptr)
    {
        return "";
    }
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
    return {reinterpret_cast<const char*>(text)};
}

auto col_int64(sqlite3_stmt* stmt, int col) -> int64_t
{
    return sqlite3_column_int64(stmt, col);
}

} // anonymous namespace

// -- BlockDatabase Implementation --

BlockDatabase::BlockDatabase(const std::filesystem::path& db_path)
    : db_path_(db_path)
{
}

BlockDatabase::~BlockDatabase()
{
    close();
}

auto BlockDatabase::open() -> std::expected<void, std::string>
{
    if (db_ != nullptr)
    {
        return {};
    }

    std::error_code err_code;
    std::filesystem::create_directories(db_path_.parent_path(), err_code);
    if (err_code)
    {
        return std::unexpected("Failed to create database directory: " + err_code.message());
    }

    const int result =
        sqlite3_open_v2(db_path_.string().c_str(),
                        &db_,
                        SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_FULLMUTEX,
                        nullptr);
    if (result != SQLITE_OK)
    {
        const std::string error_msg = db_ != nullptr ? sqlite3_errmsg(db_) : "Unknown error";
        if (db_ != nullptr)
        {
            sqlite3_close_v2(db_);
            db_ = nullptr;
        }
        return std::unexpected("Failed to open database: " + error_msg);
    }

    // Set WAL mode and busy timeout
    char* err_msg = nullptr;
    sqlite3_exec(db_, "PRAGMA journal_mode=WAL;", nullptr, nullptr, &err_msg);
    if (err_msg != nullptr)
    {
        sqlite3_free(err_msg);
        err_msg = nullptr;
    }

    sqlite3_exec(db_, "PRAGMA foreign_keys=ON;", nullptr, nullptr, &err_msg);
    if (err_msg != nullptr)
    {
        sqlite3_free(err_msg);
    }

    sqlite3_busy_timeout(db_, BUSY_TIMEOUT_MS);

    return {};
}

auto BlockDatabase::close() -> void
{
    stop_queue_worker();
    finalize_statements();

    if (db_ != nullptr)
    {
        sqlite3_close_v2(db_);
        db_ = nullptr;
    }
}

auto BlockDatabase::is_open() const -> bool
{
    return db_ != nullptr;
}

auto BlockDatabase::init_tables() -> std::expected<void, std::string>
{
    if (db_ == nullptr)
    {
        return std::unexpected("Database is not open");
    }

    char* err_msg = nullptr;
    const int result = sqlite3_exec(db_, kSchemaSql, nullptr, nullptr, &err_msg);
    if (result != SQLITE_OK)
    {
        const std::string error(err_msg != nullptr ? err_msg : "Unknown SQL error");
        if (err_msg != nullptr)
        {
            sqlite3_free(err_msg);
        }
        return std::unexpected("Failed to initialize tables: " + error);
    }

    return {};
}

auto BlockDatabase::upgrade_schema(int /*from_version*/, int /*to_version*/)
    -> std::expected<void, std::string>
{
    // Currently only version 1 exists. Future migrations go here.
    return {};
}

auto BlockDatabase::schema_version() const -> int
{
    if (db_ == nullptr)
    {
        return 0;
    }

    sqlite3_stmt* stmt = nullptr;
    const int result = sqlite3_prepare_v2(db_, kSqlGetSchemaVersion, -1, &stmt, nullptr);
    if (result != SQLITE_OK || stmt == nullptr)
    {
        return 0;
    }
    int version = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW)
    {
        auto val_str = col_text(stmt, 0);
        if (!val_str.empty())
        {
            version = std::stoi(val_str);
        }
    }
    sqlite3_finalize(stmt);
    return version;
}

// -- Prepared Statement Cache --

auto BlockDatabase::prepare(const std::string& key, const std::string& sql_text) -> sqlite3_stmt*
{
    auto found = stmts_.find(key);
    if (found != stmts_.end())
    {
        sqlite3_reset(found->second);
        sqlite3_clear_bindings(found->second);
        return found->second;
    }

    sqlite3_stmt* stmt = nullptr;
    const int result = sqlite3_prepare_v2(db_, sql_text.c_str(), -1, &stmt, nullptr);
    if (result != SQLITE_OK || stmt == nullptr)
    {
        return nullptr;
    }
    stmts_[key] = stmt;
    return stmt;
}

auto BlockDatabase::finalize_statements() -> void
{
    for (auto& [key, stmt] : stmts_)
    {
        if (stmt != nullptr)
        {
            sqlite3_finalize(stmt);
        }
    }
    stmts_.clear();
}

// -- Transaction Helpers --

auto BlockDatabase::begin_transaction() -> std::expected<void, std::string>
{
    char* err_msg = nullptr;
    const int result = sqlite3_exec(db_, "BEGIN TRANSACTION", nullptr, nullptr, &err_msg);
    if (result != SQLITE_OK)
    {
        const std::string error(err_msg != nullptr ? err_msg : "Unknown error");
        if (err_msg != nullptr)
        {
            sqlite3_free(err_msg);
        }
        return std::unexpected("Failed to begin transaction: " + error);
    }
    return {};
}

auto BlockDatabase::commit_transaction() -> std::expected<void, std::string>
{
    char* err_msg = nullptr;
    const int result = sqlite3_exec(db_, "COMMIT", nullptr, nullptr, &err_msg);
    if (result != SQLITE_OK)
    {
        const std::string error(err_msg != nullptr ? err_msg : "Unknown error");
        if (err_msg != nullptr)
        {
            sqlite3_free(err_msg);
        }
        return std::unexpected("Failed to commit transaction: " + error);
    }
    return {};
}

auto BlockDatabase::rollback_transaction() -> void
{
    char* err_msg = nullptr;
    sqlite3_exec(db_, "ROLLBACK", nullptr, nullptr, &err_msg);
    if (err_msg != nullptr)
    {
        sqlite3_free(err_msg);
    }
}

// -- Type Conversion --

auto BlockDatabase::abbr_to_block_type(const std::string& abbr) -> BlockType
{
    if (abbr == "d")
    {
        return BlockType::Document;
    }
    if (abbr == "p")
    {
        return BlockType::Paragraph;
    }
    if (abbr == "h")
    {
        return BlockType::Heading;
    }
    if (abbr == "b")
    {
        return BlockType::BlockQuote;
    }
    if (abbr == "l")
    {
        return BlockType::List;
    }
    if (abbr == "i")
    {
        return BlockType::ListItem;
    }
    if (abbr == "c")
    {
        return BlockType::CodeBlock;
    }
    if (abbr == "m")
    {
        return BlockType::MathBlock;
    }
    if (abbr == "t")
    {
        return BlockType::Table;
    }
    if (abbr == "tr")
    {
        return BlockType::TableRow;
    }
    if (abbr == "td")
    {
        return BlockType::TableCell;
    }
    if (abbr == "html")
    {
        return BlockType::HTMLBlock;
    }
    if (abbr == "tb")
    {
        return BlockType::ThematicBreak;
    }
    if (abbr == "audio")
    {
        return BlockType::Audio;
    }
    if (abbr == "video")
    {
        return BlockType::Video;
    }
    if (abbr == "iframe")
    {
        return BlockType::IFrame;
    }
    if (abbr == "widget")
    {
        return BlockType::Widget;
    }
    if (abbr == "s")
    {
        return BlockType::SuperBlock;
    }
    if (abbr == "e")
    {
        return BlockType::EmbedBlock;
    }
    if (abbr == "av")
    {
        return BlockType::AttributeView;
    }
    if (abbr == "callout")
    {
        return BlockType::CalloutBlock;
    }
    return BlockType::Unknown;
}

auto BlockDatabase::abbr_to_block_sub_type(const std::string& abbr) -> BlockSubType
{
    if (abbr == "h1")
    {
        return BlockSubType::Heading1;
    }
    if (abbr == "h2")
    {
        return BlockSubType::Heading2;
    }
    if (abbr == "h3")
    {
        return BlockSubType::Heading3;
    }
    if (abbr == "h4")
    {
        return BlockSubType::Heading4;
    }
    if (abbr == "h5")
    {
        return BlockSubType::Heading5;
    }
    if (abbr == "h6")
    {
        return BlockSubType::Heading6;
    }
    if (abbr == "u")
    {
        return BlockSubType::UnorderedList;
    }
    if (abbr == "o")
    {
        return BlockSubType::OrderedList;
    }
    if (abbr == "task")
    {
        return BlockSubType::TaskList;
    }
    return BlockSubType::None;
}

// -- Row Mapping --

auto BlockDatabase::row_to_block(sqlite3_stmt* stmt) const -> Block
{
    Block block;
    block.id = col_text(stmt, 0);
    block.parent_id = col_text(stmt, 1);
    block.root_id = col_text(stmt, 2);
    block.hash = col_text(stmt, 3);
    block.box = col_text(stmt, 4);
    block.path = col_text(stmt, 5);
    block.hpath = col_text(stmt, 6);
    block.name = col_text(stmt, 7);
    block.alias = col_text(stmt, 8);
    block.memo = col_text(stmt, 9);
    block.tag = col_text(stmt, 10);
    block.content = col_text(stmt, 11);
    block.fcontent = col_text(stmt, 12);
    block.markdown = col_text(stmt, 13);
    block.length = col_int64(stmt, 14);
    block.type = abbr_to_block_type(col_text(stmt, 15));
    block.sub_type = abbr_to_block_sub_type(col_text(stmt, 16));

    auto ial_str = col_text(stmt, 17);
    if (!ial_str.empty())
    {
        block.set_ial_from_string(ial_str);
    }

    block.sort = col_int64(stmt, 18);
    block.created_str = col_text(stmt, 19);
    block.updated_str = col_text(stmt, 20);

    return block;
}

auto BlockDatabase::row_to_ref(sqlite3_stmt* stmt) const -> BlockRef
{
    BlockRef ref;
    // id is column 0 (autoincrement, skip)
    ref.block_id = col_text(stmt, 1);
    ref.def_block_id = col_text(stmt, 2);
    ref.def_block_root_id = col_text(stmt, 3);
    ref.content = col_text(stmt, 4);
    return ref;
}

auto BlockDatabase::bind_block(sqlite3_stmt* stmt, const Block& block) const -> void
{
    sqlite3_bind_text(stmt, 1, block.id.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, block.parent_id.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, block.root_id.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, block.hash.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 5, block.box.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 6, block.path.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 7, block.hpath.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 8, block.name.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 9, block.alias.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 10, block.memo.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 11, block.tag.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 12, block.content.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 13, block.fcontent.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 14, block.markdown.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(stmt, 15, block.length);
    auto type_str = block.type_abbr();
    auto sub_type_str = block.sub_type_abbr();
    sqlite3_bind_text(stmt, 16, type_str.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 17, sub_type_str.c_str(), -1, SQLITE_TRANSIENT);
    auto ial_str = block.ial_string();
    sqlite3_bind_text(stmt, 18, ial_str.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(stmt, 19, block.sort);
    sqlite3_bind_text(stmt, 20, block.created_str.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 21, block.updated_str.c_str(), -1, SQLITE_TRANSIENT);
}

// -- Block CRUD --

auto BlockDatabase::upsert_block(const Block& block) -> std::expected<void, std::string>
{
    auto* stmt = prepare("upsert_block", kSqlUpsertBlock);
    if (stmt == nullptr)
    {
        return std::unexpected("Failed to prepare upsert_block: " +
                               std::string(sqlite3_errmsg(db_)));
    }

    bind_block(stmt, block);

    const int result = sqlite3_step(stmt);
    if (result != SQLITE_DONE)
    {
        return std::unexpected("Failed to upsert block: " + std::string(sqlite3_errmsg(db_)));
    }

    return {};
}

auto BlockDatabase::upsert_blocks(const std::vector<Block>& blocks)
    -> std::expected<void, std::string>
{
    auto txn = begin_transaction();
    if (!txn)
    {
        return std::unexpected(txn.error());
    }

    for (const auto& block : blocks)
    {
        auto result = upsert_block(block);
        if (!result)
        {
            rollback_transaction();
            return std::unexpected(result.error());
        }
    }

    return commit_transaction();
}

auto BlockDatabase::delete_block(const std::string& block_id) -> std::expected<void, std::string>
{
    auto* stmt = prepare("delete_block", kSqlDeleteBlock);
    if (stmt == nullptr)
    {
        return std::unexpected("Failed to prepare delete_block");
    }

    sqlite3_bind_text(stmt, 1, block_id.c_str(), -1, SQLITE_TRANSIENT);
    const int result = sqlite3_step(stmt);
    if (result != SQLITE_DONE)
    {
        return std::unexpected("Failed to delete block: " + std::string(sqlite3_errmsg(db_)));
    }

    return {};
}

auto BlockDatabase::delete_blocks_by_root(const std::string& root_id)
    -> std::expected<void, std::string>
{
    auto* stmt = prepare("delete_by_root", kSqlDeleteByRoot);
    if (stmt == nullptr)
    {
        return std::unexpected("Failed to prepare delete_by_root");
    }

    sqlite3_bind_text(stmt, 1, root_id.c_str(), -1, SQLITE_TRANSIENT);
    const int result = sqlite3_step(stmt);
    if (result != SQLITE_DONE)
    {
        return std::unexpected("Failed to delete blocks by root: " +
                               std::string(sqlite3_errmsg(db_)));
    }

    return {};
}

auto BlockDatabase::delete_blocks_by_box(const std::string& box_id)
    -> std::expected<void, std::string>
{
    auto* stmt = prepare("delete_by_box", kSqlDeleteByBox);
    if (stmt == nullptr)
    {
        return std::unexpected("Failed to prepare delete_by_box");
    }

    sqlite3_bind_text(stmt, 1, box_id.c_str(), -1, SQLITE_TRANSIENT);
    const int result = sqlite3_step(stmt);
    if (result != SQLITE_DONE)
    {
        return std::unexpected("Failed to delete blocks by box: " +
                               std::string(sqlite3_errmsg(db_)));
    }

    return {};
}

// -- Block Queries --

auto BlockDatabase::get_block(const std::string& block_id) -> std::optional<Block>
{
    auto* stmt = prepare("get_block", kSqlGetBlock);
    if (stmt == nullptr)
    {
        return std::nullopt;
    }

    sqlite3_bind_text(stmt, 1, block_id.c_str(), -1, SQLITE_TRANSIENT);
    if (sqlite3_step(stmt) == SQLITE_ROW)
    {
        return row_to_block(stmt);
    }
    return std::nullopt;
}

auto BlockDatabase::get_blocks(const std::vector<std::string>& ids) -> std::vector<Block>
{
    std::vector<Block> result;
    for (const auto& block_id : ids)
    {
        auto block = get_block(block_id);
        if (block.has_value())
        {
            result.push_back(std::move(block.value()));
        }
    }
    return result;
}

auto BlockDatabase::get_child_blocks(const std::string& parent_id) -> std::vector<Block>
{
    std::vector<Block> result;
    auto* stmt = prepare("get_children", kSqlGetChildren);
    if (stmt == nullptr)
    {
        return result;
    }

    sqlite3_bind_text(stmt, 1, parent_id.c_str(), -1, SQLITE_TRANSIENT);
    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        result.push_back(row_to_block(stmt));
    }
    return result;
}

auto BlockDatabase::get_root_blocks(const std::string& box_id) -> std::vector<Block>
{
    std::vector<Block> result;
    auto* stmt = prepare("get_root_blocks", kSqlGetRootBlocks);
    if (stmt == nullptr)
    {
        return result;
    }

    sqlite3_bind_text(stmt, 1, box_id.c_str(), -1, SQLITE_TRANSIENT);
    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        result.push_back(row_to_block(stmt));
    }
    return result;
}

auto BlockDatabase::get_blocks_by_type(BlockType type) -> std::vector<Block>
{
    std::vector<Block> result;
    auto* stmt = prepare("get_by_type", kSqlGetByType);
    if (stmt == nullptr)
    {
        return result;
    }

    Block tmp;
    tmp.type = type;
    auto type_str = tmp.type_abbr();
    sqlite3_bind_text(stmt, 1, type_str.c_str(), -1, SQLITE_TRANSIENT);
    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        result.push_back(row_to_block(stmt));
    }
    return result;
}

// -- Reference CRUD --

auto BlockDatabase::upsert_ref(const std::string& block_id,
                               const std::string& def_block_id,
                               const std::string& def_block_root_id,
                               const std::string& ref_content) -> std::expected<void, std::string>
{
    auto* stmt = prepare("upsert_ref", kSqlUpsertRef);
    if (stmt == nullptr)
    {
        return std::unexpected("Failed to prepare upsert_ref");
    }

    sqlite3_bind_text(stmt, 1, block_id.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, def_block_id.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, def_block_root_id.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, ref_content.c_str(), -1, SQLITE_TRANSIENT);

    const int result = sqlite3_step(stmt);
    if (result != SQLITE_DONE)
    {
        return std::unexpected("Failed to upsert ref: " + std::string(sqlite3_errmsg(db_)));
    }
    return {};
}

auto BlockDatabase::delete_refs_by_block(const std::string& block_id)
    -> std::expected<void, std::string>
{
    auto* stmt = prepare("delete_refs", kSqlDeleteRefs);
    if (stmt == nullptr)
    {
        return std::unexpected("Failed to prepare delete_refs");
    }

    sqlite3_bind_text(stmt, 1, block_id.c_str(), -1, SQLITE_TRANSIENT);
    const int result = sqlite3_step(stmt);
    if (result != SQLITE_DONE)
    {
        return std::unexpected("Failed to delete refs: " + std::string(sqlite3_errmsg(db_)));
    }
    return {};
}

// -- Reference Queries --

auto BlockDatabase::get_refs_by_def(const std::string& def_block_id) -> std::vector<BlockRef>
{
    std::vector<BlockRef> result;
    auto* stmt = prepare("get_refs_by_def", kSqlGetRefsByDef);
    if (stmt == nullptr)
    {
        return result;
    }

    sqlite3_bind_text(stmt, 1, def_block_id.c_str(), -1, SQLITE_TRANSIENT);
    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        result.push_back(row_to_ref(stmt));
    }
    return result;
}

auto BlockDatabase::get_refs_by_block(const std::string& block_id) -> std::vector<BlockRef>
{
    std::vector<BlockRef> result;
    auto* stmt = prepare("get_refs_by_block", kSqlGetRefsByBlock);
    if (stmt == nullptr)
    {
        return result;
    }

    sqlite3_bind_text(stmt, 1, block_id.c_str(), -1, SQLITE_TRANSIENT);
    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        result.push_back(row_to_ref(stmt));
    }
    return result;
}

auto BlockDatabase::get_ref_count(const std::string& def_block_id) -> int64_t
{
    auto* stmt = prepare("count_refs_by_def", kSqlCountRefsByDef);
    if (stmt == nullptr)
    {
        return 0;
    }

    sqlite3_bind_text(stmt, 1, def_block_id.c_str(), -1, SQLITE_TRANSIENT);
    if (sqlite3_step(stmt) == SQLITE_ROW)
    {
        return col_int64(stmt, 0);
    }
    return 0;
}

// -- Full-Text Search --

auto BlockDatabase::search_blocks(const std::string& query, int limit) -> std::vector<Block>
{
    std::vector<Block> result;
    auto* stmt = prepare("search_blocks", kSqlSearchBlocks);
    if (stmt == nullptr)
    {
        return result;
    }

    sqlite3_bind_text(stmt, 1, query.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 2, limit);

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        result.push_back(row_to_block(stmt));
    }
    return result;
}

auto BlockDatabase::search_blocks_in_box(const std::string& box_id,
                                         const std::string& query,
                                         int limit) -> std::vector<Block>
{
    std::vector<Block> result;
    auto* stmt = prepare("search_blocks_in_box", kSqlSearchBlocksInBox);
    if (stmt == nullptr)
    {
        return result;
    }

    sqlite3_bind_text(stmt, 1, query.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, box_id.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 3, limit);

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        result.push_back(row_to_block(stmt));
    }
    return result;
}

// -- Attribute Queries --

auto BlockDatabase::get_block_attrs(const std::string& block_id) -> InlineAttributeList
{
    InlineAttributeList result;
    auto* stmt = prepare("get_block_ial", kSqlGetBlockIal);
    if (stmt == nullptr)
    {
        return result;
    }

    sqlite3_bind_text(stmt, 1, block_id.c_str(), -1, SQLITE_TRANSIENT);
    if (sqlite3_step(stmt) == SQLITE_ROW)
    {
        auto ial_str = col_text(stmt, 0);
        if (!ial_str.empty())
        {
            Block tmp;
            tmp.set_ial_from_string(ial_str);
            result = tmp.ial;
        }
    }
    return result;
}

auto BlockDatabase::set_block_attrs(const std::string& block_id, const InlineAttributeList& attrs)
    -> std::expected<void, std::string>
{
    Block tmp;
    tmp.ial = attrs;
    auto ial_str = tmp.ial_string();

    auto* stmt = prepare("set_block_ial", kSqlSetBlockIal);
    if (stmt == nullptr)
    {
        return std::unexpected("Failed to prepare set_block_ial");
    }

    sqlite3_bind_text(stmt, 1, ial_str.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, block_id.c_str(), -1, SQLITE_TRANSIENT);

    const int result = sqlite3_step(stmt);
    if (result != SQLITE_DONE)
    {
        return std::unexpected("Failed to set block attrs: " + std::string(sqlite3_errmsg(db_)));
    }
    return {};
}

// -- Statistics --

auto BlockDatabase::count_blocks() -> int64_t
{
    auto* stmt = prepare("count_blocks", kSqlCountBlocks);
    if (stmt == nullptr)
    {
        return 0;
    }
    if (sqlite3_step(stmt) == SQLITE_ROW)
    {
        return col_int64(stmt, 0);
    }
    return 0;
}

auto BlockDatabase::count_blocks_by_box(const std::string& box_id) -> int64_t
{
    auto* stmt = prepare("count_blocks_by_box", kSqlCountBlocksByBox);
    if (stmt == nullptr)
    {
        return 0;
    }
    sqlite3_bind_text(stmt, 1, box_id.c_str(), -1, SQLITE_TRANSIENT);
    if (sqlite3_step(stmt) == SQLITE_ROW)
    {
        return col_int64(stmt, 0);
    }
    return 0;
}

auto BlockDatabase::count_refs() -> int64_t
{
    auto* stmt = prepare("count_refs", kSqlCountRefs);
    if (stmt == nullptr)
    {
        return 0;
    }
    if (sqlite3_step(stmt) == SQLITE_ROW)
    {
        return col_int64(stmt, 0);
    }
    return 0;
}

// -- Raw SQL --

auto BlockDatabase::query_raw(const std::string& sql_query)
    -> std::expected<std::vector<std::unordered_map<std::string, std::string>>, std::string>
{
    std::vector<std::unordered_map<std::string, std::string>> results;

    sqlite3_stmt* stmt = nullptr;
    const int result = sqlite3_prepare_v2(db_, sql_query.c_str(), -1, &stmt, nullptr);
    if (result != SQLITE_OK || stmt == nullptr)
    {
        return std::unexpected("Failed to prepare raw query: " + std::string(sqlite3_errmsg(db_)));
    }

    const int col_count = sqlite3_column_count(stmt);
    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        std::unordered_map<std::string, std::string> row;
        for (int col_idx = 0; col_idx < col_count; ++col_idx)
        {
            const char* col_name = sqlite3_column_name(stmt, col_idx);
            auto col_val = col_text(stmt, col_idx);
            if (col_name != nullptr)
            {
                row[col_name] = std::move(col_val);
            }
        }
        results.push_back(std::move(row));
    }

    sqlite3_finalize(stmt);
    return results;
}

// -- Async Write Queue --

auto BlockDatabase::queue_operation(DBQueueItem item) -> void
{
    {
        const std::lock_guard lock(queue_mutex_);
        write_queue_.push(std::move(item));
    }
    queue_cv_.notify_one();
}

auto BlockDatabase::flush_queue() -> void
{
    std::queue<DBQueueItem> local_queue;
    {
        const std::lock_guard lock(queue_mutex_);
        std::swap(local_queue, write_queue_);
    }

    if (local_queue.empty())
    {
        return;
    }

    auto txn = begin_transaction();
    if (!txn)
    {
        return;
    }

    while (!local_queue.empty())
    {
        auto& item = local_queue.front();
        switch (item.operation)
        {
            case DBOperation::DeleteBlock:
                delete_block(item.primary_key);
                break;
            case DBOperation::DeleteBlocksByRoot:
                delete_blocks_by_root(item.primary_key);
                break;
            case DBOperation::DeleteBlocksByBox:
                delete_blocks_by_box(item.primary_key);
                break;
            case DBOperation::DeleteRef:
                delete_refs_by_block(item.primary_key);
                break;
            default:
                break;
        }
        local_queue.pop();
    }

    commit_transaction();
}

auto BlockDatabase::process_queue() -> int
{
    std::queue<DBQueueItem> local_queue;
    {
        const std::lock_guard lock(queue_mutex_);
        int batch_count = 0;
        while (!write_queue_.empty() && batch_count < QUEUE_BATCH_SIZE)
        {
            local_queue.push(std::move(write_queue_.front()));
            write_queue_.pop();
            ++batch_count;
        }
    }

    if (local_queue.empty())
    {
        return 0;
    }

    int processed = 0;
    auto txn = begin_transaction();
    if (!txn)
    {
        return 0;
    }

    while (!local_queue.empty())
    {
        auto& item = local_queue.front();
        switch (item.operation)
        {
            case DBOperation::DeleteBlock:
                delete_block(item.primary_key);
                break;
            case DBOperation::DeleteBlocksByRoot:
                delete_blocks_by_root(item.primary_key);
                break;
            case DBOperation::DeleteBlocksByBox:
                delete_blocks_by_box(item.primary_key);
                break;
            case DBOperation::DeleteRef:
                delete_refs_by_block(item.primary_key);
                break;
            default:
                break;
        }
        local_queue.pop();
        ++processed;
    }

    commit_transaction();
    return processed;
}

auto BlockDatabase::start_queue_worker() -> void
{
    if (queue_running_.exchange(true))
    {
        return;
    }
    queue_stop_requested_.store(false);
    queue_worker_ = std::thread([this]() { queue_worker_fn(); });
}

auto BlockDatabase::stop_queue_worker() -> void
{
    if (!queue_running_.exchange(false))
    {
        return;
    }

    queue_stop_requested_.store(true);
    queue_cv_.notify_all();

    if (queue_worker_.joinable())
    {
        queue_worker_.join();
    }

    flush_queue();
}

auto BlockDatabase::queue_worker_fn() -> void
{
    while (!queue_stop_requested_.load())
    {
        {
            std::unique_lock lock(queue_mutex_);
            queue_cv_.wait(
                lock, [this] { return !write_queue_.empty() || queue_stop_requested_.load(); });
        }

        if (queue_stop_requested_.load())
        {
            break;
        }

        process_queue();
    }

    process_queue();
}

} // namespace markamp::core
