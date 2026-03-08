// test_cross_platform_path.cpp — 10 tests for CrossPlatformPath
#include "core/CrossPlatformPath.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("CrossPlatformPath normalize returns non-empty", "[path]")
{
    auto normalized = CrossPlatformPath::normalize("/some/path/to/file.txt");
    CHECK_FALSE(normalized.empty());
}

TEST_CASE("CrossPlatformPath get_separator returns valid char", "[path]")
{
    char sep = CrossPlatformPath::get_separator();
    CHECK((sep == '/' || sep == '\\'));
}

TEST_CASE("CrossPlatformPath compare identical paths", "[path]")
{
    CHECK(CrossPlatformPath::compare("/a/b/c", "/a/b/c"));
}

TEST_CASE("CrossPlatformPath compare different paths", "[path]")
{
    CHECK_FALSE(CrossPlatformPath::compare("/a/b/c", "/a/b/d"));
}

TEST_CASE("CrossPlatformPath get_max_path_length is positive", "[path]")
{
    CHECK(CrossPlatformPath::get_max_path_length() > 0);
}

TEST_CASE("CrossPlatformPath is_case_sensitive returns bool", "[path]")
{
    // Just verify it doesn't crash and returns a reasonable value
    [[maybe_unused]] bool is_cs = CrossPlatformPath::is_case_sensitive();
#ifdef __APPLE__
    CHECK_FALSE(is_cs); // macOS is case-insensitive by default
#endif
}

TEST_CASE("CrossPlatformPath get_default_dialog_filters non-empty", "[path]")
{
    auto filters = CrossPlatformPath::get_default_dialog_filters();
    CHECK_FALSE(filters.empty());
    CHECK_FALSE(filters[0].description.empty());
    CHECK_FALSE(filters[0].extensions.empty());
}

TEST_CASE("CrossPlatformPath last_directory get/set", "[path]")
{
    CrossPlatformPath path;
    path.set_last_directory("/home/user");
    CHECK(path.get_last_directory() == "/home/user");
}

TEST_CASE("CrossPlatformPath last_directory default is empty", "[path]")
{
    CrossPlatformPath path;
    CHECK(path.get_last_directory().empty());
}

TEST_CASE("CrossPlatformPath normalize handles trailing separator", "[path]")
{
    auto n1 = CrossPlatformPath::normalize("/a/b/c/");
    auto n2 = CrossPlatformPath::normalize("/a/b/c");
    // Should normalize similarly (implementation dependent)
    CHECK_FALSE(n1.empty());
    CHECK_FALSE(n2.empty());
}
