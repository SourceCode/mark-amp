/// test_tag.cpp — Unit tests for Tag types and TagParser/TagIndex
#include "core/Tag.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

// =============================================================================
// TagInfo struct tests
// =============================================================================

TEST_CASE("TagInfo: default constructed has expected defaults", "[tag]")
{
    TagInfo info;
    REQUIRE(info.label.empty());
    REQUIRE(info.full_path.empty());
    REQUIRE(info.direct_count == 0);
    REQUIRE(info.total_count == 0);
    REQUIRE(info.depth == 0);
    REQUIRE_FALSE(info.has_children());
}

TEST_CASE("TagInfo: has_children returns true when children present", "[tag]")
{
    TagInfo info;
    info.label = "parent";
    TagInfo child;
    child.label = "child";
    info.children.push_back(child);
    REQUIRE(info.has_children());
}

// =============================================================================
// ParsedTag struct tests
// =============================================================================

TEST_CASE("ParsedTag: default values", "[tag]")
{
    ParsedTag parsed;
    REQUIRE(parsed.full_path.empty());
    REQUIRE(parsed.source_offset == 0);
    REQUIRE(parsed.source_length == 0);
}

TEST_CASE("ParsedTag: components splits hierarchical path", "[tag]")
{
    ParsedTag parsed;
    parsed.full_path = "project/work/alpha";
    auto comps = parsed.components();
    REQUIRE(comps.size() == 3);
    REQUIRE(comps[0] == "project");
    REQUIRE(comps[1] == "work");
    REQUIRE(comps[2] == "alpha");
}

TEST_CASE("ParsedTag: components with single-level tag", "[tag]")
{
    ParsedTag parsed;
    parsed.full_path = "simple";
    auto comps = parsed.components();
    REQUIRE(comps.size() == 1);
    REQUIRE(comps[0] == "simple");
}

// =============================================================================
// TagTree struct tests
// =============================================================================

TEST_CASE("TagTree: default is empty", "[tag]")
{
    TagTree tree;
    REQUIRE(tree.is_empty());
    REQUIRE(tree.unique_tag_count == 0);
    REQUIRE(tree.total_usage_count == 0);
}

TEST_CASE("TagTree: not empty after adding roots", "[tag]")
{
    TagTree tree;
    TagInfo root;
    root.label = "work";
    tree.roots.push_back(root);
    REQUIRE_FALSE(tree.is_empty());
}

// =============================================================================
// TagParser tests
// =============================================================================

TEST_CASE("TagParser: validate_tag accepts valid tag paths", "[tag][parser]")
{
    TagParser parser;
    REQUIRE(parser.validate_tag("work"));
    REQUIRE(parser.validate_tag("project/alpha"));
    REQUIRE(parser.validate_tag("a-b_c/d"));
}

TEST_CASE("TagParser: validate_tag rejects invalid paths", "[tag][parser]")
{
    TagParser parser;
    REQUIRE_FALSE(parser.validate_tag(""));
    REQUIRE_FALSE(parser.validate_tag("/leading-slash"));
    REQUIRE_FALSE(parser.validate_tag("trailing/"));
    REQUIRE_FALSE(parser.validate_tag("double//slash"));
}

TEST_CASE("TagParser: split_path splits correctly", "[tag][parser]")
{
    auto parts = TagParser::split_path("a/b/c");
    REQUIRE(parts.size() == 3);
    REQUIRE(parts[0] == "a");
    REQUIRE(parts[1] == "b");
    REQUIRE(parts[2] == "c");
}

TEST_CASE("TagParser: join_path joins correctly", "[tag][parser]")
{
    std::vector<std::string> parts = {"project", "work"};
    REQUIRE(TagParser::join_path(parts) == "project/work");
}

TEST_CASE("TagParser: parse_tags extracts tags from content", "[tag][parser]")
{
    TagParser parser;
    auto tags = parser.parse_tags("Some text #work# and #project/alpha# here");
    REQUIRE(tags.size() == 2);
    REQUIRE(tags[0].full_path == "work");
    REQUIRE(tags[1].full_path == "project/alpha");
}

