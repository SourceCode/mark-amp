// NOLINTBEGIN(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
#include "canvas/ImageAssetModel.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::canvas;

TEST_CASE("Source and replacement", "[image][source]")
{
    ImageAssetModel model;
    model.set_source("photo.png");
    REQUIRE(model.source() == "photo.png");
    model.replace_source("new_photo.png");
    REQUIRE(model.source() == "new_photo.png");
    REQUIRE(model.replacement_count() == 1);
}

TEST_CASE("Fit modes", "[image][fit]")
{
    ImageAssetModel model;
    REQUIRE(model.fit_mode() == ImageFitMode::kFit);
    model.set_fit_mode(ImageFitMode::kCrop);
    REQUIRE(model.fit_mode() == ImageFitMode::kCrop);
}

TEST_CASE("Crop detection", "[image][crop]")
{
    ImageAssetModel model;
    REQUIRE_FALSE(model.is_cropped());
    model.set_crop({0.1, 0.0, 0.9, 1.0});
    REQUIRE(model.is_cropped());
}

TEST_CASE("Crop reset", "[image][crop]")
{
    ImageAssetModel model;
    model.set_crop({0.1, 0.1, 0.9, 0.9});
    model.reset_crop();
    REQUIRE_FALSE(model.is_cropped());
}

TEST_CASE("Render quality clamped", "[image][quality]")
{
    ImageAssetModel model;
    model.set_render_quality(1.5);
    REQUIRE(model.render_quality() == 1.0);
    model.set_render_quality(-0.5);
    REQUIRE(model.render_quality() == 0.0);
}
// NOLINTEND(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
