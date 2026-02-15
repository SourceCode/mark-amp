#include "core/Block.h"

#include <catch2/catch_test_macros.hpp>

#include <algorithm>
#include <vector>

using namespace markamp::core;

TEST_CASE("Block default construction", "[block][phase01]")
{
    Block block;
    CHECK(block.id.empty());
    CHECK(block.type == BlockType::Paragraph);
    CHECK(block.sub_type == BlockSubType::None);
    CHECK(block.sort == 0);
    CHECK(block.folded == false);
    CHECK(block.depth == 0);
    CHECK(block.children.empty());
    CHECK(block.refs.empty());
    CHECK(block.defs.empty());
    CHECK(block.created == 0);
    CHECK(block.updated == 0);
}

TEST_CASE("Block type detection", "[block][phase01]")
{
    SECTION("Document is container, not heading, not leaf")
    {
        Block b;
        b.type = BlockType::Document;
        CHECK(b.is_document());
        CHECK_FALSE(b.is_heading());
        CHECK(b.is_container());
        CHECK_FALSE(b.is_leaf());
    }

    SECTION("Paragraph is leaf, not container, not heading, not document")
    {
        Block b;
        b.type = BlockType::Paragraph;
        CHECK_FALSE(b.is_document());
        CHECK_FALSE(b.is_heading());
        CHECK_FALSE(b.is_container());
        CHECK(b.is_leaf());
    }

    SECTION("Heading is leaf, not container")
    {
        Block b;
        b.type = BlockType::Heading;
        CHECK_FALSE(b.is_document());
        CHECK(b.is_heading());
        CHECK_FALSE(b.is_container());
        CHECK(b.is_leaf());
    }

    SECTION("List is container")
    {
        Block b;
        b.type = BlockType::List;
        CHECK(b.is_container());
        CHECK_FALSE(b.is_leaf());
    }

    SECTION("ListItem is container")
    {
        Block b;
        b.type = BlockType::ListItem;
        CHECK(b.is_container());
    }

    SECTION("BlockQuote is container")
    {
        Block b;
        b.type = BlockType::BlockQuote;
        CHECK(b.is_container());
    }

    SECTION("SuperBlock is container")
    {
        Block b;
        b.type = BlockType::SuperBlock;
        CHECK(b.is_container());
    }

    SECTION("Table and TableRow are containers")
    {
        Block b;
        b.type = BlockType::Table;
        CHECK(b.is_container());

        b.type = BlockType::TableRow;
        CHECK(b.is_container());
    }

    SECTION("CalloutBlock is container")
    {
        Block b;
        b.type = BlockType::CalloutBlock;
        CHECK(b.is_container());
    }

    SECTION("CodeBlock, MathBlock, TableCell, HTMLBlock are leaves")
    {
        Block b;
        for (auto t : {BlockType::CodeBlock,
                       BlockType::MathBlock,
                       BlockType::TableCell,
                       BlockType::HTMLBlock,
                       BlockType::ThematicBreak})
        {
            b.type = t;
            CHECK(b.is_leaf());
        }
    }
}

TEST_CASE("Block type abbreviation", "[block][phase01]")
{
    Block b;

    b.type = BlockType::Document;
    CHECK(b.type_abbr() == "d");
    b.type = BlockType::Paragraph;
    CHECK(b.type_abbr() == "p");
    b.type = BlockType::Heading;
    CHECK(b.type_abbr() == "h");
    b.type = BlockType::List;
    CHECK(b.type_abbr() == "l");
    b.type = BlockType::CodeBlock;
    CHECK(b.type_abbr() == "c");
    b.type = BlockType::Table;
    CHECK(b.type_abbr() == "t");
    b.type = BlockType::SuperBlock;
    CHECK(b.type_abbr() == "s");
    b.type = BlockType::EmbedBlock;
    CHECK(b.type_abbr() == "e");
    b.type = BlockType::AttributeView;
    CHECK(b.type_abbr() == "av");
    b.type = BlockType::Unknown;
    CHECK(b.type_abbr() == "unknown");
}

TEST_CASE("Block subtype abbreviation", "[block][phase01]")
{
    Block b;

    b.sub_type = BlockSubType::None;
    CHECK(b.sub_type_abbr().empty());
    b.sub_type = BlockSubType::Heading1;
    CHECK(b.sub_type_abbr() == "h1");
    b.sub_type = BlockSubType::Heading6;
    CHECK(b.sub_type_abbr() == "h6");
    b.sub_type = BlockSubType::OrderedList;
    CHECK(b.sub_type_abbr() == "o");
    b.sub_type = BlockSubType::UnorderedList;
    CHECK(b.sub_type_abbr() == "u");
    b.sub_type = BlockSubType::TaskList;
    CHECK(b.sub_type_abbr() == "t");
}

