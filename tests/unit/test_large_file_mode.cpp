/// test_large_file_mode.cpp — Phase 35: LargeFileMode tests

#include "core/LargeFileMode.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("LargeFileMode: inactive by default", "[large_file_mode]")
{
    LargeFileMode lfm;
    REQUIRE_FALSE(lfm.is_active());
}

TEST_CASE("LargeFileMode: activates on size threshold", "[large_file_mode]")
{
    LargeFileMode lfm;
    lfm.set_size_threshold(1024);

    lfm.evaluate(2048, 10);

    REQUIRE(lfm.is_active());
    REQUIRE(lfm.file_size() == 2048);
}

TEST_CASE("LargeFileMode: activates on line threshold", "[large_file_mode]")
{
    LargeFileMode lfm;
    lfm.set_line_threshold(100);

    lfm.evaluate(10, 200);

    REQUIRE(lfm.is_active());
    REQUIRE(lfm.line_count() == 200);
}

TEST_CASE("LargeFileMode: stays inactive under thresholds", "[large_file_mode]")
{
    LargeFileMode lfm;
    lfm.set_size_threshold(10000);
    lfm.set_line_threshold(10000);

    lfm.evaluate(100, 50);

    REQUIRE_FALSE(lfm.is_active());
}

TEST_CASE("LargeFileMode: disables features when active", "[large_file_mode]")
{
    LargeFileMode lfm;
    lfm.set_size_threshold(100);

    lfm.evaluate(1000, 0);

    REQUIRE(lfm.is_active());
    auto features = lfm.allowed_features();
    REQUIRE_FALSE(features.minimap); // always off in LFM
    REQUIRE_FALSE(features.link_detection);
    REQUIRE_FALSE(features.spell_check);
    REQUIRE(features.line_numbers); // always on
    REQUIRE(features.auto_indent);  // always on
}

TEST_CASE("LargeFileMode: force_active overrides", "[large_file_mode]")
{
    LargeFileMode lfm;
    lfm.force_active(true);

    lfm.evaluate(10, 5); // tiny file

    REQUIRE(lfm.is_active());
}

TEST_CASE("LargeFileMode: force_active false keeps off", "[large_file_mode]")
{
    LargeFileMode lfm;
    lfm.set_size_threshold(100);
    lfm.force_active(false);

    lfm.evaluate(10000, 0); // large file

    REQUIRE_FALSE(lfm.is_active());
}

TEST_CASE("LargeFileMode: clear_force restores evaluation", "[large_file_mode]")
{
    LargeFileMode lfm;
    lfm.set_size_threshold(100);
    lfm.force_active(false);
    lfm.evaluate(10000, 0);
    REQUIRE_FALSE(lfm.is_active());

    lfm.clear_force();
    lfm.evaluate(10000, 0);
    REQUIRE(lfm.is_active());
}

TEST_CASE("LargeFileMode: status_text", "[large_file_mode]")
{
    LargeFileMode lfm;
    REQUIRE(lfm.status_text() == "Normal mode");

    lfm.set_size_threshold(100);
    lfm.evaluate(2048, 500);
    REQUIRE(lfm.status_text().find("Large file mode") != std::string::npos);
}

TEST_CASE("LargeFileMode: default thresholds", "[large_file_mode]")
{
    LargeFileMode lfm;
    REQUIRE(lfm.size_threshold() == LargeFileMode::kDefaultSizeThreshold);
    REQUIRE(lfm.line_threshold() == LargeFileMode::kDefaultLineThreshold);
}
