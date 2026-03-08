// test_slide_theme_provider.cpp — 10 tests for SlideThemeProvider
#include "core/SlideThemeProvider.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("SlideThemeProvider starts empty", "[presentation][theme]")
{
    SlideThemeProvider provider;
    CHECK(provider.theme_count() == 0);
}

TEST_CASE("SlideThemeProvider register and find theme", "[presentation][theme]")
{
    SlideThemeProvider provider;
    SlideTheme theme;
    theme.theme_id = "dark-modern";
    theme.name = "Dark Modern";
    theme.is_dark = true;
    provider.register_theme(theme);
    CHECK(provider.theme_count() == 1);
    auto found = provider.find_theme("dark-modern");
    REQUIRE(found != nullptr);
    CHECK(found->name == "Dark Modern");
}

TEST_CASE("SlideThemeProvider remove_theme", "[presentation][theme]")
{
    SlideThemeProvider provider;
    SlideTheme theme;
    theme.theme_id = "temp";
    provider.register_theme(theme);
    CHECK(provider.remove_theme("temp"));
    CHECK(provider.theme_count() == 0);
}

TEST_CASE("SlideThemeProvider find missing returns null", "[presentation][theme]")
{
    SlideThemeProvider provider;
    CHECK(provider.find_theme("ghost") == nullptr);
}

TEST_CASE("SlideThemeProvider load_defaults adds themes", "[presentation][theme]")
{
    SlideThemeProvider provider;
    provider.load_defaults();
    CHECK(provider.theme_count() > 0);
}

TEST_CASE("SlideThemeProvider dark_themes filters correctly", "[presentation][theme]")
{
    SlideThemeProvider provider;
    SlideTheme dark;
    dark.theme_id = "dark";
    dark.is_dark = true;
    SlideTheme light;
    light.theme_id = "light";
    light.is_dark = false;
    provider.register_theme(dark);
    provider.register_theme(light);
    auto darks = provider.dark_themes();
    CHECK(darks.size() == 1);
}

TEST_CASE("SlideThemeProvider light_themes filters correctly", "[presentation][theme]")
{
    SlideThemeProvider provider;
    SlideTheme dark;
    dark.theme_id = "dark";
    dark.is_dark = true;
    SlideTheme light;
    light.theme_id = "light";
    light.is_dark = false;
    provider.register_theme(dark);
    provider.register_theme(light);
    auto lights = provider.light_themes();
    CHECK(lights.size() == 1);
}

TEST_CASE("SlideThemeProvider all_themes returns all", "[presentation][theme]")
{
    SlideThemeProvider provider;
    SlideTheme t1;
    t1.theme_id = "a";
    SlideTheme t2;
    t2.theme_id = "b";
    provider.register_theme(t1);
    provider.register_theme(t2);
    CHECK(provider.all_themes().size() == 2);
}

TEST_CASE("SlideThemeProvider clear_all", "[presentation][theme]")
{
    SlideThemeProvider provider;
    SlideTheme theme;
    theme.theme_id = "x";
    provider.register_theme(theme);
    provider.clear_all();
    CHECK(provider.theme_count() == 0);
}

TEST_CASE("SlideTheme defaults", "[presentation][theme]")
{
    SlideTheme theme;
    CHECK(theme.theme_id.empty());
    CHECK(theme.name.empty());
    CHECK_FALSE(theme.is_dark);
    CHECK_FALSE(theme.is_builtin);
    CHECK(theme.primary_color.empty());
}
