// =============================================================================
// V3 Phases 4–6 Tests: BlockDatabase, DocumentFileSystem, BlockTreeIndex
// =============================================================================
#include "core/Block.h"
#include "core/BlockDatabase.h"
#include "core/BlockID.h"
#include "core/BlockTreeIndex.h"
#include "core/Config.h"
#include "core/DocumentFileSystem.h"
#include "core/EventBus.h"
#include "core/Events.h"

#include <catch2/catch_test_macros.hpp>
#include <nlohmann/json.hpp>

#include <filesystem>
#include <fstream>
#include <memory>
#include <string>
#include <thread>
#include <vector>

using namespace markamp::core;
namespace fs = std::filesystem;

// -- Helpers --

static auto make_test_db_path() -> fs::path
{
    auto tmp_dir = fs::temp_directory_path() / "markamp_test";
    fs::create_directories(tmp_dir);
    return tmp_dir / ("test_" + BlockIDGenerator::generate() + ".db");
}

static auto make_test_block(const std::string& block_id = "", BlockType type = BlockType::Paragraph)
    -> Block
{
    Block blk;
    blk.id = block_id.empty() ? BlockIDGenerator::generate() : block_id;
    blk.root_id = blk.id;
    blk.parent_id = "";
    blk.box = "test-box";
    blk.path = "/test/doc.sy";
    blk.hpath = "/Test Doc";
    blk.name = "Test Block";
    blk.content = "Hello world";
    blk.fcontent = "Hello world";
    blk.markdown = "Hello world";
    blk.type = type;
    blk.sub_type = BlockSubType::None;
    blk.sort = 10;
    blk.length = 11;
    blk.created_str = "20240101120000";
    blk.updated_str = "20240101120000";
    blk.hash = "abc123";
    blk.set_ial("id", blk.id);
    blk.set_ial("updated", blk.updated_str);
    return blk;
}

// =============================================================================
// Phase 4: BlockDatabase
// =============================================================================

TEST_CASE("BlockDatabase: Open and close", "[phase4][blockdb]")
{
    auto path = make_test_db_path();
    BlockDatabase database(path);

    auto result = database.open();
    REQUIRE(result.has_value());
    REQUIRE(database.is_open());

    database.close();
    REQUIRE_FALSE(database.is_open());

    fs::remove(path);
}

TEST_CASE("BlockDatabase: Init tables", "[phase4][blockdb]")
{
    auto path = make_test_db_path();
    BlockDatabase database(path);
    REQUIRE(database.open().has_value());

    auto result = database.init_tables();
    REQUIRE(result.has_value());
    REQUIRE(database.schema_version() >= 1);

    database.close();
    fs::remove(path);
}

TEST_CASE("BlockDatabase: Upsert and get block", "[phase4][blockdb]")
{
    auto path = make_test_db_path();
    BlockDatabase database(path);
    REQUIRE(database.open().has_value());
    REQUIRE(database.init_tables().has_value());

    auto block = make_test_block();
    auto upsert_result = database.upsert_block(block);
    REQUIRE(upsert_result.has_value());

    auto retrieved = database.get_block(block.id);
    REQUIRE(retrieved.has_value());
    CHECK(retrieved->id == block.id);
    CHECK(retrieved->content == block.content);
    CHECK(retrieved->type == block.type);
    CHECK(retrieved->box == block.box);

    database.close();
    fs::remove(path);
}

TEST_CASE("BlockDatabase: Delete block", "[phase4][blockdb]")
{
    auto path = make_test_db_path();
    BlockDatabase database(path);
    REQUIRE(database.open().has_value());
    REQUIRE(database.init_tables().has_value());

    auto block = make_test_block();
    REQUIRE(database.upsert_block(block).has_value());

    auto del_result = database.delete_block(block.id);
    REQUIRE(del_result.has_value());

    auto retrieved = database.get_block(block.id);
    REQUIRE_FALSE(retrieved.has_value());

    database.close();
    fs::remove(path);
}

