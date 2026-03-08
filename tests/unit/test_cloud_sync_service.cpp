/// test_cloud_sync_service.cpp — Unit tests for CloudSyncService types
#include "core/CloudSyncService.h"

#include <catch2/catch_test_macros.hpp>
using namespace markamp::core;
TEST_CASE("CloudSyncService: type compiles", "[cloud_sync_service]")
{
    static_assert(sizeof(CloudSyncService) > 0);
}
