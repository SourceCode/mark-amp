/// test_safe_path.cpp — V7 Phase 08: SafePath tests

#include "core/SafePath.h"

#include <catch2/catch_test_macros.hpp>

#include <filesystem>

using namespace markamp::core;
namespace fs = std::filesystem;

TEST_CASE("SafePath::resolve: accepts simple relative path", "[safe_path]")
{
    auto tmp = fs::temp_directory_path();
    auto result = SafePath::resolve(tmp, "subdir/file.txt");
    REQUIRE(result.has_value());
    // macOS resolves /tmp -> /private/tmp, so use weakly_canonical for comparison
    auto canonical_tmp = fs::weakly_canonical(tmp).string();
    REQUIRE(result->string().starts_with(canonical_tmp));
}

TEST_CASE("SafePath::resolve: rejects directory traversal", "[safe_path]")
{
    auto tmp = fs::temp_directory_path();
    auto result = SafePath::resolve(tmp, "../../../etc/passwd");
    REQUIRE_FALSE(result.has_value());
    REQUIRE(result.error().code == ErrorCode::DirectoryTraversal);
}

TEST_CASE("SafePath::resolve: rejects absolute relative path", "[safe_path]")
{
    auto tmp = fs::temp_directory_path();
    auto result = SafePath::resolve(tmp, "/etc/passwd");
    REQUIRE_FALSE(result.has_value());
    REQUIRE(result.error().code == ErrorCode::DirectoryTraversal);
}

TEST_CASE("SafePath::resolve: rejects null bytes", "[safe_path]")
{
    auto tmp = fs::temp_directory_path();
    std::string with_null = "file";
    with_null += '\0';
    with_null += ".txt";
    auto result = SafePath::resolve(tmp, std::string_view(with_null.data(), with_null.size()));
    REQUIRE_FALSE(result.has_value());
    REQUIRE(result.error().code == ErrorCode::NullByte);
}

TEST_CASE("SafePath::join: works like resolve", "[safe_path]")
{
    auto tmp = fs::temp_directory_path();
    auto result = SafePath::join(tmp, "data/notes.md");
    REQUIRE(result.has_value());
}

TEST_CASE("SafePath::join: rejects traversal", "[safe_path]")
{
    auto tmp = fs::temp_directory_path();
    auto result = SafePath::join(tmp, "../../outside");
    REQUIRE_FALSE(result.has_value());
}

TEST_CASE("SafePath::is_within: returns true for child path", "[safe_path]")
{
    auto tmp = fs::temp_directory_path();
    auto child = tmp / "subdir" / "file.txt";
    REQUIRE(SafePath::is_within(child, tmp));
}

TEST_CASE("SafePath::is_within: returns false for outside path", "[safe_path]")
{
    auto home = fs::path("/Users/test");
    auto outside = fs::path("/etc/passwd");
    REQUIRE_FALSE(SafePath::is_within(outside, home));
}
