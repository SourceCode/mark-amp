// test_sync_ignore_patterns.cpp — 10 tests for SyncIgnorePatterns
#include "core/CloudSyncTypes.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("SyncIgnorePatterns default_patterns has common excludes", "[sync][ignore]")
{
    auto defaults = SyncIgnorePatterns::default_patterns();
    CHECK_FALSE(defaults.empty());
}

TEST_CASE("SyncIgnorePatterns default has .DS_Store", "[sync][ignore]")
{
    auto defaults = SyncIgnorePatterns::default_patterns();
    bool found = false;
    for (const auto& pattern : defaults)
    {
        if (pattern == ".DS_Store")
        {
            found = true;
            break;
        }
    }
    CHECK(found);
}

TEST_CASE("SyncIgnorePatterns default has .git/", "[sync][ignore]")
{
    auto defaults = SyncIgnorePatterns::default_patterns();
    bool found = false;
    for (const auto& pattern : defaults)
    {
        if (pattern == ".git/")
        {
            found = true;
            break;
        }
    }
    CHECK(found);
}

TEST_CASE("SyncIgnorePatterns default has node_modules/", "[sync][ignore]")
{
    auto defaults = SyncIgnorePatterns::default_patterns();
    bool found = false;
    for (const auto& pattern : defaults)
    {
        if (pattern == "node_modules/")
        {
            found = true;
            break;
        }
    }
    CHECK(found);
}

TEST_CASE("SyncIgnorePatterns default has *.tmp", "[sync][ignore]")
{
    auto defaults = SyncIgnorePatterns::default_patterns();
    bool found = false;
    for (const auto& pattern : defaults)
    {
        if (pattern == "*.tmp")
        {
            found = true;
            break;
        }
    }
    CHECK(found);
}

TEST_CASE("SyncIgnorePatterns use_defaults default is true", "[sync][ignore]")
{
    SyncIgnorePatterns patterns;
    CHECK(patterns.use_defaults);
}

TEST_CASE("SyncIgnorePatterns custom patterns storage", "[sync][ignore]")
{
    SyncIgnorePatterns patterns;
    patterns.patterns = {"*.bak", "*.old"};
    CHECK(patterns.patterns.size() == 2);
}

TEST_CASE("SyncIgnorePatterns combine custom and defaults", "[sync][ignore]")
{
    SyncIgnorePatterns patterns;
    patterns.use_defaults = true;
    patterns.patterns = {"*.custom"};
    auto all_defaults = SyncIgnorePatterns::default_patterns();
    // Custom patterns are separate from defaults
    CHECK(patterns.patterns.size() == 1);
    CHECK_FALSE(all_defaults.empty());
}

TEST_CASE("SyncIgnorePatterns empty custom patterns", "[sync][ignore]")
{
    SyncIgnorePatterns patterns;
    patterns.patterns = {};
    CHECK(patterns.patterns.empty());
}

TEST_CASE("SyncIgnorePatterns default has *.log", "[sync][ignore]")
{
    auto defaults = SyncIgnorePatterns::default_patterns();
    bool found = false;
    for (const auto& pattern : defaults)
    {
        if (pattern == "*.log")
        {
            found = true;
            break;
        }
    }
    CHECK(found);
}
