/// test_bookmark.cpp — Unit tests for Bookmark types and BookmarkIndex
#include "core/Bookmark.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

// =============================================================================
// BookmarkEntry struct tests
// =============================================================================

TEST_CASE("BookmarkEntry: default constructed fields", "[bookmark]")
{
    BookmarkEntry entry;
    REQUIRE(entry.block_id.value.empty());
    REQUIRE(entry.label.empty());
    REQUIRE(entry.root_id.empty());
    REQUIRE(entry.doc_title.empty());
    REQUIRE(entry.block_type.empty());
    REQUIRE(entry.content_snippet.empty());
}

// =============================================================================
// BookmarkGroup struct tests
// =============================================================================

TEST_CASE("BookmarkGroup: default is empty with zero count", "[bookmark]")
{
    BookmarkGroup group;
    REQUIRE(group.is_empty());
    REQUIRE(group.count() == 0);
}

TEST_CASE("BookmarkGroup: count reflects entries", "[bookmark]")
{
    BookmarkGroup group;
    group.label = "favorites";
    BookmarkEntry e1;
    e1.block_id = {"20240101120000-aaaaaaa"};
    group.entries.push_back(e1);
    REQUIRE(group.count() == 1);
    REQUIRE_FALSE(group.is_empty());
}

// =============================================================================
// BookmarkIndex tests
// =============================================================================

TEST_CASE("BookmarkIndex: add and is_bookmarked", "[bookmark][index]")
{
    BookmarkIndex index;
    BlockId bid{"20240101120000-abcdefg"};
    REQUIRE_FALSE(index.is_bookmarked(bid));

    index.add(bid, "favorites");
    REQUIRE(index.is_bookmarked(bid));
}

TEST_CASE("BookmarkIndex: get_label returns correct label", "[bookmark][index]")
{
    BookmarkIndex index;
    BlockId bid{"20240101120000-abcdefg"};
    index.add(bid, "important");
    REQUIRE(index.get_label(bid) == "important");
}

TEST_CASE("BookmarkIndex: get_label returns empty for non-bookmarked", "[bookmark][index]")
{
    BookmarkIndex index;
    BlockId bid{"20240101120000-abcdefg"};
    REQUIRE(index.get_label(bid).empty());
}

TEST_CASE("BookmarkIndex: remove removes bookmark", "[bookmark][index]")
{
    BookmarkIndex index;
    BlockId bid{"20240101120000-abcdefg"};
    index.add(bid, "favorites");
    index.remove(bid);
    REQUIRE_FALSE(index.is_bookmarked(bid));
}

TEST_CASE("BookmarkIndex: get_blocks_with_label returns matching blocks", "[bookmark][index]")
{
    BookmarkIndex index;
    BlockId b1{"20240101120000-aaaaaaa"};
    BlockId b2{"20240101120000-bbbbbbb"};
    BlockId b3{"20240101120000-ccccccc"};
    index.add(b1, "work");
    index.add(b2, "work");
    index.add(b3, "personal");

    auto blocks = index.get_blocks_with_label("work");
    REQUIRE(blocks.size() == 2);
}

TEST_CASE("BookmarkIndex: get_all_labels returns unique labels", "[bookmark][index]")
{
    BookmarkIndex index;
    BlockId b1{"20240101120000-aaaaaaa"};
    BlockId b2{"20240101120000-bbbbbbb"};
    index.add(b1, "work");
    index.add(b2, "personal");
    auto labels = index.get_all_labels();
    REQUIRE(labels.size() == 2);
}

TEST_CASE("BookmarkIndex: rename_label updates all bookmarks", "[bookmark][index]")
{
    BookmarkIndex index;
    BlockId b1{"20240101120000-aaaaaaa"};
    BlockId b2{"20240101120000-bbbbbbb"};
    index.add(b1, "old_name");
    index.add(b2, "old_name");

    auto count = index.rename_label("old_name", "new_name");
    REQUIRE(count == 2);
    REQUIRE(index.get_label(b1) == "new_name");
    REQUIRE(index.get_label(b2) == "new_name");
}

TEST_CASE("BookmarkIndex: remove_label removes all with label", "[bookmark][index]")
{
    BookmarkIndex index;
    BlockId b1{"20240101120000-aaaaaaa"};
    BlockId b2{"20240101120000-bbbbbbb"};
    index.add(b1, "temp");
    index.add(b2, "temp");

    auto count = index.remove_label("temp");
    REQUIRE(count == 2);
    REQUIRE(index.total_count() == 0);
}

TEST_CASE("BookmarkIndex: total_count tracks bookmarks", "[bookmark][index]")
{
    BookmarkIndex index;
    REQUIRE(index.total_count() == 0);

    BlockId b1{"20240101120000-aaaaaaa"};
    BlockId b2{"20240101120000-bbbbbbb"};
    index.add(b1, "a");
    index.add(b2, "b");
    REQUIRE(index.total_count() == 2);
}

TEST_CASE("BookmarkIndex: clear empties everything", "[bookmark][index]")
{
    BookmarkIndex index;
    BlockId bid{"20240101120000-abcdefg"};
    index.add(bid, "test");
    index.clear();
    REQUIRE(index.total_count() == 0);
    REQUIRE_FALSE(index.is_bookmarked(bid));
}
