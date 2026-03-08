/// test_snapshot_store.cpp
#include "core/SnapshotStore.h"

#include <catch2/catch_test_macros.hpp>
using namespace markamp::core;
TEST_CASE("SnapshotStore: template instantiates", "[snapshot_store]")
{
    static_assert(sizeof(SnapshotStore<std::string>) > 0);
}
