#include "core/Config.h"

#include <catch2/catch_test_macros.hpp>

#include <filesystem>

using namespace markamp::core;

TEST_CASE("Config: set and get string", "[core][config]")
{
    Config config;

    config.set("test_key", std::string_view("hello"));
    REQUIRE(config.get_string("test_key") == "hello");
}

TEST_CASE("Config: set and get int", "[core][config]")
{
    Config config;

    config.set("font_size", 16);
    REQUIRE(config.get_int("font_size") == 16);
}

TEST_CASE("Config: set and get bool", "[core][config]")
{
    Config config;

    config.set("word_wrap", true);
    REQUIRE(config.get_bool("word_wrap") == true);
}

TEST_CASE("Config: set and get double", "[core][config]")
{
    Config config;

    config.set("zoom_level", 1.5);
    REQUIRE(config.get_double("zoom_level") == 1.5);
}

TEST_CASE("Config: defaults for missing keys", "[core][config]")
{
    Config config;

    REQUIRE(config.get_string("nonexistent", "fallback") == "fallback");
    REQUIRE(config.get_int("nonexistent", 99) == 99);
    REQUIRE(config.get_bool("nonexistent", true) == true);
    REQUIRE(config.get_double("nonexistent", 3.14) == 3.14);
}

TEST_CASE("Config: config_directory is non-empty", "[core][config]")
{
    auto dir = Config::config_directory();
    REQUIRE(!dir.empty());
}

TEST_CASE("Config: config_file_path ends with config.json", "[core][config]")
{
    auto path = Config::config_file_path();
    REQUIRE(path.filename() == "config.json");
}