TEST_CASE("TagParser: parse_tags ignores markdown headings", "[tag][parser]")
{
    TagParser parser;
    auto tags = parser.parse_tags("# Heading\nSome #valid# text");
    // Should only find #valid#, not the heading #
    REQUIRE(tags.size() == 1);
    REQUIRE(tags[0].full_path == "valid");
}

// =============================================================================
// TagIndex tests
// =============================================================================

TEST_CASE("TagIndex: add and retrieve tags for a block", "[tag][index]")
{
    TagIndex index;
    BlockId bid{"20240101120000-abcdefg"};
    index.add_tag(bid, "work");
    index.add_tag(bid, "project/alpha");

    auto tags = index.get_tags_for_block(bid);
    REQUIRE(tags.size() == 2);
}

TEST_CASE("TagIndex: get_blocks_with_tag returns correct blocks", "[tag][index]")
{
    TagIndex index;
    BlockId b1{"20240101120000-aaaaaaa"};
    BlockId b2{"20240101120000-bbbbbbb"};
    index.add_tag(b1, "work");
    index.add_tag(b2, "work");

    auto blocks = index.get_blocks_with_tag("work");
    REQUIRE(blocks.size() == 2);
}

TEST_CASE("TagIndex: remove_tag removes specific tag", "[tag][index]")
{
    TagIndex index;
    BlockId bid{"20240101120000-abcdefg"};
    index.add_tag(bid, "work");
    index.add_tag(bid, "personal");
    index.remove_tag(bid, "work");

    auto tags = index.get_tags_for_block(bid);
    REQUIRE(tags.size() == 1);
    REQUIRE(tags[0] == "personal");
}

TEST_CASE("TagIndex: remove_block removes all tags", "[tag][index]")
{
    TagIndex index;
    BlockId bid{"20240101120000-abcdefg"};
    index.add_tag(bid, "work");
    index.add_tag(bid, "personal");
    index.remove_block(bid);

    auto tags = index.get_tags_for_block(bid);
    REQUIRE(tags.empty());
}

TEST_CASE("TagIndex: tag_count returns correct number", "[tag][index]")
{
    TagIndex index;
    BlockId b1{"20240101120000-aaaaaaa"};
    BlockId b2{"20240101120000-bbbbbbb"};
    index.add_tag(b1, "work");
    index.add_tag(b2, "work");
    REQUIRE(index.tag_count("work") == 2);
    REQUIRE(index.tag_count("nonexistent") == 0);
}

TEST_CASE("TagIndex: all_tag_paths returns all unique paths", "[tag][index]")
{
    TagIndex index;
    BlockId bid{"20240101120000-abcdefg"};
    index.add_tag(bid, "work");
    index.add_tag(bid, "project");
    auto paths = index.all_tag_paths();
    REQUIRE(paths.size() == 2);
}

TEST_CASE("TagIndex: clear empties the index", "[tag][index]")
{
    TagIndex index;
    BlockId bid{"20240101120000-abcdefg"};
    index.add_tag(bid, "work");
    index.clear();
    REQUIRE(index.all_tag_paths().empty());
}

TEST_CASE("TagIndex: build_tree produces valid tree", "[tag][index]")
{
    TagIndex index;
    BlockId bid{"20240101120000-abcdefg"};
    index.add_tag(bid, "project/alpha");
    index.add_tag(bid, "project/beta");
    auto tree = index.build_tree();
    REQUIRE_FALSE(tree.is_empty());
}

TEST_CASE("TagIndex: search_tags with prefix match", "[tag][index]")
{
    TagIndex index;
    BlockId bid{"20240101120000-abcdefg"};
    index.add_tag(bid, "project/alpha");
    index.add_tag(bid, "project/beta");
    index.add_tag(bid, "work");

    auto results = index.search_tags("proj");
    REQUIRE_FALSE(results.empty());
}

TEST_CASE("TagIndex: get_blocks_with_tag_recursive", "[tag][index]")
{
    TagIndex index;
    BlockId b1{"20240101120000-aaaaaaa"};
    BlockId b2{"20240101120000-bbbbbbb"};
    index.add_tag(b1, "project/alpha");
    index.add_tag(b2, "project/beta");

    auto blocks = index.get_blocks_with_tag_recursive("project");
    REQUIRE(blocks.size() == 2);
}
