/// test_block_ref.cpp — Unit tests for BlockRef types and RefIndex
#include "core/BlockRef.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

// =============================================================================
// BlockId tests
// =============================================================================

TEST_CASE("BlockId: default is empty", "[block_ref]")
{
    BlockId id;
    REQUIRE(id.value.empty());
}

TEST_CASE("BlockId: equality comparison", "[block_ref]")
{
    BlockId a{"20240101120000-abcdefg"};
    BlockId b{"20240101120000-abcdefg"};
    BlockId c{"20240101120000-aaaaaaa"};
    REQUIRE(a == b);
    REQUIRE_FALSE(a == c);
}

TEST_CASE("BlockId: is_valid for valid format", "[block_ref]")
{
    BlockId valid{"20240101120000-abcdefg"};
    REQUIRE(valid.is_valid());
}

TEST_CASE("BlockId: is_valid rejects empty", "[block_ref]")
{
    BlockId empty;
    REQUIRE_FALSE(empty.is_valid());
}

TEST_CASE("BlockId: is_valid rejects malformed", "[block_ref]")
{
    BlockId bad{"not-valid"};
    REQUIRE_FALSE(bad.is_valid());
}

TEST_CASE("BlockId: generate creates valid ID", "[block_ref]")
{
    auto id = BlockId::generate();
    REQUIRE(id.is_valid());
    REQUIRE_FALSE(id.value.empty());
}

TEST_CASE("BlockId: generate creates unique IDs", "[block_ref]")
{
    auto id1 = BlockId::generate();
    auto id2 = BlockId::generate();
    REQUIRE_FALSE(id1 == id2);
}

// =============================================================================
// BlockIdHash tests
// =============================================================================

TEST_CASE("BlockIdHash: hashes equal IDs to same value", "[block_ref]")
{
    BlockIdHash hasher;
    BlockId a{"20240101120000-abcdefg"};
    BlockId b{"20240101120000-abcdefg"};
    REQUIRE(hasher(a) == hasher(b));
}

// =============================================================================
// ParsedRef tests
// =============================================================================

TEST_CASE("ParsedRef: default type is BlockRef", "[block_ref]")
{
    ParsedRef ref;
    REQUIRE(ref.type == RefType::BlockRef);
    REQUIRE(ref.source_offset == 0);
    REQUIRE(ref.source_length == 0);
    REQUIRE_FALSE(ref.anchor_text.has_value());
    REQUIRE_FALSE(ref.asset_path.has_value());
}

// =============================================================================
// ResolvedRef tests
// =============================================================================

TEST_CASE("ResolvedRef: default target does not exist", "[block_ref]")
{
    ResolvedRef resolved;
    REQUIRE_FALSE(resolved.target_exists);
    REQUIRE(resolved.display_text.empty());
    REQUIRE(resolved.target_type.empty());
    REQUIRE(resolved.target_snippet.empty());
}

// =============================================================================
// RefIndex tests
// =============================================================================

TEST_CASE("RefIndex: empty index has zero refs", "[block_ref][ref_index]")
{
    RefIndex index;
    REQUIRE(index.total_refs() == 0);
}

TEST_CASE("RefIndex: add_ref and get_refs_to", "[block_ref][ref_index]")
{
    RefIndex index;
    BlockId source{"20240101120000-aaaaaaa"};
    BlockId def{"20240101120000-bbbbbbb"};

    index.add_ref(source, def, RefType::BlockRef);
    auto refs = index.get_refs_to(def);
    REQUIRE(refs.size() == 1);
    REQUIRE(refs[0] == source);
}

TEST_CASE("RefIndex: add_ref and get_refs_from", "[block_ref][ref_index]")
{
    RefIndex index;
    BlockId source{"20240101120000-aaaaaaa"};
    BlockId def{"20240101120000-bbbbbbb"};

    index.add_ref(source, def, RefType::BlockRef);
    auto refs = index.get_refs_from(source);
    REQUIRE(refs.size() == 1);
    REQUIRE(refs[0] == def);
}

TEST_CASE("RefIndex: ref_count tracks references", "[block_ref][ref_index]")
{
    RefIndex index;
    BlockId s1{"20240101120000-aaaaaaa"};
    BlockId s2{"20240101120000-ccccccc"};
    BlockId def{"20240101120000-bbbbbbb"};

    index.add_ref(s1, def, RefType::BlockRef);
    index.add_ref(s2, def, RefType::BlockEmbed);
    REQUIRE(index.ref_count(def) == 2);
}

TEST_CASE("RefIndex: remove_refs_by_source clears source refs", "[block_ref][ref_index]")
{
    RefIndex index;
    BlockId source{"20240101120000-aaaaaaa"};
    BlockId def{"20240101120000-bbbbbbb"};

    index.add_ref(source, def, RefType::BlockRef);
    index.remove_refs_by_source(source);
    REQUIRE(index.get_refs_to(def).empty());
}

TEST_CASE("RefIndex: has_circular_ref detects cycles", "[block_ref][ref_index]")
{
    RefIndex index;
    BlockId a{"20240101120000-aaaaaaa"};
    BlockId b{"20240101120000-bbbbbbb"};

    index.add_ref(a, b, RefType::BlockRef);
    index.add_ref(b, a, RefType::BlockRef);
    REQUIRE(index.has_circular_ref(a, b));
}

TEST_CASE("RefIndex: has_circular_ref returns false when no cycle", "[block_ref][ref_index]")
{
    RefIndex index;
    BlockId a{"20240101120000-aaaaaaa"};
    BlockId b{"20240101120000-bbbbbbb"};

    index.add_ref(a, b, RefType::BlockRef);
    REQUIRE_FALSE(index.has_circular_ref(a, b));
}

TEST_CASE("RefIndex: clear resets everything", "[block_ref][ref_index]")
{
    RefIndex index;
    BlockId source{"20240101120000-aaaaaaa"};
    BlockId def{"20240101120000-bbbbbbb"};
    index.add_ref(source, def, RefType::BlockRef);
    index.clear();
    REQUIRE(index.total_refs() == 0);
    REQUIRE(index.get_refs_to(def).empty());
}

TEST_CASE("RefIndex: total_refs tracks count", "[block_ref][ref_index]")
{
    RefIndex index;
    BlockId s1{"20240101120000-aaaaaaa"};
    BlockId d1{"20240101120000-bbbbbbb"};
    BlockId d2{"20240101120000-ccccccc"};

    index.add_ref(s1, d1, RefType::BlockRef);
    index.add_ref(s1, d2, RefType::BlockEmbed);
    REQUIRE(index.total_refs() == 2);
}
