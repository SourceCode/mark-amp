/// test_cloud_sync_types.cpp — Unit tests
#include "core/CloudSyncTypes.h"
#include <catch2/catch_test_macros.hpp>
using namespace markamp::core;

TEST_CASE("S3Config: compiles", "[cloud_sync_types]")
{
    static_assert(sizeof(S3Config) > 0);
}

TEST_CASE("WebDavConfig: compiles", "[cloud_sync_types]")
{
    static_assert(sizeof(WebDavConfig) > 0);
}

TEST_CASE("CloudSyncConfig: compiles", "[cloud_sync_types]")
{
    static_assert(sizeof(CloudSyncConfig) > 0);
}