TEST_CASE("IAL set and get", "[block][phase01]")
{
    Block b;
    b.set_ial("id", "20210808180117-6v0mkxr");
    b.set_ial("name", "test block");
    b.set_ial("custom-color", "red");

    CHECK(b.get_ial("id") == "20210808180117-6v0mkxr");
    CHECK(b.get_ial("name") == "test block");
    CHECK(b.get_ial("custom-color") == "red");
    CHECK(b.get_ial("nonexistent") == std::nullopt);
}

TEST_CASE("IAL remove", "[block][phase01]")
{
    Block b;
    b.set_ial("a", "1");
    b.set_ial("b", "2");
    b.set_ial("c", "3");

    b.remove_ial("b");
    CHECK(b.get_ial("b") == std::nullopt);
    CHECK(b.get_ial("a") == "1");
    CHECK(b.get_ial("c") == "3");

    // Remove non-existent key — no crash
    b.remove_ial("nonexistent");
}

TEST_CASE("IAL string serialization", "[block][phase01]")
{
    Block b;
    b.set_ial("id", "20210808180117-6v0mkxr");
    b.set_ial("name", "test");
    b.set_ial("custom-color", "red");

    auto result = b.ial_string();
    // Keys must be alphabetically sorted
    CHECK(result == "{: custom-color=\"red\" id=\"20210808180117-6v0mkxr\" name=\"test\" }");

    // Empty IAL
    Block empty;
    CHECK(empty.ial_string().empty());
}

TEST_CASE("IAL string with special characters", "[block][phase01]")
{
    Block b;
    b.set_ial("title", "He said \"hello\"");

    auto result = b.ial_string();
    CHECK(result == "{: title=\"He said \\\"hello\\\"\" }");
}

TEST_CASE("Block sorting by sort field", "[block][phase01]")
{
    std::vector<Block> blocks(5);
    blocks[0].id = "e";
    blocks[0].sort = 5;
    blocks[1].id = "d";
    blocks[1].sort = 1;
    blocks[2].id = "c";
    blocks[2].sort = 3;
    blocks[3].id = "b";
    blocks[3].sort = 1; // same sort as d, tiebreak by id
    blocks[4].id = "a";
    blocks[4].sort = 0;

    std::sort(blocks.begin(), blocks.end(), compare_blocks_by_sort);

    CHECK(blocks[0].id == "a"); // sort=0
    CHECK(blocks[1].id == "b"); // sort=1, id="b" < "d"
    CHECK(blocks[2].id == "d"); // sort=1, id="d"
    CHECK(blocks[3].id == "c"); // sort=3
    CHECK(blocks[4].id == "e"); // sort=5
}

TEST_CASE("Block sorting by updated", "[block][phase01]")
{
    std::vector<Block> blocks(5);
    blocks[0].id = "a";
    blocks[0].updated = 100;
    blocks[1].id = "b";
    blocks[1].updated = 500;
    blocks[2].id = "c";
    blocks[2].updated = 300;
    blocks[3].id = "d";
    blocks[3].updated = 500; // same as b, tiebreak by id
    blocks[4].id = "e";
    blocks[4].updated = 200;

    std::sort(blocks.begin(), blocks.end(), compare_blocks_by_updated);

    CHECK(blocks[0].id == "b"); // updated=500, id="b" < "d"
    CHECK(blocks[1].id == "d"); // updated=500, id="d"
    CHECK(blocks[2].id == "c"); // updated=300
    CHECK(blocks[3].id == "e"); // updated=200
    CHECK(blocks[4].id == "a"); // updated=100
}

TEST_CASE("Block parent-child relationships", "[block][phase01]")
{
    auto doc = std::make_shared<Block>();
    doc->id = "doc1";
    doc->type = BlockType::Document;

    for (int i = 0; i < 3; ++i)
    {
        auto child = std::make_shared<Block>();
        child->id = "p" + std::to_string(i);
        child->parent_id = doc->id;
        child->root_id = doc->id;
        child->type = BlockType::Paragraph;
        doc->children.push_back(child);
    }

    CHECK(doc->children.size() == 3);
    CHECK(doc->children[0]->parent_id == "doc1");
    CHECK(doc->children[1]->root_id == "doc1");
    CHECK(doc->children[2]->type == BlockType::Paragraph);
}

TEST_CASE("BlockRef construction", "[block][phase01]")
{
    BlockRef ref;
    ref.block_id = "20210808180117-6v0mkxr";
    ref.ref_text = "Referenced block";
    ref.def_block_id = "20210808180117-abc1234";
    ref.def_block_path = "/notebook/document.sy";

    CHECK(ref.block_id == "20210808180117-6v0mkxr");
    CHECK(ref.ref_text == "Referenced block");
    CHECK(ref.def_block_id == "20210808180117-abc1234");
    CHECK(ref.def_block_path == "/notebook/document.sy");
}
