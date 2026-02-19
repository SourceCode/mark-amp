// NOLINTBEGIN(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
#include "canvas/CanvasThemeModel.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::canvas;

TEST_CASE("Token resolution with value", "[theme][token]")
{
    CanvasThemeModel model;
    model.set_tokens({{"grid-color", "#333333", "#888888"}});
    REQUIRE(model.resolve("grid-color") == "#333333");
}

TEST_CASE("Token fallback when value empty", "[theme][token]")
{
    CanvasThemeModel model;
    model.set_tokens({{"guide-color", "", "#00FF00"}});
    REQUIRE(model.resolve("guide-color") == "#00FF00");
}

TEST_CASE("Missing token returns empty", "[theme][token]")
{
    CanvasThemeModel model;
    REQUIRE(model.resolve("nonexistent").empty());
}

TEST_CASE("Missing tokens detection", "[theme][token]")
{
    CanvasThemeModel model;
    model.set_tokens({
        {"grid", "#333", "#888"},
        {"broken", "", ""},
    });
    REQUIRE(model.missing_tokens().size() == 1);
    REQUIRE(model.missing_tokens()[0] == "broken");
}

TEST_CASE("Board background presets", "[theme][background]")
{
    CanvasThemeModel model;
    model.set_background(BoardBackground::kDotGrid);
    REQUIRE(model.background() == BoardBackground::kDotGrid);
}

TEST_CASE("Contrast legibility check", "[theme][contrast]")
{
    CanvasThemeModel model;
    model.set_min_contrast(4.5);
    REQUIRE(model.passes_contrast(5.0));
    REQUIRE_FALSE(model.passes_contrast(3.0));
}
// NOLINTEND(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