TEST_CASE("BlockDatabase: Batch upsert", "[phase4][blockdb]")
{
    auto path = make_test_db_path();
    BlockDatabase database(path);
    REQUIRE(database.open().has_value());
    REQUIRE(database.init_tables().has_value());

    std::vector<Block> blocks;
    for (int idx = 0; idx < 5; ++idx)
    {
        auto blk = make_test_block();
        blk.content = "Block " + std::to_string(idx);
        blocks.push_back(std::move(blk));
    }

    auto result = database.upsert_blocks(blocks);
    REQUIRE(result.has_value());
    CHECK(database.count_blocks() == 5);

    database.close();
    fs::remove(path);
}

TEST_CASE("BlockDatabase: Delete blocks by root", "[phase4][blockdb]")
{
    auto path = make_test_db_path();
    BlockDatabase database(path);
    REQUIRE(database.open().has_value());
    REQUIRE(database.init_tables().has_value());

    const std::string root_id = BlockIDGenerator::generate();
    auto root_blk = make_test_block();
    root_blk.id = root_id;
    root_blk.root_id = root_id;
    root_blk.type = BlockType::Document;
    REQUIRE(database.upsert_block(root_blk).has_value());

    auto child = make_test_block();
    child.root_id = root_id;
    child.parent_id = root_id;
    REQUIRE(database.upsert_block(child).has_value());

    CHECK(database.count_blocks() == 2);

    auto del_result = database.delete_blocks_by_root(root_id);
    REQUIRE(del_result.has_value());
    CHECK(database.count_blocks() == 0);

    database.close();
    fs::remove(path);
}

TEST_CASE("BlockDatabase: Full-text search", "[phase4][blockdb]")
{
    auto path = make_test_db_path();
    BlockDatabase database(path);
    REQUIRE(database.open().has_value());
    REQUIRE(database.init_tables().has_value());

    auto blk1 = make_test_block();
    blk1.content = "The quick brown fox jumps over the lazy dog";
    blk1.fcontent = blk1.content;
    blk1.name = "fox doc";
    REQUIRE(database.upsert_block(blk1).has_value());

    auto blk2 = make_test_block();
    blk2.content = "A different piece of text about cats";
    blk2.fcontent = blk2.content;
    blk2.name = "cat doc";
    REQUIRE(database.upsert_block(blk2).has_value());

    auto results = database.search_blocks("fox");
    CHECK(results.size() == 1);
    CHECK(results[0].id == blk1.id);

    auto cat_results = database.search_blocks("cats");
    CHECK(cat_results.size() == 1);
    CHECK(cat_results[0].id == blk2.id);

    database.close();
    fs::remove(path);
}

TEST_CASE("BlockDatabase: Reference CRUD", "[phase4][blockdb]")
{
    auto path = make_test_db_path();
    BlockDatabase database(path);
    REQUIRE(database.open().has_value());
    REQUIRE(database.init_tables().has_value());

    auto src_block = make_test_block();
    auto def_block = make_test_block();
    REQUIRE(database.upsert_block(src_block).has_value());
    REQUIRE(database.upsert_block(def_block).has_value());

    auto ref_result =
        database.upsert_ref(src_block.id, def_block.id, def_block.root_id, "ref text");
    REQUIRE(ref_result.has_value());

    auto refs = database.get_refs_by_block(src_block.id);
    CHECK(refs.size() == 1);
    CHECK(refs[0].def_block_id == def_block.id);

    auto ref_count = database.get_ref_count(def_block.id);
    CHECK(ref_count == 1);

    auto del_result = database.delete_refs_by_block(src_block.id);
    REQUIRE(del_result.has_value());
    CHECK(database.get_ref_count(def_block.id) == 0);

    database.close();
    fs::remove(path);
}

TEST_CASE("BlockDatabase: Attribute queries", "[phase4][blockdb]")
{
    auto path = make_test_db_path();
    BlockDatabase database(path);
    REQUIRE(database.open().has_value());
    REQUIRE(database.init_tables().has_value());

    auto block = make_test_block();
    block.set_ial("custom-attr", "custom-value");
    REQUIRE(database.upsert_block(block).has_value());

    auto attrs = database.get_block_attrs(block.id);
    CHECK(attrs["id"] == block.id);
    CHECK(attrs["custom-attr"] == "custom-value");

    database.close();
    fs::remove(path);
}

