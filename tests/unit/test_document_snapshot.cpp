/// test_document_snapshot.cpp — Unit tests for DocumentSnapshot
#include "core/DocumentSnapshot.h"

#include <catch2/catch_test_macros.hpp>
using namespace markamp::core;

TEST_CASE("DocumentSnapshot: type compiles", "[document_snapshot]")
{
    static_assert(sizeof(DocumentSnapshot) > 0);
}
TEST_CASE("DocumentSnapshot: default construction", "[document_snapshot]")
{
    DocumentSnapshot snapshot;
    REQUIRE(snapshot.version == 0);
}
