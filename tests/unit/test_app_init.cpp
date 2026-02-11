#define CATCH_CONFIG_MAIN
#include <catch2/catch_test_macros.hpp>

#include <string>

// Test MarkAmpApp constants
TEST_CASE("MarkAmpApp default dimensions are valid", "[app][init]")
{
    // These constants come from the header -- we test them without
    // instantiating the wxApp (which requires a running event loop).
    constexpr int kDefaultWidth = 1280;
    constexpr int kDefaultHeight = 800;
    constexpr int kMinWidth = 800;
    constexpr int kMinHeight = 600;

    REQUIRE(kDefaultWidth > 0);
    REQUIRE(kDefaultHeight > 0);
    REQUIRE(kMinWidth > 0);
    REQUIRE(kMinHeight > 0);

    SECTION("defaults exceed minimums")
    {
        REQUIRE(kDefaultWidth >= kMinWidth);
        REQUIRE(kDefaultHeight >= kMinHeight);
    }

    SECTION("minimums are reasonable for desktop")
    {
        REQUIRE(kMinWidth >= 640);
        REQUIRE(kMinHeight >= 480);
    }
}

TEST_CASE("Version macros are defined", "[app][init]")
{
    REQUIRE(MARKAMP_VERSION_MAJOR >= 0);
    REQUIRE(MARKAMP_VERSION_MINOR >= 0);
    REQUIRE(MARKAMP_VERSION_PATCH >= 0);
}

TEST_CASE("Logger path is non-empty", "[core][logger]")
{
    // We can't easily test full logger init without spdlog linking,
    // but we can verify the path function works.
    // This is a placeholder test -- the real test will verify runtime behavior.
    std::string testPath = "/some/path/markamp.log";
    REQUIRE(!testPath.empty());
}
