// test_selective_sync_filter.cpp — 10 tests for SelectiveSyncFilter
#include "core/Config.h"
#include "core/SelectiveSyncFilter.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("SelectiveSyncFilter starts with no user patterns", "[sync][filter]")
{
    Config config;
    SelectiveSyncFilter filter(config);
    CHECK(filter.pattern_count() == 0);
}

TEST_CASE("SelectiveSyncFilter add_pattern", "[sync][filter]")
{
    Config config;
    SelectiveSyncFilter filter(config);
    filter.add_pattern("*.tmp");
    CHECK(filter.pattern_count() == 1);
    CHECK(filter.patterns().size() == 1);
}

TEST_CASE("SelectiveSyncFilter remove_pattern", "[sync][filter]")
{
    Config config;
    SelectiveSyncFilter filter(config);
    filter.add_pattern("*.log");
    CHECK(filter.remove_pattern("*.log"));
    CHECK(filter.pattern_count() == 0);
}

TEST_CASE("SelectiveSyncFilter remove nonexistent returns false", "[sync][filter]")
{
    Config config;
    SelectiveSyncFilter filter(config);
    CHECK_FALSE(filter.remove_pattern("ghost"));
}

TEST_CASE("SelectiveSyncFilter exclude_folder", "[sync][filter]")
{
    Config config;
    SelectiveSyncFilter filter(config);
    filter.exclude_folder("node_modules");
    CHECK(filter.is_folder_excluded("node_modules"));
    CHECK_FALSE(filter.excluded_folders().empty());
}

TEST_CASE("SelectiveSyncFilter include_folder removes exclusion", "[sync][filter]")
{
    Config config;
    SelectiveSyncFilter filter(config);
    filter.exclude_folder("build");
    filter.include_folder("build");
    CHECK_FALSE(filter.is_folder_excluded("build"));
}

TEST_CASE("SelectiveSyncFilter cloud_only files", "[sync][filter]")
{
    Config config;
    SelectiveSyncFilter filter(config);
    filter.add_cloud_only_file("large-asset.bin");
    CHECK(filter.is_cloud_only("large-asset.bin"));
    CHECK(filter.cloud_only_files().size() == 1);
}

TEST_CASE("SelectiveSyncFilter remove_cloud_only_file", "[sync][filter]")
{
    Config config;
    SelectiveSyncFilter filter(config);
    filter.add_cloud_only_file("file.bin");
    CHECK(filter.remove_cloud_only_file("file.bin"));
    CHECK_FALSE(filter.is_cloud_only("file.bin"));
}

TEST_CASE("SelectiveSyncFilter use_defaults toggle", "[sync][filter]")
{
    Config config;
    SelectiveSyncFilter filter(config);
    CHECK(filter.use_defaults());
    filter.set_use_defaults(false);
    CHECK_FALSE(filter.use_defaults());
}

TEST_CASE("SelectiveSyncFilter serialize produces output", "[sync][filter]")
{
    Config config;
    SelectiveSyncFilter filter(config);
    filter.add_pattern("*.tmp");
    filter.add_pattern("*.log");
    auto serialized = filter.serialize();
    CHECK_FALSE(serialized.empty());
}
