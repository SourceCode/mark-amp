/// test_file_snapshot_service.cpp — Unit tests

#include "core/FileSnapshotService.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("FileSnapshotService: compiles", "[file_snapshot_service]")
{
    static_assert(sizeof(FileSnapshotService) > 0);
}