TEST_CASE("BlockDatabase: Count operations", "[phase4][blockdb]")
{
    auto path = make_test_db_path();
    BlockDatabase database(path);
    REQUIRE(database.open().has_value());
    REQUIRE(database.init_tables().has_value());

    CHECK(database.count_blocks() == 0);
    CHECK(database.count_refs() == 0);

    auto block = make_test_block();
    REQUIRE(database.upsert_block(block).has_value());
    CHECK(database.count_blocks() == 1);
    CHECK(database.count_blocks_by_box("test-box") == 1);
    CHECK(database.count_blocks_by_box("other-box") == 0);

    database.close();
    fs::remove(path);
}

TEST_CASE("BlockDatabase: Type conversions", "[phase4][blockdb]")
{
    CHECK(BlockDatabase::abbr_to_block_type("d") == BlockType::Document);
    CHECK(BlockDatabase::abbr_to_block_type("p") == BlockType::Paragraph);
    CHECK(BlockDatabase::abbr_to_block_type("h") == BlockType::Heading);
    CHECK(BlockDatabase::abbr_to_block_type("l") == BlockType::List);
    CHECK(BlockDatabase::abbr_to_block_type("c") == BlockType::CodeBlock);
    CHECK(BlockDatabase::abbr_to_block_type("m") == BlockType::MathBlock);
    CHECK(BlockDatabase::abbr_to_block_type("t") == BlockType::Table);
    CHECK(BlockDatabase::abbr_to_block_type("b") == BlockType::BlockQuote);
    CHECK(BlockDatabase::abbr_to_block_type("s") == BlockType::SuperBlock);
    CHECK(BlockDatabase::abbr_to_block_type("") == BlockType::Unknown);

    CHECK(BlockDatabase::abbr_to_block_sub_type("o") == BlockSubType::OrderedList);
    CHECK(BlockDatabase::abbr_to_block_sub_type("u") == BlockSubType::UnorderedList);
    CHECK(BlockDatabase::abbr_to_block_sub_type("task") == BlockSubType::TaskList);
    CHECK(BlockDatabase::abbr_to_block_sub_type("") == BlockSubType::None);
}

TEST_CASE("BlockDatabase: Async write queue", "[phase4][blockdb]")
{
    auto path = make_test_db_path();
    BlockDatabase database(path);
    REQUIRE(database.open().has_value());
    REQUIRE(database.init_tables().has_value());

    database.start_queue_worker();

    // Queue a block upsert
    auto block = make_test_block();
    DBQueueItem item;
    item.operation = DBOperation::UpsertBlock;
    item.primary_key = block.id;
    item.data_json = "{\"id\":\"" + block.id + "\"}";
    database.queue_operation(std::move(item));

    database.flush_queue();
    database.stop_queue_worker();

    database.close();
    fs::remove(path);
}

// =============================================================================
// Phase 5: DocumentFileSystem
// =============================================================================

