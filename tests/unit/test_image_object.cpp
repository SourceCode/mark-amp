#include "canvas/ImageObject.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

using namespace markamp::canvas;

// ── Construction ────────────────────────────────────────────────

TEST_CASE("ImageObject default construction", "[image]")
{
    ImageObject img;
    REQUIRE(img.type() == CanvasObjectType::Image);
    REQUIRE(img.width() == Catch::Approx(200.0));
    REQUIRE(img.height() == Catch::Approx(200.0));
    REQUIRE(img.maintain_aspect() == true);
    REQUIRE(img.file_path().empty());
    REQUIRE(img.alt_text().empty());
    REQUIRE(img.has_crop_region() == false);
}

// ── File Path ───────────────────────────────────────────────────

TEST_CASE("ImageObject load_from_file", "[image]")
{
    ImageObject img;
    const bool result = img.load_from_file("/path/to/image.png");
    REQUIRE(result == true);
    REQUIRE(img.file_path() == "/path/to/image.png");

    const bool empty_result = img.load_from_file("");
    REQUIRE(empty_result == false);
}

// ── Dimensions and bounds ───────────────────────────────────────

TEST_CASE("ImageObject local_bounds", "[image]")
{
    ImageObject img;
    const auto bounds = img.local_bounds();
    REQUIRE(bounds.min_x == Catch::Approx(0.0));
    REQUIRE(bounds.min_y == Catch::Approx(0.0));
    REQUIRE(bounds.max_x == Catch::Approx(200.0));
    REQUIRE(bounds.max_y == Catch::Approx(200.0));
}

// ── Resize with Aspect Lock ────────────────────────────────────

TEST_CASE("ImageObject resize maintains aspect ratio", "[image]")
{
    ImageObject img;
    // Default original is 200x200 (1:1 aspect ratio).
    img.resize(400.0, 200.0);

    // With 1:1 aspect, width-dominant resize should scale height to match.
    REQUIRE(img.width() == Catch::Approx(400.0));
    REQUIRE(img.height() == Catch::Approx(400.0));
}

TEST_CASE("ImageObject resize without aspect lock", "[image]")
{
    ImageObject img;
    img.set_maintain_aspect(false);
    img.resize(300.0, 150.0);

    REQUIRE(img.width() == Catch::Approx(300.0));
    REQUIRE(img.height() == Catch::Approx(150.0));
}

TEST_CASE("ImageObject minimum resize", "[image]")
{
    ImageObject img;
    img.set_maintain_aspect(false);
    img.resize(1.0, 1.0);

    REQUIRE(img.width() >= 10.0);
    REQUIRE(img.height() >= 10.0);
}

// ── Aspect Ratio ────────────────────────────────────────────────

TEST_CASE("ImageObject aspect_ratio", "[image]")
{
    ImageObject img;
    // Default 200x200 = 1:1.
    REQUIRE(img.aspect_ratio() == Catch::Approx(1.0));
}

// ── Crop Region ─────────────────────────────────────────────────

TEST_CASE("ImageObject crop region", "[image]")
{
    ImageObject img;
    REQUIRE(img.has_crop_region() == false);

    img.set_crop_region({10.0, 20.0, 100.0, 80.0});
    REQUIRE(img.has_crop_region() == true);

    const auto crop = img.crop_region();
    REQUIRE(crop.has_value());
    REQUIRE(crop->min_x == Catch::Approx(10.0));
    REQUIRE(crop->max_y == Catch::Approx(80.0));

    img.clear_crop_region();
    REQUIRE(img.has_crop_region() == false);
}

// ── Alt Text ────────────────────────────────────────────────────

TEST_CASE("ImageObject alt text", "[image]")
{
    ImageObject img;
    img.set_alt_text("A nice photo");
    REQUIRE(img.alt_text() == "A nice photo");
}

// ── Clone ───────────────────────────────────────────────────────

TEST_CASE("ImageObject clone", "[image]")
{
    ImageObject img;
    img.load_from_file("/test.png");
    img.set_alt_text("test");
    img.set_name("photo");
    img.set_crop_region({5.0, 5.0, 50.0, 50.0});

    auto cloned = img.clone();
    const auto& copy = static_cast<const ImageObject&>(*cloned);

    REQUIRE(copy.file_path() == "/test.png");
    REQUIRE(copy.alt_text() == "test");
    REQUIRE(copy.name() == "photo");
    REQUIRE(copy.has_crop_region() == true);
    REQUIRE(copy.id() != img.id());
}

// ── JSON serialization ──────────────────────────────────────────

TEST_CASE("ImageObject to_json", "[image]")
{
    ImageObject img;
    img.load_from_file("/img.png");
    img.set_alt_text("photo");

    const auto json = img.to_json();
    REQUIRE(json.find("Image") != std::string::npos);
    REQUIRE(json.find("/img.png") != std::string::npos);
    REQUIRE(json.find("alt_text") != std::string::npos);
}

TEST_CASE("ImageObject to_json with crop", "[image]")
{
    ImageObject img;
    img.set_crop_region({10.0, 20.0, 100.0, 80.0});

    const auto json = img.to_json();
    REQUIRE(json.find("crop") != std::string::npos);
}
