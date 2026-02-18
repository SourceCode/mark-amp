/// test_snapshot_store.cpp — Unit tests
#include "core/SnapshotStore.h"
#include <catch2/catch_test_macros.hpp>
#include <string>
using namespace markamp::core;

TEST_CASE("SnapshotStore: compiles", "[snapshot_store]")
{
    static_assert(sizeof(SnapshotStore<std::string>) > 0);
}
