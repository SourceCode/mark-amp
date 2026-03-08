/// test_color.cpp — Unit tests for Color struct
#include "core/Color.h"

#include <catch2/catch_test_macros.hpp>

#include <cmath>

using namespace markamp::core;

TEST_CASE("Color: default construction is black opaque", "[color]")
{
    Color c;
    REQUIRE(c.r == 0);
    REQUIRE(c.g == 0);
    REQUIRE(c.b == 0);
    REQUIRE(c.a == 255);
}

TEST_CASE("Color: RGBA construction", "[color]")
{
    Color c(128, 64, 32, 200);
    REQUIRE(c.r == 128);
    REQUIRE(c.g == 64);
    REQUIRE(c.b == 32);
    REQUIRE(c.a == 200);
}

TEST_CASE("Color: RGB construction defaults alpha to 255", "[color]")
{
    Color c(100, 150, 200);
    REQUIRE(c.a == 255);
}

TEST_CASE("Color: from_rgb factory", "[color]")
{
    auto c = Color::from_rgb(10, 20, 30);
    REQUIRE(c.r == 10);
    REQUIRE(c.g == 20);
    REQUIRE(c.b == 30);
    REQUIRE(c.a == 255);
}

TEST_CASE("Color: from_hex parses 6-digit hex", "[color]")
{
    auto result = Color::from_hex("#FF8040");
    REQUIRE(result.has_value());
    REQUIRE(result->r == 255);
    REQUIRE(result->g == 128);
    REQUIRE(result->b == 64);
    REQUIRE(result->a == 255);
}

TEST_CASE("Color: from_hex parses 3-digit hex", "[color]")
{
    auto result = Color::from_hex("#F80");
    REQUIRE(result.has_value());
    REQUIRE(result->r == 0xFF);
    REQUIRE(result->g == 0x88);
    REQUIRE(result->b == 0x00);
}

TEST_CASE("Color: from_hex returns error for invalid input", "[color]")
{
    auto result = Color::from_hex("not-a-color");
    REQUIRE_FALSE(result.has_value());
}

TEST_CASE("Color: to_hex round-trips correctly", "[color]")
{
    Color c(255, 128, 64);
    auto hex = c.to_hex();
    auto back = Color::from_hex(hex);
    REQUIRE(back.has_value());
    REQUIRE(back->r == c.r);
    REQUIRE(back->g == c.g);
    REQUIRE(back->b == c.b);
}

TEST_CASE("Color: with_alpha returns new color with modified alpha", "[color]")
{
    Color c(100, 100, 100, 255);
    auto c2 = c.with_alpha(static_cast<uint8_t>(128));
    REQUIRE(c2.r == 100);
    REQUIRE(c2.g == 100);
    REQUIRE(c2.b == 100);
    REQUIRE(c2.a == 128);
    // Original unchanged
    REQUIRE(c.a == 255);
}

TEST_CASE("Color: equality comparison", "[color]")
{
    Color a(10, 20, 30, 40);
    Color b(10, 20, 30, 40);
    Color c(10, 20, 30, 41);
    REQUIRE(a == b);
    REQUIRE_FALSE(a == c);
}

TEST_CASE("Color: luminance of white is approximately 1.0", "[color]")
{
    Color white(255, 255, 255);
    REQUIRE(std::abs(white.luminance() - 1.0f) < 0.01f);
}

TEST_CASE("Color: luminance of black is approximately 0.0", "[color]")
{
    Color black(0, 0, 0);
    REQUIRE(std::abs(black.luminance()) < 0.01f);
}

TEST_CASE("Color: contrast_ratio black/white is ~21", "[color]")
{
    Color black(0, 0, 0);
    Color white(255, 255, 255);
    auto ratio = black.contrast_ratio(white);
    REQUIRE(ratio > 20.0f);
    REQUIRE(ratio < 22.0f);
}

TEST_CASE("Color: contrast_ratio is symmetric", "[color]")
{
    Color a(100, 50, 200);
    Color b(200, 150, 50);
    REQUIRE(std::abs(a.contrast_ratio(b) - b.contrast_ratio(a)) < 0.01f);
}

TEST_CASE("Color: blend produces midpoint color", "[color]")
{
    Color black(0, 0, 0);
    Color white(255, 255, 255);
    auto mid = black.blend(white, 0.5f);
    // Should be roughly in the middle
    REQUIRE(mid.r > 100);
    REQUIRE(mid.r < 200);
}

TEST_CASE("Color: lighten produces brighter color", "[color]")
{
    Color dark(50, 50, 50);
    auto lighter = dark.lighten(0.5f);
    REQUIRE(lighter.luminance() > dark.luminance());
}

TEST_CASE("Color: to_rgba_string produces valid format", "[color]")
{
    Color c(100, 150, 200, 128);
    auto str = c.to_rgba_string();
    REQUIRE_FALSE(str.empty());
}

TEST_CASE("Color: from_string handles hex", "[color]")
{
    auto result = Color::from_string("#FF0000");
    REQUIRE(result.has_value());
    REQUIRE(result->r == 255);
    REQUIRE(result->g == 0);
    REQUIRE(result->b == 0);
}

TEST_CASE("Color: to_theme_string for opaque uses hex", "[color]")
{
    Color c(255, 0, 0, 255);
    auto str = c.to_theme_string();
    REQUIRE(str.find('#') != std::string::npos);
}
