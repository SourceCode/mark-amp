/// test_piece_table.cpp — Unit tests for PieceTable text buffer

#include "core/PieceTable.h"

#include <catch2/catch_test_macros.hpp>

#include <string>

using namespace markamp::core;

// ══════════════════════════════════════════════════════════════════════════════
// Construction
// ══════════════════════════════════════════════════════════════════════════════

TEST_CASE("PieceTable: default construction is empty", "[piece_table]")
{
    PieceTable pt;
    REQUIRE(pt.empty());
    REQUIRE(pt.size() == 0);
    REQUIRE(pt.text().empty());
    REQUIRE(pt.piece_count() == 0); // no pieces when empty
}

TEST_CASE("PieceTable: construction with content", "[piece_table]")
{
    PieceTable pt("Hello, World!");
    REQUIRE_FALSE(pt.empty());
    REQUIRE(pt.size() == 13);
    REQUIRE(pt.text() == "Hello, World!");
}

// ══════════════════════════════════════════════════════════════════════════════
// Insert
// ══════════════════════════════════════════════════════════════════════════════

TEST_CASE("PieceTable: insert at beginning", "[piece_table]")
{
    PieceTable pt("World");
    pt.insert(0, "Hello ");
    REQUIRE(pt.text() == "Hello World");
    REQUIRE(pt.size() == 11);
}

TEST_CASE("PieceTable: insert at end", "[piece_table]")
{
    PieceTable pt("Hello");
    pt.insert(5, " World");
    REQUIRE(pt.text() == "Hello World");
}

TEST_CASE("PieceTable: insert in middle", "[piece_table]")
{
    PieceTable pt("Heo");
    pt.insert(2, "ll");
    REQUIRE(pt.text() == "Hello");
}

TEST_CASE("PieceTable: multiple inserts", "[piece_table]")
{
    PieceTable pt;
    pt.insert(0, "a");
    pt.insert(1, "c");
    pt.insert(1, "b");
    REQUIRE(pt.text() == "abc");
}

// ══════════════════════════════════════════════════════════════════════════════
// Erase
// ══════════════════════════════════════════════════════════════════════════════

TEST_CASE("PieceTable: erase from beginning", "[piece_table]")
{
    PieceTable pt("Hello World");
    pt.erase(0, 6);
    REQUIRE(pt.text() == "World");
}

TEST_CASE("PieceTable: erase from end", "[piece_table]")
{
    PieceTable pt("Hello World");
    pt.erase(5, 6);
    REQUIRE(pt.text() == "Hello");
}

TEST_CASE("PieceTable: erase from middle", "[piece_table]")
{
    PieceTable pt("Hello World");
    pt.erase(5, 1);
    REQUIRE(pt.text() == "HelloWorld");
}

TEST_CASE("PieceTable: erase all content", "[piece_table]")
{
    PieceTable pt("Hello");
    pt.erase(0, 5);
    REQUIRE(pt.empty());
    REQUIRE(pt.size() == 0);
}

// ══════════════════════════════════════════════════════════════════════════════
// Substr and At
// ══════════════════════════════════════════════════════════════════════════════

TEST_CASE("PieceTable: substr returns correct slice", "[piece_table]")
{
    PieceTable pt("Hello World");
    REQUIRE(pt.substr(0, 5) == "Hello");
    REQUIRE(pt.substr(6, 5) == "World");
    REQUIRE(pt.substr(0, 11) == "Hello World");
}

TEST_CASE("PieceTable: at returns correct character", "[piece_table]")
{
    PieceTable pt("abc");
    REQUIRE(pt.at(0) == 'a');
    REQUIRE(pt.at(1) == 'b');
    REQUIRE(pt.at(2) == 'c');
}

// ══════════════════════════════════════════════════════════════════════════════
// Combined Operations
// ══════════════════════════════════════════════════════════════════════════════

TEST_CASE("PieceTable: insert then erase round-trips", "[piece_table]")
{
    PieceTable pt("original");
    pt.insert(4, "INSERTED");
    REQUIRE(pt.text() == "origINSERTEDinal");
    pt.erase(4, 8);
    REQUIRE(pt.text() == "original");
}

TEST_CASE("PieceTable: piece_count increases with edits", "[piece_table]")
{
    PieceTable pt("Hello");
    auto initial = pt.piece_count();
    pt.insert(2, "XX");
    REQUIRE(pt.piece_count() > initial);
}