TEST_CASE("DocumentFileSystem: Atomic write", "[phase5][docfs]")
{
    auto tmp_dir = fs::temp_directory_path() / "markamp_docfs_test";
    fs::create_directories(tmp_dir);

    auto event_bus = std::make_shared<EventBus>();
    DocumentFileSystem docfs(nullptr, event_bus, nullptr);

    auto target = tmp_dir / "test_file.txt";
    auto result = docfs.atomic_write(target, "test content");
    REQUIRE(result.has_value());

    std::ifstream ifs(target);
    std::string content((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
    CHECK(content == "test content");

    fs::remove_all(tmp_dir);
}

TEST_CASE("DocumentFileSystem: Serialize and deserialize tree", "[phase5][docfs]")
{
    auto event_bus = std::make_shared<EventBus>();
    DocumentFileSystem docfs(nullptr, event_bus, nullptr);

    // Create a simple tree
    Block root;
    root.id = BlockIDGenerator::generate();
    root.root_id = root.id;
    root.type = BlockType::Document;
    root.content = "My Document";
    root.set_ial("title", "My Document");

    auto child = std::make_shared<Block>();
    child->id = BlockIDGenerator::generate();
    child->root_id = root.id;
    child->parent_id = root.id;
    child->type = BlockType::Paragraph;
    child->content = "Hello world";
    root.children.push_back(child);

    // Serialize
    auto json_data = docfs.serialize_tree(root);
    CHECK(json_data["ID"] == root.id);
    CHECK(json_data["Children"].size() == 1);

    // Deserialize
    auto reconstructed = docfs.deserialize_tree(json_data, "box1", "doc.sy");
    CHECK(reconstructed.id == root.id);
    CHECK(reconstructed.type == BlockType::Document);
    CHECK(reconstructed.children.size() == 1);
    CHECK(reconstructed.children[0]->content == "Hello world");
}

TEST_CASE("DocumentFileSystem: Block to SyNode conversion", "[phase5][docfs]")
{
    Block block;
    block.id = "test-id";
    block.type = BlockType::Paragraph;
    block.content = "Some content";
    block.set_ial("custom", "value");

    auto node = DocumentFileSystem::block_to_sy_node(block);
    CHECK(node.id == "test-id");
    CHECK(node.type == block.type_abbr());
    CHECK(node.content == "Some content");
    CHECK(node.properties["custom"] == "value");
}

TEST_CASE("DocumentFileSystem: SyNode to Block conversion", "[phase5][docfs]")
{
    SyNode node;
    node.id = "test-id-2";
    node.type = "p";
    node.sub_type = "";
    node.content = "Paragraph text";
    node.properties["attr1"] = "val1";

    auto block =
        DocumentFileSystem::sy_node_to_block(node, "root-1", "parent-1", "box-1", "/doc.sy");
    CHECK(block.id == "test-id-2");
    CHECK(block.type == BlockType::Paragraph);
    CHECK(block.root_id == "root-1");
    CHECK(block.parent_id == "parent-1");
    CHECK(block.box == "box-1");
    CHECK(block.path == "/doc.sy");
    CHECK(block.ial["attr1"] == "val1");
}

TEST_CASE("DocumentFileSystem: Type helpers", "[phase5][docfs]")
{
    auto abbr = DocumentFileSystem::block_type_to_node_type(BlockType::Document);
    CHECK(abbr == "d");

    auto type = DocumentFileSystem::node_type_to_block_type("d");
    CHECK(type == BlockType::Document);
}

TEST_CASE("DocumentFileSystem: Create and load document", "[phase5][docfs]")
{
    auto tmp_dir = fs::temp_directory_path() / "markamp_docfs_create_test";
    fs::create_directories(tmp_dir);

    auto config = std::make_shared<Config>();
    config->set("storage.data_dir", tmp_dir.string());
    auto event_bus = std::make_shared<EventBus>();
    auto db_path = tmp_dir / "test.db";
    auto database = std::make_shared<BlockDatabase>(db_path);
    REQUIRE(database->open().has_value());
    REQUIRE(database->init_tables().has_value());

    DocumentFileSystem docfs(config, event_bus, database);

    // Create box dir
    fs::create_directories(tmp_dir / "test-box");

    auto result = docfs.create_doc("test-box", "", "Test Document", "Initial paragraph");
    REQUIRE(result.has_value());
    CHECK(result->type == BlockType::Document);
    CHECK(result->name == "Test Document");
    CHECK(result->children.size() == 1);
    CHECK(result->children[0]->content == "Initial paragraph");

    // Verify it was saved to database
    auto db_block = database->get_block(result->id);
    REQUIRE(db_block.has_value());
    CHECK(db_block->name == "Test Document");

    database->close();
    fs::remove_all(tmp_dir);
}

TEST_CASE("DocumentFileSystem: Directory operations", "[phase5][docfs]")
{
    auto tmp_dir = fs::temp_directory_path() / "markamp_docfs_dir_test";
    fs::create_directories(tmp_dir);

    auto config = std::make_shared<Config>();
    config->set("storage.data_dir", tmp_dir.string());
    auto event_bus = std::make_shared<EventBus>();

    DocumentFileSystem docfs(config, event_bus, nullptr);

    auto result = docfs.create_dir("test-box", "", "my_folder");
    REQUIRE(result.has_value());
    CHECK(fs::exists(result.value()));

    auto remove_result = docfs.remove_dir("test-box", "my_folder");
    REQUIRE(remove_result.has_value());

    fs::remove_all(tmp_dir);
}

// =============================================================================
// Phase 6: BlockTreeIndex
// =============================================================================

TEST_CASE("BlockTreeIndex: Upsert and lookup by ID", "[phase6][index]")
{
    BlockTreeIndex index;

    BlockTreeEntry entry;
    entry.id = "block-1";
    entry.root_id = "root-1";
    entry.parent_id = "";
    entry.box_id = "box-1";
    entry.path = "/test.sy";
    entry.hpath = "/Test";
    entry.type = BlockType::Document;

    index.upsert(entry);

    auto found = index.get("block-1");
    REQUIRE(found.has_value());
    CHECK(found->root_id == "root-1");
    CHECK(found->box_id == "box-1");
    CHECK(found->type == BlockType::Document);
}

TEST_CASE("BlockTreeIndex: Find missing entry", "[phase6][index]")
{
    BlockTreeIndex index;
    auto found = index.get("nonexistent");
    REQUIRE_FALSE(found.has_value());
}

TEST_CASE("BlockTreeIndex: Update entry", "[phase6][index]")
{
    BlockTreeIndex index;

    BlockTreeEntry entry;
    entry.id = "block-2";
    entry.root_id = "root-1";
    entry.box_id = "box-1";
    entry.path = "/old.sy";
    entry.type = BlockType::Paragraph;

    index.upsert(entry);

    // Update the path
    entry.path = "/new.sy";
    index.upsert(entry);

    auto found = index.get("block-2");
    REQUIRE(found.has_value());
    CHECK(found->path == "/new.sy");
}

TEST_CASE("BlockTreeIndex: Remove entry", "[phase6][index]")
{
    BlockTreeIndex index;

    BlockTreeEntry entry;
    entry.id = "block-3";
    entry.root_id = "root-1";
    entry.box_id = "box-1";
    entry.type = BlockType::Heading;

    index.upsert(entry);
    REQUIRE(index.get("block-3").has_value());

    index.remove("block-3");
    REQUIRE_FALSE(index.get("block-3").has_value());
}

TEST_CASE("BlockTreeIndex: Query by root ID", "[phase6][index]")
{
    BlockTreeIndex index;

    for (int idx = 0; idx < 3; ++idx)
    {
        BlockTreeEntry entry;
        entry.id = "child-" + std::to_string(idx);
        entry.root_id = "root-A";
        entry.box_id = "box-1";
        entry.type = BlockType::Paragraph;
        index.upsert(entry);
    }

    BlockTreeEntry other_entry;
    other_entry.id = "other-1";
    other_entry.root_id = "root-B";
    other_entry.box_id = "box-1";
    other_entry.type = BlockType::Paragraph;
    index.upsert(other_entry);

    auto results = index.get_by_root("root-A");
    CHECK(results.size() == 3);

    auto other_results = index.get_by_root("root-B");
    CHECK(other_results.size() == 1);
}

TEST_CASE("BlockTreeIndex: Query by box ID", "[phase6][index]")
{
    BlockTreeIndex index;

    BlockTreeEntry entry1;
    entry1.id = "b-1";
    entry1.box_id = "box-alpha";
    entry1.type = BlockType::Paragraph;
    index.upsert(entry1);

    BlockTreeEntry entry2;
    entry2.id = "b-2";
    entry2.box_id = "box-beta";
    entry2.type = BlockType::Paragraph;
    index.upsert(entry2);

    CHECK(index.get_by_box("box-alpha").size() == 1);
    CHECK(index.get_by_box("box-beta").size() == 1);
    CHECK(index.get_by_box("box-gamma").empty());
}

TEST_CASE("BlockTreeIndex: Count operations", "[phase6][index]")
{
    BlockTreeIndex index;

    CHECK(index.count() == 0);

    BlockTreeEntry entry;
    entry.id = "cnt-1";
    entry.root_id = "root-1";
    entry.box_id = "box-1";
    entry.type = BlockType::Paragraph;
    index.upsert(entry);

    CHECK(index.count() == 1);
    CHECK(index.count_by_box("box-1") == 1);
    CHECK(index.count_by_box("box-2") == 0);
    CHECK(index.contains("cnt-1"));
    CHECK_FALSE(index.contains("missing"));
}

TEST_CASE("BlockTreeIndex: Clear", "[phase6][index]")
{
    BlockTreeIndex index;

    BlockTreeEntry entry;
    entry.id = "clear-1";
    entry.root_id = "root-1";
    entry.box_id = "box-1";
    entry.type = BlockType::Paragraph;
    index.upsert(entry);

    CHECK(index.count() == 1);
    index.clear();
    CHECK(index.count() == 0);
}

TEST_CASE("BlockTreeIndex: Orphan detection", "[phase6][index]")
{
    BlockTreeIndex index;

    // Root block
    BlockTreeEntry root_entry;
    root_entry.id = "root-1";
    root_entry.root_id = "root-1";
    root_entry.parent_id = "";
    root_entry.box_id = "box-1";
    root_entry.type = BlockType::Document;
    index.upsert(root_entry);

    // Child with valid parent
    BlockTreeEntry child;
    child.id = "child-1";
    child.root_id = "root-1";
    child.parent_id = "root-1";
    child.box_id = "box-1";
    child.type = BlockType::Paragraph;
    index.upsert(child);

    // Orphan — parent does not exist
    BlockTreeEntry orphan;
    orphan.id = "orphan-1";
    orphan.root_id = "root-1";
    orphan.parent_id = "nonexistent-parent";
    orphan.box_id = "box-1";
    orphan.type = BlockType::Paragraph;
    index.upsert(orphan);

    auto orphans = index.find_orphans();
    CHECK(orphans.size() == 1);
    CHECK(orphans[0].id == "orphan-1");
}

TEST_CASE("BlockTreeIndex: Thread safety – concurrent upserts", "[phase6][index]")
{
    BlockTreeIndex index;
    constexpr int num_threads = 4;
    constexpr int per_thread = 50;

    std::vector<std::thread> threads;
    threads.reserve(num_threads);

    for (int thread_idx = 0; thread_idx < num_threads; ++thread_idx)
    {
        threads.emplace_back(
            [&index, thread_idx]()
            {
                for (int item_idx = 0; item_idx < per_thread; ++item_idx)
                {
                    BlockTreeEntry entry;
                    entry.id = "t" + std::to_string(thread_idx) + "-" + std::to_string(item_idx);
                    entry.root_id = "root-concurrent";
                    entry.box_id = "box-concurrent";
                    entry.type = BlockType::Paragraph;
                    index.upsert(entry);
                }
            });
    }

    for (auto& thr : threads)
    {
        thr.join();
    }

    CHECK(index.count() == num_threads * per_thread);
}

// =============================================================================
// Event integration tests
// =============================================================================

TEST_CASE("Document events: Proper type names", "[events]")
{
    events::DocumentLoadedEvent load_evt;
    load_evt.box_id = "box-1";
    load_evt.path = "/test.sy";
    CHECK(load_evt.type_name() == "DocumentLoadedEvent");

    events::DocumentSavedEvent save_evt;
    CHECK(save_evt.type_name() == "DocumentSavedEvent");

    events::DocumentCreatedEvent create_evt;
    CHECK(create_evt.type_name() == "DocumentCreatedEvent");

    events::DocumentRenamedEvent rename_evt;
    CHECK(rename_evt.type_name() == "DocumentRenamedEvent");

    events::DocumentRemovedEvent remove_evt;
    CHECK(remove_evt.type_name() == "DocumentRemovedEvent");

    events::DocumentMovedEvent move_evt;
    CHECK(move_evt.type_name() == "DocumentMovedEvent");
}

TEST_CASE("Document events: EventBus integration", "[events]")
{
    EventBus bus;
    bool received = false;
    std::string received_box;

    auto sub = bus.subscribe<events::DocumentCreatedEvent>(
        [&](const events::DocumentCreatedEvent& evt)
        {
            received = true;
            received_box = evt.box_id;
        });

    events::DocumentCreatedEvent evt;
    evt.box_id = "my-box";
    evt.block_id = "block-123";
    evt.title = "New Doc";
    bus.publish(evt);

    CHECK(received);
    CHECK(received_box == "my-box");
}
